/**
  ******************************************************************************
  * @file    usb_bsp.c
  * @author  Megahuntmicro
  * @version V1.0.0
  * @date    21-October-2014
  * @brief   This file implements the board support package for the USB host library
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usb_bsp.h"
#include "usb_conf.h"


/** @defgroup USB_BSP_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USB_BSP_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_BSP_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_BSP_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USB_BSP_Private_Functions
  * @{
  */ 

/**
  * @brief  USB_OTG_BSP_Init
  *         Initilizes BSP configurations
  * @param  None
  * @retval None
  */

void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
	GPIO_PinRemapConfig(GPIOD, GPIO_Pin_0, GPIO_Remap_0);
}
/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{

}

/**
  * @brief  BSP_Drive_VBUS
  *         Drives the Vbus signal through IO
  * @param  state : VBUS states
  * @retval None
  */

void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
	
}

/**
  * @brief  USB_OTG_BSP_ConfigVBUS
  *         Configures the IO for the Vbus and OverCurrent
  * @param  None
  * @retval None
  */

void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
	USB_OTG_BSP_mDelay(200);   /* Delay is need for stabilising the Vbus Low 
						        in Reset Condition, when Vbus=1 and Reset-button is pressed by user */

}

/**
* @brief  USBID_detc_CMD 
*         Control USB ID line Sensor state
* @param  NewState: new state of sensor of USB ID line.
*         This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void USBID_detc_CMD(FunctionalState NewState)
{
    USBPHY_CR3_TypeDef usbphy_cr3;
    usbphy_cr3.d32 = MHSCPU_READ_REG32(&SYSCTRL->USBPHY_CR3);
    if (NewState == ENABLE)
    {
        usbphy_cr3.b.idpullup = 1;
        MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR3,usbphy_cr3.d32);
    }else{
        usbphy_cr3.b.idpullup = 0;
        MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR3,usbphy_cr3.d32);
    }
}

/**
  * @brief  USB_OTG_BSP_TimeInit
  *         Initializes delay unit using Timer2
  * @param  None
  * @retval None
  */
static void USB_OTG_BSP_TimeInit ( void )
{

}

/**
  * @brief  USB_OTG_BSP_uDelay
  *         This function provides delay time in micro sec
  * @param  usec : Value of delay required in micro sec
  * @retval None
  */
void USB_OTG_BSP_uDelay (const uint32_t usec)
{
	__IO uint32_t count = 0;
	const uint32_t utime = (120 * usec / 7);
	
	do
	{
		if ( ++count > utime )
		{
			return ;
		}
	}
	while (1);
}

/**
  * @brief  USB_OTG_BSP_mDelay
  *          This function provides delay time in milli sec
  * @param  msec : Value of delay required in milli sec
  * @retval None
  */
void USB_OTG_BSP_mDelay (const uint32_t msec)
{ 
    USB_OTG_BSP_uDelay(msec * 1000); 
}


/**
* @}
*/ 

/**
* @}
*/


/************************ (C) COPYRIGHT 2014 Megahuntmicro ****END OF FILE****/
