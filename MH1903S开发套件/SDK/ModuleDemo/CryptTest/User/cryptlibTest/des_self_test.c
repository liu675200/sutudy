#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define DES_PLAN_LEN	(1024*4)
void test_des(void);
void test_tdes(void);

void DES_Rand_Test(void);
void TDES_Rand_Test(void);

void DES_AutoRng_Test(void)
{
	test_des();
	DES_Rand_Test();
}

void TDES_AutoRng_Test(void)
{
	test_tdes();
	TDES_Rand_Test();
}

void DES_Rand_Test(void)
{
	uint8_t i, j;
	uint32_t t;
	mh_pack_mode_def modes[2] = { ECB, CBC};
	unsigned char s[][10] = {"ECB", "CBC"};
    mh_rng_callback f_rng = mh_rand_p;
	
	uint8_t au8Plain[DES_PLAN_LEN] = {0};
	mh_des_key_def KEY = {0x00};
	uint8_t au8Iv[8] = {0x00};
	uint8_t au8Cipher[DES_PLAN_LEN] = {0x00};
	uint8_t au8Mplain[DES_PLAN_LEN] = {0x00};
	
	uint32_t u32PlainLen = 0;
	
	mh_rand(&u32PlainLen, sizeof(u32PlainLen));
	u32PlainLen &= ((DES_PLAN_LEN - 1) & ~(0x07));		//8Bytes*n
	
	mh_rand(au8Plain, u32PlainLen);
	mh_rand(au8Iv, sizeof(au8Iv));
	mh_rand(KEY, sizeof(KEY));
	
	for(i = 0; i < 2; i ++)
	{
		mh_des_enc(modes[1], au8Cipher, sizeof(au8Cipher), au8Plain, u32PlainLen, KEY, au8Iv, f_rng, NULL);
		mh_des_dec(modes[1], au8Mplain, sizeof(au8Mplain), au8Cipher, u32PlainLen, KEY, au8Iv, f_rng, NULL);
		
		t = (!memcmp(au8Mplain, au8Plain, u32PlainLen));
		r_printf(t, "DES RNG");
		printf("%s Test.\n", s[i]);
		
	}
}
	
void test_des(void)
{
    
    mh_rng_callback f_rng = mh_rand_p;
    uint32_t t;
    mh_pack_mode_def modes[2] = {ECB, CBC};
    char *cmodes[2] = {"ECB", "CBC"};
    uint8_t xPlain[2][1024];
    uint8_t xCrypt[1024];

    uint8_t PLAIN[8] = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96};

    mh_des_key_def KEY = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};              
    uint8_t IV[8] = {0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17};
    
    uint8_t cCrypt[8] = {0x2D, 0x12, 0x1F, 0x90, 0xFC, 0xF6, 0x86, 0x31};            
    uint8_t eCrypt[8] = {0x72, 0x77, 0xA0, 0x0D, 0xC1, 0xC1, 0xC3, 0x6B};
    
    uint8_t plain[8] ={0};
    uint8_t cipher[8] = {0};

    uint8_t m = 0;  

    DBG_PRINTF("\nDES Test In\n");
    
    memset(plain, 0, sizeof(plain));
    memset(cipher, 0, sizeof(cipher));
    
    mh_des_enc(modes[0],  cipher, sizeof(cipher), PLAIN, sizeof(PLAIN), KEY, NULL, f_rng, NULL);
    t = (!memcmp(eCrypt, cipher, sizeof(cipher)));
    r_printf(t, "DES_Encrypt ECB test\n");
    
    mh_des_dec(modes[0],  plain, sizeof(plain), cipher, sizeof(cipher), KEY, NULL, f_rng, NULL);
    t = (!memcmp(plain, PLAIN, sizeof(PLAIN)));
    r_printf(t, "DES_Decrypt ECB test\n");
    
    
    //CBC
    memset(plain, 0, sizeof(plain));
    memset(cipher, 0, sizeof(cipher));
    
    mh_des_enc(modes[1], cipher, sizeof(cipher), PLAIN, sizeof(PLAIN), KEY, IV, f_rng, NULL);
    t = (!memcmp(cCrypt, cipher, sizeof(cipher)));
    r_printf(t, "DES_Encrypt CBC test\n");
    
    mh_des_dec(modes[1], plain, sizeof(plain), cipher, sizeof(cipher), KEY, IV, f_rng, NULL);
    t = (!memcmp(plain, PLAIN, sizeof(PLAIN)));
    r_printf(t, "DES_Decrypt CBC test\n");

    
    
    for (m = 0; m < 2; m++)
    {
        char s[30] = {0};
        memset(xPlain, 0, sizeof(xPlain));
        
        mh_rand(xPlain[0], sizeof(xPlain[0]));
        mh_des_enc(modes[m],  xCrypt, sizeof(xCrypt), xPlain[0], sizeof(xPlain[0]), KEY, NULL, f_rng, NULL);
        mh_des_dec(modes[m],  xPlain[1], sizeof(xPlain[1]), xCrypt, sizeof(xCrypt), KEY, NULL, f_rng, NULL);
        
        if(!memcmp(xPlain[1], xPlain[0], sizeof(cipher)))
        sprintf(s, "DES %d %s Test\n",sizeof(xPlain[0]), cmodes[m]);
        r_printf(t, s);
    }
}




