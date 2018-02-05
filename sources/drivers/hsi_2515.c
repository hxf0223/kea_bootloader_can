#include "hsi_2515.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* 函数名  : MCP2515_Reset
* 描述    : 发送复位指令软件复位MCP2515
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 将内部寄存器复位为缺省状态,并将器件设定为配置模式
*******************************************************************************/
void hsi_2515_reset(byte channel,byte cmd)
{
	dev_2515_reset(channel,cmd);
}

void hsi_2515_config_rate(byte channel,byte baud_rate)
{
/*
 * 配置需满足一下条件：
 * 传播段+相位缓冲段PS1 >= 相位缓冲段PS2
 * 传播段+相位缓冲段PS1 >= Tdelay(默认为1~2TQ)
 * 相位缓冲段PS2 > 同步跳转宽带SJW(默认为1TQ)
 */
	byte cnf1_data = baud_rate, cnf2_data, cnf3_data;

	switch ( baud_rate )
	{
	case CAN_500Kbps:	// 8Mhz/(2*(baud_rate+1)) = 4Mhz ;	4Mhz/(1TQ+PHSEG1_3TQ+PRSEG_1TQ+PHSEG2_3TQ) = 500Khz
		cnf2_data = 0x80|PHSEG1_3TQ|PRSEG_1TQ;
		cnf3_data = PHSEG2_3TQ;
		break;
	case CAN_250Kbps:	// 8Mhz/(2*(baud_rate+1)) = 2Mhz ;	2Mhz/(1TQ+PHSEG1_3TQ+PRSEG_1TQ+PHSEG2_3TQ) = 250Khz
		cnf2_data = 0x80|PHSEG1_3TQ|PRSEG_1TQ;
		cnf3_data = PHSEG2_3TQ;
		break;
	/*case CAN_125Kbps:	// 8Mhz/(2*(baud_rate+1)) = 1Mhz ;	1Mhz/(1TQ+PHSEG1_3TQ+PRSEG_1TQ+PHSEG2_3TQ) = 125Khz
		cnf2_data = 0x80|PHSEG1_3TQ|PRSEG_1TQ;
		cnf3_data = PHSEG2_3TQ;
		break;*/
	case CAN_100Kbps:	// 8Mhz/(2*(baud_rate+1)) = 800Khz ;8Khz/(1TQ+PHSEG1_3TQ+PRSEG_1TQ+PHSEG2_3TQ) = 100Khz
		cnf2_data = 0x80|PHSEG1_3TQ|PRSEG_1TQ;
		cnf3_data = PHSEG2_3TQ;
		break;
	default:
		cnf1_data = CAN_125Kbps;
		cnf2_data = 0x80|PHSEG1_3TQ|PRSEG_1TQ;
		cnf3_data = PHSEG2_3TQ;
		break;
	}

	//设置波特率为125Kbps
	//set CNF1,SJW=00,长度为1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/8M=12.5us
	dev_2515_send_register(channel,CNF1,cnf1_data);	// 8Mhz/(2*(baud_rate+1)) = 1Mhz ; 1Mhz/(1TQ+PHSEG1_3TQ+PRSEG_1TQ+PHSEG2_3TQ) = 125Khz
	//set CNF2,SAM=0,在采样点对总线进行一次采样，PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	dev_2515_send_register(channel,CNF2,cnf2_data);
	//set CNF3,PHSEG2=(2+1)TQ=3TQ,同时当CANCTRL.CLKEN=1时设定CLKOUT引脚为时间输出使能位
	dev_2515_send_register(channel,CNF3,cnf3_data);
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
byte hsi_2515_set_standard_id(byte channel,uint16_t standard_id,uint32_t extended_id,byte mode)
{
	byte id_high,id_low;
	if(standard_id>STANDARD_MAX || extended_id>EXTENDED_MAX)
		return 1;

	if(mode==EXTENDED_DATA_FRAME)
	{
		id_high = (byte)((extended_id&0x1fe00000)>>21);
		id_low = (byte)((extended_id&0x1c0000)>>13);
		id_low = id_low | EXIDE | (byte)((extended_id&0x30000)>>16);
	}
	else
	{
		id_high = (byte)(standard_id>>3);
		id_low = (byte)(standard_id<<5);
	}

	dev_2515_send_register(channel,TXB0SIDH,id_high);//发送缓冲器0标准标识符高位
	dev_2515_send_register(channel,TXB0SIDL,id_low);//发送缓冲器0标准标识符低位
	return 0;
}
byte hsi_2515_set_extended_id(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,TXB0EID8,id_high);//发送缓冲器0标准标识符高位
	dev_2515_send_register(channel,TXB0EID0,id_low);//发送缓冲器0标准标识符低位
	return 0;
}

