#include "inc.h"
extern void xmodem();
extern void Flash_Erase();

void delay_s(uint16_t s)//秒级延时
{
    RCC_APB1ENR |= (1<<4);//TIM6时钟初始化
	//设置定时器预分频系数,TIM6时钟为90MHz,分频后时钟为2KHz即500us,由于PSC为16位寄存器,所以无法分频至1KHz
    //不同CPU的时钟可能不一样
	TIM6_PSC = (41000-1);
	//设置自动重装载值,定时器计数器的值自增到ARR时,会产生更新事件,ARR的值就是需要延时的时间的一半
	TIM6_ARR = (s*1000*2);
	
	//重新初始化定时器计数器并生成寄存器更新事件,确保预分频值被采用，此时定时器将采用刚刚写入的预分频值，如果此处不更新，那么定时器需要等待下次更新事件的到来才会重新加载预分频值
	TIM6_EGR |= (1<<0);
	//清除更新标志位,该位在发生更新事件时通过硬件置 1,但需要通过软件清零
	TIM6_SR = 0;
	
	//CR1的bit3(OPM)置一,计数器在发生下一更新事件时停止计数,单脉冲模式
	TIM6_CR1 |= (1<<3);
	//CR1的bit0(CEN)置一,启动定时器开始计数
	TIM6_CR1 |= (1<<0);
	//等待更新事件到来,计数器的值自增到自动重装载寄存器的时候,会产生更新事件,此时延时时间已到
	while((TIM6_SR & 0x01)==0);
	//清除更新标志位,该位在发生更新事件时通过硬件置 1,但需要通过软件清零
	TIM6_SR &= ~(1<<0);
}

//ms级延时函数
//最大延时时间 65535/2 = 32767.5ms
void delay_ms(uint16_t ms)
{
    RCC_APB1ENR |= (1<<4);//TIM6时钟初始化
	//设置定时器预分频系数,TIM6时钟为90MHz,分频后时钟为2KHz即500us,由于PSC为16位寄存器,所以无法分频至1KHz
    //不同CPU的时钟可能不一样
	TIM6_PSC = (41000-1);
	//设置自动重装载值,定时器计数器的值自增到ARR时,会产生更新事件,ARR的值就是需要延时的时间的一半
	TIM6_ARR = (ms*2);
	
	//重新初始化定时器计数器并生成寄存器更新事件,确保预分频值被采用，此时定时器将采用刚刚写入的预分频值，如果此处不更新，那么定时器需要等待下次更新事件的到来才会重新加载预分频值
	TIM6_EGR |= (1<<0);
	//清除更新标志位,该位在发生更新事件时通过硬件置 1,但需要通过软件清零
	TIM6_SR = 0;
	
	//CR1的bit3(OPM)置一,计数器在发生下一更新事件时停止计数,单脉冲模式
	TIM6_CR1 |= (1<<3);
	//CR1的bit0(CEN)置一,启动定时器开始计数
	TIM6_CR1 |= (1<<0);
	//等待更新事件到来,计数器的值自增到自动重装载寄存器的时候,会产生更新事件,此时延时时间已到
	while((TIM6_SR & 0x01)==0);
	//清除更新标志位,该位在发生更新事件时通过硬件置 1,但需要通过软件清零
	TIM6_SR &= ~(1<<0);
}

void TIM6_ms_ON(uint16_t ms)//打开延时
{
    RCC_APB1ENR |= (1<<4);//TIM6时钟初始化
	TIM6_PSC = (41000-1);
	TIM6_ARR = (ms*2);
	TIM6_EGR |= (1<<0);
	TIM6_SR = 0;
	TIM6_CR1 |= (1<<3);
	TIM6_CR1 |= (1<<0);
}
int TIM6_ms_OFF()//延时未到返回0，延时到了返回1
{
	if((TIM6_SR & 0x01)==0)
		return 0;
	else
	{
		TIM6_SR &= ~(1<<0);
		return 1;
	}
}

void Led_Init()
{
	// 开启GPIOB 端口时钟
	RCC_APB2ENR |= (1<<3);
	//清空控制PB0的端口位
	GPIOB_CRL &= ~( 0x0F<< (4*0));	
	// 配置PB0为通用推挽输出，速度为10M
	GPIOB_CRL |= (1<<4*0);

	//清空控制PB1的端口位
	GPIOB_CRL &= ~( 0x0F<< (4*1));	
	// 配置PB1为通用推挽输出，速度为10M
	GPIOB_CRL |= (1<<4*1);

	//清空控制PB5的端口位
	GPIOB_CRL &= ~( 0x0F<< (4*5));
	// 配置PB1为通用推挽输出，速度为10M
	GPIOB_CRL |= (1<<4*5);
	LED_OFF;
}

