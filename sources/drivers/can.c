//============================================================================
//文件名称：can.c
//功能概要：can底层驱动构件源文件
//版权所有：苏州大学飞思卡尔嵌入式中心(sumcu.suda.edu.cn)
//更新记录：2015-5-11  V1.5
//芯片类型：KEA128
//============================================================================
#include "can.h"

//===========================================================================
//函数名称：CAN_Init
//功能概要：初始化MSCAN模块，采用24MHz总线时钟作为模块时钟，波特率固定为500Kbps
//参数说明：mode:工作模式选择:
//        LOOP_MODE(0)   - 回环模式
//        NORMAL_MODE(1) - 普通模式
//        open_filter:接收过滤器开关
//          1 - 开启过滤器，只接收标识符ID为acc_id的帧
//          0 - 关闭过滤器，接收所有帧
//        acc_id:接受ID，接收过滤器只接收标识符符合此ID的帧，过滤其他ID的帧
//函数返回：无
//===========================================================================
void mscan_init(uint_8 mode,uint_8 open_filter,uint_32 acc_id, can_baud_rate_e baudRate)
{
	MSCAN_MemMapPtr p = (MSCAN_MemMapPtr) MSCAN_BASE_PTR;

    //MSCAN的引脚复用选择CAN_TX为PTC7，CAN_RX为PTC6
    SIM_PINSEL1 &= (uint_32)~(uint_32)(SIM_PINSEL1_MSCANPS_MASK);
    //MSCAN模块时钟门控制；SIM_SCGC的MSCAN位=1：使能总线给MSCAN时钟；
    //SIM_SCGC的MSCAN位=0：禁止总线给MSCAN时钟
    SIM_SCGC |=(uint_32)(SIM_SCGC_MSCAN_MASK);

    //MSCAN12模块使能
    p->CANCTL1 |= MSCAN_CANCTL1_CANE_MASK;

    //请求进入初始化模式
    p->CANCTL0 |= MSCAN_CANCTL0_INITRQ_MASK;

    //等待应答进入初始化模式
    while ((p->CANCTL1 & MSCAN_CANCTL1_INITAK_MASK)== 0);

    if(open_filter)         //开启接收过滤器,只接收标示符为acc_id的帧
    {
        // 使用2个32位接收过滤器
        p->CANIDAC &= ~MSCAN_CANIDAC_IDAM_MASK;

        // 第1个32位滤波器
        // 设置接受寄存器的值
        p->CANIDAR_BANK_1[0]=acc_id>>3;
        p->CANIDAR_BANK_1[1]=acc_id<<5;
        p->CANIDAR_BANK_1[2]=0xFF;
        p->CANIDAR_BANK_1[3]=0xFF;

        // 设置屏蔽寄存器的值
        p->CANIDMR_BANK_1[0]=0x00;
        p->CANIDMR_BANK_1[1]=0x1F;
        p->CANIDMR_BANK_1[2]=0xFF;
        p->CANIDMR_BANK_1[3]=0xFF;

        // 第2个32位滤波器
        // 设置接受寄存器的值
        p->CANIDAR_BANK_2[0]=acc_id>>3;
        p->CANIDAR_BANK_2[1]=acc_id<<5;
        p->CANIDAR_BANK_2[2]=0xFF;
        p->CANIDAR_BANK_2[3]=0xFF;

        // 设置屏蔽寄存器的值
        p->CANIDMR_BANK_2[0]=0x00;
        p->CANIDMR_BANK_2[1]=0x1F;
        p->CANIDMR_BANK_2[2]=0xFF;
        p->CANIDMR_BANK_2[3]=0xFF;
    }
    else//关闭过滤器
    {
        for (uint_8 i = 0; i < 16; i++)
        {
            if(i>=0 && i <=3)
                p->CANIDAR_BANK_1[i] = 0xFF;
            else if(i>=4 && i <=7)
                p->CANIDMR_BANK_1[i-4] = 0xFF;
            else if(i>=8 && i <=11)
                p->CANIDAR_BANK_2[i-8] = 0xFF;
            else
                p->CANIDMR_BANK_2[i-12]= 0xFF;
        }
    }

    //配置时钟
    p->CANCTL1 |= MSCAN_CANCTL1_CLKSRC_MASK;    // 采用总线时钟(24MHz)作为模块时钟
    p->CANBTR0 |= MSCAN_CANBTR0_SJW(0);
    p->CANBTR1 |= MSCAN_CANBTR1_SAMP_MASK;     // 位时间采样次数为3

	switch (baudRate) {
	case can_baud_rate_500k: // baudrate = 模块时钟 / [(1 + TSEG1 + TSEG2) * 分频因子] = 20000K/[(1+13+6)*2]=500Kbps
		p->CANBTR0 |= MSCAN_CANBTR0_BRP(1);		// 模块时钟2分频
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG2(5);	// TSEG2:5+1=6 Tq
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG1(12);	// TSEG1:12+1=13 Tq
		break;

	case can_baud_rate_125k:
		p->CANBTR0 |= MSCAN_CANBTR0_BRP(0x1A);	// 模块时钟27分频
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG2(0x01);// TSEG2:1+1=6 Tq
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG1(0x03);// TSEG1:3+1=13 Tq
		break;

	default:	// default to 250k
		p->CANBTR0 |= MSCAN_CANBTR0_BRP(0x0D);	// 模块时钟14分频
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG2(0x01);// TSEG2:1+1=6 Tq
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG1(0x03);// TSEG1:3+1=13 Tq
		break;
	}

    //配置工作模式
    if(LOOP_MODE == mode)
        p->CANCTL1 |= MSCAN_CANCTL1_LOOPB_MASK; // 开启回环模式
    p->CANCTL1 &= ~MSCAN_CANCTL1_LISTEN_MASK;   // 禁止侦听模式，即开启正常模式

    //退出初始化模式
    p->CANCTL0 &= ~MSCAN_CANCTL0_INITRQ_MASK;   //退出初始化模式，进入正常模式

    //设置中断方式
    p->CANTIER = 0x00;                          //禁止发送中断
    p->CANRIER = 0x00;                          //开启接收满中断
    //设置嵌套向量中断控制寄存器值，使能CAN接收中断
    //NVIC_EnableIRQ(30);

    //等待应答初始化模式
    while ((p->CANCTL1 & MSCAN_CANCTL1_INITAK_MASK) == 1){}

    //等待总线通信时钟同步
    while ((p->CANCTL0 & MSCAN_CANCTL0_SYNCH_MASK) == 0){}
}

