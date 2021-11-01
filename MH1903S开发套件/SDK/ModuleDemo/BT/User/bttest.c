#include <stdio.h>
#include <bttest.h>
#include "mhscpu.h"
#include "bt42_api.h"
#include "mhscpu_exti.h"


#define MAX_INIT_TIMEOUT_MS 		4000		//初始化等待时间超时
#define MAX_CMD_TIMEOUT_MS			4000		//命令等待超时时间


static char bt_name[]="ws8200-br";	//设置的BR名字
static char ble_name[]="ws8200-le";	//设置的BLE名字
static uint8_t bt_addr[6]={0xc2, 0xc0, 0x67, 0x77, 0x52, 0xc1};//br地址
static uint8_t ble_addr[6]={0xc4, 0x21, 0x56, 0xd5, 0xc2, 0xcc};//ble地址



static uint8_t g_bt_is_ready = 0; //蓝牙是否ready
static uint8_t g_cmd_is_complete = 0; //蓝牙命令是否完成


int bt42_local_config(void);
int bt42_wait_cmd_complete(uint8_t event, uint32_t tmo_ms);



void EXTI4_IRQHandler(void)
{
	//< 拉高唤醒bt管脚，等待蓝牙发送数据给mcu
	mcu_wake_bt_pin(1);
	EXTI_ClearITPendingBit(EXTI_Line4);
	NVIC_ClearPendingIRQ(EXTI4_IRQn);
}

void soft_msleep(int ms)
{
	uint32_t i , j;
	
	for (i = 0; i < ms; i++)
	{
		for (j = 0; j < 45000; j++);
	}
}

/*
*	连接pin的初始化
*/
void bt_pin_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	//< BT_RST
	GPIO_InitStruct.GPIO_Pin = BT_RSTN_IO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(BT_RSTN_IO_GROUP, &GPIO_InitStruct);

	//< BT_WAKE_MCU
	GPIO_InitStruct.GPIO_Pin = BT_BTWAKEUPMCU_IO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(BT_BTWAKEUPMCU_IO_GROUP, &GPIO_InitStruct);
	
	//< MCU_WAKE_BT
	GPIO_InitStruct.GPIO_Pin = BT_MCUWAKEUPBT_IO_PIN; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(BT_MCUWAKEUPBT_IO_GROUP, &GPIO_InitStruct);

	//< 下降沿触发中断
	EXTI_LineConfig(EXTI_Line4, EXTI_PinSource7, EXTI_Trigger_Falling);
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
}

//蓝牙上电
void bt42_bluetooth_poweron()
{
	//< RST_PIN = 0;	
	BT_RST_CLR();
	soft_msleep(10);
	//< RST_PIN = 1;
	BT_RST_SET();
	soft_msleep(10);
}

//蓝牙初始化配置
int bt42_bluetooth_init(void)
{
	int ret;
	
	bt_pin_init();//初始化蓝牙连接的引脚状态
	
	bt42_bluetooth_poweron();//蓝牙上电
	mcu_wake_bt_pin(1); //保证初始化前mcu_wake_bt脚为高
	
	bt42_init();				//完成协议栈初始化
	ret = bt42_wait_cmd_complete(HCI_EVENT_I_AM_READY, MAX_INIT_TIMEOUT_MS);//等待蓝牙初始化完成上报用户ready事件

	if(ret == 0)
	{
		hal_print("ready...");
		ret = bt42_local_config();//完成本地初始化配置（名字,地址,可发现模式等等）
	}
	
	return ret;
}
//客户event函数处理
void bttest_event_process(char *event, uint8_t len)
{
	//i_am_ready事件
	if (event[1] == HCI_EVENT_I_AM_READY && len == 3)
	{
		g_bt_is_ready = 1;
		g_cmd_is_complete = 0;
	}
	//命令完成事件
	else if (event[1] == HCI_EVENT_CMD_COMPLETE)
	{
		g_cmd_is_complete = 1;
	}
	//i_need_restart事件
	else if (event[1] == HCI_EVENT_I_NEED_RESTART)
	{
		
	}
	//BR连接事件处理
	else if (event[1] == HCI_EVENT_BT_CONNECTED)
	{
		
	}
	//BLE连接事件处理
	else if (event[1] == HCI_EVENT_BLE_CONNECTED)
	{
		
	}
	//BR断开事件处理
	else if (event[1] == HCI_EVENT_BT_DISCONNECTED)
	{
		
	}
	//BLE断开事件处理
	else if (event[1] == HCI_EVENT_BLE_DISCONNECTED)
	{
		
	}
	//收到BLE数据
	else if (event[1] == HCI_EVENT_BLE_DATA_RECEIVED)
	{
		
	}
	//收到BR数据
	else if (event[1] == HCI_EVENT_SPP_DATA_RECEIVED)
	{
		
	}
	//nvram事件处理-客户需要把数据存储在flash中确保下次不会走配对流程
	else if (event[1] == HCI_EVENT_NVRAM_CHANGED)
	{
		
	}
	
}

//等待超时函数
int bt42_wait_cmd_complete(uint8_t event, uint32_t tmo_ms)
{
	uint32_t t;

	t = hal_time_ms();
	
	do {
		
		bt42_run_loop(1);
		
		switch(event)
		{
			case HCI_EVENT_I_AM_READY:
			{
				if (g_bt_is_ready == 1)
				{
					g_bt_is_ready = 0;
					return 0;
				}
				break;
			}
			case HCI_EVENT_CMD_COMPLETE:
			{
				if (g_cmd_is_complete == 1)
				{
					g_cmd_is_complete = 0;
					return 0;
				}
				break;
			}
		}
	}while(0 == bt42_is_timeout(t, tmo_ms));
	return -1;
}



int bt42_local_config(void)
{
	int ret = 0;
	uint8_t cmd_buf[300];
	struct_bt42_hci *local_cmd = (struct_bt42_hci *)cmd_buf;

	//设置BT3.0蓝牙名字
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BT_NAME;
		local_cmd->length = strlen(bt_name);
		memcpy(local_cmd->payload, bt_name, strlen(bt_name));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//设置BLE蓝牙名字
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BLE_NAME;
		local_cmd->length = strlen(ble_name);
		memcpy(local_cmd->payload, ble_name, strlen(ble_name));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//设置BT3.0蓝牙地址
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BT_ADDR;
		local_cmd->length = sizeof(bt_addr);
		memcpy(local_cmd->payload, bt_addr, sizeof(bt_addr));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//设置BLE蓝牙地址
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BLE_ADDR;
		local_cmd->length = sizeof(ble_addr);
		memcpy(local_cmd->payload, ble_addr, sizeof(ble_addr));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//设置配对模式
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_PAIRING_MODE;
		local_cmd->length = 1;
		local_cmd->payload[0] = PARING_MODE_JUSTWORK;
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	
	//设置蓝牙可见模式 
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_VISIBILITY;
		local_cmd->length = 1;
		local_cmd->payload[0] = 0x07; //LE可发现可连接，BT3.0可发现可连接
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//使能硬件流控-非必选项
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_UART_FLOW;
		local_cmd->length = 1;
		local_cmd->payload[0] = 0x01; //LE可发现可连接，BT3.0可发现可连接
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);//先打开蓝牙流控
	}
	
	return ret;
}
