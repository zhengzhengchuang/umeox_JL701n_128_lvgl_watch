#include "includes.h"
#include "asm/psram_hw.h"
#include "asm/psram_api.h"
#include "asm/clock.h"
#include "asm/timer.h"
#include "asm/gpio.h"
#include "asm/power/power_api.h"
#include "app_config.h"

#define AT_VOLATILE_RAM_CODE        AT(.volatile_ram_code)

#if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)

#define PSRAM_SPI_1      1                   // spi_psram 1-wire
#define PSRAM_SPI_4_0    1                   // spi_psram 4-wire : cmd is 1-wire,adr and dat is 4-wire
#define PSRAM_SPI_4_1    1                   // spi_psram 4-wire : cmd,adr and dat are all 4-wire

#define     PSRAM_STATE_POWER_OFF       0
#define     PSRAM_STATE_POWER_ON        1
#define     PSRAM_STATE_POWER_STANDBY   2

struct psram_handle {
    u8 init;
    u8 power_state;
    const struct psram_platform_data *config;
};

static struct psram_handle psram = {0};

#define __this 				(&psram)


void ___psram_test_code(void *priv);
void psram_test_code(void);

extern u32 ps_ram_size;

extern u32 ps_ram_data_code_addr;
extern u32 ps_ram_data_code_begin;
extern u32 ps_ram_data_code_size;
extern u32 ps_ram_bss_addr;
extern u32 ps_ram_bss_size;

extern u32 ps_ram_noinit_addr;
extern u32 ps_ram_noinit_size;

extern void flushinv_dcache(void *ptr, int len);
extern void DcuInvalidAll(void);
extern void DcuFlushAll(void);
extern void DcuFlushinvAll(void);

static void psram_hw_exit_sleep(void);


#define PSRAM_DATA_CODE_ADDR 			((u32 *)(&ps_ram_data_code_addr)) //dst
#define PSRAM_DATA_CODE_SIZE 			((u32)(&ps_ram_data_code_size))   //size
#define PSRAM_DATA_CODE_BEGIN 			((u32 *)(&ps_ram_data_code_begin)) //src

#define PSRAM_BSS_ADDR 					((u32 *)(&ps_ram_bss_addr)) //dst
#define PSRAM_BSS_SIZE 					((u32)(&ps_ram_bss_size)) //size

#define PSRAM_NOINIT_ADDR 				((u32 *)(&ps_ram_noinit_addr)) //dst
#define PSRAM_NOINIT_SIZE 				((u32)(&ps_ram_noinit_size)) //size

#define PSRAM_TOTAL_SIZE 				((u32)(&ps_ram_size))


void psram_flush_invaild_cache(u32 *begin, u32 len)
{
    u32 addr = (u32)begin;

    if ((begin < PSRAM_CACHE_ADDR) || (begin >= PSRAM_NO_CACHE_ADDR)) {
        return;
    }
    if ((addr + len) >= PSRAM_NO_CACHE_ADDR) {
        len -= addr + len - PSRAM_NO_CACHE_ADDR;
    }
    flushinv_dcache(begin, len);
}

void psram_flush_invaild_all_cache(void)
{
    /* u32 *begin = (u32 *)PSRAM_CACHE_ADDR; */
    /* u32 len = PSRAM_TOTAL_SIZE; */

    /* flushinv_dcache(begin, len); */
    DcuFlushinvAll();
}

void psram_flush_cache(u32 *begin, u32 len)
{
    u32 addr = (u32)begin;

    if ((begin < PSRAM_CACHE_ADDR) || (begin >= PSRAM_NO_CACHE_ADDR)) {
        return;
    }
    if ((addr + len) >= PSRAM_NO_CACHE_ADDR) {
        len -= addr + len - PSRAM_NO_CACHE_ADDR;
    }
    flush_dcache(begin, len);
}

void psram_flush_all_cache(void)
{
    /* u32 *begin = (u32 *)PSRAM_CACHE_ADDR; */
    /* u32 len = PSRAM_TOTAL_SIZE; */

    /* flush_dcache(begin, len); */
    DcuFlushAll();
}



