/**
 ****************************************************************
 * @file iso14443a.c
 *
 * @brief  iso1443a protocol driver
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
#include "define.h"
#include "uart.h"
#include "timer.h"
#include "mh523.h"
#include "iso14443a.h"
#include "iso14443_4.h"

// 14443-4
#define PICC_RATS			  0xE0				 //Request for Answer To Select
#define PICC_NAK			  0xB2 // no CID

unsigned short CODE gausMaxFrameSizeTable[] = 
{
    16,  24,  32,  40,  48,  64,  96,  128, 256, 
    256, 256, 256, 256, 256, 256, 256, 256, 256,
};

/*
 * DEFINES
 ****************************************************************
 */




/*
 * TYPE DEFINITIONS
 ****************************************************************
 */
/*
 * GLOBAL VARIABLE DECLARATIONS
 ****************************************************************
 */
/*
 * FUNCTION DECLARATIONS
 ****************************************************************
 */



/**
 ****************************************************************
 * @brief pcd_request()
 *
 * ��    �ܣ�Ѱ��
 *
 * @param: req_code[IN]:Ѱ����ʽ
 *                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
 *                0x26 = Ѱδ��������״̬�Ŀ�
 * @param: ptagtype[OUT]����Ƭ���ʹ���
 *                0x4400 = Mifare_UltraLight
 *                0x4400 = Mifare_One(S50_0)
 *                0x0400 = Mifare_One(S50_3)
 *                0x0200 = Mifare_One(S70_0)
 				  0x4200 = Mifare_One(S70_3)
 *                0x0800 = Mifare_Pro
 *                0x0403 = Mifare_ProX
 *                0x4403 = Mifare_DESFire
 * 
 * @return: �ɹ�����MI_OK
 * @retval:
 ****************************************************************
 */
int pcd_request(u8 req_code, u8 *ptagtype)
{
	int status;
	
	transceive_buffer XDATA *pi;
	pi = &mf_com_data;
	
#if (NFC_DEBUG)
	printf("REQA/WUPA:\n");
#endif
	write_reg(BitFramingReg,0x07);	// Tx last bytes = 7

	clear_bit_mask(TxModeReg, BIT7); //��ʹ�ܷ���crc
	clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc
	clear_bit_mask(Status2Reg, BIT3);//����MF crypto1��֤�ɹ����
	pcd_set_tmo(4);
	
	mf_com_data.mf_command = PCD_TRANSCEIVE;
	mf_com_data.mf_length = 1;
	mf_com_data.mf_data[0] = req_code;

	status = pcd_com_transceive(pi);

	if (!status && mf_com_data.mf_length != 0x10)
	{
		status = MI_BITCOUNTERR;
	}
	*ptagtype = mf_com_data.mf_data[0];
	*(ptagtype + 1) = mf_com_data.mf_data[1];

	return status;
}


/**
 ****************************************************************
 * @brief pcd_anticoll() 
 *
 * ����ײ����
 * @param: select_code    0x93  cascaded level 1
 *                        0x95  cascaded level 2
 *                        0x97  cascaded level 3
 * @param: psnr ������к�(4byte)���ڴ浥Ԫ�׵�ַ
 * @return: status ֵΪMI_OK:�ɹ�
 * @retval: psnr  �õ������кŷ���ָ����Ԫ 
 ****************************************************************
 */
