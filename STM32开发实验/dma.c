#include "inc.h"
extern void USART_SendData(uint8_t Data);
extern void NVIC_Configuration(IRQn_Type IRQ,int Preemption,int Sub);
extern uint32_t sourth1[BUFFER_SIZE];
extern uint32_t sourth2[BUFFER_SIZE];
extern uint32_t* add1;

/* 用const定义到FLASH中
 */

const uint8_t aSRC_Const_Buffer[BUFFER_SIZE]= {
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08,
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08,
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08,
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08};


/* 目的存储器，存放在SRAM 中
 */
uint8_t aDST_Buffer[BUFFER_SIZE]= {
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08,
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08,
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08,
                                    0x01,0x02,0x3,0x04,
                                    0x05,0x06,0x07,0x08};

uint8_t aDST_Buffer2[BUFFER_SIZE]= {
                                    0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,
                                    0x00,0x00,0x00,0x00,
                                    0x01,0x02,0x03,0x04,
                                    0x05,0x06,0x07,0x08};

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

void DMA_Config_M2M(uint32_t Add_Read,uint32_t Add_Wirte,uint32_t Size)//M2M模式，DAM1
{
	DMA_InitTypeDef DMA_InitStructure;  
  RCC_AHBENR |= ((uint32_t)0x00000001);// 开DMA时钟
  DMA_InitStructure.DMA_DIR = ((uint32_t)0x00000000);// 方向：从外设读取（外设是内部flash）
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)Add_Read;// 源数据地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Add_Wirte;// 目标地址

  //外设
  DMA_InitStructure.DMA_PeripheralInc = ((uint32_t)0x00000040);// 外设地址递增
  DMA_InitStructure.DMA_PeripheralDataSize = ((uint32_t)0x00000000);// 外设数据单位

  //内存
  DMA_InitStructure.DMA_MemoryInc = ((uint32_t)0x00000080);// 内存地址递增
  DMA_InitStructure.DMA_MemoryDataSize = ((uint32_t)0x00000000);// 内存数据单位

  //模式配置
  DMA_InitStructure.DMA_BufferSize = Size;// 传输数据大小
  DMA_InitStructure.DMA_M2M = ((uint32_t)0x00004000);// 使能内存到内存传输
  DMA_InitStructure.DMA_Mode = ((uint32_t)0x00000000);// DMA模式：单次不循环
  //DMA_InitStructure.DMA_Mode = ((uint32_t)0x00000020);
  DMA_InitStructure.DMA_Priority = ((uint32_t)0x00002000);// 优先级：高


  // 初始化结构体		   
  DMA_Init(DMA1_Channel6, &DMA_InitStructure);
  //清除DMA1线6传输完成标志位
  DMA1->IFCR = ((uint32_t)0x00200000);
  NVIC_Configuration(DMA1_Channel6_IRQn,1,1);//NVIC配置
  DMA1_Channel6->CCR |= (1<<1); //使能传输完成中断
  // 使能DMA线6
  USART_SendData(0xAA);
  DMA1_Channel6->CCR |= ((uint16_t)0x0001);
}


void DMA_Config_M2P(uint32_t ADD)//M2P模式，内存到串口发送寄存器,可以弄一个（1<<x）x用宏定义来快捷设置
{
  USART_SendData(0x77);
	DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBENR |= ((uint32_t)0x00000001);// 开DMA时钟
  // 源数据地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADD;
  // 目标地址
  DMA_InitStructure.DMA_PeripheralBaseAddr = (USART1_BASE + 0x04);

  // 方向1：从存储器读取数据  0：从外设读取数据
  DMA_InitStructure.DMA_DIR = ((uint32_t)0x00000010);

  //内存配置
  DMA_InitStructure.DMA_MemoryDataSize = ((uint32_t)0x00000000); // 内存数据单位
  DMA_InitStructure.DMA_MemoryInc = ((uint32_t)0x00000080);// 内存地址递增
  //外设配置
  DMA_InitStructure.DMA_PeripheralDataSize = ((uint32_t)0x00000000);// 外设数据单位
  DMA_InitStructure.DMA_PeripheralInc = ((uint32_t)0x00000000);// 外设地址不变

  //DMA模式配置
  DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;// 传输数据大小
  DMA_InitStructure.DMA_Mode = ((uint32_t)0x00000000);// DMA模式：单次不循环
  //DMA_InitStructure.DMA_Mode = ((uint32_t)0x00000020);  
  DMA_InitStructure.DMA_Priority = ((uint32_t)0x00002000);// 优先级：高	
  DMA_InitStructure.DMA_M2M = ((uint32_t)0x00000000);// 禁止内存到内存传输

  DMA_Init(DMA1_Channel4, &DMA_InitStructure);// 初始化结构体
  NVIC_Configuration(DMA1_Channel4_IRQn,1,1);
  DMA1_Channel4->CCR |= (1<<1); //使能传输完成中断
  DMA1_Channel4->CCR |= ((uint16_t)0x0001);// 使能DMA线4
}

/*  使能返回1，未使能返回0
*/
uint32_t DMA_GetStatus(DMA_Channel_TypeDef* DMA1_Channelx)
{
  if(DMA1_Channelx->CCR & ((uint16_t)0x0001))
    return 1;
  else
    return 0;
}