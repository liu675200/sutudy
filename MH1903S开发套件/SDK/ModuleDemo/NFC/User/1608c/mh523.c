/**
 ****************************************************************
 * @file rc523.c
 *
 * @brief  rc523 driver.
 *
 * @author 
 *
 * 
 ****************************************************************
 */ 

/*
 * INCLUDE FILES
 ****************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mh523.h"
#include "uart.h"
#include "gpio.h"
#include "timer.h"
#include "iso14443_4.h"

#define WATER_LEVEL	     16
#define FIFO_SIZE	     64
#define FSD              256 //Frame Size for proximity coupling Device


#define	READ_REG_CTRL	 0x80
#define	TP_FWT_302us	 2048
#define TP_dFWT	         192 

#define MAX_RX_REQ_WAIT_MS	     5000 // ����ȴ���ʱʱ��100ms

static u32 XDATA g_polling_cnt = 0;
transceive_buffer XDATA mf_com_data;

/**
 ****************************************************************
 * @brief pcd_init() 
 *
 * ��ʼ��оƬ
 *
 * @param:    
 * @return: 
 * @retval: 
 ****************************************************************
 */
void pcd_init(void)
{
	pcd_poweron();
	mdelay(5);
	
	pcd_reset();
	mdelay(5);
	pcd_config('A');	
}


/**
 ****************************************************************
 * @brief pcd_config() 
 *
 * ����оƬ��A/Bģʽ
 *
 * @param: u8 type   
 * @return: 
 * @retval: 
 ****************************************************************
 */
int pcd_config(u8 type)
{
	pcd_antenna_off();
	mdelay(7);

	if ('A' == type)
	{
	#if(NFC_DEBUG)
		printf("config type A\n");
	#endif		
	    clear_bit_mask(Status2Reg, BIT3);
		clear_bit_mask(ComIEnReg, BIT7); // �ߵ�ƽ
        write_reg(ModeReg,0x3D);	// 11 // CRC seed:6363
        write_reg(RxSelReg, 0x86);//RxWait
		write_reg(RFCfgReg, 0x58); // 
		write_reg(TxASKReg, 0x40);//15  //typeA
		write_reg(TxModeReg, 0x00);//12 //Tx Framing A
		write_reg(RxModeReg, 0x00);//13 //Rx framing A
		write_reg(0x0C, 0x10);	//^_^

		//��������
		{
			u8 backup, adc;
			backup = read_reg(0x37);
			write_reg(0x37, 0x00);	
			adc = read_reg(0x37);
			
			if (adc >= 0x12)
			{
				// ���¼Ĵ������밴˳������
				write_reg(0x37, 0x5E);
				write_reg(0x26, 0x48);
				write_reg(0x17, 0x88);
				write_reg(0x29, 0x12);//0x0F); //����ָ��	
				write_reg(0x35, 0xED);
				write_reg(0x3b, 0xA5);
				write_reg(0x37, 0xAE);
				write_reg(0x3b, 0x72);
				
			}
			write_reg(0x37, backup);
		}
			
	}
	else if ('B' == type)
	{
	#if(NFC_DEBUG)
		printf("config type B\n");
	#endif		
		write_reg(Status2Reg, 0x00);	//��MFCrypto1On
		clear_bit_mask(ComIEnReg, BIT7);// �ߵ�ƽ�����ж�
		write_reg(ModeReg, 0x3F);	// CRC seed:FFFF
		write_reg(RxSelReg, 0x88);	//RxWait
		//Tx
		write_reg(GsNReg, 0xF8);	//����ϵ��
		write_reg(CWGsPReg, 0x3F);	// 
		write_reg(ModGsPReg, 0x12);	//����ָ��
		write_reg(AutoTestReg, 0x00);
		write_reg(TxASKReg, 0x00);	// typeB
		write_reg(TypeBReg, 0x13);
		write_reg(TxModeReg, 0x83);	//Tx Framing B
		write_reg(RxModeReg, 0x83);	//Rx framing B
		write_reg(BitFramingReg, 0x00);	//TxLastBits=0

		//��������
		{
			u8 backup, adc;
			backup = read_reg(0x37);
			write_reg(0x37, 0x00);
			adc = read_reg(0x37);

			if (adc >= 0x12)
			{	
				write_reg(0x37, 0x5E);
				write_reg(0x26, 0x48);
				write_reg(0x17, 0x88);
				write_reg(0x29, 0x12);
				write_reg(0x35, 0xED);
				write_reg(0x3b, 0xA5);
				write_reg(0x37, 0xAE);
				write_reg(0x3b, 0x72);
			}
			write_reg(0x37, backup);
		}
	}
	else
	{
		return USER_ERROR;
	}
	
	pcd_antenna_on();
	mdelay(2);
	
	return MI_OK;
}

