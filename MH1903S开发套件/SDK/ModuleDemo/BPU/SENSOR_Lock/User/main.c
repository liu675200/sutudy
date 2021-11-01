#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


void UART_Configuration(void);
void SENSOR_Configuration(void);
void SENSOR_EnableStatePrintf(void);


int main(void)
{
	uint32_t i;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
    SENSOR_ClearITPendingBit();
	
    printf("MegaHunt SCPU SENSOR Lock Demo V1.0.\n");
	printf("SENSOR->SEN_LOCK = %x\n",SENSOR->SEN_LOCK);
	SENSOR_EnableStatePrintf();
	
	printf("Lock All Sensor\n");
	SENSOR_Lock(SENSOR_LOCK_EXTS |
				SENSOR_LOCK_VOL_HIGH |
				SENSOR_LOCK_VOL_LOW |
				SENSOR_LOCK_TEMPER_HIGH |
				SENSOR_LOCK_TEMPER_LOW |
				SENSOR_LOCK_VOLGLITCH |
				SENSOR_LOCK_XTAL32K |
				SENSOR_LOCK_MESH);
	printf("SENSOR->SEN_LOCK = %x\n",SENSOR->SEN_LOCK);

	for (i = 0; i < EXT_SENSOR_NUM; i++)
	{
		SENSOR->SEN_EN[i] = 0x55;
	}
	SENSOR_EnableStatePrintf();
 
	printf("\n\n*****************************************************\n");
	printf("After Lock SEN_EXTS_LOCK, you need repower the battery or use BPK_RR to reset SEN_EXTS_LOCK reg!!!");
	printf("\n*****************************************************\n");
    
	while(1)
	{
	}
}

void SENSOR_EnableStatePrintf(void)
{
	uint32_t i;	
	const char *SENSOR_NAME[] = 
	{
		"SENSOR_EN_EXT0", 
		"SENSOR_EN_EXT1",
		"SENSOR_EN_EXT2",
		"SENSOR_EN_EXT3",
		"SENSOR_EN_EXT4",
		"SENSOR_EN_EXT5",
		"SENSOR_EN_EXT6",
		"SENSOR_EN_EXT7",
		"SENSOR_RSVD0",
		"SENSOR_RSVD1",
		"SENSOR_RSVD2",
		"SENSOR_RSVD3",		
		"SENSOR_EN_VH",
		"SENSOR_EN_VL",
		"SENSOR_EN_TH",
		"SENSOR_EN_TL",
		"SENSOR_EN_XTAL32",
		"SENSOR_EN_MESH",
		"SENSOR_EN_VOLGLITCH"
	};

	for (i = 0; i < sizeof(SENSOR_NAME)/sizeof(SENSOR_NAME[0]); i++)
	{
		printf("	%s value %08X\n", SENSOR_NAME[i], SENSOR->SEN_EN[i]);
	}
	printf("	SENSOR_EN_EXTS value %08X\n", SENSOR->SEN_EXTS_START);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 |GPIO_Pin_1, GPIO_Remap_0);	
	
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
