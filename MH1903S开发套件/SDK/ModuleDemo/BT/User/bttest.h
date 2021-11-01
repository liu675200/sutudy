#ifndef BTTEST_H
#define BTTEST_H



/*********Packet type*******************/
#define CMD   0x01  
#define EVENT 0x02

#define CMD_SUCCESS	0x00
#define CMD_FAIL	0x01

/*********CMD�������� Opcode ����������***/
#define HCI_CMD_SET_BT_ADDR        0x00     //���� BT3.0 ��ַ
#define HCI_CMD_SET_BLE_ADDR       0x01     //���� BLE ��ַ
#define HCI_CMD_SET_VISIBILITY     0x02     //���ÿɷ��ֺ͹㲥
#define HCI_CMD_SET_BT_NAME        0x03     //���� BT3.0 ����
#define HCI_CMD_SET_BLE_NAME       0x04     //���� BLE ����
#define HCI_CMD_SEND_SPP_DATA      0x05     // ���� BT3.0��SPP������
#define HCI_CMD_SEND_BLE_DATA      0x09 	//���� BLE ����
#define HCI_CMD_SEND_DATA          0x0A 	//�������ݣ��Զ�ѡ��ͨ����
#define HCI_CMD_STATUS_REQUEST     0x0B 	//��������״̬
#define HCI_CMD_SET_PAIRING_MODE   0x0C 	//�������ģʽ
#define HCI_CMD_SET_PINCODE        0x0D 	//���������
#define HCI_CMD_SET_UART_FLOW      0x0E 	//���� UART ����
#define HCI_CMD_SET_UART_BAUD      0x0F 	//���� UART ������
#define HCI_CMD_VERSION_REQUEST    0x10 	//��ѯģ��̼��汾
#define HCI_CMD_BT_DISCONNECT      0x11		//�Ͽ� BT3.0 ����
#define HCI_CMD_BLE_DISCONNECT     0x12 	//�Ͽ� BLE ����
#define HCI_CMD_SET_COD            0x15 	//���� COD
#define HCI_CMD_SET_NVRAM          0x26     //�·� NV ����
#define HCI_CMD_ENTER_SLEEP_MODE   0x27 	//����˯��ģʽ
#define HCI_CMD_CONFIRM_GKEY       0x28 	//Numeric Comparison ��Է�ʽ�ж���Կ�ıȽ�
#define HCI_CMD_SPP_DATA_COMPLETE  0x29 	//SPP ���ݴ������
#define HCI_CMD_SET_ADV_DATA       0x2A 	//���� ADV ����
#define HCI_CMD_POWER_REQ          0x2B 	//��ѯģ���Դ��ѹ
#define HCI_CMD_POWER_SET          0x2C 	//��ȡ��Դ��ѹ���ܿ���
#define HCI_CMD_PASSKEY_ENTRY      0x30 	//���� PASSKEY ��Է�ʽ�ж���Կ�ıȽ�
#define HCI_CMD_SET_GPIO           0x31 	//���� GPIO
#define HCI_CMD_READ_GPIO          0x32 	//��ȡ GPIO ����


#define HCI_CMD_SET_SLEEP_MODE			0x8B	//��������ģʽ

/********EVENT�¼����� Opcode ����������**/
#define HCI_EVENT_BT_CONNECTED 	   	0x00 	//BT3.0 ���ӽ���
#define HCI_EVENT_BLE_CONNECTED    	0x02 	//BLE ���ӽ���
#define HCI_EVENT_BT_DISCONNECTED  	0x03 	//BT3.0 ���ӶϿ�
#define HCI_EVENT_BLE_DISCONNECTED 	0x05 	//BLE ���ӶϿ�
#define HCI_EVENT_CMD_COMPLETE     	0x06 	//���������
#define HCI_EVENT_SPP_DATA_RECEIVED 0x07 	//���յ� BT3.0 ���ݣ�SPP��
#define HCI_EVENT_BLE_DATA_RECEIVED 0x08 	//���յ� BLE ����
#define HCI_EVENT_I_AM_READY 				0x09 	//ģ��׼����
#define HCI_EVENT_STATUS_RESPONSE 	0x0A 	//״̬�ظ�
#define HCI_EVENT_NVRAM_CHANGED 		0x0D 	//�ϴ� NVRAM ����
#define HCI_EVENT_UART_EXCEPTION 		0x0F 	//HCI ����ʽ����
#define HCI_EVENT_GKEY 							0x0E 	//���� Numeric Comparison ��Է�ʽ�в�������Կ
#define HCI_EVENT_GET_PASSKEY 			0x10 	//PASSKEY ��Է�ʽ��֪ͨMCU ������Կ
#define HCI_EVENT_PAIRING_COMPLETED 0x11
#define HCI_EVENT_REMOTE_MTU        0x12
#define HCI_EVENT_I_NEED_RESTART    0x13	//������������

#define PARING_MODE_PINCODE		0x00
#define PARING_MODE_JUSTWORK	0x01
#define PARING_MODE_PASSKEY		0x02
#define PARING_MODE_CONFIRM		0x03


int bt42_bluetooth_init(void);
void bt42_loopback_test(void);

#endif
