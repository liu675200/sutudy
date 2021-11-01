#include <string.h>
#include <stdio.h>
#include "mhscpu.h"

extern void ecc_naf(int8_t *w, uint32_t *w_len, uint32_t *w_oe, const uint32_t *k, uint32_t k_len);
extern void ecc_naf_point(ecc_point_naf (*naf_p)[8],ecc_point_a *a,const ecc_para *para);

static mh_rng_callback f_rng = mh_rand_p;

typedef struct
{
    uint32_t x[8];
    uint32_t y[8];
}ecc_point_a_value;

typedef struct
{
    uint32_t x[8];
    uint32_t y[8];
    uint32_t z[8];
}ecc_point_j_value;

typedef struct
{
    uint32_t x[8];
    uint32_t y[8];
    uint32_t z[8];
    uint32_t t[8];
}ecc_point_mj_value;

typedef struct
{
    uint32_t p[8];              //??--?????
    uint32_t a[8];              //??????a--?????
    uint32_t b[8];              //??????b--?????
    ecc_point_a_value g;            //??--?????
    uint32_t n[8];              //????--?????
    uint32_t p_c[8];            //?n??????c
    /*
    uint32_t p_q;               //?n????????q
    */
    uint32_t n_c[8];            //?p??????c
    /*
    uint32_t n_q;               //?p????????q
    uint32_t len_bits;          //????,??
    uint32_t len_words;         //????,?
    uint32_t field;             //?
    uint32_t a_type;            //??a????-3bit0:0-??,1-??--?????
    */
}ecc_para_value;                //ecc?????????

typedef struct
{
    uint32_t d[8];              //ecc??
    ecc_point_a_value e;        //ecc??
}ecc_key_value;                 //ecc???????

void ecc_point_a_init(ecc_point_a *a, ecc_point_a_value *a_data)
{
    a->x = a_data->x;
    a->y = a_data->y;
}

void ecc_point_j_init(ecc_point_j *a, ecc_point_j_value *a_data)
{
    a->x = a_data->x;
    a->y = a_data->y;
    a->z = a_data->z;
}

void ecc_point_mj_init(ecc_point_mj *a, ecc_point_mj_value *a_data)
{
    a->x = a_data->x;
    a->y = a_data->y;
    a->z = a_data->z;
    a->t = a_data->t;
}

void ecc_key_init(ecc_key *key, ecc_key_value *key_v)
{
    key->d = key_v->d;
    ecc_point_a_init(&(key->e), &(key_v->e));
}

void ecc_para_init(ecc_para *para, ecc_para_value *para_data)
{
    para->p = para_data->p;
    para->a = para_data->a;
    para->b = para_data->b;
    para->n = para_data->n;
    para->p_c = para_data->p_c;
    para->n_c = para_data->n_c;
    ecc_point_a_init(&(para->g), &(para_data->g));
}



#define KEY_WORDS 256 / 32

#define cp      "8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3"
#define ca      "787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498"
#define cb      "63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A"
#define cGx     "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D"
#define cGy     "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2"
#define cn      "8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7"

void ecc_naf_test(void);
void ecc_naf_point_test(void);
void ecc_pdbl_a_time_test(void);
void ecc_pdbl_mj_time_test(void);
void ecc_padd_a_time_test(void);
void ecc_padd_ja_time_test(void);
void ecc_pmul_calc_time_test(void);

void ecc_get_point(ecc_point_a *p, uint32_t len_words, const char* x, const char* y)
{
    bn_read_string(p->x, len_words, x);
    bn_read_string(p->y, len_words, y);
}

void ecc_printf_point(ecc_point_a *p, uint32_t len_words)
{
    ouputRes("x:\n", p->x, len_words);

    ouputRes("y:\n", p->y, len_words);
}

void ecc_printf_para(ecc_para *p)
{
    printf("para:\n");
    
    printf("len_bits:%d\n", p->len_bits);
    printf("len_words:%d\n", p->len_words);
    
    ouputRes("p:\n", p->p, p->len_words);

    ouputRes("a:\n", p->a, p->len_words);
    ouputRes("b:\n", p->b, p->len_words);
    ecc_printf_point(&p->g, p->len_words);
    
    ouputRes("n:\n", p->n, p->len_words);
    
    ouputRes("p_C:\n", p->p_c, p->len_words);
    DBG_PRINT("p_Q:%X\n", p->p_q);
    
    ouputRes("n_C:\n", p->n_c, p->len_words);printf("\n");
    DBG_PRINT("n_Q:%X\n", p->n_q);
    
}

