#include "uart.h"
#include <stdio.h>

//�������´��룬֧��printf������������Ҫѡ��use MicroLIB
#if 1
//ȡ��ARM�İ�����ģʽ
#pragma import(__use_no_semihosting)
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
    x = x;
}

void _ttywrch(int ch)
{
    ch = ch;
}
//��׼����Ҫ��֧�ֺ���
struct __FILE {
    int handle;
};

FILE __stdout;        
#endif

int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  while(!UART_IsTXEmpty(UART0));
  UART_SendData(UART0, (uint8_t) ch);
  return ch;
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
