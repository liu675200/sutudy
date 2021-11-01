#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "uart.h"
#include "define.h"
#include "test_emv.h"
#include "emv_core.h"
#include "iso7816_3.h"

#define ALIGN_4(pAddr) ((uint8_t *)((int32_t)((uint8_t *)pAddr + 3) & ~0x03UL))

void NVIC_Configuration(void);
void SCI_Init(void);
uint8_t atr_buf[64];

extern struct emv_core emv_devs[];
#define MH_SMCU_SCU_DES_MASK_BASE       (0x40003600)


#if   defined ( __CC_ARM )
extern uint32_t __Vectors;
#elif defined ( __ICCARM__ )
extern uint32_t __vector_table;
#endif

extern RNG_BUF_ID  uart_SendBuf;
extern RNG_BUF_ID  uart_RecvBuf;
extern RNG_BUF_ID  spi_SendBuf;
extern RNG_BUF_ID  spi_RecvBuf;


int main(int argc, char *argv[])
{
    uint32_t u32Ms;
#if   defined ( __CC_ARM )
    uint8_t *pMemMap = (uint8_t *)((__Vectors + 4) & ~0x03UL);
#elif defined ( __ICCARM__ )
    uint8_t *pMemMap = (uint8_t *)((__vector_table + 4) & ~0x03UL);
#endif
    uart_SendBuf = (RNG_BUF_ID)pMemMap;
    pMemMap += UART_SEND_BUF_SIZE + sizeof(RNG_BUF);
    pMemMap = ALIGN_4(pMemMap);
    
    uart_RecvBuf = (RNG_BUF_ID)pMemMap;
    pMemMap += UART_RECV_BUF_SIZE + sizeof(RNG_BUF) + 64;
    pMemMap = ALIGN_4(pMemMap);

    //Enable clock SCI0, SCI2, UART0, and GPIO.
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_SCI0 | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_SCI0 | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_TIMM0, ENABLE);
    
    //card detect
    SYSCTRL->PHER_CTRL |= BIT(16);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(20);

    
    //card detect
    SYSCTRL->PHER_CTRL &= ~BIT(18);
    //Choose active level(Low level active).
    SYSCTRL->PHER_CTRL |= BIT(22);

    u32Ms = SYSCTRL->PCLK_1MS_VAL * 5;
    uart_Config((u32Ms + (1152 << 2)) / (1152 << 3));

    GPIO_PinRemapConfig(GPIOA, GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, GPIO_Remap_0);
    GPIO_PullUpCmd(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, DISABLE);
    
    SCI_ConfigEMV(0x01, 3000000);

    NVIC_Configuration();

    loop_back(0);
	
    return 0;
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_0);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = SCI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
     ex: PRINT("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small DBG_PRINT (option LD Linker->Libraries->Small DBG_PRINT
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
  #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  #define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    
    if (ch == '\n')
    {
        while(0 != uart_SendChar('\r'));
    }
    while(0 != uart_SendChar((uint8_t) ch));

    return ch;
}

GETCHAR_PROTOTYPE
{
    int32_t s32Recv;

    while (-1 == (s32Recv = uart_RecvChar()));

    return s32Recv;
}





