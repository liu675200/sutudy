#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define KEY_WORDS 256 / 32



/**************************Add by link*********************/

//1?¡ì2??¡ì12?¡ìa???2??¡ìoy?¡§o??eDT?? cp??¡§|ca??¡§|cb??¡§|cGx??¡§|cGy??¡§|cn2??¡ìoy
#define cp      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define ca		"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC"
#define cb		"28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
#define cGx		"32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"
#define cGy		"BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0"
#define cn		"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"

//1??????32Byte                
#define cex     "1CDFD6D0375D84164EA5CBCD929874F19384743E20407EF1CCFDC07D960643F7"
//1???o?¡ì?32Byte                
#define cey     "F0053CCD7FE91230A12003A084964928DE12DC97E6F51F2B46C43935DB9F7857"
//????                
#define ckeyd	"C46841E769D7F6A7599046C029900CB6D47BBD54BE83AF50284584BC1E390BF9"

/**************************Add by link*********************/


#define cm		"115960C5598A2797C2D02D76D468CDC9"
#define ccipher         "0470342F90FA3213647483B18B6974187F9D16F73CBFBECCF88CF57388C0C663E5CD73A4460987F928ABCD184C0196B55B97E535A45E841BAA62C73B7B81CF31F464B7FCC88FFC327247A49B36872131A7A4EEE1AF44C46E32977A14BE98C2606BAFFA7A0BA0391A64BC549920357939E8"
#define std_ccipher     "70342F90FA3213647483B18B6974187F9D16F73CBFBECCF88CF57388C0C663E5CD73A4460987F928ABCD184C0196B55B97E535A45E841BAA62C73B7B81CF31F4A4EEE1AF44C46E32977A14BE98C2606BAFFA7A0BA0391A64BC549920357939E864B7FCC88FFC327247A49B36872131A7"



uint32_t mh_sm2_enc_std(uint8_t *output, uint32_t *olen,
					const uint8_t *input, uint32_t ilen,
					const mh_sm2_public_key *key, const mh_sm2_ellipse_para *para,
					mh_rng_callback f_rng, void *p_rng)
{
	  uint8_t  tmp[2048];
	  uint32_t lenth;
      if(MH_RET_SM2_ENC_SUCCESS != mh_sm2_enc(tmp,&lenth,input, ilen,key, para,f_rng, NULL))
      { 
			return MH_RET_SM2_ENC_FAILURE;
	  }
	  else
	  {
			memcpy(output,&tmp[1],64);
			memcpy(output+64,&tmp[lenth-32],32);
			memcpy(output+64+32,&tmp[65],lenth-1-32-64);
			*olen = lenth -1;
	  }	
	  return MH_RET_SM2_ENC_SUCCESS;
	
	
}

uint32_t mh_sm2_dec_std(uint8_t *output, uint32_t *olen,
					const uint8_t *input, uint32_t ilen,
					const mh_sm2_private_key *key, const mh_sm2_ellipse_para *para,
					mh_rng_callback f_rng, void *p_rng)
{
	  uint8_t  tmp[2048];
	  uint32_t lenth;
	  tmp[0] = 0x04;
	  memcpy(&tmp[1],input,64);
      memcpy(&tmp[65],input+64+32,ilen-64-32);
	  memcpy(&tmp[1+ilen-32],input+64,32);
	  lenth = ilen + 1;
	  return mh_sm2_dec(output,olen,tmp, lenth,key, para,f_rng, p_rng);	
}