void ecc_read_para(ecc_para *para_def)
{
    para_def->len_bits = KEY_WORDS * 32;
    para_def->len_words = KEY_WORDS;
    para_def->a_type = !MH_ECC_A_IS_NEGATIVE_3;
    para_def->field = MH_ECC_PRIME;
    bn_read_string(para_def->p, KEY_WORDS, cp);
    bn_read_string(para_def->a, KEY_WORDS, ca);
    bn_read_string(para_def->b, KEY_WORDS, cb);
    bn_read_string(para_def->g.x, KEY_WORDS, cGx);
    bn_read_string(para_def->g.y, KEY_WORDS, cGy);
    bn_read_string(para_def->n, KEY_WORDS, cn);
    bn_get_C(para_def->p_c, para_def->p, para_def->len_words);
    bn_get_Q(&para_def->p_q, para_def->p);
    bn_get_C(para_def->n_c, para_def->n, para_def->len_words);
    bn_get_Q(&para_def->n_q, para_def->n);
}

void ecc_padd_ja_time_test(void)
{
    const char *cx1 = "7DEACE5FD121BC385A3C6317249F413D28C17291A60DFD83B835A45392D22B0A";
    const char *cy1 = "2E49D5E5279E5FA91E71FD8F693A64A3C4A9461115A4FC9D79F34EDC8BDDEBD0"; 
    const char *cx2 = "1657FA75BF2ADCDC3C1F6CF05AB7B45E04D3ACBE8E4085CFA669CB2564F17A9F"; 
    const char *cy2 = "19F0115F21E16D2F5C3A485F8575A128BBCDDF80296A62F6AC2EB842DD058E50"; 
    const char *cx1_x2 = "110FCDA57615705D5E7B9324AC4B856D23E6D9188B2AE47759514657CE25D112"; 
    const char *cy1_y2 = "1C65D68A4A08601DF24B431E0CAB4EBE084772B3817E85811A8510B2DF7ECA1A"; 
    
    uint32_t t;
    
    ecc_para para;
    ecc_point_j a;
    ecc_point_a b;
    ecc_point_j c;
    ecc_point_a exp_point;
    ecc_point_a calc_point;

    ecc_para_value para_v;
    ecc_point_j_value a_v;
    ecc_point_a_value b_v;
    ecc_point_j_value c_v;
    ecc_point_a_value exp_point_v;
    ecc_point_a_value calc_point_v;

    TIME_MS_US time;
    
    //printf("\n/****************ecc_padd_ja test****************/\n");
    
    ecc_para_init(&para, &para_v);
    ecc_point_j_init(&a, &a_v);
    ecc_point_a_init(&b, &b_v);
    ecc_point_j_init(&c, &c_v);
    ecc_point_a_init(&exp_point, &exp_point_v);
    ecc_point_a_init(&calc_point, &calc_point_v);
    
    ecc_read_para(&para);
    
    bn_read_string(a.x, para.len_words, cx1);
    bn_read_string(a.y, para.len_words, cy1);
    bn_set_int(a.z, 1, para.len_words);
    bn_read_string(b.x, para.len_words, cx2);
    bn_read_string(b.y, para.len_words, cy2);
    bn_read_string(exp_point.x, para.len_words, cx1_x2);
    bn_read_string(exp_point.y, para.len_words, cy1_y2);

    timer_init_ms(1);
    ecc_padd_ja(&c, &a, &b, &para);
    get_time(&time);

    ecc_j2a(&calc_point, &c, &para);
    t = (EQUAL == bn_cmp((exp_point.x), (calc_point.x), para.len_words) &&
        EQUAL == bn_cmp((exp_point.y), (calc_point.y), para.len_words));
    printf("%dms %dus ecc_padd_ja time\n", time.u32Ms, time.u32Us);
    r_printf(t, "ecc_padd_ja test\n");
    
}


