#ifndef UTIL_KFIFO_H_
#define UTIL_KFIFO_H_

#include <stdint.h>
#include <sys/types.h>

typedef struct {
    uint8_t *buffer;       /* the buffer holding the data */
    size_t size;           /* the size of the allocated buffer */
    size_t in;             /* data is added at offset (in % size) */
    size_t out;            /* data is extracted from off. (out % size) */
} kfifo_t;//in,out存放的都是指针，栈顶指针和栈低指针






static inline size_t kfifo_count(kfifo_t *fifo)
{
    return fifo->in - fifo->out;
}

static inline void kfifo_empty(kfifo_t *fifo)
{
    fifo->in = fifo->out = 0;
}

size_t kfifo_put(kfifo_t *fifo, const uint8_t *buffer, size_t len);
size_t kfifo_get(kfifo_t *fifo, uint8_t *buffer, size_t len);

#endif