static void psram_io_init(u8 port, u8 mode)
{
    /* printf("psram sel %d:port %c\n", port, port + 'A'); */

    if (port == PSRAM_PORT_SEL_PORTA) 
    {
        gpio_set_output_value(PSRAM_PORTA_CS, 1);
        gpio_set_direction(PSRAM_PORTA_CS, 0);
        gpio_set_pull_up(PSRAM_PORTA_CS, 0);
        gpio_set_pull_down(PSRAM_PORTA_CS, 0);

        gpio_set_output_value(PSRAM_PORTA_CLK, 0);
        gpio_set_direction(PSRAM_PORTA_CLK, 0);
        gpio_set_pull_up(PSRAM_PORTA_CLK, 0);
        gpio_set_pull_down(PSRAM_PORTA_CLK, 0);

        gpio_set_output_value(PSRAM_PORTA_D0, 0);
        gpio_set_direction(PSRAM_PORTA_D0, 0);
        gpio_set_die(PSRAM_PORTA_D0, 1);
        gpio_set_pull_up(PSRAM_PORTA_D0, 0);
        gpio_set_pull_down(PSRAM_PORTA_D0, 0);
        //gpio_set_dieh(PSRAM_PORTA_D0, 1);

        gpio_set_output_value(PSRAM_PORTA_D1, 0);
        gpio_set_direction(PSRAM_PORTA_D1, 0);
        gpio_set_die(PSRAM_PORTA_D1, 1);
        gpio_set_pull_up(PSRAM_PORTA_D1, 0);
        gpio_set_pull_down(PSRAM_PORTA_D1, 0);

        if (mode != PSRAM_MODE_1_WIRE) {
            gpio_set_output_value(PSRAM_PORTA_D2, 0);
            gpio_set_direction(PSRAM_PORTA_D2, 0);
            gpio_set_die(PSRAM_PORTA_D2, 1);
            gpio_set_pull_up(PSRAM_PORTA_D2, 0);
            gpio_set_pull_down(PSRAM_PORTA_D2, 0);

            gpio_set_output_value(PSRAM_PORTA_D3, 0);
            gpio_set_direction(PSRAM_PORTA_D3, 0);
            gpio_set_die(PSRAM_PORTA_D3, 1);
            gpio_set_pull_up(PSRAM_PORTA_D3, 0);
            gpio_set_pull_down(PSRAM_PORTA_D3, 0);
        }

        PSRAM_PORT_SEL(PSRAM_PORT_SEL_PORTA);

    } else {
        printf("psram port %d:%c sel err!!!\n", port, port + 'A');
    }
}

static void psram_io_uninit(u8 port, u8 mode)
{
    if (port == PSRAM_PORT_SEL_PORTA) {
        gpio_set_pull_up(PSRAM_PORTA_CS, 0);
        gpio_set_pull_down(PSRAM_PORTA_CS, 0);
        gpio_set_direction(PSRAM_PORTA_CS, 1);
        gpio_set_die(PSRAM_PORTA_CS, 0);
        gpio_set_dieh(PSRAM_PORTA_CS, 0);

        gpio_set_pull_up(PSRAM_PORTA_CLK, 0);
        gpio_set_pull_down(PSRAM_PORTA_CLK, 0);
        gpio_set_direction(PSRAM_PORTA_CLK, 1);
        gpio_set_die(PSRAM_PORTA_CLK, 0);
        gpio_set_dieh(PSRAM_PORTA_CLK, 0);

        gpio_set_pull_up(PSRAM_PORTA_D0, 0);
        gpio_set_pull_down(PSRAM_PORTA_D0, 0);
        gpio_set_direction(PSRAM_PORTA_D0, 1);
        gpio_set_die(PSRAM_PORTA_D0, 0);
        gpio_set_dieh(PSRAM_PORTA_D0, 0);

        gpio_set_pull_up(PSRAM_PORTA_D1, 0);
        gpio_set_pull_down(PSRAM_PORTA_D1, 0);
        gpio_set_direction(PSRAM_PORTA_D1, 1);
        gpio_set_die(PSRAM_PORTA_D1, 0);
        gpio_set_dieh(PSRAM_PORTA_D1, 0);

        if (mode != PSRAM_MODE_1_WIRE) {
            gpio_set_pull_up(PSRAM_PORTA_D2, 0);
            gpio_set_pull_down(PSRAM_PORTA_D2, 0);
            gpio_set_direction(PSRAM_PORTA_D2, 1);
            gpio_set_die(PSRAM_PORTA_D2, 0);
            gpio_set_dieh(PSRAM_PORTA_D2, 0);

            gpio_set_pull_up(PSRAM_PORTA_D3, 0);
            gpio_set_pull_down(PSRAM_PORTA_D3, 0);
            gpio_set_direction(PSRAM_PORTA_D3, 1);
            gpio_set_die(PSRAM_PORTA_D3, 0);
            gpio_set_dieh(PSRAM_PORTA_D3, 0);
        }

        PSRAM_PORT_SEL(PSRAM_PORT_SEL_PORTA);
    }
}

static void psram_power_on(u8 port)
{
    if (port > IO_PORT_MAX) {
        return;
    }

    /* if (port == IO_PORTE_06) { */
    /*     sdpg_config(4);        */
    /*     return;                */
    /* }                          */

    gpio_set_output_value(port, 1);
    gpio_set_hd(port, 1);
    gpio_set_hd0(port, 1);
    gpio_set_direction(port, 0);

    // > 150uS
    udelay(250);
}

static void psram_power_off(u8 port)
{
    if (port > IO_PORT_MAX) {
        return;
    }

    /* if (port == IO_PORTE_06) { */
    /*     sdpg_config(0);        */
    /*     return;                */
    /* }                          */

    gpio_set_pull_up(port, 0);
    gpio_set_pull_down(port, 0);
    gpio_set_direction(port, 1);
    gpio_set_die(port, 0);
    gpio_set_dieh(port, 0);
}


static void psram_section_init(void)
{
    u32 i = 0;
    u32 *src_ptr = NULL;
    u32 *dest_ptr = NULL;
    u32 data_len = 0;

    //===================================//
    //       Copy Psram Data & Code      //
    //===================================//
    src_ptr = PSRAM_DATA_CODE_BEGIN;
    dest_ptr = PSRAM_DATA_CODE_ADDR;
    data_len = (PSRAM_DATA_CODE_SIZE / sizeof(u32));
    if (data_len) {
        for (i = 0; i < data_len; i++) {
            dest_ptr[i] = src_ptr[i];
        }
    }

    //===================================//
    //          Clear Psram Bss          //
    //===================================//
    dest_ptr = PSRAM_BSS_ADDR;
    data_len = (PSRAM_BSS_SIZE / sizeof(u32));
    if (data_len) {
        for (i = 0; i < data_len; i++) {
            dest_ptr[i] = 0;
        }
    }
}

