#include "inc.h"
extern void USART_SendData(uint16_t Data);

extern void TIM6_ms_ON(uint16_t ms);
extern int TIM6_ms_OFF();
extern void Flash_Erase();
extern int Flash_Write(uint8_t AllData[2048],uint32_t NowAddr);


//数据帧格式，且对齐
/* /-------- Packet in IAP memory ------------------------------------------\
 * | 0      |  1    |  2   |     3   | ... |   n+3   | n+1  | n+5  | 
 * |------------------------------------------------------------------------|
 * | start  | number| !num | data[0] | ... | data[n] | crc0 | crc1 |
 * \------------------------------------------------------------------------/
 */
typedef struct
{
	uint8_t Start;
	uint8_t Number;
	uint8_t Inverse_Number;
	uint8_t Data[128];
	uint8_t check_sum;
}Xmoden_Typedef;


int RecData(Xmoden_Typedef* Message,int number,uint32_t* NowAddr)
{
    uint8_t* addr = &(Message->Start);
    volatile int i,m = 0;
    if(IF){USART_SendData((uint16_t)(0x51));}

    TIM6_ms_ON(3000);
    while(m<132)//接收数据
    {
        if(i>=10)//超时退出
            {
                if(IF){USART_SendData((uint16_t)(0x54));}
                return -1;
            }
        if((USART1_SR & (1<<5)) != 0)
        {
            *(addr + m) = (uint8_t)(USART1_DR & (uint16_t)0xFF);
            i = 0;
            if(*addr == EOT)//接收完成
                {
                    if(IF){USART_SendData((uint16_t)(0x52));}
                    return 0;
                }
            else 
                m++;
        }
        else if(TIM6_ms_OFF())
            {
                if(IF){USART_SendData((uint16_t)(0x53));}
                USART_SendData(NAK);
                i++;
                TIM6_ms_ON(3000);
            }
    }

    if(IF){USART_SendData((uint16_t)(0x55));}

    if(Message->Start != SOH)//包头不对，重发
        {
            if(IF){USART_SendData((uint16_t)(0x56));}
            return -2;
        }
    if(((Message->Number)^(Message->Inverse_Number)) != 0xFF)//包名不对，重发
        {
            if(IF){USART_SendData((uint16_t)(0x57));}
            return -3;
        }
    if(Message->Number != number)//顺序不对，出错停止
        {
            if(IF){USART_SendData((uint16_t)(0x58));}
            return -5;
        }
    uint8_t sum = 0;
    for(int i=0;i<128;i++)
        {sum += Message->Data[i];}
    if(sum != Message->check_sum)//CRC累加和校验错误，重发
        {
            if(IF){USART_SendData((uint16_t)(0x59));}
            return -4;
        }

    if(Flash_Write(Message->Data,(*NowAddr)))
        {
            return -4;
        }
    (*NowAddr) = (*NowAddr) + 128;

    if(IF){USART_SendData((uint16_t)(0x5A));}
    return 1;//校验无误，发送ACK
}

void xmodem()
{
    int status = -1;
    int end = 3;
    Xmoden_Typedef Message[50];
    uint8_t number = 0x01;
    uint32_t NowAddr = WRITE_START_ADDR;

    while (1)//开始接收数据
    {
        if(IF){USART_SendData((uint16_t)(0x50));}
        status = RecData(Message,number,&NowAddr);//进入函数查询转态
        if(IF){USART_SendData((uint16_t)(0x5B));}
        switch (status)
        {
            case 1://接收成功，解析正确
                end = 3;
                if(IF){USART_SendData((uint16_t)(0x5C));}
                USART_SendData(ACK);
                number++;
                break;
            case 0://接收到EOT，结束程序
                end = -1;
                if(IF){USART_SendData((uint16_t)(0x5D));}
                USART_SendData(ACK);
                break;
            case -1://没有接收到数据，超时，重新查询，最多3次
                end--;
                if(IF){USART_SendData((uint16_t)(0x5E));}
                USART_SendData(NAK);
                //TIM6_ms_ON(3000);
                break;
            case -2://数据头不对,重发
                end = 3;
                if(IF){USART_SendData((uint16_t)(0x5F));}
                USART_SendData(NAK);
                break;
            case -3://包名不对,重发
                end = 3;
                if(IF){USART_SendData((uint16_t)(0x60));}
                USART_SendData(NAK);
                break;
            case -4://CRC校验不对，重发
                end = 3;
                if(IF){USART_SendData((uint16_t)(0x61));}
                USART_SendData(NAK);
                break;
            case -5://接收顺序出错，退出程序
                end = 0;
                if(IF){USART_SendData((uint16_t)(0x62));}
                USART_SendData(CAN);
                break;
            default ://接收出错，退出程序
                end = 0;
                if(IF){USART_SendData((uint16_t)(0x62));}
                USART_SendData(CAN);
                break;
        }
        if(end == 0)//  接收出错/超时3次
        {
            if(IF){USART_SendData((uint16_t)(0x63));}
            number = 0x01;//重新开始接收
            end = 3;
        }
        if(end == -1)
        {
            break;
        }
        /*
        else while((end == -1))
        {
            if(TIM6_ms_OFF())
            {
                for(uint32_t address=WRITE_START_ADDR;address<=WRITE_END_ADDR;address++)//打印检查数据书写是否正确
                    {USART_SendData(*(volatile uint32_t*)address);}
                TIM6_ms_ON(3000);
            }
            
        }*/
    }
}