/**
 ****************************************************************
 * @brief pcd_com_transceive() 
 *
 * ͨ��оƬ��ISO14443��ͨѶ
 *
 * @param: pi->mf_command = оƬ������
 * @param: pi->mf_length  = ���͵����ݳ���
 * @param: pi->mf_data[]  = ��������
 * @return: status ֵΪMI_OK:�ɹ�
 * @retval: pi->mf_length  = ���յ�����BIT����
 * @retval: pi->mf_data[]  = ��������
 ****************************************************************
 */
int pcd_com_transceive(struct transceive_buffer *pi)
{
	u8 XDATA recebyte;
	int XDATA status;
	u8 XDATA irq_en;
	u8 XDATA wait_for;
	u8 XDATA last_bits;
	u8 XDATA j;
	u8 XDATA val;
	u8 XDATA err;
	tick XDATA start_tick;
	u8 irq_inv;
	u16  len_rest;
	u8  len;
	
	len = 0;
	len_rest = 0;
	err = 0;
	recebyte = 0;
	irq_en = 0;
	wait_for = 0;

	switch (pi->mf_command)
	{
	  case PCD_IDLE:
	     irq_en   = 0x00;
	     wait_for = 0x00;
	     break;
	  case PCD_AUTHENT:    
		irq_en = IdleIEn | TimerIEn;
		wait_for = IdleIRq;
		break;
	  case PCD_RECEIVE:
	     irq_en   = RxIEn | IdleIEn;
	     wait_for = RxIRq;
	     recebyte=1;
	     break;
	  case PCD_TRANSMIT:
	     irq_en   = TxIEn | IdleIEn;
	     wait_for = TxIRq;
	     break;
	  case PCD_TRANSCEIVE:   
		 irq_en = RxIEn | IdleIEn | TimerIEn | TxIEn;
	     wait_for = RxIRq;
	     recebyte=1;
	     break;
	  default:
	     pi->mf_command = MI_UNKNOWN_COMMAND;
	     break;
	}
   
	if (pi->mf_command != MI_UNKNOWN_COMMAND
		&& (((pi->mf_command == PCD_TRANSCEIVE || pi->mf_command == PCD_TRANSMIT) && pi->mf_length > 0)
		|| (pi->mf_command != PCD_TRANSCEIVE && pi->mf_command != PCD_TRANSMIT))
		)
	{		
		write_reg(CommandReg, PCD_IDLE);
		
		irq_inv = read_reg(ComIEnReg) & BIT7;
		write_reg(ComIEnReg, irq_inv |irq_en | BIT0);//ʹ��Timer ��ʱ���ж�
		write_reg(ComIrqReg, 0x7F); //Clear INT
		write_reg(DivIrqReg, 0x7F); //Clear INT
		//Flush Fifo
		set_bit_mask(FIFOLevelReg, BIT7);
		if (pi->mf_command == PCD_TRANSCEIVE || pi->mf_command == PCD_TRANSMIT || pi->mf_command == PCD_AUTHENT)
		{
			#if (NFC_DEBUG)
			printf(" PCD_tx:");
			#endif
			for (j = 0; j < pi->mf_length; j++)
			{
				
				#if (NFC_DEBUG)
				printf("%02x ", (u16)pi->mf_data[j]);
				#endif
			}
			#if (NFC_DEBUG)
			printf("\n");
			#endif
		
			len_rest = pi->mf_length;
			if (len_rest >= FIFO_SIZE)
			{
				len = FIFO_SIZE;
			}else
			{
				len = len_rest;
			}
			
			for (j = 0; j < len; j++)
			{
				write_reg(FIFODataReg, pi->mf_data[j]);
			}
			len_rest -= len;//Rest bytes
			if (len_rest != 0)
			{
				write_reg(ComIrqReg, BIT2); // clear LoAlertIRq
				set_bit_mask(ComIEnReg, BIT2);// enable LoAlertIRq
			}

			write_reg(CommandReg, pi->mf_command);
			if (pi->mf_command == PCD_TRANSCEIVE)
		    {    
				set_bit_mask(BitFramingReg,0x80);  
			}
		
			while (len_rest != 0)
			{
				while(INT_PIN == 0);//Wait LoAlertIRq		
				if (len_rest > (FIFO_SIZE - WATER_LEVEL))
				{
					len = FIFO_SIZE - WATER_LEVEL;
				}
				else
				{
					len = len_rest;
				}
				for (j = 0; j < len; j++)
				{
					write_reg(FIFODataReg, pi->mf_data[pi->mf_length - len_rest + j]);
				}

				write_reg(ComIrqReg, BIT2);//��write fifo֮��������жϱ�ǲſ���
			
				//printf("\n8 comirq=%02x,ien=%02x,INT= %bd \n", read_reg(ComIrqReg), read_reg(ComIEnReg), (u8)INT_PIN);
				len_rest -= len;//Rest bytes
				if (len_rest == 0)
				{
					clear_bit_mask(ComIEnReg, BIT2);// disable LoAlertIRq
					//printf("\n9 comirq=%02x,ien=%02x,INT= %bd \n", read_reg(ComIrqReg), read_reg(ComIEnReg), (u8)INT_PIN);
				}
			}
	
		#if (NFC_DEBUG)
			start_tick = get_tick();
		#endif
			//Wait TxIRq
			while (INT_PIN == 0)
			{
			#if (NFC_DEBUG)
			//���оƬ�Ƿ�û��TxIRq�ж�
			if (is_timeout(start_tick, 500))
			{
				printf("CHIP_has_no TxIRq interrupt > %d ms, current reg:\n", (u16)500);
				mdelay(500);
				for(j = 0; j <= 0x3F; j++)
				{
					printf("%02x=%02x\n", j, read_reg(j));
				}
				g_polling_cnt=0;
				
				return -1;
			}				
			#endif
			}
			val = read_reg(ComIrqReg);
			if (val & TxIRq)
			{
				write_reg(ComIrqReg, TxIRq);
			}
		}
		if (PCD_RECEIVE == pi->mf_command)
		{	
			set_bit_mask(ControlReg, BIT6);// TStartNow
		}
	
		len_rest = 0; // bytes received
		write_reg(ComIrqReg, BIT3); // clear HoAlertIRq
		set_bit_mask(ComIEnReg, BIT3); // enable HoAlertIRq

	#if (NFC_DEBUG)
		start_tick = get_tick();
	#endif		
		//�ȴ�����ִ�����
		while(INT_PIN == 0)
		{
		#if (NFC_DEBUG)
			//���оƬ�Ƿ�û��timeout�ж�
			if (is_timeout(start_tick, MAX_RX_REQ_WAIT_MS))
			{
				printf("CHIP_has_no_timeout_interrupt > %d ms, current reg:\n", (u16)MAX_RX_REQ_WAIT_MS);
				mdelay(500);
				for(j = 0; j <= 0x3F; j++)
				{
					printf("%02x=%02x\n", j, read_reg(j));
				}
				g_polling_cnt=0;
				
				return -1;
			}
		#endif
		}
		
		while(1)
		{
		#if (NFC_DEBUG)
			start_tick = get_tick();
		#endif			
			while(0 == INT_PIN)
			{
			#if (NFC_DEBUG)
				//���оƬ�Ƿ�û��RxIrq�ж�
				if (is_timeout(start_tick, 500))
				{
					printf("CHIP_has_no RxIrq interrupt > %d ms, current reg:\n", (u16)500);
					mdelay(500);
					for(j = 0; j <= 0x3F; j++)
					{
						printf("%02x=%02x\n", j, read_reg(j));
					}
					g_polling_cnt=0;
					
					return -1;
				}
			#endif
			}
			val = read_reg(ComIrqReg);
			if ((val & BIT3) && !(val & BIT5))
			{
				if (len_rest + FIFO_SIZE - WATER_LEVEL > 255)
				{
					#if (NFC_DEBUG)
					printf("AF RX_LEN > 255B\n");
					#endif
					break;
				}
		        for (j = 0; j <FIFO_SIZE - WATER_LEVEL; j++)
		        {
					pi->mf_data[len_rest + j] = read_reg(FIFODataReg);
		        }
				write_reg(ComIrqReg, BIT3);//��read fifo֮��������жϱ�ǲſ���
				len_rest += FIFO_SIZE - WATER_LEVEL; 
			}
			else
			{
				clear_bit_mask(ComIEnReg, BIT3);//disable HoAlertIRq
				break;
			}			
		}


		val = read_reg(ComIrqReg);
		#if (NFC_DEBUG)
		printf(" INT:fflvl=%d,rxlst=%02x ,ien=%02x,cirq=%02x\n", (u16)read_reg(FIFOLevelReg),read_reg(ControlReg)&0x07,read_reg(ComIEnReg), val);//XU
		#endif
		write_reg(ComIrqReg, val);// ���ж�
		
		if (val & BIT0)
		{//������ʱ
			status = MI_NOTAGERR;
			printf(" Time_out!!\n");
		}
		else
		{
			err = read_reg(ErrorReg);
			
			status = MI_COM_ERR;
			if ((val & wait_for) && (val & irq_en))
			{
				if (!(val & ErrIRq))
				 {//ָ��ִ����ȷ
				    status = MI_OK;

				    if (recebyte)
				    {
						val = 0x7F & read_reg(FIFOLevelReg);
				      	last_bits = read_reg(ControlReg) & 0x07;
						if (len_rest + val > MAX_TRX_BUF_SIZE)
						{//���ȹ�����������
							status = MI_COM_ERR;
							#if (NFC_DEBUG)
							printf("RX_LEN > 255B\n");
							#endif
						}
						else
						{	
							if (last_bits && val) //��ֹspi����� val-1��Ϊ��ֵ
					        {
					           pi->mf_length = (val-1)*8 + last_bits;
					        }
					        else
					        {
					           pi->mf_length = val*8;
					        }
							pi->mf_length += len_rest*8;

							#if (NFC_DEBUG)
							printf(" RX:len=%02x,dat:", (u16)pi->mf_length);
							#endif
					        if (val == 0)
					        {
					           val = 1;
					        }
					        for (j = 0; j < val; j++)
					        {
								pi->mf_data[len_rest + j] = read_reg(FIFODataReg);
					        }					

							#if (NFC_DEBUG)
						    for (j = 0; j < pi->mf_length/8 + !!(pi->mf_length%8); j++)
					        {
							//	if (j > 4)
							//	{
							//		printf("..");
							//		break;
							//	}
							//	else
							//	{
									printf("%02X ", (u16)pi->mf_data[j]);
							//	}
								//printf("%02X ", (u16)pi->mf_data[j]);
					        }
							//printf("l=%d", pi->mf_length/8 + !!(pi->mf_length%8));
							printf("\n");
							#endif
						}
				    }
				 }					
				 else if ((err & CollErr) && (!(read_reg(CollReg) & BIT5)))
				 {//a bit-collision is detected				 	
				    status = MI_COLLERR;
				    if (recebyte)
				    {
						val = 0x7F & read_reg(FIFOLevelReg);
				      	last_bits = read_reg(ControlReg) & 0x07;
						if (len_rest + val > MAX_TRX_BUF_SIZE)
						{//���ȹ�����������
							#if (NFC_DEBUG)
							printf("COLL RX_LEN > 255B\n");
							#endif
						}
						else
						{
					        if (last_bits && val) //��ֹspi����� val-1��Ϊ��ֵ
					        {
					           pi->mf_length = (val-1)*8 + last_bits;
					        }
					        else
					        {
					           pi->mf_length = val*8;
					        }		
							pi->mf_length += len_rest*8;
							#if (NFC_DEBUG)
							printf(" RX: pi_cmd=%02x,pi_len=%02x,pi_dat:", (u16)pi->mf_command, (u16)pi->mf_length);
							#endif
					        if (val == 0)
					        {
					           val = 1;
					        }
							for (j = 0; j < val; j++)
					        {
								pi->mf_data[len_rest + j +1] = read_reg(FIFODataReg);				
					        }				
							#if (NFC_DEBUG)
					        for (j = 0; j < pi->mf_length/8 + !!(pi->mf_length%8); j++)
					        {
								printf("%02X ", (u16)pi->mf_data[j+1]);
					        }
							printf("\n");
							#endif
						}
				    }
					pi->mf_data[0] = (read_reg(CollReg) & CollPos);
					if (pi->mf_data[0] == 0)
					{
						pi->mf_data[0] = 32;
					}
				#if(NFC_DEBUG)
					printf("\n COLL_DET pos=%02x\n", (u16)pi->mf_data[0]);
				#endif
					pi->mf_data[0]--;// ��֮ǰ�汾�е�ӳ������Ϊ�˲��ı��ϲ���룬����ֱ�Ӽ�һ��

				}
				else if ((err & CollErr) && (read_reg(CollReg) & BIT5))
				{
					//printf("COLL_DET,but CollPosNotValid=1\n");		
				}
				//else if (err & (CrcErr | ParityErr | ProtocolErr))
				else if (err & (ProtocolErr))
				{
					#if (NFC_DEBUG)
					printf("protocol err=%02x\n", err);
					#endif
					status = MI_FRAMINGERR;				
				}
				else if ((err & (CrcErr | ParityErr)) && !(err &ProtocolErr) )
				{
					//EMV  parity err EMV 307.2.3.4		
					val = 0x7F & read_reg(FIFOLevelReg);
			      	last_bits = read_reg(ControlReg) & 0x07;
					if (len_rest + val > MAX_TRX_BUF_SIZE)
					{//���ȹ�����������
						status = MI_COM_ERR;
						#if (NFC_DEBUG)
						printf("RX_LEN > 255B\n");
						#endif
					}
					else
					{
				        if (last_bits && val)
				        {
				           pi->mf_length = (val-1)*8 + last_bits;
				        }
				        else
				        {
				           pi->mf_length = val*8;
				        }
						pi->mf_length += len_rest*8;
					}
					#if (NFC_DEBUG)
					printf("crc-parity err=%02x\n", err);
					printf("l=%d\n", pi->mf_length );
					#endif


					
					status = MI_INTEGRITY_ERR;
				}				
				else
				{
					#if (NFC_DEBUG)
					printf("unknown ErrorReg=%02x\n", err);
					#endif
					status = MI_INTEGRITY_ERR;
				}
			}
			else
			{   
				status = MI_COM_ERR;
				#if (NFC_DEBUG)
				printf(" MI_COM_ERR\n");
				#endif
			}
		}
	
 		set_bit_mask(ControlReg, BIT7);// TStopNow =1,��Ҫ�ģ�
		write_reg(ComIrqReg, 0x7F);// ���ж�0
		write_reg(DivIrqReg, 0x7F);// ���ж�1
		clear_bit_mask(ComIEnReg, 0x7F);//���ж�ʹ��,���λ�ǿ���λ
		clear_bit_mask(DivIEnReg, 0x7F);//���ж�ʹ��,���λ�ǿ���λ
		write_reg(CommandReg, PCD_IDLE);

	}
	else
	{
		status = USER_ERROR;
		#if (NFC_DEBUG)
		printf("USER_ERROR\n");
		#endif
	}
	#if (NFC_DEBUG)
		printf(" pcd_com: sta=%d,err=0x%02x\n", status, err);
	#endif	
	return status;
}