byte hsi_2515_set_txb0dlc(byte channel,byte data_length,byte mode)
{
	byte remote_request;
	remote_request = data_length;
	dev_2515_send_register(channel,TXB0DLC,remote_request);//将本帧待发送的数据长度写入发送缓冲器0的发送长度寄存器
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void hsi_2515_config_work_mode(byte channel,byte canctrl_mode)
{
	dev_2515_send_register(channel,CANCTRL, canctrl_mode );// 配置2515工作模式：
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void hsi_2515_config_recv_rxb0ctrl(byte channel,byte mode)
{
	dev_2515_send_register(channel,RXB0CTRL,mode);//仅仅接收标准标识符的有效信息
}

byte hsi_2515_config_standard_recv_id(byte channel,uint16_t standard_id,uint32_t extended_id,byte mode)
{
	byte id_high,id_low;
	if(standard_id>STANDARD_MAX || extended_id>EXTENDED_MAX)
		return 1;

	if(mode==EXTENDED_DATA_FRAME)
	{
		id_high = (byte)((extended_id&0x1fe00000)>>21);
		id_low = (byte)((extended_id&0x1c0000)>>13);
		id_low = id_low | (byte)((extended_id&0x30000)>>16);
	}
	else
	{
		id_high = (byte)(standard_id>>3);
		id_low = (byte)(standard_id<<5);
	}

	dev_2515_send_register(channel,RXB0SIDH,id_high);//清空接收缓冲器0的标准标识符高位
	dev_2515_send_register(channel,RXB0SIDL,id_low);//清空接收缓冲器0的标准标识符低位
	return 0;
}

byte hsi_2515_config_extended_recv_id(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,RXB0EID8,id_high);//发送缓冲器0标准标识符高位
	dev_2515_send_register(channel,RXB0EID0,id_low);//发送缓冲器0标准标识符低位
	return 0;
}

byte hsi_2515_get_rxb0dlc(byte channel,byte *lenth)
{
	return dev_2515_recv_register(channel,RXB0DLC,lenth);//读取接收缓冲器0接收到的数据长度(0~8个字节)
}


/*
void hsi_2515_config_recv_dlc(byte channel,byte dlc)
{
	dev_2515_send_register(channel,RXB0DLC,dlc);//设置接收数据的长度为8个字节
}*/
byte hsi_2515_config_standard_recv_rxf0(byte channel,uint16_t standard_id,uint32_t extended_id,byte mode)
{
  	byte id_high,id_low;
	if(standard_id>STANDARD_MAX || extended_id>EXTENDED_MAX)
		return 1;

	if(mode==EXTENDED_DATA_FRAME)
	{
		id_high = (byte)((extended_id&0x1fe00000)>>21);
		id_low = (byte)((extended_id&0x1c0000)>>13);
		id_low = id_low | EXIDE | (byte)((extended_id&0x30000)>>16);
	}
	else
	{
		id_high = (byte)(standard_id>>3);
		id_low = (byte)(standard_id<<5);
	}
	dev_2515_send_register(channel,RXF0SIDH,id_high);//配置验收滤波寄存器n标准标识符高位
	dev_2515_send_register(channel,RXF0SIDL,id_low);//配置验收滤波寄存器n标准标识符低位
	return 0;
}

byte hsi_2515_config_extended_recv_rxf0_eid(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,RXF0EID8,id_high);//发送缓冲器0标准标识符高位
	dev_2515_send_register(channel,RXF0EID0,id_low);//发送缓冲器0标准标识符低位
	return 0;
}

