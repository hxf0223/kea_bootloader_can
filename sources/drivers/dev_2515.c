#include "dev_2515.h"
#include "hsi_2515.h"


/*******************************************************************************
* 函数名  : Delay_Nms
* 描述    : 通过软件延时约nms(不准确)
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 此方式延时时间是不准确的,准确延时建议用定时器
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
* 函数名  : dev_2515_send_byte
* 描述    : 通过SPI向MCP2515指定地址寄存器写1个字节数据
* 输入    : addr:MCP2515寄存器地址,dat:待写入的数据
* 输入    : channel:MCP2515 SPI通道选择 	0：SPI0； 	1：SPI1
* 输出    : 无
* 返回值  : 无
* 说明    : 无
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
* 函数名  : dev_2515_send_register
* 描述    : 通过SPI向MCP2515指定地址寄存器写1个字节数据
* 输入    : addr:MCP2515寄存器地址,dat:待写入的数据
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
byte dev_2515_send_register(byte channel,byte address,byte data)
{
	channel_cs_set(channel, 0);

	dev_2515_send_byte(channel,CAN_WRITE);	//发送写命令
	dev_2515_send_byte(channel,address);	//发送地址
	dev_2515_send_byte(channel,data);		//写入数据
	//dev_2515_delay(1);						//通过软件延时约nms(不准确)

	channel_cs_set(channel, 1);
}

/*******************************************************************************
* 函数名  : dev_2515_recv_byte
* 描述    : 通过SPI向MCP2515读1个字节数据
* 输入    : dat:存放接收值
* 输出    : 无
* 返回值  : 无
* 说明    : 无
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
* 函数名  : dev_2515_recv_register
* 描述    : 通过SPI从MCP2515指定地址寄器读1个字节数据
* 输入    : addr:MCP2515寄存器地址
* 输出    : 无
* 返回值  : ret:读取到寄存器的1个字节数据
* 说明    : 无
*******************************************************************************/
byte dev_2515_recv_register(byte channel,byte address,byte *data)
{
	byte ret;
	channel_cs_set(channel, 0);

	dev_2515_send_byte(channel,CAN_READ);				//发送读命令
	dev_2515_send_byte(channel,address);				//发送地址
	dev_2515_send_byte(channel,0xff);					//发送任意值
	ret=dev_2515_recv_byte(channel,data);				//读取数据

	channel_cs_set(channel, 1);
	return ret;					//返回读到的一个字节数据
}

/*******************************************************************************
* 函数名  : MCP2515_Reset
* 描述    : 发送复位指令软件复位MCP2515
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 将内部寄存器复位为缺省状态,并将器件设定为配置模式
*******************************************************************************/
void dev_2515_reset(byte channel,byte cmd)
{
	channel_cs_set(channel, 0);

	dev_2515_send_byte(channel,cmd);	// 发送寄存器复位命令
	dev_2515_delay(1);		// 通过软件延时约nms(不准确)

	channel_cs_set(channel, 1);
}
