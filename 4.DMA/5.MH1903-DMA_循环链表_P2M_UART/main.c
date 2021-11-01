#include "inc.h"
extern void NVIC_Configuration(IRQn_Type IRQn,uint32_t Preemption,uint32_t Sub);
extern void UART_Configuration(UART_TypeDef* UARTx);
extern uint8_t UART_Read(UART_TypeDef* UARTx);
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);
extern void DMA_Config_P2M(void);
extern void DMA_ChannelCmd(void);
extern void LLI_Init(void);
void system_set(void);
extern void Time_config(uint32_t ms);

extern LLI llilist[MULTIBLOCK_BLOCKCOUNTER];

uint32_t dst_Buf0[MULTIBLOCK_BLOCKSIZE] = {0x11,0x11,0x11,0x11,0x11};


int main(void)
{
    uint32_t i;
    system_set();//设置时钟，使能时钟
    UART_Configuration(UART0);
    UART_Configuration(UART1);

    DMA_Config_P2M();//先配置DMA
    LLI_Init();//再配置链表，将链表连接起来

    NVIC_Configuration(DMA_IRQn,1,1);
    UART_Send(UART1,0x44);

    DMA_ChannelCmd();//使能DMA线0
    Time_config(5000);
    while (1)
    {
        if(TIM0->CurrentValue < 100)
        {
            for (i = 0; i < (20); i++)//打印目的地址数据
            {
                UART_Send(UART1,dst_Buf0[i]);
            }
            Time_config(10000);
        }
    }
    
    return 0;
}


void system_set(void)//门控，软复位，时钟频率设定
{
    SYSCTRL->CG_CTRL1 |= 0X04310003;
    SYSCTRL->SOFT_RST1 |= 0x00380003;
    SYSCTRL->FREQ_SEL |=  0x200F395A;//取PLL时钟192MHZ
}



