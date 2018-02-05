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

// 1��оƬ�Ĵ���ӳ���ļ����������ں������ļ�
#define  SYSTEM_CLK_KHZ   (DEFAULT_SYSTEM_CLOCK/1000)// оƬϵͳʱ��Ƶ��(KHz)
#define  CORE_CLK_KHZ     SYSTEM_CLK_KHZ           // оƬ�ں�ʱ��Ƶ��(KHz)
#define  BUS_CLK_KHZ      (SYSTEM_CLK_KHZ/2)         // оƬ����ʱ��Ƶ��(KHz)

// 2�����忪�����ж�
#define ENABLE_INTERRUPTS   __enable_irq   // �����ж�
#define DISABLE_INTERRUPTS  __disable_irq  // �����ж�

// 3��λ�����꺯������λ����λ����üĴ���һλ��״̬��
#define BSET(bit,Register)  ((Register)|= (1<<(bit)))    // �üĴ�����һλ
#define BCLR(bit,Register)  ((Register) &= ~(1<<(bit)))  // ��Ĵ�����һλ
#define BGET(bit,Register)  (((Register) >> (bit)) & 1)  // ��üĴ���һλ��״̬

// 4���ض�������������ͣ����ͱ����궨�壩
typedef unsigned char uint_8;   // �޷���8λ�����ֽ�
typedef unsigned short int uint_16;  // �޷���16λ������
typedef unsigned long int uint_32;  // �޷���32λ��������
typedef char int_8;    // �з���8λ��
typedef short int int_16;   // �з���16λ��
typedef int int_32;   // �з���32λ��
// ���Ż�����
typedef volatile uint_8 vuint_8;  // ���Ż��޷���8λ�����ֽ�
typedef volatile uint_16 vuint_16; // ���Ż��޷���16λ������
typedef volatile uint_32 vuint_32; // ���Ż��޷���32λ��������
typedef volatile int_8 vint_8;   // ���Ż��з���8λ��
typedef volatile int_16 vint_16;  // ���Ż��з���16λ��
typedef volatile int_32 vint_32;  // ���Ż��з���32λ��



#endif /* SOURCES_DRIVERS_INC_COMMON_H_ */
