#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define UART_SEND_BUF_SIZE   (512)
#define UART_RECV_BUF_SIZE   (512)

#define UART_INT_MODE        (1)

void uart_Config(uint32_t f_Khz);
void uart_RecvFlush(void);
int32_t uart_RecvChar(void);
int32_t uart_IsSendFinish(void);
int32_t uart_SendChar(uint8_t ch);
int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len);

	
#ifdef __cplusplus
}
#endif


#endif   ///< __UART_H__