void pcd_reset(void)
{	
#if(NFC_DEBUG)
	printf("pcd_reset\n");
#endif
	write_reg(CommandReg, PCD_RESETPHASE); //��λ����оƬ
}

void pcd_antenna_on(void)
{
	write_reg(TxControlReg, read_reg(TxControlReg) | 0x03); //Tx1RFEn=1 Tx2RFEn=1
}

void pcd_antenna_off(void)
{
	write_reg(TxControlReg, read_reg(TxControlReg) & (~0x03));
}
/////////////////////////////////////////////////////////////////////
//����PCD��ʱ��
//input:fwi=0~15
/////////////////////////////////////////////////////////////////////
void pcd_set_tmo(u8 fwi)
{
	write_reg(TPrescalerReg, (TP_FWT_302us) & 0xFF);
	write_reg(TModeReg, BIT7 | (((TP_FWT_302us)>>8) & 0xFF));

	write_reg(TReloadRegL, (1 << fwi)  & 0xFF);
	write_reg(TReloadRegH, ((1 << fwi)  & 0xFF00) >> 8);
}



void pcd_delay_sfgi(u8 sfgi)
{
	//SFGT = (SFGT+dSFGT) = [(256 x 16/fc) x 2^SFGI] + [384/fc x 2^SFGI] 
	//dSFGT =  384 x 2^FWI / fc
		write_reg(TPrescalerReg, (TP_FWT_302us + TP_dFWT) & 0xFF);
		write_reg(TModeReg, BIT7 | (((TP_FWT_302us + TP_dFWT)>>8) & 0xFF)); 

		if (sfgi > 14 || sfgi < 1)
		{//FDTA,PCD,MIN = 6078 * 1 / fc
			sfgi = 1;
		}

		write_reg(TReloadRegL, (1 << sfgi) & 0xFF);
		write_reg(TReloadRegH, ((1 << sfgi) >> 8) & 0xFF);

		write_reg(ComIrqReg, 0x7F);//����ж�
		write_reg(ComIEnReg, BIT0);
		clear_bit_mask(TModeReg,BIT7);// clear TAuto
		set_bit_mask(ControlReg,BIT6);// set TStartNow
		
		while(!INT_PIN);// wait new INT
		//set_bit_mask(TModeReg,BIT7);// recover TAuto
		pcd_set_tmo(g_pcd_module_info.uiFwi); //recover timeout set
		
}