static void __psram_hw_init(u8 mode)
{
    SFR(JL_PSRAM->PSRAM_CON0, 0, 1, 1);     //psr_gate_en
    udelay(2);
    SFR(JL_PSRAM->PSRAM_CON0, 1, 1, 1);     //psr_en
    SFR(JL_PSRAM->PSRAM_CON0, 2, 1, 0);     //cur_is_ddr
    SFR(JL_PSRAM->PSRAM_CON0, 13, 1, 0);     //has_wrap_wr
    SFR(JL_PSRAM->PSRAM_CON0, 14, 1, 0);     //psram_qck_sel
    SFR(JL_PSRAM->PSRAM_CON0, 15, 2, 0);     //psram_page_size 0:512 1:1k 2:2k 3:4k
    SFR(JL_PSRAM->PSRAM_CON2, 14, 3, 2);     //tcph_cfg
    SFR(JL_PSRAM->PSRAM_CON2, 17, 11, 400); //tcem_cfg
    SFR(JL_PSRAM->PSRAM_CON2, 28, 4, 10); //idle_cfg
//-------------------debug-----------------//
    //SFR(JL_PSRAM->PSRAM_DBG_CON,21,1,1);    //dbg_pnding_clr
    //SFR(JL_PSRAM->PSRAM_DBG_START,0,31,0);  //debug start adr
    //SFR(JL_PSRAM->PSRAM_DBG_END,0,31,20);  //debug end adr
    //SFR(JL_PSRAM->PSRAM_DBG_CON,0,8,0);     //ok0_id
    //SFR(JL_PSRAM->PSRAM_DBG_CON,8,8,0x20);  //ok1_id
    //SFR(JL_PSRAM->PSRAM_DBG_CON,16,1,1);    //id0_wr_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,17,1,1);    //id0_rd_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,18,1,1);    //id1_wr_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,19,1,0);    //id1_rd_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,20,1,1);    //dbg_int_en
    //SFR(JL_PSRAM->PSRAM_DBG_CON,22,1,1);    //id0_out_en
    //SFR(JL_PSRAM->PSRAM_DBG_CON,23,1,1);    //id1_out_en
//-----------reset_enable----------------//
    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
    SFR(JL_PSRAM->PSRAM_CON0, 17, 1, 0); //psram_4cmd_en
    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x66); // reset_enable
    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
    udelay(20);
//-----------reset----------------//
    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x99); // reset
    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
    udelay(20);

    switch (mode) {
    case PSRAM_MODE_1_WIRE:
        //printf("%s mode: 1 wire\n", __func__);
#if PSRAM_SPI_1
        SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 1);     //psram fast read
        SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 7);    // dat_wait_cyc
        SFR(JL_PSRAM->PSRAM_CON0, 12, 1, 0);     //psram write

        //SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 0);     //psram read
        //SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 0);    // dat_wait_cyc
        //SFR(JL_PSRAM->PSRAM_CON0, 12, 1, 0);     //psram write

        //------------------------mode write-------------------------//
        //SFR(JL_PSRAM->PSRAM_CFG1, 0, 4, 0); //cmd_wait_cyc
        //SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 1); //adr_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 1); //cmd_wr_rd
        //SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 0); //cmd_else
        //SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0xb1); //cmd_code
        //SFR(JL_PSRAM->PSRAM_CFG1, 16, 8, 0x60); //cmd_wdat
        //SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
        //delay(1);

        //------------------------mode read---------------------//
        //SFR(JL_PSRAM->PSRAM_CFG1, 0, 4, 7); //cmd_wait_cyc
        //SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 1); //wait_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 1); //adr_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd
        //SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_else
        //SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0xb5); //cmd_code
        //SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
        //delay(1);
        break;

#endif  //#if PSRAM_SPI_1

    case PSRAM_MODE_4_WIRE_CMD1_ADR4_DAT4:
#if PSRAM_SPI_4_0
        //printf("%s mode: 4 wire, cmd 1 wire, addr 4 wire, data 4 wire\n", __func__);
        SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 2);     //psram quad read
        SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 5);    // wait_cyc
        //SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 1);     //psram fast read
        //SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 7);    // wait_cyc
        SFR(JL_PSRAM->PSRAM_CON0, 12, 1, 1);     //psram quad write
        break;
#endif //#if PSRAM_SPI_4_0

    case PSRAM_MODE_4_WIRE_CMD4_ADR4_DAT4:
        //printf("%s mode: 4 wire, cmd 4 wire, addr 4 wire, data 4 wire\n", __func__);
#if PSRAM_SPI_4_1
        SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 2);     //psram quad read
        SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 5);    // wait_cyc
        //SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 1);     //psram fast read
        //SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 4);    // wait_cyc
        SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
        SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x35); // enter quad mode
        SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
        udelay(5);
