
// #define NDEBUG  //禁用断言

#include "hi_3593.h"
#include "interface_layer.h"
#include <stdio.h>
#include <assert.h>
 #include <unistd.h>

// ------------------- 数据常量定义  ----------------------

#define WRITE_OP_CODE(x) (x & 0x7C) 		// 写操作 bit7置零
#define READ_OP_CODE(x) ((x | 0x80) & 0xFC) // 写操作 bit7置一

// ------------ ---- 调试函数   --------------------
// spi op code 对写入的数据进行读取，然后比较是否一致，不一致则断言
void debug_reg_read(spi_transfer handle, unsigned char* p, int len);

// 根据写地址查找读地址， 返回
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


// ------------ ---- 寄存器操作函数  --------------------
// 芯片复位
void chip_reset(spi_transfer handle)
{
	unsigned char op = WRITE_OP_CODE(HI3593_OP_MR);
	handle(&op, 1, NULL);
}

// 软复位
void soft_reset(spi_transfer handle)
{
	unsigned char op = WRITE_OP_CODE(HI3593_OP_SR);
	handle(&op, 1, NULL);
}

// 配置RX控制寄存器
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

// 配置TX控制寄存器
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

// 配置TX为高阻态
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

// 配置SELFTEST
void cfg_tx_selftest(spi_transfer handle)
{
	unsigned char op[2];
	unsigned char op1[2];
	TX_CONTROL_REG *dp;
	// 读取 当前配置
	op[0] = READ_OP_CODE(HI3593_OP_R_TX_CTRL);
	op[1] = 0;
	handle(op, 2, op1);

	op1[0] = WRITE_OP_CODE(HI3593_OP_W_TX_CTRL);
	dp = (TX_CONTROL_REG *) op1+1;
	dp->it.SELFTEST = 1;
	handle(op1, 2, NULL);
}


// 设置时钟分频：
// 外部确保数值为偶数
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

// 字节序转换
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

// 读 通道状态寄存器
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


// 读一条429消息，需外部保证FIFO非空
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


// 写一条429消息，需外部保证FIFO非空
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

// ----------------- 寄存器操作函数定义  --------------------

// 初始化 3593
void init_3593(DEVICE_3593 *node)
{
	// 芯片复位 MR
	chip_reset(node->spi_trans);
	// 写控制rx1寄存器
	cfg_rx_control(node->spi_trans, 1, node->rx1_rate);
	// 写控制rx2寄存器
	cfg_rx_control(node->spi_trans, 2, node->rx2_rate);
	// 写控制tx寄存器
	if(node->tx_en == 1){
		cfg_tx_control(node->spi_trans, node->oddeven, node->tx_rate);
	}
	else{
		cfg_tx_resistance(node->spi_trans);
	}

	// 设置时钟分频
	set_aclk(node->spi_trans, node->aclk_div);
}

// FIFO中 读出429指消息,
// p 读取的数组指针,确保数组长度 >=32
// 返回读出的条数，
int read_429(DEVICE_3593 *node, char ch, unsigned int *p)
{
	RX_STATUS_REG st;
	int i;
	for(i=0; i<32; ++i){
		st.bytes = ch_status(node->spi_trans, ch);
		if(st.it.FFEMPTY == 1)	// 读空 fifo
			break;
		p[i] = read_fifo(node->spi_trans, ch);
	}
	return i;
}

// 写429数据到FIFO,
// p 写数据指针，len 写入的429消息数
// 返回实际写入的条数
int write_429(DEVICE_3593 *node, unsigned int *p, int num)
{
	TX_STATUS_REG st;
	int i;
	for(i=0; i < num; ++i){
		st.bytes = ch_status(node->spi_trans, TX);
		if(st.it.FFFULL == 1)	// FIFO写满
			break;
		write_fifo(node->spi_trans, *(p+i));
	}
	return i;
}

// 芯片测试 test_loop
// 成功返回0
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
	usleep (20000); // 延时
	read_429(node, 1, (unsigned int*)data_rx);

	for(i=0; i<32; ++i)
		printf("%d %d\n", data[i].it.data, data_rx[i].it.data);
	return 0;
}
