#include <string.h>
#include "gfifo.h"
#include "comfunc.h"
#include "stm32f10x_conf.h"
#define min(x,y)    (x < y? x: y)


/* ���ܣ� ��̬��ʼ��һ��fifo�ṹ������ڶ���fifo������ͬʱ��ֱ�ӽ����˳�ʼ������ô��û��Ҫ�ٵ��øú�����
 * �βΣ� buffer������������2�������η�
 */
void init_fifo(struct kfifo *fifo, uint8_t *buffer, uint16_t size)
{
    if(0 == (size & (size-1)))
		while(1);//�趨��size����2��N�η������޸�
	
	fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = 0;
    fifo->out = 0;
}

uint16_t __kfifo_put(gfifo_t *fifo, const uint8_t *buffer, uint16_t len)
{
    uint16_t l;
	logFuncName();//#include "comfunc.h"
	DISABLE_INT_OF_USING_FIFO();
	
    len = min(len, fifo->size - fifo->in + fifo->out);
    /*
     * Ensure that we sample the fifo->out index -before- we
     * start putting bytes into the kfifo.
     */
    //smp_mb();
    /* first put the data starting from fifo->in to buffer end */
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(fifo->buffer, buffer + l, len - l);

    /*
     * Ensure that we add the bytes to the kfifo -before-
     * we update the fifo->in index.
     */
    //smp_wmb();
    fifo->in += len;
	
	ENABLE_INT_OF_USING_FIFO();
    return len;
}

uint16_t __kfifo_peek(gfifo_t *fifo, uint8_t *buffer, uint16_t len)
{
    logFuncName();//#include "comfunc.h"
	DISABLE_INT_OF_USING_FIFO();
	uint16_t l;
        //�����ݵĻ������ĳ���
    len = min(len, fifo->in - fifo->out);
    /*
     * Ensure that we sample the fifo->in index -before- we
     * start removing bytes from the kfifo.
     */
    //smp_rmb();
    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);
    /*
     * Ensure that we remove the bytes from the kfifo -before-
     * we update the fifo->out index.
     */
    //smp_mb();
    //fifo->out += len;
	ENABLE_INT_OF_USING_FIFO();
    return len;
}

uint16_t __kfifo_get(gfifo_t *fifo, uint8_t *buffer, uint16_t len)
{
	
	DISABLE_INT_OF_USING_FIFO();
	logFuncName();//#include "comfunc.h"
	uint16_t l;
        //�����ݵĻ������ĳ���
    len = min(len, fifo->in - fifo->out);
    /*
     * Ensure that we sample the fifo->in index -before- we
     * start removing bytes from the kfifo.
     */
    //smp_rmb();
    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);
    /*
     * Ensure that we remove the bytes from the kfifo -before-
     * we update the fifo->out index.
     */
    //smp_mb();
    fifo->out += len;
	ENABLE_INT_OF_USING_FIFO();
    return len;
}

/*
�ӵ�һЩ������
*/
void __kfifo_discard(gfifo_t *fifo, uint16_t len)
{
	logFuncName();//#include "comfunc.h"
	DISABLE_INT_OF_USING_FIFO();
	fifo->out += len;
	ENABLE_INT_OF_USING_FIFO();
}



int16_t get_fifo_used_size(gfifo_t *pFifo)
{
	logFuncName();//#include "comfunc.h"
	DISABLE_INT_OF_USING_FIFO();	
	int16_t size = (((pFifo)->in - (pFifo)->out));  //��ѯfifo��δ�������ֽ���uint16_t
	ENABLE_INT_OF_USING_FIFO();
	return size;
}
int16_t get_fifo_unused_size(gfifo_t *pFifo)
{
	logFuncName();//#include "comfunc.h"
	DISABLE_INT_OF_USING_FIFO();	
	int16_t size = ((pFifo)->size - get_fifo_used_size((pFifo)));   		//��ѯfifo��ʣ������uint16_t
	ENABLE_INT_OF_USING_FIFO();
	return size;
}