byte hsi_2515_config_standard_recv_rxm0(byte channel,uint16_t standard_id,uint32_t extended_id,byte mode)
{
  	byte id_high,id_low;
	if(standard_id>STANDARD_MAX || extended_id>EXTENDED_MAX)
		return 1;

	if(mode==EXTENDED_DATA_FRAME)
	{
		id_high = (byte)((extended_id&0x1fe00000)>>21);
		id_low = (byte)((extended_id&0x1c0000)>>13);
		id_low = id_low | (byte)((extended_id&0x30000)>>16);
	}
	else
	{
		id_high = (byte)(standard_id>>3);
		id_low = (byte)(standard_id<<5);
	}
	dev_2515_send_register(channel,RXM0SIDH,id_high);//配置验收屏蔽寄存器n标准标识符高位
	dev_2515_send_register(channel,RXM0SIDL,id_low);//配置验收屏蔽寄存器n标准标识符低位
	return 0;
}

byte hsi_2515_config_extended_recv_rxm0_eid(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,RXM0EID8,id_high);//发送缓冲器0标准标识符高位
	dev_2515_send_register(channel,RXM0EID0,id_low);//发送缓冲器0标准标识符低位
	return 0;
}

void hsi_2515_config_canintf(byte channel,byte clear_interrupt_flag)
{
	dev_2515_send_register(channel,CANINTF,clear_interrupt_flag);//清空CAN中断标志寄存器的所有位(必须由MCU清空)
}

byte hsi_2515_config_caninte(byte channel,byte enable_or_disable)
{
	dev_2515_send_register(channel,CANINTE,enable_or_disable);//配置CAN中断使能寄存器的接收缓冲器0满中断使能,其它位禁止中断
}

/*******************************************************************************
* 函数名  : dev_2515_init
* 描述    : MCP2515初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 初始化包括：软件复位、工作波特率设置、标识符相关配置等。
*******************************************************************************/
byte hsi_2515_init(byte channel)
{
	byte ret,temp;
	uint16_t standard_id;
	uint32_t extended_id;
	byte	can_work_mode;
	byte	rxb0ctrl;
	byte baud_rate;
	if(channel==A_CAN_CHANNEL)
	{
		standard_id = DEFAULT_STANDARD_SEND_ID0;
		extended_id = DEFAULT_EXTENDED_SEND_ID0;
		can_work_mode = DEFAULT_CAN_WORK_MODE0;
		rxb0ctrl = DEFAULT_2515_RXB0CTRL0;
		baud_rate = DEFAULT_BAUD_RATE0;
	}
	else
	{
		standard_id = DEFAULT_STANDARD_SEND_ID1;
		extended_id = DEFAULT_EXTENDED_SEND_ID1;
		can_work_mode = DEFAULT_CAN_WORK_MODE1;
		rxb0ctrl = DEFAULT_2515_RXB0CTRL1;
		baud_rate = DEFAULT_BAUD_RATE1;
	}

	//--------------------reset------------------------
	hsi_2515_reset(channel,CAN_RESET);						// 上电或者复位后，默认进入 配置模式
	hsi_2515_config_rate(channel,baud_rate);		// 波特率

	//--------------------tx---------------------------
	hsi_2515_set_standard_id(channel,standard_id,extended_id,can_work_mode);
	hsi_2515_set_extended_id(channel,extended_id);
	hsi_2515_set_txb0dlc(channel,DLC_8,can_work_mode);

	//--------------------rx---------------------------
	hsi_2515_config_recv_rxb0ctrl(channel,rxb0ctrl);
	/*hsi_2515_config_standard_recv_id(channel,standard_id,extended_id,can_work_mode);
	hsi_2515_config_extended_recv_id(channel,extended_id);
	hsi_2515_config_standard_recv_rxf0(channel,standard_id,extended_id,can_work_mode);
	hsi_2515_config_extended_recv_rxf0_eid(channel,extended_id);
	hsi_2515_config_standard_recv_rxm0(channel,standard_id,extended_id,can_work_mode);
	hsi_2515_config_extended_recv_rxm0_eid(channel,extended_id);*/

	hsi_2515_config_canintf(channel,DEFAULT_CANINTF);
	hsi_2515_config_caninte(channel,DEFAULT_CANINTE);

	//--------------------start---------------------------
	hsi_2515_config_work_mode(channel,DEFAULT_CANCTR);//将MCP2515设置为正常模式,退出配置模式

	ret=dev_2515_recv_register(channel,CANSTAT,&temp);//读取CAN状态寄存器的值
	if ( OPMODE_NORMAL!=(temp&&0xE0) )	//判断MCP2515是否已经进入正常模式
	{
		hsi_2515_config_work_mode(channel,DEFAULT_CANCTR);
	}

	return ret;
}

