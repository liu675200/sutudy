#include "inc.h"
void NVIC_Configuration(IRQn_Type IRQ,int Preemption,int Sub);
void Usart_Init()
{
  uint32_t tmpreg = 0x00, apbclock = 0x00;
  uint32_t integerdivider = 0x00;
  uint32_t fractionaldivider = 0x00;
  //GPIOA�ĵ�ַ����RCC_APB2ENR��ַ�ĵ�3λ��usart�ǵ�14λ
  //ʹ��GPIOAʱ��
  RCC_APB2ENR |= (1<<2);
  //��USART1ʱ��
  RCC_APB2ENR |= (1<<14);

  //��տ���PA9��PA10�Ķ˿�λ
  GPIOA_CRH &= ~( 0x0F<< (4*1));
  GPIOA_CRH &= ~( 0x0F<< (4*2));

  //����PA9���츴��ģʽ��50MHz    1011
  GPIOA_CRH |= (11<<1*4);
  //����PA10��������ģʽ  0100
  GPIOA_CRH |= (4<<2*4);

  //����ֹͣλ����
  USART1_CR2 &= 0xCFFF;  //����STOPλ
  USART1_CR2 &= ~(STOP<<12);

  //����CR1
  USART1_CR1 &= 0xE9F3;  
  //����֡����
  USART1_CR1 |= (M<<12);
  //ʹ��У��λ
  USART1_CR1 &= ~(PCE<<10);
  //����У��λ
  //USART1_CR1 |= (PS<<9);
  //����ģʽ
  USART1_CR1 |= (TE<<3);
  USART1_CR1 |= (RE<<2);
  //Ӳ��������
  USART1_CR3 &= 0xFCFF;
  USART1_CR3 |= (CTSE<<9);
  USART1_CR3 |= (RTSE<<8);

  //���ò�����
  apbclock = PCLK2_Frequency;
  integerdivider = ((25*apbclock)/(4*BAUDRATE));
  tmpreg = (integerdivider / 100) << 4;
  fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
  tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
  USART1_BRR = (uint16_t)tmpreg;

//-------------------------�ж�����------
  //NVIC_Configuration(USART1_IRQn,1,1);
  //USART1_CR1 |= (1<<5); //ʹ�ܽ����ж�
  //UEʹ��
  USART1_CR1 |= (1<<13);
}
/*******************����һ���ֽ�**************************/
void USART_SendData(uint16_t Data)
{
  USART1_DR = ((uint16_t)Data & (uint16_t)0xFF);
  while ((USART1_SR & (1<<7)) == 0);
}

/*****************�����ַ���**********************/
void USART_SendString(char *str)
{
	unsigned int k=0;
  do 
  {
      USART_SendData(*(str + k));
      k++;
  } while(*(str + k)!='\0');
  
  while((USART1_SR & (1<<6)) == 0);
}
void USART_SendHalfWord(uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	temp_h = ((uint16_t)(ch)&0XFF00)>>8;
	temp_l = (uint16_t)(ch)&0XFF;
	
	USART_SendData(temp_h);	
	USART_SendData(temp_l);	
}


//���ڲ���
void Usart_test(void)
{
	uint16_t ch;
	
	//ԭ������
	while (1)
	{
		if((USART1_SR & (1<<5)) != 0)	//�����ݷǿ�
		{
			ch = (uint16_t)(USART1_DR & (uint16_t)0x01FF);//�յ��ź�
			USART_SendData(ch);
		}
	}
/*
	//���͵��ֽ�
	while(1)
	{	
		if((USART1_SR & (1<<5)) != 0)	//�����ݷǿ�
		{
			ch = (char)(USART1_DR & (uint16_t)0x01FF);//�յ��ź�
			switch(ch)
			{
				case '1':
					USART_SendHalfWord(ACK);
					break;
				case '2':
					//USART_SendString("10101");
					USART_SendHalfWord((uint16_t)(111));
					break;
				default:
					USART_SendHalfWord((uint16_t)(58));
					break;
			}
		}
	}
*/
}