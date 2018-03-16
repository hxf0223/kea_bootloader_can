/*
 * bootloader_task.h
 *
 *  Created on: 2018Äê2ÔÂ7ÈÕ
 *      Author: Administrator
 */

#ifndef SOURCES_BOOTLOADER_TASK_H_
#define SOURCES_BOOTLOADER_TASK_H_

#include <stdint.h>
#include "non_volatile_config.h"

typedef struct {
    non_volatile_config_t* nv_config_ram;
} bootloader_task_init_data_t;

void bootloader_task_init(void* p);

#endif /* SOURCES_BOOTLOADER_TASK_H_ */
