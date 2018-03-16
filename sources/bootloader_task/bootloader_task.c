/*
 * bootloader_task.c
 *
 *  Created on: 2018Äê2ÔÂ7ÈÕ
 *      Author: Administrator
 */

#include "ring_buffer.h"
#include "flash_task.h"
#include "bootloader_task.h"

static ring_buffer_t g_rb_rx;
static bootloader_task_init_data_t g_init_data = {NULL};

void bootloader_task_init(void* p) {
	g_init_data = *((bootloader_task_init_data_t*)p);
	ring_buffer_init(&g_rb_rx);

	flash_task_init_data_t ft_init_data;
	ft_init_data.rb_rx = &g_rb_rx;
	ft_init_data.rb_tx = NULL;
}


