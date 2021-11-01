/**
  ******************************************************************************
  * @file    main.c
  * @author  Megahuntmicro
  * @version V1.0.0
  * @date    21-October-2014
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * 
  ******************************************************************************
  */
#include "mhscpu.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include <string.h>
#include <stdio.h>
#include "usb_otg.h"
#include "usb_dcd_int.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void NVIC_Configuration(void);
void UART_Configuration(void);

uint8_t Buf[64] = {0, 0, 0, 0};

int main(void)
{
	uint8_t ReceiveCode = 0;
	int8_t x = 0, y = 0;

	uint32_t J = 60000;
    USBPHY_CR1_TypeDef usbphy_cr1;
	
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);    
	
    usbphy_cr1.d32 = MHSCPU_READ_REG32(&SYSCTRL->USBPHY_CR1);
    usbphy_cr1.b.commononn = 0;
    usbphy_cr1.b.stop_ck_for_suspend = 0;
    MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR1,usbphy_cr1.d32);

    /* Enable CLock */
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_USB,ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_USB,ENABLE);

	NVIC_Configuration();
	UART_Configuration();
    
	memset(&USB_OTG_dev, 0x00, sizeof(USB_OTG_dev));
    USBD_Init(&USB_OTG_dev,
        #ifdef USE_USB_OTG_HS
              USB_OTG_HS_CORE_ID,
        #else
              USB_OTG_FS_CORE_ID, 
        #endif
              &USR_desc,
              &USBD_HID_cb, 
              &USRD_cb);
    printf("MegaHunt SCPU HID Device Demo V1.0.\n");

	while(1)
	{
        if (UART0->LSR & 0x01)
        {
            ReceiveCode = (uint8_t )UART0->OFFSET_0.RBR;
            x = 0;
            y = 0;
            switch (ReceiveCode)
            {
                case 'w':                      /* ио */
                    y -= 10;
                    Buf[1] = 0;
                    Buf[2] = y;
                    DCD_EP_Tx ( &USB_OTG_dev,
                                HID_IN_EP,
                                Buf,
                                64);
                    break;
                case 's':                      /* об */
                    y += 10;
                    Buf[1] = 0;
                    Buf[2] = y;
                    DCD_EP_Tx ( &USB_OTG_dev,
                                HID_IN_EP,
                                Buf,
                                64);
                    break;
                case 'd':                      /* ср */
                    x += 10;
                    Buf[1] = x;
                    Buf[2] = 0;
                    DCD_EP_Tx ( &USB_OTG_dev,
                                HID_IN_EP,
                                Buf,
                                64);
                    break;
                case 'a':                      /* вС */
                    x -= 10;
                    Buf[1] = x;
                    Buf[2] = 0;
                    DCD_EP_Tx ( &USB_OTG_dev,
                                HID_IN_EP,
                                Buf,
                                64);
                    break;
                default:
                    break;
            }
        }
	}

}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Enable USB Interrupt */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannel = USB_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;
	
    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
    
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
//	UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;	
    
	UART_Init(UART0, &UART_InitStructure);	
//   UART_Init(UART1, &UART_InitStructure);	
}


int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    while(!UART_IsTXEmpty(UART0));
    UART_SendData(UART0, (uint8_t) ch);
//    while(!UART_IsTXEmpty(UART1));
//    UART_SendData(UART1, (uint8_t) ch);
	
    return ch;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

