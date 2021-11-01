#ifndef __MH_ECIES_H
#define __MH_ECIES_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "mh_ecc.h"
#include "mh_sm3.h"
#include "mh_misc.h"
#include "mh_rand.h"
#include "mh_sm2.h"

typedef struct
{
    uint8_t *pu8Sinfo1;
    uint8_t *pu8Sinfo2;
    uint32_t u32Sinfo1len;
    uint32_t u32Sinfo2len;
}mh_ecc_share_info;

uint32_t mh_ecies_dec(ecc_bignum_data *pOutput,ecc_bignum_data *pInput, mh_ecc_share_info *ShareInfo, 
                        mh_ecc_private_key *pKey,mh_ecc_curve_para *pEccPara,mh_rng_callback f_rng,void *p_rng);

uint32_t mh_ecies_enc(ecc_bignum_data *pOutput,ecc_bignum_data *pInput, mh_ecc_share_info *ShareInfo,
                        mh_ecc_public_key *pubKey, mh_ecc_curve_para *pEccPara,mh_rng_callback f_rng, void *p_rng);


#ifdef __cplusplus
}
#endif

#endif 

