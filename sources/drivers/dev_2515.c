#include "dev_2515.h"
#include "hsi_2515.h"


/*******************************************************************************
* ������  : Delay_Nms
* ����    : ͨ�������ʱԼnms(��׼ȷ)
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �˷�ʽ��ʱʱ���ǲ�׼ȷ��,׼ȷ��ʱ�����ö�ʱ��
*******************************************************************************/
void dev_2515_delay(unsigned int x)
{
	unsigned int y;

	for ( ; x > 0; x-- ) {
		for (y = 0; y < 100; y++);
	}
}


static void channel_cs_set(byte chan, byte v) {
	if ( chan == A_CAN_CHANNEL ) {
		O_D_ACAN_CS(v);
	} else {
		O_D_DCAN_CS(v);
	}
}

/*******************************************************************************
* ������  : dev_2515_send_byte
* ����    : ͨ��SPI��MCP2515ָ����ַ�Ĵ���д1���ֽ�����
* ����    : addr:MCP2515�Ĵ�����ַ,dat:��д�������
* ����    : channel:MCP2515 SPIͨ��ѡ�� 	0��SPI0�� 	1��SPI1
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
byte dev_2515_send_byte(byte channel,byte data)
{
	uint16_t timeout = 3000;
	while( ((SPI0_S & SPI_S_SPTEF_MASK)==0) && timeout ) {
		--timeout;
		if ( timeout == 0 ) {
			(void)SPI0_S;
			return 1;
		}
	}

	(void)SPI0_S;
	SPI0_D;
	SPI0_D = data;

    timeout = 3000;
    while ( ((SPI0_S & SPI_S_SPRF_MASK)==0) && timeout ) {
        --timeout;
        if ( timeout == 0 ) {
            return 1;
        }
    }

	return 0;
}

/*******************************************************************************
* ������  : dev_2515_send_register
* ����    : ͨ��SPI��MCP2515ָ����ַ�Ĵ���д1���ֽ�����
* ����    : addr:MCP2515�Ĵ�����ַ,dat:��д�������
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
byte dev_2515_send_register(byte channel,byte address,byte data)
{
	channel_cs_set(channel, 0);

	dev_2515_send_byte(channel,CAN_WRITE);	//����д����
	dev_2515_send_byte(channel,address);	//���͵�ַ
	dev_2515_send_byte(channel,data);		//д������
	//dev_2515_delay(1);						//ͨ�������ʱԼnms(��׼ȷ)

	channel_cs_set(channel, 1);
}

/*******************************************************************************
* ������  : dev_2515_recv_byte
* ����    : ͨ��SPI��MCP2515��1���ֽ�����
* ����    : dat:��Ž���ֵ
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
byte dev_2515_recv_byte(byte channel,byte *data)
{
	uint16_t timeout = 0x1fff;
	while( (SPI0_S & SPI_S_SPRF_MASK) == 0 ) {
		if ( (--timeout) == 0 ) {
			(void)SPI0_S;
			return DEV_2515_ERROR_RECV_DATA;
		}
	}

	(void)SPI0_S;
	*data = SPI0_D;
	return 0;
}


/*******************************************************************************
* ������  : dev_2515_recv_register
* ����    : ͨ��SPI��MCP2515ָ����ַ������1���ֽ�����
* ����    : addr:MCP2515�Ĵ�����ַ
* ���    : ��
* ����ֵ  : ret:��ȡ���Ĵ�����1���ֽ�����
* ˵��    : ��
*******************************************************************************/
byte dev_2515_recv_register(byte channel,byte address,byte *data)
{
	byte ret;
	channel_cs_set(channel, 0);

	dev_2515_send_byte(channel,CAN_READ);				//���Ͷ�����
	dev_2515_send_byte(channel,address);				//���͵�ַ
	dev_2515_send_byte(channel,0xff);					//��������ֵ
	ret=dev_2515_recv_byte(channel,data);				//��ȡ����

	channel_cs_set(channel, 1);
	return ret;					//���ض�����һ���ֽ�����
}

/*******************************************************************************
* ������  : MCP2515_Reset
* ����    : ���͸�λָ�������λMCP2515
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ڲ��Ĵ�����λΪȱʡ״̬,���������趨Ϊ����ģʽ
*******************************************************************************/
void dev_2515_reset(byte channel,byte cmd)
{
	channel_cs_set(channel, 0);

	dev_2515_send_byte(channel,cmd);	// ���ͼĴ�����λ����
	dev_2515_delay(1);		// ͨ�������ʱԼnms(��׼ȷ)

	channel_cs_set(channel, 1);
}
