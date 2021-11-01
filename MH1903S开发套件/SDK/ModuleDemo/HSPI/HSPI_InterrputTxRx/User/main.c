
#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DATA_SIZE					(256)
#define UARTx  						(UART0)

void UART_Configuration(void);
void HSPIM_Configuration(uint8_t SampleMode);
void NVIC_Configuration(void);
void DataPrintf(uint8_t *Buff, uint32_t BuffSize);

uint8_t SendBuff[256],RecvBuff[256];
uint8_t SendBuff[DATA_SIZE];
uint8_t RecvBuff[DATA_SIZE];
uint16_t SendIndex = 0;
uint16_t RecvIndex = 0;
uint8_t SendFlag = 0;
uint8_t RecvFlag = 0;

int main(void)
{
	uint16_t i = 0;
	uint8_t buff[DATA_SIZE];

    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_HSPI, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);
	
	UART_Configuration();
	
	for ( i = 0; i < DATA_SIZE; i++)
	{
		SendBuff[i] = i & 0xFF;
		buff[i] = SendBuff[i];
	}
		
	memset(RecvBuff, 0xA5, 256);
	
	HSPIM_Configuration(3);
	NVIC_Configuration();
	
	printf("MH1903S HSPI DEMO V1.0\n");
	
	while(1)
	{
		if (RecvFlag)
		{
			RecvFlag = 0;
			
			if (0 == memcmp(buff, RecvBuff, DATA_SIZE))
			{
				printf(">>> Test Pass !\n");
				DataPrintf(RecvBuff, DATA_SIZE);
			}
			else 
			{
				printf(">>> Test Fail !\n");
				DataPrintf(RecvBuff, DATA_SIZE);
			}
		}
	}
}

void SPI5_IRQHandler(void)
{
	HSPIM->FLSR;
	
	if (SET == HSPIM_GetITStatus(HSPIM_IT_ID_TXTF))
	{	
		HSPIM_SendData(SendBuff[SendIndex++]);
		if(DATA_SIZE == SendIndex)
		{
			SendFlag = 1;
			SendIndex = 0;
			HSPIM_ITConfig(HSPIM_IT_TX, DISABLE);	
		}
	}
	
	if (SET == HSPIM_GetITStatus(HSPIM_IT_ID_RXTF))
	{
		while(RESET == HSPIM_GetFlagStatus(HSPIM_FLAG_RXE))
		{
			RecvBuff[RecvIndex++] = HSPIM_ReceiveData();
			if (DATA_SIZE == RecvIndex)
			{
				RecvFlag = 1;
			}
		}
	}
	
	if(RESET == HSPIM_GetFlagStatus(HSPIM_FLAG_RXE))
	{
		RecvBuff[RecvIndex++] = HSPIM_ReceiveData();
		if ((DATA_SIZE) == RecvIndex)
		{
			RecvFlag = 1;
		}
	}
}


void HSPIM_Configuration(uint8_t SampleMode)
{
	HSPIM_InitTypeDef hspim;
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_14 |GPIO_Pin_15 , GPIO_Remap_3);
	
	hspim.HSPIM_Enable = ENABLE;
	hspim.HSPIM_Mode = HSPIM_MODE_STANDARD;
	hspim.HSPIM_CPOL = (SampleMode >> 1) & 0x01;
	hspim.HSPIM_CPHA = SampleMode & 0x01;
	hspim.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	hspim.HSPIM_DivideEnable = ENABLE;
	hspim.HSPIM_BaudRatePrescaler = HSPIM_BAUDRATE_PRESCALER_16;
	hspim.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_1;
	hspim.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_1;
	
	HSPIM_Init(&hspim);	
	HSPIM_FIFOReset(HSPIM_FIFO_TX | HSPIM_FIFO_RX);	
	HSPIM_TransmitCmd(ENABLE);
	
	HSPIM_ITConfig(HSPIM_IT_EN, ENABLE);
	HSPIM_ITConfig(HSPIM_IT_RX, ENABLE);
	HSPIM_ITConfig(HSPIM_IT_TX, ENABLE);
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef nvic;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	
	nvic.NVIC_IRQChannel = SPI5_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 1;
	nvic.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvic);
}

void UART_Configuration(void)
{
	UART_InitTypeDef uart;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

	uart.UART_BaudRate = 115200;
	uart.UART_WordLength = UART_WordLength_8b;
	uart.UART_StopBits = UART_StopBits_1;
	uart.UART_Parity = UART_Parity_No;

	UART_Init(UARTx, &uart);
}


void DataPrintf(uint8_t *Buff, uint32_t BuffSize)
{
	uint32_t i = 0;
	if(0 != BuffSize)
	{
		for(i = 0; i < BuffSize; i++)
		{
			if(0 != i && 0 == i%16)
				printf("\n");
			printf("%02X ",Buff[i]);
		}
	}
	printf("\n\n");
}

int SER_PutChar(int ch)
{
	while(!UART_IsTXEmpty(UARTx));
	UART_SendData(UARTx, (uint8_t) ch);

	return ch;
}

int fputc(int c, FILE *f)
{
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
	while(1);
}
#endif

