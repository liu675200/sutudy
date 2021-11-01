#include "inc.h"
extern int UART_IsRXFIFONotEmpty(UART_TypeDef* UARTx);
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);
extern void ringBufferPut(RingBufferTypeDef * buf, uint8_t ch);
extern void Time_config(uint32_t ms);
extern uint32_t dst_Buf[128];
__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  NVIC->ICPR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F)); /* Clear pending interrupt */
}

__STATIC_INLINE uint32_t NVIC_GetPriorityGrouping(void)//获取优先级分组设置数值
{
    return ((SCB->AIRCR & (7UL << 8)) >> 8);
}

__STATIC_INLINE uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)//基于优先级分组，抢占优先级，子优先级生成优先级数值
{
    uint32_t PriorityGroupTmp = (PriorityGroup & 0x07);          
    uint32_t PreemptPriorityBits;
    uint32_t SubPriorityBits;

    PreemptPriorityBits = ((7 - PriorityGroupTmp) > __NVIC_PRIO_BITS) ? __NVIC_PRIO_BITS : 7 - PriorityGroupTmp;
    SubPriorityBits     = ((PriorityGroupTmp + __NVIC_PRIO_BITS) < 7) ? 0 : PriorityGroupTmp - 7 + __NVIC_PRIO_BITS;

    return (
                ((PreemptPriority & ((1 << (PreemptPriorityBits)) - 1)) << SubPriorityBits) |
                ((SubPriority     & ((1 << (SubPriorityBits    )) - 1)))
           );
}

__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)//设置定时器timer1中断优先级
{
    if(IRQn < 0) 
    {
        SCB->SHP[((uint32_t)(IRQn) & 0xF) - 4] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xFF); 
    }
    else 
    {
        NVIC->IP[(uint32_t)(IRQn)] = ((priority << (8 - __NVIC_PRIO_BITS)) & 0xFF);    
    } 
}

__STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)//NVIC中断使能函数
{
    NVIC->ISER[(uint32_t)((int32_t)IRQn) >> 5] = (uint32_t)(1 << ((uint32_t)((int32_t)IRQn) & (uint32_t)0x1F));
}

__STATIC_INLINE void NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07);               /* only values 0..7 are used          */

  reg_value  =  SCB->AIRCR;                                                   /* read old register configuration    */
  reg_value &= ~((0xFFFFUL << 16) | (7UL << 8));             /* clear bits to change               */
  reg_value  =  (reg_value                                 |
                ((uint32_t)(0x5FA << 16)) |
                (PriorityGroupTmp << 8));                                     /* Insert write key and priorty group */
  SCB->AIRCR =  reg_value;
}

void NVIC_Configuration(IRQn_Type IRQn,uint32_t Preemption,uint32_t Sub)//NVIC中断初始化
{
    uint32_t tmpPriority ,tmpPriorityGrouping;
    NVIC_SetPriorityGrouping((uint32_t)0x06);
    tmpPriorityGrouping = NVIC_GetPriorityGrouping();//获取优先级分组设置情况
	tmpPriority = NVIC_EncodePriority(tmpPriorityGrouping,Preemption,Sub);//基于优先级分组，抢占优先级，子优先级
	NVIC_SetPriority((IRQn_Type)IRQn,tmpPriority);
    NVIC_EnableIRQ((IRQn_Type)IRQn);
}




void EXTI_LineConfig(uint32_t EXTI_Line, uint32_t EXTI_PinSource,  uint32_t EXTI_Trigger)
{
	uint32_t i;
	for (i = 0; i < 16; i++)
	{
		if (EXTI_PinSource & BIT(i))
		{
			GPIO->INTP_TYPE_STA[EXTI_Line].INTP_TYPE &= ~(0x03 << (i * 2));
			GPIO->INTP_TYPE_STA[EXTI_Line].INTP_TYPE |= (EXTI_Trigger << (i * 2));
		}
	}
}

//-----------------------------------------------------------------------------------

extern uint32_t dst_Buf0[MULTIBLOCK_BLOCKSIZE];

