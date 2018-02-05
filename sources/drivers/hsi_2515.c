#include "hsi_2515.h"

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
/*******************************************************************************
* ������  : MCP2515_Reset
* ����    : ���͸�λָ�������λMCP2515
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ڲ��Ĵ�����λΪȱʡ״̬,���������趨Ϊ����ģʽ
*******************************************************************************/
void hsi_2515_reset(byte channel,byte cmd)
{
	dev_2515_reset(channel,cmd);
}

void hsi_2515_config_rate(byte channel,byte baud_rate)
{
/*
 * ����������һ��������
 * ������+��λ�����PS1 >= ��λ�����PS2
 * ������+��λ�����PS1 >= Tdelay(Ĭ��Ϊ1~2TQ)
 * ��λ�����PS2 > ͬ����ת���SJW(Ĭ��Ϊ1TQ)
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

	//���ò�����Ϊ125Kbps
	//set CNF1,SJW=00,����Ϊ1TQ,BRP=49,TQ=[2*(BRP+1)]/Fsoc=2*50/8M=12.5us
	dev_2515_send_register(channel,CNF1,cnf1_data);	// 8Mhz/(2*(baud_rate+1)) = 1Mhz ; 1Mhz/(1TQ+PHSEG1_3TQ+PRSEG_1TQ+PHSEG2_3TQ) = 125Khz
	//set CNF2,SAM=0,�ڲ���������߽���һ�β�����PHSEG1=(2+1)TQ=3TQ,PRSEG=(0+1)TQ=1TQ
	dev_2515_send_register(channel,CNF2,cnf2_data);
	//set CNF3,PHSEG2=(2+1)TQ=3TQ,ͬʱ��CANCTRL.CLKEN=1ʱ�趨CLKOUT����Ϊʱ�����ʹ��λ
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

	dev_2515_send_register(channel,TXB0SIDH,id_high);//���ͻ�����0��׼��ʶ����λ
	dev_2515_send_register(channel,TXB0SIDL,id_low);//���ͻ�����0��׼��ʶ����λ
	return 0;
}
byte hsi_2515_set_extended_id(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,TXB0EID8,id_high);//���ͻ�����0��׼��ʶ����λ
	dev_2515_send_register(channel,TXB0EID0,id_low);//���ͻ�����0��׼��ʶ����λ
	return 0;
}

byte hsi_2515_set_txb0dlc(byte channel,byte data_length,byte mode)
{
	byte remote_request;
	remote_request = data_length;
	dev_2515_send_register(channel,TXB0DLC,remote_request);//����֡�����͵����ݳ���д�뷢�ͻ�����0�ķ��ͳ��ȼĴ���
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void hsi_2515_config_work_mode(byte channel,byte canctrl_mode)
{
	dev_2515_send_register(channel,CANCTRL, canctrl_mode );// ����2515����ģʽ��
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void hsi_2515_config_recv_rxb0ctrl(byte channel,byte mode)
{
	dev_2515_send_register(channel,RXB0CTRL,mode);//�������ձ�׼��ʶ������Ч��Ϣ
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

	dev_2515_send_register(channel,RXB0SIDH,id_high);//��ս��ջ�����0�ı�׼��ʶ����λ
	dev_2515_send_register(channel,RXB0SIDL,id_low);//��ս��ջ�����0�ı�׼��ʶ����λ
	return 0;
}

byte hsi_2515_config_extended_recv_id(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,RXB0EID8,id_high);//���ͻ�����0��׼��ʶ����λ
	dev_2515_send_register(channel,RXB0EID0,id_low);//���ͻ�����0��׼��ʶ����λ
	return 0;
}

byte hsi_2515_get_rxb0dlc(byte channel,byte *lenth)
{
	return dev_2515_recv_register(channel,RXB0DLC,lenth);//��ȡ���ջ�����0���յ������ݳ���(0~8���ֽ�)
}


/*
void hsi_2515_config_recv_dlc(byte channel,byte dlc)
{
	dev_2515_send_register(channel,RXB0DLC,dlc);//���ý������ݵĳ���Ϊ8���ֽ�
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
	dev_2515_send_register(channel,RXF0SIDH,id_high);//���������˲��Ĵ���n��׼��ʶ����λ
	dev_2515_send_register(channel,RXF0SIDL,id_low);//���������˲��Ĵ���n��׼��ʶ����λ
	return 0;
}

byte hsi_2515_config_extended_recv_rxf0_eid(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,RXF0EID8,id_high);//���ͻ�����0��׼��ʶ����λ
	dev_2515_send_register(channel,RXF0EID0,id_low);//���ͻ�����0��׼��ʶ����λ
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
	dev_2515_send_register(channel,RXM0SIDH,id_high);//�����������μĴ���n��׼��ʶ����λ
	dev_2515_send_register(channel,RXM0SIDL,id_low);//�����������μĴ���n��׼��ʶ����λ
	return 0;
}

byte hsi_2515_config_extended_recv_rxm0_eid(byte channel,uint32_t extended_id)
{
	byte id_high,id_low;
	if(extended_id>EXTENDED_MAX)
		return 1;

	id_high = (byte)(extended_id>>8);
	id_low = (byte)extended_id;

	dev_2515_send_register(channel,RXM0EID8,id_high);//���ͻ�����0��׼��ʶ����λ
	dev_2515_send_register(channel,RXM0EID0,id_low);//���ͻ�����0��׼��ʶ����λ
	return 0;
}

void hsi_2515_config_canintf(byte channel,byte clear_interrupt_flag)
{
	dev_2515_send_register(channel,CANINTF,clear_interrupt_flag);//���CAN�жϱ�־�Ĵ���������λ(������MCU���)
}

byte hsi_2515_config_caninte(byte channel,byte enable_or_disable)
{
	dev_2515_send_register(channel,CANINTE,enable_or_disable);//����CAN�ж�ʹ�ܼĴ����Ľ��ջ�����0���ж�ʹ��,����λ��ֹ�ж�
}

/*******************************************************************************
* ������  : dev_2515_init
* ����    : MCP2515��ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ʼ�������������λ���������������á���ʶ��������õȡ�
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
	hsi_2515_reset(channel,CAN_RESET);						// �ϵ���߸�λ��Ĭ�Ͻ��� ����ģʽ
	hsi_2515_config_rate(channel,baud_rate);		// ������

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
	hsi_2515_config_work_mode(channel,DEFAULT_CANCTR);//��MCP2515����Ϊ����ģʽ,�˳�����ģʽ

	ret=dev_2515_recv_register(channel,CANSTAT,&temp);//��ȡCAN״̬�Ĵ�����ֵ
	if ( OPMODE_NORMAL!=(temp&&0xE0) )	//�ж�MCP2515�Ƿ��Ѿ���������ģʽ
	{
		hsi_2515_config_work_mode(channel,DEFAULT_CANCTR);
	}

	return ret;
}

/*******************************************************************************
* ������  :	hsi_2515_can_send_buffer
* ����    : 	CAN����ָ�����ȵ�����
* ����    :	CAN_TX_Buf(���������ݻ�����ָ��),len(���������ݳ���)
* ���    :	��
* ����ֵ  :	��
* ˵��    :	��
*******************************************************************************/
byte hsi_2515_can_send_buffer(byte channel, uint32_t id, byte *can_tx_buf, byte len, byte can_type)
{
	uint16_t standard_id = 0;
	uint32_t extended_id = 0;
	volatile uint16_t timeout = 0;
	byte temp = 0xff;

	if ( can_type == STANDARD_DATA_FRAME )	// ��׼֡
	{
		if(id>STANDARD_MAX)
			return 0xff;
		standard_id = id;
	}
	else							// ��չ֡
	{
		if ( id > EXTENDED_MAX )
			return 0xff;
		extended_id = id;
	}

	do {
		dev_2515_recv_register(channel, TXB0CTRL, &temp);
		temp = temp & 0x08;
		dev_2515_delay(1);//ͨ�������ʱԼnms(��׼ȷ)
		++timeout;
	} while( temp && (timeout < 50) ); // ���ٶ�ĳЩ״ָ̬��,�ȴ�TXREQ��־����

	hsi_2515_set_standard_id(channel,standard_id,extended_id,can_type);
	hsi_2515_set_extended_id(channel,extended_id);

	for ( uint8_t i = 0; i < len; i++ ) {
		dev_2515_send_register(channel, TXB0D0+i, can_tx_buf[i]);	// �������͵�����д�뷢�ͻ���Ĵ���
	}

	hsi_2515_set_txb0dlc(channel, len, can_type);
	dev_2515_send_register(channel,TXB0CTRL,0x08|0x03);//�����ͱ���
	return 0;
}

/*******************************************************************************
* ������  : hsi_2515_can_receive_buffer
* ����    : CAN����һ֡����
* ����    : *CAN_TX_Buf(���������ݻ�����ָ��)
* ���    : ��
* ����ֵ  : len(���յ����ݵĳ���,0~8�ֽ�)
* ˵��    : ��
*******************************************************************************/
byte hsi_2515_can_receive_data(byte channel, uint32_t *id, byte *flag, byte *CAN_RX_Buf, byte bufCapacity,byte *lenth)
{
	byte i, temp, len = 0;
	byte id_buf[4];

	byte ret = dev_2515_recv_register(channel,CANINTF,&temp);
	if ( temp & 0x01 )
	{
		ret = hsi_2515_get_rxb0dlc(channel,lenth);//��ȡ���ջ�����0���յ������ݳ���(0~8���ֽ�)
		if ( ((*lenth) & 0x0f) == 0 )	// ������
			return DEV_2515_ERROR_RECV_LENTH;
		for( i = 0; i < (*lenth) && i < bufCapacity; i++ )
		{
			ret = dev_2515_recv_register(channel,RXB0D0+i,&temp);//��CAN���յ������ݷ���ָ��������
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
	if((id_buf[1]&IDE) && ((len&0x40) == 0))	// ��չ֡
	{
		*id = (id_buf[0]<<21) | (temp<<16) | (id_buf[2]<<8) | id_buf[3];
		*flag = EXTENDED_DATA_FRAME;
	}
	else
	{
		*id = ((id_buf[0])<<3) | (id_buf[1]>>5);
		*flag = STANDARD_DATA_FRAME;
	}

	dev_2515_send_register(channel,CANINTF,0);//����жϱ�־λ(�жϱ�־�Ĵ���������MCU����)
	return ret;
}

// ���������ͨ�������ջ�������flag==1 ��չ֡��flag==2��׼֡��id�����յ���ID
byte hsi_2515_can_receive_buffer(byte channel,byte *CAN_RX_Buf, byte bufCapacity,byte *flag, uint32_t *id,byte *lenth)
{
	byte ret = 0;
	/*
	if((I_D_ACAN_INT==0) || (I_D_DCAN_INT==0))
	{
		ret = hsi_2515_can_receive_data(channel,id,flag,CAN_RX_Buf,lenth);
		dev_2515_send_register(channel,CANINTF,0);//����жϱ�־λ(�жϱ�־�Ĵ���������MCU����)
	}*/
	if ( (channel==D_CAN_CHANNEL) && (I_D_DCAN_INT==0) )
	{
		ret = hsi_2515_can_receive_data(channel,id,flag,CAN_RX_Buf, bufCapacity,lenth);
		dev_2515_send_register(channel,CANINTF,0);//����жϱ�־λ(�жϱ�־�Ĵ���������MCU����)
	}
	else if ( (channel == A_CAN_CHANNEL) && (I_D_ACAN_INT == 0) )
	{
		ret = hsi_2515_can_receive_data(channel,id,flag,CAN_RX_Buf, bufCapacity,lenth);
		dev_2515_send_register(channel,CANINTF,0);//����жϱ�־λ(�жϱ�־�Ĵ���������MCU����)
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
		while((dev_2515_recv_register(channel,TXB0CTRL)&0x08) && (dly<50))//���ٶ�ĳЩ״ָ̬��,�ȴ�TXREQ��־����
		{
			dev_2515_delay(1);//ͨ�������ʱԼnms(��׼ȷ)
			dly++;
		}
		for(j=0;j<len;)
		{
			hsi_2515_set_standard_id(channel,DEFAULT_STANDARD_SEND_ID1,DEFAULT_EXTENDED_SEND_ID1,EXTENDED_DATA_FRAME);
			hsi_2515_set_extended_id(channel,DEFAULT_EXTENDED_SEND_ID1);
			/*
	        dev_2515_send_register(channel,0x31, 0x08); //���ͻ�����0��׼��ʶ����λ
	        dev_2515_send_register(channel,0x32, 0xff); //���ͻ�����0��׼��ʶ����λ
	        dev_2515_send_register(channel,0x33, 0xff); //���ͻ�����0��չ��ʶ����λ
	        dev_2515_send_register(channel,0x34, 0x01); //���ͻ�����0��չ��ʶ����λ
*/
			dev_2515_send_register(channel,TXB0D0+j,can_tx_buf[count++]);//�������͵�����д�뷢�ͻ���Ĵ���
			j++;
			if(count>=len) break;
		}
		if(channel == SPI0_CHANNEL)
			can_channel = DEFAULT_CAN_WORK_MODE0;
		else
			can_channel = DEFAULT_CAN_WORK_MODE1;
		hsi_2515_set_txb0dlc(channel,len,can_channel);
		dev_2515_send_register(channel,TXB0CTRL,0x08|0x03);//�����ͱ���
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




