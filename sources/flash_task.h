#include "low_level_init.h"

#ifndef _FLASH_TASK_H
#define _FLASH_TASK_H

//#define UINT16_MAX		((uint16_t)-1)
//#define UINT32_MAX		((uint32_t)-1)

#define APP_ENTRY_ADDRESS	0x4000U

#define FLASH_TASK_RUN_ERR_OK					0x00U
#define FLASH_TASK_RUN_ERR_TIMEOUT				0x01U
#define FLASH_TASK_RUN_ERR_FAIL					0x0FU
#define FLASH_TASK_RUN_ERR_START_WAIT_TIMEOUT	0x10U
#define FLASH_TASK_RUN_ERR_FLASH_FINISH			0x20U


void flash_task_init(void);
uint16_t flash_task_push(const void* buff, uint16_t sizeInBytes);
uint8_t flash_task_run();

#endif
