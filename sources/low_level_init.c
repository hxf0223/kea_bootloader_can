#include "low_level_init.h"

#ifdef __cplusplus
extern "C" {
#endif


void low_level_init(void) {
	//SIM_SOPT0 &= ~((uint32_t)SIM_SOPT0_NMIE_MASK); /* Disable NMI pin */

	/* SIM_PINSEL0: SPI0PS=1 */
	SIM_PINSEL0 |= SIM_PINSEL_SPI0PS_MASK;
	/* SIM_PINSEL1: SPI1PS=1 */
	SIM_PINSEL1 |= SIM_PINSEL1_SPI1PS_MASK;
	/* SIM_SCGC: SPI1=1,SPI0=1 */
	SIM_SCGC |= (SIM_SCGC_SPI1_MASK | SIM_SCGC_SPI0_MASK);

	/* enable the PTE0, PTE1,PTE2,PTE3 pull up */
	//PORT_PUE1 |= PORT_PUE1_PTEPE0_MASK | PORT_PUE1_PTEPE1_MASK | PORT_PUE1_PTEPE2_MASK | PORT_PUE1_PTEPE3_MASK;

	GPIOA_Init();
	GPIOB_Init();
	SPI0_Init();
	SPI1_Init();
	hsi_2515_init(D_CAN_CHANNEL, can_baud_rate_250k);
	hsi_2515_init(A_CAN_CHANNEL, can_baud_rate_250k);

	mscan_init(1, 0, 0, can_baud_rate_250k);
	O_D_CAN_Silent_OUT;
	O_D_CAN_Silent_Disable;

	FLASH_Init((DEFAULT_SYSTEM_CLOCK / 2));
}



#ifdef __cplusplus
}
#endif

