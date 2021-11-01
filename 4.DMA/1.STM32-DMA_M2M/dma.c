#include "inc.h"


/* 用const定义到FLASH中
 */

const uint32_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
                                    0x11121314,0x15161718,0x191A1B1C,0x1D1E1F20,
                                    0x21222324,0x25262728,0x292A2B2C,0x2D2E2F30,
                                    0x31323334,0x35363738,0x393A3B3C,0x3D3E3F40,
                                    0x41424344,0x45464748,0x494A4B4C,0x4D4E4F50,
                                    0x51525354,0x55565758,0x595A5B5C,0x5D5E5F60,
                                    0x61626364,0x65666768,0x696A6B6C,0x6D6E6F70,
                                    0x71727374,0x75767778,0x797A7B7C,0x7D7E7F80};


/* 目的存储器，存放在SRAM 中																	
 */
uint32_t aDST_Buffer[BUFFER_SIZE];

int DMA_GetFlagStatus(uint32_t DMAy_FLAG)
{
  int bitstatus = 0;
  uint32_t tmpreg = 0;
  /* Calculate the used DMAy */
  if ((DMAy_FLAG & ((uint32_t)0x10000000)) != (uint32_t)0)//----------------------存疑
  {
    /* Get DMA2 ISR register value */
    tmpreg = DMA2->ISR ;
  }
  else
  {
    /* Get DMA1 ISR register value */
    tmpreg = DMA1->ISR ;
  }
  /* Check the status of the specified DMAy flag */
  if ((tmpreg & DMAy_FLAG) != (uint32_t)0)
  {
    /* DMAy_FLAG is set */
    bitstatus = 1;
  }
  else
  {
    /* DMAy_FLAG is reset */
    bitstatus = 0;
  }
  /* Return the DMAy_FLAG status */
  return  bitstatus;
}

uint8_t Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  /* Êý¾Ý³¤¶ÈµÝ¼õ */
  while(BufferLength--)
  {
    /* ÅÐ¶ÏÁ½¸öÊý¾ÝÔ´ÊÇ·ñ¶ÔÓ¦ÏàµÈ */
    if(*pBuffer != *pBuffer1)
    {
      /* ¶ÔÓ¦Êý¾ÝÔ´²»ÏàµÈÂíÉÏÍË³öº¯Êý£¬²¢·µ»Ø0 */
      return 0;
    }
    /* µÝÔöÁ½¸öÊý¾ÝÔ´µÄµØÖ·Ö¸Õë */
    pBuffer++;
    pBuffer1++;
  }
  /* Íê³ÉÅÐ¶Ï²¢ÇÒ¶ÔÓ¦Êý¾ÝÏà¶Ô */
  return 1;  
}                       

void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct)
{
  uint32_t tmpreg = 0;


/*--------------------------- DMAy Channelx CCR Configuration -----------------*/
  /* Get the DMAy_Channelx CCR value */
  tmpreg = DMAy_Channelx->CCR;
  /* Clear MEM2MEM, PL, MSIZE, PSIZE, MINC, PINC, CIRC and DIR bits */
  tmpreg &= ((uint32_t)0xFFFF800F);
  /* Configure DMAy Channelx: data transfer, data size, priority level and mode */
  /* Set DIR bit according to DMA_DIR value */
  /* Set CIRC bit according to DMA_Mode value */
  /* Set PINC bit according to DMA_PeripheralInc value */
  /* Set MINC bit according to DMA_MemoryInc value */
  /* Set PSIZE bits according to DMA_PeripheralDataSize value */
  /* Set MSIZE bits according to DMA_MemoryDataSize value */
  /* Set PL bits according to DMA_Priority value */
  /* Set the MEM2MEM bit according to DMA_M2M value */
  tmpreg |= DMA_InitStruct->DMA_DIR | DMA_InitStruct->DMA_Mode |
            DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
            DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
            DMA_InitStruct->DMA_Priority | DMA_InitStruct->DMA_M2M;

  /* Write to DMAy Channelx CCR */
  DMAy_Channelx->CCR = tmpreg;

/*--------------------------- DMAy Channelx CNDTR Configuration ---------------*/
  /* Write to DMAy Channelx CNDTR */
  DMAy_Channelx->CNDTR = DMA_InitStruct->DMA_BufferSize;

/*--------------------------- DMAy Channelx CPAR Configuration ----------------*/
  /* Write to DMAy Channelx CPAR */
  DMAy_Channelx->CPAR = DMA_InitStruct->DMA_PeripheralBaseAddr;

/*--------------------------- DMAy Channelx CMAR Configuration ----------------*/
  /* Write to DMAy Channelx CMAR */
  DMAy_Channelx->CMAR = DMA_InitStruct->DMA_MemoryBaseAddr;
}

void DMA_Config(void)//M2M模式，DAM1
{
	DMA_InitTypeDef DMA_InitStructure;
	
    // 开DMA时钟
    RCC_AHBENR |= ((uint32_t)0x00000001);
    // 源数据地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aSRC_Const_Buffer;
    // 目标地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)aDST_Buffer;
    // 方向：从外设读取（外设是内部flash）
    DMA_InitStructure.DMA_DIR = ((uint32_t)0x00000000);
    // 使能内存到内存传输
    DMA_InitStructure.DMA_M2M = ((uint32_t)0x00004000);
    // 传输数据大小	
    DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
    // 内存地址递增
    DMA_InitStructure.DMA_MemoryInc = ((uint32_t)0x00000080);
    // 外设地址递增
    DMA_InitStructure.DMA_PeripheralInc = ((uint32_t)0x00000040);
    // 外设数据单位
    DMA_InitStructure.DMA_PeripheralDataSize = ((uint32_t)0x00000200);
    // 内存数据单位
    DMA_InitStructure.DMA_MemoryDataSize = ((uint32_t)0x00000800);	 
    // DMA模式：单次不循环
    DMA_InitStructure.DMA_Mode = ((uint32_t)0x00000000) ;
    //DMA_InitStructure.DMA_Mode = ((uint32_t)0x00000020);  
    // 优先级：高	
    DMA_InitStructure.DMA_Priority = ((uint32_t)0x00002000);

    // 初始化结构体		   
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
    //清除DMA1线6传输完成标志位
    DMA1->IFCR = ((uint32_t)0x00200000);
    // 使能DMA线6
    DMA1_Channel6->CCR |= ((uint16_t)0x0001);
}