void ecc_padd_a_time_test(void)
{
    const char *cx1 = "7DEACE5FD121BC385A3C6317249F413D28C17291A60DFD83B835A45392D22B0A";
    const char *cy1 = "2E49D5E5279E5FA91E71FD8F693A64A3C4A9461115A4FC9D79F34EDC8BDDEBD0"; 
    const char *cx2 = "1657FA75BF2ADCDC3C1F6CF05AB7B45E04D3ACBE8E4085CFA669CB2564F17A9F"; 
    const char *cy2 = "19F0115F21E16D2F5C3A485F8575A128BBCDDF80296A62F6AC2EB842DD058E50"; 
    const char *cx1_x2 = "110FCDA57615705D5E7B9324AC4B856D23E6D9188B2AE47759514657CE25D112"; 
    const char *cy1_y2 = "1C65D68A4A08601DF24B431E0CAB4EBE084772B3817E85811A8510B2DF7ECA1A"; 
    
    uint32_t t;
    TIME_MS_US time;
    
    ecc_para para;
    ecc_point_a a;
    ecc_point_a b;
    ecc_point_a exp_point;
    ecc_point_a calc_point;

    ecc_para_value para_v;
    ecc_point_a_value a_v;
    ecc_point_a_value b_v;
    ecc_point_a_value exp_point_v;
    ecc_point_a_value calc_point_v;

    //printf("\n/****************ecc_padd_a test****************/\n");
    
    ecc_para_init(&para, &para_v);
    ecc_point_a_init(&a, &a_v);
    ecc_point_a_init(&b, &b_v);
    ecc_point_a_init(&exp_point, &exp_point_v);
    ecc_point_a_init(&calc_point, &calc_point_v);
    
    ecc_read_para(&para);
    
    bn_read_string(a.x, para.len_words, cx1);
    bn_read_string(a.y, para.len_words, cy1);
    bn_read_string(b.x, para.len_words, cx2);
    bn_read_string(b.y, para.len_words, cy2);
    bn_read_string(exp_point.x, para.len_words, cx1_x2);
    bn_read_string(exp_point.y, para.len_words, cy1_y2);

    timer_init_ms(1);
    ecc_padd_a(&calc_point, &a, &b, &para);
    get_time(&time);
    
    t = (EQUAL == bn_cmp((exp_point.x), (calc_point.x), para.len_words) &&
        EQUAL == bn_cmp((exp_point.y), (calc_point.y), para.len_words));
    printf("%dms %dus ecc_padd_a time\n", time.u32Ms, time.u32Us);
    r_printf(t, "ecc_padd_a test\n");
}


void ecc_pdbl_a_time_test(void)
{
    const char *cdbGx = "5E1E24ACC7FB884895BB954DE21FE26456357D9EE8A32410635F35CDFB0D2846";
    const char *cdbGy = "8314D639881A8BAF1BDD076BF82B090020D80F7A5ACC3C56BDB61BBD305708F4";
    
    uint32_t t;
    TIME_MS_US time;
    
    ecc_para para;
    ecc_point_a exp_point;
    ecc_point_a calc_point;

    ecc_para_value para_v;
    ecc_point_a_value exp_point_v;
    ecc_point_a_value calc_point_v;


    //printf("\n/****************ecc_pdbl_a test****************/\n");
    
    ecc_para_init(&para, &para_v);
    ecc_point_a_init(&exp_point, &exp_point_v);
    ecc_point_a_init(&calc_point, &calc_point_v);
    
    ecc_read_para(&para);
    
    bn_read_string(exp_point.x, para.len_words, cdbGx);
    bn_read_string(exp_point.y, para.len_words, cdbGy);

    timer_init_ms(1);
    ecc_pdbl_a(&calc_point, &(para.g), &para);
    get_time(&time);
    
    t = (EQUAL == bn_cmp((exp_point.x), (calc_point.x), para.len_words) &&
        EQUAL == bn_cmp((exp_point.y), (calc_point.y), para.len_words));
    printf("%dms %dus ecc_pdbl_a time\n", time.u32Ms, time.u32Us);
    r_printf(t, "ecc_pdbl_a test\n");
}


