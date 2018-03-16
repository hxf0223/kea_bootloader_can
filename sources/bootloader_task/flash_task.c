#include <stdint.h>
#include <stdlib.h>
#include "typedef.h"
#include "crc.h"
#include "memcpy2.h"
#include "ring_buffer.h"
#include "can_interface.h"
#include "StbM.h"
#include "flash.h"
#include "bootloader_command_def.h"
#include "flash_task.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	fp_state_wait_start = 0,
	fp_state_query_capacity,
	fp_state_command_frame_start,
	fp_state_data_transfer,
	fp_state_command_frame_finish,
	fp_state_command_verify,
	fp_state_command_boot,
	fp_state_error
} fp_state_e;



#define FP_BUFFER_SIZE		256U
typedef struct {
	uint8_t buffer[FP_BUFFER_SIZE] __attribute__ ((aligned (32)));
	uint32_t address __attribute__ ((aligned (32)));
	uint16_t receive_size;
	uint16_t frame_size;
	uint8_t  data_err_code;
} flash_frame_progress_t;

typedef struct {
	flash_frame_progress_t fp;
	uint32_t app_entry_address;		// app's entry address
	uint32_t next_erase_address;
	uint32_t file_size;				// actually whole download data size
	uint32_t file_size_accu;		// actually whole download data size accumulator

	// first sector buffer, flash at last
	uint8_t first_sector_buff[FLASH_SECTOR_SIZE];

	fp_state_e next_fp_state;
} flash_progress_t;


static void flash_frame_progress_reset(flash_frame_progress_t* p) {
	memset2(p, 0, sizeof(flash_frame_progress_t));
	p->address = UINT32_MAX;
}

static void flash_progress_reset(flash_progress_t* p) {
	flash_frame_progress_reset(&p->fp);
	memset2(p->first_sector_buff, 0xff, sizeof(p->first_sector_buff));
	p->app_entry_address = UINT32_MAX;
	p->next_erase_address = UINT32_MAX;
	p->file_size = 0;
	p->file_size_accu = 0;

	//p->next_fp_state = fp_state_command_wait_start;
}

static uint8_t is_erase_needed(flash_progress_t* p) {
	if ( p->next_erase_address == UINT32_MAX ||
			(p->fp.address + p->fp.frame_size) > (p->next_erase_address) ) {
		return 1;
	}

	return 0;
}

//#define DEBUG_NO_ERASE

static uint8_t do_erase_sector(flash_progress_t* p) {
	if ( p->next_erase_address == UINT32_MAX ) {
		p->next_erase_address = p->app_entry_address;
	}

#ifndef DEBUG_NO_ERASE
	if ( FLASH_ERR_SUCCESS == FLASH_EraseSector(p->next_erase_address) ) {
		p->next_erase_address += FLASH_SECTOR_SIZE;
		return 1;
	}

	return 0;
#else
	return 1;
#endif
}

static uint8_t do_flash2(uint32_t address, uint8_t* buffer, uint16_t size) {
	if ( address == UINT32_MAX ) {
		return 0;
	}

#ifndef DEBUG_NO_ERASE
	uint16_t err = FLASH_Program(address, buffer, size);
	return (err == FLASH_ERR_SUCCESS);
#else
	return 1;
#endif
}

static inline uint8_t do_flash(flash_progress_t* p) {
	return do_flash2(p->fp.address, p->fp.buffer, p->fp.frame_size);
}


typedef uint8_t (*command_process_func_t)(uint8_t*, uint16_t, void*);
typedef uint8_t (*command_error_check_func_t)(uint8_t, uint8_t*, uint16_t);

#pragma pack(push, 1)
typedef struct {
	fp_state_e state;
	/*uint8_t command;*/
	uint16_t command_len;
	command_process_func_t func;
	void* func_param;
	command_error_check_func_t err_check_func;
} command_process_t;
#pragma pack(pop)

static void ok_response(uint8_t command, uint8_t* data, uint8_t dataLength) {
	uint16_t offset = 0;
	uint8_t crc8;
	CANMsg msg;

	init_can_frame(&msg);
	msg.m_data[offset++] = command + FP_RESPONSE_ADD_VALUE;
	for (uint8_t i = 0; i < dataLength && data; i++ ) {
		msg.m_data[offset++] = data[i];
	}

	crc8 = cal_crc_table(msg.m_data, offset, 0xff);
	msg.m_data[offset++] = crc8;

	msg.m_dataLen = offset;
	can_transmit(&msg);
}