/*******************************************************************************
* 函数名  :	hsi_2515_can_send_buffer
* 描述    : 	CAN发送指定长度的数据
* 输入    :	CAN_TX_Buf(待发送数据缓冲区指针),len(待发送数据长度)
* 输出    :	无
* 返回值  :	无
* 说明    :	无
*******************************************************************************/
byte hsi_2515_can_send_buffer(byte channel, uint32_t id, byte *can_tx_buf, byte len, byte can_type)
{
	uint16_t standard_id = 0;
	uint32_t extended_id = 0;
	volatile uint16_t timeout = 0;
	byte temp = 0xff;

	if ( can_type == STANDARD_DATA_FRAME )	// 标准帧
	{
		if(id>STANDARD_MAX)
			return 0xff;
		standard_id = id;
	}
	else							// 扩展帧
	{
		if ( id > EXTENDED_MAX )
			return 0xff;
		extended_id = id;
	}

	do {
		dev_2515_recv_register(channel, TXB0CTRL, &temp);
		temp = temp & 0x08;
		dev_2515_delay(1);//通过软件延时约nms(不准确)
		++timeout;
	} while( temp && (timeout < 50) ); // 快速读某些状态指令,等待TXREQ标志清零

	hsi_2515_set_standard_id(channel,standard_id,extended_id,can_type);
	hsi_2515_set_extended_id(channel,extended_id);

	for ( uint8_t i = 0; i < len; i++ ) {
		dev_2515_send_register(channel, TXB0D0+i, can_tx_buf[i]);	// 将待发送的数据写入发送缓冲寄存器
	}

	hsi_2515_set_txb0dlc(channel, len, can_type);
	dev_2515_send_register(channel,TXB0CTRL,0x08|0x03);//请求发送报文
	return 0;
}

/*******************************************************************************
* 函数名  : hsi_2515_can_receive_buffer
* 描述    : CAN接收一帧数据
* 输入    : *CAN_TX_Buf(待接收数据缓冲区指针)
* 输出    : 无
* 返回值  : len(接收到数据的长度,0~8字节)
* 说明    : 无
*******************************************************************************/
byte hsi_2515_can_receive_data(byte channel, uint32_t *id, byte *flag, byte *CAN_RX_Buf, byte bufCapacity,byte *lenth)
{
	byte i, temp, len = 0;
	byte id_buf[4];

	byte ret = dev_2515_recv_register(channel,CANINTF,&temp);
	if ( temp & 0x01 )
	{
		ret = hsi_2515_get_rxb0dlc(channel,lenth);//读取接收缓冲器0接收到的数据长度(0~8个字节)
		if ( ((*lenth) & 0x0f) == 0 )	// 无数据
			return DEV_2515_ERROR_RECV_LENTH;
		for( i = 0; i < (*lenth) && i < bufCapacity; i++ )
		{
			ret = dev_2515_recv_register(channel,RXB0D0+i,&temp);//把CAN接收到的数据放入指定缓冲区
			if ( ret != 0 )
				return ret;
			CAN_RX_Buf[i]= temp;
		}
	}

	ret = dev_2515_recv_register(channel,RXB0SIDH,&id_buf[0]);
	ret = dev_2515_recv_register(channel,RXB0SIDL,&id_buf[1]);
	ret = dev_2515_recv_register(channel,RXB0EID8,&id_buf[2]);
	ret = dev_2515_recv_register(channel,RXB0EID0,&id_buf[3]);
	temp = id_buf[1]&0x03;
	temp = ((id_buf[1]>>5)<<2) | temp;
	if((id_buf[1]&IDE) && ((len&0x40) == 0))	// 扩展帧
	{
		*id = (id_buf[0]<<21) | (temp<<16) | (id_buf[2]<<8) | id_buf[3];
		*flag = EXTENDED_DATA_FRAME;
	}
	else
	{
		*id = ((id_buf[0])<<3) | (id_buf[1]>>5);
		*flag = STANDARD_DATA_FRAME;
	}

	dev_2515_send_register(channel,CANINTF,0);//清除中断标志位(中断标志寄存器必须由MCU清零)
	return ret;
}