void TDES_Rand_Test(void)
{
	uint8_t i, j;
	uint32_t t;
	mh_pack_mode_def modes[2] = { ECB, CBC};
	unsigned char s[][10] = {"ECB", "CBC"};
    mh_rng_callback f_rng = mh_rand_p;
	
	uint8_t au8Plain[DES_PLAN_LEN] = {0};
	mh_tdes_key_def KEY = {0x00};
	uint8_t au8Iv[8] = {0x00};
	
	uint8_t au8Cipher[DES_PLAN_LEN] = {0x00};
	uint8_t au8Mplain[DES_PLAN_LEN] = {0x00};
	
	uint32_t u32PlainLen = 0;
	
	mh_rand(&u32PlainLen, sizeof(u32PlainLen));
	u32PlainLen &= ((DES_PLAN_LEN - 1) & ~(0x07));		//8Bytes*n
	
	mh_rand(au8Plain, u32PlainLen);
	mh_rand(au8Iv, sizeof(au8Iv));
	mh_rand(&KEY, sizeof(KEY));
	
	for(i = 0; i < 2; i ++)
	{
		mh_tdes_enc(modes[1], au8Cipher, sizeof(au8Cipher), au8Plain, u32PlainLen, &KEY, au8Iv, f_rng, NULL);
		mh_tdes_dec(modes[1], au8Mplain, sizeof(au8Mplain), au8Cipher, u32PlainLen, &KEY, au8Iv, f_rng, NULL);
		
		t = (!memcmp(au8Mplain, au8Plain, u32PlainLen));
		r_printf(t, "TDES RNG ");
		printf("%s Test.\n", s[i]);
		
	}
}

