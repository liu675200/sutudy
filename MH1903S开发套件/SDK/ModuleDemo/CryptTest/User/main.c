#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "mhscpu.h"
#include "mh_ecc.h"
#include "mhscpu_crc.h"
#include "mh_rsa.h"
#include "mh_rand.h"
#include "mh_bignum.h"
#include "mh_bignum_tool.h"
#include "mh_misc.h"
#include "mh_crypt_version.h"

#define CRYPT_INT						0

#define MH_MAX_RSA_MODULUS_WORDS		((MH_MAX_RSA_MODULUS_BYTES + 3) / 4)
#define MH_MAX_RSA_PRIME_WORDS			((MH_MAX_RSA_PRIME_BYTES + 3) / 4)


void UART_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);

extern void ecc_naf_test(void);
extern void ecc_naf_point_test(void);
extern void ecc_pdbl_a_test(void);
extern void ecc_pdbl_mj_test(void);
extern void ecc_padd_a_test(void);
extern void ecc_padd_ja_test(void);
extern void ecc_pmul_calc_test(void);
extern void rand_self_test(void);

extern void rsa_self_test(void);
extern void rsa_gen_key_self_test(void);
extern void rsa_complete_key_self_test(void);
extern void __mh_rsa_gen_key_test(void);
extern void __mh_rsa_efc_gen_key_test(void);
extern void mh_rsa_gen_key_time_test(void);
extern void mh_rsa_time_test(void);
extern void mh_sm4_time_test(void);
extern void mh_sm3_time_test(void);
extern void	mh_aes_time_test(void);
extern void mh_des_time_test(void);
extern void	mh_tdes_time_test(void);
extern void	mh_sha_time_test(void);
extern void test_aes(void);
extern void test_des(void);
extern void test_tdes(void);

extern void sm2_encrypt_decrypt_self_test(void);
extern void sm2_sign_self_test(void);
extern void sm2_key_exchange_self_test(void);

extern void test_SM3(void);
extern void test_SM4(void);
extern void test_SHA(void);
//extern void TestHmacSha1();

extern void mh_SM3_test_with_known_data(void);
extern void mh_SM4_test_with_known_data0(void);
extern void mh_SM4_test_with_known_data1(void);
extern void mh_SM4_test_with_known_data2(void);

extern void mem_cpy_test(void);

extern void mh_rsa_gen_efc_para_test(void);
extern void mh_rsa_efc_gen_prime_test(void);
extern void mh_sm2_time_test(void);
extern void mh_sm2_sign_time_test(void);

extern void TestStanderdSM2(void);

extern void ClientTest(void);


extern void AES_AutoRng_Test(void);
extern void DES_AutoRng_Test(void);
extern void TDES_AutoRng_Test(void);
extern void SHA_Test(void);
extern void SM3_Test(void);
extern void SM4_AutoRng_Test(void);
extern void ecies_Test(void);
extern void esdsa_Test(void);

uint8_t testtrand[512];
	

int main(void)
{
	uint32_t i,ver;
	SYSCTRL_PLLConfig(SYSCTRL_PLL_204MHz);
    SYSCTRL_PLLDivConfig(SYSCTRL_PLL_Div_None);
	SYSCTRL_HCLKConfig(SYSCTRL_HCLK_Div2);
	SYSCTRL_PCLKConfig(SYSCTRL_PCLK_Div2);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_GPIO, ENABLE);
	SYSCTRL_AHBPeriphClockCmd(SYSCTRL_AHBPeriph_CRYPT, ENABLE);
	SYSCTRL_AHBPeriphResetCmd(SYSCTRL_AHBPeriph_CRYPT, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TRNG, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_UART0, ENABLE);
	SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
	SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
	
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 |GPIO_Pin_1, GPIO_Remap_0);
	mh_crypt_it_clear();          // 加解密完成都会产生中断
	UART_Configuration();
	//GPIO_Configuration();
	mh_rand_init();               //硬件产生随机数
    NVIC_Configuration();
	ver = mh_crypt_version();
	
	DBG_PRINT("MegaHunt SCPU Secure Test Demo V1.0, secure lib version is V%02x.%02x.%02x.%02x\n", ver >> 24, (ver >> 16)&0xFF, (ver>>8)&0xFF, ver & 0xFF);
	DBG_PRINT("MegaHunt SCPU Crypt Test V1.0 start......\r\n");
	while(1)
	{
        AES_AutoRng_Test();
		DES_AutoRng_Test();
		TDES_AutoRng_Test();
		SHA_Test();
		SM3_Test();
		SM4_AutoRng_Test();
		
		sm2_encrypt_decrypt_self_test();
		sm2_sign_self_test();
		sm2_key_exchange_self_test();
		rsa_self_test();
		
		ecies_Test();
		esdsa_Test();
		
		DBG_PRINT("\r\n");
		DBG_PRINT("MegaHunt SCPU Crypt Test V1.0 finish.......\r\n");
		DBG_PRINT("MegaHunt SCPU Crypt Time Test Start.......\r\n");
	/***************************************************/
#if 0
		mh_aes_time_test();
		mh_des_time_test();
		mh_tdes_time_test();
		mh_sha_time_test();
		mh_rsa_gen_key_time_test();   //测试耗时长
		mh_rsa_time_test();
		mh_sm2_sign_time_test();
		mh_sm2_time_test();
		mh_sm3_time_test();
		mh_sm4_time_test();
 #endif
	/***************************************************/

	}

}

void GPIO_Reset(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
}

void GPIO_Set(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}

void GPIO_Toggle(void)
{
	uint32_t i = 0;
// 	GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	for(i = 0; i < 100;i++);
// 	GPIO_SetBits(GPIOA,GPIO_Pin_7);
}

void UART_Configuration(void)
{
	UART_InitTypeDef UART_InitStructure;

	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;

	UART_Init(UART0, &UART_InitStructure);	
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	GPIO_PinRemapConfig(GPIOA, GPIO_Pin_0 | GPIO_Pin_1, GPIO_Remap_0);
}


void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
    
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_3);
	
	NVIC_InitStructure.NVIC_IRQChannel = UART0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM0_0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_ClearPendingIRQ(TRNG_IRQn);
	NVIC_InitStructure.NVIC_IRQChannel = TRNG_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);

#if CRYPT_INT
	NVIC_ClearPendingIRQ(CRYPT0_IRQn);
	NVIC_InitStructure.NVIC_IRQChannel = CRYPT0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 	NVIC_Init(&NVIC_InitStructure);
#endif
}


int SER_PutChar (int ch) {

	while(!UART_IsTXEmpty(UART0));
	UART_SendData(UART0, (uint8_t) ch);
	return ch;
}

#if   defined ( __GNUC__ )
int _write(int fd, char *pBuffer, int size)  
{
	int i = 0;

	for (i = 0; i < size; i++)  
	{
		if ('\n' == pBuffer[i])
		{
			SER_PutChar('\r');  
		}
		SER_PutChar(pBuffer[i]);  
	} 

	return size;  
}  

int printf_GCC(char *fmt, ...)
{
	int ret;
	va_list varg;

	va_start(varg, fmt);
	ret = vprintf(fmt, varg);
	va_end(varg);
    
    fflush(stdout);
    
	return ret;
}

#elif defined ( __ICCARM__ ) || defined ( __CC_ARM )


int fputc(int c, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	if (c == '\n')  {
		SER_PutChar('\r');
	}
	return (SER_PutChar(c));
}
#endif


void printf_monobit(void)
{
	uint32_t i,j;
	uint32_t t;
	int32_t sum;
	for(i = 0; i <= 0xff; i++)
	{
		sum = 0;
		for(j = 0; j < 8; j++)
		{
			t = (i >> j) & 0x01;
			sum += 2*(int)t-1;
		}
		DBG_PRINT("%3d,",sum);
		if(15 == i % 16)
			DBG_PRINT("\n");
	}
	
}

void printf_byte_bit(void)
{
	uint32_t i,j;
	uint32_t t;
	int32_t sum;
	for(i = 0; i <= 0xff; i++)
	{
		sum = 0;
		for(j = 0; j < 8; j++)
		{
			t = (i >> j) & 0x01;
			sum +=t;
		}
		DBG_PRINT("%3d,",sum);
		if(15 == i % 16)
			DBG_PRINT("\n");
	}
	
}


void printf_byte_bit_runs(void)
{
	uint32_t i,j;
	int32_t sum;
	for(i = 0; i <= 0xff; i++)
	{
		sum = 0;
		for(j = 0; j < 7; j++)
		{
			if((i>>j & 0x03) == 0x01 || ((i>>j & 0x03) == 0x02))
			sum ++;
		}
		DBG_PRINT("%3d,",sum);
		if(15 == i % 16)
			DBG_PRINT("\n");
	}
}


void printf_long_runs_max(void)
{
	uint32_t i,j;
	int32_t sum;
	int32_t max;
	for(i = 0; i <= 0xff; i++)
	{
		sum = 0;max = 0;
		for(j = 0; j < 8; j++)
		{
			if(i>>j & 0x01)
			{
				sum ++;
				if( sum > max)
					max = sum;
			}
			else
				sum = 0;
		}
		DBG_PRINT("%3d,",max);
		if(15 == i % 16)
			DBG_PRINT("\n");
	}
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