// 输入参数，通道，接收缓冲区，flag==1 扩展帧，flag==2标准帧，id：接收到的ID
byte hsi_2515_can_receive_buffer(byte channel,byte *CAN_RX_Buf, byte bufCapacity,byte *flag, uint32_t *id,byte *lenth)
{
	byte ret = 0;
	/*
	if((I_D_ACAN_INT==0) || (I_D_DCAN_INT==0))
	{
		ret = hsi_2515_can_receive_data(channel,id,flag,CAN_RX_Buf,lenth);
		dev_2515_send_register(channel,CANINTF,0);//清除中断标志位(中断标志寄存器必须由MCU清零)
	}*/
	if ( (channel==D_CAN_CHANNEL) && (I_D_DCAN_INT==0) )
	{
		ret = hsi_2515_can_receive_data(channel,id,flag,CAN_RX_Buf, bufCapacity,lenth);
		dev_2515_send_register(channel,CANINTF,0);//清除中断标志位(中断标志寄存器必须由MCU清零)
	}
	else if ( (channel == A_CAN_CHANNEL) && (I_D_ACAN_INT == 0) )
	{
		ret = hsi_2515_can_receive_data(channel,id,flag,CAN_RX_Buf, bufCapacity,lenth);
		dev_2515_send_register(channel,CANINTF,0);//清除中断标志位(中断标志寄存器必须由MCU清零)
	}

	return ret;
}

#if 0
byte hsi_2515_test_send_buffer(byte channel,byte *can_tx_buf,byte len)
{
	unsigned char j,dly,count;
	byte can_channel;
	count=0;
	while(count<len)
	{
		dly=0;
		while((dev_2515_recv_register(channel,TXB0CTRL)&0x08) && (dly<50))//快速读某些状态指令,等待TXREQ标志清零
		{
			dev_2515_delay(1);//通过软件延时约nms(不准确)
			dly++;
		}
		for(j=0;j<len;)
		{
			hsi_2515_set_standard_id(channel,DEFAULT_STANDARD_SEND_ID1,DEFAULT_EXTENDED_SEND_ID1,EXTENDED_DATA_FRAME);
			hsi_2515_set_extended_id(channel,DEFAULT_EXTENDED_SEND_ID1);
			/*
	        dev_2515_send_register(channel,0x31, 0x08); //发送缓冲器0标准标识符高位
	        dev_2515_send_register(channel,0x32, 0xff); //发送缓冲器0标准标识符低位
	        dev_2515_send_register(channel,0x33, 0xff); //发送缓冲器0扩展标识符高位
	        dev_2515_send_register(channel,0x34, 0x01); //发送缓冲器0扩展标识符低位
*/
			dev_2515_send_register(channel,TXB0D0+j,can_tx_buf[count++]);//将待发送的数据写入发送缓冲寄存器
			j++;
			if(count>=len) break;
		}
		if(channel == SPI0_CHANNEL)
			can_channel = DEFAULT_CAN_WORK_MODE0;
		else
			can_channel = DEFAULT_CAN_WORK_MODE1;
		hsi_2515_set_txb0dlc(channel,len,can_channel);
		dev_2515_send_register(channel,TXB0CTRL,0x08|0x03);//请求发送报文
	}
}
#endif


#if 0
void send_LSB(uint8_t *buf,uint8_t length)
{
	uint8_t temp,i,j;
	uint8_t temp_buf[8];
	for(i=0;i<8;i++)
	{
		temp_buf[i] = 0x00;
	}
	//memset(temp_buf,0,8);
	for(i=0;i<length;i++)
	{
		temp = buf[i];
		for(j=0;j<8;j++)
		{
			temp_buf[i] |= (temp<<(7-j));
		}
	}
	for(i=0;i<8;i++)
	{
		*(buf + i) = temp_buf[i];
	}
	//memcpy(buf,temp_buf,8);
}
#endif




