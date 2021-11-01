#ifndef BT42_UART_H
#define BT42_UART_H

#include <stdint.h>

void blue_uart_Configuration(void);
int uart_block_write(uint8_t *pbuffer,uint32_t len);
uint16_t uart_read(uint8_t *pDataBuf, uint16_t len);


#endif
