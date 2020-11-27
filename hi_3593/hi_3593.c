
// #define NDEBUG  //���ö���

#include "hi_3593.h"
#include "interface_layer.h"
#include <stdio.h>
#include <assert.h>
 #include <unistd.h>

// ------------------- ���ݳ�������  ----------------------

#define WRITE_OP_CODE(x) (x & 0x7C) 		// д���� bit7����
#define READ_OP_CODE(x) ((x | 0x80) & 0xFC) // д���� bit7��һ

// ------------ ---- ���Ժ���   --------------------
// spi op code ��д������ݽ��ж�ȡ��Ȼ��Ƚ��Ƿ�һ�£���һ�������
void debug_reg_read(spi_transfer handle, unsigned char* p, int len);

// ����д��ַ���Ҷ���ַ�� ����
unsigned char find_read_reg(unsigned char op_w);

inline unsigned char find_read_reg(unsigned char op_w)
{
	switch(op_w){
    	case HI3593_OP_W_TX_CTRL      :    return HI3593_OP_R_TX_CTRL      ;
    	case HI3593_OP_W_RX1_CTRL     :    return HI3593_OP_R_RX1_CTRL     ;
    	case HI3593_OP_W_RX1_LAB      :    return HI3593_OP_R_RX1_LAB      ;
    	case HI3593_OP_W_RX1_PRIORITY :    return HI3593_OP_R_RX1_PRIORITY ;
    	case HI3593_OP_W_RX2_CTRL     :    return HI3593_OP_R_RX2_CTRL     ;
    	case HI3593_OP_W_RX2_LAB      :    return HI3593_OP_R_RX2_LAB      ;
    	case HI3593_OP_W_RX2_PRIORITY :    return HI3593_OP_R_RX2_PRIORITY ;
    	case HI3593_OP_W_INT          :    return HI3593_OP_R_INT          ;
    	case HI3593_OP_W_ACLK         :    return HI3593_OP_R_ACLK         ;
    	default: return HI3593_OP_NULL;
	}

}

inline void debug_reg_read(spi_transfer handle, unsigned char* p, int len)
#ifdef NDEBUG
{;}
#else
{
	unsigned char data[32];
	unsigned char op[32];
	int i;

	op[0]= READ_OP_CODE(find_read_reg(p[0]));
	handle(op, len, data);

	for(i=1;i<len; ++i){
		if(data[i] != *(p+i)){
			//spi_close();
			printf("spi read err close spi");
		}
		assert(data[i] == *(p+i));
	}
}
#endif


// ------------ ---- �Ĵ�����������  --------------------
// оƬ��λ
void chip_reset(spi_transfer handle)
{
	unsigned char op = WRITE_OP_CODE(HI3593_OP_MR);
	handle(&op, 1, NULL);
}

// ��λ
void soft_reset(spi_transfer handle)
{
	unsigned char op = WRITE_OP_CODE(HI3593_OP_SR);
	handle(&op, 1, NULL);
}

// ����RX���ƼĴ���
void cfg_rx_control(spi_transfer handle, int ch, enum RATE_TYPE rate)
{
	unsigned char op[2];
	RX_CONTROL_REG reg;
	reg.it.RFLIP = FLIP;
	reg.it.SD9   = 0;
	reg.it.SD10  = 0;
	reg.it.SDON  = 0;
	reg.it.PARITY= 0;
	reg.it.LABREC= 0;
	reg.it.PLON  = 0;
	reg.it.RATE  = rate;

	if(ch == 1)
		op[0] = WRITE_OP_CODE(HI3593_OP_W_RX1_CTRL);
	else if(ch == 2)
		op[0] = WRITE_OP_CODE(HI3593_OP_W_RX2_CTRL);
	else
		op[0] = WRITE_OP_CODE(HI3593_OP_NULL);

	op[1] = reg.bytes;
	handle(op, 2, NULL);

	debug_reg_read(handle, op, 2);

}

// ����TX���ƼĴ���
void cfg_tx_control(spi_transfer handle, enum CHECK_TYPE oddeven, enum RATE_TYPE rate)
{
	unsigned char op[2];
	TX_CONTROL_REG reg;
	reg.it.HIZ      = 0;
	reg.it.TFLIP     = FLIP;
	reg.it.TMODE    = 1; // data sent immediately
	reg.it.SELFTEST = 0;
	reg.it.ODDEVEN  = oddeven;
	reg.it.TPARITY  = 0;
	reg.it.RATE  = rate;


	op[0] = WRITE_OP_CODE(HI3593_OP_W_TX_CTRL);
	op[1] = reg.bytes;
	handle(op, 2, NULL);

	debug_reg_read(handle, op, 2);
}

// ����TXΪ����̬
void cfg_tx_resistance(spi_transfer handle)
{
	unsigned char op[2];
	TX_CONTROL_REG reg;
	reg.it.HIZ      = 1;
	reg.it.TFLIP    = 0;
	reg.it.TMODE    = 1; // data sent immediately
	reg.it.SELFTEST = 0;
	reg.it.ODDEVEN  = 0;
	reg.it.TPARITY  = 0;
	reg.it.RATE     = 0;

	op[0] = WRITE_OP_CODE(HI3593_OP_W_TX_CTRL);
	op[1] = reg.bytes;
	handle(op, 2, NULL);

	debug_reg_read(handle, op, 2);
}