//===========================================================================
//函数名称：CAN_Fill_STD_Msg
//功能概要：填充一个CAN数据包
//参数说明：message:待发送CAN数据包结构体的首地址
//        id:待发送CAN标准帧的标识符ID（当前节点ID）
//        data:待发送数据缓冲区首地址
//        len:待发送数据长度（<=8 Bytes）
//函数返回：0 - 填充成功
//        1 - 数据长度输入错误
//===========================================================================
uint_8 CAN_Fill_STD_Msg(CANMsg *message,uint_32 id,uint_8 *data,uint_8 len)
{
    uint_8 i;

    message->m_ID = id;
    message->m_IDE = 0;
    message->m_RTR = 0;
    message->m_priority = 0;                      //设置发送缓冲区优先级

    if ( len > 8 ) {
        return 1;                                 //数据长度输入错误
    }

    message->m_dataLen = len;
    for ( i = 0; i < message->m_dataLen; i++ ) {
        message->m_data[i] = data[i];
    }

    return 0;                                     //填充成功
}

//===========================================================================
//函数名称：CAN_Send_Msg
//功能概要：CAN发送数据包函数
//参数说明：sendmsg:所发送的CAN数据包的首地址
//函数返回：0 - 发送成功
//        1 - 数据帧长度错误
//        2 - 发送帧为扩展帧
//===========================================================================
uint_8 CAN_Send_Msg(CANMsg *sendmsg)
{
     uint_8 txEmptyBuf;                      //空闲缓冲区掩码
     uint_8 i;
     MSCAN_MemMapPtr p = (MSCAN_MemMapPtr) MSCAN_BASE_PTR;

     //检查数据长度
     if (sendmsg->m_dataLen > 8)
     {
        return 1;                            //数据帧长度错误
     }

     //检查总线是否同步
     while(!(p->CANCTL0 & MSCAN_CANCTL0_SYNCH_MASK));

     //寻找空闲缓冲区
     txEmptyBuf = 0;
     do
     {
         p->CANTBSEL = p->CANTFLG;
         txEmptyBuf = p->CANTBSEL;
     }
     while (!txEmptyBuf);

     //写入标识符ID
     if (sendmsg->m_IDE == 0)                //按标准帧填充ID
     {
         p->TSIDR0 = (uint_8)(sendmsg->m_ID>>3);
         p->TSIDR1 = (uint_8)(sendmsg->m_ID<<5);
         p->TSIDR1 |= (sendmsg->m_RTR)<<MSCAN_TSIDR1_TSRTR_SHIFT;
         p->TSIDR1 |= (sendmsg->m_IDE)<<MSCAN_TSIDR1_TSIDE_SHIFT;
     }
     else
     {
        return 2;                             //发送帧为扩展帧
     }

     //写入数据段
     if (sendmsg->m_RTR == 0)                 //按数据帧填充
     {
        for (i = 0; i < sendmsg->m_dataLen; i++)
        {
            *((&(p->TEDSR[0]))+i) = sendmsg->m_data[i];
        }
        p->TDLR = sendmsg->m_dataLen;
     }
     else                                    //按远程帧填充
     {
         p->TDLR = 0;
     }

     //配置发送优先级
     p->TBPR = sendmsg->m_priority;

     //清TXEx标志，通知CAN模块发送
     p->CANTFLG = txEmptyBuf;

     return 0;                               //配置发送完成
}

