/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : usbd_conf.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 21-October-2014
 * Description          : USB Device configuration file
 *****************************************************************************/
 

#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

#define USBD_CFG_MAX_NUM           1
#define USBD_ITF_MAX_NUM           1

#define USB_MAX_STR_DESC_SIZ       64 

#define USBD_SELF_POWERED               

#define USBD_DYNAMIC_DESCRIPTOR_CHANGE_ENABLED 

/** @defgroup USB_String_Descriptors
  * @{
  */ 


/** @defgroup USB_HID_Class_Layer_Parameter
  * @{
  */ 
#define HID_IN_EP                    0x81
#define HID_OUT_EP                   0x01

#define HID_IN_PACKET                8
#define HID_OUT_PACKET               8

/**
  * @}
  */ 

#endif //__USBD_CONF__H__


/************************ (C) COPYRIGHT 2014 Megahuntmicro ****END OF FILE****/
