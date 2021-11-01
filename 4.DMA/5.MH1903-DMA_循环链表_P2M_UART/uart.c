#include "inc.h"

void UART_SetRTS(UART_TypeDef *UARTx)
{
	UARTx->MCR |= ((uint32_t)0x0002);
}

void UART_ResetRTS(UART_TypeDef *UARTx)
{
	UARTx->MCR &= ~((uint32_t)0x0002);
}

int UART_IsRXFIFONotEmpty(UART_TypeDef* UARTx)//有数据返回1，无数据返回0
{
	if (0 != (UARTx->USR & ((uint32_t)0x0008)))
	{
		return 1;
	}
	return 0;
}                            

uint8_t UART_Read(UART_TypeDef* UARTx)
{
    return (UARTx->OFFSET_0.RBR & 0xFF);
}

void UART_Send(UART_TypeDef* UARTx,uint8_t ch)//fifo 模式下不能使用忙，有数据在fifo时一直忙
{
    while (UARTx->USR & 0x1);    
    UARTx->OFFSET_0.THR = ch;
}

void UART_Configuration(UART_TypeDef* UARTx)//Gpio端口复用及串口初始化
{
    uint32_t BaudRate = 115200;//定义波特率为115200
    uint32_t tmpBaudRateDiv;
    uint32_t PCLK;
    GPIO_PA_ALT=0x55555550;  //PA0,PA1复用为串口
    GPIO_PB_ALT=0xFF555555;  //PB12,PB13、14、15复用为串口

    //-----------------------fifo开启-------------------
    UARTx->LCR &= ~((uint32_t)0x0080);
    //UARTx->OFFSET_4.IER &= ~((uint32_t)0x0080);//开启fifo发送中断
	UARTx->SFE &= ~((uint32_t)0x0001);	
    //UARTx->SDMAM &= ~((uint32_t)0x0001);//不用DMA模式
    UARTx->SDMAM |= ((uint32_t)0x0001);//使用DMA模式
    UARTx->SRT = ((uint32_t)0x0000);//接收只有1/4时取消接收
    UARTx->STET = ((uint32_t)0x0002);//发送只剩1/4个字节时取消发送
    UARTx->SFE |= ((uint32_t)0x0001);//使能FIFO
    
    //-----------------------串口开启---------------------------------
    PCLK = (SYSCTRL->PCLK_1MS_VAL) *1000;//UART输入时钟周期,即PCLK时钟周期。使用PCLK 1ms对应的cycle乘1000获得

    UARTx -> LCR |= ((uint32_t)0x0080);//使能分频寄存器DLL和DLH读写操作，即DLAB=1
    tmpBaudRateDiv = (uint32_t)((PCLK / 16) + BaudRate / 2) / BaudRate;//波特率计算公式
    UARTx -> OFFSET_0.DLL = (tmpBaudRateDiv & 0x00FF);//分频寄存器低位赋值
	UARTx -> OFFSET_4.DLH = ((tmpBaudRateDiv >> 8) & 0x00FF);//分频寄存器高位赋值
    UARTx -> LCR &= ~((uint32_t)0x0080);
    UARTx -> LCR = 0x00000003;//软件清0，DLAB=0

    //------------------------------------------------------------------- 
    UARTx->SFE |= ((uint32_t)0x0001);	//使能FIFO
    UARTx->MCR &= ~((uint32_t)0x0020);//自动流控关闭
    //UART_Send(UART1,0x11);
/*     if(UARTx == UART1)
    {
        UART1->MCR |= ((uint32_t)0x0020);//自动流控打开
    } 
        UART_Send(UART0,0x21);*/
    //UART_Send(UART1,0x22);
    //UARTx->LCR &= ~((uint32_t)0x0080);
    //UARTx->OFFSET_4.IER |= ((uint32_t)0x0001);//使能接收中断
}

