#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern int32_t TimerFlag;

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	printf("SysTick_Handler\r\n");
}

void TIM0_0_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_0);
	TIM_Cmd(TIMM0, TIM_0, DISABLE);
	TIM_Cmd(TIMM0, TIM_1, ENABLE);
	
	NVIC_ClearPendingIRQ(TIM0_0_IRQn);
}

void TIM0_1_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_1);
	TIM_Cmd(TIMM0, TIM_1, DISABLE);
	TIM_Cmd(TIMM0, TIM_2, ENABLE);
	
	NVIC_ClearPendingIRQ(TIM0_1_IRQn);	
}

void TIM0_2_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_2);
	TIM_Cmd(TIMM0, TIM_2, DISABLE);
	TIM_Cmd(TIMM0, TIM_3, ENABLE);
	
	NVIC_ClearPendingIRQ(TIM0_2_IRQn);	
}

void TIM0_3_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_3);
	TIM_Cmd(TIMM0, TIM_3, DISABLE);
	TIM_Cmd(TIMM0, TIM_4, ENABLE);
	
	NVIC_ClearPendingIRQ(TIM0_3_IRQn);	
}

void TIM0_4_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_4);
	TIM_Cmd(TIMM0, TIM_4, DISABLE);
	TIM_Cmd(TIMM0, TIM_5, ENABLE);
	
	NVIC_ClearPendingIRQ(TIM0_4_IRQn);	
}

void TIM0_5_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_5);
	TIM_Cmd(TIMM0, TIM_5, DISABLE);
	TIM_Cmd(TIMM0, TIM_6, ENABLE);
	
	NVIC_ClearPendingIRQ(TIM0_5_IRQn);	
}

void TIM0_6_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_6);
	TIM_Cmd(TIMM0, TIM_6, DISABLE);
	TIM_Cmd(TIMM0, TIM_7, ENABLE);	
	
	NVIC_ClearPendingIRQ(TIM0_6_IRQn);	
}

void TIM0_7_IRQHandler(void)
{
	printf("%s In\n", __func__);
	TIM_ClearITPendingBit(TIMM0, TIM_7);
	TIM_Cmd(TIMM0, TIM_7, DISABLE);
	TimerFlag = 1;
	
	NVIC_ClearPendingIRQ(TIM0_7_IRQn);	
}
