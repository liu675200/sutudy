/**
 ****************************************************************
 * @file iso14443b.h
 *
 * @brief 
 *
 * @author 
 *
 *
 ****************************************************************
 */
#ifndef ISO_14443B_H
#define ISO_14443B_H
/////////////////////////////////////////////////////////////////////
#include "define.h"

//ISO14443B COMMAND
///////////////////////////////////////////////////////////////////// 
#define	ISO14443B_ANTICOLLISION                  0x05
#define	ISO14443B_ATTRIB                         0x1D
#define	ISO14443B_HLTB                           0x50


/////////////////////////////////////////////////////////////////////
//����ԭ��
/////////////////////////////////////////////////////////////////////
int pcd_request_b(u8 req_code, u8 AFI, u8 N, u8 *ATQB);
int pcd_slot_marker(u8 N, u8 *ATQB);
int pcd_attri_b(u8 *PUPI, u8 dsi_dri, u8 pro_type,u8 CID, u8 *answer);
int get_idcard_num(u8 *ATQB);
int pcd_halt_b(u8 *PUPI);
int select_sr(u8 *chip_id);
int read_sr176(u8 addr, u8 *readdata);
int write_sr176(u8 addr, u8 *writedata);
int get_prot_sr176(u8 lockreg);
int protect_sr176(u8 lockreg);
int completion_sr(void);
int pcd_change_rate_b(u8 CID, u8 *ATQB);



#endif
