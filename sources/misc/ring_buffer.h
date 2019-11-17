#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <stdint.h>

#define RING_BUFFER_SIZE		256U
typedef struct {
	uint16_t head, tail;	// next write/read position
	uint8_t buff[RING_BUFFER_SIZE];
} ring_buffer_t;

extern void ring_buffer_init(ring_buffer_t* buff);
extern uint16_t ring_buff_push(ring_buffer_t* rb, const void* buff, uint16_t sizeInBytes);
extern uint16_t ring_buffer_pop(ring_buffer_t* rb, void* buff, uint16_t capacityInBytes);

#endif
