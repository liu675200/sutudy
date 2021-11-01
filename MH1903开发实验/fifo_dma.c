#include "inc.h"
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);
void ringBufferInit(RingBufferTypeDef * buf)
{
    uint32_t len = sizeof(buf->buffer);
    for(;len>0;len--)
    {
        buf->buffer[sizeof(buf->buffer)-len] = 0;
    }
	buf->get_index = 0;
	buf->put_index = 0;
	buf->count = 0;
}

void ringBufferPut(RingBufferTypeDef * buf, uint8_t ch)	//输入
{
	buf->buffer[buf->put_index] = ch;   //将数据复制到指定位置
	buf->put_index = (buf->put_index + 1) & (RING_BUFFER_SIZE - 1);
	buf->count += 1;
	if(buf->put_index == buf->get_index)
	{
		buf->get_index = (buf->get_index + 1) & (RING_BUFFER_SIZE - 1);
	}
}

uint8_t ringBufferGet(RingBufferTypeDef * buf)		//输出
{
	uint8_t ch = -1;
	if(buf->get_index != buf->put_index)
	{
		ch = buf->buffer[buf->get_index];
		buf->count -= 1;
		buf->get_index = (buf->get_index + 1) & (RING_BUFFER_SIZE - 1);
	}
	return ch;
}

//----------------------------kififo--------------------------------
/**
 * 输入
 * 只执行in指针的变化，不执行数据复制操作
 */
uint16_t __kfifo_put(kfifo_t *fifo, uint16_t len)
{
	//输入长度和内存余量的对比
	len = min(len, fifo->size - fifo->in + fifo->out);//实际存入的数量
	if(!len)
	{
		UART_Send(UART0,0xEE);
	}
	UART0->OFFSET_0.THR = 0x53;
    fifo->in += len;
    return len;
}


/**
 * 输出
 * 实际输出了数据，并且out指针加了
 */
uint16_t __kfifo_get(kfifo_t *fifo, uint8_t *buffer, uint16_t len)
{
	uint16_t l,i;
    len = min(len, fifo->in - fifo->out);//实际输出的数量
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));

	for(i = 0;i<l;i++)
	{
		*(buffer + i) = *(fifo->buffer + (fifo->out & (fifo->size - 1)) + i);
	}
	if(len>l)
	{
		for(i = 0;i<(len-l);i++)
		{
			*(buffer + l + i) = *(fifo->buffer + i);
		}
	}
    //memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    //memcpy(buffer + l, fifo->buffer, len - l);
    fifo->out += len;
    return len;
}


















