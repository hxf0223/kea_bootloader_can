/*
 * pit.c
 *
 *  Created on: Nov 4, 2016
 *      Author: B52932
 */

#include "pit.h"
#include "SKEAZ1284.h"

void init_pit(void) {
	/* Enable bus clock to PIT */
	SIM->SCGC |= SIM_SCGC_PIT_MASK;

	/* Turn on PIT */
	PIT->MCR = 0x00;

	/* Configure Channel 1 to trigger as defined in pit.h */
	PIT->CHANNEL[0].LDVAL = TRIG_PERIOD;

	/* Enable timer */
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK;
}

void Reset_pit(void)
{
	/* Disable timer */
	PIT->CHANNEL[0].TCTRL = 0;

	/* Configure Channel 1 to trigger as defined in pit.h */
	PIT->CHANNEL[0].LDVAL = TRIG_PERIOD;

	/* Enable timer */
	PIT->CHANNEL[0].TCTRL = PIT_TCTRL_TEN_MASK;
}

void disable_pit(void) {

	/* Disable timer */
	PIT->CHANNEL[0].TCTRL = 0;

	/* Restore register */
	PIT->CHANNEL[0].LDVAL = 0x00;

	/* Turn off PIT */
	PIT->MCR = 0x06;

	/* Disable bus clock to PIT */
	SIM->SCGC &= (0xFFFFFFFF) & (~SIM_SCGC_PIT_MASK);
}

uint8_t is_pit_triggered(void) {
	uint8_t tmp;

	/* Read status */
	tmp = PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK;

	/* Clear flag */
	if ( tmp ) {
		PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK;
	}

	return tmp;
}

uint32_t pit_get_current_counter() {
	return PIT->CHANNEL[0].CVAL;
}

