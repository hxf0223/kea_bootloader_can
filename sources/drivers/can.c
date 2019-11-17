//============================================================================
//�ļ����ƣ�can.c
//���ܸ�Ҫ��can�ײ���������Դ�ļ�
//��Ȩ���У����ݴ�ѧ��˼����Ƕ��ʽ����(sumcu.suda.edu.cn)
//���¼�¼��2015-5-11  V1.5
//оƬ���ͣ�KEA128
//============================================================================
#include "can.h"

//===========================================================================
//�������ƣ�CAN_Init
//���ܸ�Ҫ����ʼ��MSCANģ�飬����24MHz����ʱ����Ϊģ��ʱ�ӣ������ʹ̶�Ϊ500Kbps
//����˵����mode:����ģʽѡ��:
//        LOOP_MODE(0)   - �ػ�ģʽ
//        NORMAL_MODE(1) - ��ͨģʽ
//        open_filter:���չ���������
//          1 - ������������ֻ���ձ�ʶ��IDΪacc_id��֡
//          0 - �رչ���������������֡
//        acc_id:����ID�����չ�����ֻ���ձ�ʶ�����ϴ�ID��֡����������ID��֡
//�������أ���
//===========================================================================
void mscan_init(uint_8 mode,uint_8 open_filter,uint_32 acc_id, can_baud_rate_e baudRate)
{
	MSCAN_MemMapPtr p = (MSCAN_MemMapPtr) MSCAN_BASE_PTR;

    //MSCAN�����Ÿ���ѡ��CAN_TXΪPTC7��CAN_RXΪPTC6
    SIM_PINSEL1 &= (uint_32)~(uint_32)(SIM_PINSEL1_MSCANPS_MASK);
    //MSCANģ��ʱ���ſ��ƣ�SIM_SCGC��MSCANλ=1��ʹ�����߸�MSCANʱ�ӣ�
    //SIM_SCGC��MSCANλ=0����ֹ���߸�MSCANʱ��
    SIM_SCGC |=(uint_32)(SIM_SCGC_MSCAN_MASK);

    //MSCAN12ģ��ʹ��
    p->CANCTL1 |= MSCAN_CANCTL1_CANE_MASK;

    //��������ʼ��ģʽ
    p->CANCTL0 |= MSCAN_CANCTL0_INITRQ_MASK;

    //�ȴ�Ӧ������ʼ��ģʽ
    while ((p->CANCTL1 & MSCAN_CANCTL1_INITAK_MASK)== 0);

    if(open_filter)         //�������չ�����,ֻ���ձ�ʾ��Ϊacc_id��֡
    {
        // ʹ��2��32λ���չ�����
        p->CANIDAC &= ~MSCAN_CANIDAC_IDAM_MASK;

        // ��1��32λ�˲���
        // ���ý��ܼĴ�����ֵ
        p->CANIDAR_BANK_1[0]=acc_id>>3;
        p->CANIDAR_BANK_1[1]=acc_id<<5;
        p->CANIDAR_BANK_1[2]=0xFF;
        p->CANIDAR_BANK_1[3]=0xFF;

        // �������μĴ�����ֵ
        p->CANIDMR_BANK_1[0]=0x00;
        p->CANIDMR_BANK_1[1]=0x1F;
        p->CANIDMR_BANK_1[2]=0xFF;
        p->CANIDMR_BANK_1[3]=0xFF;

        // ��2��32λ�˲���
        // ���ý��ܼĴ�����ֵ
        p->CANIDAR_BANK_2[0]=acc_id>>3;
        p->CANIDAR_BANK_2[1]=acc_id<<5;
        p->CANIDAR_BANK_2[2]=0xFF;
        p->CANIDAR_BANK_2[3]=0xFF;

        // �������μĴ�����ֵ
        p->CANIDMR_BANK_2[0]=0x00;
        p->CANIDMR_BANK_2[1]=0x1F;
        p->CANIDMR_BANK_2[2]=0xFF;
        p->CANIDMR_BANK_2[3]=0xFF;
    }
    else//�رչ�����
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

    //����ʱ��
    p->CANCTL1 |= MSCAN_CANCTL1_CLKSRC_MASK;    // ��������ʱ��(24MHz)��Ϊģ��ʱ��
    p->CANBTR0 |= MSCAN_CANBTR0_SJW(0);
    p->CANBTR1 |= MSCAN_CANBTR1_SAMP_MASK;     // λʱ���������Ϊ3

	switch (baudRate) {
	case can_baud_rate_500k: // baudrate = ģ��ʱ�� / [(1 + TSEG1 + TSEG2) * ��Ƶ����] = 20000K/[(1+13+6)*2]=500Kbps
		p->CANBTR0 |= MSCAN_CANBTR0_BRP(1);		// ģ��ʱ��2��Ƶ
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG2(5);	// TSEG2:5+1=6 Tq
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG1(12);	// TSEG1:12+1=13 Tq
		break;

	case can_baud_rate_125k:
		p->CANBTR0 |= MSCAN_CANBTR0_BRP(0x1A);	// ģ��ʱ��27��Ƶ
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG2(0x01);// TSEG2:1+1=6 Tq
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG1(0x03);// TSEG1:3+1=13 Tq
		break;

	default:	// default to 250k
		p->CANBTR0 |= MSCAN_CANBTR0_BRP(0x0D);	// ģ��ʱ��14��Ƶ
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG2(0x01);// TSEG2:1+1=6 Tq
		p->CANBTR1 |= MSCAN_CANBTR1_TSEG1(0x03);// TSEG1:3+1=13 Tq
		break;
	}

    //���ù���ģʽ
    if(LOOP_MODE == mode)
        p->CANCTL1 |= MSCAN_CANCTL1_LOOPB_MASK; // �����ػ�ģʽ
    p->CANCTL1 &= ~MSCAN_CANCTL1_LISTEN_MASK;   // ��ֹ����ģʽ������������ģʽ

    //�˳���ʼ��ģʽ
    p->CANCTL0 &= ~MSCAN_CANCTL0_INITRQ_MASK;   //�˳���ʼ��ģʽ����������ģʽ

    //�����жϷ�ʽ
    p->CANTIER = 0x00;                          //��ֹ�����ж�
    p->CANRIER = 0x00;                          //�����������ж�
    //����Ƕ�������жϿ��ƼĴ���ֵ��ʹ��CAN�����ж�
    //NVIC_EnableIRQ(30);

    //�ȴ�Ӧ���ʼ��ģʽ
    while ((p->CANCTL1 & MSCAN_CANCTL1_INITAK_MASK) == 1){}

    //�ȴ�����ͨ��ʱ��ͬ��
    while ((p->CANCTL0 & MSCAN_CANCTL0_SYNCH_MASK) == 0){}
}

