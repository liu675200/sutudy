#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DES_TIME_TEST_DATA              1024*4
#define DES_TIME_TEST_COUNT             1

typedef struct
{
    uint32_t    val;
    char *      name;
}var_name_t;

static mh_rng_callback f_rng = mh_rand_p;

void mh_des_time_test(void)
{
    uint32_t i, j;
    var_name_t modes[2] =   {{ECB, "ECB"},
                            {CBC, "CBC"}};
    TIME_MS_US time;
    
    uint8_t plain[2][DES_TIME_TEST_DATA];
    uint8_t cipher[DES_TIME_TEST_DATA];
        
    uint8_t iv[] = {0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17};
    uint8_t key[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};


    memset(plain, 0, sizeof(plain));
    f_rng(plain[0], sizeof(plain[0]), NULL);

    i = 0xFFFFF;
    while(i--);

    printf("\n%d bytes data size\n", DES_TIME_TEST_DATA);

    //pack mode ECB CBC
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < DES_TIME_TEST_COUNT; j++)
        {
            memset(cipher, 0, sizeof(cipher));
            memset(plain[1], 0, sizeof(plain[1]));

            timer_init_ms(1);
            mh_des_enc((mh_pack_mode_def)modes[i].val, cipher, sizeof(cipher), plain[0], sizeof(plain[0]), key, iv, f_rng, NULL);
            get_time(&time);
            printf("%d ms %dus DES %s encrypt time\n", time.u32Ms, time.u32Us, modes[i].name);

            timer_init_ms(1);
            mh_des_dec((mh_pack_mode_def)modes[i].val, plain[1], sizeof(plain[1]), cipher, sizeof(cipher), key, iv, f_rng, NULL);
            get_time(&time);
            printf("%d ms %dus DES %s decrypt time\n", time.u32Ms, time.u32Us, modes[i].name);
            
            if(!memcmp(plain[0], plain[1], sizeof(plain[0])))
            {
//              printf("DES %s Test Pass...\n", cmodes[i]);
            }
            else
            {
                printf("DES %s Test Fail...\n", modes[i].name);
            }
        }
    }
}


void mh_tdes_time_test(void)
{
    uint32_t i, j;
    TIME_MS_US time, timeTotal[2] = { 0 };
    var_name_t modes[2] =   {{ECB, "ECB"},
                            {CBC, "CBC"}};
    
    uint8_t plain[2][DES_TIME_TEST_DATA];
    uint8_t cipher[DES_TIME_TEST_DATA];

    uint8_t iv[] = {0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17};
    mh_tdes_key_def key = {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
                        {0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01},
                        {0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23}};


    memset(plain, 0, sizeof(plain));
    mh_rand(plain[0], sizeof(plain[0]));

    i = 0xFFFF;
    while(i--);

    printf("\n%d bytes data size\n", DES_TIME_TEST_DATA);

    //pack mode ECB CBC
    for (i = 0; i < 2; i++)
    {
        memset(timeTotal, 0, sizeof(timeTotal));
        for (j = 0; j < DES_TIME_TEST_COUNT; j++)
        {
            memset(cipher, 0, sizeof(cipher));
            memset(plain[1], 0, sizeof(plain[1]));

            timer_init_ms(1);
            mh_tdes_enc((mh_pack_mode_def)modes[i].val, cipher, sizeof(cipher), plain[0], sizeof(plain[0]), &key, iv, f_rng, NULL);
            get_time(&time);
            time_add(&time, timeTotal + 0, timeTotal + 0);
            printf("%d ms %dus TDES %s encrypt time\n", time.u32Ms, time.u32Us, modes[i].name);

            timer_init_ms(1);
            mh_tdes_dec((mh_pack_mode_def)modes[i].val, plain[1], sizeof(plain[1]), cipher, sizeof(cipher), &key, iv, f_rng, NULL);
            get_time(&time);
            time_add(&time, timeTotal + 1, timeTotal + 1);
            printf("%d ms %dus TDES %s decrypt time\n", time.u32Ms, time.u32Us, modes[i].name);

            if(!memcmp(plain[0], plain[1], sizeof(plain[0])))
            {
//              printf("TDES %s Test Pass...\n", cmodes[i]);
            }
            else
            {
                printf("TDES %s Test Fail...\n", modes[i].name);
            }
        }
        time_avg(timeTotal + 0, DES_TIME_TEST_COUNT, &time);
        printf("total %dms %dus TDES %s encrypt time\n", timeTotal[0].u32Ms, timeTotal[0].u32Us, modes[i].name);
        printf("\r\navg %dms %dus TDES %s encrypt time\r\n\r\n", time.u32Ms, time.u32Us, modes[i].name);
        
        time_avg(timeTotal + 1, DES_TIME_TEST_COUNT, &time);
        printf("%dms %dus TDES %s decrypt time\n", timeTotal[1].u32Ms, timeTotal[1].u32Us, modes[i].name);
        printf("\r\navg %dms %dus TDES %s decrypt time\r\n\r\n", time.u32Ms, time.u32Us, modes[i].name);
    }
}

