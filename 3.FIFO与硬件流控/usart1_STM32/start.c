#include "inc.h"
typedef void (*Handler)(void);

void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void WWDG_IRQHandler(void);
void PVD_IRQHandler(void);
void TAMPER_IRQHandler(void);
void RTC_IRQHandler(void);
void FLASH_IRQHandler(void);
void RCC_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void DMA1_Channel1_IRQHandler(void);
void DMA1_Channel2_IRQHandler(void);
void DMA1_Channel3_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void DMA1_Channel6_IRQHandler(void);
void DMA1_Channel7_IRQHandler(void);
void ADC1_2_IRQHandler(void);
void USB_HP_CAN1_TX_IRQHandler(void);
void USB_LP_CAN1_RX0_IRQHandler(void);
void CAN1_RX1_IRQHandler(void);
void CAN1_SCE_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void TIM1_BRK_IRQHandler(void);
void TIM1_UP_IRQHandler(void);
void TIM1_TRG_COM_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);
void SPI1_IRQHandler(void);
void SPI2_IRQHandler(void);
void USART1_IRQHandler(void);

typedef struct{                //定义空间结构体
	void* estack;
	Handler vector[256];
} Vectors;

extern void SysClock();
extern int main(void);
extern  uint32_t _estack;        //引用链接脚本中地址
extern  uint32_t __text_end__;
extern  uint32_t __data_start__;
extern  uint32_t __data_end__;
extern  uint32_t __bss_start__;
extern  uint32_t __bss_end__;

const Vectors cortex_vectors = {    //初始化结构体，赋予指针地址与内容
	&_estack,
	{	
		Reset_Handler,
		NMI_Handler,
		HardFault_Handler,
		MemManage_Handler,
		BusFault_Handler,
		UsageFault_Handler,
		0,
		0,
		0,
		0,
		SVC_Handler,
		DebugMon_Handler,
		0,
		PendSV_Handler,
		SysTick_Handler,
		WWDG_IRQHandler,
		PVD_IRQHandler,
		TAMPER_IRQHandler,
		RTC_IRQHandler,
		FLASH_IRQHandler,
		RCC_IRQHandler,
		EXTI0_IRQHandler,
		EXTI1_IRQHandler,
		EXTI2_IRQHandler,
		EXTI3_IRQHandler,
		EXTI4_IRQHandler,
		DMA1_Channel1_IRQHandler,
		DMA1_Channel2_IRQHandler,
		DMA1_Channel3_IRQHandler,
		DMA1_Channel4_IRQHandler,
		DMA1_Channel5_IRQHandler,
		DMA1_Channel6_IRQHandler,
		DMA1_Channel7_IRQHandler,
		ADC1_2_IRQHandler,
		USB_HP_CAN1_TX_IRQHandler,
		USB_LP_CAN1_RX0_IRQHandler,
		CAN1_RX1_IRQHandler,
		CAN1_SCE_IRQHandler,
		EXTI9_5_IRQHandler,
		TIM1_BRK_IRQHandler,
		TIM1_UP_IRQHandler,
		TIM1_TRG_COM_IRQHandler,
		TIM1_CC_IRQHandler,
		TIM2_IRQHandler,
		TIM3_IRQHandler,
		TIM4_IRQHandler,
		I2C1_EV_IRQHandler,
		I2C1_ER_IRQHandler,
		I2C2_EV_IRQHandler,
		I2C2_ER_IRQHandler,
		SPI1_IRQHandler,
		SPI2_IRQHandler,
		USART1_IRQHandler
	}
};


void Reset_Handler(void)            //复位函数
{
    uint32_t *flash, *ram;
	flash = &__text_end__;
	ram = &__data_start__;
	if(flash != ram)       //ram实际地址不是连在flash后面时，复制数据
		{
			for(;ram < &__data_end__;)
			{
				*ram = *flash;
				flash ++;
				ram ++;
			}
		}
	for(ram = &__bss_start__; ram < &__bss_end__; ram ++)
		{
			*ram = 0;    //进入bss段指向的地址中清零
		}
  	SysClock();       //初始化系统时钟
	main();           //进入main函数
}


__attribute__((weak)) void NMI_Handler(void){}
__attribute__((weak)) void HardFault_Handler(void){}
__attribute__((weak)) void MemManage_Handler(void){}
__attribute__((weak)) void BusFault_Handler(void){}
__attribute__((weak)) void UsageFault_Handler(void){}
__attribute__((weak)) void SVC_Handler(void){}
__attribute__((weak)) void DebugMon_Handler(void){}
__attribute__((weak)) void PendSV_Handler(void){}
__attribute__((weak)) void SysTick_Handler(void){}
__attribute__((weak)) void WWDG_IRQHandler(void){}
__attribute__((weak)) void PVD_IRQHandler(void){}
__attribute__((weak)) void TAMPER_IRQHandler(void){}
__attribute__((weak)) void RTC_IRQHandler(void){}
__attribute__((weak)) void FLASH_IRQHandler(void){}
__attribute__((weak)) void RCC_IRQHandler(void){}

__attribute__((weak)) void EXTI0_IRQHandler(void){}
__attribute__((weak)) void EXTI1_IRQHandler(void){}
__attribute__((weak)) void EXTI2_IRQHandler(void){}
__attribute__((weak)) void EXTI3_IRQHandler(void){}
__attribute__((weak)) void EXTI4_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel1_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel2_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel3_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel4_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel5_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel6_IRQHandler(void){}
__attribute__((weak)) void DMA1_Channel7_IRQHandler(void){}
__attribute__((weak)) void ADC1_2_IRQHandler(void){}
__attribute__((weak)) void USB_HP_CAN1_TX_IRQHandler(void){}
__attribute__((weak)) void USB_LP_CAN1_RX0_IRQHandler(void){}
__attribute__((weak)) void CAN1_RX1_IRQHandler(void){}
__attribute__((weak)) void CAN1_SCE_IRQHandler(void){}
__attribute__((weak)) void EXTI9_5_IRQHandler(void){}
__attribute__((weak)) void TIM1_BRK_IRQHandler(void){}
__attribute__((weak)) void TIM1_UP_IRQHandler(void){}
__attribute__((weak)) void TIM1_TRG_COM_IRQHandler(void){}
__attribute__((weak)) void TIM1_CC_IRQHandler(void){}
__attribute__((weak)) void TIM2_IRQHandler(void){}
__attribute__((weak)) void TIM3_IRQHandler(void){}
__attribute__((weak)) void TIM4_IRQHandler(void){}
__attribute__((weak)) void I2C1_EV_IRQHandler(void){}
__attribute__((weak)) void I2C1_ER_IRQHandler(void){}
__attribute__((weak)) void I2C2_EV_IRQHandler(void){}
__attribute__((weak)) void I2C2_ER_IRQHandler(void){}
__attribute__((weak)) void SPI1_IRQHandler(void){}
__attribute__((weak)) void SPI2_IRQHandler(void){}
__attribute__((weak)) void USART1_IRQHandler(void){}