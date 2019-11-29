#ifndef __STBM_H__
#define __STBM_H__

#include <stdint.h>

typedef uint16_t StbM_TickType;

extern void stbm_init(void);
extern void stbm_main(void);

extern StbM_TickType stbm_get_tick(void);
extern uint16_t stbm_elapsed(StbM_TickType start_ms);


#endif
