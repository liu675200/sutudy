#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

typedef enum
{
	MENU_SEL_MAIN = 0,
	MENU_SEL_CPU_SLEEP,
	MENU_SEL_DEEP_SLEEP
}MENU_SEL;

void PrintMenu(MENU_SEL menu);
uint8_t GetCmd(void);

void NVIC_Configuration(void);
void UART_Configuration(void);
void SENSOR_Configuration(void);
void LOWPOWER_Configuration(void);

void CPU_Sleep_Test(void);
void Deep_Sleep_Test(void);

void RTC_WakeUpTest(SleepMode_TypeDef sleepMode);
void Timer_CPUWakeUpTest(void);
void GPIO_WakeUpTest(SleepMode_TypeDef sleepMode);


int main(void)
{	
    SYSCTRL_PLLConfig(SYSCTRL_PLL_168MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
    SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
    SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
    QSPI_SetLatency(0);
    
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA | SYSCTRL_AHBPeriph_USB | \
                              SYSCTRL_AHBPeriph_LCD | SYSCTRL_AHBPeriph_CRYPT, ENABLE);

    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_DMA | SYSCTRL_AHBPeriph_USB | \
                              SYSCTRL_AHBPeriph_LCD | SYSCTRL_AHBPeriph_CRYPT, ENABLE);    
    
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TRNG | SYSCTRL_APBPeriph_ADC | SYSCTRL_APBPeriph_CRC | \
                              SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_TIMM0 | SYSCTRL_APBPeriph_GPIO | \
							  SYSCTRL_APBPeriph_SCI0 | SYSCTRL_APBPeriph_SPI2 | SYSCTRL_APBPeriph_SPI1 | \
							  SYSCTRL_APBPeriph_SPI0 | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);   
    
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TRNG | SYSCTRL_APBPeriph_ADC | SYSCTRL_APBPeriph_CRC | \
                              SYSCTRL_APBPeriph_BPU | SYSCTRL_APBPeriph_TIMM0 | \
							  SYSCTRL_APBPeriph_SCI0 | SYSCTRL_APBPeriph_SPI2 | SYSCTRL_APBPeriph_SPI1 | \
							  SYSCTRL_APBPeriph_SPI0 | SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_UART1, ENABLE);	

    UART_Configuration();
	SENSOR_Configuration();
	NVIC_Configuration();
	printf("\nMegaHunt SCPU LowPower Demo V1.0.\n");

	LOWPOWER_Configuration();
	while (1)
	{
		
	}
}

void PrintMenu(MENU_SEL menu)
{
	switch((uint32_t)menu)
	{
	case (uint32_t)MENU_SEL_MAIN: 
		printf("===============MAIN MENU================\n");
		printf("1 CPU Sleep Test\n");
		printf("2 Deep Sleep Test\n");
		printf("p Print Menu\n");
		printf("================MENU END================\n");
		break;
	
	case (uint32_t)MENU_SEL_CPU_SLEEP:
		printf("===============CPU SLEEP MENU================\n");
		printf("1 RTC WakeUp Test\n");
		printf("2 Timer WakeUp CPU Sleep Test\n");
		printf("3 Sensor WakeUp CPU Sleep Test\n");
		printf("4 GPIO(PC15) WakeUp CPU Sleep Test\n");
		printf("b Back\n");
		printf("p Print Menu\n");
		printf("===================MENU END==================\n");	
		break;
	
	case (uint32_t)MENU_SEL_DEEP_SLEEP:
		printf("===============DEEP SLEEP MENU================\n");
		printf("1 RTC WakeUp Deep Sleep Test\n");
		printf("2 Sensor WakeUp Deep Sleep Test\n");
		printf("3 GPIO(PC15) WakeUp Deep Sleep Test\n");
		printf("b Back\n");
		printf("p Print Menu\n");
		printf("===================MENU END===================\n");		
		break;
	
	default:
		break;
	}
}


