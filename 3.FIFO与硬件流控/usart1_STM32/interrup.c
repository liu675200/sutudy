#include "inc.h"
extern void USART_SendData(uint16_t Data);
extern uint32_t kfifo_put(kfifo_t *fifo, const uint8_t *buffer, uint32_t len);//输入
extern uint32_t kfifo_get(kfifo_t *fifo, uint8_t *buffer, uint32_t len);//输出

uint8_t usart1Buf[1 << 8];
uint32_t flag = 0;
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
	if(EXTI_GetITStatus((uint32_t)0x00001) != 0) 
	{
		if(flag == 0)
			{
				LED_OFF;
				LED_Red_ON;
				GPIOA_ODR |= (1<<12);//拉高RTS
				flag = 1;
			}
		else
			{
				LED_OFF;
				LED_Green_ON;
				GPIOA_ODR &= ~(1<<12);//拉低电平
				flag = 0;
			}
		EXTI->PR = (uint32_t)0x00001;
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


