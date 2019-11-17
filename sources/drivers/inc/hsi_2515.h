#ifndef _HSI_2515_H_
#define _HSI_2515_H_


#include "can_common.h"
#include "dev_2515.h"

/*
 * 娉ㄦ剰锛�
 * 鍙湁鍦ㄩ厤缃ā寮忎笅锛屾墠鑳藉浠ヤ笅瀵勫瓨鍣ㄨ繘琛屼慨鏀癸細
//CNF1銆�CNF2 鍜�CNF3
//TXRTSCTRL
//楠屾敹杩囨护瀵勫瓨鍣�
//楠屾敹灞忚斀瀵勫瓨鍣�
 */


#define A_CAN_CHANNEL	0
#define D_CAN_CHANNEL	1

#define STANDARD_MAX	0X07FF
#define EXTENDED_MAX	0X01FFFFFFF

#define STANDARD_DATA_FRAME		1	// 鏍囧噯鏁版嵁甯�
//#define STANDARD_REMOTE_FRAME	1	// 鏍囧噯杩滅▼甯�
#define EXTENDED_DATA_FRAME		2	// 鎵╁睍鏁版嵁甯�
//#define EXTENDED_REMOTE_FRAME	3	// 鎵╁睍杩滅▼甯�

// 鍒濆鍖栭厤缃�515宸ヤ綔
// A_CAN_CHANNEL
#define DEFAULT_CAN_WORK_MODE0		STANDARD_DATA_FRAME
#define DEFAULT_STANDARD_SEND_ID0	0x0015	// 11浣�
#define DEFAULT_STANDARD_RECV_ID0	0x18FF50E8		//鏄撳姩鍔�0S鍏呯數鏈烘帴鏀禝D
#define DEFAULT_EXTENDED_SEND_ID0	0xffffff	// 29浣�11+18
#define DEFAULT_EXTENDED_RECV_ID0	0xffffff
//#define DEFAULT_BAUD_RATE0			CAN_250Kbps
#define DEFAULT_BAUD_RATE0			ACAN_BAUD_RATE_SET
#define DEFAULT_2515_RXB0CTRL0		RXM_RCV_ALL | RXRTR_NO_REMOTE | BUKT_NO_ROLLOVER | FILHIT0_FLTR_0
// D_CAN_CHANNEL
#define DEFAULT_CAN_WORK_MODE1		STANDARD_DATA_FRAME
#define DEFAULT_STANDARD_SEND_ID1	0x0016	// 11浣�
#define DEFAULT_STANDARD_RECV_ID1	0x0017
#define DEFAULT_EXTENDED_SEND_ID1	0x01234	// 29浣�11+18
#define DEFAULT_EXTENDED_RECV_ID1	0x18FF50E8
//#define DEFAULT_BAUD_RATE1			CAN_250Kbps//CAN_500Kbps//CAN_250Kbps//CAN_125Kbps//CAN_500Kbps
#define DEFAULT_BAUD_RATE1			DCAN_BAUD_RATE_SET
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
