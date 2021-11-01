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
void CRC_Calculate(void);

  
int main(void)
{
	uint32_t i, j, k, l;
	SYSCTRL_ClocksTypeDef clocks;
	uint32_t sysclk[] = {SYSCTRL_PLL_108MHz, SYSCTRL_PLL_120MHz, SYSCTRL_PLL_132MHz, SYSCTRL_PLL_144MHz, SYSCTRL_PLL_156MHz, SYSCTRL_PLL_168MHz};
	uint32_t pllDiv[] = {SYSCTRL_PLL_Div_None, SYSCTRL_PLL_Div2, SYSCTRL_PLL_Div4};
	uint32_t hclk[] = {SYSCTRL_HCLK_Div_None, SYSCTRL_HCLK_Div2};
	uint32_t pclk[] = {SYSCTRL_PCLK_Div2, SYSCTRL_PCLK_Div4};
	
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_CRC | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_CRC, ENABLE);

	UART_Configuration();
	
	printf("MegaHunt SCPU CRC Demo V1.0.\n");
	
	for (i = 0; i < sizeof(sysclk) / sizeof(sysclk[0]); i++)
	{
		SYSCTRL_PLLConfig((SYSCTRL_PLL_TypeDef)sysclk[i]);
		for (j = 0; j < sizeof(pllDiv) / sizeof(pllDiv[0]); j++)
		{
			SYSCTRL_PLLDivConfig(pllDiv[j]);
			for (k = 0; k < sizeof(hclk) / sizeof(hclk[0]); k++)
			{
				SYSCTRL_HCLKConfig(hclk[k]);
				for (l = 0; l < sizeof(pclk) / sizeof(pclk[0]); l++)
				{
					SYSCTRL_PCLKConfig(pclk[l]);
					
					SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
					UART_Configuration();
					
					SYSCTRL_GetClocksFreq(&clocks);
					printf("\nsysclk is %d\n", clocks.PLL_Frequency);
					printf("cpu freq is %d\n", clocks.CPU_Frequency);
					printf("hclk is %d\n", clocks.HCLK_Frequency);
					printf("pclk is %d\n", clocks.PCLK_Frequency);
					CRC_Calculate();
				}
			}		
		}
	}
	
	printf("Test End\n");

	while(1);
}

void CRC_Calculate(void)
{
	uint8_t str[] = "123456789";
	uint32_t result = 0;
	
	printf("string for CRC caculate : %s \n",str);
	
	result = CRC_CalcBlockCRC(CRC_16,str,sizeof(str)-1);
	printf("CRC_16 :            %08X ",result);
	0x0000BB3D == result ? printf("succeed\n") : printf("failed\n");
	
	result = CRC_CalcBlockCRC(CRC_16_Modbus,str,sizeof(str)-1);
	printf("CRC_16_Modbus :     %08X ",result);
	0x00004B37 == result ? printf("succeed\n") : printf("failed\n");
	
	result = CRC_CalcBlockCRC(CRC_CCITT_0xffff,str,sizeof(str)-1);
	printf("CRC_CCITT_0xffff :  %08X ",result);
	0x000029B1 == result ? printf("succeed\n") : printf("failed\n");
	
	result = CRC_CalcBlockCRC(CRC_CCITT_XModem,str,sizeof(str)-1);
	printf("CRC_CCITT_XModem :  %08X ",result);
	0x000031C3 == result ? printf("succeed\n") : printf("failed\n");
	
	result = CRC_CalcBlockCRC(CRC_32,str,sizeof(str)-1);
	printf("CRC_32 :            %08X ",result);
	0xCBF43926 == result ? printf("succeed\n") : printf("failed\n");
	
	printf("\n");
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

