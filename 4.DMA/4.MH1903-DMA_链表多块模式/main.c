#include "inc.h"
extern void NVIC_Configuration(IRQn_Type IRQn,uint32_t Preemption,uint32_t Sub);
extern void UART_Configuration(UART_TypeDef* UARTx);
extern uint8_t UART_Read(UART_TypeDef* UARTx);
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);
extern void DMA_Config_M2M(void);
extern void DMA_ChannelCmd(void);
extern void LLI_Init(void);
void system_set(void);

extern LLI llilist[MULTIBLOCK_BLOCKCOUNTER];

uint32_t src_Buf0[MULTIBLOCK_BLOCKSIZE] = {0};
uint32_t src_Buf1[MULTIBLOCK_BLOCKSIZE] = {0};
uint32_t src_Buf2[MULTIBLOCK_BLOCKSIZE] = {0};
uint32_t src_Buf3[MULTIBLOCK_BLOCKSIZE] = {0};

uint32_t dst_Buf0[MULTIBLOCK_BLOCKSIZE] = {0};
uint32_t dst_Buf1[MULTIBLOCK_BLOCKSIZE] = {0};
uint32_t dst_Buf2[MULTIBLOCK_BLOCKSIZE] = {0};
uint32_t dst_Buf3[MULTIBLOCK_BLOCKSIZE] = {0};

int main(void)
{
    uint32_t i;
    system_set();//设置时钟，使能时钟
    UART_Configuration(UART0);
    DMA_Config_M2M();//先配置DMA
    LLI_Init();//再配置链表，将链表连接起来
    NVIC_Configuration(DMA_IRQn,1,1);
    UART_Send(UART0,0x44);
    
    for (i = 0; i < MULTIBLOCK_BLOCKSIZE; i++)//源数据赋值
    {
        src_Buf0[i] = 0;
        src_Buf1[i] = 1;
        src_Buf2[i] = 2;
        src_Buf3[i] = 3;
    }
    DMA_ChannelCmd();
    UART_Send(UART0,0x66);
    while (0 == (llilist[2].CTL_H & BIT(12)));
    for (i = 0; i < (MULTIBLOCK_BLOCKSIZE); i++)//打印目的地址数据
    {
        UART_Send(UART0,dst_Buf0[i]);
        UART_Send(UART0,dst_Buf1[i]);
        UART_Send(UART0,dst_Buf2[i]);
        UART_Send(UART0,dst_Buf3[i]);
    }
    UART_Send(UART0,0x77);
    while (1)
    {
        /* code */
    }
    return 0;
}


void system_set(void)//门控，软复位，时钟频率设定
{
    SYSCTRL->CG_CTRL1 |= 0X04310003;
    SYSCTRL->SOFT_RST1 |= 0x00380003;
    SYSCTRL->FREQ_SEL |=  0x200F395A;//取PLL时钟192MHZ
}



