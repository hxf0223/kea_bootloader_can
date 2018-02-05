/*
 * clock.c
 *
 *  Created on: Nov 3, 2016
 *      Author: B52932
 */

#include "clock.h"

static __inline void OSC_Disable(void)
{
    OSC_CR &= ~(OSC_CR_OSCEN_MASK);
}

static __inline void ICS_SetBusDivider(uint8_t u8BusDivide)
{
    ICS_C2 = (ICS_C2 & ~(ICS_C2_BDIV_MASK)) | ICS_C2_BDIV(u8BusDivide);
}

/*****************************************************************************//*!
   *
   * @brief change clock from FEE mode to FEI mode.
   *
   * @param  pConfig     pointer to configuration.
   *
   * @return none
   *
   * @ Pass/ Fail criteria: none
   * @see   ICS_ConfigType
   *****************************************************************************/
void reset_clock(void)
{
	/* select internal reference for FLL */
    ICS_C1 =  ICS_C1 | (ICS_C1_IREFS_MASK);

	/* wait for the reference clock to be changed */
	while(!(ICS_S & ICS_S_IREFST_MASK));

	/* wait for FLL to lock */
	while(!(ICS_S & ICS_S_LOCK_MASK));

	/* clear Loss of lock sticky bit */
#if defined(MCU_SKEAZ1284)
	ICS_S |= ICS_S_LOLS_MASK;
#elif defined(MCU_SKEAZN642)
/* This line causes problems in KEAZN64. */
#else
/* Write the appropriate instruction for your device. */
#endif

	/* now FLL output clock is target frequency*/
	/* disable OSC */
     OSC_Disable();

	 /* Set core frequency*/
	ICS_SetBusDivider(2);

     /*Core frequency divide by 2 for Bus freq*/
#if defined(MCU_SKEAZ1284)
     SIM_CLKDIV|=SIM_CLKDIV_OUTDIV2_MASK;
#elif defined(MCU_SKEAZN642)
	SIM_BUSDIV |=SIM_BUSDIV_BUSDIV_MASK;
#else
/* Write the appropriate instruction for your device. */
#endif
}

/*****************************************************************************//*!
   *
   * @brief change clock from FBE mode to FEI mode.
   *
   * @param  pConfig     pointer to configuration.
   *
   * @return none
   *
   * @ Pass/ Fail criteria: none
   * @see   ICS_ConfigType
   *****************************************************************************/
void FBE_to_FEI(void)
{
	/* select internal reference clock */
	/* select the internal clock as clock source */

    ICS_C1 =  ICS_C1 | (ICS_C1_IREFS_MASK);
	ICS_C1 = (ICS_C1 & ~(ICS_C1_CLKS_MASK));

	/* wait for the clock source to be changed */
	while((ICS_S & ICS_S_CLKST_MASK));
	while(!(ICS_S & ICS_S_IREFST_MASK));

	/* now system clock source is internal clock*/
#if defined(MCU_SKEAZ1284)
	/* clear Loss of lock sticky bit */
	ICS_S |= ICS_S_LOLS_MASK;
#elif defined(MCU_SKEAZN642)
/* This line causes errors for KEAZN64. */
#else
/* Write the appropriate instruction for your device. */
#endif

    /* Disable OSC to save power
     */
    OSC_Disable();


	 /* Set core frequency, bus frequency = core freq /2*/
    ICS_SetBusDivider(2);

    /*Core frequency divide by 2 for Bus freq*/
#if defined(MCU_SKEAZ1284)
    SIM_CLKDIV|=SIM_CLKDIV_OUTDIV2_MASK;
#elif defined(MCU_SKEAZN642)
	SIM_BUSDIV |=SIM_BUSDIV_BUSDIV_MASK;
#else
/* Write the appropriate instruction for your device. */
#endif
}