void DMA0_IRQHandler()
{
    UART_Send(UART0,0x77);

    if(DMA->StatusBlock_L & (1<<channel))
    {
        UART_Send(UART0,0x88);
        for (uint32_t i = 0; i < (MULTIBLOCK_BLOCKSIZE); i++)//打印目的地址数据
        {
            UART_Send(UART1,dst_Buf0[i]);
        }
        DMA->ClearBlock_L = (1<<channel);
    }

    if(DMA->StatusErr_L & (1<<channel))
    {
        UART_Send(UART0,0x99);
        DMA->ClearErr_L = (1<<channel);
    }
    NVIC_ClearPendingIRQ(DMA_IRQn);
}





void EXTI1_IRQHandler(void)
{
    UART0->MCR &= ~(uint32_t)0x0001;//设置RTS,关闭输入
    if(UART0->MCR & (uint32_t)(0x0001))
    {
        UART0->OFFSET_0.THR = 0x11;
    }
    else
    {
        UART0->OFFSET_0.THR = 0x00;
    }
    //flag = 1;
    UART0->OFFSET_0.THR = 0xAB;
    Time_config(5000);//开定时器，
    GPIO->INTP_TYPE_STA[1].INTP_STA;//中断状态
    GPIO->INTP_TYPE_STA[1].INTP_STA = 0xFFFF;//清除中断标志位
	NVIC_ClearPendingIRQ(EXTI1_IRQn);//清除中断
}


void UART0_IRQHandler(void)
{
    //uint8_t ch;
    switch(UART0->OFFSET_8.IIR & 0x0f)
    {
        case ((uint32_t)0x0004):
                    {  
                        UART0->OFFSET_0.THR = 0x46;
                        while(UART_IsRXFIFONotEmpty(UART0))	//FIFO·Ç¿ÕÊ±Êä³öÊý¾Ý
                        {
                            //ch = (uint8_t)(UART0->OFFSET_0.RBR & 0xFF);
                            //ringBufferPut(&ringbuf, ch); //ÊäÈëringbuf
                        }
                    }
        break;
        case ((uint32_t)0x0002)://发送中断
                    {
                        UART0->OFFSET_0.THR = 0x47;
                        UART0->OFFSET_4.IER &= ~((uint32_t)0x0002);//关闭发送中断
                    }
        break;

        case ((uint32_t)0x0000):
                    {
                        UART_Send(UART0,0x48);
                        uint32_t msr = UART0->MSR;
                        UART_Send(UART0,msr);
                    }
		break;
	    case ((uint32_t)0x0006):
                    {
                        UART_Send(UART0,0x49);
                        uint32_t lsr = UART0->LSR;
                        UART_Send(UART0,lsr);			
                    }	
		break;
	    case ((uint32_t)0x0007):
                    {
                        UART_Send(UART0,0x50);
                        uint32_t usr = UART0->USR;
                        UART_Send(UART0,usr);			
                    }	
		break;
        case ((uint32_t)0x000C)://超时未取出，存入buffer，不操作，只做提示
                    {
                        UART0->OFFSET_0.THR = 0x51;
                        while(UART_IsRXFIFONotEmpty(UART0))
                        {
                            //ch = (uint8_t)(UART0->OFFSET_0.RBR & 0xFF);
                            //ringBufferPut(&ringbuf, ch); //ÊäÈëringbuf
                        }
                    }	
		break;

        default:
        UART0->OFFSET_0.THR = 0x52;
        break;
    }

}

void UART1_IRQHandler(void)
{
    uint8_t ch;
    switch(UART1->OFFSET_8.IIR & 0x0f)
    {
        case ((uint32_t)0x0004):
                    {
                        UART0->OFFSET_0.THR = 0x37;
                        while(UART_IsRXFIFONotEmpty(UART1))	//转给UART0
                        {
                            ch = (uint8_t)(UART1->OFFSET_0.RBR & 0xFF);
                            UART0->OFFSET_0.THR = ch;
                        }
                    }
        break;
        case ((uint32_t)0x000C)://超时未取出，转给UART0
            {
                UART0->OFFSET_0.THR = 0x38;
                while(UART_IsRXFIFONotEmpty(UART1))
                {
                        ch = (uint8_t)(UART1->OFFSET_0.RBR & 0xFF);
                        UART0->OFFSET_0.THR = ch;
                }
            }	
		break;
        default:
        UART0->OFFSET_0.THR = 0x99;
        break;
    }
}