static void error_response(uint8_t command, uint8_t errCode) {
	uint16_t offset = 0;
	uint8_t crc8;
	CANMsg msg;

	init_can_frame(&msg);
	msg.m_data[offset++] = FP_ERROR_RESPONSE;
	msg.m_data[offset++] = command + FP_RESPONSE_ADD_VALUE;
	msg.m_data[offset++] = errCode;
	crc8 = cal_crc_table(msg.m_data, offset, 0xff);
	msg.m_data[offset++] = crc8;

	msg.m_dataLen = offset;
	can_transmit(&msg);
}

static uint8_t common_error_response( uint8_t command, uint8_t* pData, uint16_t len ) {
	if ( !len || !pData ) {
		error_response(command, FP_RESPONSE_ERR_CODE_UNKNOWN);
		return 0;
	}

	if ( pData[0] != command ) {
		error_response(command, FP_RESPONSE_ERR_CODE_STATE);
		return 0;
	}

	uint8_t crc8 = cal_crc_table(pData, len-1, 0xff);
	if ( crc8 != pData[len-1] ) {
		error_response(command, FP_RESPONSE_ERR_CODE_CRC);
		return 0;
	}

	return 1;
}


// client query in 6 bytes, response in 2 bytes
static uint8_t command_process_start( uint8_t* pData, uint16_t len, void* param ) {
	flash_progress_t* pfp = (flash_progress_t*)param;
	uint8_t command = FP_COMMAND_START;
	uint16_t r_offset;

	if ( !pData || !len ) {
		pfp->next_fp_state = fp_state_wait_start;
		return 0;
	}

	/* parse */
	r_offset = 1;
	flash_progress_reset(pfp);
	pfp->file_size = get_u32_from_stream(pData, r_offset);
	pfp->app_entry_address = APP_ENTRY_ADDRESS;
	pfp->next_fp_state = fp_state_query_capacity;

	ok_response(command, NULL, 0);
	return 1;
}

// client query in 2 bytes, response in 4 bytes
static uint8_t command_process_query_capacity( uint8_t* pData, uint16_t len, void* param ) {
	flash_progress_t* pfp = (flash_progress_t*)param;
	uint8_t send_data[2];

	uint16_t capacity = FP_BUFFER_SIZE;
	pfp->next_fp_state = fp_state_command_frame_start;
	put_u16_to_stream(capacity, send_data, 0);
	ok_response(FP_QUERY_CAPACITY, send_data, 2);

	return 1;
}

// client query in 8 bytes, response in 2 bytes
static uint8_t command_process_frame_start( uint8_t* pData, uint16_t len, void* param ) {
	flash_progress_t* pfp = (flash_progress_t*)param;
	uint8_t command = FP_COMMAND_FRAME_START;
	uint8_t erase_flag[1] = {0};
	uint16_t r_offset;

	/* parse */
	r_offset = 1;
	pfp->fp.address = get_u32_from_stream(pData, r_offset); r_offset += 4;
	pfp->fp.frame_size = get_u16_from_stream(pData, r_offset); r_offset += 2;
	pfp->fp.receive_size = 0;

	if ( is_erase_needed(pfp) ) {
		if (!do_erase_sector(pfp)) {
			error_response(command, FP_RESPONSE_ERR_CODE_ERASE_FAIL);
			return 0;
		}

		erase_flag[0] = 1;
	}

	/* response */
	if ( pfp->fp.frame_size > FP_BUFFER_SIZE ) {
		error_response(command, FP_RESPONSE_ERR_CODE_CAP_LIMIT);
		return 0;
	}

	if ( pfp->fp.address < APP_ENTRY_ADDRESS ) {
		error_response(command, FP_RESPONSE_ERR_CODE_ADDR_INVALID);
		return 0;
	}

	pfp->next_fp_state = fp_state_data_transfer;
	ok_response(command, erase_flag, 1);

	return 1;
}

