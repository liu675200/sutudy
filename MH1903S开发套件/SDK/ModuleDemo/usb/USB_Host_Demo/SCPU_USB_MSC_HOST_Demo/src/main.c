#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"
#include "usbh_usr.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE   USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST             USB_Host     __ALIGN_END;


void NVIC_Configuration(void);
void UART_Configuration(void);

int main(void)
{
	USBPHY_CR1_TypeDef usbphy_cr1;
    USBPHY_CR3_TypeDef usbphy_cr3;	
	
    usbphy_cr1.d32 = MHSCPU_READ_REG32(&SYSCTRL->USBPHY_CR1);
    usbphy_cr1.b.commononn = 0;
    usbphy_cr1.b.stop_ck_for_suspend = 0;
    MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR1,usbphy_cr1.d32);
    
    usbphy_cr3.d32 = MHSCPU_READ_REG32(&SYSCTRL->USBPHY_CR3);
    usbphy_cr3.b.idpullup = 1;
    MHSCPU_WRITE_REG32(&SYSCTRL->USBPHY_CR3,usbphy_cr3.d32);
	
    /* Enable CLock */
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_USB,ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_USB,ENABLE);
	
	UART_Configuration();
	NVIC_Configuration();

	memset(&USB_Host, 0x00, sizeof(USB_Host));
	memset(&USB_OTG_Core, 0x00, sizeof(USB_OTG_Core));
	
    USBH_Init(&USB_OTG_Core,
             USB_OTG_FS_CORE_ID,
             &USB_Host,
             &USBH_MSC_cb,
             &USR_cb);

	printf("MegaHunt SCPU MassStorage Host Demo V1.0.\n");
	
	while (1)
    {
        USBH_Process(&USB_OTG_Core , &USB_Host);
    }
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
	
	/* Enable USB Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
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

//Retarget Printf
int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
    
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
    {
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
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
