#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

#define cp		"8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3"
#define ca		"787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498"
#define cb		"63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A"
#define cGx		"421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D"
#define cGy		"0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2"
#define cn		"8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7"

#define cAda		"6FCBA2EF9AE0AB902BC3BDE3FF915D44BA4CC78F88E2F8E7F8996D3B8CCEEDEE"
#define cAPx		"3099093BF3C137D8FCBBCDF4A2AE50F3B0F216C3122D79425FE03A45DBFE1655"
#define cAPy		"3DF79E8DAC1CF0ECBAA2F2B49D51A4B387F2EFAF482339086A27A8E05BAED98B"


#define cBda		"5E35D7D3F3C54DBAC72E61819E730B019A84208CA3A35E4C2E353DFCCB2A3B53"
#define cBPx		"245493D446C38D8CC0F118374690E7DF633A8A4BFB3329B5ECE604B2B4F37F43"
#define cBPy		"53C0869F4B9E17773DE68FEC45E14904E0DEA45BF6CECF9918C85EA047C60A4C"

#define cZa			"E4D1D0C3CA4C7F11BC8FF8CB3F4C02A78F108FA098E51A668487240F75E20F31"
#define cZb			"6B4B6D0E276691BD4A11BF72F4FB501AE309FDACB72FA6CC336E6656119ABD67"

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

	bn_read_string_to_bytes(para.p, MH_SM2_KEY_BYTES, cp);
	bn_read_string_to_bytes(para.a, MH_SM2_KEY_BYTES, ca);
	bn_read_string_to_bytes(para.b, MH_SM2_KEY_BYTES, cb);
	bn_read_string_to_bytes(para.n, MH_SM2_KEY_BYTES, cn);
	bn_read_string_to_bytes(para.g.x, MH_SM2_KEY_BYTES, cGx);
	bn_read_string_to_bytes(para.g.y, MH_SM2_KEY_BYTES, cGy);

	bn_read_string_to_bytes(keya.d, MH_SM2_KEY_BYTES, cAda);
	bn_read_string_to_bytes(keya.e.x, MH_SM2_KEY_BYTES, cAPx);
	bn_read_string_to_bytes(keya.e.y, MH_SM2_KEY_BYTES, cAPy);
	bn_read_string_to_bytes(keyb.d, MH_SM2_KEY_BYTES, cBda);
	bn_read_string_to_bytes(keyb.e.x, MH_SM2_KEY_BYTES, cBPx);
	bn_read_string_to_bytes(keyb.e.y, MH_SM2_KEY_BYTES, cBPy);

	bn_read_string_to_bytes(Za, MH_SM2_KEY_BYTES, cZa);
	bn_read_string_to_bytes(Zb, MH_SM2_KEY_BYTES, cZb);
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
