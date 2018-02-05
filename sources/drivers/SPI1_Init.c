/** ###################################################################
 **     Filename    : SPI1_Init.c
 **     Processor   : SKEAZ128LK4
 **     Abstract    :
 **          This file implements the SPI1 module initialization
 **          according to the Peripheral Initialization settings, and
 **          defines interrupt service routines prototypes.
 **
 **     Contents    :
 **         Int   - void SPI1_Init(void);
 **
 **     Copyright : 1997 - 2015 Freescale Semiconductor, Inc.
 **     All Rights Reserved.
 **
 **     Redistribution and use in source and binary forms, with or without modification,
 **     are permitted provided that the following conditions are met:
 **
 **     o Redistributions of source code must retain the above copyright notice, this list
 **       of conditions and the following disclaimer.
 **
 **     o Redistributions in binary form must reproduce the above copyright notice, this
 **       list of conditions and the following disclaimer in the documentation and/or
 **       other materials provided with the distribution.
 **
 **     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 **       contributors may be used to endorse or promote products derived from this
 **       software without specific prior written permission.
 **
 **     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 **     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 **     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 **     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 **     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 **     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 **     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 **     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 **     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 **     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 **     http: www.freescale.com
 **     mail: support@freescale.com
 ** ###################################################################*/

/*!
 * @file SPI1_Init.c
 * @brief This file implements the SPI1 module initialization according to the
 *        Peripheral Initialization settings, and defines interrupt service
 *        routines prototypes.
 */


/* MODULE SPI1_Init. */

#include "SPI1_Init.h"
#include "SKEAZ1284.h"
#include "Init_Config.h"

#ifdef __cplusplus
extern "C" {
#endif

void SPI1_Init(void) {

  /* Register 'SPI1_C1' initialization */
  #ifdef SPI1_C1_VALUE_1
  SPI1_C1 = SPI1_C1_VALUE_1;
  #endif

  /* Register 'SPI1_C2' initialization */
  #if SPI1_C2_MASK
    #if SPI1_C2_MASK == 0xFF
  SPI1_C2 = SPI1_C2_VALUE;
    #elif SPI1_C2_MASK == SPI1_C2_VALUE
  SPI1_C2 |= SPI1_C2_VALUE;
    #elif SPI1_C2_VALUE == 0
  SPI1_C2 &= ~SPI1_C2_MASK;
    #else
  SPI1_C2 = (SPI1_C2 & (~SPI1_C2_MASK)) | SPI1_C2_VALUE;
    #endif
  #elif defined(SPI1_C2_VALUE)
  SPI1_C2 = SPI1_C2_VALUE;
  #endif

  /* Register 'SPI1_BR' initialization */
  #ifdef SPI1_BR_VALUE
  SPI1_BR = SPI1_BR_VALUE;
  #endif

  /* Register 'SPI1_M' initialization */
  #ifdef SPI1_M_VALUE
  SPI1_M = SPI1_M_VALUE;
  #endif

  /* Register 'SPI1_C1' initialization */
  #ifdef SPI1_C1_VALUE_2
  SPI1_C1 = SPI1_C1_VALUE_2;
  #endif
}

#ifdef __cplusplus
}
#endif

/* END SPI1_Init. */

/** ###################################################################
 **
 **     This file is a part of Processor Expert static initialization
 **     library for Freescale microcontrollers.
 **
 ** ################################################################### */
