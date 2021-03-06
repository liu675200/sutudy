/**
 ****************************************************************
 * @file iso14443b.c
 *
 * @brief  iso1443b protocol driver
 *
 * @author 
 *
 * 
 ****************************************************************
 */ 

/*
 * INCLUDE FILES
 ****************************************************************
 */	 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"
#include "uart.h"
#include "timer.h"
#include "mh523.h"
#include "iso14443b.h" 
 
extern u8 XDATA g_fwi;//frame waiting time integer

u8 XDATA g_fwi = 4;//frame waiting time integer


//////////////////////////////////////////////////////////////////////
// 函数原型:    char pcd_request_b(u8 req_code, u8 AFI, u8 N, u8 *ATQB)
// 函数功能:    B型卡请求
// 入口参数:    req_code				// 请求代码	ISO14443_3B_REQIDL 0x00 -- 空闲的卡
//										//			ISO14443_3B_REQALL 0x08 -- 所有的卡
//				AFI						// 应用标识符，0x00：全选
//				N						// 时隙总数,取值范围0--4。
// 出口参数:    *ATQB					// 请求应答，11字节
// 返 回 值:    STATUS_SUCCESS -- 成功；其它值 -- 失败。
// 说    明:	-   
//////////////////////////////////////////////////////////////////////
int pcd_request_b(u8 req_code, u8 AFI, u8 N, u8 *ATQB)
{
	int  status;
	
	transceive_buffer XDATA *pi;
    pi=&mf_com_data;

#if (NFC_DEBUG)
	printf("REQB/WUPB:\n");
#endif

	pcd_set_tmo(5);

	mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length = 3;
    mf_com_data.mf_data[0] = ISO14443B_ANTICOLLISION;     	       // APf code
    mf_com_data.mf_data[1] = AFI;                // 
    mf_com_data.mf_data[2] = (req_code & 0x08) | (N&0x07);  // PARAM
 
	status = pcd_com_transceive(pi);

    if (status!=MI_OK && status!=MI_NOTAGERR)
    {   
		status = MI_COLLERR;   
	}
    if (status == MI_OK && mf_com_data.mf_length != 96)
    {   
		status = MI_COM_ERR;   
	}
    if (status == MI_OK) 
    {	
    	memcpy(ATQB, &mf_com_data.mf_data[0], 16);
        pcd_set_tmo(ATQB[11]>>4); // set FWT 
        g_fwi = (ATQB[11]>>4);
    }
#if (NFC_DEBUG)
	printf(" sta=%bd\n", status);
#endif
    return status;
}                      

//////////////////////////////////////////////////////////////////////
//SLOT-MARKER
//////////////////////////////////////////////////////////////////////
int pcd_slot_marker(u8 N, u8 *ATQB)
{
    int status;
	
	transceive_buffer XDATA *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
	printf("SLOT:\n");
#endif
	pcd_set_tmo(5);

    if(!N || N>15)
	{
		status = MI_WRONG_PARAMETER_VALUE;	
    }
	else
    {
		mf_com_data.mf_command = PCD_TRANSCEIVE;
		mf_com_data.mf_length = 1;
		mf_com_data.mf_data[0] = 0x05 |(N << 4); // APn code

		status = pcd_com_transceive(pi);

	    if (status != MI_OK && status != MI_NOTAGERR)
	    {   
			status = MI_COLLERR;   
		}
	    if (status == MI_OK && mf_com_data.mf_length != 96)
	    {   
			status = MI_COM_ERR;   
		}
		if (status == MI_OK) 
		{	
		  memcpy(ATQB, &mf_com_data.mf_data[0], 16);
		  pcd_set_tmo(ATQB[11]>>4); // set FWT 
		  g_fwi = ATQB[11]>>4;
		} 	
    }
	
    return status;
}                      

             