void sm2_encrypt_decrypt_self_test(void)
{
	mh_sm2_ellipse_para para;
	mh_sm2_private_key key, keyTmp;

	uint8_t test_cplain[40];
	uint8_t test_ccipher[40 + 65 + 32];

	uint8_t message[40];
	uint8_t cipher[40 + 65 + 32];
    
	uint32_t mlen;
	uint32_t clen, u32Tmp;
//	uint8_t rand[32];
	uint32_t ret = 0;

	DBG_PRINTF("\nSM2 crypt Test In\n");
		

	bn_read_string_to_bytes(para.p, MH_SM2_KEY_BYTES, cp);
	bn_read_string_to_bytes(para.a, MH_SM2_KEY_BYTES, ca);
	bn_read_string_to_bytes(para.b, MH_SM2_KEY_BYTES, cb);
	bn_read_string_to_bytes(para.n, MH_SM2_KEY_BYTES, cn);
	bn_read_string_to_bytes(para.g.x, MH_SM2_KEY_BYTES, cGx);
	bn_read_string_to_bytes(para.g.y, MH_SM2_KEY_BYTES, cGy);

    bn_read_string_to_bytes(key.d, MH_SM2_KEY_BYTES, ckeyd);
    bn_read_string_to_bytes(key.e.x, MH_SM2_KEY_BYTES, cex);
    bn_read_string_to_bytes(key.e.y, MH_SM2_KEY_BYTES, cey);
    
    memset(&keyTmp, 0, sizeof(keyTmp));
    memcpy(keyTmp.d, key.d, sizeof(key.d));
    
    sm2_completeKey(&keyTmp, &para, mh_rand_p, NULL);
    r_printf((0 == memcmp(key.e.x, keyTmp.e.x, sizeof(key.e.x))), "ecc_completeKey X\n");
    r_printf((0 == memcmp(key.e.y, keyTmp.e.y, sizeof(key.e.y))), "ecc_completeKey Y\n");

    clen = bn_read_string_from_head(cipher, sizeof(cipher), ccipher);
	mlen = bn_read_string_from_head(message, sizeof(message), cm);
	memset(test_ccipher, 0, sizeof(test_ccipher));
	memset(test_cplain, 0, sizeof(test_cplain));
	
    if (0 == mlen)
    {
        DBG_PRINT("NULL plain\n");
        while(1);
    }
    
    //?¡ìaa2???¡ì???¡ì1?¡§o???¡ì22??a??¡ì1
    ret = mh_sm2_dec(test_cplain, &u32Tmp, cipher, clen, &key, &para, mh_rand_p, NULL);
    ouputRes("text\n", message, mlen);
    ouputRes("text1\n", test_cplain, u32Tmp);
    
    r_printf((0 == memcmp(message, test_cplain, mlen)), "sm2 decrypt\n");

    //?¡§¡é???¡ì???¡§|?¡§¡é??a??¡ì1
	r_printf((MH_RET_SM2_ENC_SUCCESS == mh_sm2_enc(test_ccipher, &u32Tmp, message, mlen, &key.e, &para, mh_rand_p, NULL)), "sm2 encrypt\n");
    ouputRes("SM2\n", test_ccipher, u32Tmp);
    memset(test_cplain, 0, sizeof(test_cplain));
	mh_sm2_dec(test_cplain, &u32Tmp, test_ccipher, u32Tmp, &key, &para, mh_rand_p, NULL);
	r_printf((0 == memcmp(message, test_cplain, mlen)), "sm2 decrypt\n");
    
    
    clen = bn_read_string_from_head(cipher, sizeof(cipher), std_ccipher);
	mlen = bn_read_string_from_head(message, sizeof(message), cm);
    memset(test_ccipher, 0, sizeof(test_ccipher));
	memset(test_cplain, 0, sizeof(test_cplain));
    
    if (0 == mlen)
    {
        DBG_PRINT("NULL plain\n");
        while(1);
    }
    
    mh_sm2_dec_std(test_cplain, &u32Tmp, cipher, clen, &key, &para, mh_rand_p, NULL);
	r_printf((0 == memcmp(message, test_cplain, mlen)), "sm2 decrypt\n");
    
    //?¡§¡é???¡ì???¡§|?¡§¡é??a??¡ì1
	r_printf((MH_RET_SM2_ENC_SUCCESS == mh_sm2_enc_std(test_ccipher, &u32Tmp, message, mlen, &key.e, &para, mh_rand_p, NULL)), "sm2 encrypt\n");
    ouputRes("SM2\n", test_ccipher, clen);
    memset(test_cplain, 0, sizeof(test_cplain));
	mh_sm2_dec_std(test_cplain, &u32Tmp, test_ccipher, u32Tmp, &key, &para, mh_rand_p, NULL);
	r_printf((0 == memcmp(message, test_cplain, mlen)), "sm2 decrypt\n");
    //?¡ìaa2???¡ì???¡ì1?¡§o???¡ì22??a??¡ì1
    
    
    //Generate a new key, then use it.
    memset(&keyTmp, 0, sizeof(keyTmp));
    r_printf((MH_RET_ECC_KEY_GENERATION_SUCCESS == sm2_genkey(&keyTmp, &para, mh_rand_p, NULL)), "sm2 genkey encrypt");
    ouputRes("genKey\n", keyTmp.d, MH_SM2_KEY_BYTES);
    ouputRes("genKey\n", keyTmp.e.x, MH_SM2_KEY_BYTES);
    ouputRes("genKey\n", keyTmp.e.y, MH_SM2_KEY_BYTES);
    
    mlen = bn_read_string_from_head(message, sizeof(message), cm);
    memset(test_ccipher, 0, sizeof(test_ccipher));
    memset(test_cplain, 0, sizeof(test_cplain));
    
    r_printf((MH_RET_SM2_ENC_SUCCESS == mh_sm2_enc(test_ccipher, &u32Tmp, message, mlen, &key.e, &para, mh_rand_p, NULL)), "sm2 encrypt\n");
    mh_sm2_dec(test_cplain, &u32Tmp, test_ccipher, u32Tmp, &key, &para, mh_rand_p, NULL);
    r_printf((0 == memcmp(message, test_cplain, mlen > u32Tmp ? mlen : u32Tmp)), "sm2 genkey encrypt decrypt\n");
    
    

}

