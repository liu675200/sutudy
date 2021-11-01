#ifndef _DECODELIB_H_
#define _DECODELIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mhscpu.h"

/************************************************************************************************/
/*ͼ��ת��������*/
typedef enum
{
    FLIP_NORMAL,   //ԭʼͼ��
    FLIP_VER,      //��ֱ��ת
    FLIP_HOZ,      //ˮƽ��ת
    FLIP_VERHOZ    //��ֱˮƽ��ת����ԭʼͼ����ת180��
}SensorImageFlipType;

/************************************************************************************************/
/*����CodeID*/
typedef enum
{
    ID_NONE = 0,      /*δ�⵽��*/
    ID_CODE128,       /*code128*/
    ID_CODE39,        /*code39*/
    ID_CODE93,        /*code93*/
    ID_EAN13,         /*EAN 13*/
    ID_EAN8,          /*EAN 8*/
    ID_UPC_A,         /*UPC_A*/
    ID_UPC_E0,        /*UPC_E0*/
    ID_UPC_E1,        /*UPC_E1*/
    ID_ISBN13,        /*ISBN13*/
    ID_ITF25,         /*���������*/
    ID_QRCODE,        /*QRCode*/
    ID_PDF417,        /*PDF417*/
    ID_DATAMATRIX     /*DataMatrix*/
}DecodeIDType;
/*�������ݽṹ��*/
typedef struct
{
    DecodeIDType id;   /*������id*/
    uint8_t AIM[4];    /*AIM ID*/
    uint8_t *result;   /*��������������׵�ַ*/
    uint32_t resn;     /*���ؽ���������*/
    uint32_t maxn;     /*���������������*/
}DecodeResultTypeDef;
/************************************************************************************************/
/*�����ʼ�����ýṹ��*/
typedef struct
{
    void            *pool;                 /*�ڴ���׵�ַ */
    unsigned int    size;                  /*�ڴ�ش�С   ���ڵ��� 410 * 1024kbyte,ջ�ռ���ڵ���4k */
    GPIO_TypeDef*   CAM_PWDN_GPIOx;        /*camera PWDN gpio */
    uint16_t        CAM_PWDN_GPIO_Pin;     /*camera PWDN gpio pin */
    GPIO_TypeDef*   CAM_RST_GPIOx;         /*camera RESET gpio */
    uint16_t        CAM_RST_GPIO_Pin;      /*camera RESET gpio pin */
    I2C_TypeDef*    CAM_I2Cx;              /*��������ͷʹ�õ�I2C */
    uint32_t        CAM_I2CClockSpeed;     /*I2Cʱ���������ã����ʷ�Χ[100000, 400000]*/
    uint8_t       (*SensorConfig)[2];      /*�Զ�������ͷ���������׵�ַ��ΪNULLʱʹ��Ĭ�����ò���*/
    uint32_t        SensorCfgSize;         /*�Զ�������ͷ���������С*/ 
}DecodeInitTypeDef;

/*������ʼ������״̬��Ϣ*/
typedef enum
{
    DecodeInitSuccess,             /*������ʼ���ɹ�*/            
    DecodeInitCheckError,          /*�����У��ʧ�ܣ�����ϵоƬ����*/
    DecodeInitMemoryError,         /*��������buff��С*/
    DecodeInitSensorError,         /*����ͷ��ʼ��ʧ��*/
}DecodeFlagTypeDef;

/*DeocdeStart ���󷵻�ֵ״̬��Ϣ*/
enum
{
    DecodeResOverflowError = -1,   /*����������buff��С����buff��СС��256byte*/            
    DecodeImageNoDoneError = -2,   /*ͼƬδ�ɼ����*/
    DecodeLibNoInitError   = -3,   /*�����δ��ʼ��*/
};
/*�������ýṹ��*/
typedef struct
{
    /*һά������*/
    uint32_t cfgCODE128;            /*code 128��������      */
    uint32_t cfgCODE39;             /*code 39��������       */
    uint32_t cfgCODE93;             /*code 93��������       */
    uint32_t cfgEAN13;              /*EAN13 ��������        */
    uint32_t cfgEAN8;               /*EAN8 ��������         */
    uint32_t cfgUPC_A;              /*UPC_A ��������        */
    uint32_t cfgUPC_E0;             /*UPC_E0 ��������(UPC_Eǰ����Ϊ0)*/
    uint32_t cfgUPC_E1;             /*UPC_E1 ��������(UPC_Eǰ����Ϊ1)*/
    uint32_t cfgISBN13;             /*ISBN13 ��������       */
    uint32_t cfgInterleaved2of5;    /*����������������    */
    /*��ά������*/
    uint32_t cfgQRCode;             /*qr code ��������      */
    uint32_t cfgPDF417;             /*pdf 417 ��������      */
    uint32_t cfgDataMatrix;         /*data matrix ��������  */
    /*ȫ�ֽ�������*/
    uint32_t cfgGlobal;             /*����ȫ������*/
}DecodeConfigTypeDef;

/*����״̬*/
typedef enum
{
    DECODE_DISABLE = 0,               /*ʧ������*/
    DECODE_ENABLE = !DECODE_DISABLE    /*ʹ�ܵ�ǰ����*/
}DecodeStatus_t;

