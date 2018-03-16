/*
 * non_volatile_config.c
 *
 *  Created on: 2018Äê2ÔÂ5ÈÕ
 *      Author: Administrator
 */

#include "crc.h"
#include "memcpy2.h"
#include "non_volatile_config.h"

#define CONFIG_FLASH_SECTOR_ADDR    0x0001F200UL
#define sector_config               ((non_volatile_config_t*)CONFIG_FLASH_SECTOR_ADDR)

static inline uint8_t nonvc_calc_config_data_crc8(const non_volatile_config_t* p) {
    return cal_crc_table((uint8_t*)p + 1, sizeof(non_volatile_config_t) - 1, 0xff);
}

uint8_t nonvc_get_config(non_volatile_config_t* pConfig) {
    uint8_t calc_crc8 = nonvc_calc_config_data_crc8(sector_config);
    if ( calc_crc8 != sector_config->config.crc8 ) {
        return 1;
    }

    memcpy2(pConfig, sector_config, sizeof(non_volatile_config_t));   
    return 0;
}

uint8_t nonvc_set_config(const non_volatile_config_t* pConfig) {
    non_volatile_config_t config;
    memcpy2(&config, pConfig, sizeof(non_volatile_config_t));
    config.config.crc8 = nonvc_calc_config_data_crc8(&config);

    uint16_t err16 = FLASH_EraseSector(CONFIG_FLASH_SECTOR_ADDR);
    if ( 0 != err16 ) return 1;

    err16 = FLASH_Program(CONFIG_FLASH_SECTOR_ADDR, (uint8_t*)(&config), FLASH_SECTOR_SIZE);
    if ( 0 != err16 ) return 1;

    return 0;
}


