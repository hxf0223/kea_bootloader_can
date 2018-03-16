#include <stdint.h>

#ifndef _CRC_H
#define _CRC_H

#ifdef CRC_16_ENABLE
uint16_t Crc_CalculateCRC16(const uint8_t* dataPtr, uint32_t len, uint16_t startVal);
#else
//#define CRC8_WITH_TABLE
uint8_t cal_crc_table(uint8_t *ptr, uint32_t len, uint8_t startVal);
#endif


/*
 * There are multiple 16-bit CRC polynomials in common use, but this is
 * *the* standard CRC-32 polynomial, first popularized by Ethernet.
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
 */
#define CRCPOLY_LE 0xedb88320
#define CRCPOLY_BE 0x04c11db7

/*
 * This is the CRC32c polynomial, as outlined by Castagnoli.
 * x^32+x^28+x^27+x^26+x^25+x^23+x^22+x^20+x^19+x^18+x^14+x^13+x^11+x^10+x^9+
 * x^8+x^6+x^0
 */
#define CRC32C_POLY_LE 0x82F63B78

/* https://github.com/wlshiu/crc32/blob/master/main.c */
uint32_t crc32_le(uint32_t crc, unsigned char const *p, uint32_t len);
uint32_t crc32c_le(uint32_t crc, unsigned char const *p, uint32_t len);


#endif
