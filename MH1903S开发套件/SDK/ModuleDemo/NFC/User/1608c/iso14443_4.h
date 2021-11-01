/**
 * =============================================================================
 * 
 *
 * File   : iso14443_4.c
 *
 * Author : Lihongxu     
 * 
 * Date   : 2014-12-03
 *
 * Description:
 *      implement the half duplex communication protocol with ISO14443-4
 *
 * History:
 *
 * =============================================================================
 */

 #ifndef ISO14443_4_H
#define ISO14443_4_H

#ifdef __cplusplus
extern "C" {
#endif 



typedef struct pcd_info_s
{
    unsigned char     ucNad;
    unsigned char     ucNadEn;/*the picc support NAD*/
    unsigned char     ucCid;/*the identification channel*/
    unsigned char     ucCidEn;/*the picc support CID*/
    unsigned int      uiFsc;/*the maximum size of PICC frame*/
    unsigned int      uiFwi;/*the maximum frame waiting time*/
    unsigned int      uiSfgi;/*the frame guard time*/
    /*the PCD and PICC maintain last i block PCB*/
    unsigned char     ucPcdPcb;
    unsigned char     ucPiccPcb;
    /*rf data buffer*/
    unsigned int      uiPcdTxRNum;
    unsigned int      uiPcdTxRLastBits;
	unsigned char	  ucWTXM;

}pcd_info_s;


extern struct pcd_info_s   XDATA   g_pcd_module_info;


enum exe_step_t 
    { 
        ORG_IBLOCK = 1,
        ORG_ACKBLOCK,  
        ORG_NACKBLOCK,
        ORG_SBLOCK,
        ORG_TRARCV, /*transmitted and received*/
        RCV_IBLOCK,
        RCV_RBLOCK,
        RCV_SBLOCK,
        RCV_INVBLOCK,/*protocol error*/ 
        NON_EVENT /*end*/
    };

/*the re-transmission maxium count*/
#define ISO14443_PROTOCOL_RETRANSMISSION_LIMITED  ( 2 )

/**
 * protocol PCB structure 
 */
#define ISO14443_CL_PROTOCOL_CHAINED              ( 0x10 )
#define ISO14443_CL_PROTOCOL_CID                  ( 0x8 )
#define ISO14443_CL_PROTOCOL_NAD                  ( 0x4 )
#define ISO14443_CL_PROTOCOL_ISN                  ( 0x1 )


/**
 * implement the half duplex communication protocol with ISO14443-4
 * 
 * parameters:
 *             ppcd_info  : PCD information structure pointer
 *             psrc     : the datas information will be transmitted by ppcd_info
 *             i_tx_num     : the number of transmitted datas by ppcd_info
 *             pdes     : the datas information will be transmitted by PICC
 *             pi_rx_n      : the number of transmitted datas by PICC.
 * retval:
 *            0 - successfully
 *            others, error.
 */
int ISO14443_4_HalfDuplexExchange( struct pcd_info_s *ppcd_info, 
                                   unsigned char *psrc, 
                                   int i_tx_n, 
                                   unsigned char *pdes, 
                                  unsigned int *pi_rx_n );
/**
 * send 'DESELECT' command to PICC
 *
 * param:
 *       ppcd_info  :  PCD information structure pointer
 *       ucSPcb     :  S-block PCB
 *                       0xC2 - DESELECT( no parameter )
 *                       0xF2 - WTX( one byte wtx parameter )
 *       ucParam    :  S-block parameters
 * reval:
 *       0 - successfully
 *       others, failure
 */
int ISO14443_4_SRequest( struct pcd_info_s *ppcd_info, 
                         unsigned char      ucSPcb,
                         unsigned char      ucParam );
void pcd_default_info(void);

int iso14443_4_deselect(u8 CID);

#define ISO14443_4_ERR_PROTOCOL  ( -100 )

#ifdef __cplusplus
}
#endif

#endif






