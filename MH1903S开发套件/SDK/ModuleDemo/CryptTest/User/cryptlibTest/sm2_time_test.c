#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define SM2_TIME_TEST_DATA              128
#define SM2_TIME_TEST_COUNT             1
#define SM2_TIME_TEST_SIGN_DATA         128
#define SM2_TIME_TEST_SIGN_COUNT        1

static mh_rng_callback f_rng = mh_rand_p;

#define KEY_WORDS 256 / 32



#if 0
#define cp      "8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3"
#define ca      "787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498"
#define cb      "63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A"
#define cGx     "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D"
#define cGy     "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2"
#define cn      "8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7"

#define crand   "4C62EEFD6ECFC2B95B92FD6C3D9575148AFA17425546D49018E5388D49DD7B4F"

#define ckeyd   "1649AB77A00637BD5E2EFE283FBF353534AA7F7CB89463F208DDBC2920BB0DA0"
#define cex     "435B39CCA8F3B508C1488AFC67BE491A0F7BA07E581A0E4849A5CF70628A7E0A"
#define cey     "75DDBA78F15FEECB4C7895E2C1CDF5FE01DEBB2CDBADF45399CCF77BBA076A42"

#else

#define cp      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define ca      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC"
#define cb      "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
#define cGx     "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"
#define cGy     "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0"
#define cn      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"

//1??????32Byte                
#define cex     "1CDFD6D0375D84164EA5CBCD929874F19384743E20407EF1CCFDC07D960643F7"
//1???o?¡ì?32Byte                
#define cey     "F0053CCD7FE91230A12003A084964928DE12DC97E6F51F2B46C43935DB9F7857"
//????                
#define ckeyd   "C46841E769D7F6A7599046C029900CB6D47BBD54BE83AF50284584BC1E390BF9"


#endif




void mh_sm2_time_test_read_para(mh_sm2_ellipse_para *para)
{
    bn_read_string_to_bytes(para->p, MH_SM2_KEY_BYTES, cp);
    bn_read_string_to_bytes(para->a, MH_SM2_KEY_BYTES, ca);
    bn_read_string_to_bytes(para->b, MH_SM2_KEY_BYTES, cb);
    bn_read_string_to_bytes(para->n, MH_SM2_KEY_BYTES, cn);
    bn_read_string_to_bytes(para->g.x, MH_SM2_KEY_BYTES, cGx);
    bn_read_string_to_bytes(para->g.y, MH_SM2_KEY_BYTES, cGy);
}
void mh_sm2_time_test_read_key(mh_sm2_private_key *key)
{
    bn_read_string_to_bytes(key->d, MH_SM2_KEY_BYTES, ckeyd);
    bn_read_string_to_bytes(key->e.x, MH_SM2_KEY_BYTES, cex);
    bn_read_string_to_bytes(key->e.y, MH_SM2_KEY_BYTES, cey);
}


