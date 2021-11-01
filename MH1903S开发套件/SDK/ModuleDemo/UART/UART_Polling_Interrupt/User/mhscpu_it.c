#include "mhscpu.h"
#include "mhscpu_it.h"
#include <string.h>
#include <stdio.h>

extern uint8_t tmpRece;
extern uint32_t flagRece;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

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

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */

void UART0_IRQHandler(void)
{
	volatile uint32_t iir;
	
	UART_TypeDef * UARTx = UART0;

	iir = UART_GetITIdentity(UARTx);
	
	switch(iir & 0x0f)
	{
	case UART_IT_ID_RX_RECVD:
		{
			tmpRece = UART_ReceiveData(UARTx);
			flagRece = 1;
		}
		break;
	case UART_IT_ID_TX_EMPTY:
		{
			UART_SendData(UARTx, tmpRece);
			flagRece = 0;
			UART_ITConfig(UARTx, UART_IT_TX_EMPTY, DISABLE);
		}
		break;
	case UART_IT_ID_MODEM_STATUS:
		{
			volatile uint32_t msr = UARTx->MSR;			
		}
		break;
	case UART_IT_ID_LINE_STATUS:
		{
			volatile uint32_t lsr = UARTx->LSR;			
		}	
		break;
	case UART_IT_ID_BUSY_DETECT:
		{
			volatile uint32_t usr = UARTx->USR;			
		}	
		break;
	case UART_IT_ID_CHAR_TIMEOUT:
		{
			volatile uint32_t rbr = UART_ReceiveData(UARTx);			
		}	
		break;
	default:
		break;

	}
}
