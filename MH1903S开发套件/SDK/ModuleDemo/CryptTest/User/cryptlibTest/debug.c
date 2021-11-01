#include <stdio.h>
#include "mhscpu.h"



void r_printf(uint32_t b, char *s)
{
    if (0 != b)
    {
        DBG_PRINT("Pass ");DBG_PRINT("%s", s);
    }
    else
    {
        DBG_PRINT("Fail ");DBG_PRINT("%s", s);
        while(1);
    }
}


void ouputRes(char *pcFmt, void *pvbuff, uint32_t u32Len)
{
    int32_t i, j;
    uint8_t *pu8Buff = pvbuff;
    
    DBG_PRINT("%s", pcFmt);
    for (i = 0; i < u32Len; i++)
    {
        printf("%02X", pu8Buff[i]);
    }
    DBG_PRINT("\n");
}


volatile uint32_t gu32Tick = 0;


void timer_init_ms(uint32_t ms)
{
    TIM_InitTypeDef s;

    SYSCTRL_APBPeriphClockCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
    SYSCTRL_APBPeriphResetCmd(SYSCTRL_APBPeriph_TIMM0, ENABLE);
    
    s.TIMx = TIM_0;
    s.TIM_Period = SYSCTRL->PCLK_1MS_VAL * ms;

    TIM_DeInit(TIMM0);
    TIM_Cmd(TIMM0, TIM_0, DISABLE);
    TIM_Init(TIMM0, &s);
    TIM_ITConfig(TIMM0, TIM_0, ENABLE);
    gu32Tick = 0;
    TIM_Cmd(TIMM0, TIM_0, ENABLE);
}

void time_avg(TIME_MS_US *pTimeTotal, uint32_t u32Count, TIME_MS_US *pTimeAvg)
{
    assert_param(NULL != pTimeTotal);
    assert_param(NULL != pTimeAvg);
    assert_param(0 != u32Count);
    
    pTimeAvg->u32Us = (pTimeTotal->u32Us + (pTimeTotal->u32Ms % u32Count) * 1000) / u32Count;
    pTimeAvg->u32Ms = pTimeTotal->u32Ms / u32Count;
}

void time_add(TIME_MS_US *pTime1, TIME_MS_US *pTime2, TIME_MS_US *pTimeSum)
{
    assert_param(NULL != pTime1);
    assert_param(NULL != pTime2);
    assert_param(NULL != pTimeSum);
    
    pTimeSum->u32Ms = pTime1->u32Ms + pTime2->u32Ms + (pTime1->u32Us + pTime2->u32Us) / 1000;
    pTimeSum->u32Us = (pTime1->u32Us + pTime2->u32Us) % 1000;
}


/**
 * Don't use this function in a interrupt which priority is less than timer tick.
 */

uint32_t get_time(TIME_MS_US *pTime)
{
    uint32_t u32Ms, u32Left, u32Tick;
    
    assert_param(NULL != pTime);
    
    TIM_Cmd(TIMM0, TIM_0, DISABLE);
    u32Left = TIMM0->TIM[TIM_0].CurrentValue;
    u32Tick = gu32Tick;
    //TIM_Cmd(TIMM0, TIM_0, ENABLE);
    
    //Left count less than a tick.
    u32Left = TIMM0->TIM[TIM_0].LoadCount - u32Left;
    //Get left ms
    u32Ms = u32Left / SYSCTRL->PCLK_1MS_VAL;
    
    //Get left us
    pTime->u32Us = (u32Left % SYSCTRL->PCLK_1MS_VAL)  * 1000 / SYSCTRL->PCLK_1MS_VAL;

    //Get total ms(left ms + tick ms).
    pTime->u32Ms = u32Ms + TIMM0->TIM[TIM_0].LoadCount / SYSCTRL->PCLK_1MS_VAL * u32Tick;
    
    TIM_Cmd(TIMM0, TIM_0, ENABLE);
    return 0;
}


void TIM0_0_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIMM0, TIM_0);
    gu32Tick++;
}