// ����SELFTEST
void cfg_tx_selftest(spi_transfer handle)
{
	unsigned char op[2];
	unsigned char op1[2];
	TX_CONTROL_REG *dp;
	// ��ȡ ��ǰ����
	op[0] = READ_OP_CODE(HI3593_OP_R_TX_CTRL);
	op[1] = 0;
	handle(op, 2, op1);

	op1[0] = WRITE_OP_CODE(HI3593_OP_W_TX_CTRL);
	dp = (TX_CONTROL_REG *) op1+1;
	dp->it.SELFTEST = 1;
	handle(op1, 2, NULL);
}


// ����ʱ�ӷ�Ƶ��
// �ⲿȷ����ֵΪż��
void set_aclk(spi_transfer handle, char aclk_div)
{
	unsigned char op[2];
	ACLK_DIVISION_REG reg;
	reg.bytes = 0;
	reg.it.DIV = aclk_div;

	op[0] = WRITE_OP_CODE(HI3593_OP_W_ACLK);
	op[1] = reg.bytes;
	handle(op, 2, NULL);

	debug_reg_read(handle, op, 2);
}

// �ֽ���ת��
unsigned int sw_order(unsigned int data)
{
	unsigned char tmp[4];
	unsigned char* p = (unsigned char*) &data;
	tmp[3] = *(p++);
	tmp[2] = *(p++);
	tmp[1] = *(p++);
	tmp[0] = *p;

	return *((unsigned int*) tmp);
}

// �� ͨ��״̬�Ĵ���
unsigned char ch_status(spi_transfer handle, char ch)
{
	unsigned char op[2];
	unsigned char data[2];
	switch((enum CHANNEL_TYPE)ch){
		case TX: 	op[0] = READ_OP_CODE(HI3593_OP_R_TX_ST); break;
		case RX1: 	op[0] = READ_OP_CODE(HI3593_OP_R_RX1_ST); break;
		case RX2: 	op[0] = READ_OP_CODE(HI3593_OP_R_RX2_ST); break;
		default:	op[0] = READ_OP_CODE(HI3593_OP_NULL); break;
	}
	op[1] = 0;
	handle(op, 2, data);
	return data[1];
}


// ��һ��429��Ϣ�����ⲿ��֤FIFO�ǿ�
unsigned int read_fifo(spi_transfer handle, char ch)
{
	unsigned char data[5];
	unsigned char op[5] = {0};
	if(ch == 1)
		op[0] = READ_OP_CODE(HI3593_OP_R_RX1_FIFO);
	else if(ch == 2)
		op[0] = READ_OP_CODE(HI3593_OP_R_RX2_FIFO);
	handle(op, 5, data);
	return *((unsigned *)(data+1));
}


// дһ��429��Ϣ�����ⲿ��֤FIFO�ǿ�
void write_fifo(spi_transfer handle, unsigned int data)
{
	unsigned char op[5] = {0};
	unsigned char* p = (unsigned char*) &data;
	op[0] = WRITE_OP_CODE(HI3593_OP_W_TX_FIFO);
	op[4] = *(p++);
	op[3] = *(p++);
	op[2] = *(p++);
	op[1] = *p;

	handle(op, 5, NULL);
}

// ----------------- �Ĵ���������������  --------------------

// ��ʼ�� 3593
void init_3593(DEVICE_3593 *node)
{
	// оƬ��λ MR
	chip_reset(node->spi_trans);
	// д����rx1�Ĵ���
	cfg_rx_control(node->spi_trans, 1, node->rx1_rate);
	// д����rx2�Ĵ���
	cfg_rx_control(node->spi_trans, 2, node->rx2_rate);
	// д����tx�Ĵ���
	if(node->tx_en == 1){
		cfg_tx_control(node->spi_trans, node->oddeven, node->tx_rate);
	}
	else{
		cfg_tx_resistance(node->spi_trans);
	}

	// ����ʱ�ӷ�Ƶ
	set_aclk(node->spi_trans, node->aclk_div);
}

// FIFO�� ����429ָ��Ϣ,
// p ��ȡ������ָ��,ȷ�����鳤�� >=32
// ���ض�����������
int read_429(DEVICE_3593 *node, char ch, unsigned int *p)
{
	RX_STATUS_REG st;
	int i;
	for(i=0; i<32; ++i){
		st.bytes = ch_status(node->spi_trans, ch);
		if(st.it.FFEMPTY == 1)	// ���� fifo
			break;
		p[i] = read_fifo(node->spi_trans, ch);
	}
	return i;
}

// д429���ݵ�FIFO,
// p д����ָ�룬len д���429��Ϣ��
// ����ʵ��д�������
int write_429(DEVICE_3593 *node, unsigned int *p, int num)
{
	TX_STATUS_REG st;
	int i;
	for(i=0; i < num; ++i){
		st.bytes = ch_status(node->spi_trans, TX);
		if(st.it.FFFULL == 1)	// FIFOд��
			break;
		write_fifo(node->spi_trans, *(p+i));
	}
	return i;
}

// оƬ���� test_loop
// �ɹ�����0
int test_loop_429(DEVICE_3593 *node)
{
	int i;
	MSG_429 data[32];
	MSG_429 data_rx[32];
	int label = 1;
	int num = 32;

	for(i=0; i<num; ++i){
		data[i].it.data = i;
		data[i].it.label = label;
	}

	cfg_tx_selftest(node->spi_trans);

	write_429(node, (unsigned int*)data, num);
	usleep (20000); // ��ʱ
	read_429(node, 1, (unsigned int*)data_rx);

	for(i=0; i<32; ++i)
		printf("%d %d\n", data[i].it.data, data_rx[i].it.data);
	return 0;
}
