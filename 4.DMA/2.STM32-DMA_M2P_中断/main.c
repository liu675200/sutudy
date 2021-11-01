#include "inc.h"

extern void Usart_Init();
extern void USART_SendData(uint16_t Data);
extern void xmodem();
extern void Led_Init(void);
extern void EXTI0_KEY_Init(void);
extern void DMA_Config_M2P(uint32_t ADD);
extern uint8_t aDST_Buffer[BUFFER_SIZE];
extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];
extern uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
extern int DMA_GetFlagStatus(uint32_t DMAy_FLAG);

static void Delay(__IO uint32_t nCount); 


int main(void)
{
	Led_Init();//LEDGPIO初始化
	Usart_Init();//串口初始化,关中断
	EXTI0_KEY_Init();
	DMA_Config_M2P((uint32_t)aDST_Buffer);
	//使能USART1的DMA发送请求,此时开始传输
	USART1_CR3 |= ((uint16_t)0x0080);

	while (1)
	{
		LED_OFF;
		LED_Green_ON;
		Delay(0xFFFFFF);
		LED_OFF;
		LED_Blue_ON;
		Delay(0xFFFFFF);
	}
	return 0;
}



static void Delay(__IO uint32_t nCount)	 //¼òµ¥µÄÑÓÊ±º¯Êý
{
	for(; nCount != 0; nCount--);
}







	
