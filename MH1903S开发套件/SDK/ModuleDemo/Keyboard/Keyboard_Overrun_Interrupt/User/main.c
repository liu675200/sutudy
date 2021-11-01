#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

 
void NVIC_Configuration(void);
void UART_Configuration(void);
void KCU_Configuration(void);
void GPIO_Configuration(void);

  
int main(void)
{
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_KBD | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_KBD, ENABLE);

	UART_Configuration();
	UART_SendData(UART0, (uint8_t)0xA1);
	KCU_Configuration();
	UART_SendData(UART0, (uint8_t)0xA2);
	GPIO_Configuration();
	UART_SendData(UART0, (uint8_t)0xA3);
	NVIC_Configuration();
	UART_SendData(UART0, (uint8_t)0xA4);

	//printf("MegaHunt SCPU KCU Demo V1.0.\r\n");
	UART_SendData(UART0, (uint8_t)(0xA5));
	while(1)
	{
	}
	
}

void GPIO_Configuration(void)
{
    GPIO_PinRemapConfig(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_2);	
	GPIO_PullUpCmd(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, ENABLE);
	
    GPIO_PinRemapConfig(GPIOE, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_Remap_0);	
    GPIO_PullUpCmd(GPIOE, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, ENABLE);
}

void KCU_Configuration(void)
{
	KCU_InitTypeDef KCU_InitStruct;
	
	KCU_StructInit(&KCU_InitStruct);
    KCU_InitStruct.KCU_DebounceTimeLevel = KCU_DEBOUNCETIMELEVEL_0;
	KCU_InitStruct.KCU_PortOutput = KCU_PORT_0 | KCU_PORT_1 | KCU_PORT_2 | KCU_PORT_3; 
	KCU_InitStruct.KCU_PortInput = KCU_PORT_4 | KCU_PORT_5 | KCU_PORT_6; 
	KCU_Init(&KCU_InitStruct);
	
	KCU_ITConfig(KCU_IT_PUSH | KCU_IT_RELEASE, ENABLE);
	KCU_Cmd(ENABLE);
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = KBD_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
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


int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
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
