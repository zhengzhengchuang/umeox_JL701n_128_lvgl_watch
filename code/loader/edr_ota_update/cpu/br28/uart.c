#include "asm/uart.h"
#include "asm/gpio.h"
#include "asm/hwi.h"
#include "generic/printf.h"

#define LOG_TAG_CONST       UART
#define LOG_TAG             "[UART]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

static u8 used_tx_io;
static u32 *used_tx_io_omap = NULL;
static volatile u8 uart_busy_flag = 0;

#define DEBUG_UART  JL_UART0

__attribute__((always_inline_when_const_args))
void putchar(char a)
{
#if APP_DEBUG
    if (uart_busy_flag) {
        return;
    }
    uart_busy_flag = 1;
    if (DEBUG_UART->CON0 & BIT(0)) {
        if (a == '\n') {
            DEBUG_UART->BUF = '\r';
            __asm__ volatile("csync");
            while ((DEBUG_UART->CON0 & BIT(15)) == 0);
            DEBUG_UART->CON0 |= BIT(13);
        }

        DEBUG_UART->BUF = a;
        __asm__ volatile("csync");
        while ((DEBUG_UART->CON0 & BIT(15)) == 0);
        DEBUG_UART->CON0 |= BIT(13);
    }

    uart_busy_flag = 0;
#endif
}


static void uart_set_baud(u32 baud)
{
#if APP_DEBUG
    JL_CLOCK->CLK_CON2 &= ~BIT(11);
    JL_CLOCK->CLK_CON2 |= BIT(10);//osc clk

    DEBUG_UART->CON0 &= ~BIT(0);
    DEBUG_UART->CON0 |= BIT(13) | BIT(12) | BIT(10);
    DEBUG_UART->BAUD = (24000000 / baud) / 4 - 1;
    DEBUG_UART->CON0 |= BIT(13) | BIT(12) | BIT(10) | BIT(0);
#endif
}


void uart_init(const char *tx_pin, u32 baud)
{
#if APP_DEBUG
    if ((baud) && (tx_pin && *tx_pin)) {
        u32 io = get_gpio(tx_pin);

        unsigned int *omap_port = gpio2crossbar_outreg(io);

        if (DEBUG_UART == JL_UART0) {
            *omap_port = FO_UART0_TX;
        } else if (DEBUG_UART == JL_UART1) {
            *omap_port = FO_UART1_TX;
        } else if (DEBUG_UART == JL_UART2) {
            *omap_port = FO_UART2_TX;
        }

        gpio_set_direction(io, 0);
        gpio_set_die(io, 1);

        DEBUG_UART->CON0 = BIT(13) | BIT(12) | BIT(10);
        uart_set_baud(baud);
    }
    mask_init(NULL, putchar);
#endif
}

/* void uart_init(const char *tx_io, u32 baud) */
/* { */
/*     //std24M */
/*     JL_CLOCK->CLK_CON2 &= ~BIT(11); */
/*     JL_CLOCK->CLK_CON2 |=  BIT(10); */
/* #if (APP_DEBUG) */
/*     used_tx_io = get_gpio(tx_io); */
/*     if ((baud) && (used_tx_io < IO_PORT_MAX)) { */
/*         used_tx_io_omap = gpio2crossbar_outreg(used_tx_io); */
/*         if (DEBUG_UART == JL_UART0) { */
/*             *used_tx_io_omap = FO_UART0_TX << 2 | 3 ; */
/*         } else if (DEBUG_UART == JL_UART1) { */
/*             *used_tx_io_omap = FO_UART1_TX << 2 | 3 ; */
/*         } else if (DEBUG_UART == JL_UART2) { */
/*             *used_tx_io_omap = FO_UART2_TX << 2 | 3 ; */
/*         } */
/*         gpio_set_die(used_tx_io, 1); */
/*         gpio_set_direction(used_tx_io, 0); */
/*         DEBUG_UART->BAUD = (24000000 / baud ) / 4 - 1; */
/*         DEBUG_UART->CON0 = BIT(13) | BIT(12) | BIT(10) | BIT(0); */
/*     } */
/* #endif */
/*     mask_init(NULL, NULL, NULL, NULL, */
/*               NULL, */
/*               NULL, */
/*               NULL, NULL, */
/*               putchar, NULL); */
/* } */