void pcd_lpcd_config_start(u8 delta, u32 t_inactivity_ms, u8 skip_times, u8 t_detect_us)
{
	u8 XDATA WUPeriod;
	u8 XDATA SwingsCnt;
#if (NFC_DEBUG)
	printf("pcd_lpcd_config_start\n");
#endif
	WUPeriod = t_inactivity_ms * 32.768 / 256  + 0.5;
	SwingsCnt = t_detect_us * 27.12 / 2 / 16 + 0.5;

	write_reg(0x01,0x0F); //�ȸ�λ�Ĵ����ٽ���lpcd

	write_reg(0x14, 0x8B);	// Tx2CW = 1 ��continue�ز������
	write_reg(0x37, 0x00);//�ָ��汾��
	write_reg(0x37, 0x5e);	// ��˽�мĴ�����������
	write_reg(0x3c, 0x30 | delta);	//����Delta[3:0]��ֵ, ����32k
	write_reg(0x3d, WUPeriod);	//��������ʱ��	
	write_reg(0x3e, 0x80 | ((skip_times & 0x07) << 4) | (SwingsCnt & 0x0F));	//����LPCD_en����,����̽�������̽��ʱ��
	write_reg(0x37, 0x00);	// �ر�˽�мĴ�����������
	write_reg(0x03, 0x20);	//�򿪿�̽���ж�ʹ��
	write_reg(0x01, 0x10);	//PCD soft powerdown

	//����Ӧ����أ���ʾ����������Ϊ�ߵ�ƽΪ���ж�
	clear_bit_mask(0x02, BIT7);
}

