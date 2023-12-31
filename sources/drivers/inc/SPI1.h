/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : SPI1.h
**     Project     : BMU_113_BSW_ZB_20S
**     Processor   : SKEAZ128MLK4
**     Component   : Init_SPI
**     Version     : Component 01.001, Driver 01.01, CPU db: 3.00.000
**     Repository  : Kinetis
**     Compiler    : GNU C Compiler
**     Date/Time   : 2017-12-08, 19:16, # CodeGen: 20
**     Abstract    :
**          This file implements the SPI (SPI1) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
**     Settings    :
**          Component name                                 : SPI1
**          Device                                         : SPI1
**          Settings                                       : 
**            Clock gate                                   : Enabled
**            Clock setting                                : 
**              Baud rate prescaler                        : divide by 3
**              Baud rate divisor                          : divide by 8
**              SCK frequency                              : 1 MHz
**            Mode Select                                  : Master
**            Clock polarity                               : Active-high
**            Clock phase                                  : First edge
**            Data shift order                             : MSB first
**            Bidirectional mode                           : Disabled
**            Output enable in bidirect. mode              : no
**            SS pin function                              : General Purpose I/O
**            Match value                                  : 0
**            Stop in wait mode                            : Disabled
**          Pins                                           : 
**            SCK pin allocation                           : Enabled
**              SCK pin                                    : PTG4/KBI1_P20/FTM2_CH2/SPI1_SCK
**            MISO pin allocation                          : Enabled
**              MISO pin                                   : <Automatic>
**            MOSI pin allocation                          : Enabled
**              MOSI pin                                   : <Automatic>
**            SS pin allocation                            : Disabled
**          Interrupts/DMA                                 : 
**            SPI interrupt/DMA                            : 
**              Interrupt                                  : INT_SPI1
**              Interrupt request                          : <Automatic>
**              Interrupt priority                         : <Automatic>
**              ISR Name                                   : 
**              Receive and fault interrupt                : Disabled
**              Transmit interrupt                         : Disabled
**              Match interrupt                            : Disabled
**          Initialization                                 : 
**            Module enable                                : yes
**            Call Init method                             : yes
**            Utilize after reset values                   : default
**     Contents    :
**         Init - void SPI1_Init(void);
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
** @file SPI1.h
** @version 01.01
** @brief
**          This file implements the SPI (SPI1) module initialization
**          according to the Peripheral Initialization settings, and
**          defines interrupt service routines prototypes.
*/         
/*!
**  @addtogroup SPI1_module SPI1 module documentation
**  @{
*/         

#ifndef SPI1_H_
#define SPI1_H_

/* MODULE SPI1. */

#include "SPI1_Init.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialization method user name */
#define SPI1_Init SPI1_Init
/* PDD macros peripheral base address parameter */
#define SPI1_DEVICE SPI1_BASE_PTR

#ifdef __cplusplus
}
#endif

/* END SPI1. */
#endif /* #ifndef __SPI1_H_ */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/