void mh_sm2_time_test(void)
{
    mh_sm2_ellipse_para para;
    mh_sm2_private_key key, keyTmp;
    uint32_t i, t;

    unsigned char plain0[SM2_TIME_TEST_DATA];
    unsigned char plain1[SM2_TIME_TEST_DATA];
    unsigned char cipher[SM2_TIME_TEST_DATA + 65 + 32];
    
    uint32_t mlen;
    uint32_t clen;
    uint8_t rand[32];
    TIME_MS_US time, timeTotal[2] = { 0 };
    
    mh_sm2_time_test_read_para(&para);
    mh_sm2_time_test_read_key(&key);
    
    printf("\n%d bytes data size\n", SM2_TIME_TEST_DATA);
	
	timer_init_ms(1);
	sm2_genkey(&keyTmp, &para, mh_rand_p, NULL);
	get_time(&time);
	printf("%dms %dus sm2 genkey time\n", time.u32Ms, time.u32Us);
	
    memset(&keyTmp, 0, sizeof(keyTmp));
    memcpy(keyTmp.d, key.d, sizeof(key.d));
    
    timer_init_ms(1);
    sm2_completeKey(&keyTmp, &para, mh_rand_p, NULL);
    get_time(&time);
    
    r_printf((0 == memcmp(key.e.x, keyTmp.e.x, sizeof(key.e.x))), "ecc_completeKey X\n");
    r_printf((0 == memcmp(key.e.y, keyTmp.e.y, sizeof(key.e.y))), "ecc_completeKey Y\n");
    printf("%dms %dus complete key time\n", time.u32Ms, time.u32Us);
    
    memset(timeTotal, 0, sizeof(timeTotal));
    for (i = 0; i < SM2_TIME_TEST_COUNT; i++)
    {
        f_rng(rand, sizeof(rand), NULL);

        memset(plain0, 0, sizeof(plain0));
        memset(plain1, 0, sizeof(plain1));
        memset(cipher, 0, sizeof(cipher));
        
        f_rng(plain0, sizeof(plain0), NULL);

        timer_init_ms(1);
        t = mh_sm2_enc(cipher, &clen, plain0, sizeof(plain0), &key.e ,&para, mh_rand_p, NULL);
        get_time(&time);
        time_add(&time, timeTotal + 0, timeTotal + 0);
        printf("%dms %dus sm2 encrypt time\n", time.u32Ms, time.u32Us);

        if(MH_RET_SM2_ENC_SUCCESS == t)
        {
            timer_init_ms(1);
            mh_sm2_dec(plain1, &mlen, cipher, clen, &key ,&para, mh_rand_p, NULL);
            get_time(&time);
            time_add(&time, timeTotal + 1, timeTotal + 1);
            printf("%dms %dus sm2 decrypt time\n", time.u32Ms, time.u32Us);
            if(0 == memcmp(plain1, plain0, sizeof(plain0)))
            {
//              printf("sm2 encrypt decrypt pass\n");
            }
            else
            {
                printf("sm2 encrypt decrypt err\n");
                while(1);
            }
        }
        else
        {
            printf("encrypt err\n");
            while(1);
        }
    }

    time_avg(timeTotal + 0, SM2_TIME_TEST_COUNT, &time);
    printf("avg %dms %dus sm2 encrypt time\n", time.u32Ms, time.u32Us);
    time_avg(timeTotal + 1, SM2_TIME_TEST_COUNT, &time);
    printf("avg %dms %dus sm2 decrypt time\n", time.u32Ms, time.u32Us);
    
}



#define cIDa    "414C4943453132333435363738394142434445464748405941484F4F2E434F4D"

#define cm      "115960C5598A2797C2D02D76D468CDC9"

void mh_sm2_sign_time_test(void)
{
    uint32_t t, i;
    mh_sm2_ellipse_para para;
    mh_sm2_private_key key;
    mh_sm2_sign sign;

    uint8_t message[SM2_TIME_TEST_SIGN_DATA];

    uint8_t Za[32];
    uint8_t rand[32];
    uint16_t u16MLen, u16IDlen,ENTLa;
    uint8_t IDa[128];
    
    TIME_MS_US time, timeTotal[2] = { 0 };

    
    mh_sm2_time_test_read_para(&para);
    mh_sm2_time_test_read_key(&key);
    
    printf("\n%d bytes data size\n", SM2_TIME_TEST_SIGN_DATA);
    

    u16MLen = bn_read_string_from_head(message, sizeof(message), cm);
    u16IDlen = bn_read_string_from_head(IDa, sizeof(IDa), cIDa);
    //Bit len.
    ENTLa = u16IDlen<<3;
    
    mh_sm2_hash_z(Za, IDa, ENTLa, &para, &key.e);
    
    memset(timeTotal, 0, sizeof(timeTotal));
    for (i = 0; i < SM2_TIME_TEST_SIGN_COUNT; i++)
    {
        f_rng(rand, sizeof(rand), NULL);
        f_rng(message, sizeof(message), NULL);

        memset(&sign, 0, sizeof(sign));
        timer_init_ms(1);
        mh_sm2_digital_sign(&sign, Za, message, sizeof(message), &key, &para, mh_rand_p, NULL);
        get_time(&time);
        time_add(&time, timeTotal + 0, timeTotal + 0);
        printf("%dms %dus sm2 sign time\n", time.u32Ms, time.u32Us);

        timer_init_ms(1);
        t = mh_sm2_verify_sign(&sign, Za, message, sizeof(message), &key.e ,&para, mh_rand_p, NULL);
        get_time(&time);
        time_add(&time, timeTotal + 1, timeTotal + 1);

        printf("%dms %dus sm2 verify time\n", time.u32Ms, time.u32Us);
        if(MH_RET_SM2_VERIFY_SUCCESS == t)
        {
//          printf("Sign test pass\n");
        }
        else
        {
            printf("Sign test err\n");
            while(1);
        }
    }
    time_avg(timeTotal + 0, SM2_TIME_TEST_SIGN_COUNT, &time);
    printf("%dms %dus sm2 sign time\n", time.u32Ms, time.u32Us);
    
    time_avg(timeTotal + 1, SM2_TIME_TEST_SIGN_COUNT, &time);
    printf("%dms %dus sm2 verify time\n", time.u32Ms, time.u32Us);
    
}
