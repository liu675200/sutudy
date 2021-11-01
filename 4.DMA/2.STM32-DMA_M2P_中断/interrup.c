#include "inc.h"
extern uint8_t aDST_Buffer[BUFFER_SIZE];     
extern const uint32_t aSRC_Const_Buffer[BUFFER_SIZE];
extern void Key_Init(void);
extern void USART_SendData(uint16_t Data);
extern uint32_t kfifo_put(kfifo_t *fifo, const uint8_t *buffer, uint32_t len);//输入
extern uint32_t kfifo_get(kfifo_t *fifo, uint8_t *buffer, uint32_t len);//输出

uint8_t usart1Buf[1 << 8];
kfifo_t usart1Fifo ={	.buffer = usart1Buf,
                        .size = sizeof(usart1Buf),
                        .in = 0,
                        .out = 0};


void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct)
{
  uint32_t tmppriority = 0x00, tmppre = 0x00, tmpsub = 0x0F;
    
    tmppriority = (0x700 - ((SCB->AIRCR) & (uint32_t)0x700))>> 0x08;
    tmppre = (0x4 - tmppriority);
    tmpsub = tmpsub >> tmppriority;

    tmppriority = (uint32_t)NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << tmppre;
    tmppriority |=  NVIC_InitStruct->NVIC_IRQChannelSubPriority & tmpsub;
    tmppriority = tmppriority << 0x04;
        
    NVIC->IP[NVIC_InitStruct->NVIC_IRQChannel] = tmppriority;
    
    /* Enable the Selected IRQ Channels --------------------------------------*/
    NVIC->ISER[NVIC_InitStruct->NVIC_IRQChannel >> 0x05] =
      (uint32_t)0x01 << (NVIC_InitStruct->NVIC_IRQChannel & (uint8_t)0x1F);
}

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  SCB->AIRCR = ((uint32_t)0x05FA0000) | NVIC_PriorityGroup;
}

void NVIC_Configuration(IRQn_Type IRQ,int Preemption,int Sub)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ç¶Ì×ÏòÁ¿ÖÐ¶Ï¿ØÖÆÆ÷×éÑ¡Ôñ */
  NVIC_PriorityGroupConfig((uint32_t)0x500);
  
  /* ÅäÖÃUSARTÎªÖÐ¶ÏÔ´ */
  NVIC_InitStructure.NVIC_IRQChannel = IRQ;
  /* ÇÀ¶ÏÓÅÏÈ¼¶*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Preemption;
  /* ×ÓÓÅÏÈ¼¶ */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = Sub;
  /* ³õÊ¼»¯ÅäÖÃNVIC */
  NVIC_Init(&NVIC_InitStructure);
}

void EXTI0_KEY_Init(void)
{
	Key_Init();//GPIO 配置
	//打开AFIO时钟
	RCC_APB2ENR |= (1<<0);
	NVIC_Configuration(EXTI0_IRQn,0,1);

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


//----------------------------------Handle------------------------------------------

void DMA1_Channel4_IRQHandler()//DMA1 4接收中断
{
	USART_SendData(0x55);
	if(DMA1->ISR & (1<<12))//是否产生中断
	{
		if(DMA1->ISR & (1<<13))
		{
			USART_SendData(0x66);
			DMA1->IFCR |= (1<<13);//清除中断标志位
		}
		DMA1->IFCR |= (1<<12);//清除中断标志位
	}
}

void DMA1_Channel6_IRQHandler()//DMA1 6接收完成中断
{
	USART_SendData(0x33);
	if(DMA1->ISR & (1<<20))//是否产生中断
	{
		if(DMA1->ISR & (1<<21))
		{
			LED_Green_ON;
			USART_SendData(0x44);
			DMA1->IFCR |= (1<<21);//清除中断标志位
		}
		DMA1->IFCR |= (1<<20);//清除中断标志位
	}
}





void EXTI0_IRQHandler(void)//按键中断
{
	LED_OFF;
	LED_Green_ON;
	if(EXTI_GetITStatus((uint32_t)0x00001) != 0) 
	{
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