////-----------reset_enable----------------//
//    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
//    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
//    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x66); // reset_enable
//    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
//    delay(2);
//
////-----------reset----------------//
//    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
//    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
//    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x99); // reset
//    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
//    delay(2);
//
//    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
//    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
//    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0xf5); // exit quad mode
//    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd

#endif //#if PSRAM_SPI_4_1
        break;
    default:
        break;
    }
    /* printf("psram clk source: PLL %d Hz, div %d, output clk = %d Hz\n", */
    /* psram_clk_value_table[PSRAM_PLL_CLK_SEL], */
    /* psram_div_value_table[arch_info->psram.psram_div], */
    /* psram_clk_value_table[PSRAM_PLL_CLK_SEL] / psram_div_value_table[arch_info->psram.psram_div] */
    /* ); */
}


static void psram_hw_uninit(void)
{
    JL_PSRAM->PSRAM_CON0 = 0;
}

static void psram_hw_enter_powerdown(void)
{
    if (__this->init) {
        psram_io_uninit(__this->config->port, __this->config->mode);
        psram_power_off(__this->config->power_port);
        psram_hw_uninit();
        clk_set("psram", 0);
    }
}


/* ---------------------------------------------------------------------------- */
/**
 * @brief psram硬件初始化函数
 *
 * @param config: psram配置参数
 */
/* ---------------------------------------------------------------------------- */
void psram_hw_init(const struct psram_platform_data *config)
{
    ASSERT(config != NULL);
    __this->config = config;
    /* psram_clk_init: */
    clk_set("psram", config->init_clk);

    psram_io_init(config->port, config->mode);
    /* JL_PORTE->DIR = 0; */
    /* JL_PORTE->OUT = 0x8; */

    psram_power_on(config->power_port);

    __psram_hw_init(config->mode);

    u32 psram_size = PSRAM_TOTAL_SIZE;
    printf("ps_ram_size = %d KB", psram_size / 1024);
    printf("\nJL_PSRAM->PSRAM_CON0 = 0x%x", JL_PSRAM->PSRAM_CON0);
    printf("JL_PSRAM->PSRAM_CON1 = 0x%x", JL_PSRAM->PSRAM_CON1);
    printf("JL_PSRAM->PSRAM_CON2 = 0x%x", JL_PSRAM->PSRAM_CON2);
    printf("JL_PSRAM->PSRAM_CFG0 = 0x%x", JL_PSRAM->PSRAM_CFG0);
    printf("JL_PSRAM->PSRAM_CFG1 = 0x%x", JL_PSRAM->PSRAM_CFG1);
    printf("JL_PSRAM->PSRAM_CMD  = 0x%x\n", JL_PSRAM->PSRAM_CMD);
    printf("JL_CLOCK->CLK_CON3   = 0x%x\n", JL_CLOCK->CLK_CON3);
    printf("JL_PORTE->DIR   = 0x%x\n", JL_PORTE->DIR);
    printf("JL_PORTE->OUT   = 0x%x\n", JL_PORTE->OUT);
    printf("JL_PORTE->DIE   = 0x%x\n", JL_PORTE->DIE);
    printf("JL_PORTE->DIEH   = 0x%x\n", JL_PORTE->DIEH);
    printf("JL_PORTE->PU   = 0x%x\n", JL_PORTE->PU);
    printf("JL_PORTE->PD   = 0x%x\n", JL_PORTE->PD);

    __this->init = 1;
    __this->power_state = PSRAM_STATE_POWER_ON;

    psram_section_init();

    psram_heap_register_powerdown_handle(psram_hw_exit_sleep);
    psram_heap_register_flush_dcu_handle(psram_flush_cache);

    /* psram_test_code(); */
}


_INLINE_
u32 psram_cache2nocache_addr(u32 addr)
{
    ASSERT((addr >= PSRAM_CACHE_ADDR) && (addr < (PSRAM_CACHE_ADDR + PSRAM_MAPPING_SIZE)));

    return addr + PSRAM_MAPPING_SIZE;
}

_INLINE_
u32 psram_nocache2cache_addr(u32 addr)
{
    ASSERT((addr >= PSRAM_NO_CACHE_ADDR) && (addr < (PSRAM_NO_CACHE_ADDR + PSRAM_MAPPING_SIZE)));

    return addr - PSRAM_MAPPING_SIZE;
}

static u8 psram_idle_query(void)
{
    volatile u32 data_code_size = PSRAM_DATA_CODE_SIZE;
    volatile u32 bss_size = PSRAM_BSS_SIZE;
    volatile u32 noinit_size = PSRAM_NOINIT_SIZE;

    if (__this->init) {
        if ((data_code_size != 0) || (bss_size != 0) || (noinit_size != 0)) {
            //printf("psram have data code: 0x%x, bss: 0x%x, not enter sleep", data_code_size, bss_size);

            return 0;
        }

        if (psram_get_used_block() != 0) {
            //printf("psram heap remain %d blocks", psram_get_used_block());
            return 0;
        }
    }

    return 1;
}

/* REGISTER_LP_TARGET(psram_lp_target) = { */
/* .name = "psram", */
/* .is_idle = psram_idle_query, */
/* }; */

