#include <stdlib.h>
#include "SKEAZ1284.h"
#include "crc.h"
#include "memcpy2.h"
#include "rand.h"

static uint32_t g_seed32;

void rand_init() {
    uint8_t source[12];
    memcpy2(source, (uint8_t*)(&SIM->UUIDMH), 4);
    memcpy2(source+4, (uint8_t*)(&SIM->UUIDML), 4);
    memcpy2(source+8, (uint8_t*)(&SIM->UUIDL), 4);

    g_seed32 = crc32_le(UINT32_MAX, source, sizeof(source));
    srand(g_seed32);
}

uint16_t rand_get_uid16() {
    return (uint16_t)g_seed32;
}
