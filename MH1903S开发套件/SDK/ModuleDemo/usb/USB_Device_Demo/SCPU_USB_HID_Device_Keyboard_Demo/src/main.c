/**
  ******************************************************************************
  * @file    app.c
  * @author  Megahuntmicro
  * @version V1.0.0
  * @date    2019-03-27
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
  
#include <stdio.h>

#include "mhscpu.h"
#include "hid_keyboard.h"

void NVIC_Configuration(void);
void UART_Configuration(void);


int main(void)
{
    uint8_t ReceiveCode = 0;    
    
    SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);    
       
    /* Enable CLock */
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);

	UART_Configuration();
    Keyboard_Configuration();
	NVIC_Configuration();	
    
    printf("MegaHunt SCPU HID Keyboard Demo V1.0.\n");
    
	while(1)
	{
        if (UART0->LSR & 0x01)
        {
            ReceiveCode = (uint8_t )UART0->OFFSET_0.RBR;
            Keyboard_SendKey(ReceiveCode);
        }
	}
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable USB Interrupt */	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = USB_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
    
	UART_Init(UART0, &UART_InitStructure);	
}

int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
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

