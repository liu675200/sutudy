#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>
#include "mhscpu_dcmi.h"
#include "DecodeLib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

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
  printf("HardFault_Handler\n");

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

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */



void UART0_IRQHandler(void)
{
}
/**
 * DCMI 中断处理函数
 *
 */
void DCMI_IRQHandler(void)
{  
	if (DCMI_GetITStatus(DCMI_IT_LINE) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_LINE); 			  
	}
	
	if (DCMI_GetITStatus(DCMI_IT_VSYNC) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_VSYNC);	  
	}

	if (DCMI_GetITStatus(DCMI_IT_FRAME) != RESET) 
	{
        //调用回调函数
        DCMI_CallBackFrame();
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	} 
	
	if (DCMI_GetITStatus(DCMI_IT_OVF) != RESET)
	{
		DCMI_ClearITPendingBit(DCMI_IT_OVF); 
	}
    
	if (DCMI_GetITStatus(DCMI_IT_ERR) != RESET) 
	{
		DCMI_ClearITPendingBit(DCMI_IT_ERR);
	}
}

