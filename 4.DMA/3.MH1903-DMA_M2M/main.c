#include "inc.h"

extern void NVIC_Configuration(IRQn_Type IRQn,uint32_t Preemption,uint32_t Sub);
extern void UART_Configuration(UART_TypeDef* UARTx);
extern int UART_IsRXFIFONotEmpty(UART_TypeDef* UARTx);
extern uint8_t UART_Read(UART_TypeDef* UARTx);
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);
extern void ringBufferPut(RingBufferTypeDef * buf, uint8_t ch);
extern uint8_t ringBufferGet(RingBufferTypeDef * buf);
extern void Time_config(uint32_t ms);
extern void DMA_Config_M2M(void);
extern void DMA_ChannelCmd(void);
void system_set(void);
RingBufferTypeDef ringbuf;

uint32_t src_Buf[128] = {0x00,0x01,0x02,0x03,
                         0x04,0x05,0x06,0x07,
                         0x08,0x09,0x0A,0x0B};
uint32_t dst_Buf[128] = {0};


int main(void)
{
    system_set();//设置时钟，使能时钟
    //NVIC_Configuration(UART1_IRQn,1,1);
    GPIO_PA_ALT=0x55555550;  //PA0,PA1复用为串口
    UART_Configuration(UART0);
    //UART_Send(UART0,0x22);
    DMA_Config_M2M();
    UART_Send(UART0,0x44);
    DMA_ChannelCmd();

    while((DMA->RawTfr_L & (1<<channel)) == 0);
    UART_Send(UART0,0x66);
    for(uint32_t i=0;i<128;i++)
    {
        UART_Send(UART0,dst_Buf[i]);
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

    SYSCTRL->CG_CTRL2 |= ((uint32_t)0x20000000);
    SYSCTRL->LOCK_R &= ~(((uint32_t)0x20000000) & 0xF0000000);
    SYSCTRL->SOFT_RST2 |= ((uint32_t)0x20000000);
    SYSCTRL->LOCK_R |= (((uint32_t)0x20000000) & 0xF0000000);
}