uint8_t GetCmd(void)
{
	uint8_t tmp = 0;

	if (UART_GetLineStatus(UART0) & UART_LINE_STATUS_RX_RECVD)
	{
		tmp = UART_ReceiveData(UART0);
	}
	
	if (tmp)
	{
		printf("cmd is %c\n", tmp);
	}

	return tmp;
}

typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t Pin_Num;
    uint32_t Pin_status;
}TypeRampPinDef;

#define PIN_STATUS_HIGH     1
#define PIN_STATUS_LOW      0

TypeRampPinDef SpecialPin_List[] = 
{
    //CM_LASER,Pull-down resistor
    {GPIOA, GPIO_Pin_3, PIN_STATUS_LOW},
    
    //SCI0_DET
    {GPIOA, GPIO_Pin_6, PIN_STATUS_LOW},
    
    //NFC_CS
    {GPIOB, GPIO_Pin_3, PIN_STATUS_HIGH},
    
    //SPI
    {GPIOB, GPIO_Pin_2, PIN_STATUS_LOW},
    {GPIOB, GPIO_Pin_4, PIN_STATUS_LOW},
    {GPIOB, GPIO_Pin_5, PIN_STATUS_LOW},
    
    //DAC
    {GPIOC, GPIO_Pin_1, PIN_STATUS_LOW},
    
    //Flash_CS
    {GPIOE, GPIO_Pin_6, PIN_STATUS_HIGH},
    
    //CM_LED
    {GPIOA, GPIO_Pin_2, PIN_STATUS_HIGH},
    
    //LCD_CS
    {GPIOC, GPIO_Pin_8, PIN_STATUS_HIGH},
    
    //JTAG
    {GPIOC, GPIO_Pin_4, PIN_STATUS_LOW},
};

void DeepSleep_Prepare(void)
{
    uint32_t u32Count, i;
    uint32_t delay;
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
    
    for (i = 0; i < (sizeof(SpecialPin_List)/sizeof(SpecialPin_List[0])); i++)
    {
        GPIO_InitStruct.GPIO_Pin = SpecialPin_List[i].Pin_Num;
        GPIO_Init(SpecialPin_List[i].GPIOx, &GPIO_InitStruct);
        
        if (SpecialPin_List[i].Pin_status == PIN_STATUS_LOW)
        {
            GPIO_ResetBits(SpecialPin_List[i].GPIOx, SpecialPin_List[i].Pin_Num);
        }
        if (SpecialPin_List[i].Pin_status == PIN_STATUS_HIGH)
        {
            GPIO_SetBits(SpecialPin_List[i].GPIOx, SpecialPin_List[i].Pin_Num);
        }
    }
    
    ADC_StartCmd(DISABLE);
    ADC_DivResistorCmd(DISABLE);
    DAC_Cmd(DISABLE);
    
    /**************************** for MSR *****************************/
    SYSCTRL->MSR_CR1 |= BIT(27);
    for (u32Count = 0; u32Count < 0xFFFFF; u32Count++);

    /************************* power down ROM *************************/    
    SYSCTRL->ANA_CTRL |= BIT(7);
    
    /************************ power down LDO25 ************************/      
    SYSCTRL->LDO25_CR |= (BIT(4) | BIT(5));

    /***************************** for usb ****************************/
    *(uint32_t *)(0x40000C00 +0x0060) = 0x01; 
    *(uint32_t *)(0x40000C00 +0x0000) = 0x4100;
    delay = 0xAFFFFF;
    while (delay--);
}

void DeepSleep_Recover(void)
{
    /************************ for LDO25 ************************/      
    SYSCTRL->LDO25_CR &= ~(BIT(4) | BIT(5));
  
    /************************* for ROM *************************/    
    SYSCTRL->ANA_CTRL &= ~BIT(7);
    
    /************************* for MSR *************************/
    SYSCTRL->MSR_CR1 &= ~BIT(27);

    /************************** for usb ************************/
    *(uint32_t *)(0x40000C00 +0x0000) = 0x0; 
    *(uint32_t *)(0x40000C00 +0x0060) = 0x81;    
}

