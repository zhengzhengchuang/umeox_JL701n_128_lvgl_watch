#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".iic.bss")
#pragma data_seg(".iic.data")
#pragma const_seg(".iic.const")
#pragma code_seg(".iic.text")
#pragma str_literal_override(".iic.text")
#endif /* #ifdef SUPPORT_MS_EXTENSIONS */

#include "includes.h"
#include "gpio.h"
#include "iic_hw.h"

/* #pragma bss_seg(".iic") */
/* #pragma const_seg(".iic") */
/* #pragma code_seg(".iic") */
/*  */


/**********************iic io config************************/
#define IIC_SCL_IO 			IO_PORTB_04//PB组
#define IIC_SDA_IO 			IO_PORTB_05//PB组
/* #define iic_io_hdrive       0//p11 have no hd */
#define iic_io_filt         1
#define iic_io_pu           1//1:内部上拉

/**********************iic master config************************/
#define iic_master_baud 	200000//波特率, 单位:HZ
#define iic_role_slave_en   0//1:slave, 0:master

/**********************iic slave config************************/
#define IIC_SLAVE_ADDR 		0b1010110

static u32 iic_get_clk_freq(void)
{
    return 10000000;
}

//=========================================================================//
//                              IIC MASTER                                 //
//=========================================================================//

static void iic_slave_isr(void);
int hw_iic_init(void)
{
    u32 baud = 0;
    IIC_CON0_CLEAR();
    IIC_CON1_CLEAR();
    if (iic_io_filt) { //
        IIC_IO_FILTER_INPUT_ENABLE(1);
    } else {
        IIC_IO_FILTER_INPUT_ENABLE(0);
    }

    //SCL:
    gpio_set_die(IIC_SCL_IO, 1);
    gpio_set_output_value(IIC_SCL_IO, 1);
    gpio_set_fun_input_port(IIC_SCL_IO, PFI_IIC_SCL);
    gpio_set_fun_output_port(IIC_SCL_IO, P11_FO_IIC_SCL, 1, 1);
    //SDA:
    gpio_set_die(IIC_SDA_IO, 1);
    gpio_set_fun_output_port(IIC_SDA_IO, P11_FO_IIC_SDA, 1, 1);
    gpio_set_fun_input_port(IIC_SDA_IO, PFI_IIC_SDA);
    if (iic_io_pu) {
        gpio_set_pull_up(IIC_SDA_IO, 1);
        gpio_set_pull_down(IIC_SDA_IO, 0);
        gpio_set_pull_up(IIC_SCL_IO, 1);
        gpio_set_pull_down(IIC_SCL_IO, 0);
    } else {
        gpio_set_pull_up(IIC_SDA_IO, 0);
        gpio_set_pull_down(IIC_SDA_IO, 0);
        gpio_set_pull_up(IIC_SCL_IO, 0);
        gpio_set_pull_down(IIC_SCL_IO, 0);
    }

    if (iic_role_slave_en) { //slave
        IIC_SLAVE_MODE();
        IIC_SLAVE_IGNORE_BUS_MODE(1);
        IIC_SLAVE_ADDR_CONFIG(IIC_SLAVE_ADDR);
        IIC_END_PENDING_INT_ENABLE(1);
        //无byte & start pending int enable?
        INTALL_HWI(IRQ_IIC_IDX, iic_slave_isr, IRQ_IIC_IP);
        IIC_INT_ENABLE(1);
    } else {
        IIC_MASTER_MODE();
        baud = iic_get_clk_freq() / (iic_master_baud - 1) / 2 - 1;
        IIC_MASTER_BAUD_CONFIG(baud);
    }

    IIC_BYTE_PENDING_CLEAR();
    IIC_END_PENDING_CLEAR();
    IIC_START_PENDING_CLEAR();
    IIC_MODULE_ENABLE(1);

    if (iic_role_slave_en) { //slave
        IIC_RECEIVE_ACK_SET(0);
        IIC_DIR_RECEIVE();
        IIC_WRITE_BUF(0xFF);
        delay(10);
        IIC_CONFIG_DONE();
    }
#if 0
    printf("\nP11_IIC->CON0 = 0x%x\n", P11_IIC->CON0);
    printf("P11_IIC->CON1 = 0x%x\n", P11_IIC->CON1);
    printf("baud = 0x%x\n", baud);
    printf("P11_IIC->BAUD = 0x%x\n", P11_IIC->BAUD);
#endif
    return 0;
}

void hw_iic_close(void)
{
    gpio_set_die(IIC_SCL_IO, 0);
    gpio_set_pull_up(IIC_SCL_IO, 0);
    gpio_set_die(IIC_SDA_IO, 0);
    gpio_set_pull_up(IIC_SDA_IO, 0);
    IIC_MODULE_ENABLE(0);
}

void hw_iic_start(void)
{
    /* printf("%s\n", __func__); */
    IIC_MASTER_RESTART_SET(1);
}

