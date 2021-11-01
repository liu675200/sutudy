/************************ (C) COPYRIGHT Megahuntmicro ********************************************************
 * @file                : main.c
 * @author              : Megahuntmicro
 * @version             : V1.0.0
 * @date                : 9-November-2019
 *************************************************************************************************************/
#include <stdio.h>
#include "mhscpu.h"
#include "hspi_flash.h"

#define DATA_SIZE								(0x100)
#define FLASH_WRITE_ADDR						(0x000000)
#define UARTx									(UART0)

void UART_Configuration(void);
void HSPIM_Configuration(uint8_t SampleMode, uint8_t BaudRate);
void DataPrintf(uint8_t* Buff, uint32_t BuffSize);
extern void LCD_Test(void);

uint8_t SendBuff[DATA_SIZE] = {0};
uint8_t RecvBuff[DATA_SIZE] = {0};

uint32_t FlashID = 0;

int main(void)
{
	uint16_t i = 0;

    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_HSPI, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 , ENABLE);

	UART_Configuration();
	HSPIM_Configuration(0, 8);	// SampleMode : 0 3
	
	printf(">>> MHSCPU HSPIM Flash Demo v1.0\n");
	
	FlashID = QF_ReadID();	
	printf("FlashID : %08x\n", FlashID);
	

	while(1)
	{
		memset(SendBuff, 0xA5, DATA_SIZE);
		memset(RecvBuff, 0x00, DATA_SIZE);
		QF_EraseSector(FLASH_WRITE_ADDR);
		QF_ReadBuff(RecvBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		DataPrintf(RecvBuff, DATA_SIZE);	
		QF_WriteBuff(SendBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		QF_ReadBuff(RecvBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		DataPrintf(RecvBuff, DATA_SIZE);
		//printf(">>>>> QSPI flash test : %s\n", (0 == memcmp(SendBuff, RecvBuff, DATA_SIZE)) ? "Pass !" : "Failed");	
		if (0 != memcmp(SendBuff, RecvBuff, DATA_SIZE))
		{
			while(1);
		}


		memset(SendBuff, 0x5A, DATA_SIZE);
		memset(RecvBuff, 0x00, DATA_SIZE);
		QF_EraseSector(FLASH_WRITE_ADDR);
		QF_ReadBuff(RecvBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		DataPrintf(RecvBuff, DATA_SIZE);
		QF_WriteBuff(SendBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		QF_ReadBuff(RecvBuff, FLASH_WRITE_ADDR, DATA_SIZE);
			DataPrintf(RecvBuff, DATA_SIZE);
	//		printf(">>>>> QSPI flash test : %s\n", (0 == memcmp(SendBuff, RecvBuff, DATA_SIZE)) ? "Pass !" : "Failed");	
		if (0 != memcmp(SendBuff, RecvBuff, DATA_SIZE))
		{
			while(1);
		}


		for(i = 0; i < DATA_SIZE; i++)
		{
			SendBuff[i] = i & 0xFF;
		}
		memset(RecvBuff, 0x00, DATA_SIZE);
		//QF_EraseChip();
		QF_EraseSector(FLASH_WRITE_ADDR);
		QF_ReadBuff(RecvBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		DataPrintf(RecvBuff, DATA_SIZE);	
		QF_WriteBuff(SendBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		QF_ReadBuff(RecvBuff, FLASH_WRITE_ADDR, DATA_SIZE);
		DataPrintf(RecvBuff, DATA_SIZE);
	//		printf(">>>>> QSPI flash test : %s\n", (0 == memcmp(SendBuff, RecvBuff, DATA_SIZE)) ? "Pass !" : "Failed");			
		if (0 != memcmp(SendBuff, RecvBuff, DATA_SIZE))
		{
			while(1);
		}
		
			
		QF_CS_HIGH();
		
		LCD_Test();
		
		HSPIM_FIFOReset(HSPIM_FIFO_TX | HSPIM_FIFO_RX);	
		
	}

	

}


void HSPIM_Configuration(uint8_t SampleMode, uint8_t BaudRate)
{
	HSPIM_InitTypeDef hspim;
	GPIO_InitTypeDef gpio;
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Pin = QF_CS_PIN;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(QF_CS_PORT, &gpio);
	QF_CS_HIGH();
	
	gpio.GPIO_Pin = GPIO_Pin_5;						// LCD_CS
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_6;						// LCD_A0
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_7;						// LCD_RST
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &gpio);
	
	gpio.GPIO_Pin = GPIO_Pin_8;						// LCD_LIGHT
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &gpio);
	
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 
					| GPIO_Pin_13 |GPIO_Pin_15 , GPIO_Remap_3);

	hspim.HSPIM_Enable = ENABLE;
	hspim.HSPIM_Mode = HSPIM_MODE_STANDARD;
	hspim.HSPIM_CPOL = (SampleMode >> 1) & 0x01;
	hspim.HSPIM_CPHA = SampleMode & 0x01;
	hspim.HSPIM_FirstBit = HSPIM_FIRST_BIT_MSB;
	hspim.HSPIM_DivideEnable = ENABLE;
	hspim.HSPIM_BaudRatePrescaler = BaudRate;
	hspim.HSPIM_RXFIFOFullThreshold = HSPIM_RX_FIFO_FULL_THRESHOLD_1;
	hspim.HSPIM_TXFIFOEmptyThreshold = HSPIM_TX_FIFO_EMPTY_THRESHOLD_1;
	
	HSPIM_Init(&hspim);
	HSPIM_FIFOReset(HSPIM_FIFO_TX | HSPIM_FIFO_RX);	
	HSPIM_TransmitCmd(ENABLE);
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

