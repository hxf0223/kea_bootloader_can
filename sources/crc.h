#include <stdint.h>

#ifndef _CRC_H
#define _CRC_H

#ifdef CRC_16_ENABLE
uint16_t Crc_CalculateCRC16(const uint8_t* dataPtr, uint32_t len, uint16_t startVal);
#else
uint8_t cal_crc_table(uint8_t *ptr, uint32_t len, uint8_t startVal);
#endif

#endif
