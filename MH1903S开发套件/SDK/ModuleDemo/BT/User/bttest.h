#ifndef BTTEST_H
#define BTTEST_H



/*********Packet type*******************/
#define CMD   0x01  
#define EVENT 0x02

#define CMD_SUCCESS	0x00
#define CMD_FAIL	0x01

/*********CMD命令名称 Opcode 操作码描述***/
#define HCI_CMD_SET_BT_ADDR        0x00     //设置 BT3.0 地址
#define HCI_CMD_SET_BLE_ADDR       0x01     //设置 BLE 地址
#define HCI_CMD_SET_VISIBILITY     0x02     //设置可发现和广播
#define HCI_CMD_SET_BT_NAME        0x03     //设置 BT3.0 名称
#define HCI_CMD_SET_BLE_NAME       0x04     //设置 BLE 名称
#define HCI_CMD_SEND_SPP_DATA      0x05     // 发送 BT3.0（SPP）数据
#define HCI_CMD_SEND_BLE_DATA      0x09 	//发送 BLE 数据
#define HCI_CMD_SEND_DATA          0x0A 	//发送数据（自动选择通道）
#define HCI_CMD_STATUS_REQUEST     0x0B 	//请求蓝牙状态
#define HCI_CMD_SET_PAIRING_MODE   0x0C 	//设置配对模式
#define HCI_CMD_SET_PINCODE        0x0D 	//设置配对码
#define HCI_CMD_SET_UART_FLOW      0x0E 	//设置 UART 流控
#define HCI_CMD_SET_UART_BAUD      0x0F 	//设置 UART 波特率
#define HCI_CMD_VERSION_REQUEST    0x10 	//查询模块固件版本
#define HCI_CMD_BT_DISCONNECT      0x11		//断开 BT3.0 连接
#define HCI_CMD_BLE_DISCONNECT     0x12 	//断开 BLE 连接
#define HCI_CMD_SET_COD            0x15 	//设置 COD
#define HCI_CMD_SET_NVRAM          0x26     //下发 NV 数据
#define HCI_CMD_ENTER_SLEEP_MODE   0x27 	//进入睡眠模式
#define HCI_CMD_CONFIRM_GKEY       0x28 	//Numeric Comparison 配对方式中对密钥的比较
#define HCI_CMD_SPP_DATA_COMPLETE  0x29 	//SPP 数据处理完成
#define HCI_CMD_SET_ADV_DATA       0x2A 	//设置 ADV 数据
#define HCI_CMD_POWER_REQ          0x2B 	//查询模块电源电压
#define HCI_CMD_POWER_SET          0x2C 	//读取电源电压功能开关
#define HCI_CMD_PASSKEY_ENTRY      0x30 	//用于 PASSKEY 配对方式中对密钥的比较
#define HCI_CMD_SET_GPIO           0x31 	//设置 GPIO
#define HCI_CMD_READ_GPIO          0x32 	//读取 GPIO 设置


#define HCI_CMD_SET_SLEEP_MODE			0x8B	//设置休眠模式

/********EVENT事件名称 Opcode 操作码描述**/
#define HCI_EVENT_BT_CONNECTED 	   	0x00 	//BT3.0 连接建立
#define HCI_EVENT_BLE_CONNECTED    	0x02 	//BLE 连接建立
#define HCI_EVENT_BT_DISCONNECTED  	0x03 	//BT3.0 连接断开
#define HCI_EVENT_BLE_DISCONNECTED 	0x05 	//BLE 连接断开
#define HCI_EVENT_CMD_COMPLETE     	0x06 	//命令已完成
#define HCI_EVENT_SPP_DATA_RECEIVED 0x07 	//接收到 BT3.0 数据（SPP）
#define HCI_EVENT_BLE_DATA_RECEIVED 0x08 	//接收到 BLE 数据
#define HCI_EVENT_I_AM_READY 				0x09 	//模块准备好
#define HCI_EVENT_STATUS_RESPONSE 	0x0A 	//状态回复
#define HCI_EVENT_NVRAM_CHANGED 		0x0D 	//上传 NVRAM 数据
#define HCI_EVENT_UART_EXCEPTION 		0x0F 	//HCI 包格式错误
#define HCI_EVENT_GKEY 							0x0E 	//发送 Numeric Comparison 配对方式中产生的密钥
#define HCI_EVENT_GET_PASSKEY 			0x10 	//PASSKEY 配对方式中通知MCU 返回密钥
#define HCI_EVENT_PAIRING_COMPLETED 0x11
#define HCI_EVENT_REMOTE_MTU        0x12
#define HCI_EVENT_I_NEED_RESTART    0x13	//蓝牙重启请求

#define PARING_MODE_PINCODE		0x00
#define PARING_MODE_JUSTWORK	0x01
#define PARING_MODE_PASSKEY		0x02
#define PARING_MODE_CONFIRM		0x03


int bt42_bluetooth_init(void);
void bt42_loopback_test(void);

#endif