void ecc_pdbl_mj_time_test(void)
{
    const char *cdbGx = "5E1E24ACC7FB884895BB954DE21FE26456357D9EE8A32410635F35CDFB0D2846";
    const char *cdbGy = "8314D639881A8BAF1BDD076BF82B090020D80F7A5ACC3C56BDB61BBD305708F4";
    
    uint32_t t;
    TIME_MS_US time;
    
    ecc_para para;
    ecc_point_a exp_point;
    ecc_point_a calc_point;
    ecc_point_mj mj_g;
    ecc_point_mj mj_gdbl;
    
    ecc_para_value para_v;
    ecc_point_a_value exp_point_v;
    ecc_point_a_value calc_point_v;
    ecc_point_mj_value mj_g_v;
    ecc_point_mj_value mj_gdbl_v;

//  printf("\n/****************ecc_pdbl_mj test****************/\n");
    
    ecc_para_init(&para, &para_v);
    ecc_point_a_init(&exp_point, &exp_point_v);
    ecc_point_a_init(&calc_point, &calc_point_v);
    ecc_point_mj_init(&mj_g, &mj_g_v);
    ecc_point_mj_init(&mj_gdbl, &mj_gdbl_v);
    
    ecc_read_para(&para);
    
    bn_read_string(exp_point.x, para.len_words, cdbGx);
    bn_read_string(exp_point.y, para.len_words, cdbGy); 
    
    bn_cpy(mj_g.x, para.g.x, para.len_words);
    bn_cpy(mj_g.y, para.g.y, para.len_words);
    bn_set_int(mj_g.z, 1, para.len_words);
    ecc_j2mj_t(mj_g.t, mj_g.z, &para);

    //double j

    timer_init_ms(1);
    ecc_pdbl_mj(&mj_gdbl, &mj_g, &para);

    //j to a
    ecc_j2a(&calc_point, (ecc_point_j*)(&(mj_gdbl)), &para);
    get_time(&time);
    
    t = (EQUAL == bn_cmp((exp_point.x), (calc_point.x), para.len_words) &&
        EQUAL == bn_cmp((exp_point.y), (calc_point.y), para.len_words));
    printf("%dms %dus ecc_pdbl_mj time\n", time.u32Ms, time.u32Us);
    r_printf(t, "ecc_j2a test\n");
}



void ecc_pmul_calc_time_test(void)
{
    const char *cK =    "6CB28D99385C175C94F94E934817663FC176D925DD72B727260DBAAE1FB2F96F";
    const char *cKGx =  "110FCDA57615705D5E7B9324AC4B856D23E6D9188B2AE47759514657CE25D112";
    const char *cKGy =  "1C65D68A4A08601DF24B431E0CAB4EBE084772B3817E85811A8510B2DF7ECA1A";
    
    uint32_t t;
    TIME_MS_US time;

    ecc_para para;
    ecc_point_a exp_point;
    ecc_point_a calc_point;

    ecc_para_value para_v;
    ecc_point_a_value exp_point_v;
    ecc_point_a_value calc_point_v;

    uint32_t k[8];

//  printf("\n/****************ecc_pmul_calc test****************/\n");

    ecc_para_init(&para, &para_v);
    ecc_point_a_init(&exp_point, &exp_point_v);
    ecc_point_a_init(&calc_point, &calc_point_v);

    ecc_read_para(&para);
    bn_read_string(k, para.len_words, cK);
    bn_read_string(exp_point.x, para.len_words, cKGx);
    bn_read_string(exp_point.y, para.len_words, cKGy);
//  ecc_printf_para(&para);
//  printf("\nK:\n");bn_printf(k, para.len_words);printf("\n");
//  ecc_printf_point(&exp_point, para.len_words);

    timer_init_ms(1);
    ecc_pmul_calc(&calc_point, &(para.g), k, &para,f_rng, NULL);
    get_time(&time);

    t = (EQUAL == bn_cmp((exp_point.x), (calc_point.x), para.len_words) &&
        EQUAL == bn_cmp((exp_point.y), (calc_point.y), para.len_words));
    printf("%dms %dus ecc_pmul_calc time\n", time.u32Ms, time.u32Us);
    r_printf(t, "ecc_pmul_calc test\n");
}


#if ECC_NAF_SELF_TEST

/**
  * @method ecc_naf_test
  * @brief  ecc_naf??????
  * @param  void
  * @retval void
  */
