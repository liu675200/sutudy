/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : usb_conf.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 21-October-2014
 * Description          : General low level driver configuration
 *****************************************************************************/
 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF__H__
#define __USB_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "mhscpu.h"

/*******************************************************************************
*                     FIFO Size Configuration in Host mode
*  
*  (i) Receive data FIFO size = (Largest Packet Size / 4) + 1 or 
*                             2x (Largest Packet Size / 4) + 1,  If a 
*                             high-bandwidth channel or multiple isochronous 
*                             channels are enabled
*
*  (ii) For the host nonperiodic Transmit FIFO is the largest maximum packet size 
*      for all supported nonperiodic OUT channels. Typically, a space 
*      corresponding to two Largest Packet Size is recommended.
*
*  (iii) The minimum amount of RAM required for Host periodic Transmit FIFO is 
*        the largest maximum packet size for all supported periodic OUT channels.
*        If there is at least one High Bandwidth Isochronous OUT endpoint, 
*        then the space must be at least two times the maximum packet size for 
*        that channel.
*******************************************************************************/

/****************** USB OTG MODE CONFIGURATION ********************************/
#define USE_HOST_MODE

/****************** C Compilers dependant keywords ****************************/
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */    
#define __ALIGN_BEGIN
#define __ALIGN_END   

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __packed    __packed
#elif defined (__ICCARM__)     /* IAR Compiler */
  #define __packed    __packed
#elif defined   ( __GNUC__ )   /* GNU Compiler */                        
  #define __packed    __attribute__ ((__packed__))
#elif defined   (__TASKING__)  /* TASKING Compiler */
  #define __packed    __unaligned
#endif /* __CC_ARM */


/****************** USB OTG FS CONFIGURATION **********************************/
// total fifo size <= 448 (512 - 64)
#define RX1_FIFO_FS_SIZE                         	64
#define RX2_FIFO_FS_SIZE                          	64
#define RX3_FIFO_FS_SIZE                          	64
#define RX4_FIFO_FS_SIZE                          	64
#define RX5_FIFO_FS_SIZE                          	64
#define RX6_FIFO_FS_SIZE                          	64
#define RX7_FIFO_FS_SIZE                          	64

#define TX1_FIFO_FS_SIZE                         	64
#define TX2_FIFO_FS_SIZE                          	64
#define TX3_FIFO_FS_SIZE                          	64
#define TX4_FIFO_FS_SIZE                          	64
#define TX5_FIFO_FS_SIZE                          	64
#define TX6_FIFO_FS_SIZE                          	64
#define TX7_FIFO_FS_SIZE                          	64

#define TX0_FIFO_FS_SIZE                           64
#define RX0_FIFO_FS_SIZE                           64

#define IS_FIFO_SIZE(size)          (((size) == 0)   || \
                                     ((size) == 8)   || \
                                     ((size) == 16)  || \
                                     ((size) == 32)  || \
                                     ((size) == 64)  || \
                                     ((size) == 128) || \
                                     ((size) == 256) || \
                                     ((size) == 512)) 

#endif //__USB_CONF__H__


/************************ (C) COPYRIGHT 2014 Megahuntmicro ****END OF FILE****/
