#include <stdint.h>
#include "ring_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t is_ring_buffer_full( const ring_buffer_t* rb ) {
	return (((rb->head+1) % RING_BUFFER_SIZE) == rb->tail);
}

static inline uint8_t is_ring_buffer_empty( const ring_buffer_t* rb ) {
	return ( rb->head == rb->tail );
}

static inline void ring_buffer_push_byte( ring_buffer_t* rb, uint8_t b ) {
	rb->buff[rb->head] = b;
	rb->head = (rb->head+1) % RING_BUFFER_SIZE;
}

static inline uint8_t ring_buffer_pop_byte ( ring_buffer_t* rb ) {
	const uint8_t b = rb->buff[rb->tail];
	rb->tail = (rb->tail+1) % RING_BUFFER_SIZE;
	return b;
}


void ring_buffer_init(ring_buffer_t* buff) {
	buff->head = buff->tail = 0;
}

uint16_t ring_buff_push(ring_buffer_t* rb, const void* buff, uint16_t sizeInBytes) {
	uint16_t num = 0;
	uint8_t* psrc = (uint8_t*)buff;

	while ( !is_ring_buffer_full(rb) && num < sizeInBytes ) {
		ring_buffer_push_byte(rb, psrc[num++]);
	}

	return num;
}

uint16_t ring_buffer_pop(ring_buffer_t* rb, void* buff, uint16_t capacityInBytes) {
	uint16_t num = 0;
	uint8_t* pdst = (uint8_t*)buff;

	while ( !is_ring_buffer_empty(rb) && num < capacityInBytes ) {
		pdst[num++] = ring_buffer_pop_byte(rb);
	}

	return num;
}


#ifdef __cplusplus
}
#endif
