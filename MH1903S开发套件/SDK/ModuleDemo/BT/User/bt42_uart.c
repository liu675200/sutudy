#include <stdio.h>
#include "mhscpu.h"
#include "bt42_api.h"



#define MAX_BUFFER_SIZE 		1024

struct uart_param{
	uint32_t w_ptr;
	uint32_t r_ptr;
	uint8_t UartRxBuf[MAX_BUFFER_SIZE];
}uart_param_t = {
	.w_ptr = 0,
	.r_ptr = 0,
};


void blue_uart_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
    UART_FIFOInitTypeDef UART_FIFOInitStruct;
    NVIC_InitTypeDef NVIC_InitStructure;

    //enable uart1 & dma
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART1, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART1, ENABLE);

    //UART1 GPIO PIN Remap
    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 , GPIO_Remap_3);
	
    UART_StructInit(&UART_InitStructure);
    UART_FIFOStructInit(&UART_FIFOInitStruct);

    UART_InitStructure.UART_BaudRate = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;

    UART_FIFOInitStruct.FIFO_Enable = ENABLE;
    UART_FIFOInitStruct.FIFO_DMA_Mode = UART_FIFO_DMA_Mode_0;
    UART_FIFOInitStruct.FIFO_RX_Trigger = UART_FIFO_RX_Trigger_1_2_Full;
    UART_FIFOInitStruct.FIFO_TX_Trigger = UART_FIFO_TX_Trigger_1_4_Full;
    UART_FIFOInitStruct.FIFO_TX_TriggerIntEnable = DISABLE;

    UART_Init(UART1, &UART_InitStructure);	
    UART_FIFOInit(UART1, &UART_FIFOInitStruct);

    NVIC_InitStructure.NVIC_IRQChannel = UART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    UART_ITConfig(UART1, UART_IT_RX_RECVD, ENABLE);
}

int uart_block_write(uint8_t *pbuffer,uint32_t len)
{
	for(int i = 0;i < len ;i++)
	{
		while(!UART_IsTXEmpty(UART1));
		UART_SendData(UART1, pbuffer[i]);
	}
	return 0;
}

uint16_t uart_read(uint8_t *pDataBuf, uint16_t len)
{
  uint16_t ulLen = 0;

	while (len--)
	{
		if (uart_param_t.r_ptr != uart_param_t.w_ptr)
		{
			*pDataBuf++ = uart_param_t.UartRxBuf[uart_param_t.r_ptr];
			uart_param_t.r_ptr = (uart_param_t.r_ptr + 1) % MAX_BUFFER_SIZE;
			ulLen++;
		}
		else
		{
				break;
		}
	}
  return ulLen;
}

void UART1_IRQHandler(void)
{
	UART_TypeDef * UARTx = UART1;

    switch(UARTx->OFFSET_8.IIR & 0x0f)
    {
        case UART_IT_ID_CHAR_TIMEOUT:
        case UART_IT_ID_RX_RECVD:
        {
            while (UART_IsRXFIFONotEmpty(UARTx))
            {
                uart_param_t.UartRxBuf[uart_param_t.w_ptr] = UART_ReceiveData(UARTx);
                uart_param_t.w_ptr = (uart_param_t.w_ptr + 1) % MAX_BUFFER_SIZE;
            }
        }
        break;
        case UART_IT_ID_TX_EMPTY:
        {
            UART_ITConfig(UARTx, UART_IT_TX_EMPTY, DISABLE);
        }
        break;
        case UART_IT_ID_MODEM_STATUS:
        {
            uint32_t msr = UARTx->MSR;          
        }
        break;
        case UART_IT_ID_LINE_STATUS:
        {
            uint32_t lsr = UARTx->LSR;   
        }   
        break;
        case UART_IT_ID_BUSY_DETECT:
        {
            uint32_t usr = UARTx->USR;          
        }   
    break;

    default:
        break;
    }
}
