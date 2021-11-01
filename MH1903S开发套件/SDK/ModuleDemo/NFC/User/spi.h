/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : spi.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 08/30/2016
 * Description          : SPI wrapper.
 *****************************************************************************/
 
#ifndef __SPI_H__
#define __SPI_H__
 
 
#ifdef __cplusplus
extern "C" {
#endif

#define	READ_REG_CTRL	0x80
	


//bit	SPISS = PA8;//��һλ��Ƭѡ�ź�	 pan

/* Include ------------------------------------------------------------------*/
#include "mhscpu.h"	

/* Exported types -----------------------------------------------------------*/
/* Exported constants -------------------------------------------------------*/	
/* Exported macro -----------------------------------------------------------*/	
/* Exported functions -------------------------------------------------------*/	
/* Exported variables -------------------------------------------------------*/
void spi_config(void);

uint8_t spi_write_byte(uint8_t write_data);
uint8_t spi_read_byte(void); 
uint8_t read_reg(uint8_t addr);
void write_reg(uint8_t addr, uint8_t val);


#ifdef __cplusplus
}
#endif	 

#endif	/* __SPI_H__ */


/********************* (C) COPYRIGHT 2014 Megahuntmicro *********************/
