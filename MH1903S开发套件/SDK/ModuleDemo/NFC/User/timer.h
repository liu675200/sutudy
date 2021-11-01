/*****************************************************************************
 * Filename	: mhscpu_msr.h
 * Description	: 
 * Author		: Jin yuanbing
 * ---------------------------------------------------------------------------
 * Modification Record:
 *   Version	Name			date 		Content
 *		1.0		Jin yuanbing	2015-12-12 	Create this file.
 *
 * Note:
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2015 Mega Hunt Micro Technology Inc. All Rights Reserved.
 *****************************************************************************/

#ifndef __TIMER_H_
#define	__TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "mhscpu.h"
#include "stdint.h"

typedef uint32_t tick;


/*****************************************************************************
 * Name		: init_timer0
 * Function	: initial the timer 0, it will be token interrupt interval 1ms.
 * ---------------------------------------------------------------------------
 * Input Parameters:None
 * Output Parameters:None
 * Return Value:
 * ---------------------------------------------------------------------------
 * Description:
 * 		Calling get_tick(), mdelay(), is_timeout() before must init the timer 0.
 *****************************************************************************/
void init_timer0(void);

/*****************************************************************************
 * Name		: get_tick
 * Function	: Get current tick.
 * ---------------------------------------------------------------------------
 * Input Parameters:None
 * Output Parameters:None
 * Return Value:
 *		Return current tick.
 * ---------------------------------------------------------------------------
 * Description:
 * 
 *****************************************************************************/
tick get_tick(void);


/*****************************************************************************
 * Name		: get_diff_tick
 * Function	: Get the time interval (unit is 1ms) for two tick.
 * ---------------------------------------------------------------------------
 * Input Parameters:
 *			cur_tick		lastest tick
 *			prior_tick		prior tick
 * Output Parameters:None
 * Return Value:
 *			Return the ticks of two tick difference.
 * ---------------------------------------------------------------------------
 * Description:
 * 
 *****************************************************************************/
tick get_diff_tick(tick cur_tick, tick prior_tick);


/*****************************************************************************
 * Name		: mdelay
 * Function	: Millisecond delay.
 * ---------------------------------------------------------------------------
 * Input Parameters:
 *			msec		Milliseconds of delay
 * Output Parameters:None
 * Return Value:none
 * ---------------------------------------------------------------------------
 * Description:
 * 	
 *****************************************************************************/
void mdelay(tick msec);

/*****************************************************************************
 * Name		: is_timeout
 * Function	: Determine whether the timeout that millisecond.
 * ---------------------------------------------------------------------------
 * Input Parameters:
 *			start_time	start time
 *			interval		time interval
 * Output Parameters:None
 * Return Value:
 *			TRUE		timeout
 *			FALSE		It is not timeout
 * ---------------------------------------------------------------------------
 * Description:
 * 	
 *****************************************************************************/
int is_timeout(tick start_time, tick interval);


#ifdef __cplusplus
}
#endif
	
#endif   ///< __TIMER_H_