static void psram_hw_exit_sleep(void)
{
    if (__this->init) {
        if (__this->power_state == PSRAM_STATE_POWER_OFF) {
            clk_set("psram", __this->config->init_clk);
            psram_io_init(__this->config->port, __this->config->mode);
            psram_power_on(__this->config->power_port);
            __psram_hw_init(__this->config->mode);

            __this->power_state = PSRAM_STATE_POWER_ON;
        }
    }
}

void psram_suspend(u32 port, u32 mode)
{
    if (port == PSRAM_PORT_SEL_PORTA) {
        gpio_set_pull_up(PSRAM_PORTA_CS, 1);
        gpio_set_pull_down(PSRAM_PORTA_CS, 0);
        gpio_set_direction(PSRAM_PORTA_CS, 1);

        gpio_set_pull_up(PSRAM_PORTA_CLK, 0);
        gpio_set_pull_down(PSRAM_PORTA_CLK, 1);
        gpio_set_direction(PSRAM_PORTA_CLK, 1);

        gpio_set_pull_up(PSRAM_PORTA_D0, 0);
        gpio_set_pull_down(PSRAM_PORTA_D0, 1);
        gpio_set_direction(PSRAM_PORTA_D0, 1);

        gpio_set_pull_up(PSRAM_PORTA_D1, 0);
        gpio_set_pull_down(PSRAM_PORTA_D1, 1);
        gpio_set_direction(PSRAM_PORTA_D1, 1);

        if (mode != PSRAM_MODE_1_WIRE) {
            gpio_set_pull_up(PSRAM_PORTA_D2, 0);
            gpio_set_pull_down(PSRAM_PORTA_D2, 1);
            gpio_set_direction(PSRAM_PORTA_D2, 1);

            gpio_set_pull_up(PSRAM_PORTA_D3, 0);
            gpio_set_pull_down(PSRAM_PORTA_D3, 1);
            gpio_set_direction(PSRAM_PORTA_D3, 1);
        }
    }
    __this->power_state = PSRAM_STATE_POWER_STANDBY;
    JL_PSRAM->PSRAM_CON0 = 0;
}
static u32 psram_con0;
static u32 psram_con1;
static u32 psram_con2;
static u32 psram_cfg0;
static u32 psram_cfg1;
void psram_hw_enter_sleep(void)
{
    volatile u32 data_code_size = PSRAM_DATA_CODE_SIZE;
    volatile u32 bss_size = PSRAM_BSS_SIZE;
    volatile u32 noinit_size = PSRAM_NOINIT_SIZE;

    if (__this->init == 0) {
        return;
    }

    psram_con0 = JL_PSRAM->PSRAM_CON0;
    psram_con1 = JL_PSRAM->PSRAM_CON1;
    psram_con2 = JL_PSRAM->PSRAM_CON2;
    psram_cfg0 = JL_PSRAM->PSRAM_CFG0;
    psram_cfg1 = JL_PSRAM->PSRAM_CFG1;

    if (__this->power_state != PSRAM_STATE_POWER_ON) {
        return;
    }

    DcuFlushAll();
    /* DcuFlushinvAll(); */
    __asm__ volatile("ssync");
    while (!(JL_PSRAM->PSRAM_CON0 & BIT(31))); //Wait Psram Controler Idle

    if ((data_code_size != 0) || (bss_size != 0) || (noinit_size != 0)) {
        //printf("psram have data code: 0x%x, bss: 0x%x, not enter sleep", data_code_size, bss_size);
        goto __psram_standby;
    }

    if (psram_get_used_block() != 0) {
        //printf("psram heap remain %d blocks", psram_get_used_block());
        goto __psram_standby;
    }

    //Psram PowerDown
    psram_heap_reset();

    psram_hw_enter_powerdown();

    __this->power_state = PSRAM_STATE_POWER_OFF;
    return ;

__psram_standby:
    psram_suspend(__this->config->port, __this->config->mode);
}


void psram_exit_sleep(void)
{
    if (__this->init == 0) {
        return ;
    }

    if (__this->power_state == PSRAM_STATE_POWER_OFF) {
        return ;
    }

    if (__this->power_state == PSRAM_STATE_POWER_STANDBY) {
        psram_io_init(__this->config->port, __this->config->mode);

        JL_PSRAM->PSRAM_CON0 = psram_con0 & BIT(0); //psr_gate_en
        delay(200);
        JL_PSRAM->PSRAM_CON0 |= psram_con0 & BIT(1); //psr_gate_en

        JL_PSRAM->PSRAM_CON0 |= psram_con0;

        JL_PSRAM->PSRAM_CON2 = psram_con2;
        JL_PSRAM->PSRAM_CON1 = psram_con1;

        JL_PSRAM->PSRAM_CFG0 = psram_cfg0;
        JL_PSRAM->PSRAM_CFG1 = psram_cfg1;
    }

    __this->power_state = PSRAM_STATE_POWER_ON;


    return ;
}



#if 0
//===================================================//
//                PSRAM代码, 堆栈放RAM测试           //
//===================================================//
AT(.startup2.text)
void __delay(u32 cnt)
{
    while (cnt--) {
        __asm__ volatile("nop");
    }
}

#if 0
AT(.startup2.text)
void psram_io_test(u8 cnt)
{
    JL_PORTA->DIR &= ~BIT(13);
    while (cnt--) {
        JL_PORTA->OUT ^= BIT(13);
        __delay(10);
    }
}
#endif