void test_tdes(void)
{
    
    mh_rng_callback f_rng = mh_rand_p;
    uint32_t t;
    mh_pack_mode_def modes[2] = { ECB, CBC};
    char *cmodes[2] = {"ECB", "CBC"};

    uint8_t PLAIN[8] = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96};

    mh_tdes_key_def KEY = {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
                        {0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01},
                        {0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01, 0x23}};
                                        
    uint8_t IV[8] = {0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17};
    

    uint8_t eCrypt[2][8] = {{0x06, 0xED, 0xE3, 0xD8, 0x28, 0x84, 0x09, 0x0A},
                            {0x71, 0x47, 0x72, 0xF3, 0x39, 0x84, 0x1D, 0x34}};

    uint8_t cCrypt[2][8] = {{0x74, 0x01, 0xCE, 0x1E, 0xAB, 0x6D, 0x00, 0x3C},
                             {0x20, 0x79, 0xC3, 0xD5, 0x3A, 0xA7, 0x63, 0xE1}};
                                                 
    uint8_t plain[8];
    uint8_t cipher[8];


    uint8_t xPlain[2][1024];
    uint8_t xCrypt[1024];
    
    uint32_t m;

                                                 
    DBG_PRINTF("\nTDES Test In\n");

    memset(plain, 0, sizeof(plain));
    memset(cipher, 0, sizeof(cipher));
    //===================ECB======================
    mh_tdes_enc(modes[0], cipher, sizeof(cipher), PLAIN, sizeof(PLAIN), &KEY, NULL, f_rng, NULL);
    t = (!memcmp(&eCrypt[1][0], cipher, sizeof(cipher)));
    r_printf(t, "TDES_Encrypt ECB test\n");
    
    mh_tdes_dec(modes[0], plain, sizeof(plain), cipher, sizeof(cipher), &KEY, NULL, f_rng, NULL);
    t = (!memcmp(plain, PLAIN, sizeof(PLAIN)));
    r_printf(t, "TDES_Decrypt ECB test\n");
    
    //===================CBC======================
    memset(plain, 0, sizeof(plain));
    memset(cipher, 0, sizeof(cipher));
    mh_tdes_enc(modes[1], cipher, sizeof(cipher), PLAIN, sizeof(PLAIN), &KEY, IV, f_rng, NULL);
    //cmp cipher
    t = (!memcmp(&cCrypt[1][0], cipher, sizeof(cipher)));
    r_printf(t, "TDES_Encrypt CBC test\n");
    
    mh_tdes_dec(modes[1], plain, sizeof(plain), cipher, sizeof(cipher), &KEY, IV, f_rng, NULL);

    //cmp cipher
    t = (!memcmp(plain, PLAIN, sizeof(PLAIN)));
    r_printf(t, "TDES_Decrypt CBC test\n");

    
    //===================xLen=================================================================
    for (m = 0; m < 2; m++)
    {
        char s[30] = {0};
        memset(xPlain, 0, sizeof(xPlain));
        mh_rand(xPlain[0], sizeof(xPlain[0]));
        mh_tdes_enc(modes[m],  xCrypt, sizeof(xCrypt), xPlain[0], sizeof(xPlain[0]), &KEY, NULL, f_rng, NULL);
        mh_tdes_dec(modes[m],  xPlain[1], sizeof(xPlain[1]), xCrypt, sizeof(xCrypt), &KEY, NULL, f_rng, NULL);
        
        t = (!memcmp(xPlain[1], xPlain[0], sizeof(cipher)));
        sprintf(s, "TDES %d %s Test\n",sizeof(xPlain[0]), cmodes[m]);
        r_printf(t, s);
    }
}
#define DES_BLOCK_SIZE      8

static int mh_des1(uint8_t *key, uint8_t *text_in, uint8_t *text_out, mh_crypt_mode_def crypt_mode)
{
    mh_des_key_def mh_key;
    mh_rng_callback f_rng = mh_rand_p;
    int ret;

    memset(mh_key, 0, sizeof(mh_key));
    memcpy(mh_key, key, sizeof(mh_key));
    
    if (crypt_mode == ENC) {
        ret = mh_des_enc(ECB, text_out, DES_BLOCK_SIZE, text_in, DES_BLOCK_SIZE, mh_key, NULL, f_rng, NULL);
    } else {
        ret = mh_des_dec(ECB, text_out, DES_BLOCK_SIZE, text_in, DES_BLOCK_SIZE, mh_key, NULL, f_rng, NULL);
    }
    if (ret != MH_RET_DES_SUCCESS) {
        return -1;
    }

    return 0;

}