/*�������״̬����*/
typedef enum
{
    DE_STATUS  = 0,          /*0bit ����״̬λ ����һά�룬��ά�룬��λ����ʹ����ʧ��*/
    DE_EMIT_CHECK = 1,       /*1bit ʹ���������У��λ (��code128,��������)*/
    DE_ADD_CHECK = 2,        /*2bit ʹ������У��       (code39, Interleaved2of5)*/
    //code128
    DE_CODE128_ENHANCE = 7,  /*7bit ʹ��code128ʶ����ǿ����Ҫ����code128���ձȲ���׼����*/
    //code39
    DE_ASCII     = 3,        /*3bit ʹ������full ASCIIģʽ��� (code39)*/
    DE_EMIT_START = 6,       /*6bit ʹ��code39��ʼ�����('*')*/
    DE_EMIT_END   = 7,       /*7bit ʹ��code39���������('*')*/
    //EAN UPC
    DE_UPC2EAN   = 4,        /*4bit UPC_AתEAN13���ʹ��(UPC_A)*/
    DE_MUSTADDN  = 5,        /*5bit ����������������������� (EAN13,UPC_A,EAN8,UPC_E0,UPC_E1)*/
    DE_EANADDON2 = 6,        /*6bit ���2λ������ (EAN13,UPC_A,EAN8,UPC_E0,UPC_E1)*/
    DE_EANADDON5 = 7,        /*7bit ���5λ������ (EAN13,UPC_A,EAN8,UPC_E0,UPC_E1)*/
    
    DE_MIN_LEN   = 16,       /*16bit-23bit ��8bit ������С����*/
    DE_MAX_LEN   = 24        /*24bit-31bit ��8bit ���������*/  
}DeBarCodeStatus_t;

/*QRCode����״̬����*/
typedef enum
{
    DEQR_STATUS        = 0,   /*0bit QRCode����״̬ʹ��λ*/
    DEQR_MISSINGCORNER = 1,   /*1bit QRCodeȱ���㷨ʹ��*/
    DEQR_CURVE         = 3,   /*3bit QRCode��������㷨ʹ��*/
    DEQR_ECICONFIG     = 7,  /*16bit QRCode���ECI����*/
}DeQRCodeStatus_t;

/*�������ȫ������*/
typedef enum
{
    DEBAR_PRECISION   = 0,     /*0bit ���뾫�����������λ*/
    DECODE_ANTICOLOR  = 1,     /*1bit ֧�ַ�ɫ���������λ*/
    DEBAR_DAMAGE      = 2,     /*2bit ���������߽�������λ*/
    DECODE_MIRROR     = 3,     /*3-4bit ���루DM��������������0/1Ϊ�������룬2Ϊ�����룬3Ϊ�����붼���Խ�*/
}DecodeGlobalStatus_t;
/****************************************************************************************************/
/**
 * �����ýṹ���ʼ������
 *
 */
void DecodeConfigInit(DecodeConfigTypeDef *cfg);
/**
 * BCTC��������Ƽ����ýṹ���ʼ������
 *
 */
void BCTCTestConfigInit(DecodeConfigTypeDef *cfg);
/****************************************************************************************************/
/**
 *�����ʼ�����ڴ�س�ʼ��
 * ��ʼ���ṹ��
 *����ֵΪ 1 �����ʼ���ɹ���0 �����ʼ��ʧ��
 */
DecodeFlagTypeDef DecodeInit(DecodeInitTypeDef* DecodeInitStruct);

/**
 *  ��ʼ���뺯��
 *  ����ֵΪ����������
 *  ���� 0 ����ʧ��
 *       DecodeResOverflowError �������������������buffer��Χ�����߸�������buffС��256byte
 *       DecodeImageNoDoneError ��ͼδ���
 *       DecodeLibNoInitError δ��ʼ�����߳�ʼ��ʧ��
 */
int DecodeStart(DecodeConfigTypeDef *cfg, DecodeResultTypeDef *res);
    
/**
 * ʹ��DCMI��ͼ
 *
 *
 */
void DecodeDcmiStart(void);

/**
 * �жϲ�ͼ�Ƿ���� 
 * ����������жϵ��ô˺���(DCMI_CallBackFrame())
 * ����ֵ 0δ�ɼ���� 1 �ɼ���ɣ��Ǳ�����ú���
 */
int DecodeDcmiFinish(void);

/**
 * ���ȫ������buff��־�����õ�ǰ����ͼ��buff����ʧЧ
 *
 */
void CleanDecodeBuffFlag(void);

/**
 * ��ȡ��ǰ����buff�Ƿ��ǿ���״̬
 * ����FALSE��ʾ��ǰ����ͼ��buff��Ϊ��Ч״̬����Ҫ���²�ͼ
 * ����TRUE��ʾ��ǰͼ��buff������һ��buff������Ч״̬
 */
Boolean GetDecodeBuffFlag(void);

/**
 * DCMI_IT_FRAME �жϻص�����
 */
void DCMI_CallBackFrame(void);

/**
 * ����ͼ��ת
 * 
 */
void SetSensorImageFlip(SensorImageFlipType fliptype);

/**
 * �ͷŽ�������Ҫ����Ӳ����Դ
 *
 */
void CloseDecode(void);
     
/**
  * ��ȡ��ǰ�㷨�汾��
  * 16-31bitΪ���汾�ţ�8-15bitΪ�ΰ汾�ţ�0-7bitΪ�����汾��
  */
uint32_t GetDecodeLibVerison(void);

#ifdef __cplusplus
}
#endif

#endif 
