#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define KEY_WORDS 256 / 32

#define cp      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define ca		"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC"
#define cb		"28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
#define cGx		"32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"
#define cGy		"BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0"
#define cn		"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"
          
#define cex     "1CDFD6D0375D84164EA5CBCD929874F19384743E20407EF1CCFDC07D960643F7"           
#define cey     "F0053CCD7FE91230A12003A084964928DE12DC97E6F51F2B46C43935DB9F7857"         
#define ckeyd	"C46841E769D7F6A7599046C029900CB6D47BBD54BE83AF50284584BC1E390BF9"

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
    
    //?§aa2???§???§1?¨o???§22??a??§1
    ret = mh_sm2_dec(test_cplain, &u32Tmp, cipher, clen, &key, &para, mh_rand_p, NULL);
    ouputRes("text\n", message, mlen);
    ouputRes("text1\n", test_cplain, u32Tmp);
    
    r_printf((0 == memcmp(message, test_cplain, mlen)), "sm2 decrypt\n");

    //?¨￠???§???¨|?¨￠??a??§1
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
    
    //?¨￠???§???¨|?¨￠??a??§1
	r_printf((MH_RET_SM2_ENC_SUCCESS == mh_sm2_enc_std(test_ccipher, &u32Tmp, message, mlen, &key.e, &para, mh_rand_p, NULL)), "sm2 encrypt\n");
    ouputRes("SM2\n", test_ccipher, clen);
    memset(test_cplain, 0, sizeof(test_cplain));
	mh_sm2_dec_std(test_cplain, &u32Tmp, test_ccipher, u32Tmp, &key, &para, mh_rand_p, NULL);
	r_printf((0 == memcmp(message, test_cplain, mlen)), "sm2 decrypt\n");
    //?§aa2???§???§1?¨o???§22??a??§1
    
    
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

#define cIDa    "414C4943453132333435363738394142434445464748405941484F4F2E434F4D"

#define cm      "115960C5598A2797C2D02D76D468CDC9"
//#define cZa       "F4A38489E32B45B6F876E3AC2168CA392362DC8F23459C1D1146FC3DBFB7BC9A"
#define cZa     "0C4133256FD68E71D4B7A86C1AF197B3F48499D84CEBF20941375F251B2B414D"
#define rs      "F34CA942DD37EAC7462A2F3D8C6EEAB095CE4DBE50BCC89448DD70D52581919914B507311CC6DDDFCDCA263D4BD1C7522E0BA53BA6DA597897AA01E1EB31A776"