int pcd_cascaded_anticoll(u8 select_code, u8 coll_position, u8 *psnr)
{
	int status;
	u8 i;
	u8 temp;
	u8 bits;
	u8 bytes;
	//u8 coll_position;
	u8 snr_check;
	//u8 XDATA snr[4];
	u8 XDATA snr[5];
		
	transceive_buffer XDATA *pi;
	
	pi = &mf_com_data;
	snr_check = 0;
	coll_position = 0;
	memset(snr, 0, sizeof(snr));
#if (NFC_DEBUG)
	printf("ANT:\n");
#endif
	write_reg(BitFramingReg,0x00);	// // Tx last bits = 0, rx align = 0
	clear_bit_mask(TxModeReg, BIT7); //��ʹ�ܷ���crc
	clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc
	pcd_set_tmo(4);
	
	do 
	{
		bits = coll_position % 8;
		if (bits != 0)
		{
			bytes = coll_position / 8 + 1;
			clear_bit_mask(BitFramingReg, TxLastBits | RxAlign);
			set_bit_mask(BitFramingReg, (TxLastBits & (bits)) | (RxAlign & (bits << 4))); // tx lastbits , rx align
		}
		else
		{
			bytes = coll_position /8;
		}
		mf_com_data.mf_command = PCD_TRANSCEIVE;
		mf_com_data.mf_data[0] = select_code;
		mf_com_data.mf_data[1] = 0x20 + ((coll_position / 8) << 4) + (bits & 0x0F);
		
		for (i = 0; i < bytes; i++)
		{
			mf_com_data.mf_data[i + 2] = snr[i];
		}
		mf_com_data.mf_length = bytes + 2;

		status = pcd_com_transceive(pi);


		temp = snr[coll_position / 8];
		if (status == MI_COLLERR)
		{
			for (i = 0; (5 >= coll_position / 8) && (i < 5 - (coll_position / 8)); i++)
	        {
		         snr[i + (coll_position / 8)] = mf_com_data.mf_data[i + 1];
	        }
	        snr[(coll_position / 8)] |= temp;
			if(mf_com_data.mf_data[0] >= bits)
			{
	        	coll_position += mf_com_data.mf_data[0] - bits;
			}
			else
			{
				#if (NFC_DEBUG)
				printf("Err:coll_p  mf_data[0]=%02bx < bits=%02bx\n", mf_com_data.mf_data[0] ,  bits);
				#endif
			}


			//������ͻλ��ǰ����Чλ
			snr[(coll_position / 8)] &= (0xff >> (8 - (coll_position % 8)));
			//ѡ���ͻbitλΪ1����0�Ŀ�
			snr[(coll_position / 8)] |=  1 << (coll_position % 8);//ѡ��bit=1�Ŀ�
			//snr[(coll_position / 8)] &=  ~(1 << (coll_position % 8));//ѡ��bit=0�Ŀ�
			coll_position++; //��ͻbitλ��1
		}
		else if (status == MI_OK)
        {
            for (i=0; i < (mf_com_data.mf_length / 8) && (i <= 4); i++) //����(i <= 4)��ֹsnr[4-i]���
            {
                 snr[4 - i] = mf_com_data.mf_data[mf_com_data.mf_length / 8 - i - 1];
            }
            snr[(coll_position / 8)] |= temp;
        }

	}while (status == MI_COLLERR);

	if (status == MI_OK)
	{
		for (i = 0; i < 4; i++)
		{
			*(psnr + i) = snr[i];
			snr_check ^= snr[i];
		}
		if (snr_check != snr[i])
		{
			status = MI_COM_ERR;
		}
	}
	
	write_reg(BitFramingReg,0x00);	// // Tx last bits = 0, rx align = 0
	
	return status;
}



/**
 ****************************************************************
 * @brief pcd_select() 
 *
 * ѡ��һ�ſ�
 * @param: select_code    0x93  cascaded level 1
 *                        0x95  cascaded level 2
 *                        0x97  cascaded level 3
 * @param: psnr ������к�(4byte)���ڴ浥Ԫ�׵�ַ
 * @return: status ֵΪMI_OK:�ɹ�
 * @retval: psnr  �õ������кŷ���ָ����Ԫ 
 * @retval: psak  �õ���Select acknolege �ظ�
 *
 *			  sak:
 *            Corresponding to the specification in ISO 14443, this function
 *            is able to handle extended serial numbers. Therefore more than
 *            one select_code is possible.
 *
 *            Select codes:
 *
 *            +----+----+----+----+----+----+----+----+
 *            | b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |
 *            +-|--+-|--+-|--+-|--+----+----+----+-|--+
 *              |    |    |    |  |              | |
 *                                |              |
 *              1    0    0    1  | 001..std     | 1..bit frame anticoll
 *                                | 010..double  |
 *                                | 011..triple  |
 *
 *            SAK:
 *
 *            +----+----+----+----+----+----+----+----+
 *            | b8 | b7 | b6 | b5 | b4 | b3 | b2 | b1 |
 *            +-|--+-|--+-|--+-|--+-|--+-|--+-|--+-|--+
 *              |    |    |    |    |    |    |    |
 *                        |              |
 *                RFU     |      RFU     |      RFU
 *
 *                        1              0 .. UID complete, ATS available
 *                        0              0 .. UID complete, ATS not available
 *                        X              1 .. UID not complete
 *
 ****************************************************************
 */
