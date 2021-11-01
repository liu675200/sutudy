#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

typedef struct
{
    uint32_t    val;
    char *      name;
}var_name_t;

#define SM4_TIME_TEST_DATA				1024 * 4
#define SM4_TIME_TEST_COUNT				8

static mh_rng_callback f_rng = mh_rand_p;

void mh_sm4_time_test(void)
{
    uint32_t i, j;
    TIME_MS_US time[2], timeTotal[2] = { 0 };
    var_name_t modes[2] =   {{ECB, "ECB"},
                            {CBC, "CBC"}};
    
    uint8_t plain[2][SM4_TIME_TEST_DATA];
    uint8_t cipher[SM4_TIME_TEST_DATA];

    uint8_t key[16] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};
    uint8_t iv[16] = {0xA3, 0xB1, 0xBA, 0xC6, 0x56, 0xAA, 0x33, 0x50, 0x67, 0x7D, 0x91, 0x97, 0xB2, 0x70, 0x22, 0xDC};


    memset(plain, 0, sizeof(plain));
    f_rng(plain[0], sizeof(plain[0]), NULL);

    printf("\n%d bytes data size\n", SM4_TIME_TEST_DATA);

    //pack mode ECB CBC
    for (i = 0; i < 2; i++)
    {
        memset(timeTotal, 0, sizeof(timeTotal));
        for (j = 0; j < SM4_TIME_TEST_COUNT; j++)
        {
            memset(cipher, 0, sizeof(cipher));
            memset(plain[1], 0, sizeof(plain[1]));
            
            timer_init_ms(1);
            mh_sm4_enc((mh_pack_mode_def)modes[i].val, cipher, sizeof(cipher), plain[0], sizeof(plain[0]), key, iv, mh_rand_p, NULL);
            get_time(time);
            time_add(time + 0, timeTotal + 0, timeTotal + 0);
            
            timer_init_ms(1);
            mh_sm4_dec((mh_pack_mode_def)modes[i].val, plain[1], sizeof(plain[1]), cipher, sizeof(cipher), key, iv, mh_rand_p, NULL);
            get_time(time + 1);
            time_add(time + 1, timeTotal + 1, timeTotal + 1);
            
            if(!memcmp(plain[0], plain[1], sizeof(plain[0])))
            {
//              printf("SM4 %s Test Pass...\n", cmodes[i]);
            }
            else
            {
                printf("SM4 %s Test Fail...\n", modes[i].name);
            }
            printf("%dms %dus SM4 %s encrypt time\n", time[0].u32Ms, time[0].u32Us, modes[i].name);
            printf("%dms %dus SM4 %s decrypt time\n", time[1].u32Ms, time[1].u32Us, modes[i].name);
        }
        printf("\n");
        time_avg(timeTotal + 0, SM4_TIME_TEST_COUNT, time + 0);
        time_avg(timeTotal + 1, SM4_TIME_TEST_COUNT, time + 1);
        printf("Total %dms %dus avg %dms %dus SM4 %s encrypt time_sum\n", 
               timeTotal[0].u32Ms, timeTotal[0].u32Us,  time[0].u32Ms, time[0].u32Us, modes[i].name);
        printf("Total %dms %dus avg %dms %dus SM4 %s decrypt time_sum\n", 
               timeTotal[1].u32Ms, timeTotal[1].u32Us,  time[1].u32Ms, time[1].u32Us, modes[i].name);
        printf("\n");

    }
}

