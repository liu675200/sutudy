#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define ADC_BUFFER_SIZE     2000            //��������
#define WINDOWN_WIDTH   80  

typedef struct
{
    uint16_t X_Axis;
    uint16_t Y_Axis;
}Type_TableDataDef;

typedef struct
{
    Type_TableDataDef *Table;
    uint16_t TableSize;
    uint16_t Frequency_MAX;
    uint16_t Frequency_MIN;
}Type_HistogramDef;

uint16_t ADC_Buffer[ADC_BUFFER_SIZE];

void Delay(void);
void NVIC_Configuration(void);
void UART_Configuration(void);
void ADC_Configuration(void);
void AnalyzeData(Type_HistogramDef *Histogram, uint16_t *buf, uint16_t BufLen);
uint8_t AddX_AxisData(Type_HistogramDef *Histogram, uint16_t X_data);
void PrintHistogram(Type_HistogramDef *Histogram);
void Sequence(Type_HistogramDef *Histogram);

int main(void)
{
    uint32_t i, u32_PointNumber = 200;
    Type_HistogramDef Histogram;

	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC | SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0 | SYSCTRL_APBPeriph_ADC, ENABLE);
        
    UART_Configuration();
    NVIC_Configuration();
	ADC_Configuration(); 
	
    ADC_StartCmd(ENABLE);
    ADC_BuffCmd(ENABLE);
    ADC_DivResistorCmd(ENABLE);
	
    Delay();
	printf("MegaHunt SCPU ADC Demo V1.0.\n");
    printf("Start work.\n");
    
    for(i = 0; i < u32_PointNumber; i++)
    {
        ADC_Buffer[i] = ADC_GetResult();
    }
	
    for (i = 0; i < u32_PointNumber; i++)
    {
        if (ADC_CHANNEL_CHARGE_VBAT == ADC_GetChannel())
        {
            printf("ADC %3d Value is :%d \t %.3fV \r\n", i, ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1880) * 14 / 5) / 1000);
        }
        else
        {
            if (DISABLE == ADC_IsDivResistorEnable())
            {
                printf("ADC %3d Value is :%d \t %.3fV \r\n", i, ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1880)) / 1000);
            }
            else
            {
                if (ADC_CHANNEL_6 == ADC_GetChannel())
                {
                    printf("ADC %3d Value is :%d \t %.3fV \r\n", i, ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1880) * 1511 / 558) / 1000);
                }
                else
                {
                    printf("ADC %3d Value is :%d \t %.3fV \r\n", i, ADC_Buffer[i], ((float)ADC_CalVoltage(ADC_Buffer[i], 1880) * 2) / 1000);
                }
                
            }
            
        }
		
    }
    
    AnalyzeData(&Histogram, ADC_Buffer, u32_PointNumber);
    
	while(1) 
    {
    }
}
void Delay(void)
{
    uint32_t number = 0x5FFFFF;
    while(number--);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = ADC0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
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

void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStruct;
    
	GPIO_PinRemapConfig(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Remap_2);
    GPIO_PullUpCmd(GPIOC,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5,DISABLE);
    
    //ADC channel ChargeVbat��Charge_Vbat�ܽţ�channel1��ӦPC0��channel2��ӦPC1���Դ�����
    ADC_InitStruct.ADC_Channel = ADC_CHANNEL_6;
    ADC_InitStruct.ADC_SampSpeed = ADC_SpeedPrescaler_2;
    ADC_InitStruct.ADC_IRQ_EN = DISABLE;
    ADC_InitStruct.ADC_FIFO_EN = DISABLE;

    ADC_Init(&ADC_InitStruct);
}

