#include <stdio.h>
#include "mhscpu.h"
#include "test_emv.h"
#include "test_loopback.h"
#include "uart.h"
#include "emv_core.h"
#include "iso7816_3.h"


#if VERSION_410 == EMV_VERSION

uint8_t cal_lrc(uint8_t *p_buf, uint16_t len)
{
    uint8_t lrc = 0;
    int i = 0;

    for (i = 0; i < len; i++)
    {
        lrc ^= p_buf[i];
    }
    return lrc;
} 

void loop_back(uint8_t u8Slot)
{
    uint8_t i, lrc = 0;
    int32_t s32Stat = 0;
    uint8_t  atr[65];
    ST_APDU_RSP     rsp;
    ST_APDU_REQ     apdu_req;

#if 1
    ST_APDU_REQ  sel_visa_req = {{0x00, 0xA4, 0x04, 0x00},  //cmd
                                  0x00000007,               //lc
                                {0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10, 0x00},
                                  0x00000100                //le
                                };
#endif

    while (1)
    {
        wait_Ns(3);
        //Init emv_devs param.
        iso7816_device_init();
        while (0 != iso7816_detect(u8Slot))
        {
            DBG_PRINT("Wait insert ICC!\n");
            delay_Ms(500);
        }
        DBG_PRINT("Insert IC\n");
        if (0 == (s32Stat = iso7816_init(u8Slot, VCC_5000mV | SPD_1X, atr)))
        {
            //DBG_PRINT("iso7816_init() finished.\n");
        }
        else
        {
            DBG_PRINT("iso7816_init() failed %d.\n", s32Stat);
        }

        lrc = 0;
        rsp.len_out = 0;
        while (0 == iso7816_detect(u8Slot) && 0 == s32Stat)
        {
            if ('b' == uart_RecvChar())
            {
                DBG_PRINT("Break loop!\n");
                break;
            }
            if (rsp.len_out < 6)
            {
                memcpy(&apdu_req, &sel_visa_req, 16);//lc is four bytes len
                apdu_req.cmd[3] = lrc;
                apdu_req.le = 256;
            }
            else
            {
                memcpy(apdu_req.cmd, rsp.data_out, 3);
                apdu_req.cmd[3] = lrc;
                apdu_req.lc = 0;
                apdu_req.le = 0;

                if (rsp.data_out[3] > 2 && rsp.data_out[4] > 0)
                {
                    apdu_req.lc = rsp.data_out[4];
                    for (i = 0; i < apdu_req.lc; i++)
                    {
                        apdu_req.data_in[i] = i;
                    }
                }
                if (4 == rsp.data_out[3] || 2 == rsp.data_out[3])
                {
                    if (0 == rsp.data_out[5])
                    {
                        apdu_req.le = 256;
                    }
                    else
                    {
                        apdu_req.le = rsp.data_out[5];
                    }
                }
            }
            //delay_Ms(300);
            if (0 != (s32Stat = iso7816_exchange(u8Slot, AUTO_GET_RSP, &apdu_req, &rsp)))
            {
                DBG_PRINT("Exchange failed %d!\n", s32Stat);
                rsp.len_out = 0;
                break;
            }
            /*
            else
            {
                print_hex("recv:", rsp.data_out, rsp.len_out);
                DBG_PRINT("SWA: %x SWB: %x!\n", rsp.swa, rsp.swb);
            }
            */
            
            if (0x6A == rsp.swa && 0x82 == rsp.swb)
            {
                DBG_PRINT("This test Case finished!\n");
                break;
            }
            lrc = cal_lrc(rsp.data_out, rsp.len_out);
            lrc ^= rsp.swa;
            lrc ^= rsp.swb;
        }
        iso7816_close(u8Slot);
        //DBG_PRINT("Shut down ICC!\n");
        /*
        while('c' != s32Stat)
        {
            DBG_PRINT("Press 'c' to continue\n");
            while (-1UL == (s32Stat = uart_RecvChar()));
            DBG_PRINT("%c\n", s32Stat);
        }
        */
    }
}

#elif VERSION_43A == EMV_VERSION

#define ICBS_TEST       (1)