// client query in 7 bytes, response in 2 bytes
static uint8_t command_process_frame_finish( uint8_t* pData, uint16_t len, void* param ) {
	flash_progress_t* pfp = (flash_progress_t*)param;
	uint8_t command = FP_COMMAND_FRAME_FINISH;
	uint8_t calc_crc8, client_frame_crc8;
	uint32_t address;
	uint16_t r_offset;

	/* parse */
	r_offset = 1;
	address = get_u32_from_stream(pData, r_offset); r_offset += 4;
	client_frame_crc8 = pData[r_offset++];

	/* response */
	if ( pfp->fp.data_err_code ) {
		uint8_t data_err_code = pfp->fp.data_err_code;
		error_response(command, data_err_code);
		return 0;
	}

	if ( address != pfp->fp.address || pfp->fp.frame_size != pfp->fp.receive_size ) {
		error_response(command, FP_RESPONSE_ERR_CODE_FSF_SYNC);
		return 0;
	}

	calc_crc8 = cal_crc_table(pfp->fp.buffer, pfp->fp.frame_size, 0xff);
	if ( calc_crc8 != client_frame_crc8 ) {
		error_response(command, FP_RESPONSE_ERR_CODE_DATA_CRC);
		return 0;
	}

	if ( (pfp->fp.address+pfp->fp.frame_size) > (pfp->app_entry_address + FLASH_SECTOR_SIZE) ) {
		if (!do_flash(pfp)) {
			error_response(command, FP_RESPONSE_ERR_CODE_FLASH_FAIL);
			return 0;
		}
	} else {
		uint16_t ff_offset = pfp->fp.address - APP_ENTRY_ADDRESS;
		memcpy2(pfp->first_sector_buff + ff_offset, pfp->fp.buffer, pfp->fp.frame_size);
	}

	if ( pfp->file_size_accu == pfp->file_size ) {
		pfp->next_fp_state = fp_state_command_verify;
	} else {
		pfp->next_fp_state = fp_state_command_frame_start;
	}

	ok_response(command, NULL, 0);
	return 1;
}

// data transfer, no data format, no response
static uint8_t command_process_data_transfer(uint8_t* pData, uint16_t len, void* param) {
	flash_progress_t* pfp = (flash_progress_t*)param;
	if ( pfp->fp.address == UINT32_MAX )
		return 0;

	if ( (pfp->fp.receive_size+len) <= pfp->fp.frame_size ) {
		uint16_t pos = pfp->fp.receive_size;
		for ( uint16_t i = 0; i < len; i++ ) {
			pfp->fp.buffer[pos++] = pData[i];
		}

		pfp->fp.receive_size = pos;
		pfp->file_size_accu += len;

		if ( pfp->fp.receive_size == pfp->fp.frame_size ) {
			pfp->next_fp_state = fp_state_command_frame_finish;
		}
	} else {
		//flash_progress_reset(pfp);
		pfp->fp.data_err_code = FP_RESPONSE_ERR_CODE_DATA_OVERFLOW;
		pfp->next_fp_state = fp_state_command_frame_finish;
	}

	return 1;
}

// client query in 3 bytes, response in 3 bytes
static uint8_t command_process_verify(uint8_t* pData, uint16_t len, void* param) {
	flash_progress_t* pfp = (flash_progress_t*)param;
	uint8_t command = FP_COMMAND_VERIFY;

	if ( !do_flash2(pfp->app_entry_address, pfp->first_sector_buff, FLASH_SECTOR_SIZE) ) {
		error_response(command, FP_RESPONSE_ERR_CODE_FLASH_FAIL);
		return 0;
	}

	uint8_t* app_start_addr = (uint8_t*)(pfp->app_entry_address);
	uint8_t crc8 = cal_crc_table(app_start_addr, pfp->file_size, 0xff);

	if ( crc8 != pData[1] ) {
		error_response(command, FP_RESPONSE_ERR_VERIFY_CRC_NOT_MATCH);
		return 0;
	}

	pfp->next_fp_state = fp_state_command_boot;
	ok_response(command, &crc8, 1);

	return 1;
}

// client query in 2 bytes, response in 2 bytes
static uint8_t command_process_boot(uint8_t* pData, uint16_t len, void* param) {
	flash_progress_t* pfp = (flash_progress_t*)param;

	pfp->next_fp_state = fp_state_wait_start;
	ok_response(FP_COMMAND_BOOT, NULL, 0);

	return 1;
}



static flash_progress_t g_fp;
static fp_state_e g_fp_state;
static uint16_t g_tm_wait_start, g_tm_ellapsed;
static flash_task_init_data_t g_init_data = { NULL, NULL };