extern u32 clock_pll1_init(u32 target_clk);

AT(.startup2.text)
void psram_hw_init_test(void)
{
    //clk init:
    //128MHz:
    PLL_CLK_3DIV2_OE(1);
    PLL_PSRAM_SEL(PLL_PSRAM_SEL_PLL0_D1P5); //128

    /* PLL_PSRAM_SEL(PLL_PSRAM_SEL_PLL0_D1P0); //96 */
    /* PLL_PSRAM_DIV(0b0100); //div2 */

    //PLL_CLK_3DIV2_OE(1);
    /* clock_pll1_init(140000000); */
    /* PLL_PSRAM_SEL(PLL_PSRAM_SEL_PLL1_D1P0); //140 */

    //power on:
    JL_PORTE->OUT |= BIT(6);
    JL_PORTE->HD |= BIT(6);
    JL_PORTE->HD0 |= BIT(6);
    JL_PORTE->DIR &= ~BIT(6);

    //IO config:
    //cs
    JL_PORTE->OUT |= BIT(3);
    JL_PORTE->DIR &= ~BIT(3);

    //clk
    JL_PORTE->OUT |= BIT(4);
    JL_PORTE->DIR &= ~BIT(4);

    //D0:
    JL_PORTE->OUT &= ~BIT(0);
    JL_PORTE->DIR &= ~BIT(0);
    JL_PORTE->DIE |= BIT(0);
    //D1:
    JL_PORTE->OUT &= ~BIT(2);
    JL_PORTE->DIR &= ~BIT(2);
    JL_PORTE->DIE |= BIT(2);

    //D2:
    JL_PORTE->OUT &= ~BIT(1);
    JL_PORTE->DIR &= ~BIT(1);
    JL_PORTE->DIE |= BIT(1);

    //D3:
    JL_PORTE->OUT &= ~BIT(5);
    JL_PORTE->DIR &= ~BIT(5);
    JL_PORTE->DIE |= BIT(5);


    SFR(JL_PSRAM->PSRAM_CON0, 0, 1, 1);     //psr_gate_en
    __delay(1);
    SFR(JL_PSRAM->PSRAM_CON0, 1, 1, 1);     //psr_en
    SFR(JL_PSRAM->PSRAM_CON0, 2, 1, 0);     //cur_is_ddr
    SFR(JL_PSRAM->PSRAM_CON0, 13, 1, 0);     //has_wrap_wr
    SFR(JL_PSRAM->PSRAM_CON0, 14, 1, 0);     //psram_qck_sel
    SFR(JL_PSRAM->PSRAM_CON0, 15, 2, 0);     //psram_page_size 0:512 1:1k 2:2k 3:4k
    SFR(JL_PSRAM->PSRAM_CON2, 14, 3, 2);     //tcph_cfg
    SFR(JL_PSRAM->PSRAM_CON2, 17, 11, 400); //tcem_cfg
    SFR(JL_PSRAM->PSRAM_CON2, 28, 4, 10); //idle_cfg
//-------------------debug-----------------//
    //SFR(JL_PSRAM->PSRAM_DBG_CON,21,1,1);    //dbg_pnding_clr
    //SFR(JL_PSRAM->PSRAM_DBG_START,0,31,0);  //debug start adr
    //SFR(JL_PSRAM->PSRAM_DBG_END,0,31,20);  //debug end adr
    //SFR(JL_PSRAM->PSRAM_DBG_CON,0,8,0);     //ok0_id
    //SFR(JL_PSRAM->PSRAM_DBG_CON,8,8,0x20);  //ok1_id
    //SFR(JL_PSRAM->PSRAM_DBG_CON,16,1,1);    //id0_wr_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,17,1,1);    //id0_rd_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,18,1,1);    //id1_wr_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,19,1,0);    //id1_rd_permit
    //SFR(JL_PSRAM->PSRAM_DBG_CON,20,1,1);    //dbg_int_en
    //SFR(JL_PSRAM->PSRAM_DBG_CON,22,1,1);    //id0_out_en
    //SFR(JL_PSRAM->PSRAM_DBG_CON,23,1,1);    //id1_out_en
//-----------reset_enable----------------//
    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
    SFR(JL_PSRAM->PSRAM_CON0, 17, 1, 0); //psram_4cmd_en
    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x66); // reset_enable
    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
    __delay(100);
//-----------reset----------------//
    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x99); // reset
    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
    __delay(100);


    u8 mode = PSRAM_MODE_4_WIRE_CMD1_ADR4_DAT4;
    switch (mode) {
    case PSRAM_MODE_1_WIRE:
        /* printf("%s mode: 1 wire\n", __func__); */
#if PSRAM_SPI_1
        SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 1);     //psram fast read
        SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 7);    // dat_wait_cyc
        SFR(JL_PSRAM->PSRAM_CON0, 12, 1, 0);     //psram write

        //SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 0);     //psram read
        //SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 0);    // dat_wait_cyc
        //SFR(JL_PSRAM->PSRAM_CON0, 12, 1, 0);     //psram write

        //------------------------mode write-------------------------//
        //SFR(JL_PSRAM->PSRAM_CFG1, 0, 4, 0); //cmd_wait_cyc
        //SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 1); //adr_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 1); //cmd_wr_rd
        //SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 0); //cmd_else
        //SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0xb1); //cmd_code
        //SFR(JL_PSRAM->PSRAM_CFG1, 16, 8, 0x60); //cmd_wdat
        //SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
        //delay(1);

        //------------------------mode read---------------------//
        //SFR(JL_PSRAM->PSRAM_CFG1, 0, 4, 7); //cmd_wait_cyc
        //SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 1); //wait_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 1); //adr_en
        //SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd
        //SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_else
        //SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0xb5); //cmd_code
        //SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
        //delay(1);
        break;

