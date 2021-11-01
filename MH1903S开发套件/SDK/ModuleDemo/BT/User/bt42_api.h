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
* @brief     提供给客户使用 bt42.lib库的接口介绍说明
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
* @NOTE  初始化接口和调用接口，此接口为lib库重要调度接口。
*/


/**
* @brief 蓝牙库初始化接口函数，蓝牙工作前必须调用初始化函数.
*/
extern int bt42_init(void);	

/**
* @brief    蓝牙任务处理函数，需要循环调用，建议10ms调度周期.
* @param[1] run_times : run_times为单次调用执行的循环次数。
*/
extern void bt42_run_loop(int run_times);


/*
* @note    回调客户接口，这些函数需要客户实现。
*/

/**
* @brief    单位时间调度接口，为lib库内部提供时间戳。
* @param[1] void
* @return   当前时间点
*/
extern uint32_t hal_time_ms(void);


/**
* @brief    打印调用接口，为lib提供基础打印信息。
* @param[1] fmt
*/

extern int hal_print(const char *fmt, ...);


extern void bt42_hci_event_debug(uint8_t *event, uint16_t len);

/*
* @NOTE: 硬件适配层接口(调用)
*/

/**
* @brief    蓝牙芯片的串口中断处理函数
* @param[1] void
* @return   void
*/
extern void bt42_uart_isr(void);


/*
* @note	收发接口(调用vs实现)
*/

/*
* @brief   蓝牙lib库的输入接口，需要给lib发送命令，需要调用次函数。
* @param[1] cmd: 往lib库输入的命令数据流指针。
* @param[2] len: 命令数据流的长度。
* @return   void
*/
extern int bt42_api_cmd(uint8_t *cmd, uint16_t len);

/**
* @brief   蓝牙lib库的输出函数，此函数为lib的事件输出接口。
* @param[1] event；lib输出的事件数据流指针。
* @param[2] len  : lib输出事件数据流的长度。
* @return   void
*/
extern void bt42_api_event(uint8_t *event, uint16_t len);


/*
* @note	休眠接口(调用)
*/

/**
* @brief   休眠初始化
* @param[1] 0休眠1唤醒
* @return   void
*/
extern void mcu_wake_bt_pin(uint8_t state);


/**
* @brief   检测是否超时
* @param[1] 开始时间
* @param[2] 超时间隔
* @return   1：超时 0：未超时
*/
extern uint8_t bt42_is_timeout(uint32_t start_time, uint32_t interval);


#endif

