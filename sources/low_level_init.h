#include "GPIOA_Init.h"
#include "GPIOB_Init.h"
#include "SPI0.h"
#include "SPI1.h"
#include "hsi_2515.h"
#include "can.h"
#include "flash.h"


#ifndef _LOW_LEVEL_INIT_H
#define _LOW_LEVEL_INIT_H


#define PORT_A  A
#define PORT_B  A
#define PORT_C  A
#define PORT_D  A

#define PORT_E	B
#define PORT_F	B
#define PORT_G	B
#define PORT_H	B

#define PORT_I	C
#define OUTPUT  1
#define INPUT	0

#define CONFIG_PIN_AS_GPIO(port,register_number,mode)    XCONFIG_PIN_AS_GPIO(port,register_number,mode)
#define XCONFIG_PIN_AS_GPIO(port,register_number,mode)   (mode == 0) ? (GPIO##port##_PDDR |= 0 << register_number) : (GPIO##port##_PDDR |= 1 << register_number)


#define LOCAL_OUTPUT_DATA(port,register_num,true_or_false)		XOUTPUT_DATA(port,register_num,true_or_false)
#define XOUTPUT_DATA(port,register_num,true_or_false)			(true_or_false == 1) ? (GPIO##port##_PDOR |= 1 << register_num) : (GPIO##port##_PDOR &= (~ (1<< register_num)) )

#define O_D_485_DE_RE_OUT		CONFIG_PIN_AS_GPIO(PORT_I,2,1); 	/* Configure LED 0 (PTC0) as an output */
#define O_D_485_DE_RE(X)		LOCAL_OUTPUT_DATA(PORT_I,2,X)		// RS485通信收发控制 低电平：接受模式，高电平：发送模式

#define O_D_CAN_Silent_OUT      CONFIG_PIN_AS_GPIO(PORT_B,13,1); 	/* Configure CAN Silent (PTB5) as an output */
#define O_D_CAN_Silent_Disable	LOCAL_OUTPUT_DATA(PORT_B,13,0); 	/* Configure CAN Silent (PTB5) as low */

#define O_D_LED1_DE_RE_OUT      CONFIG_PIN_AS_GPIO(PORT_G,16,1);
#define LED1_DATA(X)			LOCAL_OUTPUT_DATA(PORT_G,16,X)		// jason-171025

extern void low_level_init(void);


#endif

