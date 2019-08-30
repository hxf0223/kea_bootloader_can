#include "low_level_init.h"
#include "ring_buffer.h"

#ifndef _FLASH_TASK_H
#define _FLASH_TASK_H

//#define UINT16_MAX		((uint16_t)-1)
//#define UINT32_MAX		((uint32_t)-1)

#define APP_ENTRY_ADDRESS	        0x4000U
#define FLASH_TASK_TIMEOUT_WAIT_START	    ((uint16_t)300)     /* start flash task wait timeout value */
#define FLASH_TASK_TIMEOUT_RUN			    ((uint16_t)200)     /* state machine step timeout value */

#define FLASH_TASK_RUN_ERR_OK					0x00U
#define FLASH_TASK_RUN_ERR_TIMEOUT				0x01U
#define FLASH_TASK_RUN_ERR_FAIL					0x0FU
#define FLASH_TASK_RUN_ERR_START_WAIT_TIMEOUT	0x10U
#define FLASH_TASK_RUN_ERR_FLASH_FINISH			0x20U

typedef struct {
	ring_buffer_t* rb_rx;
	ring_buffer_t* rb_tx;
} flash_task_init_data_t;


void flash_task_init(void* p);
void flash_task_start();

uint16_t flash_task_push(const void* buff, uint16_t sizeInBytes);
uint32_t flash_task_run();

#endif
