#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".spi.bss")
#pragma data_seg(".spi.data")
#pragma const_seg(".spi.const")
#pragma code_seg(".spi.text")
#pragma str_literal_override(".spi.text")
#endif /* #ifdef SUPPORT_MS_EXTENSIONS */

#include "includes.h"
#include "spi_hw.h"
#include "gpio.h"

//================================================================//
// 注意: SPI 只做主机, 不做从机
//================================================================//

#define SPI_CS_IO 			IO_PORTB_05
#define SPI_CLK_IO 			IO_PORTB_06
#define SPI_DI_IO 			IO_PORTB_07
#define SPI_DO_IO 			IO_PORTB_08

#define SPI_MASTER_CLOCK_FREQ 		200000
#define SYS_CLOCK_FREQ 				16000000UL

static u32 spi_get_clk_freq(void)
{
    return 1600000;
}

___interrupt
static void spi_isr(void)
{
    printf("%s\n", __func__);

    if (SPI_PENDING_IS()) {
        printf("spi pending\n");
        SPI_PENDING_CLEAR();
    }
}


void spi_cs_set_active(void)
{
    gpio_set_output_value(SPI_CS_IO, 0);
}

void spi_cs_set_idle(void)
{
    gpio_set_output_value(SPI_CS_IO, 1);
}


void spi_master_tx_byte(u8 data)
{
    SPI_DIR_TRAMSMIT();
    SPI_BUF_WRITE(data);

    while (!(SPI_PENDING_IS()));
    SPI_PENDING_CLEAR();
}

u32 spi_master_dma_tx(u8 *buf, u32 len)
{
    SPI_DIR_TRAMSMIT();

    if (((u32)buf % 4) != 0) {
        printf("spi buf: 0x%x not align 4\n", buf);
        return 0;
    }
    SPI_DMA_ADDR_CONFIG(buf);
    SPI_START_DMA_CNT(len);

    while (!(SPI_PENDING_IS()));
    SPI_PENDING_CLEAR();

    return len;
}

u8 spi_master_rx_byte(void)
{
    u8 data = 0;

    SPI_DIR_RECEIVE();
    SPI_BUF_WRITE(0xFF);

    while (!(SPI_PENDING_IS()));
    SPI_PENDING_CLEAR();

    data = SPI_BUF_READ();

    return data;
}

u32 spi_master_dma_rx(u8 *buf, u32 len)
{
    SPI_DIR_RECEIVE();

    if (((u32)buf % 4) != 0) {
        printf("spi buf: 0x%x not align 4\n", buf);
        return 0;
    }
    SPI_DMA_ADDR_CONFIG(buf);
    SPI_START_DMA_CNT(len);

    while (!(SPI_PENDING_IS()));
    SPI_PENDING_CLEAR();

    return len;
}

void spi_master_init(void)
{
    u32 baud = 0;

    SPI_CON_CLEAR();
    SPI_PENDING_CLEAR();

    baud = spi_get_clk_freq() / SPI_MASTER_CLOCK_FREQ - 1;

    SPI_BAUD_CONFIG(baud);

    SPI_CS_IDLE_LOGIC(1);

    SPI_CLK_IDLE_LOGIC(1);

    SPI_UPDETE_AT_FALL_EDGE();
    SPI_SIMPLE_AT_RISE_EDGE();

    SPI_BI_DIR(0);
    SPI_CS_ENABLE(1); //always set 1

    SPI_MASTER_MODE();

    /* SPI_INT_ENABLE(1); */

    //gpio:
    //CS:
    gpio_set_direction(SPI_CS_IO, 0);
    gpio_set_output_value(SPI_CS_IO, 1);

    //CLK:
    gpio_set_direction(SPI_CLK_IO, 0);
    gpio_set_output_value(SPI_CLK_IO, 1);
    //gpio_set_fun_input_port(SPI_CLK_IO, );
    gpio_set_fun_output_port(SPI_CLK_IO, P11_FO_SPI_CLK, 1, 1);

    //DO:
    gpio_set_direction(SPI_DO_IO, 0);
    gpio_set_output_value(SPI_DO_IO, 1);
    //gpio_set_fun_input_port(SPI_DO_IO, );
    gpio_set_fun_output_port(SPI_DO_IO, P11_FO_SPI_DO, 1, 1);

    //DI:
    gpio_set_direction(SPI_DI_IO, 1);
    gpio_set_pull_down(SPI_DI_IO, 0);
    gpio_set_pull_up(SPI_DI_IO, 1);
    gpio_set_die(SPI_DI_IO, 1);
    //gpio_set_fun_output_port(, P11_FO_, 1, 1);
    gpio_set_fun_input_port(SPI_DI_IO, PFI_SPI_DI);

    SPI_MODULE_ENABLE(1);

    printf("P11_SPI->CON  = 0x%x\n", P11_SPI->CON);
    //printf("P11_SPI->BAUD = 0x%x\n", P11_SPI->BAUD);
    printf("P11_SPI->BAUD = 0x%x\n", baud);
    printf("P11_SPI->CNT = 0x%x\n", P11_SPI->CNT);
    printf("P11_SPI->ADR = 0x%x\n", P11_SPI->ADR);
}

void spi_close(void)
{
    SPI_MODULE_ENABLE(0);
}

