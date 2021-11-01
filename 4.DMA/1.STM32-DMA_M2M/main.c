#include "inc.h"

extern void Usart_Init();
extern void USART_SendData(uint16_t Data);
extern void xmodem();
extern void Led_Init(void);
extern void EXTI0_KEY_Init(void);
extern void DMA_Config(void);
extern uint32_t aDST_Buffer[BUFFER_SIZE];     
extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];
extern uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
extern int DMA_GetFlagStatus(uint32_t DMAy_FLAG);



int main(void)
{
	Led_Init();//LEDGPIO初始化
	uint8_t TransferStatus;
	//EXTI0_KEY_Init();
	Usart_Init();//串口初始化,关中断，开流控使能
	EXTI0_KEY_Init();
	DMA_Config();

	while (DMA_GetFlagStatus((uint32_t)0x00200000))
	{

	}

	TransferStatus=Buffercmp(aSRC_Const_Buffer, aDST_Buffer, BUFFER_SIZE);
	if(TransferStatus==0)  
	{
		LED_Red_ON;
	}
	else
	{ 
		LED_Blue_ON;
	}

	return 0;
}















	
