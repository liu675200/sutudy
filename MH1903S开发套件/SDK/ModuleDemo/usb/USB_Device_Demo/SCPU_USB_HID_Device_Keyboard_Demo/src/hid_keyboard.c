#include "usb_bsp.h"
#include "hid_keyboard.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void Keyboard_Configuration(void)
{
     USBPHY_CR1_TypeDef usbphy_cr1;   
    
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_USB,ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_USB,ENABLE);
    
    usbphy_cr1.d32 = MHSCPU_READ_REG32(&SYSCTRL->USBPHY_CR1);
    usbphy_cr1.b.commononn = 0;
    usbphy_cr1.b.stop_ck_for_suspend = 0;
    MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR1,usbphy_cr1.d32);
    
    memset(&USB_OTG_dev, 0x00, sizeof(USB_OTG_dev));
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_HID_Keyboard_cb, &USRD_cb);
}

void Keyboard_SendKey(char key)
{
    uint8_t KeyBuff[8] = {0x02, 0, 0, 0, 0, 0, 0, 0};

    KeyBuff[2] = key - 'a' + 0x04;
    USBD_HID_SendReport(&USB_OTG_dev, KeyBuff, 8);
    USB_OTG_BSP_mDelay(2);

    memset(KeyBuff, 0x00, sizeof(KeyBuff));
    KeyBuff[2] = 0x58;
    USBD_HID_SendReport(&USB_OTG_dev, KeyBuff, 8);

    memset(KeyBuff, 0x00, sizeof(KeyBuff));
    USBD_HID_SendReport(&USB_OTG_dev, KeyBuff, 8);

    USB_OTG_BSP_mDelay(2);
}

void USB_IRQHandler(void)
{
    USBD_OTG_ISR_Handler (&USB_OTG_dev);
    NVIC_ClearPendingIRQ(USB_IRQn);
}
