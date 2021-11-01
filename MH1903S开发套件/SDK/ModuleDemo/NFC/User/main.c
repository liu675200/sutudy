/**
 ****************************************************************
 * @file main.c
 *
 * @brief  main entry of test.
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
#include "gpio.h"
#include "spi.h"
#include "uart.h"
#include "timer.h"
#include "rfid.h"
#include "mh523.h"
#include "iso14443_4.h"
#include "iso14443a.h"
#include "iso14443b.h"

#define COM_BUF_SIZE        50
#define HEAD                0x68

typedef struct r_w_reg_s
{
	u8 addr;          //��д�ļĴ�����ַ
	u8 rw_dat;        //��д�ļĴ���ֵ
}r_w_reg;

typedef struct pc_tx_pkt_s
{
	u8 head;          //��ͷ
	u8 len;           //�������Ӱ�ͷ��У���
	u8 cmd;	          //������

	union datu
	{
		r_w_reg reg;
		u8 dat[1];    //���ͻ���յ�����
	}datu;
}pc_tx_pkt;

typedef struct lpcd_config_s
{
	u8 delta;
	u32 t_inactivity_ms;
	u8 skip_times;
	u8 t_detect_us;
}lpcd_config_t;

lpcd_config_t XDATA g_lpcd_config;
static u8 XDATA com_tx_buf[ COM_BUF_SIZE ];//pc to mcu
static u16 tx_buf_index = 0;

#if (POWERON_POLLING)
	u8 g_query_mode = 1; //�ֶ�����оƬģʽ
	u8 g_typa_poll = TRUE;
	u8 g_typb_poll = FALSE;
	u8 g_need_reconfig = TRUE;//��Ҫ���³�ʼ��PCD��Э������
#else
	u8 g_query_mode = 0; //�ֶ�����оƬģʽ
	u8 g_typa_poll = FALSE;
	u8 g_typb_poll = FALSE;
	u8 g_need_reconfig = FALSE;//��Ҫ���³�ʼ��PCD��Э������
#endif

u32 XDATA g_polling_cnt = 0;
u8 g_lpcd_started = FALSE; //LPCD�����Զ�̽�⿨�����Ƿ���
u8 g_lpcd_config_test_start = FALSE;//LPCD�����Զ�̽�⿨�����Ƿ���
pc_tx_pkt XDATA *recv_packet(void);
char prase_packet(pc_tx_pkt XDATA *ppkt);
void discard_pc_pkt(pc_tx_pkt XDATA *ppkt);

u8 check_sum(u8 XDATA *buf, int len)
{
	u8 sum = 0;

	while (len--)
		sum += *buf++;

	return sum;
}

void make_packet(u8 cmd, u8 *buf, u8 len)
{
	u8 sum = 0;

	sum += HEAD;
	uart_SendChar(HEAD);
	sum += len + 4;
	uart_SendChar(len + 4);	
	sum += cmd;
	uart_SendChar(cmd);

	while (len--)
	{
		uart_SendChar(*buf);
		sum += *buf++;
	}
	uart_SendChar(sum);
}

pc_tx_pkt XDATA *recv_packet(void)
{
	pc_tx_pkt *XDATA ppkt;
	
	while(uart_getbytes() && tx_buf_index < COM_BUF_SIZE )
	{
		com_tx_buf[tx_buf_index] = (int8_t)uart_RecvChar();
		tx_buf_index++;
	}
	if ( tx_buf_index >= 4 )
	{
		u8 i;
		u8 rest = 4;
		
		for (i=0; i<tx_buf_index-3; )//
		{
			ppkt = (pc_tx_pkt *)(com_tx_buf + i);
			if (ppkt->head == 0x68 && ppkt->len > 0 
				&& ppkt->len + i < COM_BUF_SIZE)//�ж��ǲ����ܽ������
			{
				
				if (ppkt->len + i <= tx_buf_index)//�жϸð���û�н��������
				{
					u8 sum = check_sum((u8 XDATA *)ppkt, ppkt->len - 1);
					
					if (sum == ppkt->datu.dat[ppkt->len - 4] )
					{	
						return ppkt;
					}
					else
					{
						i++;
					}
				}
				else
				{
					 rest = tx_buf_index - i;
					 break;
				}
			}
			else
			{
				i++;
			}
		}
		memmove(com_tx_buf, com_tx_buf + tx_buf_index - rest, rest);
		tx_buf_index = rest;
	}			

	return NULL;
}

char prase_packet(pc_tx_pkt XDATA *ppkt)
{
	switch(ppkt->cmd)
	{
		case COM_PKT_CMD_READ_REG:	//Ҫ��ѵ�ַ��ֵ�����ظ�pc
		{
			r_w_reg XDATA reg;			 
			reg.addr = ppkt->datu.reg.addr;
			reg.rw_dat = read_reg(ppkt->datu.reg.addr);		
				
			make_packet(COM_PKT_CMD_READ_REG, (u8 *)&reg, sizeof(reg));
			break;
		}
		
		case COM_PKT_CMD_WRITE_REG:
		{		
			write_reg(ppkt->datu.reg.addr, ppkt->datu.reg.rw_dat);
			break;
		}

		case COM_PKT_CMD_QUERY_MODE:
		{
			if (ppkt->datu.dat[0] == 1)
			{
				g_query_mode = TRUE;
				if (ppkt->datu.dat[1] == 1)
				{//typeA poll
					pcd_config('A');
					g_typa_poll = TRUE;
				}
				else
				{
					g_typa_poll = FALSE;
				}
				if (ppkt->datu.dat[2] == 1)
				{//typeB poll
					pcd_config('B');
					g_typb_poll = TRUE;
				}
				else
				{
					g_typb_poll = FALSE;
				}
				if (g_typa_poll && g_typb_poll)
				{
					g_need_reconfig = TRUE;
				}
				else
				{
					g_need_reconfig = FALSE;
				}
				g_polling_cnt = *((u32*)&ppkt->datu.dat[3]);
				printf("g_polling_cnt=%lu\n", g_polling_cnt);
			}
			else
			{
				g_query_mode = FALSE;
			}

			break;
		}
		case COM_PKT_CMD_CHIP_RESET:
		{
			pcd_reset();
			mdelay(7);//carrier off 7ms
			pcd_antenna_on();
		break;
		}
		case COM_PKT_CMD_HALT:
		{
			pcd_hlta();
			printf("HltA\n");
		 	break;
		}
		case COM_PKT_CMD_LPCD:
		{
			g_lpcd_started = TRUE;
			pcd_lpcd_start();
			break;
		}
		case COM_PKT_CMD_LPCD_CONFIG_TEST:
		{
			int XDATA z;
			memcpy(&g_lpcd_config, &ppkt->datu.dat[0], sizeof(g_lpcd_config));
			g_lpcd_config.t_inactivity_ms = ntohl(g_lpcd_config.t_inactivity_ms);
			for (z = 0; z < sizeof(g_lpcd_config);z++)
			{
				printf(" %02x", ppkt->datu.dat[z]);
			}
			pcd_lpcd_config_start(g_lpcd_config.delta, g_lpcd_config.t_inactivity_ms, g_lpcd_config.skip_times, g_lpcd_config.t_detect_us);	
			printf("config=%d,%lu,%d,%d\n",g_lpcd_config.delta, g_lpcd_config.t_inactivity_ms, g_lpcd_config.skip_times, g_lpcd_config.t_detect_us);
			g_lpcd_config_test_start = TRUE;
			break;
		}
		case COM_PKT_CMD_LPCD_CONFIG_TEST_STOP:
		{
			pcd_lpcd_end();
			g_lpcd_config_test_start = FALSE;
			break;
		}
	
		default:
			return FALSE;
			//break;
	}
	return TRUE;
}


void discard_pc_pkt(pc_tx_pkt XDATA *ppkt)
{
	int bytes;
	u8 XDATA *p = ((u8 XDATA *)ppkt) + ppkt->len;

	bytes = (p - com_tx_buf);
	if (bytes <= tx_buf_index)
	{
		memmove(com_tx_buf, com_tx_buf + bytes, tx_buf_index - bytes);
		tx_buf_index -= bytes;
	}
	else
	{
		tx_buf_index = 0;
	}
}

int main()
{
	pc_tx_pkt XDATA *ppkt;
	tick XDATA g_statistic_last_t = 0;
	u8 XDATA status;

	SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_168MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI2 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_SPI2 | SYSCTRL_APBPeriph_TIMM0, ENABLE);

	uart_Config(115200, UART_Parity_No);
	init_timer0();
	gpio_config();
	spi_config();
	
	printf("NFC_SDK_STC_V%s start...\n", VERSION);
	
	pcd_init();//��ʼ��pcd�Ĵ���
	rfid_init();//��ʼ��rfid���ر���

	while(1)
	{	
		ppkt = recv_packet();
		if (ppkt)
		{
			//����Ƿ��ǵ��������
			if (FALSE == prase_packet(ppkt))
			{
				//RFID�����
				rfid_operation(&ppkt->cmd);
			}
			discard_pc_pkt(ppkt);
		}

		//�Զ���ѯѰ��ģʽ
		if (g_query_mode && (g_typa_poll || g_typb_poll))
		{
			u8 cmd[2];
			u8 status_a = 1, status_b = 1;

			pcd_antenna_off();
			mdelay(10);
			pcd_antenna_on();
			mdelay(10);
			
			if (g_typa_poll == TRUE)
			{
				if (g_typa_poll & g_typb_poll)
				{
					pcd_config('A');
				}
				cmd[1] = 0x52;

				//pcd_default_info();
					
				status_a = com_reqa((u8 *)&cmd);//ѯ����A��				
			}
		
			if (g_typb_poll == TRUE)
			{	
				if (g_typa_poll & g_typb_poll)
				{
					pcd_config('B');
				}			
				cmd[1] = 0x08;
				status_b = com_reqb((u8 *)&cmd);//ѯ����B��
			}
		
			//������Ӧ��
			if (status_a == MI_OK || status_b == MI_OK)
			{
				led_success_on();//�ɹ�
			}
			else
			{
				led_success_off();//ʧ��
			}
		
		}

		//cosָ����ѯ����
		if (g_cos_loop == TRUE)
		{
			if (g_cos_loop_times > 0)
			{
				com_exchange(g_loop_buf);
				g_cos_loop_times--;
				mdelay(20);
			}
			else
			{
				mdelay(200);
				g_cos_loop = FALSE;
				g_statistic_refreshed = TRUE;
				statistic_print();
				make_packet(COM_PKT_CMD_TEST_STOP, NULL, NULL);
			}
		}
		
		//�Զ������
		if (g_lpcd_started == TRUE)
		{
			if (TRUE == pcd_lpcd_check())
			{
				g_lpcd_started = FALSE;
				make_packet(COM_PKT_CMD_LPCD, NULL, NULL);
			}
		}
		//�Զ�������������
		if (g_lpcd_config_test_start == TRUE)
		{
			if (TRUE == pcd_lpcd_check())
			{
				u8 XDATA tag_type[2];
					
				g_statistics.lpcd_cnt++;
				g_statistic_refreshed = TRUE;
				statistic_print();
				
				//��֤�Ƿ��п��볡
				pcd_config('A');
				status = pcd_request(0x52, tag_type);
				//�ռ�⵽��ʱ���ܴ��ھ����Ե���п��ܵ�һ��Ѱ��ʧ�ܣ��������ӵڶ���Ѱ����֤
				if (status != MI_OK)
				{					
					status = pcd_request(0x52, tag_type);
				}
				///�ռ�⵽��ʱ���ܴ��ھ����Ե�����ǰ����Ѱ��ʧ�ܣ��������Ѱ����֤
				if (status != MI_OK)
				{					
					status = pcd_request(0x52, tag_type);
				}
				
				if (status == MI_OK)
				{//�п�Ƭ�볡
					//����Ӿ����Ӧ�ù��ܴ���
					/*

					*/
					//�ȴ����볡,Ӧ�ô����пɲ��صȴ��볡
					while(1)
					{
						pcd_antenna_off();
						mdelay(10);
						pcd_antenna_on();
						mdelay(10);	
						status = pcd_request(0x52, tag_type);
						if(status != MI_OK)
						{//һ����֤���볡
							mdelay(100);
							status = pcd_request(0x52, tag_type);
							if(status != MI_OK)
							{//������֤���볡
								mdelay(100);
								status = pcd_request(0x52, tag_type);
								if(status != MI_OK)
								{//������֤���볡
									mdelay(100);
									pcd_lpcd_config_start(g_lpcd_config.delta, g_lpcd_config.t_inactivity_ms, g_lpcd_config.skip_times, g_lpcd_config.t_detect_us);	
									break;	
								}
							}
	
						}
					}
				}
				else
				{
					g_statistics.lpcd_fail++;
					g_statistic_refreshed = TRUE;
					pcd_lpcd_config_start(g_lpcd_config.delta, g_lpcd_config.t_inactivity_ms, g_lpcd_config.skip_times, g_lpcd_config.t_detect_us);	
				}
			}
		}
		
		//���ͳ����Ϣ
		if (is_timeout(g_statistic_last_t, 100))
		{
			g_statistic_last_t = get_tick();
			statistic_print();
		}
	}		
}

int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
	if (ch == '\n')
	{
		fputc('\r', f);
	}
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif
