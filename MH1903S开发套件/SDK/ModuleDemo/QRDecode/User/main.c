#include <stdio.h>
#include <string.h>
#include "mhscpu.h"
#include "DecodeLib.h"
#include "uart.h"
#include "beep.h"
#include "ProjectConfig.h"

/* Define Memory Pool */
unsigned char pool[DECODE_BUFF_SIZE] = {0};
extern void DCMI_NVICConfig(void);
extern void Cameraclk_Configuration(void);
extern void CameraI2CGPIOConfig(void);
extern void DCMI_GPIOInitialize(void);

int main(void)
{
    DecodeInitTypeDef DecodeInitStruct;
    DecodeFlagTypeDef ret;
    
   	SYSCTRL_SYSCLKSourceSelect(SELECT_EXT12M);
	SYSCTRL_PLLConfig(SYSCTRL_PLL_192MHz);
	SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);	/* PCLK >= 48M */

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_GPIO,ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
    SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_OTP, ENABLE);
	
	UART_Configuration();
    
    BEEP_Init();
    
    /* I2C Pin Config */
    CameraI2CGPIOConfig();
    
    /* Camera CLK enable, should before DecodeInit */
    Cameraclk_Configuration();
    
    DCMI_GPIOInitialize();
	
    /* DecodeLib & MEM Pool Init */
    DecodeInitStruct.pool = pool;
    DecodeInitStruct.size = DECODE_BUFF_SIZE;
    DecodeInitStruct.CAM_PWDN_GPIOx = CAM_PWDN_GPIO;
    DecodeInitStruct.CAM_PWDN_GPIO_Pin = CAM_PWDN_GOIO_PIN;
    DecodeInitStruct.CAM_RST_GPIOx = CAM_RST_GPIO;
    DecodeInitStruct.CAM_RST_GPIO_Pin = CAM_RST_GOIO_PIN;
    DecodeInitStruct.CAM_I2Cx = I2C0;
    DecodeInitStruct.CAM_I2CClockSpeed = I2C_ClockSpeed_400KHz;
    DecodeInitStruct.SensorConfig = NULL;
    DecodeInitStruct.SensorCfgSize = 0;
    
    ret = DecodeInit(&DecodeInitStruct);
    if (ret != DecodeInitSuccess)
    {
        switch (ret)
        {
            case DecodeInitCheckError:
                printf("Decoding Library Check Failure!\r\n");
            break;
            
            case DecodeInitMemoryError:
                printf("Insufficient memory in decoding library!\r\n");
            break;
            
            case DecodeInitSensorError:
                printf("Camera initialization failed!\r\n");
            break;
			
            default:
                break;
        }
        while(1);
    }
    
    /* Should invoked after DecodeInit() */
    DCMI_NVICConfig();
    
 	printf("Decode Demo V%d.%d.%d\n", (GetDecodeLibVerison() >> 16) & 0xff, (GetDecodeLibVerison() >> 8) & 0xff, GetDecodeLibVerison() & 0xff);
    
    SingleBuffDecodeDemo();
    
    /* Release hardware resources */
    CloseDecode();
    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_I2C0, DISABLE);
    SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_DMA, DISABLE);
}

/* DCMI Interrupt Config */
void DCMI_NVICConfig(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
   	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
    
    NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure); 
    
//  DCMI_ITConfig(DCMI_IT_VSYNC, ENABLE);
    DCMI_ITConfig(DCMI_IT_OVF, ENABLE);
//  DCMI_ITConfig(DCMI_IT_LINE, ENABLE);
    DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
    DCMI_ITConfig(DCMI_IT_ERR, ENABLE);

	DCMI_ClearITPendingBit(DCMI_IT_VSYNC);
	DCMI_ClearITPendingBit(DCMI_IT_OVF);
	DCMI_ClearITPendingBit(DCMI_IT_LINE);
	DCMI_ClearITPendingBit(DCMI_IT_FRAME);
	DCMI_ClearITPendingBit(DCMI_IT_ERR);
}

/* I2C Pin Config */
void CameraI2CGPIOConfig(void)
{
    SYSCTRL_APBPeriphClockCmd( SYSCTRL_APBPeriph_I2C0, ENABLE);
    I2C_DeInit(I2C0);
    
    GPIO_PinRemapConfig( SI2C_PORT, SI2C_SCL_PIN, SI2C_GPIO_REMAP);
    GPIO_PinRemapConfig( SI2C_PORT, SI2C_SDA_PIN, SI2C_GPIO_REMAP);
}

//DCMI管脚复用设置
void DCMI_GPIOInitialize(void) 
{
	/* 控制信号  */
	GPIO_PinRemapConfig(DCMI_VSYNC_GPIO_PORT, DCMI_VSYNC_GPIO_PIN, DCMI_VSYNC_AF);
	GPIO_PinRemapConfig(DCMI_HSYNC_GPIO_PORT, DCMI_HSYNC_GPIO_PIN, DCMI_HSYNC_AF);
	GPIO_PinRemapConfig(DCMI_PIXCLK_GPIO_PORT, DCMI_PIXCLK_GPIO_PIN, DCMI_PIXCLK_AF);

	/* 数据信号 */
	GPIO_PinRemapConfig(DCMI_D0_GPIO_PORT, DCMI_D0_GPIO_PIN, DCMI_D0_AF);
	GPIO_PinRemapConfig(DCMI_D1_GPIO_PORT, DCMI_D1_GPIO_PIN, DCMI_D1_AF);
	GPIO_PinRemapConfig(DCMI_D2_GPIO_PORT, DCMI_D2_GPIO_PIN, DCMI_D2_AF);
	GPIO_PinRemapConfig(DCMI_D3_GPIO_PORT, DCMI_D3_GPIO_PIN, DCMI_D3_AF);
	GPIO_PinRemapConfig(DCMI_D4_GPIO_PORT, DCMI_D4_GPIO_PIN, DCMI_D4_AF);
	GPIO_PinRemapConfig(DCMI_D5_GPIO_PORT, DCMI_D5_GPIO_PIN, DCMI_D5_AF);
	GPIO_PinRemapConfig(DCMI_D6_GPIO_PORT, DCMI_D6_GPIO_PIN, DCMI_D6_AF);
	GPIO_PinRemapConfig(DCMI_D7_GPIO_PORT, DCMI_D7_GPIO_PIN, DCMI_D7_AF);
}

void Cameraclk_Configuration(void)
{
    uint32_t Period = 0;
    uint32_t PWM_HZ = 24000000;
    SYSCTRL_ClocksTypeDef clocks;
    TIM_PWMInitTypeDef TIM_PWMSetStruct;

    SYSCTRL_APBPeriphClockCmd( SYSCTRL_APBPeriph_TIMM0, ENABLE);
 
    SYSCTRL_GetClocksFreq(&clocks);
    
    /* Check PCLK, need >= 48MHz */
    if(clocks.PCLK_Frequency / 2 < PWM_HZ)
    {
        PWM_HZ = clocks.PCLK_Frequency / 2;
    }

	Period = clocks.PCLK_Frequency / PWM_HZ;                                                                                                                                                                               

	TIM_PWMSetStruct.TIM_LowLevelPeriod = (Period / 2 - 1);
	TIM_PWMSetStruct.TIM_HighLevelPeriod = (Period - TIM_PWMSetStruct.TIM_LowLevelPeriod - 2);
    
    TIM_PWMSetStruct.TIMx = CAM_XCK_TIM;
    TIM_PWMInit(TIMM0, &TIM_PWMSetStruct);

	GPIO_PinRemapConfig(CAM_XCK_GPIO, CAM_XCK_GPIO_PIN, GPIO_Remap_2);

	TIM_Cmd(TIMM0, CAM_XCK_TIM, ENABLE);
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