/*
	lpcd���ܿ�ʼ����
*/
void pcd_lpcd_start(void)
{
#if (NFC_DEBUG)
	printf("pcd_lpcd_start\n");
#endif

	write_reg(0x01,0x0F); //�ȸ�λ�Ĵ����ٽ���lpcd
	
	write_reg(0x37, 0x00);//�ָ��汾��
	if (read_reg(0x37) == 0x10)
	{
		write_reg(0x01, 0x00);	// idle
	}
	write_reg(0x14, 0x8B);	// Tx2CW = 1 ��continue�ز������
	
	write_reg(0x37, 0x5e);	// ��˽�мĴ�����������

	//write_reg(0x3c, 0x30);	//����Delta[3:0]��ֵ, ����32k //0 ����ʹ��
	//write_reg(0x3c, 0x31);	//����Delta[3:0]��ֵ, ����32k
	//write_reg(0x3c, 0x32);	//����Delta[3:0]��ֵ, ����32k
	//write_reg(0x3c, 0x33);	//����Delta[3:0]��ֵ, ����32k
	//write_reg(0x3c, 0x34);	//����Delta[3:0]��ֵ, ����32k
	//write_reg(0x3c, 0x35);	//����Delta[3:0]��ֵ, ����32k XU
	write_reg(0x3c, 0x37);	//����Delta[3:0]��ֵ, ����32k XU
	//write_reg(0x3c, 0x3A);	//����Delta[3:0]��ֵ, ����32k XU
	//write_reg(0x3c, 0x3F);	//����Delta[3:0]��ֵ, ����32k XU
	
	write_reg(0x3d, 0x0d);	//��������ʱ��	
	write_reg(0x3e, 0x95);	//��������̽�����������LPCD_en
	write_reg(0x37, 0x00);	// �ر�˽�мĴ�����������
	write_reg(0x03, 0x20);	//�򿪿�̽���ж�ʹ��
	write_reg(0x01, 0x10);	//PCD soft powerdown		

	//����Ӧ����أ�����Ϊ�ߵ�ƽΪ���ж�
	clear_bit_mask(0x02, BIT7); 
}

