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


/** @addtogroup USB_OTG_DRIVER
  * @{
  */
  
/** @defgroup USB_CONF
  * @brief USB low level driver configuration file
  * @{
  */ 

/** @defgroup USB_CONF_Exported_Defines
  * @{
  */ 

/* USB Core and PHY interface configuration.
   Tip: To avoid modifying these defines each time you need to change the USB
        configuration, you can declare the needed define in your toolchain
        compiler preprocessor.
   */
/****************** USB OTG FS PHY CONFIGURATION *******************************
*  The USB OTG FS Core supports one on-chip Full Speed PHY.
*******************************************************************************/

/*******************************************************************************
*                      FIFO Size Configuration in Device mode
*  
*  (i) Receive data FIFO size = RAM for setup packets + 
*                   OUT endpoint control information +
*                   data OUT packets + miscellaneous
*      Space = ONE 32-bits words
*     --> RAM for setup packets = 10 spaces
*        (n is the nbr of CTRL EPs the device core supports) 
*     --> OUT EP CTRL info      = 1 space
*        (one space for status information written to the FIFO along with each 
*        received packet)
*     --> data OUT packets      = (Largest Packet Size / 4) + 1 spaces 
*        (MINIMUM to receive packets)
*     --> OR data OUT packets  = at least 2*(Largest Packet Size / 4) + 1 spaces 
*        (if high-bandwidth EP is enabled or multiple isochronous EPs)
*     --> miscellaneous = 1 space per OUT EP
*        (one space for transfer complete status information also pushed to the 
*        FIFO with each endpoint's last packet)
*
*  (ii)MINIMUM RAM space required for each IN EP Tx FIFO = MAX packet size for 
*       that particular IN EP. More space allocated in the IN EP Tx FIFO results
*       in a better performance on the USB and can hide latencies on the AHB.
*
*  (iii) TXn min size = 16 words. (n  : Transmit FIFO index)
*   (iv) When a TxFIFO is not used, the Configuration should be as follows: 
*       case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txm can use the space allocated for Txn.
*       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
*       --> Txn should be configured with the minimum space of 16 words
*  (v) The FIFO is used optimally when used TxFIFOs are allocated in the top 
*       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
*   (vi) In HS case12 FIFO locations should be reserved for internal DMA registers
*        so total FIFO size should be 1012 Only instead of 1024       
*******************************************************************************/

/****************** USB OTG FS CONFIGURATION **********************************/
// total fifo size <= 448 (512 - 64)

#define RX1_FIFO_FS_SIZE                         	0
#define RX2_FIFO_FS_SIZE                          	64
#define RX3_FIFO_FS_SIZE                          	0
#define RX4_FIFO_FS_SIZE                          	0
#define RX5_FIFO_FS_SIZE                          	0
#define RX6_FIFO_FS_SIZE                          	0
#define RX7_FIFO_FS_SIZE                          	0

#define TX1_FIFO_FS_SIZE                         	64
#define TX2_FIFO_FS_SIZE                          	0
#define TX3_FIFO_FS_SIZE                          	0
#define TX4_FIFO_FS_SIZE                          	0
#define TX5_FIFO_FS_SIZE                          	0
#define TX6_FIFO_FS_SIZE                          	0
#define TX7_FIFO_FS_SIZE                          	0

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

/****************** USB OTG MISC CONFIGURATION ********************************/
#define VBUS_SENSING_ENABLED

/****************** USB OTG MODE CONFIGURATION ********************************/
//#define USE_HOST_MODE
#define USE_DEVICE_MODE

/* Only one endpoind can be enable */
#define USB_OTG_DEV_DMA_EP_NUM              0x81
/****************** C Compilers dependant keywords ****************************/
#define __ALIGN_BEGIN         __align(4) 
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

/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USB_CONF_Exported_FunctionsPrototype
  * @{
  */ 
/**
  * @}
  */ 


#endif //__USB_CONF__H__


/**
  * @}
  */ 

/**
  * @}
  */ 

/**************************      (C) COPYRIGHT Megahunt    *****END OF FILE****/