int pcd_cascaded_select(u8 select_code, u8 *psnr,u8 *psak)
{
    u8 i;
    int status;
    u8 snr_check;
	
	transceive_buffer XDATA *pi;
    pi = &mf_com_data;
	snr_check = 0;
#if (NFC_DEBUG)
	printf("SELECT:\n");
#endif
	write_reg(BitFramingReg,0x00);	// // Tx last bits = 0, rx align = 0
	set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
	set_bit_mask(RxModeReg, BIT7); //ʹ�ܽ���crc
	
	pcd_set_tmo(4);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length = 7;
    mf_com_data.mf_data[0] = select_code;
    mf_com_data.mf_data[1] = 0x70;
    for (i = 0; i < 4; i++)
    {
    	snr_check ^= *(psnr + i);
    	mf_com_data.mf_data[i + 2] = *(psnr + i);
    }
    mf_com_data.mf_data[6] = snr_check;

    status = pcd_com_transceive(pi);
    
    if (status == MI_OK)
    {		
        if (mf_com_data.mf_length != 0x8)
        {   status = MI_BITCOUNTERR;   }
        else
        {  *psak = mf_com_data.mf_data[0];  }
    }

    return status;
}


  

 



/**
 ****************************************************************
 * @brief pcd_hlta() 
 *
 * ���ܣ������������״̬
 *
 * @param:
 * @return: status ֵΪMI_OK:�ɹ�
 *
 ****************************************************************
 */
int pcd_hlta(void)
{
    int status = MI_OK;
    
    transceive_buffer XDATA *pi;
    pi = &mf_com_data;
	
#if (NFC_DEBUG)
	printf("HALT:\n");
#endif
    write_reg(BitFramingReg,0x00);	// // Tx last bits = 0, rx align = 0
	set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
	clear_bit_mask(RxModeReg, BIT7); //��ʹ�ܽ���crc
	pcd_set_tmo(2); //according to 14443-3 1ms

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = PICC_HLTA;
    mf_com_data.mf_data[1] = 0;

    status = pcd_com_transceive(pi);
    if (status)
    {
        if (status==MI_NOTAGERR || status==MI_ACCESSTIMEOUT)
        {
        	status = MI_OK;
        }
    }
    return status;
}

int pcd_rats_a(u8 CID, u8 *ats)
{
    int status = MI_OK;
    transceive_buffer XDATA *pi;
	u8 XDATA * ATS;
	u8 ta,tb,tc;
							  
	pi = &mf_com_data;
	ta = tb = tc  = 0;

	/*initialiszed the PCB*/
    g_pcd_module_info.ucPcdPcb  = 0x02;
    g_pcd_module_info.ucPiccPcb = 0x03;
    g_pcd_module_info.ucCid = CID;

	pcd_delay_sfgi(g_pcd_module_info.uiSfgi);

	pcd_set_tmo(4); //according to 14443-4 4.8ms
	
    write_reg(BitFramingReg,0x00);	// // Tx last bits = 0, rx align = 0
	set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
	set_bit_mask(RxModeReg, BIT7); //ʹ�ܽ���crc
	
	mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 2;
    mf_com_data.mf_data[0] = PICC_RATS; //Start byte
    mf_com_data.mf_data[1] = (FSDI << 4) + CID; //Parameter
	
	status = pcd_com_transceive(pi);

	if ( (status == MI_OK && (pi->mf_length % 8) != 0)
		|| (status == MI_COLLERR)
		|| (status == MI_INTEGRITY_ERR && pi->mf_length / 8 < 4)
		|| (status == MI_INTEGRITY_ERR && pi->mf_length / 8 >= 4 && (pi->mf_length % 8) != 0) 
	)
	{
		//printf("2\n");
		status = MI_NOTAGERR; 
	}
	
	if (MI_OK == status)
	{
		//ATS
		ATS = pi->mf_data;
		ats[0] = ATS[0];
		ats[1] = ATS[1];
		ats[2] = ATS[2];
			
		//printf("3\n");
		if (pi->mf_length / 8 < 1 || ATS[0] != pi->mf_length / 8)
		{//at least 1bytes, and	TL = length		
		
			//printf("4\n");
			return PROTOCOL_ERR;
		}
			
		if ( (ATS[0] < (2 + ((ATS[1]&BIT4)>>4) + ((ATS[1]&BIT5)>>5) + ((ATS[1]&BIT6)>>6))))
		{//ERR:TL length
			//return PROTOCOL_ERR;
		}
		else
		{
			if (!(ATS[1]&BIT7))
			{//T0.7 = 0
				if (ATS[1]&BIT4)
				{
					ta = 1;
				}
				if (ATS[1]&BIT5)
				{
					tb = 1;
					g_pcd_module_info.uiFwi = (ATS[2+(u8)ta] & 0xF0) >> 4;
					g_pcd_module_info.uiSfgi = ATS[2+(u8)ta] & 0x0f;
					
				}
				if (ATS[1]&BIT6)
				{
					tc = 1;
					g_pcd_module_info.ucCidEn = 0; //?
					g_pcd_module_info.ucNadEn = 0; //?					
				}
				g_pcd_module_info.uiFsc = gausMaxFrameSizeTable[ATS[1] & 0x0F];

				//FSC
				if (pi->mf_length/8 < (2 + (u8)ta + (u8)tb + (u8)tc))
				{//���ȴ���
					return PROTOCOL_ERR;
				}

				pcd_set_tmo(g_pcd_module_info.uiFwi);

			}
			else
			{
				//status = PROTOCOL_ERR;
			}
		}
	}

	return status;
}

  
/**
 ****************************************************************
 * @brief pcd_pps_rate() 
 *
 * ���ܣ���ISO14443-4������COS����
 *
 *
 ****************************************************************
 */
