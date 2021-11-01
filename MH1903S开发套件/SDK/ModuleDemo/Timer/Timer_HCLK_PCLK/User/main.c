#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

void NVIC_Configuration(void);
void UART_Configuration(void);
void TIMER_Configuration(void);

int32_t TimerFlag = 0;

int main(void)
{	
	uint32_t i,j,k,n;
    
	uint32_t sysclk[] = {SYSCTRL_PLL_168MHz,SYSCTRL_PLL_156MHz,SYSCTRL_PLL_144MHz,\
                         SYSCTRL_PLL_132MHz,SYSCTRL_PLL_120MHz,SYSCTRL_PLL_108MHz};
	
    uint32_t plldiv[] = {SYSCTRL_PLL_Div_None,SYSCTRL_PLL_Div2,SYSCTRL_PLL_Div4};                     
    uint32_t hclk[] = {SYSCTRL_HCLK_Div_None};
	uint32_t pclk[] = {SYSCTRL_PCLK_Div2};
	SYSCTRL_ClocksTypeDef sclock;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
	
    UART_Configuration();
	TIMER_Configuration();
	NVIC_Configuration();

	printf("MegaHunt SCPU Timer Demo V1.0.\n");
	printf("UART0 Baudrate is 9600.\n");
	
	for (i = 0; i < sizeof(sysclk)/sizeof(sysclk[0]) ; i++)
	{
		SYSCTRL_PLLConfig((SYSCTRL_PLL_TypeDef)sysclk[i]);
        for(n = 0; n < sizeof(plldiv)/sizeof(plldiv[0]); n++)
        {
            SYSCTRL_PLLDivConfig(plldiv[n]);
            for (j = 0; j < sizeof(hclk)/sizeof(hclk[0]) ; j++)
            {
                SYSCTRL_HCLKConfig(hclk[j]);
                for (k = 0; k < sizeof(pclk)/sizeof(pclk[0]); k++)
                {
                    SYSCTRL_PCLKConfig(pclk[k]);
					
                    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0,ENABLE);
                    UART_Configuration();
                    SYSCTRL_GetClocksFreq(&sclock);
                    printf("\n");
                    printf("pllclk is %d\n", sclock.PLL_Frequency);
                    printf("cpuclk is %d\n", sclock.CPU_Frequency);
                    printf("hclk is %d\n", sclock.HCLK_Frequency);
                    printf("pclk is %d\n", sclock.PCLK_Frequency);
                    TIM_Cmd(TIMM0, TIM_0, ENABLE);
                    while(!TimerFlag);
                    TimerFlag = 0;
                }
            }
        }
	}
	
	printf("Test End\n");
	
	while(1)
	{
        
	}	
}

void TIMER_Configuration(void)
{
	TIM_InitTypeDef TIM_InitStruct;
	TIM_InitStruct.TIM_Period = (SYSCTRL->PCLK_1MS_VAL * 1000);
	
	TIM_InitStruct.TIMx = TIM_0;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_1;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_2;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_3;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_4;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_5;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
	
	TIM_InitStruct.TIMx = TIM_6;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);

	TIM_InitStruct.TIMx = TIM_7;
	TIM_Init(TIMM0, &TIM_InitStruct);
	TIM_ITConfig(TIMM0, TIM_InitStruct.TIMx, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_1_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_2_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_3_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_4_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_5_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_6_IRQn;
 	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM0_7_IRQn;
 	NVIC_Init(&NVIC_InitStructure);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 9600;
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
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
	/* Infinite loop */
	while (1)
	{
	}
}
#endif
