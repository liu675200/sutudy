
#ifndef MHSMCU_UART
#define MHSMCU_UART

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>



#define RECV_INT  (BIT0)
#define SEND_INT  (BIT1 | BIT7)


#define UART_SEND_BUF_SIZE      256
#define UART_RECV_BUF_SIZE      256

typedef struct uart_param_s
{
    int     tx_transmited:1;                //���ݴ��������ǣ�0��ʾ���ڴ������ݣ�1��ʾ������������Կ�ʼ������һ���ֽ�
    int     baudrate;                   //���ڲ�����
    struct{
        int read_index;             //�жϷ�������
        int write_index;            //д����������
        //u8 send_bytes;            //�����ֽ���
        //u8 send_int;              //������������жϵĴ���
        //u32 cnt;
        volatile int bytes;                 //�������е��ֽ���
        uint8_t buf[UART_SEND_BUF_SIZE];    //���ͻ�����
    }send;
    struct{
        int read_index;             //������������
        int write_index;            //�ж�д����������
        volatile int bytes;                 //�������е��ֽ���
        //u32   cnt;            //�����ֽ���
        //u32   error_bytes;            //żУ������ֽ���
        uint8_t buf[UART_RECV_BUF_SIZE];    //���ջ�����
        int overflow;           //���ջ�����������
    }recv;
}uart_param;

void uart_Config(uint32_t baudrate, uint32_t parity);
void uart_RecvFlush(void);
int32_t uart_RecvChar(void);
int32_t uart_IsSendFinish(void);
int32_t uart_SendChar(uint8_t ch);
int32_t uart_SendBuff(uint8_t *pBuf, uint32_t len);
int32_t uart_getbytes(void);
void uart_ReConfig(uint32_t baudrate, uint32_t parity);
int32_t uart_SendCharPoll(uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif

