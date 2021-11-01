#include <string.h>
#include <stdio.h>
#include "mhscpu.h"
#include "mh_ecies.h"
#include "debug.h"
#define KEY_BITS    (224)
#define KEY_BYTES   ((KEY_BITS + 7)>>3)
#define KEY_WORDS   ((KEY_BITS + 31)>>5)

#define KEY_LEN_MAX	(((521+7)>>3))

//SECP256r1
#define ECC_P_256r1         "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFF"
#define ECC_A_256r1         "FFFFFFFF00000001000000000000000000000000FFFFFFFFFFFFFFFFFFFFFFFC"
#define ECC_B_256r1         "5AC635D8AA3A93E7B3EBBD55769886BC651D06B0CC53B0F63BCE3C3E27D2604B"
#define ECC_GX_256r1        "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296"
#define ECC_GY_256r1        "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5"
#define ECC_N_256r1         "FFFFFFFF00000000FFFFFFFFFFFFFFFFBCE6FAADA7179E84F3B9CAC2FC632551"

#define ECC_PRIA_256r1       "3165F1A988A5AF56F2D1383FEBA7DF3C1742CD2A46DC027760C9CE9EB808F6C1"
#define ECC_PUBXA_256r1      "CE745FAA9E2EFA71404B62FBFFDA16C98300B1ECD816C5EE5D6BBC3A7604F7EE"
#define ECC_PUBYA_256r1      "FDE33B2CE1AB3C8ABABC673EF07913304EE74998827E9A93D15261E35698A844"

#define ECC_PRIB_256r1      "E4E8118F5ED6BF84D3188391F147A25ADF3993C9AB59A4713968F73F066905AE"
#define ECC_PUBXB_256r1     "A932895FC829657D0156F7A05829354DF18C74B66AD23985D8F2B5A4DD2AB817"
#define ECC_PUBYB_256r1     "25BB6882ABB9C880172568047E0A85D392B75A73BDE2487C4AC525DADF763593"

#define ECC_CIPHER_256r1    "B4EC989B8D19CFDA06D407202D8758ED2477DFCAFBAA34E9F09EF8E3D76F9ABE7857DD0414B00407D54FFAB25782790C034DCFB9E32966FF796D16628438192ED275EFBFA99F3E012B65D639C5EF571D37A8CCF05E535B4DDF9CA08F8B3B743A2FC5D85447613EAF0E061D8E7C0D27971E3EBB14294A73377983F03CF3CA92D4"


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

#define ECC_CIPHER_192r1    "6B1B9E932FC39AEA306DCFB0528716DDB755B781940C054061363022CCAD1F4D2DC24F78C19F80C05C6CFB7849C6024B6DF0E9405390E6C92C48CF665607364134F9E5E42E66731BA43A33920C7B0F296A8DD03A47A34F4A74CEEB4E878D916F177F997275F530A04E442EBBE46C8BA8"

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

#define ECC_CIPHER_224r1    "CC908007FAA70F9A4474D68ADD47E81ACF1D9B32F9BF46DE5FBF3BB4E15897536BD35315EC0D10FF232952F19821A64026A9BE928EB398D4A9C6397F52E6E620BA26ACA81FAE89482BAF4309992A312C9677112708C703CEEDA7BDB0DCA2AC56964CA2991A664F8FF34AAF8E4112BECAAD625DB125A1D122"


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

#define ECC_CIPHER_384r1    "5FDBB48BC29C7486D4BFE493FCA50351BBA15FE019ECD2992FE1E12A0482692BA40345B88B4437A194386510D45CB5330B8446BDF3EF3EBAE83187D75B5179D74A532C5C5A93A4D386CAF27334E6F2131D28CC351DFBAC4ACCAF438F7A1AA4B6371ECEF203988A4D2709D5195CC62A5C3FD9DDEF256B96186EA73E9DCC13E86A94CEE59FB406EDAF21BFC6D7BE6C8F47589008BE378C86D71109E8308A348BDD"


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

#define ECC_CIPHER_521r1    "000E0A3790EC1AA887B9789B41359EAEBAB551BB87CAC498FB3BB5F99CA36C313234622EA23F1679248B0D7E5D8B27D938239C4B70BE0314EF8F09C8E314CD0A7F96002711587C1B37ECC30B2CA154967679147379D254270A3DDAB53D09279C7F56C3400FB3EEF8C93EDDE9ACEAFDDDFFB79FCE6472E075B83B86B4C41BBB6B1C543FCE739A9FD023EE7BE51C400E7732CEB1F25C18C861BC8175111430D0BBA550FAC7445E8BBA50442CAA2DE67B44065BDF0FD5E56248E0B2AFDD3C95F14E2A4C6FB4"


#define MSG           "1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF"
#define SINFO1        "115960C5598A2797C2D02D76D468CDC91234567890ABCDEF1234567890ABCDEF"
#define SINFO2        "1234567890ABCDEF1234567890ABCDEF115960C5598A2797C2D02D76D468CDC9"

#define ECC_MAX_MSG_LEN     (512)

static void read_key_para(mh_ecc_private_key * pri_key, mh_ecc_public_key *pub_key,  mh_ecc_curve_para *para, uint32_t u32keySzie)
{
    if(u32keySzie == 256)
    {
//        u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_256r1);
        bn_read_string_from_head((uint8_t *)pri_key->d.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PRIA_256r1);
        bn_read_string_from_head((uint8_t *)pub_key->x.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBXA_256r1);
        bn_read_string_from_head((uint8_t *)pub_key->y.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBYA_256r1);
        bn_read_string_from_head((uint8_t *)para->p, MH_ECC_BYTE_SIZE, ECC_P_256r1);
        bn_read_string_from_head((uint8_t *)para->a, MH_ECC_BYTE_SIZE, ECC_A_256r1);
        bn_read_string_from_head((uint8_t *)para->b, MH_ECC_BYTE_SIZE, ECC_B_256r1);
        bn_read_string_from_head((uint8_t *)para->n, MH_ECC_BYTE_SIZE, ECC_N_256r1);
        bn_read_string_from_head((uint8_t *)para->g.x, MH_ECC_BYTE_SIZE, ECC_GX_256r1);
        bn_read_string_from_head((uint8_t *)para->g.y, MH_ECC_BYTE_SIZE, ECC_GY_256r1);
        para->u32CurveBits = 256;
        para->a_type = MH_ECC_A_NOT_NEGATIVE_3;
    }
    else if(u32keySzie == 192)
    {
//        u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_192r1);
        bn_read_string_from_head((uint8_t *)pri_key->d.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PRIA_192r1);
        bn_read_string_from_head((uint8_t *)pub_key->x.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBXA_192r1);
        bn_read_string_from_head((uint8_t *)pub_key->y.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBYA_192r1);
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
//        u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_224r1);
        bn_read_string_from_head((uint8_t *)pri_key->d.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PRIA_224r1);
        bn_read_string_from_head((uint8_t *)pub_key->x.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBXA_224r1);
        bn_read_string_from_head((uint8_t *)pub_key->y.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBYA_224r1);
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
//        u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_384r1);
        bn_read_string_from_head((uint8_t *)pri_key->d.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PRIA_384r1);
        bn_read_string_from_head((uint8_t *)pub_key->x.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBXA_384r1);
        bn_read_string_from_head((uint8_t *)pub_key->y.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBYA_384r1);
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
//        u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_521r1);
        bn_read_string_from_head((uint8_t *)pri_key->d.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PRIA_521r1);
        bn_read_string_from_head((uint8_t *)pub_key->x.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBXA_521r1);
        bn_read_string_from_head((uint8_t *)pub_key->y.pu32Buf, MH_ECC_BYTE_SIZE, ECC_PUBYA_521r1);
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

void ecies_EncDec_GenKey_test(void)
{
	uint8_t i,j;
	uint32_t au32key_size[5] = {192, 224, 256, 384, 521}, u32keySzie;
	
	ecc_para eccPara;
	ecc_key eccKey;
	
    mh_ecc_curve_para eccCurvePara;
    
	mh_ecc_private_key priKey;
    mh_ecc_public_key pubKey;
    
	ecc_bignum_data BigOutput;
    ecc_bignum_data BigInput;
    
	mh_ecc_share_info ShareInfo;
    
	uint8_t au8Sinfo1[32], au8Sinfo2[32], au8Msg[ECC_MAX_MSG_LEN],au8D[MH_ECC_BYTE_SIZE],au8PubX[MH_ECC_BYTE_SIZE],
                    au8PubY[MH_ECC_BYTE_SIZE],au8Cipher[MH_ECC_BYTE_SIZE*2 + 32 + ECC_MAX_MSG_LEN];
    uint16_t u16CipherLen, u16Sinfo1LEN, u16Sinfo2Len;
    uint32_t u32MLen;
	uint32_t u32Ret;
	
	uint8_t au8P[KEY_LEN_MAX],au8A[KEY_LEN_MAX],au8AM[KEY_LEN_MAX],au8B[KEY_LEN_MAX],au8N[KEY_LEN_MAX],au8GX[KEY_LEN_MAX],au8GY[KEY_LEN_MAX];
	uint8_t au8PC[KEY_LEN_MAX],au8NC[KEY_LEN_MAX];
	uint8_t au8CipMsg[ECC_MAX_MSG_LEN];
	uint16_t u16CurParaLen;
    
	u16Sinfo1LEN = bn_read_string_from_head(au8Sinfo1, sizeof(au8Sinfo1), SINFO1);
    u16Sinfo2Len = bn_read_string_from_head(au8Sinfo2, sizeof(au8Sinfo2), SINFO2);
		
		
	for(i = 0; i < (sizeof(au32key_size)/4); i ++)
	{
		u32keySzie = au32key_size[i];
		DBG_PRINTF("\nECIES %d GenKey Test start\n", u32keySzie);
		

		//Use GenKey & Test GenKey
		eccPara.p = (uint32_t *)au8P;
		eccPara.a = (uint32_t *)au8A;
		eccPara.b = (uint32_t *)au8B;
		eccPara.n = (uint32_t *)au8N;
		eccPara.g.x = (uint32_t *)au8GX;
		eccPara.g.y = (uint32_t *)au8GY;
		eccPara.p_c = (uint32_t *)au8PC;
		eccPara.n_c = (uint32_t *)au8NC;
		
		
		eccKey.d = (uint32_t *)au8D;
		eccKey.e.x = (uint32_t *)au8PubX;
		eccKey.e.y = (uint32_t *)au8PubY;
		
//		priKey.d.pu32Buf = (uint32_t *)au8D;
//		pubKey.x.pu32Buf = (uint32_t *)au8PubX;
//		pubKey.y.pu32Buf = (uint32_t *)au8PubY;
		
		read_key_para(NULL, NULL, &eccCurvePara, u32keySzie);
		
		u16CurParaLen = (eccCurvePara.u32CurveBits + 7)>>3;
		ouputRes("p:\n", eccCurvePara.p, u16CurParaLen);
		ouputRes("a:\n", eccCurvePara.a, u16CurParaLen);
		ouputRes("b:\n", eccCurvePara.b, u16CurParaLen);
		ouputRes("n:\n", eccCurvePara.n, u16CurParaLen);
		ouputRes("gx:\n", eccCurvePara.g.x, u16CurParaLen);
		ouputRes("gy:\n", eccCurvePara.g.y, u16CurParaLen);
		ouputRes("pc:\n", eccPara.p_c, u16CurParaLen);
		ouputRes("nc:\n", eccPara.n_c, u16CurParaLen);
		
		mh_ecc_set_para(&eccPara, &eccCurvePara);
//		
//		ouputRes("\np:\n", eccPara.p, u16CurParaLen);
//		ouputRes("a:\n", eccPara.a, u16CurParaLen);
//		ouputRes("b:\n", eccPara.b, u16CurParaLen);
//		ouputRes("n:\n", eccPara.n, u16CurParaLen);
//		ouputRes("gx:\n", eccPara.g.x, u16CurParaLen);
//		ouputRes("gy:\n", eccPara.g.y, u16CurParaLen);
//		ouputRes("pc:\n", eccPara.p_c, u16CurParaLen);
//		ouputRes("nc:\n", eccPara.n_c, u16CurParaLen);
		
		
		
		u32Ret = ecc_genkey(&eccKey, &eccPara, mh_rand_p, NULL);
		
		if(MH_RET_ECC_KEY_GENERATION_SUCCESS != u32Ret)
		{
			DBG_PRINTF("ecc GenKey Error.\n");
			while(1);
		}
		
		ouputRes("D:\n", au8D, u16CurParaLen);
		ouputRes("X:\n", au8PubX, u16CurParaLen);
		ouputRes("Y:\n", au8PubY, u16CurParaLen);
		u32MLen = 0;
		
		
		while(u32MLen == 0)
		{
			mh_rand(&u32MLen, sizeof(u32MLen));
			u32MLen = (u32MLen & (ECC_MAX_MSG_LEN - 1));
		}
		DBG_PRINTF("MSG len = %d.\n", u32MLen);
		
		memset(au8Msg, 0, sizeof(au8Msg));
		mh_rand(&au8Msg, u32MLen);
		
		ouputRes("MSG:\n", au8Msg, u32MLen);
		
		ShareInfo.pu8Sinfo1 = au8Sinfo1;    
		ShareInfo.u32Sinfo1len = u16Sinfo1LEN;
		ShareInfo.pu8Sinfo2 = au8Sinfo2;    
		ShareInfo.u32Sinfo2len = u16Sinfo2Len;
		
		//Enc
		memset(au8Cipher, 0, sizeof(au8Cipher));
//		memset(au8Msg, 0, sizeof(au8Msg));
		
//		u32MLen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MSG);
		
		pubKey.x.pu32Buf = (uint32_t *)au8PubX;      
		pubKey.x.u32Len = (eccCurvePara.u32CurveBits + 7)>>3;
		pubKey.y.pu32Buf = (uint32_t *)au8PubY;      
		pubKey.y.u32Len = (eccCurvePara.u32CurveBits + 7)>>3;
		
		BigInput.pu8Buf = au8Msg;   
		BigInput.u32Len = u32MLen;
		
		BigOutput.pu8Buf = au8Cipher;   
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS ==mh_ecies_enc(&BigOutput, &BigInput, &ShareInfo, &pubKey, &eccCurvePara, mh_rand_p, NULL));
		ouputRes("output.\n", au8Cipher,BigOutput.u32Len);
		r_printf(u32Ret, "ECIES enc test\n");
		
		//Dec
		memset(au8CipMsg, 0, sizeof(au8CipMsg));
		
		priKey.d.pu32Buf = (uint32_t *)au8D;
		priKey.d.u32Len = (eccCurvePara.u32CurveBits + 7)>>3;
		
	//	
		BigInput.pu8Buf = au8Cipher;  
		BigInput.u32Len = BigOutput.u32Len;
		
		BigOutput.pu8Buf = au8CipMsg;
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS == mh_ecies_dec(&BigOutput, &BigInput, &ShareInfo, &priKey, &eccCurvePara, mh_rand_p, NULL));
		ouputRes("Cipher MSG.\n", au8CipMsg, BigOutput.u32Len);
		
		r_printf(((0 == memcmp(au8CipMsg, au8Msg, u32MLen)) && (u32Ret)), "ECIES GenKey enc\/dec test \n");	
	}
	
	DBG_PRINTF("\nECIES GenKey Test End\n");
}

void ecies_EncDec_RandData_test(void)
{
	uint8_t i,j;
	uint32_t au32key_size[5] = {192, 224, 256, 384, 521};
	
	ecc_para eccPara;
	ecc_key eccKey;
	
    mh_ecc_curve_para para;
    mh_ecc_private_key priKey;
    mh_ecc_public_key pubKey;
    ecc_bignum_data BigOutput;
    ecc_bignum_data BigInput;
    mh_ecc_share_info ShareInfo;
    uint8_t au8Sinfo1[32], au8Sinfo2[32], au8Msg[ECC_MAX_MSG_LEN],au8D[MH_ECC_BYTE_SIZE],au8PubX[MH_ECC_BYTE_SIZE],
                    au8PubY[MH_ECC_BYTE_SIZE],au8Cipher[MH_ECC_BYTE_SIZE*2 + 32 + ECC_MAX_MSG_LEN];
	
	uint8_t au8CipherMsg[ECC_MAX_MSG_LEN];
	
    uint16_t u16MLen, u16CipherLen, u16Sinfo1LEN, u16Sinfo2Len;
    uint32_t u32Ret;
    DBG_PRINTF("\nECIES enc Test start\n");
    //set para
    
    u16Sinfo1LEN = bn_read_string_from_head(au8Sinfo1, sizeof(au8Sinfo1), SINFO1);
    u16Sinfo2Len = bn_read_string_from_head(au8Sinfo2, sizeof(au8Sinfo2), SINFO2);
	
	for(i = 0; i < (sizeof(au32key_size)/4); i ++)
	{
		DBG_PRINTF("\nECIES %d  enc\/dec Test start\n", au32key_size[i]);
		
		priKey.d.pu32Buf = (uint32_t *)au8D;
		pubKey.x.pu32Buf = (uint32_t *)au8PubX;
		pubKey.y.pu32Buf = (uint32_t *)au8PubY; 
		
		read_key_para(&priKey, &pubKey, &para, au32key_size[i]);
		
		ShareInfo.pu8Sinfo1 = au8Sinfo1;    
		ShareInfo.u32Sinfo1len = u16Sinfo1LEN;
		ShareInfo.pu8Sinfo2 = au8Sinfo2;    
		ShareInfo.u32Sinfo2len = u16Sinfo2Len;
		
		memset(au8Cipher, 0, sizeof(au8Cipher));
		memset(au8Msg, 0, sizeof(au8Msg));
			
//		u16MLen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MSG);
		
		u16MLen = 0;
		while(u16MLen == 0)
		{ 
			mh_rand(&u16MLen, sizeof(u16MLen));
			u16MLen = (u16MLen & (ECC_MAX_MSG_LEN - 1));
		}
		DBG_PRINTF("MSG len = %d.\n", u16MLen);
		
		
		mh_rand(au8Msg, u16MLen);
		ouputRes("Msg:\n", au8Msg, u16MLen);
		
		      
		pubKey.x.u32Len = (para.u32CurveBits + 7)>>3;
		     
		pubKey.y.u32Len = (para.u32CurveBits + 7)>>3;
		
		BigInput.pu8Buf = au8Msg;   
		BigInput.u32Len = u16MLen;
		
		BigOutput.pu8Buf = au8Cipher;   
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS ==mh_ecies_enc(&BigOutput, &BigInput, &ShareInfo, &pubKey, &para, mh_rand_p, NULL));
		ouputRes("output.\n", au8Cipher,BigOutput.u32Len);
		r_printf(u32Ret, "ECIES enc test\n");
		
		memset(au8CipherMsg, 0, sizeof(au8CipherMsg));
		
		    
		priKey.d.u32Len = (para.u32CurveBits + 7)>>3;
		
		BigInput.pu8Buf = au8Cipher;  
		BigInput.u32Len = BigOutput.u32Len;
		
		BigOutput.pu8Buf = au8CipherMsg;
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS == mh_ecies_dec(&BigOutput, &BigInput, &ShareInfo, &priKey, &para, mh_rand_p, NULL));
		ouputRes("output.\n", au8CipherMsg,BigOutput.u32Len);
		
		r_printf(((0 == memcmp(au8CipherMsg, au8Msg, u16MLen)) && (u32Ret)), "ECIES dec test by self enc \n");
	}
}

