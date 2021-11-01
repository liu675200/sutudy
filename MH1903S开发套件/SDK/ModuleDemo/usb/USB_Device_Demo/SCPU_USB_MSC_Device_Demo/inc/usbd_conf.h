/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : usbd_conf.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 21-October-2014
 * Description          : USB Device configuration file
 *****************************************************************************/
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

/** @defgroup USBD_CONF_Exported_Defines
  * @{
  */ 


#define USBD_CFG_MAX_NUM           1
#define USBD_ITF_MAX_NUM           1
#define USB_MAX_STR_DESC_SIZ       64 

#define USBD_SELF_POWERED               

/* Class Layer Parameter */

#define MSC_IN_EP                    0x81
#define MSC_OUT_EP                   0x01

#define MSC_MAX_PACKET                64

#define MSC_MEDIA_PACKET             4096

/**
  * @}
  */ 


/** @defgroup USB_CONF_Exported_TypesDefinitions
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

#endif //__USBD_CONF__H__

/************************ (C) COPYRIGHT 2014 Megahuntmicro ****END OF FILE****/

