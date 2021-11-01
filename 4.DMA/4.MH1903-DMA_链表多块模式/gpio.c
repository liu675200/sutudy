#include "inc.h"


void Time_config(uint32_t ms)
{
    TIM0 -> ControlReg = 0x06;//关闭定时器中断，并使用用户定义模式
    TIM0 -> LoadCount = ((SYSCTRL->PCLK_1MS_VAL) * ms + 100);//定时三秒，载入计数器初值
    TIM0 -> ControlReg |= 0x01;//定时器使能
    while(TIM0 ->CurrentValue>100);
}

void GPIO_Config(void)
{
    uint32_t i;
    for (i = 0; i < 16; i++)//GPIOB ALL
    {
        if (((uint16_t)0xffff) & (0x01U << i))
        {
            GPIO->ALT[1] &= ~(0x03U<<(i*2));
            GPIO->ALT[1] |= (0x01U<<(i*2));
        }
    }
}

void gpio_output_enable(unsigned int num)//使能输出
{
    GPIOA->OEN &= (~(1 << 2));
    GPIOA->OEN &= (~(1 << 3));
    GPIOB->OEN &= (~(1 << num));
}

void gpio_clear(unsigned int num)//低电平
{
    GPIOB->BSRR |= 1 << ( num + 16 );
    GPIOA->BSRR |= 1 << ( 2 + 16 );
    GPIOA->BSRR |= 1 << ( 3 + 16 );
}

void gpio_set(unsigned int num)//高电平
{
    GPIOB->BSRR |= 1 << num;
    GPIOA->BSRR |= 1 << 2;
    GPIOA->BSRR |= 1 << 3;
}

void ringBufferInit(RingBufferTypeDef * buf)
{
    uint32_t len = sizeof(buf->buffer);
    for(;len>0;len--)
    {
        buf->buffer[sizeof(buf->buffer)-len] = 0;
    }
	buf->get_index = 0;
	buf->put_index = 0;
	buf->count = 0;
}

void ringBufferPut(RingBufferTypeDef * buf, uint8_t ch)	//ÊäÈë
{
	buf->buffer[buf->put_index] = ch;
	buf->put_index = (buf->put_index + 1) & (RING_BUFFER_SIZE - 1);
	buf->count += 1;
	if(buf->put_index == buf->get_index)
	{
		buf->get_index = (buf->get_index + 1) & (RING_BUFFER_SIZE - 1);
	}
}

uint8_t ringBufferGet(RingBufferTypeDef * buf)		//Êä³ö
{
	uint8_t ch;
	ch = -1;
	if(buf->get_index != buf->put_index)
	{
		ch = buf->buffer[buf->get_index];
		buf->count -= 1;
		buf->get_index = (buf->get_index + 1) & (RING_BUFFER_SIZE - 1);
	}
	return ch;
}