#include "inc.h"

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