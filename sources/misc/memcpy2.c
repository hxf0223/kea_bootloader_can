/*
 * memcpy.c
 *
 *  Created on: 2018Äê2ÔÂ2ÈÕ
 *      Author: Administrator
 */

#include <stdint.h>
#include "memcpy2.h"

void memcpy2(void* dest, const void* src, unsigned int len) {
	uint8_t* pdest8 = (uint8_t*)dest;
	const uint8_t* psrc8 = (const uint8_t*)src;

	for ( unsigned int i = 0; i < len; i++ ) {
		pdest8[i] = psrc8[i];
	}
}

void memset2(void* dest, uint8_t fill, unsigned int len) {
	uint8_t* pdest8 = (uint8_t*)dest;
	for (unsigned int i = 0; i < len; i++ ) {
		pdest8[i] = fill;
	}
}