void pcd_lpcd_end(void)
{
#if (NFC_DEBUG)
	printf("pcd_lpcd_end\n");
#endif
	write_reg(0x01,0x0F); //�ȸ�λ�Ĵ����ٽ���lpcd
}

u8 pcd_lpcd_check(void)
{
	if (INT_PIN && (read_reg(DivIrqReg) & BIT5)) //TagDetIrq
	{
		write_reg(DivIrqReg, BIT5); //�������⵽�ж�
		pcd_lpcd_end();
		return TRUE;
	}
	return FALSE;
}



void pcd_set_rate(u8 rate)
{
	u8 val,rxwait;
	switch(rate)
	{
		case '1':
			clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
			clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
			write_reg(ModWidthReg, 0x26);//Miller Pulse Length

			write_reg(RxSelReg, 0x88);
			
			break;

		case '2':
			clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
			set_bit_mask(TxModeReg, BIT4);
			clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
			set_bit_mask(RxModeReg, BIT4);
			write_reg(ModWidthReg, 0x12);//Miller Pulse Length
			//rxwait�����106����������������Ӧ����
			val = read_reg(RxSelReg);
			rxwait = ((val & 0x3F)*2);
			if (rxwait > 0x3F)
			{
				rxwait = 0x3F;
			}			
			write_reg(RxSelReg,(rxwait | (val & 0xC0)));
			
			break;

		case '4':			
			clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
			set_bit_mask(TxModeReg, BIT5);
			clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
			set_bit_mask(RxModeReg, BIT5);
			write_reg(ModWidthReg, 0x0A);//Miller Pulse Length
			//rxwait�����106����������������Ӧ����
			val = read_reg(RxSelReg);
			rxwait = ((val & 0x3F)*4);
			if (rxwait > 0x3F)
			{
				rxwait = 0x3F;
			}			
			write_reg(RxSelReg,(rxwait | (val & 0xC0)));	

			break;
			
			
		default:
			clear_bit_mask(TxModeReg, BIT4 | BIT5 | BIT6);
			clear_bit_mask(RxModeReg, BIT4 | BIT5 | BIT6);
			write_reg(ModWidthReg, 0x26);//Miller Pulse Length
			
			break;
	}

	{//��ͬ����ѡ��ͬ����
		u8 adc;
		write_reg(0x37, 0x00);
		adc = read_reg(0x37);

		if (adc == 0x12)
		{
			write_reg(0x37, 0x5E);
			if (rate == '8' || rate == '4')//848k,424k
			{
				write_reg(0x3B, 0x25);
			}
			else if (rate == '2' || rate == '1')// 212k, 106k
			{
				write_reg(0x3B, 0xE5);
			}
			write_reg(0x37, 0x00);		
		}
	}
}

