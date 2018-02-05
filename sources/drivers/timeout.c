/*
 * timeout.c
 *
 *  Created on: Oct 31, 2016
 *      Author: B52932
 */

#include "timeout.h"

static uint32_t timeout_count = 0;


void init_timeout(void){

	/* Initialize PIT */
	init_pit();
}

uint8_t timeReset(void){

	timeout_count = 0;
	Reset_pit();
}

uint8_t timeout(uint32_t timeout_val){
	uint8_t tmp_stat;

	/* Check if PIT has triggered */
	tmp_stat = is_pit_triggered();

	/* Increment timeout variable */
	if(tmp_stat){
		++timeout_count;
	}

	/* Check if timeout occurred */
	if(timeout_count < timeout_val){
		return 0;
	}

	/* Restore timeout and signal it */
	timeout_count = 0;
	return 1;
}

void disable_timeout(void){

	/* Reset PIT */
	disable_pit();
}
