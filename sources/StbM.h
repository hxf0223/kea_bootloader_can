#ifndef __STBM_H__
#define __STBM_H__

/*==================================================================================================
*                                          INCLUDE FILES
*  1) system and project includes
*  2) needed interfaces from external units
*  3) internal and external interfaces from this unit
==================================================================================================*/
//#include "typedefs.h"
#include <stdint.h>


typedef uint16_t StbM_TickType;


extern void StbM_Init(void);
extern void StbM_MainFunction(void);

extern StbM_TickType StbM_GetNowTick(void);
extern uint16_t StbM_Elapsed(StbM_TickType start_ms);


#endif
