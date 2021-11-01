#include <string.h>
#include <stdio.h> 
#include "mhscpu.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"


__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
void NVIC_Configuration(void);
void UART_Configuration(void);

int main(void)
{
    USBPHY_CR1_TypeDef usbphy_cr1;
	
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
              USB_OTG_FS_CORE_ID, 
              &USR_desc,
              &USBD_MSC_cb, 
              &USRD_cb);
    printf("MegaHunt SCPU USB MSC Device Demo V1.0.\n");

	while(1)
	{

	}

}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
	
	/* Enable USB Interrupt */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
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

	UART_Init(UART0, &UART_InitStructure);		
}


int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    while(!UART_IsTXEmpty(UART0));
    UART_SendData(UART0, (uint8_t) ch);

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
