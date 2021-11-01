#ifndef GFIFO_H
#define GFIFO_H
#include <stdint.h>


/*
�ر�ע�⣺
*/




/*---------����ֲ����Ҫ��ӵ�ͷ�ļ�------��ʼ---------*/
/*ֻҪĳ�жϺ������õ���gfifo()��API����Ҫ��ֲ����жϺ�*/
#include "stm32f10x_usart.h"
#include "core_cm3.h"
/*---------����ֲ����Ҫ��ӵ�ͷ�ļ�------����---------*/



#pragma pack(1)
typedef volatile struct kfifo {
    uint8_t *buffer;     /* the buffer holding the data */
    uint16_t size;         /* the size of the allocated buffer */
    uint16_t in;           /* data is added at offset (in % size) */
    uint16_t out;          /* data is extracted from off. (out % size) */
    //spinlock_t *lock;          /* protects concurrent modifications */
}gfifo_t;
#pragma pack()





/***************************����ֲ�ĺ꿪ʼ****************************************/
//ʹ�ܺͳ�����Щ�õ���fifo���ж�
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
							
/***************************����ֲ�ĺ����****************************************/


/*-------------����ӿ�------------��ʼ-------------*/
//ע�⣺fifo��������������2�������η����ֽ�
void init_fifo(struct kfifo *fifo, uint8_t *buffer, uint16_t size);//���������ÿɲ��ã��ο������Ķ���
#define gfifo_put(...)  __kfifo_put(__VA_ARGS__)
#define gfifo_get(...)  __kfifo_get(__VA_ARGS__)
#define gfifo_peek(...)  __kfifo_peek(__VA_ARGS__)
#define gfifo_discard(...)  __kfifo_discard(__VA_ARGS__)
int16_t get_fifo_used_size(gfifo_t *pFifo);//��ѯfifo��δ�������ֽ���uint16_t
int16_t get_fifo_unused_size(gfifo_t *pFifo);//��ѯfifo��ʣ������uint16_t 		
/*-------------����ӿ�------------����--------------*/



uint16_t __kfifo_put(gfifo_t *fifo,
            const uint8_t *buffer, uint16_t len);
uint16_t __kfifo_get(gfifo_t *fifo,
             uint8_t *buffer, uint16_t len);

uint16_t __kfifo_peek(gfifo_t *fifo,
             uint8_t *buffer, uint16_t len);
void __kfifo_discard(gfifo_t *fifo, uint16_t len);
#endif // GFIFO_H