//////////////////////////////////////////////////////////////////////
//ATTRIB
// 函数原型:    INchar pcd_attrib(u8 *PUPI, u8 pro_type, u8 CID, u8 *answer)
//
// 函数功能:    选择PICC
// 入口参数:    u8 *PUPI					// 4字节PICC标识符
//				u8 dsi_dri					// PCD<-->PICC 速率选择
//				u8 pro_type					// 支持的协议，由请求回应中的ProtocolType指定
// 返 回 值:    MI_OK -- 成功；其它值 -- 失败。
// 说    明:	-
//////////////////////////////////////////////////////////////////////
int pcd_attri_b(u8 *PUPI, u8 dsi_dri, u8 pro_type, u8 CID, u8 *answer)
{
    int  status;
	
	transceive_buffer XDATA *pi;
    pi = &mf_com_data;
	pro_type = pro_type;

#if (NFC_DEBUG)
	printf("ATTRIB:\n");
#endif
	
	pcd_set_tmo(g_fwi);

    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 9;
    mf_com_data.mf_data[0] = ISO14443B_ATTRIB;
    memcpy(&mf_com_data.mf_data[1], PUPI, 4);
	//mf_com_data.mf_data[1] = 0x00;
	//mf_com_data.mf_data[2] = 0x00;
	//mf_com_data.mf_data[3] = 0x00;
	//mf_com_data.mf_data[4] = 0x00;
    mf_com_data.mf_data[5] = 0x00;  	    // EOF/SOF required, default TR0/TR1
	//EOFTEST
	//mf_com_data.mf_data[5] = 0x04; //XU //SOF not,EOF yes
	//mf_com_data.mf_data[5] = 0x08; //XU //SOF yes,EOF no
	//mf_com_data.mf_data[5] = 0x0C; //XU //SOF no,EOF no
	
	
    set_bit_mask(TypeBReg, BIT7 | BIT6); //EOF SOF required
    //write_reg(0x1E, 0x13);	//EOF SOF required
    //write_reg(0x1E, 0xD3);	//EOF SOF not required
	
    mf_com_data.mf_data[6] = ((dsi_dri << 4) | FSDI); //FSDI; // Max frame 64 
    mf_com_data.mf_data[7] = 0x01; //pro_type & 0x0f;  //;  ISO/IEC 14443-4 compliant?;
    mf_com_data.mf_data[8] = (CID & 0x0f); //   	    // CID ,0 - 14,若不支持CID，则设置为0000
    
    status  = pcd_com_transceive(pi);

    if (status == MI_OK)
    {	
    	*answer = mf_com_data.mf_data[0];
    } 	

#if (NFC_DEBUG)
    printf(" sta=%bd\n", status);
#endif

    return status;
} 
//////////////////////////////////////////////////////////////////////
//REQUEST B
//////////////////////////////////////////////////////////////////////
int get_idcard_num(u8 *pid)
{
    int XDATA status;
	
	transceive_buffer XDATA *pi;
    pi = &mf_com_data;

#if (NFC_DEBUG)
	printf("ID_NUM:\n");
#endif
	//pcd_set_tmo(5);
	
    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  =5;
    mf_com_data.mf_data[0] =0x00; //ISO14443B_ANTICOLLISION;     	       // APf code
    mf_com_data.mf_data[1] =0x36;// AFI;                // 
    mf_com_data.mf_data[2] =0x00; //((req_code<<3)&0x08) | (N&0x07);  // PARAM
	mf_com_data.mf_data[3] =0x00;
	mf_com_data.mf_data[4] =0x08;
 
    status = pcd_com_transceive(pi);

    if (status == MI_OK) 
    {	
    	memcpy(pid, &mf_com_data.mf_data[0], 10);
        //pcd_set_tmo(ATQB[11]>>4); // set FWT 
    } 
    return status;
}       

//////////////////////////////////////////////////////////////////////
// 函数原型:    char pcd_halt_b(u8 *PUPI)
// 函数功能:    挂起卡
// 入口参数:    INT8U *pPUPI					// 4字节PICC标识符
// 出口参数:    -
// 返 回 值:    MI_OK -- 成功；其它值 -- 失败。//////////////////////////////////////////////////////////////////////
int pcd_halt_b(u8 *PUPI)
{
    int  status;
	
	transceive_buffer XDATA *pi;
    pi = &mf_com_data;
	
#if (NFC_DEBUG)
	printf("HALTB:\n");
#endif
    pcd_set_tmo(g_fwi);
				                               // disable, ISO/IEC3390 enable	
    mf_com_data.mf_command = PCD_TRANSCEIVE;
    mf_com_data.mf_length  = 5;
    mf_com_data.mf_data[0] = ISO14443B_ATTRIB;
    memcpy(&mf_com_data.mf_data[1], PUPI, 4);
    
    status = pcd_com_transceive(pi);
#if (NFC_DEBUG)
	printf(" sta=%bd\n", status);
#endif

    return status;
}   





                                      