void ecc_naf_test(void)
{
    const char *cinput = "6CB28D99385C175C94F94E934817663FC176D925DD72B727260DBAAE1FB2F96F";
    uint32_t input[8] = {0};

    int8_t exp_data[8*32+1] = {-1, 0, 0, 0, 7, 0, 0, 0, 9, 0, 0, 0, -1, 0, 0, 0, 
                                    3, 0, 0, 0, 11, 0, 0, 0, 15, 0, 0, 0, -15, 0, 0, 0, 
                                    -1, 0, 0, 0, -5, 0, 0, 0, 11, 0, 0, 0, 11, 0, 0, 0, 
                                    -3, 0, 0, 0, -15, 0, 0, 0, -9, 0, 0, 0, 3, 0, 0, 0, 
                                    -9, 0, 0, 0, 3, 0, 0, 0, 7, 0, 0, 0, -5, 0, 0, 0, 
                                    3, 0, 0, 0, 7, 0, 0, 0, 13, 0, 0, 0, 13, 0, 0, 0, 
                                    -11, 0, 0, 0, 3, 0, 0, 0, 9, 0, 0, 0, -3, 0, 0, 0, 
                                    7, 0, 0, 0, 7, 0, 0, 0, -15, 0, 0, 0, 13, 0, 0, 0, 
                                    15, 0, 0, 0, -13, 0, 0, 0, -9, 0, 0, 0, 7, 0, 0, 0, 
                                    7, 0, 0, 0, -15, 0, 0, 0, -7, 0, 0, 0, 5, 0, 0, 0, 
                                    3, 0, 0, 0, -7, 0, 0, 0, -1, 0, 0, 0, 5, 0, 0, 0, 
                                    9, 0, 0, 0, -1, 0, 0, 0, 5, 0, 0, 0, -7, 0, 0, 0, 
                                    13, 0, 0, 0, 5, 0, 0, 0, 7, 0, 0, 0, -15, 0, 0, 0, 
                                    13, 0, 0, 0, -11, 0, 0, 0, 9, 0, 0, 0, 3, 0, 0, 0, 
                                    9, 0, 0, 0, 9, 0, 0, 0, -3, 0, 0, 0, -7, 0, 0, 0, 
                                    3, 0, 0, 0, -5, 0, 0, 0, -3, 0, 0, 0, -9, 0, 0, 0,
                                    1};
    
                                    
    uint32_t exp_len = 256;
    uint32_t exp_oe = SCALAR_IS_ODD;
                                
    int8_t calc_data[8*32+1] = {0};
    uint32_t calc_len = 0;
    uint32_t calc_oe = 0;
    
    printf("\n/****************ecc_naf test****************/\n");
    
    bn_read_string(input, 8, cinput);
    
    ecc_naf(calc_data, &calc_len, &calc_oe, input, 8);
    
    if(0 == memcmp(exp_data, calc_data, sizeof(exp_data)))
        printf("ecc_naf generate data pass\n");
    else
        printf("ecc_naf generate data fail\n");
                                
    if(exp_len == calc_len)
        printf("ecc_naf data len pass\n");
    else
        printf("ecc_naf data len fail\n");
    
    if(exp_oe == calc_oe)
        printf("ecc_naf data oe pass\n");
    else
        printf("ecc_naf data oe fail\n");
    
    printf("\n");
}


