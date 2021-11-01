/**
 ****************************************************************
 * @file mifare.h
 *
 * @brief 
 *
 * @author 
 *
 * 
 ****************************************************************
 */ 
#ifndef MIFARE_H
#define MIFARE_H

#include "define.h"

#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_RESET            0xE0               //��λ


//UltraLight
#define PICC_WRITE_ULTRALIGHT 0xA2				//���Ῠд��

                                  
int pcd_auth_state(u8 auth_mode, u8 block, u8 *psnr, u8 *pkey);      
int pcd_read(u8 addr,u8 *preaddata);                       
int pcd_write(u8 addr,u8 *pwritedata);
int pcd_write_ultralight(u8 addr,u8 *pwritedata);

#endif