//=========================================//
//              SPI test code              //
//=========================================//
/*************************************************/
/*
		COMMAND LIST - WinBond FLASH WX25X10
*/
/*************************************************/
#define WINBOND_WRITE_ENABLE		0x06
#define WINBOND_WRITE_DISABLE	    0x04
#define WINBOND_READ_SR1			0x05
#define WINBOND_READ_SR2			0x35
#define WINBOND_WRITE_SR			0x01
#define WINBOND_READ_DATA		    0x03
#define WINBOND_FAST_READ			0x0B
#define WINBOND_FAST_READ_DUAL	    0x3B
#define WINBOND_FAST_READ_QUAD      0x6B
#define WINBOND_PAGE_PROGRAM	    0x02
#define WINBOND_PAGE_PROGRAM_QUAD	0x32
#define WINBOND_SECTOR_ERASE		0x20
#define WINBOND_BLOCK_ERASE		    0xD8
#define WINBOND_CHIP_ERASE		    0xC7
#define WINBOND_POWER_DOWN		    0xB9
#define WINBOND_RELEASE			    0xAB
#define WINBOND_MANUFACTURE		    0x90
#define WINBOND_JEDEC_ID			0x9F

enum NORFLASH_ERASE_CMD {
    NORFLASH_ERASE_SECTOR,
    NORFLASH_ERASE_BLOCK,
    NORFLASH_ERASE_CHIP,
};

static void norflash_power_reset(void)
{
    printf("nor_power_reset\n");

    spi_cs_set_active();
    spi_master_tx_byte(0x66);
    spi_cs_set_idle();

    spi_cs_set_active();
    spi_master_tx_byte(0x99);
    spi_cs_set_idle();

    delay(1000);
}


static void norflash_wait_ok(u32 otms)
{
    while (otms--) {
        spi_cs_set_active();
        spi_master_tx_byte(WINBOND_READ_SR1);
        u32 reg = spi_master_rx_byte();
        spi_cs_set_idle();

        if ((reg & BIT(0)) == 0) {
            break;
        }
        delay(10000);
    }

    if (otms == 0) {
        printf("flash time out\n");
    }
}


static void norflash_power_release(void)
{
    spi_cs_set_active();

    spi_master_tx_byte(WINBOND_RELEASE);

    //3dummy Bytes
    spi_master_tx_byte(0xff);
    spi_master_tx_byte(0xff);
    spi_master_tx_byte(0xff);
    //device id
    spi_master_rx_byte();

    spi_cs_set_idle();
}


static u32 norflash_read_id(void)
{
    u8 i;
    u32 id = 0;

    spi_cs_set_active();

    spi_master_tx_byte(WINBOND_JEDEC_ID);


    for (i = 0; i < 3; i++) {
        id <<= 8;
        id |= spi_master_rx_byte();
    }

    spi_cs_set_idle();

    printf("flash id 0x%x\n", id);

    return id;
}


static void norflash_send_addr(u32 addr)
{
    spi_master_tx_byte(addr >> 16);
    spi_master_tx_byte(addr >> 8);
    spi_master_tx_byte(addr);
}

static void norflash_write_enable(void)
{
    spi_cs_set_active();
    spi_master_tx_byte(WINBOND_WRITE_ENABLE);
    spi_cs_set_idle();
}

static int norflash_write(u8 *buf, u32 offset, u32 len)
{
    u32 _len = len;

    if (len == 0) {
        return 0;
    }

    norflash_write_enable();

    spi_cs_set_active();

    spi_master_tx_byte(WINBOND_PAGE_PROGRAM);

    norflash_send_addr(offset);

    spi_master_dma_tx(buf, len);
    /* while (len--) { */
    /* spi_master_tx_byte(*buf++); */
    /* } */

    spi_cs_set_idle();

    norflash_wait_ok(1000);

    return 0;
}


static int norflash_read(u8 *buf, u32 offset, u32 len)
{
    spi_cs_set_active();

    spi_master_tx_byte(WINBOND_FAST_READ);

    norflash_send_addr(offset);

    spi_master_rx_byte(); //DUMMY_BYTE

    spi_master_dma_rx(buf, len);

    spi_cs_set_idle();

    return len;
}


static void norflash_erase(u32 cmd, u32 addr)
{
    switch (cmd) {
    case NORFLASH_ERASE_SECTOR:
        cmd = WINBOND_SECTOR_ERASE;
        break;

    case NORFLASH_ERASE_BLOCK:
        cmd = WINBOND_BLOCK_ERASE;
        break;

    case NORFLASH_ERASE_CHIP:
        cmd = WINBOND_CHIP_ERASE;
        break;
    default:
        cmd = WINBOND_SECTOR_ERASE;
        break;
    }

    norflash_write_enable();

    spi_cs_set_active();
    spi_master_tx_byte(cmd);
    norflash_send_addr(addr);
    spi_cs_set_idle();

    norflash_wait_ok(100);
}

void spi_master_test_code(void)
{
    int buf[256 / sizeof(int)];
    u8 *_buf = (u8 *)buf;

    spi_master_init();

    norflash_power_release();
    norflash_wait_ok(150);
    norflash_power_reset();

    norflash_read_id();

    printf("1.norflash_read: \n");
    norflash_read((u8 *)buf, 0, sizeof(buf));
    put_buf(_buf, sizeof(buf));

    printf("2.norflash_erase: \n");
    norflash_erase(NORFLASH_ERASE_SECTOR, 0);

    printf("3.norflash_read: \n");
    norflash_read((u8 *)buf, 0, sizeof(buf));
    put_buf(_buf, sizeof(buf));

    for (u32 i = 0; i < sizeof(buf); i++) {
        _buf[i] = i;
    }

    printf("4.norflash_write: \n");
    norflash_write((u8 *)buf, 0, sizeof(buf));

    memset(buf, sizeof(buf), 0);
    printf("5.norflash_read: \n");
    norflash_read((u8 *)buf, 0, sizeof(buf));
    put_buf(_buf, sizeof(buf));

    spi_close();
}