void uart_close(void)
{
#if (APP_DEBUG)
    if (DEBUG_UART->CON0 & BIT(0)) {
        DEBUG_UART->CON0 = 0;
        DEBUG_UART->CON1 = 0;
        gpio_set_pull_up(used_tx_io, 0);
        gpio_set_pull_down(used_tx_io, 0);
        gpio_set_die(used_tx_io, 0);
        gpio_set_direction(used_tx_io, 1);
    }
#endif
}



#define UT_UP_BAUD_CLK      24000000
#define UT_UP_BUF_SIZE      4128    //4096 + 32
static u8 ut_up_buf[UT_UP_BUF_SIZE] ALIGNED(32);
static u32 ut_up_rxlen = 0;

extern void set_new_data_flag(int flag);

u8 *get_ut_up_buf_p(void)
{
    return ut_up_buf;
}

u32 get_ut_up_rxlen(void)
{
    return ut_up_rxlen;
}

void ut_up_tx_data(u8 *buf, u32 len)
{
    JL_UART0->TXADR = (u32)buf;
    JL_UART0->TXCNT = len;
    __asm__ volatile("csync");
    while (!(JL_UART0->CON0 & BIT(15)));
    JL_UART0->CON0 |= BIT(13);
}

SET_INTERRUPT
void uart0_irq(void)
{
    if ((JL_UART0->CON0 & BIT(5)) && (JL_UART0->CON0 & BIT(11))) {
        JL_UART0->CON0 |= BIT(7);
        JL_UART0->CON0 |= BIT(10);
        JL_UART0->CON0 |= BIT(12);
        __asm__ volatile("csync");
        __asm__ volatile("csync");
        ut_up_rxlen = JL_UART0->HRXCNT;
        if (ut_up_rxlen >= 4) {
            set_new_data_flag(1);
        }
        JL_UART0->RXSADR = (u32)ut_up_buf;
        JL_UART0->RXEADR = (u32)ut_up_buf + UT_UP_BUF_SIZE;
        JL_UART0->RXCNT = UT_UP_BUF_SIZE;
    }
}

void ut_device_mode(const char *ut_tx, const char *ut_rx, u32 baud)
{
    log_info("\n**** ut up mode *******\n\n");
    u32 ut_tx_io = get_gpio(ut_tx);
    u32 ut_rx_io = get_gpio(ut_rx);
    if ((baud) && (ut_tx_io < IO_PORT_MAX) && (ut_rx_io < IO_PORT_MAX)) {

        u32 *omap = gpio2crossbar_outreg(ut_tx_io);
        *omap = FO_UART0_TX << 2 | 3;
        gpio_set_die(ut_tx_io, 1);
        gpio_set_direction(ut_tx_io, 0);

        u32 imap = gpio2crossbar_inport(ut_rx_io);
        JL_IMAP->FI_UART0_RX = imap;
        gpio_set_die(ut_rx_io, 1);
        gpio_set_direction(ut_rx_io, 1);
        gpio_set_pull_up(ut_rx_io, 1);
        gpio_set_pull_down(ut_rx_io, 0);

        request_irq(IRQ_UART0_IDX, 0, uart0_irq, 0);
        JL_UART0->CON0 = BIT(13) | BIT(12) | BIT(10);
        JL_UART0->BAUD = UT_UP_BAUD_CLK / baud / 4 - 1;
        JL_UART0->RXSADR = (u32)ut_up_buf;
        JL_UART0->RXEADR = (u32)ut_up_buf + UT_UP_BUF_SIZE;
        JL_UART0->RXCNT = UT_UP_BUF_SIZE;
        JL_UART0->OTCNT = UT_UP_BAUD_CLK / 1000; //1ms
        JL_UART0->CON0 |= BIT(5);   //OT中断
        JL_UART0->CON0 |= BIT(6);   //启动DMA
        JL_UART0->CON0 |= BIT(13) | BIT(12) | BIT(10) | BIT(1) | BIT(0);
    }
}

void ut_devic_mode_close(void)
{
    JL_UART0->CON0 = BIT(13) | BIT(12) | BIT(10);
}
