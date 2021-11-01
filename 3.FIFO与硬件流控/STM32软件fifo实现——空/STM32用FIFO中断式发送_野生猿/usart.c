
#include "usart.h"
#include "comfunc.h"


uint8_t uart1TxBuf[1 << 8];
gfifo_t uart1TxFifo ={.buffer = uart1TxBuf,
                                        .size = sizeof(uart1TxBuf),
                                        .in = 0,
                                        .out = 0};







gfifo_t* getUart2RxFifo(void)
{
        static uint8_t uart2RxBuf[1 << 9];
        static gfifo_t uart2RxFifo ={.buffer = uart2RxBuf,
                                        .size = sizeof(uart2RxBuf),
                                        .in = 0,
                                        .out = 0 };

        return &uart2RxFifo;
}

//串口1初始化
void USART1_Init(u32 bound)
{
        //GPIO端口设置
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

        //USART1_TX   GPIOA.9
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
        GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

        //USART1_RX	  GPIOA.10初始化
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
        GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

        //Usart1 NVIC 配置
        NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//指出中断通道为UASRT1
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
        NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

        //USART 初始化设置
        USART_InitStructure.USART_BaudRate = bound;//串口波特率
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
        USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
        USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

        USART_Init(USART1, &USART_InitStructure); //初始化串口1
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接收寄存器非空中断
        //USART_ITConfig(USART1, USART_IT_TC, ENABLE);//开启串口发送完成中断（移位寄存器发送完成）
        //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//开启串口发送寄存器空中断（只要DR空就中断，实际上，这句话会导致立马中断，因为此时DR确实是空的，所以一般只在发送时才enable它，而不会在初始化时就enable它）

        USART_Cmd(USART1, ENABLE);                    //使能串口1

}
//串口1 所有中断服务
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
        logFuncName();
        if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
        {
                USART_ClearITPendingBit(USART1, USART_IT_RXNE);
                USART_ClearFlag(USART1, USART_IT_RXNE);

                volatile u8 rxByte;
                rxByte =USART_ReceiveData(USART1);	//读取接收到的数据
        }

        if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)  //发送寄存器空中断
        {
                uint8_t sendCh;
                if(1 == gfifo_get(&uart1TxFifo, &sendCh, 1))//main调用链中操作uart1TxFifo的地方必须禁掉本中断（或全局中断）
                {
                        USART1->DR = sendCh;
                }
                else
                {
                        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);//已发完，关中断
                }
        }
}

/*
通过中断发送数据
返回：1成功，0失败
*/
int16_t uart1_send_by_int(const uint8_t *data, uint16_t len)
{
        logFuncName();
        if(get_fifo_unused_size(&uart1TxFifo) > len)//只有空闲区>len，才执行发送程序
        {
                gfifo_put(&uart1TxFifo, data, len);
        }
        else
        {
                rtt_printf("uart1 SendFifo has no space\r\n");//程序走到这里，意味着会出现发送丢失
                return 0;
        }

//	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
//	if(get_fifo_used_size(&uart1TxFifo) == len)


        if(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET)//置位SET说明已空(只有上一波已发完时，DR才会空)
        {
                USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
        }
//	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

        return 1;
}
