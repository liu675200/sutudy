#ifndef GFIFO_H
#define GFIFO_H
#include <stdint.h>


/*
特别注意：
*/




/*---------因移植宏需要添加的头文件------开始---------*/
/*只要某中断函数中用到了gfifo()的API，就要移植这个中断宏*/
#include "stm32f10x_usart.h"
#include "core_cm3.h"
/*---------因移植宏需要添加的头文件------结束---------*/



#pragma pack(1)
typedef volatile struct kfifo {
    uint8_t *buffer;     /* the buffer holding the data */
    uint16_t size;         /* the size of the allocated buffer */
    uint16_t in;           /* data is added at offset (in % size) */
    uint16_t out;          /* data is extracted from off. (out % size) */
    //spinlock_t *lock;          /* protects concurrent modifications */
}gfifo_t;
#pragma pack()





/***************************需移植的宏开始****************************************/
//使能和除能那些用到了fifo的中断
//#define	ENABLE_INT_OF_USING_FIFO()	do{USART_ITConfig(USART1, USART_IT_TC, ENABLE);	\
//										USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);\
//										USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);\
//									}while(0)
//#define	DISABLE_INT_OF_USING_FIFO()	do{USART_ITConfig(USART1, USART_IT_TC, DISABLE);	\
//										USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);\
//										USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);\
//									}while(0)

#define	DISABLE_INT_OF_USING_FIFO()	do{__disable_irq();__disable_fault_irq();}while(0)
#define	ENABLE_INT_OF_USING_FIFO()	do{__enable_irq(); __enable_fault_irq();}while(0)
							
/***************************需移植的宏结束****************************************/


/*-------------对外接口------------开始-------------*/
//注意：fifo缓冲区，必须是2的整数次方个字节
void init_fifo(struct kfifo *fifo, uint8_t *buffer, uint16_t size);//本函数可用可不用，参考函数的定义
#define gfifo_put(...)  __kfifo_put(__VA_ARGS__)
#define gfifo_get(...)  __kfifo_get(__VA_ARGS__)
#define gfifo_peek(...)  __kfifo_peek(__VA_ARGS__)
#define gfifo_discard(...)  __kfifo_discard(__VA_ARGS__)
int16_t get_fifo_used_size(gfifo_t *pFifo);//查询fifo中未读出的字节数uint16_t
int16_t get_fifo_unused_size(gfifo_t *pFifo);//查询fifo的剩余容量uint16_t 		
/*-------------对外接口------------结束--------------*/



uint16_t __kfifo_put(gfifo_t *fifo,
            const uint8_t *buffer, uint16_t len);
uint16_t __kfifo_get(gfifo_t *fifo,
             uint8_t *buffer, uint16_t len);

uint16_t __kfifo_peek(gfifo_t *fifo,
             uint8_t *buffer, uint16_t len);
void __kfifo_discard(gfifo_t *fifo, uint16_t len);
#endif // GFIFO_H
