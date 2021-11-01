/* Include ------------------------------------------------------------------*/
#include <stdio.h>
#include "mhscpu.h"
#include "gpio.h"
#include "spi.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/	

/* Private macro ------------------------------------------------------------*/	
/* Private variables --------------------------------------------------------*/
static SPI_TypeDef *NFC_SPI = SPIM2;
/* Ptivate function prototypes ----------------------------------------------*/	

/******************************************************************************
* Function Name  : spi_config
* Description    : config SPI 
* Input          : NONE
* Output         : NONE
* Return         : NONE 
******************************************************************************/
void spi_config(void)
{
    /// Mode 0, Polling
	SPI_InitTypeDef SPI_InitStructure;

    GPIO_PinRemapConfig(GPIOB, GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Remap_0);   

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;

    SPI_InitStructure.SPI_NSS = SPI_NSS_0;
    
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_RXFIFOFullThreshold = SPI_RXFIFOFullThreshold_1;
	SPI_InitStructure.SPI_TXFIFOEmptyThreshold = SPI_TXFIFOEmptyThreshold_0;
    
	SPI_Init(NFC_SPI, &SPI_InitStructure);    
	SPI_Cmd(NFC_SPI, ENABLE);
}

/******************************************************************************
* Function Name  : spi_write_byte
* Description    : SPI write byte 
* Input          : write_data:write data value
* Output         : NONE
* Return         : state
******************************************************************************/
uint8_t spi_write_byte(uint8_t write_data)
{ 
    uint8_t state = 0;
    uint16_t u16_time_out_counter = 200;
    
    SPI_SendData(NFC_SPI, write_data);
    
    while((RESET == SPI_GetFlagStatus(NFC_SPI, SPI_FLAG_RXNE)) && u16_time_out_counter)
    {
        u16_time_out_counter--;
    }
    
    if (!u16_time_out_counter)
    {
        state = 1;
    }
    
    SPI_ReceiveData(NFC_SPI);
	
    return state;
}

/******************************************************************************
* Function Name  : spi_read_byte
* Description    : SPI read byte 
* Input          : NONE
* Output         : NONE
* Return         : read data value 
******************************************************************************/
uint8_t spi_read_byte(void)
{ 
    uint8_t read_byte = 0;
    uint16_t u16_time_out_counter = 200;
    
    SPI_SendData(NFC_SPI, 0xFF);
    
    while((RESET == SPI_GetFlagStatus(NFC_SPI, SPI_FLAG_RXNE)) && u16_time_out_counter)
    {
        u16_time_out_counter--;
    }
    
    if (!u16_time_out_counter)
    {
        printf("spi_read_byte failed!\r\n");
    }
    
    read_byte = SPI_ReceiveData(NFC_SPI);
    
    return read_byte;
}

/**
 ****************************************************************
 * @brief write_reg() 
 *
 * доƬ�ļĴ���
 *
 * @param:  addr �Ĵ�����ַ
 ****************************************************************
 */
void write_reg(uint8_t addr, uint8_t val)
{
	uint8_t c;

	//���λ���У���Ч������Ϊbit1~bit6
	addr <<= 1;
	
	//��ַ���λΪ1�������Ϊ0����д��
	c = addr & ~(READ_REG_CTRL);

	NFC_SPI_CS_LOW();						
	spi_write_byte(c);
	spi_write_byte(val);
	NFC_SPI_CS_HIGH();
}

/**
 ****************************************************************
 * @brief read_reg() 
 *
 * ��оƬ�ļĴ���
 *
 * @param: addr �Ĵ�����ַ
 * @return: c �Ĵ�����ֵ
 ****************************************************************
 */
uint8_t read_reg(uint8_t addr)
{
	uint8_t c;
	
	//���λ���У���Ч������Ϊbit1~bit6
	addr <<= 1;
	
	//��ַ���λΪ1�������Ϊ0����д��
	c = addr | READ_REG_CTRL;

	NFC_SPI_CS_LOW();	
	spi_write_byte(c);	
	c = spi_read_byte();	
	NFC_SPI_CS_HIGH();	

	return c;
}


/************************ (C) COPYRIGHT 2014 Megahuntmicro **********************/
