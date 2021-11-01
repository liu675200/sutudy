#include "util/kfifo.h"
#include <string.h>

#define min(x, y) ((x) > (y) ? (y) : (x))
#define max(x, y) ((x) > (y) ? (x) : (y))


/**
* kfifo_init - allocates a new FIFO using a preallocated buffer
* @buffer: the preallocated buffer to be used.
* @size: the size of the internal buffer, this have to be a power of 2.
* @gfp_mask: get_free_pages mask, passed to kmalloc()
* @lock: the lock to be used to protect the fifo buffer
*
* Do NOT pass the kfifo to kfifo_free() after use! Simply free the
* &struct kfifo with kfree().
*/
struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size,
             gfp_t gfp_mask, spinlock_t *lock)
{
    struct kfifo *fifo;

    /* size must be a power of 2 */
    BUG_ON(size & (size - 1)); //大小必须为2的k次方(k>0)的目的在于put/get中从虚拟索引计算真实索引,size & (size - 1)是常用判断技巧
    
    fifo = kmalloc(sizeof(struct kfifo), gfp_mask); //分配kfifo数据结构
    
    if (!fifo)
        return ERR_PTR(-ENOMEM);

    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = fifo->out = 0; //当fifo->in == fifo->out 时，表示空队列
    fifo->lock = lock;

    return fifo;
}

/**
* kfifo_alloc - allocates a new FIFO and its internal buffer
* @size: the size of the internal buffer to be allocated.
* @gfp_mask: get_free_pages mask, passed to kmalloc()
* @lock: the lock to be used to protect the fifo buffer
*
* The size will be rounded-up to a power of 2.
*/
//通过调用kfifo_alloc分配队列空间，该函数会调用kfifo_init初始化kfifo结构体，并调整size的大小以适应运算
struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
{
    unsigned char *buffer;
    struct kfifo *ret;

    /*
    * round up to the next power of 2, since our 'let the indices
    * wrap' tachnique works only in this case.
    */

    if (size & (size - 1)) { //如果size不是2的k次方，代码将size调整最近的2^k次方附近
            BUG_ON(size > 0x80000000);
            size = roundup_pow_of_two(size);
    }
    
    buffer = kmalloc(size, gfp_mask);
    if (!buffer)
        return ERR_PTR(-ENOMEM);

    ret = kfifo_init(buffer, size, gfp_mask, lock);
    if (IS_ERR(ret))
        kfree(buffer);
    return ret;
}

size_t kfifo_put(kfifo_t *fifo, const uint8_t *buffer, size_t len)//输入
{
    size_t l;
    len = min(len, fifo->size - fifo->in + fifo->out);//对比输入长度和内部剩余空间谁更小

    /* first put the data starting from fifo->in to buffer end */    //从in到end输入数据
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));   //可输入的长度与len对比
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);    //从入口输入数据

    /* then put the rest (if any) at the beginning of the buffer */     //将其他数据输入到buffer开头
    memcpy(fifo->buffer, buffer + l, len - l);

    fifo->in += len;
    return len;
}

size_t kfifo_get(kfifo_t *fifo, uint8_t *buffer, size_t len)//输出
{
    size_t l;
    len = min(len, fifo->in - fifo->out);   //输出数据大小和内含数据大小对比

    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l, fifo->buffer, len - l);

    fifo->out += len;

//    if (fifo->in == fifo->out)
//        fifo->in = fifo->out = 0;
    return len;
}