void SENSOR_Configuration(void)
{   
	SENSOR_EXTInitTypeDef SENSOR_EXTInitStruct;
	memset(&SENSOR_EXTInitStruct, 0, sizeof(SENSOR_EXTInitStruct));

    SENSOR_EXTInitStruct.SENSOR_Port_Dynamic = SENSOR_Port_S01 | SENSOR_Port_S23 | SENSOR_Port_S45 | SENSOR_Port_S67;
    SENSOR_EXTInitStruct.SENSOR_DynamicFrequency = SENSOR_DynamicFrequency_31_25ms;
    SENSOR_EXTInitStruct.SENSOR_GlitchEnable = ENABLE;
    SENSOR_EXTInitStruct.SENSOR_Dynamic_Sample = SENSOR_DYNAMIC_SAMPLE_2;		

    SENSOR_EXTInitStruct.SENSOR_Port_Enable = ENABLE;	

    SENSOR_ClearITPendingBit();
	SENSOR_EXTCmd(DISABLE);
	while(SET == SENSOR_EXTIsRuning());
    
	SENSOR_EXTInit(&SENSOR_EXTInitStruct);
  	SENSOR_AttackRespMode(SENSOR_Interrupt);
	SENSOR_EXTCmd(ENABLE);

	SENSOR_ANACmd(SENSOR_ANA_VOL_HIGH | SENSOR_ANA_VOL_LOW|
				  SENSOR_ANA_TEMPER_HIGH | SENSOR_ANA_TEMPER_LOW|
				  SENSOR_ANA_XTAL32K | SENSOR_ANA_VOLGLITCH, ENABLE);
}


void LOWPOWER_Configuration(void)
{
	volatile uint8_t cmd = 0;
	
	PrintMenu(MENU_SEL_MAIN);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
		case '1':
			CPU_Sleep_Test();
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		case '2':
			Deep_Sleep_Test();
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		case 'p':
			PrintMenu(MENU_SEL_MAIN);
			break;
		
		default:
			break;
		}
	}
}

void CPU_Sleep_Test(void)
{
	uint8_t cmd = 0;

	PrintMenu(MENU_SEL_CPU_SLEEP);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
		case '1':
			RTC_WakeUpTest(SleepMode_CpuOff);
			PrintMenu(MENU_SEL_CPU_SLEEP);		
			break;
		
		case '2':
			Timer_CPUWakeUpTest();
			PrintMenu(MENU_SEL_CPU_SLEEP);
			break;
		
		case '3':
			printf("SleepMode_CPUOff In\n");
			printf("Disconnect any set of Tampers\n");
			SYSCTRL_EnterSleep(SleepMode_CpuOff);
			printf("SleepMode_CPUOff Out\n\n");
			
			PrintMenu(MENU_SEL_CPU_SLEEP);		
			break;
		
		case '4':
			GPIO_WakeUpTest(SleepMode_CpuOff);
			PrintMenu(MENU_SEL_CPU_SLEEP);
			break;
		
		case 'b':
			return;
		
		case 'p':
			PrintMenu(MENU_SEL_CPU_SLEEP);
			break;	
		
		default:
			break;	
		}
	}
}

