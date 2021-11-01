#include "inc.h"

extern  unsigned int _estack;
extern  unsigned int __text_end__;
extern  unsigned int __data_start__;
extern  unsigned int __bss_start__;
extern  unsigned int __bss_end__;
extern  unsigned int __data_end__;
extern  int main(void);

typedef void (*Handler)(void);//定义函数指针Handle

typedef struct{
	void* estack;//链接脚本定义的栈指针
	Handler vector[63];
} Vectors;

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

void DMA0_IRQHandler(void);
void USB_IRQHandler(void);
void USBDMA_IRQHandler(void);
void LCD_IRQHandler(void);
void SCI0_IRQHandler(void);
void UART0_IRQHandler(void);
void UART1_IRQHandler(void);
void SPI0_IRQHandler(void);
void CRYPT0_IRQHandler(void);
void TIM0_0_IRQHandler(void);
void TIM0_1_IRQHandler(void);
void TIM0_2_IRQHandler(void);
void TIM0_3_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);

Vectors cortex_vectors = {
	&_estack,
	{
		[0] = Reset_Handler,//boot的中断向量表应尽量简单，因此目前只包括复位异常服务函数
		[1] = NMI_Handler,
		[2] = HardFault_Handler,
		[3] = MemManage_Handler,
		[4] = BusFault_Handler,
		[5] = UsageFault_Handler,
		0,
		0,
		0,
		0,
		[10] = SVC_Handler,
		[11] = DebugMon_Handler,
		0,
		[13] = PendSV_Handler,
		[14] = SysTick_Handler,
		[15] = DMA0_IRQHandler,
		[16] = USB_IRQHandler,
		[17] = USBDMA_IRQHandler,
		[18] = LCD_IRQHandler,
		[19] = SCI0_IRQHandler,
		[20] = UART0_IRQHandler,
		[21] = UART1_IRQHandler,
		[22] = SPI0_IRQHandler,
		[23] = CRYPT0_IRQHandler,
		[24] = TIM0_0_IRQHandler,
		[25] = TIM0_1_IRQHandler,
		[26] = TIM0_2_IRQHandler,
		[27] = TIM0_3_IRQHandler,
		[28] = EXTI0_IRQHandler,
		[29] = EXTI1_IRQHandler
	}	
};

void Reset_Handler(void)//复位中断服务函数，其主要功能是初始化全局变量data段和bss段
{
	unsigned int *flash, *ram;
	flash = &__text_end__;
	ram = &__data_start__;

	SCB->VTOR = 0x01001000;
	if(!(flash == ram))
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
		*ram = 0;
	}
	//SYSCTRL->FREQ_SEL |=  0x200F395A;    //0010 0000 0000 1101 0011 1001 0101 1010 PLL:5'h0d 192MHz; 
	main();
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

__attribute__((weak)) void DMA0_IRQHandler(void){}
__attribute__((weak)) void USB_IRQHandler(void){}
__attribute__((weak)) void USBDMA_IRQHandler(void){}
__attribute__((weak)) void LCD_IRQHandler(void){}
__attribute__((weak)) void SCI0_IRQHandler(void){}
__attribute__((weak)) void UART0_IRQHandler(void){}
__attribute__((weak)) void UART1_IRQHandler(void){}
__attribute__((weak)) void SPI0_IRQHandler(void){}
__attribute__((weak)) void CRYPT0_IRQHandler(void){}
__attribute__((weak)) void TIM0_0_IRQHandler(void){}
__attribute__((weak)) void TIM0_1_IRQHandler(void){}
__attribute__((weak)) void TIM0_2_IRQHandler(void){}
__attribute__((weak)) void TIM0_3_IRQHandler(void){}
__attribute__((weak)) void EXTI0_IRQHandler(void){}
__attribute__((weak)) void EXTI1_IRQHandler(void){}