void Key_Init()//PA0  浮空输入,
{
	//打开APB2上GPIOA的时钟
	RCC_APB2ENR |= (1<<2);
	//清空控制PA0位
	GPIOA_CRL &= ~(0x0F<<(4*0));
	//配置浮空输入0100
	GPIOA_CRL |= (1<<2);
}

int Key_check()
{
	if((GPIOA_IDR & (1<<0)) != 0)
	{
		while ((GPIOA_IDR & (1<<0)) != 0);
		return 1;
	}
	else
	{
		return 0;
	}
}







void EXTI0_KEY_Init(void)
{
	Key_Init();//GPIO 配置

	//打开AFIO时钟
	RCC_APB2ENR |= (1<<0);

	//NVIC 中断配置
	//设置中断优先级组划分
	SCB->AIRCR = ((uint32_t)0x05FA0000) | ((uint32_t)0x500);
	uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
	tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

	//配置抢占优先级1
    tmppriority = (uint32_t)(0) << tmppre;

	//配置子优先级1
    tmppriority |=  ((1) & tmpsub);

    tmppriority = tmppriority << 0x04;
    NVIC->IP[6] = tmppriority;//配置优先级
    //Enable the Selected IRQ Channels -------------------------------------
    NVIC->ISER[6 >> 0x05] =				//使能中断
    (uint32_t)0x01 << (6 & (uint8_t)0x1F);
	
	//选择exti的输入信号源
	uint32_t tmp = 0x00;
	tmp = ((uint32_t)0x0F) << (0x04 * (((uint8_t)0x00) & (uint8_t)0x03));
	AFIO->EXTICR[((uint8_t)0x00) >> 0x02] &= ~tmp;
	AFIO->EXTICR[((uint8_t)0x00) >> 0x02] |= (((uint32_t)((uint8_t)0x00)) << (0x04 * (((uint8_t)0x00) & (uint8_t)0x03)));

	//输入线，中断模式，上升沿模式，使能中断

	tmp = (uint32_t)EXTI_BASE;
	//Clear EXTI line configuration 
    EXTI->IMR &= ~((uint32_t)0x00001);		//清除中断请求
    EXTI->EMR &= ~((uint32_t)0x00001);		//清除事件请求
    tmp += 0x00;
    *(__IO uint32_t *) tmp |= ((uint32_t)0x00001);	//使能中断请求
    // Clear Rising Falling edge configuration 
    EXTI->RTSR &= ~((uint32_t)0x00001);		//清除上升沿触发
    EXTI->FTSR &= ~((uint32_t)0x00001);		//清除下降沿触发
    // Select the trigger for the selected external interrupts 
	tmp = (uint32_t)EXTI_BASE;	
	tmp += 0x08;

	*(__IO uint32_t *) tmp |= ((uint32_t)0x00001);		//使能上升沿触发

}


void MCO_Init()	//由于GPIO口最大输出频率为50MHz，测得系统输出最大是25M？
{
	//设置GPIOA 8  MCO 系统时钟
    RCC_APB2ENR |= (1<<2);
    //清空控制PA8的端口位
    GPIOA_CRH &= ~( 0x0F<< (4*0));	
    //配置PA8为复用推挽输出，速度为50M 1011
    GPIOA_CRH |= (0x0B<<4*0);
	RCC_CFGR |= (0x07000000);
}


void RTS_Init(void)//配置流控的引脚
{
	//GPIOA 时钟打开
	//配置输入输出12输出，11输入或者不配置
	RCC_APB2ENR |= (1<<2);
	//清空控制PA 12的端口位
	GPIOA_CRH &= ~( 0x0F<< (4*4));	
	// 配置PB0为通用推挽输出，速度为10M
	GPIOA_CRH |= (1<<4*4);

	//清空控制PA 11的端口位
	GPIOA_CRH &= ~( 0x0F<< (4*3));	
	// 配置PA12为通用浮空输入0100
	GPIOA_CRH |= (4<<4*3);

	GPIOA_ODR &= (1<<12);
}


void Jump()
{
	//(*(void(*) ())(*((uint32_t*)(WRITE_START_ADDR + 0x4))))();
	((void(*)(void))(*((uint32_t*)(WRITE_START_ADDR + 0x4))))();
}


void boot(void)
{
	TIM6_ms_ON(5000);
	while (!TIM6_ms_OFF())
	{
		if(Key_check())
			{
				Flash_Erase();
				LED_Red_ON;
				xmodem();
				LED_Red_OFF;
			}
	}
	LED_Blue_ON;
	Jump();
}