void hw_iic_stop(void)
{
    /* printf("%s\n", __func__); */
    IIC_MASTER_END_SET(1);
    IIC_CONFIG_DONE();
    while (!(IIC_END_PENDING_IS()));
    IIC_END_PENDING_CLEAR();
}


u8 hw_iic_master_tx_byte(u8 data)
{
    int timeout = 10000;
    local_irq_disable();

    IIC_END_PENDING_CLEAR();
    IIC_BYTE_PENDING_CLEAR();

    IIC_DIR_TRANSMIT();
    IIC_WRITE_BUF(data);
    delay(1);
    IIC_CONFIG_DONE();

    do {
        asm("csync");
    } while (--timeout && !(IIC_BYTE_PENDING_IS()));

    IIC_BYTE_PENDING_CLEAR();

    IIC_MASTER_RESTART_SET(0);
    IIC_MASTER_END_SET(0);
    local_irq_enable();

    if (!timeout) {
        return 0;
    }
    return !(IIC_TRANSMIT_ACK_IS());
}

u8 hw_iic_master_rx_byte(u8 ack)
{
    int timeout = 10000;
    local_irq_disable();
    IIC_END_PENDING_CLEAR();
    IIC_BYTE_PENDING_CLEAR();

    if (ack) {
        IIC_RECEIVE_ACK_SET(0);
    } else {
        IIC_RECEIVE_ACK_SET(1);
    }
    IIC_DIR_RECEIVE();
    IIC_WRITE_BUF(0xFF);
    delay(1);
    IIC_CONFIG_DONE();

    while (--timeout && !(IIC_BYTE_PENDING_IS()));
    IIC_BYTE_PENDING_CLEAR();
    local_irq_enable();
    if (!timeout) {
        /* while(1); */
        putchar('N');
        return 0;
    }
    return IIC_READ_BUF();
}
int hw_iic_master_read_buf(void *buf, int len)
{
    int i;

    if (!buf || !len) {
        return -1;
    }
    IIC_DIR_RECEIVE();
    IIC_RECEIVE_ACK_SET(0);
    for (i = 0; i < len; i++) {
        if (i == len - 1) {
            IIC_RECEIVE_ACK_SET(1);
        }
        IIC_WRITE_BUF(0xFF);
        delay(1);
        IIC_CONFIG_DONE();
        while (!(IIC_BYTE_PENDING_IS()));
        IIC_BYTE_PENDING_CLEAR();

        ((u8 *)buf)[i] = IIC_READ_BUF();
    }
    return len;
}

int hw_iic_master_write_buf(const void *buf, int len)
{
    int i = 0;
    if (!buf || !len) {
        return -1;
    }
    IIC_DIR_TRANSMIT();
    for (i = 0; i < len; i++) {
        IIC_WRITE_BUF(((u8 *)buf)[i]);
        delay(1);
        IIC_CONFIG_DONE();

        while (!(IIC_BYTE_PENDING_IS()));
        IIC_BYTE_PENDING_CLEAR();
        if (IIC_TRANSMIT_ACK_IS()) {
            break;
        }
    }
    return i;
}

//=========================================================================//
//                              IIC SLAVE                                  //
//=========================================================================//
enum SLAVE_STATUS_TABLE {
    SLAVE_STATUS_STOP = 0,
    SLAVE_STATUS_START,
    SLAVE_STATUS_READ,
    SLAVE_STATUS_WRITE,
};