void Deep_Sleep_Test(void)
{
	uint8_t cmd = 0;
    
	PrintMenu(MENU_SEL_DEEP_SLEEP);
	while (1)
	{
		cmd = GetCmd();
		
		switch (cmd)
		{
		case '1':
			RTC_WakeUpTest(SleepMode_DeepSleep);
			PrintMenu(MENU_SEL_DEEP_SLEEP);		
			break;
		
		case '2':
			printf("SleepMode_DeepSleep In\n");
			printf("Disconnect any set of Tampers\n");
            GPIO->WAKE_TYPE_EN |= BIT(14);
            DeepSleep_Prepare();        
			SYSCTRL_EnterSleep(SleepMode_DeepSleep);
            DeepSleep_Recover();
			printf("SleepMode_DeepSleep Out\n\n");
			
			PrintMenu(MENU_SEL_DEEP_SLEEP);		
			break;
		
		case '3':
			GPIO_WakeUpTest(SleepMode_DeepSleep);
			PrintMenu(MENU_SEL_DEEP_SLEEP);
			break;

		case 'b':
			return;		
		
		case 'p':
			PrintMenu(MENU_SEL_DEEP_SLEEP);
			break;	
		
		default:
			break;	
		}
	}
}

void RTC_WakeUpTest(SleepMode_TypeDef sleepMode)
{
	char *text = (SleepMode_CpuOff == sleepMode) ?  "SleepMode_CPUOff" : "SleepMode_Deep_Sleep";
	
	RTC_ResetCounter();
	RTC_SetAlarm(5);
	RTC_SetRefRegister(0);
	RTC_ITConfig(ENABLE);
      
    if (SleepMode_DeepSleep == sleepMode)
    {
        GPIO->WAKE_TYPE_EN |= BIT(12);
        
        DeepSleep_Prepare();
        
        printf("%s In\n", text);
        SYSCTRL_EnterSleep(sleepMode);
        DeepSleep_Recover();         
    }
    else
    {
        printf("%s In\n", text);
        SYSCTRL_EnterSleep(sleepMode);        
    }
    
	printf("%s Out\n\n", text);
}

void Timer_CPUWakeUpTest(void)
{
	TIM_InitTypeDef TIM_InitStructure;
	SYSCTRL_ClocksTypeDef c;

	SYSCTRL_GetClocksFreq(&c);

	TIM_InitStructure.TIMx = TIM_0;
	TIM_InitStructure.TIM_Period = c.PCLK_Frequency * 5;

	NVIC_ClearPendingIRQ(TIM0_0_IRQn);
	
	TIM_Init(TIMM0, &TIM_InitStructure);
	TIM_ITConfig(TIMM0, TIM_0, ENABLE);
	TIM_Cmd(TIMM0, TIM_0, ENABLE);
	
	printf("SleepMode_CPUOff In\n");
	printf("Timers will trigger the interrput after 5s\n");
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
	SYSCTRL_EnterSleep(SleepMode_CpuOff);
	printf("SleepMode_CPUOff Out\n\n");
	
}

void GPIO_WakeUpTest(SleepMode_TypeDef sleepMode)
{
	char *text = (SleepMode_CpuOff == sleepMode) ?  "SleepMode_CPUOff" : "SleepMode_Deep_Sleep";	
	GPIO_InitTypeDef GPIO_InitStruct;

	//GPIO Configuration(PC15)
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
  
    EXTI_ClearITPendingBit(EXTI_Line2);
    NVIC_ClearPendingIRQ(EXTI2_IRQn);     
	EXTI_LineConfig(EXTI_Line2, EXTI_PinSource15, EXTI_Trigger_Falling);

	if (SleepMode_DeepSleep == sleepMode)
	{
		GPIO_WakeEvenConfig(GPIO_PortSourceGPIOC, GPIO_Pin_15, ENABLE);
		GPIO_WakeModeConfig(GPIO_WakeMode_Now);
        
        DeepSleep_Prepare();

        printf("%s In\n", text);     
        SYSCTRL_EnterSleep(sleepMode);
        DeepSleep_Recover();         
	}
    else
    {
        printf("%s In\n", text);
        SYSCTRL_EnterSleep(sleepMode);        
    }

	printf("%s Out\n", text);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
	
	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);	
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = SENSOR_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_Init(&NVIC_InitStructure);	

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}


int SER_PutChar (int ch)
{
	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	
	return ch;
}

int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')
	{
		SER_PutChar('\r');
	}
	
	return (SER_PutChar(c));
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

