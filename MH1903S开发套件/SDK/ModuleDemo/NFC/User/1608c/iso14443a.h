/**
 ****************************************************************
 * @file iso14443a.h
 *
 * @brief 
 *
 * @author 
 *
 * 
 ****************************************************************
 */
#ifndef ISO14443A_H
#define ISO14443A_H
 
#include "define.h"


/**
 * DEFINES ISO14443A COMMAND
 * commands which are handled by the tag,Each tag command is written to the
 * reader IC and transfered via RF
 ****************************************************************
 */
 
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_ANTICOLL3		  0x97				 //����ײ
#define PICC_HLTA             0x50               //����

/*
 * FUNCTION DECLARATIONS
 ****************************************************************
 */
 
int pcd_request(u8 req_code, u8 *ptagtype);
int pcd_cascaded_anticoll(u8 select_code, u8 coll_position, u8 *psnr);
int pcd_cascaded_select(u8 select_code, u8 *psnr,u8 *psak);
int pcd_hlta(void);
int pcd_rats_a(u8 CID, u8 *ats);

int pcd_pps_rate(transceive_buffer *pi, u8 *ATS, u8 CID, u8 rate);
#endif
