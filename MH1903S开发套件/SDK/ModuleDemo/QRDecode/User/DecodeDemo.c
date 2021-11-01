#include "ProjectConfig.h"
#include "DecodeLib.h"
#include "beep.h"

/**
 *��buff����Demo
 *
 */
 void SingleBuffDecodeDemo(void)
 {
     int i;
    //���屣�������������
    uint8_t result[2100] = {0};
    int32_t resnum;    //���ؽ�����������Ϊ0����ʾ����ʧ�ܣ�Ϊ-1��ʾ��������ƫС
    DecodeConfigTypeDef DecodeCfg = {0};  //���ý�������
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //��ʼ������ṹ��
    
    //���ý�����Ϣ
    /**
     * ����Ĭ������
     *
     * cfg->cfgCODE128 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgCODE39 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgCODE93 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgEAN13 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgUPC_A = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgEAN8 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgUPC_E0 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgUPC_E1 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgISBN13 = 0u;
     * cfg->cfgInterleaved2of5 = (0u | (6u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * 
     * cfg->cfgQRCode = 1u | 1u << (DEQR_MISSINGCORNER) | (1u << DEQR_CURVE);
     * cfg->cfgPDF417 = 0u;
     * cfg->cfgDataMatrix = 0u;
     * 
     * cfg->cfgGlobal = cfg->cfgGlobal = (1u << DEBAR_PRECISION);
     * 
     */
    DecodeConfigInit(&DecodeCfg);

	while(1) 
	{
        //ʹ�ܲ�ͼ
        DecodeDcmiStart();
        //����ͼ�Ƿ���ɣ���Ҫ�жϵ���DCMI_CallBackFrame()����
        while( !DecodeDcmiFinish());
        //��ʼ����
        resnum = DecodeStart(&DecodeCfg, &res);
        
        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\n", res.id, res.AIM);
            for(i = 0; i < resnum; i++)
            {
                printf("%c", result[i]);
            }
            printf("\n");

            //����ɹ�������buff״̬��Ϊ���У���ֹ�ظ�����         
            CleanDecodeBuffFlag();

            beep(200);       
        }
        else if(resnum < 0)
        {
            if(resnum == DecodeResOverflowError)
            {
                printf("���������\n");
            }
            else if(resnum == DecodeImageNoDoneError)
            {
                printf("ͼƬδ�ɼ����\n");
            }
            else if(resnum == DecodeLibNoInitError)
            {
                printf("��δ��ʼ��\n");
            }
        }
        else
        {
            printf("Decoding failed\n");
        }	
    }
 }

 /**
  * ˫buff����demo
  *
  */
 void DoubleBuffDecodeDemo(void)
 {
     int i;
     //���屣�������������
    uint8_t result[2100] = {0};
    int32_t resnum;    //���ؽ�����������Ϊ0����ʾ����ʧ�ܣ�Ϊ-1��ʾ��������ƫС
    DecodeConfigTypeDef DecodeCfg = {0};  //���ý�������
    DecodeResultTypeDef res = {.result = result, .maxn = 2100}; //��ʼ������ṹ��
    
    //���ý�����Ϣ
    /**
     * ����Ĭ������
     *
     * cfg->cfgCODE128 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgCODE39 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgCODE93 = (1u | (0u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * cfg->cfgEAN13 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgUPC_A = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgEAN8 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgUPC_E0 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgUPC_E1 = (1u | (1u << DE_EMIT_CHECK));
     * cfg->cfgISBN13 = 0u;
     * cfg->cfgInterleaved2of5 = (0u | (6u << DE_MIN_LEN) | (0xffu << DE_MAX_LEN));
     * 
     * cfg->cfgQRCode = 1u | 1u << (DEQR_MISSINGCORNER) | (1u << DEQR_CURVE);
     * cfg->cfgPDF417 = 0u;
     * cfg->cfgDataMatrix = 0u;
     * 
     * cfg->cfgGlobal = (1u << DEBAR_PRECISION);
     */
    DecodeConfigInit(&DecodeCfg);
    DecodeCfg.cfgDataMatrix = 1u;
    DecodeCfg.cfgPDF417 = 1u;

    while(1) 
	{
        //������Ӵ����жϣ���ֹ˫buff����Ч����ɿ�����while��
        if(GetDecodeBuffFlag() == FALSE)
        {
            //ʹ�ܲ�ͼ
            DecodeDcmiStart();
        }
        //����ͼ�Ƿ���ɣ���Ҫ�жϵ���DCMI_CallBackFrame()����
        while(!DecodeDcmiFinish());
        //�ɼ����һ��ͼ�����ǰʹ�ܲɼ���һ��buffͼ
         DecodeDcmiStart();
        //��ʼ����
        resnum = DecodeStart(&DecodeCfg, &res);

        if(resnum > 0)
        {
            printf("ID:%d\tAIMID:%s\n", res.id, res.AIM);
            for(i = 0; i < res.resn; i++)
            {
                printf("%c", res.result[i]);
            }
            printf("\n");

            //����ɹ�������buff״̬��Ϊ���У���ֹ�ظ�����         
            CleanDecodeBuffFlag();

            beep(200);       
        }
        else if(resnum < 0)
        {
            if(resnum == DecodeResOverflowError)
            {
                printf("���������\n");
            }
            else if(resnum == DecodeImageNoDoneError)
            {
                printf("ͼƬδ�ɼ����\n");
            }
            else if(resnum == DecodeLibNoInitError)
            {
                printf("��δ��ʼ��\n");
            }
        }
        else
        {
            printf("Decoding failed\n");
        }
	}
 }
