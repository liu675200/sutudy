 /**
 ****************************************************************
 * @file define.h
 *
 * @brief  define����ƺͲ��ֱ�������
 *
 * @author 
 *
 * 
 ****************************************************************
 */ 
#ifndef DEFINE_H
#define DEFINE_H


/*
 * VESRION DESCRIPTION
 ****************************************************************
 */	

//#define VERSION "1.0.0"
// 1. ���N12 SDK�ı�д
// 2. �������������LPCD���Ժ�COSָ����Թ���
//#define VERSION "1.1.0"
// 1. �Ż�0x3B�Ĵ�������
//#define VERSION "1.2.0"
// 1. �Ż�35/3b�ϵ��ʼ������
//#define VERSION "1.3.0"
// 1. �Ż�SNR
//#define VERSION "1.4.0"
// 1. �Ż�ע��
#define VERSION "1.5.0"
// 1. �Ż�������ͨ������

#define MOTHERBOARD_V20	    1	//�������µװ�V2.0
#define SIKAI_V10		    0	//��������SIKAI���ݵ�С��

#define NFC_DEBUG		    1	// printf��ӡ���ƿ���

#define POWERON_POLLING     1   //�ϵ缴��ʼ�Զ�A/B polling
#define INT_USE_CHECK_REG   1	//�жϼ��ʹ�ò�ѯ07�Ĵ�����irq bit�ķ�ʽ��������ʹ�ò�ѯ�жϹܽ�


/*********************ϵͳʱ������************************/
//#define FOSC 11059200ul
#if (MOTHERBOARD_V20)
	#define FOSC 22118400ul
#else
	#define FOSC 11059200ul
#endif

#if (MOTHERBOARD_V20 + SIKAI_V10 != 1)
	#error("multi board selected!")
#endif

/************************PC���������λ�����ͨ��������**************************/
#define COM_PKT_CMD_READ_REG 		      1
#define COM_PKT_CMD_WRITE_REG	          2

#define COM_PKT_CMD_QUERY_MODE		      0x0D	//�Ƿ����ֶ�ģʽ���������Զ�Ѱ��ģʽ
#define COM_PKT_CMD_CHIP_RESET            0x0E  //��λ����оƬ
#define COM_PKT_CMD_CARD_TYPE		      0x0F
#define COM_PKT_CMD_REQ_SELECT		      0x10
#define COM_PKT_CMD_HALT			      0x11  //����Haltָ��
#define COM_PKT_CMD_STATISTICS		      0x12  //ͳ����Ϣ
#define COM_PKT_CMD_LPCD			      0x13  //LPCD ����
#define COM_PKT_CMD_LPCD_CONFIG_TEST      0x16  //����LPCD����
#define COM_PKT_CMD_LPCD_CONFIG_TEST_STOP 0x17

#define DATA
#define IDATA
#define XDATA
#define CODE
#define REENTRANT

typedef unsigned int  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
typedef unsigned char  bool;


#endif