void ecies_EncDec_Fixed_test(void)
{
	uint8_t i,j;
	uint32_t au32key_size[5] = {192, 224, 256, 384, 521};
	
	ecc_para eccPara;
	ecc_key eccKey;
	
    mh_ecc_curve_para para;
    mh_ecc_private_key priKey;
    mh_ecc_public_key pubKey;
    ecc_bignum_data BigOutput;
    ecc_bignum_data BigInput;
    mh_ecc_share_info ShareInfo;
    uint8_t au8Sinfo1[32], au8Sinfo2[32], au8Msg[ECC_MAX_MSG_LEN],au8D[MH_ECC_BYTE_SIZE],au8PubX[MH_ECC_BYTE_SIZE],
                    au8PubY[MH_ECC_BYTE_SIZE],au8Cipher[MH_ECC_BYTE_SIZE*2 + 32 + ECC_MAX_MSG_LEN];
    uint16_t u16MLen, u16CipherLen, u16Sinfo1LEN, u16Sinfo2Len;
    uint32_t u32Ret;
    DBG_PRINTF("\nECIES enc Test start\n");
    //set para
    
    u16Sinfo1LEN = bn_read_string_from_head(au8Sinfo1, sizeof(au8Sinfo1), SINFO1);
    u16Sinfo2Len = bn_read_string_from_head(au8Sinfo2, sizeof(au8Sinfo2), SINFO2);
	
	for(i = 0; i < (sizeof(au32key_size)/4); i ++)
	{
		DBG_PRINTF("\nECIES %d  enc\/dec Test start\n", au32key_size[i]);
		
//		read_key_para(&priKey, &pubKey, &para, au32key_size[i]);
		if(256 == au32key_size[i])
		{
			u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_256r1);
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_256r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_256r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_256r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_256r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_256r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_256r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_256r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_256r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_256r1);
			para.u32CurveBits = 256;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		} 
		else if(192 == au32key_size[i])
		{
			u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_192r1);
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_192r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_192r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_192r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_192r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_192r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_192r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_192r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_192r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_192r1);
			para.u32CurveBits = 192;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
	   
		else if(224 == au32key_size[i])
		{
			u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_224r1);
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_224r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_224r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_224r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_224r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_224r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_224r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_224r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_224r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_224r1);
			para.u32CurveBits = 224;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
		else if(384 == au32key_size[i])
		{
			u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_384r1);
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_384r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_384r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_384r1);
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
			u16CipherLen = bn_read_string_from_head(au8Cipher, sizeof(au8Cipher), ECC_CIPHER_521r1);
			bn_read_string_from_head(au8D, MH_ECC_BYTE_SIZE, ECC_PRIA_521r1);
			bn_read_string_from_head(au8PubX, MH_ECC_BYTE_SIZE, ECC_PUBXA_521r1);
			bn_read_string_from_head(au8PubY, MH_ECC_BYTE_SIZE, ECC_PUBYA_521r1);
			bn_read_string_from_head((uint8_t *)para.p, MH_ECC_BYTE_SIZE, ECC_P_521r1);
			bn_read_string_from_head((uint8_t *)para.a, MH_ECC_BYTE_SIZE, ECC_A_521r1);
			bn_read_string_from_head((uint8_t *)para.b, MH_ECC_BYTE_SIZE, ECC_B_521r1);
			bn_read_string_from_head((uint8_t *)para.n, MH_ECC_BYTE_SIZE, ECC_N_521r1);
			bn_read_string_from_head((uint8_t *)para.g.x, MH_ECC_BYTE_SIZE, ECC_GX_521r1);
			bn_read_string_from_head((uint8_t *)para.g.y, MH_ECC_BYTE_SIZE, ECC_GY_521r1);
			para.u32CurveBits = 521;
			para.a_type = MH_ECC_A_NOT_NEGATIVE_3;
		}
		
		ShareInfo.pu8Sinfo1 = au8Sinfo1;    
		ShareInfo.u32Sinfo1len = u16Sinfo1LEN;
		ShareInfo.pu8Sinfo2 = au8Sinfo2;    
		ShareInfo.u32Sinfo2len = u16Sinfo2Len;
		
		priKey.d.pu32Buf = (uint32_t *)au8D;    
		priKey.d.u32Len = (para.u32CurveBits + 7)>>3;
		
		BigInput.pu8Buf = au8Cipher;  
		BigInput.u32Len = u16CipherLen;
		BigOutput.pu8Buf = au8Msg;
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS == mh_ecies_dec(&BigOutput, &BigInput, &ShareInfo, &priKey, &para, mh_rand_p, NULL));
		ouputRes("output.\n", au8Msg,BigOutput.u32Len);
		r_printf(u32Ret, "ECIES dec test\n");
		
		memset(au8Cipher, 0, sizeof(au8Cipher));
		memset(au8Msg, 0, sizeof(au8Msg));
		
		u16MLen = bn_read_string_from_head(au8Msg, sizeof(au8Msg), MSG);
		
		pubKey.x.pu32Buf = (uint32_t *)au8PubX;      
		pubKey.x.u32Len = (para.u32CurveBits + 7)>>3;
		pubKey.y.pu32Buf = (uint32_t *)au8PubY;      
		pubKey.y.u32Len = (para.u32CurveBits + 7)>>3;
		
		BigInput.pu8Buf = au8Msg;   
		BigInput.u32Len = u16MLen;
		
		BigOutput.pu8Buf = au8Cipher;   
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS ==mh_ecies_enc(&BigOutput, &BigInput, &ShareInfo, &pubKey, &para, mh_rand_p, NULL));
		ouputRes("output.\n", au8Cipher,BigOutput.u32Len);
		r_printf(u32Ret, "ECIES enc test\n");
		
		memset(au8Msg, 0, sizeof(au8Msg));
		
		priKey.d.pu32Buf = (uint32_t *)au8D;    
		priKey.d.u32Len = (para.u32CurveBits + 7)>>3;
		
		BigInput.pu8Buf = au8Cipher;  
		BigInput.u32Len = BigOutput.u32Len;
		
		BigOutput.pu8Buf = au8Msg;
		
		u32Ret = (MH_RET_ECC_ECIES_SUCCESS == mh_ecies_dec(&BigOutput, &BigInput, &ShareInfo, &priKey, &para, mh_rand_p, NULL));
		ouputRes("output.\n", au8Msg,BigOutput.u32Len);
		r_printf(u32Ret, "ECIES dec test by self enc \n");
	}
}


void ecies_Test(void)
{
	ecies_EncDec_Fixed_test();
	ecies_EncDec_RandData_test();
	ecies_EncDec_GenKey_test();
}
