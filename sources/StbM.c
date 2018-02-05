/******************************************************************************
*  Project     : 
*  Platform    : 
*  Filename    : StbM.c
*  Author      : James (zhuangmj )
*  SW Version  : 1.5.0
*  Date        : 02/09/2014
*  Description : AUTOSAR SynchronizedTimeBaseManager 定时器模块
*  Modification History: 
*   <author>     <time>     <version>   <description>
*   ZhuangMJ	05/15/2013  1.0         Added: build this moudle
*   ZhuangMJ	07/25/2013  1.1         Added: change the Marco define; change the is_timeout() function
*   ZhuangMJ    09/10/2013  1.2         Added: add timeout() function for seconds
*   ZhuangMJ    11/01/2013  1.2.1       Added: 增加 TimeoutReset() 功能检查超时并重置计时器
*                                       Fixed: 删除 StbM_GetGlobalTime() 外部引用
*   ZhuangMJ    11/02/2013  1.3         Added: 增加 Elapsed() 计算流逝时间
*   ZhuangMJ    11/07/2013  1.4         Added: 增加 SetTime() 重置内部时间
*   ZhuangMJ    02/09/2013  1.5         Added: 增加 StbM_Relay() 超时继电器模块
*
*  (c) Copyright 2010-2014  
*  All Rights Reserved.
******************************************************************************/

/*==================================================================================================
                                         INCLUDE FILES
 1) system and project includes
 2) needed interfaces from external units
 3) internal and external interfaces from this unit
==================================================================================================*/
#include "StbM.h"
#include "pit.h"



#define StbM_TaskTime     10000          /* mainfunction running time duration, base on us */
#define StbM_MAX_TIME_US  50000          /* maximum microsecond count  */
#define StbM_MAX_TIME_MS  50000          /* maximum milisecond count  */
#define StbM_MAX_TIME_S   50000          /* maximum second count  */


typedef enum {
	StbM_TB_us,       /* 10^-6 s;  1 microsecond */
	StbM_TB_ms,       /* 10^-3 s;  1 millisecond */
	StbM_TB_Second,   /* 10^0  s;  1 second      */
} StbM_SynchronizedTimeBaseType;


/*static uint16_t S_U16_us;*/       /* 10^-6;  1 microsecond */
static volatile uint16_t S_U16_ms;       	/* 10^-3;  1 millisecond */
static volatile uint16_t S_U16_s;        	/* 10^0;   1 second      */



void StbM_Init( void ) {
	S_U16_ms = 0;
	init_pit();
}

void StbM_MainFunction( void ) {
	if ( is_pit_triggered() )
		S_U16_ms++;
}


StbM_TickType StbM_GetNowTick(void) {
	return S_U16_ms;
}

uint16_t StbM_Elapsed( StbM_TickType start_ms ) {
	StbM_TickType now_ms;
	now_ms = S_U16_ms;

	if ( now_ms < start_ms )
		return ((uint16_t)0xffff - start_ms + now_ms);
	return (now_ms - start_ms);
}





