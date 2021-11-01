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

#define TEST_OTP_ADDR   0x40008B00


int main(void)
{
	uint32_t ErrorFlag = 0, tmpCmd, i;
	
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
    
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
	
	UART_Configuration();
	printf("MegaHunt SCPU OTP Demo V1.0.1\n");

    /* ��ȡOTPԭ���� */
    printf("Addr 0x%08X data is 0x%X.\n", (TEST_OTP_ADDR), *(uint32_t *)TEST_OTP_ADDR);
    
    
    printf("Enter 'w' start writing eFuse.\n");
    while (!(UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD));
    tmpCmd = UART_ReceiveData(UART0);
    
    if ('w' == tmpCmd)
    {
        OTP_PowerOn();
        
        OTP_Unlock();
        OTP_UnProtect(TEST_OTP_ADDR);
        
        printf("Start write.\n");
        if (0 != (ErrorFlag = OTP_WriteWord(TEST_OTP_ADDR, 0x12345678)))
        {
            printf("Write OTP error,flag is %d.\n",ErrorFlag);
            
            while(1);
        }
        OTP_Lock();
        OTP_SetProtect(TEST_OTP_ADDR);
        
        printf("Write OTP done.\n");
    }
    printf("Addr 0x%08X data is 0x%X.\n",TEST_OTP_ADDR, *(uint32_t *)TEST_OTP_ADDR);
    
    while(1);
}


void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 , GPIO_Remap_0);
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);
}


//Retarget Printf
int SER_PutChar(int ch)
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