#endif  //#if PSRAM_SPI_1

    case PSRAM_MODE_4_WIRE_CMD1_ADR4_DAT4:
#if PSRAM_SPI_4_0
        /* printf("%s mode: 4 wire, cmd 1 wire, addr 4 wire, data 4 wire\n", __func__); */
        SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 2);     //psram quad read
        SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 5);    // wait_cyc
        //SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 1);     //psram fast read
        //SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 7);    // wait_cyc
        SFR(JL_PSRAM->PSRAM_CON0, 12, 1, 1);     //psram quad write
        break;
#endif //#if PSRAM_SPI_4_0

    case PSRAM_MODE_4_WIRE_CMD4_ADR4_DAT4:
        /* printf("%s mode: 4 wire, cmd 4 wire, addr 4 wire, data 4 wire\n", __func__); */
#if PSRAM_SPI_4_1
        SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 2);     //psram quad read
        SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 5);    // wait_cyc
        //SFR(JL_PSRAM->PSRAM_CON0, 10, 2, 1);     //psram fast read
        //SFR(JL_PSRAM->PSRAM_CFG0, 24, 8, 4);    // wait_cyc
        SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
        SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x35); // enter quad mode
        SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
        __delay(1);
////-----------reset_enable----------------//
//    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
//    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
//    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x66); // reset_enable
//    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
//    delay(2);
//
////-----------reset----------------//
//    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
//    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
//    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0x99); // reset
//    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd
//    delay(2);
//
//    SFR(JL_PSRAM->PSRAM_CFG1, 4, 1, 0); //wait_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 5, 1, 0); //adr_en
//    SFR(JL_PSRAM->PSRAM_CFG1, 6, 1, 0); //cmd_wr_rd 0:rd 1:wr
//    SFR(JL_PSRAM->PSRAM_CFG1, 7, 1, 1); //cmd_else
//    SFR(JL_PSRAM->PSRAM_CFG1, 8, 8, 0xf5); // exit quad mode
//    SFR(JL_PSRAM->PSRAM_CMD, 0, 1, 1); //spi_wr_cmd

#endif //#if PSRAM_SPI_4_1
        break;
    default:
        break;
    }
    /* printf("psram clk source: PLL %d Hz, div %d, output clk = %d Hz\n", */
    /* psram_clk_value_table[PSRAM_PLL_CLK_SEL], */
    /* psram_div_value_table[arch_info->psram.psram_div], */
    /* psram_clk_value_table[PSRAM_PLL_CLK_SEL] / psram_div_value_table[arch_info->psram.psram_div] */
    /* ); */

    //psram_io_test(2);
}
#endif /* #if 0 */

//=====================================================================//
//                          PSRAM读写测试代码                          //
//=====================================================================//
#if 0 //psram test code

/* ---------------------------------------------------------------------------- */
/**
 * @brief 将缓存同步到外存, 不清除映射关系
 *
 * @param ptr
 * @param len
 */
/* ---------------------------------------------------------------------------- */
extern void flush_dcache(void *ptr, int len);

/* ---------------------------------------------------------------------------- */
/**
 * @brief 将缓存同步到外存, 并清除映射关系
 *
 * @param ptr
 * @param len
 */
/* ---------------------------------------------------------------------------- */
extern void flushinv_dcache(void *ptr, int len);

void sfc_drop_cache(void *ptr, u32 len);

static int test_buf[1024] AT(.ps_ram_noinit);

static u32 psram_clk_table[] = {
    8000000,
    16000000,
    32000000,
    64000000,
    96000000,
    144000000,
};

