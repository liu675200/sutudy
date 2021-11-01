#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define SM3_TIME_TEST_DATA              1024 * 10
#define SM3_TIME_TEST_COUNT             8


static mh_rng_callback f_rng = mh_rand_p;

void mh_sm3_time_test(void)
{
    uint32_t i;
    
    TIME_MS_US time, timeTotal = { 0 };
    
    uint8_t plain[SM3_TIME_TEST_DATA];
    uint8_t cipher[32];
	uint8_t PN[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};
    
    memset(cipher, 0, sizeof(cipher));
    memset(plain, 0, sizeof(plain));
    f_rng(plain, sizeof(plain),NULL);

    printf("\n%d bytes data size\n", SM3_TIME_TEST_DATA);

    for(i = 0; i < SM3_TIME_TEST_COUNT; i++)
    {
        timer_init_ms(1);
        mh_sm3(cipher, plain, sizeof(plain));
        get_time(&time);
        time_add(&time, &timeTotal, &timeTotal);
        printf("%dms %dus time SM3 Test\n", time.u32Ms, time.u32Us);
    }
    printf("\n");
    time_avg(&timeTotal, SM3_TIME_TEST_COUNT, &time);
    printf("Total %dms %dus time_sum SM3 Test\n", timeTotal.u32Ms, timeTotal.u32Us);
    printf("avg %dms %dus time_sum SM3 Test\n", timeTotal.u32Ms, timeTotal.u32Us);
    printf("\n");
}