void sm2_sign_self_test(void)
{
    mh_sm2_ellipse_para para;
    mh_sm2_sign sign;
    mh_sm2_private_key key;

    uint8_t Za[32];
    uint8_t _Za[32];
    uint32_t _e[8];
    uint8_t message[128];
    uint16_t u16MLen;

    uint8_t u8TestSign[64];
    
    uint16_t u16IDlen,ENTLa;
    uint8_t IDa[128];
    uint32_t u32Res, u32Tick;

    DBG_PRINTF("\nSM2 sign Test In\n");

    bn_read_string_to_bytes(Za, sizeof(Za), cZa);
    //standard sign data.
    bn_read_string_to_bytes(u8TestSign, sizeof(u8TestSign), rs);
    
    u16MLen = bn_read_string_from_head(message, sizeof(message), cm);
    u16IDlen = bn_read_string_from_head(IDa, sizeof(IDa), cIDa);
    //Bit len.
    ENTLa = u16IDlen<<3;


    bn_read_string_to_bytes(para.p, MH_SM2_KEY_BYTES, cp);
    bn_read_string_to_bytes(para.a, MH_SM2_KEY_BYTES, ca);
    bn_read_string_to_bytes(para.b, MH_SM2_KEY_BYTES, cb);
    bn_read_string_to_bytes(para.n, MH_SM2_KEY_BYTES, cn);
    bn_read_string_to_bytes(para.g.x, MH_SM2_KEY_BYTES, cGx);
    bn_read_string_to_bytes(para.g.y, MH_SM2_KEY_BYTES, cGy);

    bn_read_string_to_bytes(key.d, MH_SM2_KEY_BYTES, ckeyd);
    bn_read_string_to_bytes(key.e.x, MH_SM2_KEY_BYTES, cex);
    bn_read_string_to_bytes(key.e.y, MH_SM2_KEY_BYTES, cey);

    //ouputRes("SM2_sign_m\n", message,u16MLen);
    //ouputRes("SM2_sign_ID\n", IDa,u16IDlen);
    
    mh_sm2_hash_z(_Za, IDa, ENTLa, &para, &key.e);
    //ouputRes("SM2_sign_Za\n", _Za, sizeof(_Za));
    mh_sm2_hash_e(_e, _Za, message, u16MLen);
    //ouputRes("SM2_sign_e\n", _e, sizeof(_e));
    

    //??§|???¨¤?§o?¨￠??§oy?Y
    memcpy(&sign, u8TestSign, sizeof(sign));
    //GPIO_GROUP[0].BSRR = BIT(16 + 2);
    u32Res = MH_RET_SM2_VERIFY_SUCCESS==mh_sm2_verify_sign(&sign, _Za, message, u16MLen, &key.e, &para, mh_rand_p, NULL);
    //GPIO_GROUP[0].BSRR = BIT(2);
    r_printf(u32Res, "sm2 verify test\n");
    
    
    u32Res = MH_RET_SM2_VERIFY_SUCCESS==mh_sm2_verify_sign_with_e(&sign, _e, NULL, 0, &key.e, &para, mh_rand_p, NULL);
    r_printf(u32Res, "sm2 verify with e test\n");

    //?¨￠???????¨|?¨￠???§|??
    memset(&sign, 0, sizeof(sign));
    u32Res = MH_RET_SM2_SIGN_SUCCESS == mh_sm2_digital_sign_with_e(&sign, _e, NULL, 0, &key, &para, mh_rand_p, NULL);
    r_printf(u32Res, "sm2 sign with e test\n");
    //ouputRes("SM2_sign_r\n", sign.r, sizeof(sign.r));
    //ouputRes("SM2_sign_s\n", sign.s, sizeof(sign.s));
    u32Res = MH_RET_SM2_VERIFY_SUCCESS == mh_sm2_verify_sign_with_e(&sign, _e, NULL, 0, &key.e, &para, mh_rand_p, NULL);
    r_printf(u32Res, "sm2 verify with e test\n");
    
    memset(&sign, 0, sizeof(sign));
    u32Res = MH_RET_SM2_SIGN_SUCCESS == mh_sm2_digital_sign(&sign, _Za, message, u16MLen, &key, &para, mh_rand_p, NULL);
    r_printf(u32Res, "sm2 sign test\n");
    
    
    u32Res = MH_RET_SM2_VERIFY_SUCCESS == mh_sm2_verify_sign(&sign, _Za, message, u16MLen, &key.e, &para, mh_rand_p, NULL);
    r_printf(u32Res, "sm2 verify test\n");

}




#define kcp		"8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3"
#define kca		"787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498"
#define kcb		"63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A"
#define kcGx		"421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D"
#define kcGy		"0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2"
#define kcn		"8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7"

#define cAda		"6FCBA2EF9AE0AB902BC3BDE3FF915D44BA4CC78F88E2F8E7F8996D3B8CCEEDEE"
#define cAPx		"3099093BF3C137D8FCBBCDF4A2AE50F3B0F216C3122D79425FE03A45DBFE1655"
#define cAPy		"3DF79E8DAC1CF0ECBAA2F2B49D51A4B387F2EFAF482339086A27A8E05BAED98B"


#define cBda		"5E35D7D3F3C54DBAC72E61819E730B019A84208CA3A35E4C2E353DFCCB2A3B53"
#define cBPx		"245493D446C38D8CC0F118374690E7DF633A8A4BFB3329B5ECE604B2B4F37F43"
#define cBPy		"53C0869F4B9E17773DE68FEC45E14904E0DEA45BF6CECF9918C85EA047C60A4C"

#define kcZa			"E4D1D0C3CA4C7F11BC8FF8CB3F4C02A78F108FA098E51A668487240F75E20F31"
#define kcZb			"6B4B6D0E276691BD4A11BF72F4FB501AE309FDACB72FA6CC336E6656119ABD67"

#define cranda		"83A2C9C8B96E5AF70BD480B472409A9A327257F1EBB73F5B073354B248668563"
#define crandb		"33FE21940342161C55619C4A0C060293D543C80AF19748CE176D83477DE71C80"

