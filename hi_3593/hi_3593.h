#ifndef _HI_3593_H
#define _HI_3593_H

#ifdef __cplusplus
extern "C"{
#endif

// 429 label 标签翻转控制，
// set 1 字节高低位翻转，符合c编码
#define FLIP    1


// ----------------- 寄存器地址定义 ----------------------
#define HI3593_OP_NULL              0x00 //W 0 Instruction not implemented. No operation.
#define HI3593_OP_MR                0x04 //W 0 Software controlled Master Reset
#define HI3593_OP_W_TX_CTRL         0x08 //W 1 Write Transmit Control Register
#define HI3593_OP_W_TX_FIFO         0x0C //W 4 Write ARINC 429 message to Transmit FIFO
#define HI3593_OP_W_RX1_CTRL        0x10 //W 1 Write Receiver 1 Control Register
#define HI3593_OP_W_RX1_LAB         0x14 //W 32 Write label values to Receiver 1 label memory. Starting with label 0xFF, consecutively set or reset each
                                         //     label in descending order. For example, if the first data byte is programmed to 10110010 then labels FF,
                                         //     FD FC and F9 will be set and FE, FB, FA and F8 will be reset.
#define HI3593_OP_W_RX1_PRIORITY    0x18 //W 3 Write Receiver 1 Priority-Label Match Registers. The data field consists of three eight-bit labels. The first data
                                         //     byte is written to P-L filter #3, the second to P-L filter #2, and the last byte to filter #1
#define HI3593_OP_W_RX2_CTRL        0x24 //W 1 Write Receiver 2 Control Register
#define HI3593_OP_W_RX2_LAB         0x28 //W 32 Write label values to Receiver 2 label memory. Starting with label 0xFF, consecutively set or reset each
                                         //    label in descending order. For example, if the first data byte is programmed to 10110010 then labels FF,
                                         //    FD FC and F9 will be set and FE, FB, FA and F8 will be reset.
#define HI3593_OP_W_RX2_PRIORITY    0x2C //W 3 Write Receiver 2 Priority-Label Match Registers. The data field consists of three eight-bit labels. The first
                                         //    eight bits is written to P-L filter #3, the second to P-L filter #2, and the last byte to filter #1
#define HI3593_OP_W_INT             0x34 //W 1 Write Flag / Interrupt Assignment Register
#define HI3593_OP_W_ACLK            0x38 //W 1 Write ACLK Division Register
#define HI3593_OP_W_TX_TRANS        0x40 //W 0 Transmit current contents of Transmit FIFO if Transmit Control Register bit 5 (TMODE) is a “0”
#define HI3593_OP_SR                0x44 //W 0 Software Reset. Clears the Transmit and Receive FIFOs and the Priority-Label Registers
#define HI3593_OP_W_RX1_LAB_SET     0x48 //W 0 Set all bits in Receiver 1 label memory to a “1”
#define HI3593_OP_W_RX2_LAB_SET     0x4C //W 0 Set all bits in Receiver 2 label memory to a “1”
#define HI3593_OP_R_TX_ST           0x80 //R 1 Read Transmit Status Register
#define HI3593_OP_R_TX_CTRL         0x84 //R 1 Read Transmit Control Register
#define HI3593_OP_R_RX1_ST          0x90 //R 1 Read Receiver 1 Status Register
#define HI3593_OP_R_RX1_CTRL        0x94 //R 1 Read Receiver 1 Control Register
#define HI3593_OP_R_RX1_LAB         0x98 //R 32 Read label values from Receiver 1 label memory.
#define HI3593_OP_R_RX1_PRIORITY    0x9C //R 3 Read Receiver 1 Priority-Label Match Registers.
#define HI3593_OP_R_RX1_FIFO        0xA0 //R 4 Read one ARINC 429 message from the Receiver 1 FIFO
#define HI3593_OP_R_RX1_PLI1        0xA4 //R 3 Read Receiver 1 Priority-Label Register #1, ARINC429 bytes 2,3 & 4 (bits 9 - 32)
#define HI3593_OP_R_RX1_PLI2        0xA8 //R 3 Read Receiver 1 Priority-Label Register #2, ARINC429 bytes 2,3 & 4 (bits 9 - 32)
#define HI3593_OP_R_RX1_PLI3        0xAC //R 3 Read Receiver 1 Priority-Label Register #3, ARINC429 bytes 2,3 & 4 (bits 9 - 32)
#define HI3593_OP_R_RX2_ST          0xB0 //R 1 Read Receiver 2 Status Register
#define HI3593_OP_R_RX2_CTRL        0xB4 //R 1 Read Receiver 2 Control Register
#define HI3593_OP_R_RX2_LAB         0xB8 //R 32 Read label values from Receiver 2 label memory.
#define HI3593_OP_R_RX2_PRIORITY    0xBC //R 3 Read Receiver 2 Priority-Label Match Registers.
#define HI3593_OP_R_RX2_FIFO        0xC0 //R 4 Read one ARINC 429 message from the Receiver 2 FIFO
#define HI3593_OP_R_RX2_PLI1        0xC4 //R 3 Read Receiver 2 Priority-Label Register #1, ARINC429 bytes 2,3 & 4 (bits 9 - 32)
#define HI3593_OP_R_RX2_PLI2        0xC8 //R 3 Read Receiver 2 Priority-Label Register #2, ARINC429 bytes 2,3 & 4 (bits 9 - 32)
#define HI3593_OP_R_RX2_PLI3        0xCC //R 3 Read Receiver 2 Priority-Label Register #3, ARINC429 bytes 2,3 & 4 (bits 9 - 32)
#define HI3593_OP_R_INT             0xD0 //R 1 Read Flag / Interrupt Assignment Register
#define HI3593_OP_R_ACLK            0xD4 //R 1 Read ACLK Division Register
#define HI3593_OP_NULL1             0xFF //R 0 Instruction not implemented. No operation
// ----------------- 寄存器地址定义 ----------------------

// ------------------- 寄存器定义  ----------------------
typedef union{
    struct{
        unsigned char RATE    :1;   // 0 set 1, low-speed 12.25Kbps; set 0, high-speed 100Kbps
        unsigned char PLON    :1;   // 1 set 1, Priority-Label Register enable
        unsigned char LABREC  :1;   // 2 set 1, message label filtering is enabled
        unsigned char PARITY  :1;   // 3 set 1, parity check, only match label input
        unsigned char SDON    :1;   // 4 set 1, 429 bits 9 and 10 match SD9 and SD10
        unsigned char SD10    :1;   // 5
        unsigned char SD9     :1;   // 6
        unsigned char RFLIP   :1;   // 7 set 1, 429 label reverses (spi rx)

    }it;
    unsigned char bytes;
}RX_CONTROL_REG;


typedef union{
    struct{
        unsigned char RATE     :1;   // 0 set 1, low-speed 12.25Kbps; set 0, high-speed 100Kbps
        unsigned char          :1;   // 1 X
        unsigned char TPARITY  :1;   // 2 set 1, message label filtering is enabled
        unsigned char ODDEVEN  :1;   // 3 set 1, transmitter inserts even bit; set 0 odd
        unsigned char SELFTEST :1;   // 4 set 1, set 1,looping-back the transmitter outputs to both receiver inputs for self-test purposes.
        unsigned char TMODE    :1;   // 5 set 1, data sent as soon as it is available; set 0, only op-code<0x40> enable send
        unsigned char TFLIP    :1;   // 6 set 1, 429 label reverses (spi tx)
        unsigned char HIZ      :1;   // 7 set 1, high-impedance state
    }it;
    unsigned char bytes;
}TX_CONTROL_REG;


typedef union{
    struct{
        unsigned char FFEMPTY  :1;   // 0 set 1,  FIFO is empty
        unsigned char FFHALF   :1;   // 1 set 1, FIFO contains at least 16 ARINC 429 messages
        unsigned char FFFULL   :1;   // 2 set 1, FIFO contains 32 ARINC 429 messages
        unsigned char PL1      :1;   // 3 set 1, received by Priority Label filter #1
        unsigned char PL2      :1;   // 4 set 1, received by Priority Label filter #2
        unsigned char PL3      :1;   // 5 set 1, received by Priority Label filter #3
        unsigned char          :1;   // 6 X
        unsigned char          :1;   // 7 X
    }it;
    unsigned char bytes;
}RX_STATUS_REG;

typedef union{
    struct{
        unsigned char FFEMPTY  :1;   // 0 set 1, FIFO is empty
        unsigned char FFHALF   :1;   // 1 set 1, FIFO contains at least 16 ARINC 429 messages
        unsigned char FFFULL   :1;   // 2 set 1, FIFO contains 32 ARINC 429 messages
        unsigned char          :1;   // 3 X
        unsigned char          :1;   // 4 X
        unsigned char          :1;   // 5 X
        unsigned char          :1;   // 6 X
        unsigned char          :1;   // 7 X
    }it;
    unsigned char bytes;
}TX_STATUS_REG;

typedef union{
    struct{
        unsigned char DIV      :5;   // 4-0 ACLK division ratio eg: 10M =>1M DIV = 10
        unsigned char          :1;   // 5 X
        unsigned char          :1;   // 6 X
        unsigned char          :1;   // 7 X
    }it;
    unsigned char bytes;
}ACLK_DIVISION_REG;

typedef union{
    struct{
        unsigned char R1FLAG   :2;   // 1-0 flag,[]
        unsigned char R1INT    :2;   // 3-2 Interrupt
        unsigned char R2FLAG   :2;   // 5-4 flag,[]
        unsigned char R2INT    :2;   // 7-6 Interrupt
    }it;
    unsigned char bytes;
}INTERRUPT_REG;

// [00: message, box #1, box #2, box #3] [01: box #1] [02: box #2] [03: box #3]
enum INTERRUPT_TYPE{
    ALL =0, BOX1, BOX2, BOX3
};

// [00: FIFO is empty] [01: FIFO is full] [02: FIFO is half-full] [03: FIFO is not empty]
enum FLAG_TYPE{
    EMPTY=0, FULL, HALF_FULL, NOT_EMPTY
};

// 速率
enum RATE_TYPE{
	R_100K = 0, R_12_5K = 1
};

// 奇偶校验
enum CHECK_TYPE{
	ODD = 0, EVEN = 1
};

// 通道
enum CHANNEL_TYPE{
	TX = 0, RX1 = 1, RX2 = 2
};

// 定义spi设备读写层接口 （spi全双工模式）
// tx_p: 发送的指针，len：发送字节长度，  rx_p: 接收的指针
typedef void (*spi_transfer)(unsigned char* tx_p, int len, unsigned char* rx_p);

// 429数据
typedef union{
    struct{
        unsigned int label   :8;
        unsigned int sdi     :2;
        unsigned int data    :18;
        unsigned int ssm     :3;
        unsigned int oddeven :1;
    }it;
    unsigned int word;
}MSG_429;

// 设备句柄
typedef struct _DEVICE_3593{
	char id;					// 设备编号
    char tx_en;                 // 1 使能发送， 0 关闭发送 [HIZ set 1]
	char aclk_div;				// ACLK时钟分频 1,2,4,6,8...30
	enum CHECK_TYPE oddeven;    // 发送的奇偶校验
	enum RATE_TYPE tx_rate;		// 发送的速率
	enum RATE_TYPE rx1_rate;	// 接收1的速率
	enum RATE_TYPE rx2_rate;	// 接收2的速率

	spi_transfer spi_trans;		// spi收发函数指针
}DEVICE_3593;


// ------------------- 寄存器定义  ----------------------

// -------------------- 函数定义   ----------------------

// 初始化 3593
void init_3593(DEVICE_3593 *node);

// FIFO中 读出429指消息,
// ch 通道， p 读取的数组指针,确保数组长度 >=32
// 返回读出的条数，
int read_429(DEVICE_3593 *node, char ch, unsigned int *p);

// 写429数据到FIFO,
// p 写数据指针，len 写入的429消息数
// 返回实际写入的条数
int write_429(DEVICE_3593 *node, unsigned int *p, int num);

// 芯片测试 test_loop
// 成功返回0
int test_loop_429(DEVICE_3593 *node);

// -------------------- 函数定义   ----------------------




#ifdef __cplusplus
}
#endif
#endif