void loop_back(uint8_t u8Slot)
{
    uint8_t u8Wait = 5;
	char *pu8Str = "Class B(3.0V)";
    int32_t s32Vol, s32Len, s32Stat = 0;
    uint8_t  atr[65];
    ST_APDU_RSP     rsp;
    ST_APDU_REQ     apdu_req;

    s32Vol = VCC_3000mV;
  
    if ('1' == s32Stat)
    {
        s32Vol = VCC_3000mV;
        pu8Str = "Class B(3.0V)";
    }
    else if ('2' == s32Stat)
    {
        s32Vol = VCC_1800mV;
        pu8Str = "Class C(1.8V)";
    }
    printf("Voltage %s\r\n", pu8Str);
  
    while (1)
    {
        #if 1 == ICBS_TEST
        //ICBC will not drawn ICC in next test case.
        u8Wait = wait_ChangeNs(u8Wait);
        #else
        wait_Ns(5);
        #endif
        //Init emv_devs param.
        iso7816_device_init();
        while (0 != iso7816_detect(u8Slot))
        {
            DBG_PRINT("Wait insert ICC!\n");
            delay_Ms(500);
        }
        DBG_PRINT("Insert IC\n");
        if (0 == (s32Stat = iso7816_init(u8Slot, s32Vol | SPD_1X, atr)))
        {
            DBG_PRINT("iso7816_init finished!\n");
        }
        else
        {
            DBG_PRINT("iso7816_init failed %d!\n", s32Stat);
        }

        rsp.len_out = 0;
        while (0 == iso7816_detect(u8Slot) && 0 == s32Stat)
        {
            if ('b' == uart_RecvChar())
            {
                DBG_PRINT("Break loop!\n");
                break;
            }
            if (rsp.len_out < 4)
            {
                //INS equal 70 then deactivate card.
                if (rsp.len_out > 0 && 0x70 == rsp.data_out[1])
                {
                    DBG_PRINT("This test Case finished!\n");
                    break;
                }
                DBG_PRINT("Prepare to send select PSE!\n");
                memcpy(apdu_req.cmd, "\x00\xa4\x04\x00", 4);
                apdu_req.lc = 14;
                memcpy( apdu_req.data_in, "1PAY.SYS.DDF01", apdu_req.lc);
                apdu_req.le = 256;
                //print_hex("select", (void *)&apdu_req, 128);
            }
            else
            {
                //INS equal 70 then deactivate card.
                if (0x70 == rsp.data_out[1])
                {
                    DBG_PRINT("This test Case finished!\n");
                    break;
                }
                DBG_PRINT("Prepare to send loopback data!\n");
                //Copy next C-APDU from this R-APDU.
                memcpy(apdu_req.cmd, rsp.data_out, sizeof(apdu_req.cmd));
                
                //Len except (CLA INS P1 P2)
                apdu_req.lc = 0;
                apdu_req.le = 0;
                s32Len = rsp.len_out - sizeof(apdu_req.cmd);
                if (s32Len > 1 )
                {
                    apdu_req.lc = rsp.data_out[sizeof(apdu_req.cmd)];
                    memcpy(apdu_req.data_in, rsp.data_out + 1 + sizeof(apdu_req.cmd), apdu_req.lc);
                }
                //if case 2 or case 4
                if ((rsp.len_out == sizeof(apdu_req.cmd) + 1) || 
                    (apdu_req.lc > 0 && rsp.len_out > apdu_req.lc + sizeof(apdu_req.cmd) + 1))
                {
                    //Le is present.
                    apdu_req.le = rsp.data_out[rsp.len_out];
                    if (0 == apdu_req.le)
                    {
                        apdu_req.le = 256;
                    }
                }
            }
            delay_Ms(200);
            if (0 != (s32Stat = iso7816_exchange(u8Slot, AUTO_GET_RSP, &apdu_req, &rsp)))
            {
                DBG_PRINT("Exchange failed %d!\n", s32Stat);
                rsp.len_out = 0;
                break;
            }
        }
        
        //Deinit struct power down card and.
        iso7816_close(u8Slot);
        DBG_PRINT("Shut down ICC!\n");
#if 1 != ICBS_TEST
        //Waiting for card drawn.
        while (0 == iso7816_detect(u8Slot));
        DBG_PRINT("Remove ICC!\n");
        delay_Ms(1000);
#endif
    }

}
#endif