static const command_process_t g_command_process_list[] = {	// data driven
	{fp_state_wait_start, 			/*FP_COMMAND_START,*/ 		6, command_process_start, 			&g_fp, common_error_response},
	{fp_state_query_capacity, 		/*FP_QUERY_CAPACITY,*/ 		2, command_process_query_capacity, 	&g_fp, common_error_response},
	{fp_state_command_frame_start, 	/*FP_COMMAND_FRAME_START,*/ 8, command_process_frame_start, 	&g_fp, common_error_response},
	{fp_state_command_frame_finish, /*FP_COMMAND_FRAME_FINISH,*/7, command_process_frame_finish, 	&g_fp, common_error_response},
	{fp_state_data_transfer, 		/*FP_COMMAND_INVALID,*/ 	8, command_process_data_transfer, 	&g_fp, NULL},
	{fp_state_command_verify, 		/*FP_COMMAND_VERIFY,*/ 		3, command_process_verify, 			&g_fp, common_error_response},
	{fp_state_command_boot, 		/*FP_COMMAND_BOOT,*/ 		2, command_process_boot, 			&g_fp, common_error_response}
};

static command_process_t* find_command_process(fp_state_e stt) {
	for ( uint8_t i = 0; i < (sizeof(g_command_process_list) / sizeof(command_process_t)); i++ ) {
		if ( g_command_process_list[i].state == stt ) {
			return (command_process_t*)(&g_command_process_list[i]);
		}
	}

	return NULL;
}

/* start flash task */
void flash_task_init(void* p) {
	flash_task_init_data_t* pinit = (flash_task_init_data_t*)p;
	g_init_data = *pinit;

	flash_task_start();
}

void flash_task_start() {
	flash_progress_reset(&g_fp);
	g_fp.next_fp_state = fp_state_wait_start;
	g_fp_state = fp_state_wait_start;

	g_tm_wait_start = StbM_GetNowTick();
	g_tm_ellapsed = 0;
}

uint16_t flash_task_push(const void* buff, uint16_t sizeInBytes) {
	return ring_buff_push(g_init_data.rb_rx, buff, sizeInBytes);
}

static void flash_progress_reset_all() {
	flash_progress_reset(&g_fp);
	g_fp.next_fp_state = fp_state_wait_start;
	g_fp_state = fp_state_wait_start;
}

uint32_t flash_task_run() {
	uint8_t cmd_err, buff[CAN_FRAME_LEN];
	uint16_t receive_num = ring_buffer_pop(g_init_data.rb_rx, buff, CAN_FRAME_LEN);
	command_process_t* cmd_process = find_command_process(g_fp_state);

	if ( receive_num == 0 ) {
		g_tm_ellapsed = StbM_Elapsed(g_tm_wait_start);
		uint32_t return_err = (g_tm_ellapsed << 16);
		
		if ( g_fp_state == fp_state_wait_start ) {
			cmd_process->func(buff, receive_num, &g_fp);
			if ( g_tm_ellapsed > FLASH_TASK_TIMEOUT_WAIT_START )
				return_err |= FLASH_TASK_RUN_ERR_START_WAIT_TIMEOUT;		
		} else {
			if ( g_tm_ellapsed > FLASH_TASK_TIMEOUT_RUN )
				return_err |= FLASH_TASK_TIMEOUT_RUN;
		}
		
		return return_err;
	}

	g_tm_wait_start = StbM_GetNowTick();

	if ( NULL != cmd_process ) {
		if ( cmd_process->err_check_func ) {
			uint16_t cmd_len = cmd_process->command_len;
			cmd_err = cmd_process->err_check_func(buff[0], buff, cmd_len);
			if ( !cmd_err ) goto fail_process;
		}

		cmd_err = cmd_process->func(buff, receive_num, &g_fp);
		if ( !cmd_err ) goto fail_process;

		if ( fp_state_command_boot == g_fp_state ) {
			flash_progress_reset_all();
			return FLASH_TASK_RUN_ERR_FLASH_FINISH;
		}

		g_fp_state = g_fp.next_fp_state;
		return FLASH_TASK_RUN_ERR_OK;
	}

fail_process:
	flash_progress_reset_all();
	return FLASH_TASK_RUN_ERR_FAIL;
}



#ifdef __cplusplus
}
#endif
