#ifndef __NFC_FM11NT081D_H__
#define __NFC_FM11NT081D_H__

#define TCFG_FM11NT081D_DEV_ENABLE    0
#define TCFG_FM11NT081D_USER_IIC_TYPE 1

#if TCFG_FM11NT081D_DEV_ENABLE
#define FM11NT081D_RADDR        0xaf
#define FM11NT081D_WADDR        0xae

#define ON        		1
#define OFF		       	0

#define FUNC_RATS		0
#define FUNC_PPSS 		1
#define FUNC_I_BLOCK 	2
#define FUNC_R_BLOCK 	3
#define FUNC_S_BLOCK	4
#define FUNC_PDTP		5

/////寄存器地址(spi)以及相关定义
#define FIFO_ACCESS		0x00		//MCU读写FIFO的入口地址，SPI读写FIFO使用专用指令码
#define FIFO_FLUSH		0x01		//供MCU清空FIFO内容
#define FIFO_WORDCNT	0x02		//供MCU查询FIFO剩余字节
#define RF_STATUS		  0x03		//非接触口工作状态
#define RF_TXEN			  0x04		//FIFO访问寄存器
#define RF_BAUD			  0x05		//非接通信波特率选择
#define RF_RATS			  0x06		//非接通信收到的RATS数据
#define MAIN_IRQ		  0x07		//主要中断标志寄存器
#define FIFO_IRQ		  0x08		//FIFO中断标志寄存器
#define AUX_IRQ			  0x09		//辅助中断标志寄存器
#define MAIN_IRQ_MASK	0x0A		//主中断屏蔽寄存器
#define FIFO_IRQ_MASK	0x0B		//FIFO中断屏蔽寄存器
#define AUX_IRQ_MASK	0x0C		//辅助中断屏蔽寄存器
#define NFC_CFG			  0x0D		//NFC配置寄存器
#define REGU_CFG		  0x0E		//Regulator配置寄存器

//#define REG_RANGE	REGU_CFG		//寄存器取值范围
#define CS_DELAY          1000
#define MAIN_IRQ_RF_PWON  0x80
#define MAIN_IRQ_ACTIVE   0x40
#define MAIN_IRQ_RX_START 0x20
#define MAIN_IRQ_RX_DONE  0x10
#define MAIN_IRQ_TX_DONE  0x08
#define MAIN_IRQ_ARBIT    0x04
#define MAIN_IRQ_FIFO     0x02
#define MAIN_IRQ_AUX      0x01

#define FIFO_IRQ_WL      0x08
#define FIFO_IRQ_OVFLOW  0x04
#define FIFO_IRQ_FULL    0x02
#define FIFO_IRQ_EMPTY   0x01

#define TYPE_IRQ_MAIN 0
#define TYPE_IRQ_FIFO 1
#define TYPE_IRQ_AUX  2

//iic模式:传输速度：standard-mode(100kbps)、fast-mode(400kbps)、fast-mode plus(1mbps)
//spi模式:频率<=5MHz。时序:默认CPOL=0,CPHA=1; 可修改为CPOL=1,CPHA=1(资料未介绍)
struct fm11nt081d_platform_data {
    u8 comms;//0:IIC,  1:SPI
    u8 csn_port_io;//模块上电
    u8 fd_port_io;//中断检测
    u8 hdl;//iic_hdl  or spi_hdl
    u8 iic_delay; //这个延时并非影响iic的时钟频率，而是2Byte数据之间的延时.单位：ms
};


//iic
bool fm11nt08_init(void *priv);
void fm11nt08_writereg(u8 reg, u8 val);
u8 fm11nt08_readreg(u8 reg);
void fm11nt08_writefifo(u8 *wbuf, u32 len);
void fm11nt08_readfifo(u8 *rbuf, u32 len);
void fm11nt08_writeE2(u8 *addr, u8 *wbuf, u32 len);
void fm11nt08_readE2(u8 *addr, u8 *rbuf, u32 len);
void fm11nt08_RFDataTx(u8 *ibuf, u32 ilen);
unsigned int fm11nt08_RFDataRx(unsigned char *rbuf);
void fm11nt08_Clearfifo(void);
unsigned char fm11nt08_GetStatus(void);
unsigned char fm11nt08_ChkIrqInfo(u8 irq, u8 reg);
unsigned char fm11nt08_fifo_func(unsigned char ibuf);
void fm11nt08_SetRatsCfg(unsigned char rats);

//spi

#endif
#endif
