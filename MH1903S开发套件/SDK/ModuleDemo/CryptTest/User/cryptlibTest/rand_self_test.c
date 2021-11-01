#include <string.h>
#include <stdio.h>
#include "mhscpu_uart.h"
#include "mhscpu_timer.h"
#include "mh_crypt.h"
#include "mh_aes.h"
#include "timer.h"
#include "mh_bignum_tool.h"
#include "mh_rand.h"
#include "debug.h"

extern double mh_frequency(uint8_t *, uint32_t);
extern double mh_block_frequency(uint8_t *, uint32_t);
extern double mh_runs(uint8_t *, uint32_t);
extern double mh_longest_run_of_ones(uint8_t *, uint32_t);

void rand_self_test(void)
{
	uint32_t i;
	uint32_t rand[256]; 
	
	for(i = 1; i < 64; i++)
	{
		memset(rand, 0, sizeof(rand));
		mh_rand(rand, i);
		bn_printf(rand, sizeof(rand)/4);
		printf("\n");
	}
}

void rand_frequency_test(void)
{
	double r;
//	uint8_t s[] = {0x0c,0x90,0xfd,0xaa,0x22,0x16,0x8c,0x23,0x4c,0x4c,0x66,0x28,0xb8};
	uint8_t s[] = {0xb8,0x28,0x66,0x4c,0x4c,0x23,0x8c,0x16,0x22,0xaa,0xfd,0x90,0x0c};
	printf("rand_frequency_test\n");
	r = mh_frequency(s, sizeof(s));
	printf("except val is:   0.049860\n");
	printf("caculate val is: %f\n\n", r);

	
}

void rand_block_frequency_test(void)
{
	
	double r;
//	uint8_t s[] = {0xb8,0x28,0x66,0x4c,0x4c,0x23,0x8c,0x16,0x22,0xaa,0xfd,0x90,0x0c,0x0d,0xdf,0xae,0xbc,0x42,0x95,0x63,0xdb,0xfa,0x4d,0x5a,0x0c};
	uint8_t s[] = {0xb8,0x28,0x66,0x4c,0x4c,0x23,0x8c,0x16,0x22,0xaa,0xfd,0x90,0x0c};
	printf("rand_block_frequency_test\n");
	r = mh_block_frequency(s, sizeof(s));
	printf("except val is:   0.307353\n");
	printf("caculate val is: %f\n\n", r);
}

void rand_runs_test(void)
{
	
	double r;
	uint8_t s[] = {0xb8,0x28,0x66,0x4c,0x4c,0x23,0x8c,0x16,0x22,0xaa,0xfd,0x90,0x0c};
	printf("rand_runs_test\n");
	r = mh_runs(s, sizeof(s));
	printf("except val is:   0.695332\n");
	printf("caculate val is: %f\n\n", r);
}

void rand_longest_run_of_ones_test(void)
{
	
	double r;
	uint8_t s[] = {0xb2, 0xd8, 0xe6, 0xcc,0xd7,0x10,0xd6,0x13,0x51,0x4d,0x02,0xe0,0x4c,0x6c,0x15,0xcc};
	printf("rand_longest_run_of_ones_test\n");
	r = mh_longest_run_of_ones(s, sizeof(s));
	printf("except val is:   0.180609\n");
	printf("caculate val is: %f\n\n", r);
}


void test_rand_time(void)
{
	uint32_t dbg_t = 0;
	uint32_t fclk = 0;
	uint8_t rand[512]; 
	fclk = mh_dbg_fclk_get();
	BEGIN_TEST();
	mh_rand(rand, sizeof(rand));
	END_TEST();
	printf("%d bytes rand data use %dus\n", sizeof(rand), dbg_t/(fclk/1000000));
	
}


