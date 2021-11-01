
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HID_KEYBOARD__H__
#define __HID_KEYBOARD__H__

#include "mhscpu.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid_bsp.h"
#include "usbd_usr.h"
#include "usb_otg.h"
#include "usb_dcd_int.h"

extern void Keyboard_Configuration(void);
extern void Keyboard_SendKey(char key);

#endif
