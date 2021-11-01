#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include <stdio.h>
#include "gfifo.h"

void USART1_Init(u32 bound);//串口1初始化，A9-A10

gfifo_t* getUart2RxFifo(void);


int16_t uart1_send_by_int(const uint8_t *data, uint16_t len);//串口1中断式发送





#endif