//中间计算结果
#define cRax	"6CB5633816F4DD560B1DEC458310CBCC6856C09505324A6D23150C408F162BF0"
#define cRay	"0D6FCF62F1036C0A1B6DACCF57399223A65F7D7BF2D9637E5BBBEB857961BF1A"
#define cRbx	"1799B2A2C778295300D9A2325C686129B8F2B5337B3DCF4514E8BBC19D900EE5"
#define cRby	"54C9288C82733EFDF7808AE7F27D0E732F7C73A7D9AC98B7D8740A91D0DB3CF4"
#define cta		"236CF0C7A177C65C7D55E12D361F7A6C174A78698AC099C0874AD0658A4743DC"
#define ctb		"2B2E11CBF03641FC3D939262FC0B652A70ACAA25B5369AD38B375C0265490C9F"
#define cKa		"55B0AC62A6B927BA23703832C853DED4"
#define cKb		"55B0AC62A6B927BA23703832C853DED4"
#define cS1		"284C8F198F141B502E81250F1581C7E9EEB4CA6990F9E02DF388B45471F5BC5C"
#define cS2		"23444DAF8ED7534366CB901C84B3BDBB63504F4065C1116C91A4C00697E6CF7A"
#define cSa		"23444DAF8ED7534366CB901C84B3BDBB63504F4065C1116C91A4C00697E6CF7A"
#define cSb		"284C8F198F141B502E81250F1581C7E9EEB4CA6990F9E02DF388B45471F5BC5C"
#define cVx		"47C826534DC2F6F1FBF28728DD658F21E174F48179ACEF2900F8B7F566E40905"
#define cVy		"2AF86EFE732CF12AD0E09A1F2556CC650D9CCCE3E249866BBB5C6846A4C4A295"
#define cUx		"47C826534DC2F6F1FBF28728DD658F21E174F48179ACEF2900F8B7F566E40905"
#define cUy		"2AF86EFE732CF12AD0E09A1F2556CC650D9CCCE3E249866BBB5C6846A4C4A295"