___interrupt
static void iic_slave_isr(void)
{
    static enum SLAVE_STATUS_TABLE slave_status = SLAVE_STATUS_STOP;
    u8 data = 0;
    u8 ack = 0;
    static u8 read_cnt = 0;
    static u8 write_cnt = 0;

    printf("\n%s\n", __func__);
    if (IIC_START_PENDING_IS()) {
        printf("\nstart pending\n");
        IIC_START_PENDING_CLEAR();
        slave_status = SLAVE_STATUS_START;
        read_cnt = 0;
        write_cnt = 0;
    }
    if (IIC_END_PENDING_IS()) {
        printf("end pending\n");
        read_cnt = 0;
        write_cnt = 0;
        IIC_END_PENDING_CLEAR();
        slave_status = SLAVE_STATUS_STOP;
        IIC_RECEIVE_ACK_SET(0); //ack
        IIC_DIR_RECEIVE();
        IIC_CONFIG_DONE();
    }

    if (IIC_BYTE_PENDING_IS()) {
        IIC_BYTE_PENDING_CLEAR();
        printf("byte pending\n");
        //拉住总线处理数据
        if (slave_status == SLAVE_STATUS_START) {
            data = IIC_READ_BUF();
            printf("Slave Rx addr: 0x%x\n", data >> 1);
            if (data & BIT(0)) {
                slave_status = SLAVE_STATUS_READ;
                printf("cmd: Master read\n");
                IIC_DIR_TRANSMIT();
                IIC_WRITE_BUF(0xAB); //准备数据, 等待下一包发送
                read_cnt = 1;
                printf("Slave Tx: 0x%x\n", 0xAB);
                IIC_CONFIG_DONE();
            } else {
                slave_status = SLAVE_STATUS_WRITE;
                printf("cmd: Master write\n");
                IIC_RECEIVE_ACK_SET(0); //ack
                IIC_DIR_RECEIVE();
                IIC_WRITE_BUF(0xFF);
                IIC_CONFIG_DONE(); //释放总线, 等待下一包接收
            }
        } else if (slave_status == SLAVE_STATUS_STOP) {
            printf("cmd: err status\n");
        } else if (slave_status == SLAVE_STATUS_READ) {
            if (!(IIC_TRANSMIT_ACK_IS())) {
                printf("Master Rx ack\n");
            } else {
                printf("Master Rx nack\n");
            }
            IIC_DIR_TRANSMIT();
            IIC_WRITE_BUF(0xAB); //准备数据, 等待下一包发送
            IIC_CONFIG_DONE();
            printf("Slave Tx: 0x%x\n", 0xAB);

        } else if (slave_status == SLAVE_STATUS_WRITE) {
            write_cnt++;
            data = IIC_READ_BUF();
            printf("Slave Rx: 0x%x\n", data);
            if (write_cnt > 10) {
                printf("noack set\n");
                IIC_RECEIVE_ACK_SET(1); //nack
            } else {
                printf("ack set\n");
                IIC_RECEIVE_ACK_SET(0); //ack
            }
            IIC_DIR_RECEIVE();
            IIC_WRITE_BUF(0xFF);
            IIC_CONFIG_DONE();
        }
    }
}



//=========================================//
//              IIC test code              //
//=========================================//
#define P24C02A_EEPROM_DEVICE_ADDR  0b10100000
#define DIR_WRITE 					0
#define DIR_READ 					BIT(0)
#define DATA_PAGE_SIZE 				8
#define DATA_PAGE_NUM 				32
void iic_master_eeprom_test(void)
{
    u8 ack = 0;
    u8 cnt = 0;
    u8 data = 0;
    u8 page_num = 0;
    u32 addr_base = 0;
    hw_iic_init();
    //===========================//
    //         EEPROM WRITE      //
    //===========================//
__page_write:
    addr_base = page_num * DATA_PAGE_SIZE;
    printf("eeprom read write page: %d\n", page_num);
    hw_iic_start();
    ack = hw_iic_master_tx_byte(P24C02A_EEPROM_DEVICE_ADDR | DIR_WRITE);
    if (ack) {
        printf("device id ack\n");
        //word addr:
        ack = hw_iic_master_tx_byte(addr_base);
        if (ack) {
            printf("addr ack\n");
        } else {
            printf("addr nack\n");
            goto __test_end;
        }
        //data:
        for (cnt = 0; cnt < DATA_PAGE_SIZE; cnt++) {
            ack = hw_iic_master_tx_byte(cnt);
            if (ack) {
                printf("addr: %d, data %d, ack\n", addr_base + cnt, cnt);
            } else {
                printf("data nack\n");
                goto __test_end;
            }

        }
    } else {
        printf("device id nac\n");
    }

    hw_iic_stop();

    delay(30000); //delay 10ms
    //===========================//
    //        EEPROM READ        //
    //===========================//
    printf("eeprom read start:\n");
    hw_iic_start();
    ack = hw_iic_master_tx_byte(P24C02A_EEPROM_DEVICE_ADDR | DIR_WRITE);
    if (ack) {
        printf("1.device id ack\n");
    } else {
        printf("1.device id nack\n");
        goto __test_end;
    }
    //write addr
    ack = hw_iic_master_tx_byte(addr_base);
    if (ack) {
        printf("addr ack\n");
    } else {
        printf("addr nack\n");
        goto __test_end;
    }

    hw_iic_start();
    ack = hw_iic_master_tx_byte(P24C02A_EEPROM_DEVICE_ADDR | DIR_READ);
    if (ack) {
        printf("2.device id ack\n");
        //data:
        for (cnt = 0; cnt < DATA_PAGE_SIZE - 1; cnt++) {
            data = hw_iic_master_rx_byte(1);
            printf("addr: %d, Rx: %d\n", addr_base + cnt, data);
        }
        data = hw_iic_master_rx_byte(0);
        printf("addr: %d, Rx: %d\n", addr_base + cnt, data);
    } else {
        printf("device id nac\n");
    }
    if (++page_num < DATA_PAGE_NUM) {
        hw_iic_stop();
        delay(30000); //delay 10ms
        goto __page_write;
    }

__test_end:
    hw_iic_stop();
    printf("test end\n");
    return;
}


void iic_slave_test(void)
{
    printf("%s\n", __func__);
    hw_iic_init();
}
