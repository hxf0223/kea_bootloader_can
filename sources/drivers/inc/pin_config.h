/*
 * pin_config.h
 *
 *  Created on: Oct 31, 2016
 *      Author: B52932
 */

#ifndef SOURCES_DRIVERS_INC_PIN_CONFIG_H_
#define SOURCES_DRIVERS_INC_PIN_CONFIG_H_


enum UART_pinsel{
	PTB0_PTB1 = 0,
	PTA2_PTA3 = 1,
	PTC6_PTC7 = 2,
	PTF2_PTF3 = 3,
	PTD6_PTD7 = 4,
	PTI0_PTI1 = 5
};

enum SPI_pinsel{
	PTB2_3_4_5 = 0,
	PTE0_1_2_3 = 1,
	PTD0_1_2_3 = 2,
	PTG4_5_6_7 = 3
};

/* Prototypes */
void UART_pin_settings(enum UART_pinsel uart_pins);
void UART_pin_settings_reset(void);
void SPI_pin_settings(enum SPI_pinsel spi_pins);
void SPI_pin_settings_reset(void);

#endif /* SOURCES_DRIVERS_INC_PIN_CONFIG_H_ */
