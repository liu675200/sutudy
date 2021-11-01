#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

typedef struct
{
    uint32_t    val;
    char *      name;
}var_name_t;

#define SHA_TIME_TEST_DATA              1024 * 10

static mh_rng_callback f_rng = mh_rand_p;

void mh_sha_time_test(void)
{
    uint32_t i;
    var_name_t modes[5] =   {{SHA_160, "SHA_160"},
                            {SHA_224, "SHA_224"},
                            {SHA_256, "SHA_256"},
                            {SHA_384, "SHA_384"},
                            {SHA_512, "SHA_512"}};
    
    uint8_t plain[SHA_TIME_TEST_DATA];
    uint8_t cipher[512 /8];
    
    TIME_MS_US time;

    memset(plain, 0, sizeof(plain));
    f_rng(plain, sizeof(plain),NULL);

    printf("\n%d bytes data size\n", SHA_TIME_TEST_DATA);

    for (i = 0; i < 5; i++)
    {
        
        memset(cipher, 0, sizeof(cipher));
        timer_init_ms(1);
        mh_sha((mh_sha_mode_def)modes[i].val, cipher, plain, sizeof(plain));
        get_time(&time);
        printf("%d ms %d us time %s Test\n", time.u32Ms, time.u32Us, modes[i].name);
    }
}

