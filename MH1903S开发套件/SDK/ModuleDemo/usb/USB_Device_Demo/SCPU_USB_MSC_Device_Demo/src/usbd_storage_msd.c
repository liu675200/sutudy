/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : usbd_storage_msd.c
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 21-October-2014
 * Description          : This file provides the disk operations functions.
 *****************************************************************************/
#include "usbd_msc_mem.h"
#include "usb_conf.h"

#define STORAGE_LUN_NBR                  1 
#define	MSC_CMD_OFFSET					16

#define	BOOT_MAJOR_VER	0
#define	BOOT_MINOR_VER	5
#define	BOOT_REVISER_VER	0
/* USB Mass storage Standard Inquiry Data */
const int8_t  STORAGE_Inquirydata[] = {//36
  
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  'M', 'E', 'G', 'A', 'H', 'U', 'N', 'T', /* Manufacturer : 8 bytes */
  'm', 'i', 'c', 'r', 'o', 'S', 'D', ' ', /* Product      : 16 Bytes */
  'F', 'l', 'a', 's', 'h', ' ', ' ', ' ',
  '1', '.', '0' ,'0',                     /* Version      : 4 Bytes */
}; 


#define	BOOT_MSC_BLOCK_NUM	(2 << 20)	// 1GB
#define	BOOT_MSC_BLOCK_SIZE	512
#define	BOOT_MSC_CMD_OFFSET	(128 << 20)	// 128MB
#define	BOOT_MSC_CMD_BUF_SIZE	BOOT_MSC_BLOCK_SIZE
#define	BOOT_MSC_DATA_OFFSET	(512 << 20)	// 512MB
#define	BOOT_MSC_DATA_SIZE		(512)	// 128KB

//Global variable declaration that other module will be used
uint8_t gMSCCmdBuf[BOOT_MSC_CMD_BUF_SIZE] = "MH1902 ROM BOOT\x00\0x00";
uint8_t gMSCDataBuf[BOOT_MSC_DATA_SIZE];

int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun, 
                           uint32_t *block_num, 
                           uint32_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);


USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata,
};

USBD_STORAGE_cb_TypeDef  *USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;
enum MUTEX_VALUE_e
{
	MUTEX_LOCKED = 0x55,
	MUTEX_UNLOCKED = 0xAA
};

volatile uint32_t gMSCMutex = MUTEX_LOCKED;

/**
  * @brief  Initialize the storage medium
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t STORAGE_Init (uint8_t lun)
{
	gMSCMutex = MUTEX_UNLOCKED;
	//memset(gMSCCmdBuf, 0, sizeof(gMSCCmdBuf));
	//memcpy(gMSCCmdBuf, "MH1903 BOOT\x00\x00\x00\x00\x00\x00", MSC_CMD_OFFSET);
	return (0);
}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
	*block_size =  BOOT_MSC_BLOCK_SIZE;  
	*block_num = BOOT_MSC_BLOCK_NUM;
	return (0);
}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{
	return (0);
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
	return  0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Read (uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_addr,                       
                 uint16_t blk_len)
{  
	uint8_t *addr;

	blk_addr *= BOOT_MSC_BLOCK_SIZE;
	blk_len  *= BOOT_MSC_BLOCK_SIZE;
	//???那D赤?芍豕?cmd??車辰
	if(blk_addr == BOOT_MSC_CMD_OFFSET)
	{
		if (blk_len > BOOT_MSC_CMD_BUF_SIZE)
		{
			blk_len = BOOT_MSC_CMD_BUF_SIZE;
		}
		memcpy(buf, gMSCCmdBuf, blk_len);
	}
	else
	{
		memset(buf, 0, blk_len);
	}
	return 0;
}


/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */
int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
	blk_addr *= BOOT_MSC_BLOCK_SIZE;
	blk_len  *= BOOT_MSC_BLOCK_SIZE;
	
	if (blk_addr >= BOOT_MSC_DATA_OFFSET && (blk_addr + blk_len) <= (BOOT_MSC_DATA_OFFSET + BOOT_MSC_DATA_SIZE))
	{
		memcpy(gMSCDataBuf + (blk_addr -BOOT_MSC_DATA_OFFSET) , buf, blk_len);
	}
	else if(blk_addr == BOOT_MSC_CMD_OFFSET)
	{
		if (blk_len > BOOT_MSC_CMD_BUF_SIZE)
		{
			blk_len = BOOT_MSC_CMD_BUF_SIZE;
		}
		memcpy(gMSCCmdBuf + MSC_CMD_OFFSET, buf + MSC_CMD_OFFSET, blk_len - MSC_CMD_OFFSET);
		if (blk_addr + blk_len >= BOOT_MSC_CMD_OFFSET + BOOT_MSC_CMD_BUF_SIZE)
		{
			//那y?Y辰??-D∩赤那, ?a??
			gMSCMutex = MUTEX_UNLOCKED;
		}
		else
		{
			gMSCMutex = MUTEX_LOCKED;
		}
	}
	
	return (0);
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun (void)
{
  return (STORAGE_LUN_NBR - 1);
}


uint32_t WriteMSC(void *buf, uint32_t len)
{
	if (buf == NULL || len >= sizeof(gMSCCmdBuf) - MSC_CMD_OFFSET)
	{
		return -1;
	}
	while (gMSCMutex == MUTEX_LOCKED);
	NVIC_DisableIRQ(USB_IRQn);
	gMSCMutex = MUTEX_LOCKED;
	memcpy(gMSCCmdBuf + MSC_CMD_OFFSET, buf, len);
	gMSCMutex = MUTEX_UNLOCKED;
	NVIC_EnableIRQ(USB_IRQn);

	return 0;
}

uint32_t ReadMSC(void *buf, uint32_t len)
{
	if (buf == NULL)
	{
		return -1;
	}
	if (len > sizeof(gMSCCmdBuf) - MSC_CMD_OFFSET)
	{
		len = sizeof(gMSCCmdBuf) - MSC_CMD_OFFSET;
	}
	while (gMSCMutex == MUTEX_LOCKED);
	NVIC_DisableIRQ(USB_IRQn);
	gMSCMutex = MUTEX_LOCKED;
	memcpy(buf, gMSCCmdBuf + MSC_CMD_OFFSET, len);
	gMSCMutex = MUTEX_UNLOCKED;
	NVIC_EnableIRQ(USB_IRQn);

	return 0;
}

