/**
 ******************************************************************************
 * @file    app.c
 * @author  Megahuntmicro
 * @version V1.0.0
 * @date    2019-03-27
 * @brief   This file provides all the Application firmware functions.
 ******************************************************************************
 */

#include "App.h"

void UART_Configuration(void);

void SystemInit(void)
{
    SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);
    SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div_None);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
}

int main(void)
{
    /* Enable CLock */
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);

    UART_Configuration();
    USBSetup();

    printf("MegaHunt SCPU USB Composite Device Demo V1.0.\n");
    printf("HID: Send keys from UART0 received.\n");
    printf("VCP: Send all received data from VCP Port.\n");
    while (1)
    {
        if (APP_Gdata_param.COM_config_cmp)
        {
            if (VCP_GetRxChar() == 's')
            {
                printf("> Rx s.\n");
                VCP_DataTx((uint8_t *)"Test",4);
            }
        }
    }
}

void UART_Configuration(void)
{
    UART_InitTypeDef UART_InitStructure;
    /* Enable CLock */
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);

    UART_InitStructure.UART_BaudRate   = 115200;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits   = UART_StopBits_1;
    UART_InitStructure.UART_Parity     = UART_Parity_No;

    UART_Init(UART0, &UART_InitStructure);
}

int fputc(int ch, FILE* f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    while (!UART_IsTXEmpty(UART0)) {}
    UART_SendData(UART0, (uint8_t)ch);

    return ch;
}

#ifdef USE_FULL_ASSERT
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
    while (1) {}
}
#endif