//===========================================================================
//�������ƣ�CAN_Fill_STD_Msg
//���ܸ�Ҫ�����һ��CAN���ݰ�
//����˵����message:������CAN���ݰ��ṹ����׵�ַ
//        id:������CAN��׼֡�ı�ʶ��ID����ǰ�ڵ�ID��
//        data:���������ݻ������׵�ַ
//        len:���������ݳ��ȣ�<=8 Bytes��
//�������أ�0 - ���ɹ�
//        1 - ���ݳ����������
//===========================================================================
uint_8 CAN_Fill_STD_Msg(CANMsg *message,uint_32 id,uint_8 *data,uint_8 len)
{
    uint_8 i;

    message->m_ID = id;
    message->m_IDE = 0;
    message->m_RTR = 0;
    message->m_priority = 0;                      //���÷��ͻ��������ȼ�

    if ( len > 8 ) {
        return 1;                                 //���ݳ����������
    }

    message->m_dataLen = len;
    for ( i = 0; i < message->m_dataLen; i++ ) {
        message->m_data[i] = data[i];
    }

    return 0;                                     //���ɹ�
}

//===========================================================================
//�������ƣ�CAN_Send_Msg
//���ܸ�Ҫ��CAN�������ݰ�����
//����˵����sendmsg:�����͵�CAN���ݰ����׵�ַ
//�������أ�0 - ���ͳɹ�
//        1 - ����֡���ȴ���
//        2 - ����֡Ϊ��չ֡
//===========================================================================
uint_8 CAN_Send_Msg(CANMsg *sendmsg)
{
     uint_8 txEmptyBuf;                      //���л���������
     uint_8 i;
     MSCAN_MemMapPtr p = (MSCAN_MemMapPtr) MSCAN_BASE_PTR;

     //������ݳ���
     if (sendmsg->m_dataLen > 8)
     {
        return 1;                            //����֡���ȴ���
     }

     //��������Ƿ�ͬ��
     while(!(p->CANCTL0 & MSCAN_CANCTL0_SYNCH_MASK));

     //Ѱ�ҿ��л�����
     txEmptyBuf = 0;
     do
     {
         p->CANTBSEL = p->CANTFLG;
         txEmptyBuf = p->CANTBSEL;
     }
     while (!txEmptyBuf);

     //д���ʶ��ID
     if (sendmsg->m_IDE == 0)                //����׼֡���ID
     {
         p->TSIDR0 = (uint_8)(sendmsg->m_ID>>3);
         p->TSIDR1 = (uint_8)(sendmsg->m_ID<<5);
         p->TSIDR1 |= (sendmsg->m_RTR)<<MSCAN_TSIDR1_TSRTR_SHIFT;
         p->TSIDR1 |= (sendmsg->m_IDE)<<MSCAN_TSIDR1_TSIDE_SHIFT;
     }
     else
     {
        return 2;                             //����֡Ϊ��չ֡
     }

     //д�����ݶ�
     if (sendmsg->m_RTR == 0)                 //������֡���
     {
        for (i = 0; i < sendmsg->m_dataLen; i++)
        {
            *((&(p->TEDSR[0]))+i) = sendmsg->m_data[i];
        }
        p->TDLR = sendmsg->m_dataLen;
     }
     else                                    //��Զ��֡���
     {
         p->TDLR = 0;
     }

     //���÷������ȼ�
     p->TBPR = sendmsg->m_priority;

     //��TXEx��־��֪ͨCANģ�鷢��
     p->CANTFLG = txEmptyBuf;

     return 0;                               //���÷������
}