void ___psram_test_code(void *priv)
{
#if 1 //基本读写
    u8 w_data = JL_RAND->R64L & 0xFF;
    line_inf;

    put_buf(test_buf, 0x20);
    memset(test_buf, 0x5A, 8 * 0x20);
    flushinv_dcache(test_buf, 8 * 0x20);
    put_buf(test_buf, 0x20);

    //u8 *ptr = (u8 *)test_buf;
    u8 *ptr = malloc_psram(1024);

    static u8 index = 0;

    if (index >= ARRAY_SIZE(psram_clk_table)) {
        index = 0;
    }
    /* clk_set("psram", psram_clk_table[index++]); */

    /* printf("JL_PORTE->DIR   = 0x%x\n", JL_PORTE->DIR); */
    /* printf("JL_PORTE->OUT   = 0x%x\n", JL_PORTE->OUT); */
    /* printf("JL_PORTE->DIE   = 0x%x\n", JL_PORTE->DIE); */
    /* printf("JL_PORTE->DIEH   = 0x%x\n", JL_PORTE->DIEH); */
    /* printf("JL_PORTE->PU   = 0x%x\n", JL_PORTE->PU); */
    /* printf("JL_PORTE->PD   = 0x%x\n", JL_PORTE->PD); */

    printf("w_data = 0x%x, ptr = 0x%x\n", w_data, (u32)ptr);

    for (int i = 0; i < 8 * 0x20; i++) {
        //*(ptr + i) = i / 0x20;
        *(ptr + i) = w_data;
    }
    //flush_dcache(ptr, 8 * 0x20);
    flushinv_dcache(ptr, 8 * 0x20);
    /* sfc_drop_cache(ptr, 8 * 0x20); */
    put_buf(ptr, 8 * 0x20);
    //for(int i=0; i<8; i++) *(ptr + i*0x20) = i;
    /* void DcuFlushinvRegion(int *beg, int len); */
    /* DcuFlushinvRegion(ptr, 0x20 * 8); */

    //clk_out:
    /* SFR(JL_CLOCK->CLK_CON1, 0, 4, 14); //clkout0 */
    /* JL_PORTB->DIR &= ~BIT(3); */
    /* JL_IOMC->OCH_CON0 &= ~(0x3F << 0); */
    /* JL_IOMC->OCH_CON0 |= (6 << 0); //OCH0 */
    /* JL_OMAP->PB3_OUT = FO_GP_OCH0; */

    asm("trigger");

    /* free_psram(ptr); */
    line_inf;

    /* while (1); */
#endif /* #if 0 //基本读写 */
}


//===================================================//
//                  PSRAM动态申请测试                //
//===================================================//
struct psram_test_buf {
    void *ptr;
    u32 len;
};

static struct psram_test_buf test_table[0x80];

static u32 psram_freq_table[] = {
    PSRAM_CLK_16MHZ,
    PSRAM_CLK_24MHZ,
    PSRAM_CLK_32MHZ,
    PSRAM_CLK_48MHZ,
    PSRAM_CLK_64MHZ,
    PSRAM_CLK_96MHZ,
    PSRAM_CLK_128MHZ,
};

static void psram_malloc_test_code(void)
{
    u8 test_index = JL_RAND->R64L & 0x7F;
    u32 len = 0;
    u8 *ptr = NULL;

    static u8 freq_index = 0;
    static u8 freq_cnt = 0;
    static u32 test_cnt = 0;
    printf("%s, test_cnt = %d", __func__, test_cnt++);

    if (freq_index >= ARRAY_SIZE(psram_freq_table)) {
        freq_index = 0;
    }

    /* if (freq_cnt++ > 200) { */
    /* clk_set("psram", psram_freq_table[freq_index++]); */
    /* freq_cnt = 0; */
    /* } */

    if (test_table[test_index].ptr == NULL) {
        len = JL_RAND->R64H & 0xFFFF;
        ptr = malloc_psram(len);
        if (ptr == NULL) {
            //psram_heap_dump();
        } else {
            //printf("index: %d, psram malloc succ, ptr = 0x%x, len = %d bytes", test_index, ptr, len);
            memset(ptr, (u8)((u32)ptr & 0xFF), len);

            test_table[test_index].ptr = ptr;
            test_table[test_index].len = len;
            //psram_heap_stats();
            /* flush_dcache(ptr, len); */
        }
    } else {
        ptr = test_table[test_index].ptr;
        len = test_table[test_index].len;
        for (u32 i = 0; i < len; i++) {
            if (ptr[i] != ((u32)ptr & 0xFF)) {
                printf("data err: target: 0x%x, fact: 0x%x", (u32)ptr & 0xFF, ptr[i]);
                ASSERT(0);
            }
        }
        free_psram(ptr);
        test_table[test_index].ptr = NULL;
        test_table[test_index].len = 0;
        //printf("index: %d, psram free succ, ptr = 0x%x, len = %d bytes", test_index, ptr, len);
    }
}

//flush cache test
static int psram_buf_bss[1 * 1024 * 1024 / 4] AT(.psram_bss);
const static u32 test_size_table[] = {
    0,
    512,
    1 * 1024,
    2 * 1024,
    4 * 1024,
    8 * 1024,
    16 * 1024,
    32 * 1024,
    64 * 1024,
    128 * 1024
};

void flush_psram_cache_test(void)
{
    static index = 0;
    static last_offset = 0;
    if (index >= ARRAY_SIZE(test_size_table)) {
        index = 0;
    }
    u32 offset = JL_RAND->R64H & 0xFFFF / 4 * 4;
    u32 wlen = test_size_table[index++];
    u8 wdata = offset & 0xFF;

    //offset = 0;
    put_buf(psram_buf_bss + last_offset, 32);
    printf("write offset: 0x%x, data: 0x%x, logic addr: 0x%x, wlen: %d", offset, wdata, psram_buf_bss + offset, wlen);
    memset(psram_buf_bss + offset, wdata, wlen);
    last_offset = offset;

    if (wlen == 0) {
        PSRAM_IO_DEBUG_1(A, 14);
        PSRAM_IO_DEBUG_0(A, 14);
    }
}

void psram_test_code(void)
{
    /* sys_timer_add(NULL, ___psram_test_code, 2000); */
    sys_timer_add(NULL, psram_malloc_test_code, 500);
}
#endif /* #if 0 //psram test code */

#else

_INLINE_
u32 psram_cache2nocache_addr(u32 addr)
{
    return addr;
}

#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */
