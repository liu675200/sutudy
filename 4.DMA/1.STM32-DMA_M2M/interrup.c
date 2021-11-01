#include "inc.h"

extern uint32_t aDST_Buffer[BUFFER_SIZE];     
extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];

extern void USART_SendData(uint16_t Data);
extern uint32_t kfifo_put(kfifo_t *fifo, const uint8_t *buffer, uint32_t len);//输入
extern uint32_t kfifo_get(kfifo_t *fifo, uint8_t *buffer, uint32_t len);//输出

uint8_t usart1Buf[1 << 8];
kfifo_t usart1Fifo ={	.buffer = usart1Buf,
                        .size = sizeof(usart1Buf),
                        .in = 0,
                        .out = 0};

int EXTI_GetITStatus(uint32_t EXTI_Line)
{
	int bitstatus = 0;
	uint32_t enablestatus = 0;

	enablestatus =  EXTI->IMR & EXTI_Line;
	if (((EXTI->PR & EXTI_Line) != (uint32_t)(0)) && (enablestatus != (uint32_t)(0)))
		{
		bitstatus = 1;
		}
	else
		{
		bitstatus = 0;
		}
	return bitstatus;
}


void EXTI0_IRQHandler(void)
{
	LED_OFF;
	LED_Green_ON;
	if(EXTI_GetITStatus((uint32_t)0x00001) != 0) 
	{
		USART_SendData(0xA5);
		for(uint32_t i = 0;i<BUFFER_SIZE;i++)
			{
				USART_SendData(aSRC_Const_Buffer[i]);	//只打印最后两个uint8_t
			}
		USART_SendData(0xA6);
		for(uint32_t k = 0;k<BUFFER_SIZE;k++)
			{
				USART_SendData(aDST_Buffer[k]);
			}
		EXTI->PR = (uint32_t)0x00001;
		USART_SendData(0xA7);
	} 
}


void USART1_IRQHandler()
{
    uint8_t buffer;
    if((USART1_SR & (1<<5)) != 0)//接收中断
    {
        buffer = (uint8_t)(USART1_DR & (uint8_t)0xFF);//收到信号
        kfifo_put(&usart1Fifo,&buffer,sizeof(buffer));
    }
    LED_Blue_ON;
}


