#include "inc.h"

extern void Usart_Init();
extern void USART_SendData(uint8_t Data);
extern void xmodem();
extern void Led_Init(void);
extern void EXTI0_KEY_Init(void);
extern void DMA_Config_M2P(uint32_t ADD);
extern uint8_t aDST_Buffer[BUFFER_SIZE];
extern uint8_t aDST_Buffer2[BUFFER_SIZE];
extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];
extern uint32_t kfifo_dam_put(kfifo_t* fifo, uint8_t* buffer, uint32_t len);
extern uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
extern int DMA_GetFlagStatus(uint32_t DMAy_FLAG);

extern uint32_t kfifo_put(kfifo_t *fifo,uint8_t *buffer, uint32_t len);
extern uint32_t kfifo_get(kfifo_t *fifo, uint8_t *buffer, uint32_t len);
extern uint32_t kfifo_count(kfifo_t *fifo);
uint8_t usart1Buf[1 << 8];
kfifo_t usart1Fifo ={	.buffer = usart1Buf,
                        .size = sizeof(usart1Buf),
                        .in = 0,
                        .out = 0};

int main(void)
{
	Led_Init();//LEDGPIO初始化
	Usart_Init();//串口初始化,关中断
	USART_SendData(0x11);
	EXTI0_KEY_Init();
	uint8_t buffer;

	while (1)
	{
		if((USART1_SR & (1<<5)) != 0)	//读数据非空
		{
			buffer = (uint8_t)(USART1_DR & 0xFF);
			USART_SendData(0xA1);
			//kfifo_put(&usart1Fifo,&buffer,sizeof(buffer));
			kfifo_dam_put(&usart1Fifo,&buffer,sizeof(buffer));
		}
	}
	return 0;
}


/*
static void Delay(__IO uint32_t nCount)
{
	for(; nCount != 0; nCount--);
}
*/

void DMA1_Channel4_IRQHandler()
{
	DMA1_Channel4->CCR &= ~((uint16_t)0x0001);//关闭线4
	USART1_CR3 &= ~((uint16_t)0x0080);
	USART_SendData(0x55);
	if(DMA1->ISR & (1<<12))//是否产生中断
	{
		if(DMA1->ISR & (1<<13))
		{
			//*add1 = (uint32_t)(sourth2);
			DMA_Config_M2P((uint32_t)aDST_Buffer2);
			USART_SendData(0x66);
			DMA1->IFCR |= (1<<13);//清除中断标志位
		}
		DMA1->IFCR |= (1<<12);//清除中断标志位
	}
	USART1_CR3 |= ((uint16_t)0x0080);
}







	
