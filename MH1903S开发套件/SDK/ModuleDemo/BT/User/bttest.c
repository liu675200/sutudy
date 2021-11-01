#include <stdio.h>
#include <bttest.h>
#include "mhscpu.h"
#include "bt42_api.h"
#include "mhscpu_exti.h"


#define MAX_INIT_TIMEOUT_MS 		4000		//��ʼ���ȴ�ʱ�䳬ʱ
#define MAX_CMD_TIMEOUT_MS			4000		//����ȴ���ʱʱ��


static char bt_name[]="ws8200-br";	//���õ�BR����
static char ble_name[]="ws8200-le";	//���õ�BLE����
static uint8_t bt_addr[6]={0xc2, 0xc0, 0x67, 0x77, 0x52, 0xc1};//br��ַ
static uint8_t ble_addr[6]={0xc4, 0x21, 0x56, 0xd5, 0xc2, 0xcc};//ble��ַ



static uint8_t g_bt_is_ready = 0; //�����Ƿ�ready
static uint8_t g_cmd_is_complete = 0; //���������Ƿ����


int bt42_local_config(void);
int bt42_wait_cmd_complete(uint8_t event, uint32_t tmo_ms);



void EXTI4_IRQHandler(void)
{
	//< ���߻���bt�ܽţ��ȴ������������ݸ�mcu
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
*	����pin�ĳ�ʼ��
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

	//< �½��ش����ж�
	EXTI_LineConfig(EXTI_Line4, EXTI_PinSource7, EXTI_Trigger_Falling);
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
}

//�����ϵ�
void bt42_bluetooth_poweron()
{
	//< RST_PIN = 0;	
	BT_RST_CLR();
	soft_msleep(10);
	//< RST_PIN = 1;
	BT_RST_SET();
	soft_msleep(10);
}

//������ʼ������
int bt42_bluetooth_init(void)
{
	int ret;
	
	bt_pin_init();//��ʼ���������ӵ�����״̬
	
	bt42_bluetooth_poweron();//�����ϵ�
	mcu_wake_bt_pin(1); //��֤��ʼ��ǰmcu_wake_bt��Ϊ��
	
	bt42_init();				//���Э��ջ��ʼ��
	ret = bt42_wait_cmd_complete(HCI_EVENT_I_AM_READY, MAX_INIT_TIMEOUT_MS);//�ȴ�������ʼ������ϱ��û�ready�¼�

	if(ret == 0)
	{
		hal_print("ready...");
		ret = bt42_local_config();//��ɱ��س�ʼ�����ã�����,��ַ,�ɷ���ģʽ�ȵȣ�
	}
	
	return ret;
}
//�ͻ�event��������
void bttest_event_process(char *event, uint8_t len)
{
	//i_am_ready�¼�
	if (event[1] == HCI_EVENT_I_AM_READY && len == 3)
	{
		g_bt_is_ready = 1;
		g_cmd_is_complete = 0;
	}
	//��������¼�
	else if (event[1] == HCI_EVENT_CMD_COMPLETE)
	{
		g_cmd_is_complete = 1;
	}
	//i_need_restart�¼�
	else if (event[1] == HCI_EVENT_I_NEED_RESTART)
	{
		
	}
	//BR�����¼�����
	else if (event[1] == HCI_EVENT_BT_CONNECTED)
	{
		
	}
	//BLE�����¼�����
	else if (event[1] == HCI_EVENT_BLE_CONNECTED)
	{
		
	}
	//BR�Ͽ��¼�����
	else if (event[1] == HCI_EVENT_BT_DISCONNECTED)
	{
		
	}
	//BLE�Ͽ��¼�����
	else if (event[1] == HCI_EVENT_BLE_DISCONNECTED)
	{
		
	}
	//�յ�BLE����
	else if (event[1] == HCI_EVENT_BLE_DATA_RECEIVED)
	{
		
	}
	//�յ�BR����
	else if (event[1] == HCI_EVENT_SPP_DATA_RECEIVED)
	{
		
	}
	//nvram�¼�����-�ͻ���Ҫ�����ݴ洢��flash��ȷ���´β������������
	else if (event[1] == HCI_EVENT_NVRAM_CHANGED)
	{
		
	}
	
}

//�ȴ���ʱ����
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

	//����BT3.0��������
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BT_NAME;
		local_cmd->length = strlen(bt_name);
		memcpy(local_cmd->payload, bt_name, strlen(bt_name));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//����BLE��������
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BLE_NAME;
		local_cmd->length = strlen(ble_name);
		memcpy(local_cmd->payload, ble_name, strlen(ble_name));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//����BT3.0������ַ
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BT_ADDR;
		local_cmd->length = sizeof(bt_addr);
		memcpy(local_cmd->payload, bt_addr, sizeof(bt_addr));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//����BLE������ַ
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_BLE_ADDR;
		local_cmd->length = sizeof(ble_addr);
		memcpy(local_cmd->payload, ble_addr, sizeof(ble_addr));
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//�������ģʽ
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_PAIRING_MODE;
		local_cmd->length = 1;
		local_cmd->payload[0] = PARING_MODE_JUSTWORK;
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	
	//���������ɼ�ģʽ 
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_VISIBILITY;
		local_cmd->length = 1;
		local_cmd->payload[0] = 0x07; //LE�ɷ��ֿ����ӣ�BT3.0�ɷ��ֿ�����
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);
	}
	//ʹ��Ӳ������-�Ǳ�ѡ��
	if (ret == 0)
	{
		local_cmd->packet_type = CMD;
		local_cmd->opcode = HCI_CMD_SET_UART_FLOW;
		local_cmd->length = 1;
		local_cmd->payload[0] = 0x01; //LE�ɷ��ֿ����ӣ�BT3.0�ɷ��ֿ�����
		bt42_api_cmd((uint8_t *)local_cmd, local_cmd->length + 3);
		ret = bt42_wait_cmd_complete(HCI_EVENT_CMD_COMPLETE, MAX_CMD_TIMEOUT_MS);//�ȴ���������
	}
	
	return ret;
}
