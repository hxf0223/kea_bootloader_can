#ifndef _HSI_2515_H_
#define _HSI_2515_H_


#include "can_common.h"
#include "dev_2515.h"

/*
 * 注意：
 * 只有在配置模式下，才能对以下寄存器进行修改：
1, CNF1、 CNF2 和 CNF3
2, TXRTSCTRL
3, 验收过滤寄存器
4, 验收屏蔽寄存器
*/


#define A_CAN_CHANNEL	0
#define D_CAN_CHANNEL	1

#define STANDARD_MAX	0X07FF
#define EXTENDED_MAX	0X01FFFFFFF

#define STANDARD_DATA_FRAME		1
#define EXTENDED_DATA_FRAME		2

// A_CAN_CHANNEL
#define DEFAULT_CAN_WORK_MODE0		STANDARD_DATA_FRAME
#define DEFAULT_STANDARD_SEND_ID0	0x0015
#define DEFAULT_STANDARD_RECV_ID0	0x18FF50E8
#define DEFAULT_EXTENDED_SEND_ID0	0xffffff
#define DEFAULT_EXTENDED_RECV_ID0	0xffffff
#define DEFAULT_2515_RXB0CTRL0		RXM_RCV_ALL | RXRTR_NO_REMOTE | BUKT_NO_ROLLOVER | FILHIT0_FLTR_0

// D_CAN_CHANNEL
#define DEFAULT_CAN_WORK_MODE1		STANDARD_DATA_FRAME
#define DEFAULT_STANDARD_SEND_ID1	0x0016
#define DEFAULT_STANDARD_RECV_ID1	0x0017
#define DEFAULT_EXTENDED_SEND_ID1	0x01234
#define DEFAULT_EXTENDED_RECV_ID1	0x18FF50E8
#define DEFAULT_2515_RXB0CTRL1		RXM_RCV_ALL | RXRTR_NO_REMOTE | BUKT_NO_ROLLOVER | FILHIT0_FLTR_0

#define DEFAULT_CANINTF				0X00
#define DEFAULT_CANINTE				0X01
#define DEFAULT_CANCTR				REQOP_NORMAL| CLKOUT_ENABLED


extern byte hsi_2515_init(byte channel, can_baud_rate_e baudRate);
extern byte hsi_2515_can_send_buffer(byte channel,uint32_t id,byte *can_tx_buf,byte len,byte can_type);
extern byte hsi_2515_can_receive_buffer(byte channel,byte *CAN_RX_Buf, byte bufCapacity,byte *flag,uint32_t *id,byte *lenth);
extern uint8_t acan_flag;

#if 0
extern void send_LSB(uint8_t *buf,uint8_t length);
#endif

#endif