#if 0
void calculate_crc(u8 *pin, u8 len, u8 *pout)
{
	u8 XDATA i, n;

	clear_bit_mask(DivIrqReg, 0x04);
	write_reg(CommandReg, PCD_IDLE);
	set_bit_mask(FIFOLevelReg, 0x80);

	for (i = 0; i < len; i++)
	{
		write_reg(FIFODataReg, *(pin + i));
	}
	write_reg(CommandReg, PCD_CALCCRC);
	i = 0xFF;
	do
	{
		n = read_reg(DivIrqReg);
		i--;
	}while((i!=0) && !(n&0x04));

	#if (NFC_DEBUG)
	printf("crc:i=%02x,n=%02x\n", i, n);
	#endif
	pout[0] = read_reg(CRCResultRegL);
	pout[1] = read_reg(CRCResultRegM);
	clear_bit_mask(DivIrqReg, 0x04);
}
#endif

/**
 ****************************************************************
 * @brief set_bit_mask() 
 *
 * ���Ĵ�����ĳЩbitλֵ1
 *
 * @param: reg �Ĵ�����ַ
 * @param: mask ��Ҫ��λ��bitλ
 ****************************************************************
 */
void set_bit_mask(u8 reg, u8 mask)  
{
	char XDATA tmp;

	tmp = read_reg(reg);
	write_reg(reg, tmp | mask);  // set bit mask
}


/**
 ****************************************************************
 * @brief clear_bit_mask() 
 *
 * ���Ĵ�����ĳЩbitλ��0
 *
 * @param: reg �Ĵ�����ַ
 * @param: mask ��Ҫ��0��bitλ
 ****************************************************************
 */
void clear_bit_mask(u8 reg,u8 mask)  
{
	char XDATA tmp;

	tmp = read_reg(reg);
	write_reg(reg, tmp & ~mask);  // clear bit mask
}


u32 htonl(u32 lval)
{
	return (((lval >> 24)&0x000000ff) + ((lval >> 8) & 0x0000ff00) + ((lval << 8) & 0x00ff0000) + ((lval << 24) & 0xff000000));	
}

u32 ntohl(u32 lval)
{
	return (((lval >> 24)&0x000000ff) + ((lval >> 8) & 0x0000ff00) + ((lval << 8) & 0x00ff0000) + ((lval << 24) & 0xff000000));	
}
