#include <stdio.h>
#include <errno.h>
#include "mhscpu.h"
#include "bt42_api.h"
#include "bt42_uart.h"


volatile uint32_t sys_tick = 0; // 1 ms per count

//给蓝牙提供一个ms的计数器
uint32_t GetTimerCount()
{
	return sys_tick * 10;
}

void TIM0_0_IRQHandler(void)
{
    sys_tick++;
    TIM_ClearITPendingBit(TIMM0, TIM_0);
}

//初始化一个10ms的timer定时器
void timer_configuration(void)//systick init
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_InitTypeDef TIM_InitStruct;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_InitStruct.TIM_Period = (SYSCTRL->PCLK_1MS_VAL) * 10;  // 10 ms
	TIM_InitStruct.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStruct);
    
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	TIM_Cmd(TIMM0, TIM_InitStruct.TIMx, ENABLE);
}

void debug_uart_init(void)
{
	UART_InitTypeDef UART_InitStructure;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);
}

void mcu_hardware_init(void)
{
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2); 

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO | SYSCTRL_APBPeriph_TIMM0,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_TIMM0, ENABLE);

	timer_configuration();//给蓝牙提供一个ms计数器
	
	debug_uart_init();//调试串口初始化
	
	blue_uart_Configuration();//蓝牙串口初始化
}


int main(void)
{
	//初始化MCU的硬件配置
	mcu_hardware_init();
	
	//蓝牙初始化配置
	bt42_bluetooth_init();
	
	while (1)
	{
		bt42_run_loop(1);
	}

	return 0;
}



//Retarget Printf
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