void ecc_naf_point_test()
{
    const char *(cexp_naf_p[8][3]) =    {
                                    {
                                    "421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D",
                                    "0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2",
                                    "7EC28572805023111445DA63AA3487195F9A85949CA1E2DBC9D699D12483D621"
                                    },
                                    
                                    {
                                    "126FE19B98F4548A976C436D97B8C8A8F14405FF9A6D58B556355D70C2C19E69",
                                    "4BD3A18E69CC107212729F6473030132DE0B75BF7482CA5510B532EF7BDDCB93",
                                    "396F350FE2383EA6D64684D14C6CF6AB67670DD1E7C287286179A89B8D141430"
                                    },
                                    
                                    {
                                    "69FB7B95358D0F57CC871DB03C32EBB28B6A769272A630F2DCBC1187BFB037DB",
                                    "4B38C759488F97118AE94DECE05D143046DE06019988A8C9303544EB191BAB3B",
                                    "3A0A0F450374B8075DCFD648DF12E3ADFE947D8FC2BCA8B441F9969FEFD63488"
                                    },
                                    
                                    {
                                    "446F914C41A3FBAAD134C0163C69C65E28C3BBE84D1F5FD25B02879A284B2D18",
                                    "777E0C314411E5B8DB8E5633C1C22C643F842C637579040215E6CB43414F65F9",
                                    "0DC4CA6D07F269600D2ACE01FDADCB7A05EE572DE6CC4D7B5C481047C7A279CA"
                                    },
                                    
                                    {
                                    "78F649A3B453423F2685456D98E0D1D75A5C5281BF959E18BF8FEC9BBD305BC2",
                                    "2E92D2DA7D97EB760B379A6B973AA030B3BE4C061421DF4D9661A01C0313DD91",
                                    "56B003C3CE6C63A2DD8189CA283557AD91B4378B4823722FDBCD3B6F05DE0232"
                                    },
                                    
                                    {
                                    "82B35A0AA0E9C749B3854FFB6E82280695A5A278B42C0D2651E66CD8BAB6C703",
                                    "4C9B1BA6C2A8F9A6A4F434ED88AFA39D96251CF3E00B9934D7356CF45F0FF08B",
                                    "38A7BAF7895B557243C4EF4836C05440AF4D669D7C39B8489AF96E96A9E1EF38"
                                    },
                                    
                                    {
                                    "538EE264D3C80D48C346A14DA20BF896621FD7F342397ADAA155D1F76D5FCFCA",
                                    "66224E71FD4B0573DA3C20791DBBF0E4B7BCD90E3BA2F2683EB3621F206F1231",
                                    "1F20882C4EB949A50E7D03BCA1B406F98DB5AA8320A25F15337B796BE882CD92"
                                    },
                                    
                                    {
                                    "632F80529E77874D4A7EADBA34F1DCC8F64E81B9897F060ABECC0CA87B7554F4",
                                    "7523C27B4BE879C7E4C653B60B3122969924A03321AB0CD9DA6FE10FAC7163BE",
                                    "101F1423001BD55103F2D07FB43ED547AC4DE35E3A9A44A397BEFA7B5C807C05"
                                    }
                                };
    
    int32_t i;
    
    ecc_para para;
    ecc_para_value para_data;

    struct
    {
        uint32_t x[8];
        uint32_t y[8];
        uint32_t _y[8];
    }exp_naf_v[8],calc_naf_v[8];
    
    ecc_point_naf calc_naf[8];
                                
                                
    printf("\n/****************ecc_naf_point test****************/\n");
                                
    for(i = 0; i < 8; i++)
    {
        bn_read_string(exp_naf_v[i].x, 8, cexp_naf_p[i][0]);
        bn_read_string(exp_naf_v[i].y, 8, cexp_naf_p[i][1]);
        bn_read_string(exp_naf_v[i]._y, 8, cexp_naf_p[i][2]);

        calc_naf[i].x = calc_naf_v[i].x;
        calc_naf[i].y = calc_naf_v[i].y;
        calc_naf[i]._y = calc_naf_v[i]._y;
    }

    ecc_para_init(&para, &para_data);
    
    para.len_bits = KEY_WORDS * 32;
    para.len_words = KEY_WORDS;
    para.a_type = !ECC_A_IS_NEGATIVE_3;
    para.field = ECC_PRIME;
    bn_read_string(para.p, KEY_WORDS, cp);
    bn_read_string(para.a, KEY_WORDS, ca);
    bn_read_string(para.b, KEY_WORDS, cb);
    bn_read_string(para.g.x, KEY_WORDS, cGx);
    bn_read_string(para.g.y, KEY_WORDS, cGy);
    bn_read_string(para.n, KEY_WORDS, cn);
    bn_get_C(para.p_c, para.p, para.len_words);
    bn_get_Q(&para.p_q, para.p);
    bn_get_C(para.n_c, para.n, para.len_words);
    bn_get_Q(&para.n_q, para.n);
    
    ecc_naf_point(&calc_naf, &para.g, &para);
    
    if(0 == memcmp(calc_naf_v, exp_naf_v, sizeof(exp_naf_v)))
        printf("ecc_naf_point data pass\n");
    else
        printf("ecc_naf_point data fail\n");
    
}

#endif //ECC_NAF_SELF_TEST