//===========================================================================
//函数名称：CAN_Receive_Msg
//功能概要：接收CAN数据包函数
//参数说明：receiveFrame:存放待接收CAN数据包缓冲区的首地址
//函数返回：0 - 成功接收标准帧
//        1 - 未接收到帧
//        2 - 收到扩展帧
//===========================================================================
uint_8 CAN_Receive_Msg(CANMsg *recvmsg)
{
    uint_8 i;
    MSCAN_MemMapPtr p = (MSCAN_MemMapPtr) MSCAN_BASE_PTR;

    //检测接收标志
    if( (p->CANRFLG & 0x01) == 0)// (CAN0RFLG_RXF == 0)
    {
        return 1;  //未接收到帧
    }

    //判断标准帧/扩展帧
    if( (p->RSIDR1 & 0x04)  == 0)         //收到标准帧
    {
        recvmsg->m_ID = (uint_32)(p->RSIDR0<<3)
                           | (uint_32)(p->RSIDR1>>5);
        recvmsg->m_RTR = p->RSIDR1 & 0x08;
        recvmsg->m_IDE = 0;
    }
    else                                  //收到扩展帧
    {
        return 2;
    }

    //判断数据帧/远程帧
    if((p->RSIDR1 & 0x08) == 0)           //收到数据帧
    {
        recvmsg->m_dataLen = p->RDLR;     //数据长度
        //读取数据段内容
        for (i = 0; i < recvmsg->m_dataLen; i++)
        {
            recvmsg->m_data[i] = *((p->REDSR)+i);
        }
    }
    else                                 //收到远程帧
    {
        recvmsg->m_dataLen = 0;
    }

    //清RXF标志位，准备接收下一帧
    p->CANRFLG = 1;

    return 0;                             //成功接收标准帧
}
