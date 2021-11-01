/**
  ******************************************************************************
  * @file    usbh_usr.c
  * @author  Megahuntmicro
  * @version V1.0.0
  * @date    21-October-2014
  * @brief   This file includes the user application layer
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_usr.h"
#include "ff.h"       /* FATFS */
#include "usbh_msc_core.h"
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 
#define IMAGE_BUFFER_SIZE    512
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
uint8_t filenameString[15]  = {0};

FATFS fatfs;
FIL file;
uint8_t Image_Buf[IMAGE_BUFFER_SIZE];
uint8_t line_idx = 0;   

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_cb =
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError 
};

/**
* @}
*/

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 
/*--------------- LCD Messages ---------------*/
const uint8_t MSG_HOST_INIT[]        = "> Host Library Initialized\n";
const uint8_t MSG_DEV_ATTACHED[]     = "> Device Attached \n";
const uint8_t MSG_DEV_DISCONNECTED[] = "> Device Disconnected\n";
const uint8_t MSG_DEV_ENUMERATED[]   = "> Enumeration completed \n";
const uint8_t MSG_DEV_HIGHSPEED[]    = "> High speed device detected\n";
const uint8_t MSG_DEV_FULLSPEED[]    = "> Full speed device detected\n";
const uint8_t MSG_DEV_LOWSPEED[]     = "> Low speed device detected\n";
const uint8_t MSG_DEV_ERROR[]        = "> Device fault \n";

const uint8_t MSG_MSC_CLASS[]        = "> Mass storage device connected\n";
const uint8_t MSG_HID_CLASS[]        = "> HID device connected\n";
const uint8_t MSG_DISK_SIZE[]        = "> Size of the disk in MBytes: \n";
const uint8_t MSG_LUN[]              = "> LUN Available in the device:\n";
const uint8_t MSG_ROOT_CONT[]        = "> Exploring disk flash ...\n";
const uint8_t MSG_WR_PROTECT[]       = "> The disk is write protected\n";
const uint8_t MSG_UNREC_ERROR[]      = "> UNRECOVERED ERROR STATE\n";

/**
* @}
*/


/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level);
static uint8_t Image_Browser (char* path);
static void     Show_Image(void);
static void     Toggle_Leds(void);
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{

}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
    printf("MSG_DEV_ATTACHED\n");
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
    printf("MSG_UNREC_ERROR\n");
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
    printf("MSG_DEV_DISCONNECTED\n");
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
	/* callback for USB-Reset */
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("MSG_DEV_HIGHSPEED\n");
	}  
	else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("MSG_DEV_FULLSPEED\n");
	}
	else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("HPRT0_PRTSPD_LOW_SPEED\n");
	}
	else
	{
		printf("MSG_DEV_ERROR\n");
	}
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs = DeviceDesc;  

	printf("VID : %04Xh\n" , (uint32_t)(*hs).idVendor);
	printf("PID : %04Xh\n" , (uint32_t)(*hs).idProduct);
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id;

	id = itfDesc;  

	if((*id).bInterfaceClass  == 0x08)
	{
		printf("MSG_MSC_CLASS\n");
	}
	else
	{
		printf("This type of device does not support!!!\n");
	}
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
    printf("Manufacturer : %s\n", (char *)ManufacturerString);
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
    printf("Product : %s\n", (char *)ProductString);    
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
    printf("Serial Number : %s\n", (char *)SerialNumString);     
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
    printf("MSG_DEV_ENUMERATED\n"); 
} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
    printf("> Device not supported.\n"); 
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
	return USBH_USR_RESP_OK;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
    printf("Overcurrent detected.\n"); 
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
	FRESULT res;
	uint8_t writeTextBuff[] = "MEGAHUNT Connectivity line Host Demo application using FAT_FS   ";
	uint16_t bytesWritten, bytesToWrite;

	switch(USBH_USR_ApplicationState)
	{
	case USH_USR_FS_INIT: 
		/* Initialises the File System*/
		if ( f_mount( 0, &fatfs ) != FR_OK ) 
		{
			return(-1);
		}

		USBH_USR_ApplicationState = USH_USR_FS_READLIST;
	break;

	case USH_USR_FS_READLIST:
		Explore_Disk("0:/", 1);
		line_idx = 0;   
		USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
	break;

	case USH_USR_FS_WRITEFILE:  
		/* Register work area for logical drives */
		f_mount(0, &fatfs);

		if(f_open(&file, "0:MEGAHUNT.TXT",FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
		{
			/*close file and filesystem*/
			f_close(&file);
		}
		else
		{

		}

		USBH_USR_ApplicationState = USH_USR_FS_DRAW; 
	break;

	case USH_USR_FS_DRAW:
		while(HCD_IsDeviceConnected(&USB_OTG_Core))
		{
			if ( f_mount( 0, &fatfs ) != FR_OK ) 
			{
				/* fat_fs initialisation fails*/
				return(-1);
			}
			return Image_Browser("0:/");
		}
	break;
		
	default:
		break;
	}
	return(0);
}

/**
* @brief  Explore_Disk 
*         Displays disk content
* @param  path: pointer to root path
* @retval None
*/
static uint8_t Explore_Disk (char* path , uint8_t recu_level)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	char *fn;
	char tmp[14];

	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		while(HCD_IsDeviceConnected(&USB_OTG_Core)) 
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) 
			{
				break;
			}
			if (fno.fname[0] == '.')
			{
				continue;
			}

			fn = fno.fname;
			strcpy(tmp, fn); 

			line_idx++;
			if(line_idx > 9)
			{
				line_idx = 0;
			} 

			if(recu_level == 1)
			{

			}
			else if(recu_level == 2)
			{

			}
			if((fno.fattrib & AM_MASK) == AM_DIR)
			{
				strcat(tmp, "\n"); 
			}
			else
			{
				strcat(tmp, "\n"); 
			}

			if(((fno.fattrib & AM_MASK) == AM_DIR)&&(recu_level == 1))
			{
				Explore_Disk(fn, 2);
			}
		}
	}
	return res;
}

static uint8_t Image_Browser (char* path)
{
	FRESULT res;
	uint8_t ret = 1;
	FILINFO fno;
	DIR dir;
	char *fn;

	res = f_opendir(&dir, path);
	if (res == FR_OK)
	{
		for (;;)
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0)
			{
				break;			
			}
			if (fno.fname[0] == '.')
			{
				continue;			
			}

			fn = fno.fname;

			if (fno.fattrib & AM_DIR) 
			{
				continue;
			} 
			else 
			{
				if((strstr(fn, "bmp")) || (strstr(fn, "BMP")))
				{
					res = f_open(&file, fn, FA_OPEN_EXISTING | FA_READ);
					Show_Image();
					USB_OTG_BSP_mDelay(100);
					ret = 0;
		//          while((HCD_IsDeviceConnected(&USB_OTG_Core)) && \
		//            (STM_EVAL_PBGetState (BUTTON_KEY) == SET))
		//          {
		//            Toggle_Leds();
		//          }
					f_close(&file);

				}
			}
		}  
	}

	USBH_USR_ApplicationState = USH_USR_FS_READLIST;
	return ret;
}

/**
* @brief  Show_Image 
*         Displays BMP image
* @param  None
* @retval None
*/
static void Show_Image(void)
{
	uint16_t i = 0;
	uint16_t numOfReadBytes = 0;
	FRESULT res; 

	/* Bypass Bitmap header */ 
	f_lseek (&file, 54);

	while (HCD_IsDeviceConnected(&USB_OTG_Core))
	{
		res = f_read(&file, Image_Buf, IMAGE_BUFFER_SIZE, (void *)&numOfReadBytes);
		if((numOfReadBytes == 0) || (res != FR_OK)) /*EOF or Error*/
		{
			break; 
		}
		for(i = 0 ; i < IMAGE_BUFFER_SIZE; i+= 2)
		{
			
		} 
	}
}

/**
* @brief  Toggle_Leds
*         Toggle leds to shows user input state
* @param  None
* @retval None
*/
static void Toggle_Leds(void)
{
	static uint32_t i;
	
	if (i++ == 0x10000)
	{
		i = 0;
	}  
}
/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
}

/************************ (C) COPYRIGHT 2014 Megahuntmicro ****END OF FILE****/
