#include <stdint.h>
#include "low_level_init.h"

#ifndef _CAN_INTERFACE_H
#define _CAN_INTERFACE_H

#define CAN_FRAME_LEN			8U
#define CAN_FILL_BYTE			0xAAU

#define BOOTLOADER_RX_CAN_ID	((uint32_t)0x102)   /* default can rx id */
#define BOOTLOADER_TX_CAN_ID	((uint32_t)0x112)
#define BOOTLOADER_RX_BROADCAST_CAN_ID	((uint32_t)0x7DC)

typedef uint8_t (*is_valid_rx_can_id_cb_t)(uint32_t);

typedef struct {
    uint32_t nonvc_can_tx_id;
} can_interface_init_t;

uint8_t can_interface_init(const void* p);

uint8_t can_transmit(CANMsg* pMsg);
uint8_t can_receive(CANMsg* pMsg);

void init_can_frame(CANMsg* pMsg);

uint32_t get_u32_from_stream(const uint8_t* buff, uint16_t offset);
uint16_t get_u16_from_stream(const uint8_t* buff, uint16_t offset);

void put_u32_to_stream(uint32_t data, uint8_t* buff, uint16_t offset);
void put_u16_to_stream(uint16_t data, uint8_t* buff, uint16_t offset);

#endif
