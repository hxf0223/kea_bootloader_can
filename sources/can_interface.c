#include <stddef.h>
#include "GPIOA_Init.h"
#include "SKEAZ1284.h"
#include "Init_Config.h"
#include "can.h"
#include "can_interface.h"



#ifdef __cplusplus
extern "C" {
#endif

/* return 0 if success */
static uint8_t spi_can_transmit_tool(uint8_t channel, CANMsg* pMsg) {
	uint8_t frame_type = STANDARD_DATA_FRAME;
	if ( pMsg->m_ID > STANDARD_MAX )
		frame_type = EXTENDED_DATA_FRAME;

	return hsi_2515_can_send_buffer(channel, pMsg->m_ID, pMsg->m_data, pMsg->m_dataLen, frame_type);
}

/* return length in byte received */
static uint8_t spi_can_receive_tool( uint8_t channel, CANMsg* pMsg ) {
	uint8_t flag;
	pMsg->m_dataLen = 0;
	hsi_2515_can_receive_buffer(channel, pMsg->m_data, 8, &flag, &(pMsg->m_ID), &(pMsg->m_dataLen));
	return pMsg->m_dataLen;
}

/* charge can transmit */
static inline uint8_t dcan_transmit( CANMsg* pMsg ) {
	return spi_can_transmit_tool(D_CAN_CHANNEL, pMsg);
}

/* vehicle can transmit */
static inline uint8_t acan_transmit( CANMsg* pMsg ) {
	return spi_can_transmit_tool(A_CAN_CHANNEL, pMsg);
}

static inline uint8_t dcan_receive( CANMsg* pMsg ) {
	return spi_can_receive_tool(D_CAN_CHANNEL, pMsg);
}

static inline uint8_t acan_receive( CANMsg* pMsg ) {
	return spi_can_receive_tool(A_CAN_CHANNEL, pMsg);
}

static inline uint8_t scan_transmit( CANMsg* pMsg ) {
	return CAN_Send_Msg(pMsg);
}

static uint8_t scan_receive( CANMsg* pMsg ) {
	pMsg->m_dataLen = 0;
	uint8_t result = CAN_Receive_Msg(pMsg);
	return pMsg->m_dataLen;
}


typedef uint8_t (*can_transmit_func_t)(CANMsg* pMsg);
typedef uint8_t (*can_receive_func_t)(CANMsg* pMsg);

typedef struct {
	uint32_t rxid;
	uint32_t txid;
	can_receive_func_t receive_func;
	can_transmit_func_t transmit_func;
} can_entry_func_t;

static uint8_t g_active_can_entry_id = 0xff;
static can_entry_func_t g_can_entry[] = {
		{BOOTLOADER_RX_CAN_ID, BOOTLOADER_TX_CAN_ID, scan_receive, scan_transmit},
#if (ENABLE_MCP2515 == 1)
		{BOOTLOADER_RX_CAN_ID, BOOTLOADER_TX_CAN_ID, acan_receive, acan_transmit},
		{BOOTLOADER_RX_CAN_ID, BOOTLOADER_TX_CAN_ID, dcan_receive, dcan_transmit}
#endif
};

#define CAN_ENTRY_NUM		(int)(sizeof(g_can_entry)/sizeof(g_can_entry[0]))

uint8_t can_interface_init(const void* p) {
	if ( NULL != p ) {
		can_interface_init_t* pinit = (can_interface_init_t*)p;
		for ( uint8_t i = 0; i < CAN_ENTRY_NUM; i++ ) {
			g_can_entry[i].txid = pinit->nonvc_can_tx_id;
		}
	}

	return 0;
}

uint8_t can_transmit( CANMsg* pMsg ) {
	if ( g_active_can_entry_id < CAN_ENTRY_NUM ) {
		pMsg->m_ID = g_can_entry[g_active_can_entry_id].txid;
		return g_can_entry[g_active_can_entry_id].transmit_func(pMsg);
	}

	return 0;
}

uint8_t can_receive( CANMsg* pMsg ) {
	if ( g_active_can_entry_id >= CAN_ENTRY_NUM ) {
		for ( uint8_t i = 0; i < CAN_ENTRY_NUM; i++ ) {
			uint8_t rec_num = g_can_entry[i].receive_func(pMsg);
			if ( rec_num > 0 ) {
				if ( pMsg->m_ID == BOOTLOADER_RX_CAN_ID || 
					pMsg->m_ID == BOOTLOADER_RX_BROADCAST_CAN_ID )
					g_active_can_entry_id = i;
				return rec_num;
			}
		}
	} else {
		return g_can_entry[g_active_can_entry_id].receive_func(pMsg);
	}

	return 0;
}



void init_can_frame(CANMsg* pMsg) {
	pMsg->m_IDE = pMsg->m_RTR = 0;
	pMsg->m_dataLen = CAN_FRAME_LEN;
	for ( uint8_t i = 0; i < CAN_FRAME_LEN; i++ ) {
		pMsg->m_data[i] = CAN_FILL_BYTE;
	}
}

uint32_t get_u32_from_stream(const uint8_t* buff, uint16_t offset) {
#ifdef CAN_BIG_ENDIAN
	uint32_t temp = (uint32_t)(buff[offset] << 24)
			+ (uint32_t)(buff[offset+1] << 16)
			+ (uint32_t)(buff[offset+2] << 8)
			+ buff[offset+3];
#else
	uint32_t temp = buff[offset]
			+ (uint32_t)(buff[offset+1] << 8)
			+ (uint32_t)(buff[offset+2] << 16)
			+ (uint32_t)(buff[offset+3] << 24);
#endif

	return temp;
}

uint16_t get_u16_from_stream(const uint8_t* buff, uint16_t offset) {
#ifdef CAN_BIG_ENDIAN
	uint16_t temp = (uint16_t)(buff[offset] << 8) + buff[offset+1];
#else
	uint16_t temp = buff[offset] + (uint16_t)(buff[offset+1] << 8);
#endif

	return temp;
}

void put_u32_to_stream( uint32_t data, uint8_t* buff, uint16_t offset ) {
	uint8_t* p = (uint8_t*)(&data);
#ifdef CAN_BIG_ENDIAN
	for (uint8_t i = 3; i >= 0; i-- ) {
		buff[offset++] = p[i];
	}
#else
	for (uint8_t i = 0; i < 4; i++ ) {
		buff[offset++] = p[i];
	}
#endif
}

void put_u16_to_stream(uint16_t data, uint8_t* buff, uint16_t offset) {
	uint8_t* p = (uint8_t*)(&data);
#ifdef CAN_BIG_ENDIAN
	buff[offset++] = p[1];
	buff[offset++] = p[0];
#else
	buff[offset++] = p[0];
	buff[offset++] = p[1];
#endif
}




#ifdef __cplusplus
}
#endif
