#include "USBBSP.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USBDev __ALIGN_END;

USBD_Class_cb_TypeDef* VCPCallback;

void USBSetup(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    USBPHY_CR1_TypeDef usbphy_cr1;

    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_USB, ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_USB, ENABLE);

    USB_OTG_BSP_mDelay(200);

    usbphy_cr1.d32                   = MHSCPU_READ_REG32(&SYSCTRL->USBPHY_CR1);
    usbphy_cr1.b.commononn           = 0;
    usbphy_cr1.b.stop_ck_for_suspend = 0;
    MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR1, usbphy_cr1.d32);

    memset(&USBDev, 0x00, sizeof(USBDev));
    VCPCallback = &USBD_CDC_cb;

    memset(&APP_Gdata_param, 0, sizeof(APP_Gdata_param));
    
    USBD_Init(&USBDev, USB_OTG_FS_CORE_ID, &USR_desc, VCPCallback, &USRD_cb);
    
    /* Enable USB Interrupt */
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel                   = USB_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);   
}

void USB_IRQHandler(void)
{
    USBD_OTG_ISR_Handler(&USBDev);
    NVIC_ClearPendingIRQ(USB_IRQn);
}
