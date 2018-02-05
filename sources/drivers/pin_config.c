/*
 * pin_config.c
 *
 *  Created on: Oct 31, 2016
 *      Author: B52932
 */

#include "pin_config.h"

#include "SKEAZ1284.h"

void UART_pin_settings(enum UART_pinsel uart_pins){
	if((uart_pins == 0) | (uart_pins == 1)){
		SIM_PINSEL0 |= (uart_pins << SIM_PINSEL_UART0PS_SHIFT);
	} else if ((uart_pins == 2) | (uart_pins == 3)){
		SIM_PINSEL1 |= ((uart_pins % 2) << SIM_PINSEL1_UART1PS_SHIFT);
	} else if ((uart_pins == 4) | (uart_pins == 5)){
		SIM_PINSEL1 |= ((uart_pins % 2) << SIM_PINSEL1_UART2PS_SHIFT);
	}
}

void UART_pin_settings_reset(void){
	/* Set to reset state */
	SIM_PINSEL0 &= 0xFFFFFFFF & (~SIM_PINSEL_UART0PS_MASK);
	SIM_PINSEL1 &= 0xFFFFFFFF & (~(SIM_PINSEL1_UART1PS_MASK | SIM_PINSEL1_UART2PS_MASK ));
}

void SPI_pin_settings(enum SPI_pinsel spi_pins){

	if((spi_pins == 0) | (spi_pins == 1)){
		SIM_PINSEL0 |= (spi_pins << SIM_PINSEL_SPI0PS_SHIFT);
	} else if ((spi_pins == 2) | (spi_pins == 3)){
		SIM_PINSEL1 |= ((spi_pins % 2) << SIM_PINSEL1_SPI1PS_SHIFT);
	}
}

void SPI_pin_settings_reset(void){
	/* Set to reset state */
	SIM_PINSEL0 &= 0xFFFFFFFF & (~SIM_PINSEL_SPI0PS_MASK);
	SIM_PINSEL1 &= 0xFFFFFFFF & (~SIM_PINSEL1_SPI1PS_MASK);
}
