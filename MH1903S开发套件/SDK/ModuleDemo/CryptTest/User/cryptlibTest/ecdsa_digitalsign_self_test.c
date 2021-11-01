#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mh_ecdsa.h"
#include "debug.h"

#define cm                  "115960C5598A2797C2D02D76D468CDC9"
//secp 256k1
/**************************Add by link*********************/
#define ECC_P_256k1      "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F"
#define ECC_A_256k1      "0000000000000000000000000000000000000000000000000000000000000000"
#define ECC_B_256k1      "0000000000000000000000000000000000000000000000000000000000000007"
#define ECC_GX_256k1     "79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"
#define ECC_GY_256k1     "483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"
#define ECC_N_256k1      "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141"
               
#define ECC_PRIA_256k1         "F15EFC405AE3FBF3EACAA65C349E842844D83D2523B4E31DD7C2786E197831AB"
#define ECC_PUBXA_256k1        "16A55AD53686237FF48273C44BCEC9848718A06D045315D1650750387C8B6699"
#define ECC_PUBYA_256k1        "79A5A4D042A51C7850FB90FF2517E0BC0C9346A7BF0C8828A79540161EE713FD"

#define SIGN_R_256K1	   "041A5E70F06E9DE59C22C29B7DEE38D9198D18E7DD1075E2F99D687D73E12AFB"
#define SIGN_S_256K1      "397AA60F7E9DDB5D592249F5175665974CA86EC7B00EEC580DF7956DEBAEA86B"

//SECP192r1
#define ECC_P_192r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFF"
#define ECC_A_192r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFC"
#define ECC_B_192r1        "64210519E59C80E70FA7E9AB72243049FEB8DEECC146B9B1"
#define ECC_GX_192r1       "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012"
#define ECC_GY_192r1       "07192B95FFC8DA78631011ED6B24CDD573F977A11E794811"
#define ECC_N_192r1        "FFFFFFFFFFFFFFFFFFFFFFFF99DEF836146BC9B1B4D22831"

#define ECC_PRIA_192r1      "8E9A397EF37F9710E0A7352C88EDBA9F92D54E884D119AAF"
#define ECC_PUBXA_192r1     "5E0E1E4CE6D39B72A263264A19D41CBAACA1E8AFAECDE706"
#define ECC_PUBYA_192r1     "6C2090B6871CB7356D850A815014B07F8C01FE8A15A1A0E2"

#define SIGN_R_192r1	   "5B5CA592D419119DDEA56CB8CE4BBDF2F20BBA3E62EED466"
#define SIGN_S_192r1       "A8124AC298C342E034A7CCC433FA3EA28F3B5AC12BFFA840"

//SECP224r1
#define ECC_P_224r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000001"
#define ECC_A_224r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFE"
#define ECC_B_224r1        "B4050A850C04B3ABF54132565044B0B7D7BFD8BA270B39432355FFB4"
#define ECC_GX_224r1       "B70E0CBD6BB4BF7F321390B94A03C1D356C21122343280D6115C1D21"
#define ECC_GY_224r1       "BD376388B5F723FB4C22DFE6CD4375A05A07476444D5819985007E34"
#define ECC_N_224r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFF16A2E0B8F03E13DD29455C5C2A3D"

#define ECC_PRIA_224r1      "61780BFCF120232972F3993F9229A72F5F4508C6BF3CFE8A06DB33D6"
#define ECC_PUBXA_224r1     "BE810752B78F84CA2BEDAB92D0E56293F1A691F3B77765FD86D4729A"
#define ECC_PUBYA_224r1     "5AF6C5ECE0F14DB75D878FBFA83651943E6B90E29F97A802703440DD"

#define SIGN_R_224r1        "0B36C341C9812CC97687F1CFE992BB80CF0FFA3826AB6036912FC7AC"
#define SIGN_S_224r1        "B470A2664099EF7DE059AA69D169F114187E44B23FA0AAA014EDEC82"


//SECP384r1
#define ECC_P_384r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFF"
#define ECC_A_384r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFFFF0000000000000000FFFFFFFC"
#define ECC_B_384r1        "B3312FA7E23EE7E4988E056BE3F82D19181D9C6EFE8141120314088F5013875AC656398D8A2ED19D2A85C8EDD3EC2AEF"
#define ECC_GX_384r1       "AA87CA22BE8B05378EB1C71EF320AD746E1D3B628BA79B9859F741E082542A385502F25DBF55296C3A545E3872760AB7"
#define ECC_GY_384r1       "3617DE4A96262C6F5D9E98BF9292DC29F8F41DBD289A147CE9DA3113B5F0B8C00A60B1CE1D7E819D7A431D7C90EA0E5F"
#define ECC_N_384r1        "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC7634D81F4372DDF581A0DB248B0A77AECEC196ACCC52973"

#define ECC_PRIA_384r1       "6CCC9B1E2AF84D1C3C8704223F1E96BCEA6E4BB91724D0ABDA13CA587B124779063D13434BFD41587609E046C14A922E"
#define ECC_PUBXA_384r1      "420D47AC4ECF4E49264A6CA1818008A87F28E4788C8CF0511AD821A4BD1DE0BEAC2A25AB4D205BD723A63E000B5A9C83"
#define ECC_PUBYA_384r1      "8B141268999BFF1477217C8724C89AEC18C96BC88800AFC44BED6D55AF1D98A3E0CF2DBC045C3AF7E4399259FEB35685"
#define SIGN_R_384r1         "F52AA56091C158537725038A7FA5B2D2C28A2E1C0ABF3B5CB23426ED3AA5FEE4F63BF5FE86475DF6ACC01FC0FC6D11CF"
#define SIGN_S_384r1         "435B84C62DFD9DCE2F52FF69EF85EFC0782C84B53E3106F71198ED9D797A2C0850B70E4BB8FB2D8AD6AFD7C0C32186A0"


//secp521r1

#define ECC_P_521r1   "01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
#define ECC_A_521r1   "01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC"
#define ECC_B_521r1   "0051953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00"
#define ECC_GX_521r1  "00C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66"
#define ECC_GY_521r1  "011839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650"
#define ECC_N_521r1   "01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409"

#define ECC_PRIA_521r1      "00FCA49A1D526A04E30CC0BD25846005D27506532BB00CFEC811CDBEBF371A2A9DA9E5300EEF33777F5C33C861C7FD332882AE74F03612C78DAC3346039933A35EE4"
#define ECC_PUBXA_521r1     "019D0011C0B600ABDC911A420849925FA4D3474BAF8350483306B8FEB51B9C3B744A7807B58753EC57251E49ABE5C5CEAE10B3D5E60291A80920BE034F2058D140D2"
#define ECC_PUBYA_521r1     "0013706C32B03E833F3876C92EF8A552A622A6BCD1D17242DC2ACF3CDE74C957F120A8F2D0C0C6837419052324DEA323B113B04EF92875E2123041601A10B8D04811"

#define SIGN_R_521r1        "011357E5F09769D4A0083CF7735E75B925405222ECC5D6B536F720236FB0EC1E5CEC41BA85CF081E7A3074E286EBFC3DD94375C085724D96325391C4F6918977F3D3"
#define SIGN_S_521r1        "006C3113A64852DA5DA4372601955C223AF1E056A7B597C0EC34EC58FB1B858230E1615CCA685A308353BF7E552595438921984C22A7154E37E2F3621811B4B0F315"

static void read_key_para(uint32_t *d, uint32_t *x,  uint32_t *y, mh_ecc_curve_para *para, uint32_t u32keySzie)
{
	if(u32keySzie == 256)
	{
		bn_read_string_from_head((uint8_t *)d, MH_ECC_BYTE_SIZE, ECC_PRIA_256k1);
		bn_read_string_from_head((uint8_t *)x, MH_ECC_BYTE_SIZE, ECC_PUBXA_256k1);
		bn_read_string_from_head((uint8_t *)y, MH_ECC_BYTE_SIZE, ECC_PUBYA_256k1);
//		bn_read_string_from_head(au8SignR, MH_ECC_BYTE_SIZE, SIGN_R_256K1);
//		bn_read_string_from_head(au8SignS, MH_ECC_BYTE_SIZE, SIGN_S_256K1);
		bn_read_string_from_head((uint8_t *)para->p, MH_ECC_BYTE_SIZE, ECC_P_256k1);
		bn_read_string_from_head((uint8_t *)para->a, MH_ECC_BYTE_SIZE, ECC_A_256k1);
		bn_read_string_from_head((uint8_t *)para->b, MH_ECC_BYTE_SIZE, ECC_B_256k1);
		bn_read_string_from_head((uint8_t *)para->n, MH_ECC_BYTE_SIZE, ECC_N_256k1);
		bn_read_string_from_head((uint8_t *)para->g.x, MH_ECC_BYTE_SIZE, ECC_GX_256k1);
		bn_read_string_from_head((uint8_t *)para->g.y, MH_ECC_BYTE_SIZE, ECC_GY_256k1);
		para->u32CurveBits = 256;
		para->a_type = MH_ECC_A_NOT_NEGATIVE_3;
	}
	else if(u32keySzie == 192)
	{
		bn_read_string_from_head((uint8_t *)d, MH_ECC_BYTE_SIZE, ECC_PRIA_192r1);
		bn_read_string_from_head((uint8_t *)x, MH_ECC_BYTE_SIZE, ECC_PUBXA_192r1);
		bn_read_string_from_head((uint8_t *)y, MH_ECC_BYTE_SIZE, ECC_PUBYA_192r1);
//		bn_read_string_from_head(au8SignR, MH_ECC_BYTE_SIZE, SIGN_R_192r1);
//		bn_read_string_from_head(au8SignS, MH_ECC_BYTE_SIZE, SIGN_S_192r1);
		bn_read_string_from_head((uint8_t *)para->p, MH_ECC_BYTE_SIZE, ECC_P_192r1);
		bn_read_string_from_head((uint8_t *)para->a, MH_ECC_BYTE_SIZE, ECC_A_192r1);
		bn_read_string_from_head((uint8_t *)para->b, MH_ECC_BYTE_SIZE, ECC_B_192r1);
		bn_read_string_from_head((uint8_t *)para->n, MH_ECC_BYTE_SIZE, ECC_N_192r1);
		bn_read_string_from_head((uint8_t *)para->g.x, MH_ECC_BYTE_SIZE, ECC_GX_192r1);
		bn_read_string_from_head((uint8_t *)para->g.y, MH_ECC_BYTE_SIZE, ECC_GY_192r1);
		para->u32CurveBits = 192;
		para->a_type = MH_ECC_A_NOT_NEGATIVE_3;
	}
	else if(u32keySzie == 224)
	{
		bn_read_string_from_head((uint8_t *)d, MH_ECC_BYTE_SIZE, ECC_PRIA_224r1);
		bn_read_string_from_head((uint8_t *)x, MH_ECC_BYTE_SIZE, ECC_PUBXA_224r1);
		bn_read_string_from_head((uint8_t *)y, MH_ECC_BYTE_SIZE, ECC_PUBYA_224r1);
//		bn_read_string_from_head(au8SignR, sizeof(au8SignR), SIGN_R_224r1);
//		bn_read_string_from_head(au8SignS, sizeof(au8SignS), SIGN_S_224r1);
		bn_read_string_from_head((uint8_t *)para->p, MH_ECC_BYTE_SIZE, ECC_P_224r1);
		bn_read_string_from_head((uint8_t *)para->a, MH_ECC_BYTE_SIZE, ECC_A_224r1);
		bn_read_string_from_head((uint8_t *)para->b, MH_ECC_BYTE_SIZE, ECC_B_224r1);
		bn_read_string_from_head((uint8_t *)para->n, MH_ECC_BYTE_SIZE, ECC_N_224r1);
		bn_read_string_from_head((uint8_t *)para->g.x, MH_ECC_BYTE_SIZE, ECC_GX_224r1);
		bn_read_string_from_head((uint8_t *)para->g.y, MH_ECC_BYTE_SIZE, ECC_GY_224r1);
		para->u32CurveBits = 224;
		para->a_type = MH_ECC_A_NOT_NEGATIVE_3;
	}
	else if(u32keySzie == 384)
   {
		bn_read_string_from_head((uint8_t *)d, MH_ECC_BYTE_SIZE, ECC_PRIA_384r1);
		bn_read_string_from_head((uint8_t *)x, MH_ECC_BYTE_SIZE, ECC_PUBXA_384r1);
		bn_read_string_from_head((uint8_t *)y, MH_ECC_BYTE_SIZE, ECC_PUBYA_384r1);
//		bn_read_string_from_head(au8SignR, sizeof(au8SignR), SIGN_R_384r1);
//		bn_read_string_from_head(au8SignS, sizeof(au8SignS), SIGN_S_384r1);
		bn_read_string_from_head((uint8_t *)para->p, MH_ECC_BYTE_SIZE, ECC_P_384r1);
		bn_read_string_from_head((uint8_t *)para->a, MH_ECC_BYTE_SIZE, ECC_A_384r1);
		bn_read_string_from_head((uint8_t *)para->b, MH_ECC_BYTE_SIZE, ECC_B_384r1);
		bn_read_string_from_head((uint8_t *)para->n, MH_ECC_BYTE_SIZE, ECC_N_384r1);
		bn_read_string_from_head((uint8_t *)para->g.x, MH_ECC_BYTE_SIZE, ECC_GX_384r1);
		bn_read_string_from_head((uint8_t *)para->g.y, MH_ECC_BYTE_SIZE, ECC_GY_384r1);
		para->u32CurveBits = 384;
		para->a_type = MH_ECC_A_NOT_NEGATIVE_3;
	}
	else
	{
		bn_read_string_from_head((uint8_t *)d, MH_ECC_BYTE_SIZE, ECC_PRIA_521r1);
		bn_read_string_from_head((uint8_t *)x, MH_ECC_BYTE_SIZE, ECC_PUBXA_521r1);
		bn_read_string_from_head((uint8_t *)y, MH_ECC_BYTE_SIZE, ECC_PUBYA_521r1);
//		bn_read_string_from_head(au8SignR, sizeof(au8SignR), SIGN_R_521r1);
//		bn_read_string_from_head(au8SignS, sizeof(au8SignS), SIGN_S_521r1);
		bn_read_string_from_head((uint8_t *)para->p, MH_ECC_BYTE_SIZE, ECC_P_521r1);
		bn_read_string_from_head((uint8_t *)para->a, MH_ECC_BYTE_SIZE, ECC_A_521r1);
		bn_read_string_from_head((uint8_t *)para->b, MH_ECC_BYTE_SIZE, ECC_B_521r1);
		bn_read_string_from_head((uint8_t *)para->n, MH_ECC_BYTE_SIZE, ECC_N_521r1);
		bn_read_string_from_head((uint8_t *)para->g.x, MH_ECC_BYTE_SIZE, ECC_GX_521r1);
		bn_read_string_from_head((uint8_t *)para->g.y, MH_ECC_BYTE_SIZE, ECC_GY_521r1);
		para->u32CurveBits = 521;
		para->a_type = MH_ECC_A_NOT_NEGATIVE_3;
	}
}

#define MSG_MAX_LEN 512
void esdsa_sign_RandData_Test(void)
{
	uint8_t i,j;
	uint32_t au32key_size[5] = {192, 224, 256, 384, 521};
	
    mh_ecc_curve_para para;
    mh_ecc_sign sign;
    mh_ecc_private_key priKey;
    mh_ecc_public_key pubKey;
    ecc_bignum_key BigE;
    ecc_bignum_data BigMsg;
    uint8_t au8SignR[MH_ECC_BYTE_SIZE],au8SignS[MH_ECC_BYTE_SIZE],au8D[MH_ECC_BYTE_SIZE],
                        au8PubX[MH_ECC_BYTE_SIZE],au8PubY[MH_ECC_BYTE_SIZE],au8E[MH_ECC_BYTE_SIZE];
    uint16_t u16MLen;
    uint32_t u32Ret;
	uint8_t u8MaxMsgLen;
	uint8_t au8Msg[MSG_MAX_LEN];
	
    DBG_PRINTF("\nECDSA sign Test start\n");
    
    u16MLen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), cm);
	
	for(i = 0; i < (sizeof(au32key_size)/4); i ++)
	{
		DBG_PRINTF("\nECDSA %d  sign Test start\n", au32key_size[i]);
		
		read_key_para((uint32_t *)au8D, (uint32_t *)au8PubX, (uint32_t *)au8PubY, &para, au32key_size[i]);
		
//		ouputRes("D:\n", priKey.d.pu32Buf, (para.u32CurveBits + 7)>>3);
//		ouputRes("X:\n", pubKey.x.pu32Buf, (para.u32CurveBits + 7)>>3);
//		ouputRes("Y:\n", pubKey.y.pu32Buf, (para.u32CurveBits + 7)>>3);
		
		u8MaxMsgLen = ((au32key_size[i] + 7) >> 3) + 8;	//+8 Full Test
		
//		for(j = 1;  j < u8MaxMsgLen; j ++)
		{
			u16MLen = 0;
			while(u16MLen == 0)
			{ 
				mh_rand(&u16MLen, sizeof(u16MLen));
				u16MLen = (u16MLen & (MSG_MAX_LEN - 1));
			}
			
			DBG_PRINTF("MSG len = %d.\n", u16MLen);
			
			memset(au8Msg, 0, sizeof(au8Msg));
			mh_rand(au8Msg, u16MLen);
			ouputRes("Msg:\n", au8Msg, u16MLen);
			
			BigE.pu32Buf = (uint32_t *)au8E;
			BigMsg.pu8Buf = au8Msg;
			BigMsg.u32Len = u16MLen;
			
			mh_ecc_hash_e(&BigE, &BigMsg, &para);
			ouputRes("au8E.\n", au8E, BigE.u32Len); 
			
			
			memset(au8SignR, 0, (para.u32CurveBits + 7)>>3);
			memset(au8SignS, 0, (para.u32CurveBits + 7)>>3);
			
			sign.eb_r.pu32Buf = (uint32_t *)au8SignR;
			sign.eb_s.pu32Buf = (uint32_t *)au8SignS;
			sign.eb_r.u32Len = (para.u32CurveBits + 7)>>3;
			sign.eb_s.u32Len = (para.u32CurveBits + 7)>>3;
			
				
			priKey.d.pu32Buf = (uint32_t *)au8D;
			pubKey.x.pu32Buf = (uint32_t *)au8PubX;
			pubKey.y.pu32Buf = (uint32_t *)au8PubY;
			pubKey.x.u32Len = (para.u32CurveBits + 7)>>3;
			pubKey.y.u32Len = (para.u32CurveBits + 7)>>3;
			priKey.d.u32Len = (para.u32CurveBits + 7)>>3;
			
			u32Ret = MH_RET_ECC_SIGN_SUCCESS == mh_ecdsa_digital_sign(&sign, &BigE, &priKey, &para, mh_rand_p, NULL);
		//    r_printf(u32Ret, "ECDSA sign test\n");
			ouputRes("au8SignR.\n", au8SignR,(para.u32CurveBits + 31)>>3);
			ouputRes("au8SignS.\n", au8SignS,(para.u32CurveBits + 31)>>3);  
				 
			u32Ret = (MH_RET_ECC_VERIFY_SUCCESS == mh_ecdsa_verify_sign(&sign,&BigE,&pubKey,&para, mh_rand_p, NULL));
			r_printf(u32Ret, "ESDSA verify test self sign\n");
//			if(u32Ret)
//			{
//				DBG_PRINTF("%d ", u16MLen);
//			}
//			else
//			{
//				DBG_PRINTF("ESDSA %d sign\/verify Error. MSG len = %d.\n", au32key_size[i], u16MLen);
//				while(1);
//			}
		}
//		DBG_PRINTF("\nECDSA %d  sign Test Pass\n", au32key_size[i]);
		
	}
}

void esdsa_sign_Fixed_Test(void)
{
	uint8_t i,j;
	uint32_t au32key_size[5] = {192, 224, 256, 384, 521};
	
    mh_ecc_curve_para para;
    mh_ecc_sign sign;
    mh_ecc_private_key priKey;
    mh_ecc_public_key pubKey;
    ecc_bignum_key BigE;
    ecc_bignum_data BigMsg;
    uint8_t au8SignR[MH_ECC_BYTE_SIZE],au8SignS[MH_ECC_BYTE_SIZE],au8Msg[MH_ECC_BYTE_SIZE],au8D[MH_ECC_BYTE_SIZE],
                        au8PubX[MH_ECC_BYTE_SIZE],au8PubY[MH_ECC_BYTE_SIZE],au8E[MH_ECC_BYTE_SIZE];
    uint16_t u16MLen;
    uint32_t u32Ret;
    DBG_PRINTF("\nECDSA sign Test start\n");
    
    u16MLen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), cm);
	
	for(i = 0; i < (sizeof(au32key_size)/4); i ++)
	{
		DBG_PRINTF("\nECDSA %d  sign Test start\n", au32key_size[i]);
		if(au32key_size[i] == 256)
		{
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_256k1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_256k1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_256k1);
			bn_read_string_from_head(au8SignR, MH_ECC_BYTE_SIZE, SIGN_R_256K1);
			bn_read_string_from_head(au8SignS, MH_ECC_BYTE_SIZE, SIGN_S_256K1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_256k1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_256k1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_256k1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_256k1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_256k1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_256k1);
			para.u32CurveBits = 256;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
		else if(au32key_size[i] == 192)
		{
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_192r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_192r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_192r1);
			bn_read_string_from_head(au8SignR, MH_ECC_BYTE_SIZE, SIGN_R_192r1);
			bn_read_string_from_head(au8SignS, MH_ECC_BYTE_SIZE, SIGN_S_192r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_192r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_192r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_192r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_192r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_192r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_192r1);
			para.u32CurveBits = 192;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
		else if(au32key_size[i] == 224)
		{
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_224r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_224r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_224r1);
			bn_read_string_from_head(au8SignR, sizeof(au8SignR), SIGN_R_224r1);
			bn_read_string_from_head(au8SignS, sizeof(au8SignS), SIGN_S_224r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_224r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_224r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_224r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_224r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_224r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_224r1);
			para.u32CurveBits = 224;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
		else if(au32key_size[i] == 384)
	   {
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_384r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_384r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_384r1);
			bn_read_string_from_head(au8SignR, sizeof(au8SignR), SIGN_R_384r1);
			bn_read_string_from_head(au8SignS, sizeof(au8SignS), SIGN_S_384r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_384r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_384r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_384r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_384r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_384r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_384r1);
			para.u32CurveBits = 384;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
		else
		{
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_521r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_521r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_521r1);
			bn_read_string_from_head(au8SignR, sizeof(au8SignR), SIGN_R_521r1);
			bn_read_string_from_head(au8SignS, sizeof(au8SignS), SIGN_S_521r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_521r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_521r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_521r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_521r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_521r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_521r1);
			para.u32CurveBits = 521;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
	   /*
		*/
		BigE.pu32Buf = (uint32_t *)au8E;
		BigMsg.pu8Buf = au8Msg;
		BigMsg.u32Len = u16MLen;
		mh_ecc_hash_e(&BigE, &BigMsg, &para);
		ouputRes("au8E.\n", au8E, BigE.u32Len); 
		sign.eb_r.pu32Buf = (uint32_t *)au8SignR;
		sign.eb_s.pu32Buf = (uint32_t *)au8SignS;
		sign.eb_r.u32Len = (para.u32CurveBits + 7)>>3;
		sign.eb_s.u32Len = (para.u32CurveBits + 7)>>3;
		pubKey.x.pu32Buf = (uint32_t *)au8PubX;
		pubKey.y.pu32Buf = (uint32_t *)au8PubY;
		pubKey.x.u32Len = (para.u32CurveBits + 7)>>3;
		pubKey.y.u32Len = (para.u32CurveBits + 7)>>3;
		u32Ret = (MH_RET_ECC_VERIFY_SUCCESS == mh_ecdsa_verify_sign(&sign,&BigE,&pubKey,&para, mh_rand_p, NULL));
		r_printf(u32Ret, "ESDSA verify test\n");

		memset(au8SignR, 0, sizeof(au8SignR));
		memset(au8SignS, 0, sizeof(au8SignS));
		priKey.d.pu32Buf = (uint32_t *)au8D;
		priKey.d.u32Len = (para.u32CurveBits + 7)>>3;
		u32Ret = MH_RET_ECC_SIGN_SUCCESS == mh_ecdsa_digital_sign(&sign, &BigE, &priKey, &para, mh_rand_p, NULL);
	//    r_printf(u32Ret, "ECDSA sign test\n");
		ouputRes("au8SignR.\n", au8SignR,(para.u32CurveBits + 31)>>3);
		ouputRes("au8SignS.\n", au8SignS,(para.u32CurveBits + 31)>>3);  
			 
		u32Ret = (MH_RET_ECC_VERIFY_SUCCESS == mh_ecdsa_verify_sign(&sign,&BigE,&pubKey,&para, mh_rand_p, NULL));
		r_printf(u32Ret, "ESDSA verify test self sign\n");
	}
}

void esdsa_Test(void)
{
	esdsa_sign_Fixed_Test();
	esdsa_sign_RandData_Test();
}
