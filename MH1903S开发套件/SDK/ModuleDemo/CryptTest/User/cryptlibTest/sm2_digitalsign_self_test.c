#include <string.h>
#include <stdio.h>
#include "mhscpu.h"


#define KEY_WORDS 256 / 32


/**************************Add by link*********************/

#define cp      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF"
#define ca      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC"
#define cb      "28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93"
#define cGx     "32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7"
#define cGy     "BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0"
#define cn      "FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123"

#define cex     "1CDFD6D0375D84164EA5CBCD929874F19384743E20407EF1CCFDC07D960643F7"
#define cey     "F0053CCD7FE91230A12003A084964928DE12DC97E6F51F2B46C43935DB9F7857"
#define ckeyd   "C46841E769D7F6A7599046C029900CB6D47BBD54BE83AF50284584BC1E390BF9"

/**************************Add by link*********************/



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
    

    //??¡ì|???¡§¡è?¡ìo?¡§¡é??¡ìoy?Y
    memcpy(&sign, u8TestSign, sizeof(sign));
    //GPIO_GROUP[0].BSRR = BIT(16 + 2);
    u32Res = MH_RET_SM2_VERIFY_SUCCESS==mh_sm2_verify_sign(&sign, _Za, message, u16MLen, &key.e, &para, mh_rand_p, NULL);
    //GPIO_GROUP[0].BSRR = BIT(2);
    r_printf(u32Res, "sm2 verify test\n");
    
    
    u32Res = MH_RET_SM2_VERIFY_SUCCESS==mh_sm2_verify_sign_with_e(&sign, _e, NULL, 0, &key.e, &para, mh_rand_p, NULL);
    r_printf(u32Res, "sm2 verify with e test\n");

    //?¡§¡é???????¡§|?¡§¡é???¡ì|??
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
