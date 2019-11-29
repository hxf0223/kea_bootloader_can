#include "stbm.h"
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



void stbm_init( void ) {
	S_U16_ms = 0;
	init_pit();
}

void stbm_main( void ) {
	if ( is_pit_triggered() )
		S_U16_ms++;
}


StbM_TickType stbm_get_tick(void) {
	return S_U16_ms;
}

uint16_t stbm_elapsed( StbM_TickType start_ms ) {
	StbM_TickType now_ms;
	now_ms = S_U16_ms;

	if ( now_ms < start_ms )
		return ((uint16_t)0xffff - start_ms + now_ms);
	return (now_ms - start_ms);
}





