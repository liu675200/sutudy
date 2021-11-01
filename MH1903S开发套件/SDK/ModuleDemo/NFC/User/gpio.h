/************************ (C) COPYRIGHT Megahuntmicro *************************
 * File Name            : gpio.h
 * Author               : Megahuntmicro
 * Version              : V1.0.0
 * Date                 : 08/31/2016
 * Description          : GPIO wrapper.
 *****************************************************************************/
 
#ifndef __GPIO_H__
#define __GPIO_H__
 
 
#ifdef __cplusplus
extern "C" {
#endif



/* Include ------------------------------------------------------------------*/
#include "mhscpu.h"	
    
/* Exported types -----------------------------------------------------------*/
/* Exported constants -------------------------------------------------------*/	
/* Exported macro -----------------------------------------------------------*/   
/* Exported variables -------------------------------------------------------*/	  
/* Exported functions -------------------------------------------------------*/	
    
#define	NFC_PD_GPIOX	     GPIOE
#define	NFC_PD_GPIO_PIN	     GPIO_Pin_11    
 
#define	NFC_IRQ_GPIOX	     GPIOE
#define	NFC_IRQ_GPIO_PIN	 GPIO_Pin_10        
    
#define	NFC_SPI_CS_GPIOX	 GPIOB
#define	NFC_SPI_CS_PIN	     GPIO_Pin_3       
    
#define NFC_SPI_CS_LOW()	 GPIO_ResetBits(GPIOB, GPIO_Pin_3)
#define NFC_SPI_CS_HIGH() 	 GPIO_SetBits(GPIOB, GPIO_Pin_3)	    
    
void gpio_config(void);

void led_success_on(void);
void led_success_off(void);

void pcd_poweron(void);
void pcd_powerdown(void);    


#ifdef __cplusplus
}
#endif	 

#endif	/* __GPIO_H__ */


/********************** (C) COPYRIGHT 2014 Megahuntmicro ********************/
