/*
 * non_volatile_config.h
 *
 *  Created on: 2018Äê2ÔÂ5ÈÕ
 *      Author: Administrator
 */

#ifndef SOURCES_NON_VOLATILE_CONFIG_NON_VOLATILE_CONFIG_H_
#define SOURCES_NON_VOLATILE_CONFIG_NON_VOLATILE_CONFIG_H_

#include <stdint.h>
#include "flash.h"

#pragma pack(push, 1)
typedef struct {
    union {
        uint8_t flash[FLASH_SECTOR_SIZE];
        struct {
            uint8_t crc8;
            uint8_t reboot_cmd;
            uint32_t can_id;
        } config;
    };
} non_volatile_config_t;
#pragma pack(pop)

//void nonvc_init();
uint8_t nonvc_get_config(non_volatile_config_t* pConfig);
uint8_t nonvc_set_config(const non_volatile_config_t* pConfig);

#endif
