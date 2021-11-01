/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : usbh_conf.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 21-October-2014
 * Description          : General low level driver configuration
 *****************************************************************************/
 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_CONF__H__
#define __USBH_CONF__H__

/* Includes ------------------------------------------------------------------*/

/** @addtogroup USBH_OTG_DRIVER
  * @{
  */
  
/** @defgroup USBH_CONF
  * @brief usb otg low level driver configuration file
  * @{
  */ 

/** @defgroup USBH_CONF_Exported_Defines
  * @{
  */ 

#define USBH_MAX_NUM_ENDPOINTS                2
#define USBH_MAX_NUM_INTERFACES               2
#define USBH_MSC_MPS_SIZE                     0x40

/**
  * @}
  */ 


#endif //__USBH_CONF__H__


/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT 2014 Megahuntmicro ****END OF FILE****/