//���㷽��,��ֵ,�����Сֵ,ͳ��Ƶ�ʷֲ�
void AnalyzeData(Type_HistogramDef *Histogram, uint16_t *buf, uint16_t BufLen)
{
    uint16_t i, max, min;
    double Sum ,f16Average;
    uint16_t RemainTableSize, ret;
    Sum = 0;
    max = 0;
    min = 4096;
    
    for (i = 0; i < BufLen; i++)
    {
        Sum += buf[i];
        if (buf[i] > max)   max = buf[i];
        if (buf[i] < min)   min = buf[i];

        if (RemainTableSize > (ret = AddX_AxisData(Histogram, buf[i])))       //������ݵ�Ƶ��ͳ�Ʊ�
        {
            RemainTableSize = ret;
        }
    }
    f16Average = Sum / BufLen;

    Sum = 0;
    for (i = 0; i < BufLen; i++)            //���㷽��
    {
        Sum += ( (double)buf[i] - f16Average) * ( (double)buf[i] - f16Average);
    }
    
    printf("+-----------------------+\r\n");
    printf("* ������:\t  %6d\r\n", BufLen);
    printf("* ���ֵ:\t  %6d\r\n", max);
    printf("* ��Сֵ:\t  %6d\r\n", min);
    printf("* ��ֵ:\t\t  %6.2f\r\n", f16Average);
    printf("* ����:\t\t  %6.4f\r\n", Sum / BufLen);
    printf("+-----------------------+\r\n");
}

//���Ƶ��ͳ������
uint8_t AddX_AxisData(Type_HistogramDef *Histogram, uint16_t X_data)
{
    uint16_t i, isFindX_Data = 0;

    for (i = 0; i < Histogram->TableSize && (0 != Histogram->Table[i].Y_Axis); i++)
    {
        if (X_data == Histogram->Table[i].X_Axis)
        {
            Histogram->Table[i].Y_Axis ++;
            isFindX_Data = 1;
            break;
        }
    }
    if (!isFindX_Data)
    {
        Histogram->Table[i].X_Axis = X_data;
        Histogram->Table[i].Y_Axis ++;
    }
    if (Histogram->Table[i].Y_Axis > Histogram->Frequency_MAX)  Histogram->Frequency_MAX = Histogram->Table[i].Y_Axis;
    if (Histogram->Table[i].Y_Axis < Histogram->Frequency_MIN)  Histogram->Frequency_MIN = Histogram->Table[i].Y_Axis;
    
    return Histogram->TableSize - i - 1;    //����ʣ��ռ�
}

//���Ƶ�ʷֲ���
void PrintHistogram(Type_HistogramDef *Histogram)
{
    uint16_t i, j;
    float UnitLength = (float)WINDOWN_WIDTH / (float)(Histogram->Frequency_MAX - Histogram->Frequency_MIN); //���������ڷ�Χ

    Sequence(Histogram);    //��Ƶ�ʽ�������
    printf("\r\n+");
    for (i = 0; i < WINDOWN_WIDTH + 15; i++)
    {
        printf("-");
    }
    printf("\r\n");
    for (i = 0; i < Histogram->TableSize; i++)
    {
        if (0UL == Histogram->Table[i].Y_Axis)  //���ݴ�ӡ����
        {
            break;
        }
        else
        {
            printf("*%d\t| ", Histogram->Table[i].X_Axis);
            for (j = 0; j < (UnitLength * Histogram->Table[i].Y_Axis + 0.5); j++)
            {
                printf("o");
            }
            printf("  %d\r\n", Histogram->Table[i].Y_Axis);
        }
    }
    printf("+");
    for (i = 0; i < WINDOWN_WIDTH + 15; i++)
    {
        printf("-");
    }
    printf("\r\n");
}

//ֱ�Ӳ�������,��Ƶ�ʷֲ�����������ʾ
void Sequence(Type_HistogramDef *Histogram)
{
    int16_t i,j;
    Type_TableDataDef TableTmp;
    
    for(j = 1; j < Histogram->TableSize && 0 != Histogram->Table[j].Y_Axis ; j++) //ֱ�Ӳ�������
    {
        TableTmp = Histogram->Table[j];
        i = j - 1;
        //ע:��������˳�򲻿ɵ���
        while(i >= 0 && Histogram->Table[i].X_Axis > TableTmp.X_Axis)     //�Ӵ�С: < ;��С����: >
        {                                                                 //Histogram->Table[i].Y_Axis > TableTmp.Y_Axis��Ƶ��ֵ��������
            Histogram->Table[i + 1] = Histogram->Table[i];
            i--;
        }
        Histogram->Table[i + 1] = TableTmp;
    }
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
        printf("%s %s %d \r\n", __func__, file, line);
	}
}
#endif