int pcd_pps_rate(transceive_buffer *pi, u8  *ATS, u8 CID, u8 rate)
{
	u8 DRI, DSI;
	int status = MI_OK;

#if (NFC_DEBUG)
	printf("PPS:\n");
#endif 

	DRI = 0;
	DSI = 0;

	if ((ATS[0] > 1) && (ATS[1] & BIT5))
    {//TA(1) transmited
    	if (rate == 1)
    	{
			
    	}
    	else if (rate == 2)
    	{
			printf("212K\n");
			if((ATS[2]&BIT0) && (ATS[2]&BIT4))
			{// DS=2,DR=2 supported 212kbps
				DRI = 1;
				DSI = 1;
			}
			else
			{
				printf(",Unsupport\n");
				return USER_ERROR;
			}	
    	}
		else if (rate == 3)
		{
			printf("424K\n");
			if((ATS[2]&BIT1) && (ATS[2]&BIT5))
			{// DS=4,DR=4 supported 424kbps
				DRI = 2;
				DSI = 2;
			}
			else
			{
				printf(",Unsupport\n");
				return USER_ERROR;
			}
		}
		else if (rate == 4)
		{
			printf("848K\n");
			if((ATS[2]&BIT2) && (ATS[2]&BIT6))
			{// DS=4,DR=4 supported 424kbps
				DRI = 3;
				DSI = 3;
			}
			else
			{
				printf(",Unsupport\n");
				return USER_ERROR;
			}
		}
		else
		{		
			printf("USER:No Rate select\n");
			return USER_ERROR;
		}
		write_reg(BitFramingReg,0x00);	// // Tx last bits = 0, rx align = 0
		set_bit_mask(TxModeReg, BIT7); //ʹ�ܷ���crc
		set_bit_mask(RxModeReg, BIT7); //ʹ�ܽ���crc
	
		pi->mf_command = PCD_TRANSCEIVE;
	    pi->mf_length  = 3;
	    pi->mf_data[0] = (0x0D << 4) + CID; //Start byte
	    pi->mf_data[1] = 0x01 | BIT4; //PPS0 ;BIT4:PPS1 transmited
	    pi->mf_data[2] = (DSI << 2) | DRI; //PPS1
		status = pcd_com_transceive(pi);
		if (status == MI_OK)
		{
			if (pi->mf_length == 8 && pi->mf_data[0] == ((0x0D << 4) + CID))
			{//PPS ok
				printf("pcd_pps_rate OK\n");
				if (rate == 1)
				{
					//pps��106k
					
				}
				else if (rate == 2)
				{
					//pps��212k
					pcd_set_rate('2');// 212kbps
				}
				else if (rate == 3)
				{
					//pps��424k
					pcd_set_rate('4');// 424kbps
				}
				else if (rate == 4)
				{
					//pps��848k
					pcd_set_rate('8');// 848kbps
				}				
				
			}
		}

	}

	return status;
}

