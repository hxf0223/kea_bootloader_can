/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include "clock.h"
#include "StbM.h"
#include "flash.h"
#include "non_volatile_config.h"
#include "low_level_init.h"
#include "can_interface.h"
#include "bootloader_task.h"
#include "flash_task.h"


static void JumpToUserApplication(uint32_t userSP,  uint32_t userStartup);

int main(void)
{
	CANMsg can_msg;
	volatile uint32_t* app_entry_address = (uint32_t*)APP_ENTRY_ADDRESS;
	uint16_t flash_task_err, rec_num;
	uint8_t ft_ok_response_delay;	// indicate if need flash task boot response delay
	non_volatile_config_t nv_config;
	bootloader_task_init_data_t bl_task_init_data;
	bl_task_init_data.nv_config_ram = &nv_config;
	
	/*init_comm();
	O_D_485_DE_RE_OUT;*/

	low_level_init();
	StbM_Init();

	uint8_t nonvc_err = nonvc_get_config(&nv_config);
	if ( !nonvc_err ) {
		can_interface_init_t ci_init_data;
		ci_init_data.nonvc_can_tx_id = nv_config.config.can_id;
		can_interface_init(&ci_init_data);
	}

	bootloader_task_init(&bl_task_init_data);

boot_flash_pos:
	//flash_task_init();
	ft_ok_response_delay = 0;

    do {
    	StbM_MainFunction();
    	rec_num = can_receive(&can_msg);
		#if 0
    	if ( rec_num > 0 ) {
    		non_volatile_config_t temp;
			nonvc_get_config(&temp);
			temp.config.can_id = 0x120;
			temp.config.reboot_cmd = 0xff;
			uint8_t nonvc_err = nonvc_set_config(&temp);
			can_msg.m_data[0] = can_msg.m_data[0] + 0x20;
			can_msg.m_data[1] = nonvc_err;
			uint16_t config_size = sizeof(non_volatile_config_t);
			memset2(&can_msg.m_data, 0xff, 8);
			memcpy2(&(can_msg.m_data[2]), &config_size, 2);
			can_msg.m_dataLen = 8;
			can_transmit(&can_msg);
			continue;
    	}
		#endif

    	if ( rec_num > 0 && can_msg.m_ID == BOOTLOADER_RX_CAN_ID )
    		flash_task_push(can_msg.m_data, can_msg.m_dataLen);
    	flash_task_err = (uint16_t)flash_task_run();

    	if ( (flash_task_err & 0xf0) != 0 ) {
			if ( FLASH_TASK_RUN_ERR_FLASH_FINISH == flash_task_err )
    			ft_ok_response_delay = 1;
    		goto end_of_loop;
    	} else if ( flash_task_err & 0x0f ) {
			FLASH_EraseSector(APP_ENTRY_ADDRESS);
			goto end_of_loop;
		}
		
    } while( 1 );

end_of_loop:
    if ( (uint32_t)(*app_entry_address) == 0xffffffff ) {
    	goto boot_flash_pos;
    }

    if ( ft_ok_response_delay ) {
    	StbM_MainFunction();
    	uint16_t tm_resp = StbM_GetNowTick();
    	while ( StbM_Elapsed(tm_resp) < 2 ) {	// delay 2 ms
    		StbM_MainFunction();
    	}
    }


#if 0
	O_D_485_DE_RE(1);
	UART_PutChar(UART_PORT,0x42);
	UART_WaitTxComplete(UART_PORT);
	O_D_485_DE_RE(0);

	/* Check if boot start has been received or timeout occurred */
	do{
		uint8_t word_received = comm_status_rx();
		if(word_received){
			type = UART_GetChar(UART_PORT);
			if(type == 'S')
			{
			   boot_from_comm = 1;
			   comm_download_app();
			}
		}
		StbM_MainFunction();
	} while((StbM_Elapsed(0)<500) & (!boot_from_comm));
#endif

	/* Disable all systems and leave device as if coming out of reset */
	//disable_timeout();
	//disable_comm();
	reset_clock();

	//LED1_DATA(1);//jason-171025 for test

	/* Check if a valid application is loaded and jump to it */
	JumpToUserApplication((uint32_t)app_entry_address, (uint32_t)app_entry_address + 4);

    for (;;) {};
    return 0;
}

/**
 * Used to jump to the entry point of the user application
 * The Vector table of the user application must be located at 0x1000
 *
 * */
typedef void (*iapfun)(void);

void JumpToUserApplication( uint32_t userSP,  uint32_t userStartup )
{
	iapfun jump2app;

	/* Set up stack pointer */
	__asm("msr msp, r0");
	__asm("msr psp, r0");

	/* Relocate vector table */
	SCB->VTOR = (uint32_t)userSP;
	jump2app = (iapfun)*(volatile uint32_t*)(userStartup);

	/* Jump to application PC (r1) */
	//__asm("mov pc, r1");
	jump2app();
}

