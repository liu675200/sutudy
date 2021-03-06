#ifndef __USBBSP_H__
#define __USBBSP_H__

#include "mhscpu.h"
#include "usb_bsp.h"
#include "usb_dcd_int.h"
#include "usb_otg.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_usr.h"
#include "usbd_cdc_vcp.h"

extern USB_OTG_CORE_HANDLE USBDev;

extern void USBSetup(void);

#endif
