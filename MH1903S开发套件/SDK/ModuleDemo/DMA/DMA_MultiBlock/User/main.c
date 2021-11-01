#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void NVIC_Configuration(void);
void UART_Configuration(void);
void DMA_Configuration(void);
void LLI_Init(void);

#define MULTIBLOCK_BLOCKSIZE        0x100
#define MULTIBLOCK_BLOCKCOUNTER     4

uint32_t src_Buf[MULTIBLOCK_BLOCKSIZE * MULTIBLOCK_BLOCKCOUNTER] = {0};
uint32_t dst_Buf[MULTIBLOCK_BLOCKCOUNTER][MULTIBLOCK_BLOCKSIZE] = {0};

LLI llilist[MULTIBLOCK_BLOCKCOUNTER];

volatile uint8_t DMA_TcFlag = 0;

/**
 * �������δ�γɻػ���ʹ��DMA��DMA�������������������ݺ��Զ�ֹͣ��
 * ��ʱ����ʹ��DMA_IsChannelEnabled(DMA_CHannel_x)�ж�DMA�Ƿ��Ѿ�������ɣ�
 * �жϣ�������ÿһ����󶼻���һ���жϣ����������������󣬲������жϡ�
 **/

/**
 * ��������γɻػ���ʹ��DMA��DMA��Զ�ﲻ���Զ�ֹͣ������;
 * DMA����������һ����󣬻��������0������а��ˣ������Զ�ֹͣ��
 * �жϣ�������ÿһ����󶼻���һ���жϡ�
 **/
void LLI_Init(void)
{
    uint32_t i;
	
    for (i = 0; i < (MULTIBLOCK_BLOCKCOUNTER - 1); i++)
    {
        DMA_InitLLI(DMA_Channel_0, &llilist[i], &llilist[i + 1], (void*)((uint32_t)src_Buf + (i * 4 * MULTIBLOCK_BLOCKSIZE)), (void*)((uint32_t)dst_Buf[i]), MULTIBLOCK_BLOCKSIZE);
    }
/**
 * Peripheral to memery������ɻػ���
 * memery to memery��������ɻػ���������Զ����������DMA��Զ�ڰ��ˣ�������ֹ�����жϡ�
 **/
//    DMA_InitLLI(DMA_Channel_0, &llilist[3], &llilist[0], (void*)((uint32_t)src_Buf + (3 * 4 * MULTIBLOCK_BLOCKSIZE)),(void*)((uint32_t)dst_Buf[3]), MULTIBLOCK_BLOCKSIZE);
}

int main(void)
{
	uint32_t i = 0, j = 0;
	
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA, ENABLE);
    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	
	UART_Configuration();
    printf("MegaHunt SCPU DMA multiblock memery to memery Demo V1.0.\r\n");
	
    DMA_Configuration();
	LLI_Init();   /* ����DMA������ɺ󣬲��ܽ����������� */
    NVIC_Configuration();
    
    for (i = 0; i < MULTIBLOCK_BLOCKSIZE * MULTIBLOCK_BLOCKCOUNTER; i++)
    {
        src_Buf[i] = i;
    }

    memset(dst_Buf, 0, sizeof(dst_Buf));

    DMA_ChannelCmd(DMA_Channel_0, ENABLE);

	while (0 == (llilist[2].CTL_H & BIT(12)));
    for (i = 0; i < (MULTIBLOCK_BLOCKCOUNTER); i++)
    {
        printf("dst_Buf[%d] data:\n", i);
        for (j = 0; j < MULTIBLOCK_BLOCKSIZE; j++)
        {
            printf("%08X  ", dst_Buf[i][j]);
        }
        printf("\n");
    }
	
    while(1);
	
	return 0;
}


void DMA_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    /* DMA Stream Configuration */  
    DMA_InitStructure.DMA_DIR = DMA_DIR_Memory_To_Memory;
    DMA_InitStructure.DMA_Peripheral = NULL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(src_Buf[0]);	
    DMA_InitStructure.DMA_PeripheralInc = DMA_Inc_Increment;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_DataSize_Word;
    DMA_InitStructure.DMA_PeripheralBurstSize = DMA_BurstSize_1;
    
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&(dst_Buf[0][0]);
    DMA_InitStructure.DMA_MemoryInc = DMA_Inc_Increment;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_DataSize_Word;
    DMA_InitStructure.DMA_MemoryBurstSize = DMA_BurstSize_1;
    DMA_InitStructure.DMA_BlockSize = MULTIBLOCK_BLOCKSIZE;  
    DMA_InitStructure.DMA_PeripheralHandShake = DMA_PeripheralHandShake_Hardware;
    
    DMA_MultiBlockInit(DMA_Channel_0, &DMA_InitStructure, &llilist[0], Multi_Block_MODE10);
	
    DMA_ClearITPendingBit(DMA_Channel_0, DMA_IT_BlockTransferComplete);	
    DMA_ITConfig(DMA_Channel_0, DMA_IT_BlockTransferComplete, ENABLE);

	DMA_ClearITPendingBit(DMA_Channel_0,DMA_IT_Error);
    DMA_ITConfig(DMA_Channel_0, DMA_IT_Error, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQn;
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
int SER_PutChar(int ch)
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
