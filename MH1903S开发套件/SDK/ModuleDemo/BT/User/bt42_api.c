
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>       //< for va_list
#include "mhscpu.h"
#include "bt42_api.h"
#include "stdbool.h"
#include "bt42_uart.h"

void bt42_api_event(uint8_t *event, uint16_t len)
{
	
	bt42_hci_event_debug(event, len);

    //<������ÿͻ�event�������
	bttest_event_process(event, len);
	
}

uint32_t hal_time_ms(void)
{
	return GetTimerCount();
}

int hal_print(const char *fmt, ...)
{
	//<ʾ��1���ض��򵽱�׼printf��ʹ��MicroLIB 

	va_list argptr; 
	va_start(argptr, fmt);
	vprintf(fmt, argptr);
	va_end(argptr);


}

void hal_uart_set_baud (uint32_t baud)
{
    UART_InitTypeDef UART_InitStructure;
    
    UART_InitStructure.UART_BaudRate = baud;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_Init(UART1, &UART_InitStructure);	
}

void bt42_uart_manual_rts_set(bool state)
{
	if(true == state)
	{
		UART_SetRTS(UART1);
	}
	else
	{
		UART_ResetRTS(UART1);
	}
}

void bt42_uart_autoflowctrlcmd(int new_state)
{
	UART_AutoFlowCtrlCmd(UART1, new_state);
}

/*
* param1 : buf ������NULL
* param2 : len ������0
* return : ��ȡ�����ַ�����
*/
uint16_t hal_uart_read(uint8_t *buf, uint32_t len)
{
	//<������ÿͻ����ڶ�����
	return uart_read(buf,len);
}
/*
* function: ��������ʽ���ͣ���������ٷ���
* param1 : buf ������NULL
* param2 : len ������0
* return : 0:write succeed or error��
*/	
uint16_t hal_uart_block_write(uint8_t *buf, uint32_t len)
{
	//<������ÿͻ�����д����
	return uart_block_write(buf,len);
}

/*
*	������������͹���ģʽ
*	state:
*		1: normal mode
*		0: sleep mode
*/
void mcu_wake_bt_pin(uint8_t state)
{
	if (state)
	{
	    //< BT to normal mode
		BT_MCUWAKEUPBT_SET();
	}
	else
	{
	    //< BT to lowpower mode
		BT_MCUWAKEUPBT_CLR();
	}
}


/*
*	��ȡ����оƬ������״̬
*	state:
*		1: normal mode
*		0: sleep mode
*/
int get_mcu_wake_bt_pin_level(void)
{
	return GPIO_ReadOutputDataBit(BT_MCUWAKEUPBT_IO_GROUP, BT_MCUWAKEUPBT_IO_PIN); 
}