//===========================================================================
//�������ƣ�CAN_Receive_Msg
//���ܸ�Ҫ������CAN���ݰ�����
//����˵����receiveFrame:��Ŵ�����CAN���ݰ����������׵�ַ
//�������أ�0 - �ɹ����ձ�׼֡
//        1 - δ���յ�֡
//        2 - �յ���չ֡
//===========================================================================
uint_8 CAN_Receive_Msg(CANMsg *recvmsg)
{
    uint_8 i;
    MSCAN_MemMapPtr p = (MSCAN_MemMapPtr) MSCAN_BASE_PTR;

    //�����ձ�־
    if( (p->CANRFLG & 0x01) == 0)// (CAN0RFLG_RXF == 0)
    {
        return 1;  //δ���յ�֡
    }

    //�жϱ�׼֡/��չ֡
    if( (p->RSIDR1 & 0x04)  == 0)         //�յ���׼֡
    {
        recvmsg->m_ID = (uint_32)(p->RSIDR0<<3)
                           | (uint_32)(p->RSIDR1>>5);
        recvmsg->m_RTR = p->RSIDR1 & 0x08;
        recvmsg->m_IDE = 0;
    }
    else                                  //�յ���չ֡
    {
        return 2;
    }

    //�ж�����֡/Զ��֡
    if((p->RSIDR1 & 0x08) == 0)           //�յ�����֡
    {
        recvmsg->m_dataLen = p->RDLR;     //���ݳ���
        //��ȡ���ݶ�����
        for (i = 0; i < recvmsg->m_dataLen; i++)
        {
            recvmsg->m_data[i] = *((p->REDSR)+i);
        }
    }
    else                                 //�յ�Զ��֡
    {
        recvmsg->m_dataLen = 0;
    }

    //��RXF��־λ��׼��������һ֡
    p->CANRFLG = 1;

    return 0;                             //�ɹ����ձ�׼֡
}
