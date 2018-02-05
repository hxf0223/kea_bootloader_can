/*
 * common.h
 *
 *  Created on: Nov 2, 2016
 *      Author: B52932
 */

#ifndef SOURCES_DRIVERS_INC_COMMON_H_
#define SOURCES_DRIVERS_INC_COMMON_H_

#include "SKEAZ1284.h"
#include <stdint.h>

#define MAX_ADDRESS_BP	4
#define MAX_DATA_BP		32
#define MAX_PHSIZE_BP	37

typedef union
{
	uint8_t Byte[MAX_PHSIZE_BP]; 												/* Byte level access to the Phrase */
	struct
	{
		char PhraseType;														/* Type of received record (e.g. S0, S1, S5, S9...) */
		uint8_t PhraseSize; 													/* Phrase size (address + data + checksum) */
		uint8_t PhraseAddress[MAX_ADDRESS_BP]__attribute__ ((aligned (32))); 	/* Address, depending on the type of record it might vary */
		uint8_t PhraseData[MAX_DATA_BP]__attribute__ ((aligned (32))); 			/* Maximum 32 data bytes */
		uint8_t PhraseCRC;														/* Checksum of size + address + data */
	}F;
}BootPhraseStruct;


/* Little Endian to Big Endian macro */
#define LE2BE_32(c)	(((c & 0xFF) << 24) | ((c & 0xFF00) << 8) | ((c & 0xFF0000) >> 8) | ((c & 0xFF000000) >> 24))

// 1．芯片寄存器映射文件及处理器内核属性文件
#define  SYSTEM_CLK_KHZ   (DEFAULT_SYSTEM_CLOCK/1000)// 芯片系统时钟频率(KHz)
#define  CORE_CLK_KHZ     SYSTEM_CLK_KHZ           // 芯片内核时钟频率(KHz)
#define  BUS_CLK_KHZ      (SYSTEM_CLK_KHZ/2)         // 芯片总线时钟频率(KHz)

// 2．定义开关总中断
#define ENABLE_INTERRUPTS   __enable_irq   // 开总中断
#define DISABLE_INTERRUPTS  __disable_irq  // 关总中断

// 3．位操作宏函数（置位、清位、获得寄存器一位的状态）
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    // 置寄存器的一位
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  // 清寄存器的一位
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  // 获得寄存器一位的状态

// 4．重定义基本数据类型（类型别名宏定义）
typedef unsigned char uint_8;   // 无符号8位数，字节
typedef unsigned short int uint_16;  // 无符号16位数，字
typedef unsigned long int uint_32;  // 无符号32位数，长字
typedef char int_8;    // 有符号8位数
typedef short int int_16;   // 有符号16位数
typedef int int_32;   // 有符号32位数
// 不优化类型
typedef volatile uint_8 vuint_8;  // 不优化无符号8位数，字节
typedef volatile uint_16 vuint_16; // 不优化无符号16位数，字
typedef volatile uint_32 vuint_32; // 不优化无符号32位数，长字
typedef volatile int_8 vint_8;   // 不优化有符号8位数
typedef volatile int_16 vint_16;  // 不优化有符号16位数
typedef volatile int_32 vint_32;  // 不优化有符号32位数



#endif /* SOURCES_DRIVERS_INC_COMMON_H_ */
