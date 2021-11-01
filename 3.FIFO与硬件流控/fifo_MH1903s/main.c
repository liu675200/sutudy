#include "inc.h"

extern void NVIC_Configuration(IRQn_Type IRQn,uint32_t Preemption,uint32_t Sub);
extern void UART_Configuration(UART_TypeDef* UARTx);
extern int UART_IsRXFIFONotEmpty(UART_TypeDef* UARTx);
extern uint8_t UART_Read(UART_TypeDef* UARTx);
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);
extern void ringBufferPut(RingBufferTypeDef * buf, uint8_t ch);
extern uint8_t ringBufferGet(RingBufferTypeDef * buf);
extern void Time_config(uint32_t ms);

void system_set(void);
RingBufferTypeDef ringbuf;

void UART_SetRTS(UART_TypeDef *UARTx)
{
	UARTx->MCR |= ((uint32_t)0x0002);
}

void UART_ResetRTS(UART_TypeDef *UARTx)
{
	UARTx->MCR &= ~((uint32_t)0x0002);
}


int main(void)
{
    system_set();
    NVIC_Configuration(UART0_IRQn,0,1);
    NVIC_Configuration(UART1_IRQn,1,1);
    GPIO_PA_ALT=0x55555550;  //PA0,PA1复用为串口
    GPIO_PB_ALT=0xFF555555;  //PB12,PB13、14、15复用为串口

    UART_Configuration(UART0);
    UART_Configuration(UART1);

    uint8_t tmpSend;
    UART_SetRTS(UART1);//使能RTS引脚
    while(1) 
    {
        if(ringbuf.count >= 1)
        {	
            UART0->OFFSET_0.THR = 0x31;
            tmpSend = ringBufferGet(&ringbuf);
            //while ((UART1->MSR & (1<<4)) == 0);//等待从机接收	
            UART1->OFFSET_0.THR = tmpSend;//发送到从机
        }
        //Time_config(10);
        /*在此处开启中断，开启轮询查fifo非空时，
        系统将直接在此处处理掉接收到的数据，中断服务函数不会生效，
        如果给一个延时,数据过期后进入丢弃中断，如果不读取接收寄存器将一直不断进入丢弃中断，
        只有读取了数据才会硬件清除中断标志位，所以m4系列或者MH1903需要用中断处理过期的数据，其他芯片另论
        if(UART0->USR & (1<<3))
        //if((UART0->RFL & 0xF) != 0)//判断fifo内是否还有数据
        {
            UART0->OFFSET_0.THR = 0x75;
            while(UART_IsRXFIFONotEmpty(UART0))
            {
                ch = (uint8_t)(UART0->OFFSET_0.RBR & 0xFF);
                ringBufferPut(&ringbuf, ch); //ÊäÈëringbuf
            }
            flag = 0;
        }*/
        
    }
}


void UART0_IRQHandler(void)
{
    uint8_t ch;
    switch(UART0->OFFSET_8.IIR & 0x0f)
    {
        case ((uint32_t)0x0004):
                    {  
                        UART0->OFFSET_0.THR = 0x46;
                        while(UART_IsRXFIFONotEmpty(UART0))	//FIFO·Ç¿ÕÊ±Êä³öÊý¾Ý
                        {
                            ch = (uint8_t)(UART0->OFFSET_0.RBR & 0xFF);
                            ringBufferPut(&ringbuf, ch); //ÊäÈëringbuf
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
                            ch = (uint8_t)(UART0->OFFSET_0.RBR & 0xFF);
                            ringBufferPut(&ringbuf, ch); //ÊäÈëringbuf
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


void system_set(void)//门控，软复位，时钟频率设定
{
    SYSCTRL->CG_CTRL1 |= 0X04310003;
    SYSCTRL->SOFT_RST1 |= 0x00380003;
    SYSCTRL->FREQ_SEL |=  0x200F395A;//取PLL时钟192MHZ
}