void sm2_key_exchange_self_test()
{
	uint32_t t;
	mh_sm2_ellipse_para para;
	mh_sm2_private_key keya;
	mh_sm2_private_key keyb;
	mh_sm2_point Ra;
	mh_sm2_point Rb;
	mh_sm2_point U;
	mh_sm2_point V;

	uint8_t ta[MH_SM2_KEY_BYTES];
	uint8_t tb[MH_SM2_KEY_BYTES];

	uint8_t Za[MH_SM2_KEY_BYTES];
	uint8_t Zb[MH_SM2_KEY_BYTES];

	uint8_t randa[MH_SM2_KEY_BYTES];
	uint8_t randb[MH_SM2_KEY_BYTES];

	uint8_t Kb[16];
	uint8_t Ka[16];

	uint8_t Sa[32] = {0};
	uint8_t Sb[32] = {0};
	uint8_t S1[32] = {0};
	uint8_t S2[32] = {0};


	mh_sm2_point _Ra;
	mh_sm2_point _Rb;
	mh_sm2_point _U;
	mh_sm2_point _V;
	uint8_t _K[16];


	uint8_t _Sa[32] = {0};
	uint8_t _Sb[32] = {0};
	uint8_t _S1[32] = {0};
	uint8_t _S2[32] = {0};
	
	DBG_PRINTF("\nSM2 key Ex Test In\n");

	bn_read_string_to_bytes(para.p, MH_SM2_KEY_BYTES, kcp);
	bn_read_string_to_bytes(para.a, MH_SM2_KEY_BYTES, kca);
	bn_read_string_to_bytes(para.b, MH_SM2_KEY_BYTES, kcb);
	bn_read_string_to_bytes(para.n, MH_SM2_KEY_BYTES, kcn);
	bn_read_string_to_bytes(para.g.x, MH_SM2_KEY_BYTES, kcGx);
	bn_read_string_to_bytes(para.g.y, MH_SM2_KEY_BYTES, kcGy);

	bn_read_string_to_bytes(keya.d, MH_SM2_KEY_BYTES, cAda);
	bn_read_string_to_bytes(keya.e.x, MH_SM2_KEY_BYTES, cAPx);
	bn_read_string_to_bytes(keya.e.y, MH_SM2_KEY_BYTES, cAPy);
	bn_read_string_to_bytes(keyb.d, MH_SM2_KEY_BYTES, cBda);
	bn_read_string_to_bytes(keyb.e.x, MH_SM2_KEY_BYTES, cBPx);
	bn_read_string_to_bytes(keyb.e.y, MH_SM2_KEY_BYTES, cBPy);

	bn_read_string_to_bytes(Za, MH_SM2_KEY_BYTES, kcZa);
	bn_read_string_to_bytes(Zb, MH_SM2_KEY_BYTES, kcZb);
	bn_read_string_to_bytes(randa, MH_SM2_KEY_BYTES, cranda);
	bn_read_string_to_bytes(randb, MH_SM2_KEY_BYTES, crandb);

	bn_read_string_to_bytes(_Ra.x, MH_SM2_KEY_BYTES, cRax);
	bn_read_string_to_bytes(_Ra.y, MH_SM2_KEY_BYTES, cRay);
	bn_read_string_to_bytes(_Rb.x, MH_SM2_KEY_BYTES, cRbx);
	bn_read_string_to_bytes(_Rb.y, MH_SM2_KEY_BYTES, cRby);
	bn_read_string_to_bytes(_U.x, MH_SM2_KEY_BYTES, cUx);
	bn_read_string_to_bytes(_U.y, MH_SM2_KEY_BYTES, cUy);
	bn_read_string_to_bytes(_V.x, MH_SM2_KEY_BYTES, cVx);
	bn_read_string_to_bytes(_V.y, MH_SM2_KEY_BYTES, cVy);
	bn_read_string_to_bytes(_K, MH_SM2_KEY_BYTES / 2, cKa);

	bn_read_string_to_bytes(_S1, MH_SM2_KEY_BYTES, cS1);
	bn_read_string_to_bytes(_S2, MH_SM2_KEY_BYTES, cS2);
	bn_read_string_to_bytes(_Sa, MH_SM2_KEY_BYTES, cSa);
	bn_read_string_to_bytes(_Sb, MH_SM2_KEY_BYTES, cSb);

	//User A Generates the Ra and ta
	mh_sm2_key_ex_section_0(&Ra, ta, randa, &para, &keya, mh_rand_p, NULL);
	t = (0 == memcmp(Ra.x, _Ra.x, MH_SM2_KEY_BYTES) && 0 == memcmp(Ra.y, _Ra.y, MH_SM2_KEY_BYTES));
	r_printf(t, "sm2 key exchange Ra test\n");

	//User B Generates the Rb and tb
	mh_sm2_key_ex_section_0(&Rb, tb, randb, &para, &keyb, mh_rand_p, NULL);
	t = (0 == memcmp(Rb.x, _Rb.x, MH_SM2_KEY_BYTES) && 0 == memcmp(Rb.y, _Rb.y, MH_SM2_KEY_BYTES));
	r_printf(t, "sm2 key exchange Rb test\n");

	//User B Use the Ra and tb Generation the Kb and Point V
	mh_sm2_key_ex_section_1(Kb, 16, &V, &Ra, tb, Za, Zb, &para, &(keya.e), mh_rand_p, NULL);
	t = (0 == memcmp(Kb, _K, 16));
	r_printf(t, "sm2 key exchange Kb test\n");
	t = (0 == memcmp(V.x, _V.x, MH_SM2_KEY_BYTES) && 0 == memcmp(V.y, _V.y, MH_SM2_KEY_BYTES));
	r_printf(t, "sm2 key exchange V test\n");

	//User B Generates the Sb, Sb is a Hash Data
	mh_sm2_key_ex_hash(Sb, 0x02, &V, Za, Zb, &Ra, &Rb);
	mh_sm2_key_ex_hash(S2, 0x03, &V, Za, Zb, &Ra, &Rb);
	mh_sm2_key_ex_section_1(Ka, 16, &U, &Rb, ta, Za, Zb, &para, &(keyb.e), mh_rand_p, NULL);
	t = (0 == memcmp(Ka, _K, 16));
	r_printf(t, "sm2 key exchange Ka test\n");
	t = (0 == memcmp(U.x, _U.x, MH_SM2_KEY_BYTES) && 0 == memcmp(U.y, _U.y, MH_SM2_KEY_BYTES));
	r_printf(t, "sm2 key exchange U test\n");

	//Sx
	mh_sm2_key_ex_hash(S1, 0x02, &U, Za, Zb, &Ra, &Rb);
	mh_sm2_key_ex_hash(Sa, 0x03, &U, Za, Zb, &Ra, &Rb);
	t = (0 == memcmp(S1, _S1, MH_SM2_KEY_BYTES) &&
		0 == memcmp(S2, _S2, MH_SM2_KEY_BYTES) &&
		0 == memcmp(Sa, _Sa, MH_SM2_KEY_BYTES) &&
		0 == memcmp(Sb, _Sb, MH_SM2_KEY_BYTES));
	r_printf(t, "sm2 key exchange Sx test\n");

}
