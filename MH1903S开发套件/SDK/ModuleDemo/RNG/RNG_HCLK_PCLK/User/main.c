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

void UART_Configuration(void);

uint32_t buf[4];

int main(void)
{
	uint32_t i, j, k, n;
	SYSCTRL_ClocksTypeDef sclock;
	
	uint32_t sysclk[] = {SYSCTRL_PLL_168MHz, SYSCTRL_PLL_156MHz, SYSCTRL_PLL_144MHz, \
                         SYSCTRL_PLL_132MHz, SYSCTRL_PLL_120MHz, SYSCTRL_PLL_108MHz};
	
    uint32_t plldiv[] = {SYSCTRL_PLL_Div_None, SYSCTRL_PLL_Div2, SYSCTRL_PLL_Div4};                     
    uint32_t hclk[] = {SYSCTRL_HCLK_Div_None, SYSCTRL_HCLK_Div2};
	uint32_t pclk[] = {SYSCTRL_PCLK_Div2, SYSCTRL_PCLK_Div4};

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TRNG | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TRNG, ENABLE);

	UART_Configuration();
	
	printf("MegaHunt SCPU TRNG Demo V1.0.\r\n");
	printf("UART0 Baudrate is 9600\r\n");
	
	for (i = 0; i < sizeof(sysclk)/sizeof(sysclk[0]); i++)
	{
		SYSCTRL_PLLConfig((SYSCTRL_PLL_TypeDef)sysclk[i]);
        for(j = 0; j < sizeof(plldiv)/sizeof(plldiv[0]); j++)
        {
            SYSCTRL_PLLDivConfig(plldiv[j]);
            for (k = 0; k < sizeof(hclk)/sizeof(hclk[0]); k++)
            {
                SYSCTRL_HCLKConfig(hclk[k]);
                for (n = 0; n < sizeof(pclk)/sizeof(pclk[0]); n++)
                {
                    SYSCTRL_PCLKConfig(pclk[n]);
					
                    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
                    UART_Configuration();
                    SYSCTRL_GetClocksFreq(&sclock);
                    printf("\nsysclk is %d\n", sclock.PLL_Frequency);
					printf("cpuclk is %d\n", sclock.CPU_Frequency);
                    printf("hclk is %d\n", sclock.HCLK_Frequency);
                    printf("pclk is %d\n", sclock.PCLK_Frequency);
                    TRNG_Start(TRNG0);
                    while(0 != TRNG_Get(buf, TRNG0));
                    printf("Rng : %08X %08X %08X %08X\n\n",buf[0],buf[1],buf[2],buf[3]);
                }
            }
        }
	}
	
	printf("Test End\n");
	while(1)
	{
	}
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