static int mh_des3(uint8_t *key, uint8_t *text_in, uint8_t *text_out, mh_crypt_mode_def crypt_mode)
{
    mh_tdes_key_def mh_key;
    mh_des_iv_def mh_iv;
    mh_rng_callback f_rng = mh_rand_p;
    int ret;

    memset(&mh_key, 0, sizeof(mh_key));
    memset(mh_iv, 0, sizeof(mh_iv));
    memcpy(&mh_key, key, sizeof(mh_key));
    
    if (crypt_mode == ENC) {
        ret = mh_tdes_enc(ECB, text_out, DES_BLOCK_SIZE, text_in, DES_BLOCK_SIZE, &mh_key, mh_iv, f_rng, NULL);
    } else {
        ret = mh_tdes_dec(ECB, text_out, DES_BLOCK_SIZE, text_in, DES_BLOCK_SIZE, &mh_key, mh_iv, f_rng, NULL);
    }
    if (ret != MH_RET_DES_SUCCESS) {
        return -1;
    }

    return 0;

}

int des_enc(uint8_t *key, uint8_t *text, uint8_t *mText)
{
    return mh_des1(key, text, mText, ENC);
}

int des_dec(uint8_t *key, uint8_t *mText, uint8_t *text)
{
    return mh_des1(key, mText, text, DEC);
}

int des3_enc(uint8_t *key, uint8_t *text, uint8_t *mText)
{
    return mh_des3(key, text, mText, ENC);
}

int des3_dec(uint8_t *key, uint8_t *mText, uint8_t *text)
{
    return mh_des3(key, mText, text, DEC);
}

int des3_crypt(uint8_t *key, size_t keyLen, uint8_t *textIn, size_t inLen, uint8_t *textOut, int mode)
{
    mh_tdes_key_def mh_key;
    mh_des_iv_def mh_iv;
    mh_rng_callback f_rng = mh_rand_p;
    int ret;
    
//  memset(&mh_key, 0, sizeof(mh_key));
//  memset(mh_iv, 0, sizeof(mh_iv));
//  memcpy(&mh_key, key, sizeof(mh_key));

//  if (mode == TDES_CBC_ENCRYPT) {
//      ret = mh_tdes_enc(CBC, textOut, inLen, textIn, inLen, &mh_key, mh_iv, f_rng, NULL);
//  } else if (mode == TDES_CBC_DECRYPT) {
//      ret = mh_tdes_dec(CBC, textOut, inLen, textIn, inLen, &mh_key, mh_iv, f_rng, NULL);
//  } else if (mode == TDES_ECB_ENCRYPT) {
//      ret = mh_tdes_enc(ECB, textOut, inLen, textIn, inLen, &mh_key, mh_iv, f_rng, NULL);
//  } else if (mode == TDES_ECB_DECRYPT) {
//      ret = mh_tdes_dec(ECB, textOut, inLen, textIn, inLen, &mh_key, mh_iv, f_rng, NULL);
//  } else {
//      ret = -1;
//  }
//  if (ret != MH_RET_DES_SUCCESS) {
//      ret = -1;
//  } else {
//      ret = 0;
//  }

    return ret;
}


void Client_Des_test()
{

    mh_rng_callback f_rng = mh_rand_p;
    uint32_t t;
    mh_pack_mode_def modes[2] = {ECB, CBC};
    char *cmodes[2] = {"ECB", "CBC"};
    uint8_t xPlain[2][1024];
    uint8_t xCrypt[1024];

    uint8_t PLAIN[8] = {0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96};

    mh_des_key_def KEY = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};              
    uint8_t IV[8] = {0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17};
    
    uint8_t cCrypt[8] = {0x2D, 0x12, 0x1F, 0x90, 0xFC, 0xF6, 0x86, 0x31};            
    uint8_t eCrypt[8] = {0x72, 0x77, 0xA0, 0x0D, 0xC1, 0xC1, 0xC3, 0x6B};
    
    uint8_t plain[8] ={0};
    uint8_t cipher[8] = {0};


     des_enc(KEY, PLAIN, cipher);
     des_dec(KEY, cipher, plain);



}
