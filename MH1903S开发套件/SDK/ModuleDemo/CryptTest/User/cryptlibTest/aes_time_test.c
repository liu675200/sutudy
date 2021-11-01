#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define AES_TIME_TEST_DATA              1024*4
#define AES_TIME_TEST_COUNT             1

typedef struct
{
    uint8_t *   key;
    uint32_t    size;
    char *      name;
}aes_key_t;

typedef struct
{
    uint32_t    val;
    char *      name;
}var_name_t;

static mh_rng_callback f_rng = mh_rand_p;

void mh_aes_time_test(void)
{
    uint32_t i, j, k;
    var_name_t modes[2] =   {{ECB, "ECB"},
                            {CBC, "CBC"}};
    TIME_MS_US time;

    aes_key_t keys[3] =     {{NULL, MH_AES_128, "KEY_128"},
                            {NULL, MH_AES_192, "KEY_192"},
                            {NULL, MH_AES_256, "KEY_256"}};

    uint8_t plain[2][AES_TIME_TEST_DATA];
    uint8_t cipher[AES_TIME_TEST_DATA];

    uint8_t key_128[] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
                        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    uint8_t key_192[] = {0x8E, 0x73, 0xB0, 0xF7, 0xDA, 0x0E, 0x64, 0x52, 
                        0xC8, 0x10, 0xF3, 0x2B, 0x80, 0x90, 0x79, 0xE5, 
                        0X62, 0xF8, 0xEA, 0xD2, 0x52, 0x2C, 0x6B, 0x7B};
    uint8_t key_256[] = {0x60, 0x3D, 0xEB, 0x10, 0x15, 0xCA, 0x71, 0xBE, 
                        0x2B, 0x73, 0xAE, 0xF0, 0x85, 0x7D, 0x77, 0x81, 
                        0x1F, 0x35, 0x2C, 0x07, 0x3B, 0x61, 0x08, 0xD7, 
                        0x2D, 0x98, 0x10, 0xA3, 0x09, 0x14, 0xDF, 0xF4};
        
    uint8_t iv[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

					
	uint32_t ret = 0;
    keys[0].key = key_128;
    keys[1].key = key_192;
    keys[2].key = key_256;
    
    memset(plain, 0, sizeof(plain));
    f_rng(plain[0], sizeof(plain[0]), NULL);
                    
    i = 0xfffff;
    while(i--);
                    
    printf("\n%d bytes data size\n", AES_TIME_TEST_DATA);

    //key 128 192 256
    for (i = 0; i < 3; i++)
    {
        //pack mode ECB CBC
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < AES_TIME_TEST_COUNT; k++)
            {
                memset(cipher, 0, sizeof(cipher));
                memset(plain[1], 0, sizeof(plain[1]));
                
                //AES enc
                timer_init_ms(1);
                ret = mh_aes_enc((mh_pack_mode_def)modes[j].val, cipher, sizeof(cipher), plain[0], sizeof(plain[0]), keys[i].key, (mh_aes_key_size_def)keys[i].size, iv, f_rng, NULL);
                get_time(&time);
                printf("%dms %dus AES %s %s encrypt time\n", time.u32Ms, time.u32Us, modes[j].name, keys[i].name);
                //AES dec
                timer_init_ms(1);
                ret = mh_aes_dec((mh_pack_mode_def)modes[j].val, plain[1], sizeof(plain[1]), cipher, sizeof(cipher), keys[i].key, (mh_aes_key_size_def)keys[i].size, iv, f_rng, NULL);
                get_time(&time);
                printf("%dms %dus AES %s %s decrypt time\n", time.u32Ms, time.u32Us, modes[j].name, keys[i].name);

                if(!memcmp(plain[0], plain[1], sizeof(plain[0])))
                {
//                  printf("AES %s %s Test Pass...\n", cmodes[j], ckey_size[i]);
                }
                else
                {
                    
                    printf("AES %s %s Test Fail...\n", modes[j].name, keys[i].name);
					while (1);
                }
            }
        }
    }
}
