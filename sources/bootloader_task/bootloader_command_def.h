/*
 * bootloader_command_def.h
 *
 *  Created on: 2018Äê2ÔÂ7ÈÕ
 *      Author: Administrator
 */

#ifndef SOURCES_BOOTLOADER_TASK_BOOTLOADER_COMMAND_DEF_H_
#define SOURCES_BOOTLOADER_TASK_BOOTLOADER_COMMAND_DEF_H_

#define FP_COMMAND_START				0x31U
#define FP_QUERY_CAPACITY				0x32U
#define FP_COMMAND_FRAME_START			0x33U
#define FP_COMMAND_FRAME_FINISH			0x34U
#define FP_COMMAND_VERIFY				0x35U
#define FP_COMMAND_BOOT					0x36U

#define FP_QUERY_UID					0x40U
#define FP_COMMAND_SET_CAN_ID			0x41U

#define FP_COMMAND_INVALID				0xffU
#define FP_RESPONSE_INVALID				0xffU

#define FP_RESPONSE_ADD_VALUE			0x20U
#define FP_ERROR_RESPONSE				0xf7U

//#define FP_RESPONSE_ERR_CODE_OK					0x55U
#define FP_RESPONSE_ERR_CODE_STATE					0x01U	// state transfer invalid
#define FP_RESPONSE_ERR_CODE_CRC					0x02U	// payload crc fail
#define FP_RESPONSE_ERR_CODE_DATA_CRC				0x03U	// data crc fail when frame finish
#define FP_RESPONSE_ERR_CODE_TIMEOUT				0x04U
#define FP_RESPONSE_ERR_CODE_DATA_OVERFLOW			0x05U	// data receive overflow
#define FP_RESPONSE_ERR_CODE_DATA_LEN				0x06U	// data length not match when frame finish(deprecated)
#define FP_RESPONSE_ERR_CODE_FSF_SYNC				0x07U	// frame start / frame finish command not sync
#define FP_RESPONSE_ERR_CODE_CAP_LIMIT				0x08U	// capacity limit when frame start
#define FP_RESPONSE_ERR_CODE_ADDR_INVALID			0x09U	// command's address invalid
#define FP_RESPONSE_ERR_CODE_START_ADDR_NOT_MATCH	0x0AU	// TODO: TBD
#define FP_RESPONSE_ERR_CODE_ERASE_FAIL				0x0BU
#define FP_RESPONSE_ERR_CODE_FLASH_FAIL				0x0CU
#define FP_RESPONSE_ERR_VERIFY_CRC_NOT_MATCH		0x0DU
#define FP_RESPONSE_ERR_CODE_UNKNOWN				0x30U



#endif /* SOURCES_BOOTLOADER_TASK_BOOTLOADER_COMMAND_DEF_H_ */
