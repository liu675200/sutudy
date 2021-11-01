#ifndef BT42_API_H
#define BT42_API_H

#include "stdint.h"

#define  BT_MCUWAKEUPBT_IO_GROUP		GPIOE
#define  BT_MCUWAKEUPBT_IO_PIN			GPIO_Pin_8
#define  BT_MCUWAKEUPBT_CLR()           GPIO_ResetBits(GPIOE, GPIO_Pin_8)
#define  BT_MCUWAKEUPBT_SET()           GPIO_SetBits(GPIOE, GPIO_Pin_8)
			
#define  BT_RSTN_IO_GROUP			    GPIOE
#define  BT_RSTN_IO_PIN				    GPIO_Pin_9
#define  BT_RST_CLR()                   GPIO_ResetBits(GPIOE, GPIO_Pin_9)
#define  BT_RST_SET()                   GPIO_SetBits(GPIOE, GPIO_Pin_9)

#define	 BT_BTWAKEUPMCU_IO_GROUP		GPIOE
#define	 BT_BTWAKEUPMCU_IO_PIN		    GPIO_Pin_7

/*****************************************************************
* @file      bt42_api.h
* @brief     �ṩ���ͻ�ʹ�� bt42.lib��Ľӿڽ���˵��
*
* @Details
* @version   v1.0
*
*-----------------------------------------------------------------*
* Rmark      Description
*-----------------------------------------------------------------*
*
******************************************************************
**/

typedef struct struct_bt42_hci_s
{
	uint8_t packet_type;
	uint8_t opcode;
	volatile uint8_t length;
	uint8_t payload[];
}struct_bt42_hci;

/*
* @NOTE  ��ʼ���ӿں͵��ýӿڣ��˽ӿ�Ϊlib����Ҫ���Ƚӿڡ�
*/


/**
* @brief �������ʼ���ӿں�������������ǰ������ó�ʼ������.
*/
extern int bt42_init(void);	

/**
* @brief    ����������������Ҫѭ�����ã�����10ms��������.
* @param[1] run_times : run_timesΪ���ε���ִ�е�ѭ��������
*/
extern void bt42_run_loop(int run_times);


/*
* @note    �ص��ͻ��ӿڣ���Щ������Ҫ�ͻ�ʵ�֡�
*/

/**
* @brief    ��λʱ����Ƚӿڣ�Ϊlib���ڲ��ṩʱ�����
* @param[1] void
* @return   ��ǰʱ���
*/
extern uint32_t hal_time_ms(void);


/**
* @brief    ��ӡ���ýӿڣ�Ϊlib�ṩ������ӡ��Ϣ��
* @param[1] fmt
*/

extern int hal_print(const char *fmt, ...);


extern void bt42_hci_event_debug(uint8_t *event, uint16_t len);

/*
* @NOTE: Ӳ�������ӿ�(����)
*/

/**
* @brief    ����оƬ�Ĵ����жϴ�����
* @param[1] void
* @return   void
*/
extern void bt42_uart_isr(void);


/*
* @note	�շ��ӿ�(����vsʵ��)
*/

/*
* @brief   ����lib�������ӿڣ���Ҫ��lib���������Ҫ���ôκ�����
* @param[1] cmd: ��lib�����������������ָ�롣
* @param[2] len: �����������ĳ��ȡ�
* @return   void
*/
extern int bt42_api_cmd(uint8_t *cmd, uint16_t len);

/**
* @brief   ����lib�������������˺���Ϊlib���¼�����ӿڡ�
* @param[1] event��lib������¼�������ָ�롣
* @param[2] len  : lib����¼��������ĳ��ȡ�
* @return   void
*/
extern void bt42_api_event(uint8_t *event, uint16_t len);


/*
* @note	���߽ӿ�(����)
*/

/**
* @brief   ���߳�ʼ��
* @param[1] 0����1����
* @return   void
*/
extern void mcu_wake_bt_pin(uint8_t state);


/**
* @brief   ����Ƿ�ʱ
* @param[1] ��ʼʱ��
* @param[2] ��ʱ���
* @return   1����ʱ 0��δ��ʱ
*/
extern uint8_t bt42_is_timeout(uint32_t start_time, uint32_t interval);


#endif

