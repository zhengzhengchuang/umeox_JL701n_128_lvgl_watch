#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".uart.bss")
#pragma data_seg(".uart.data")
#pragma const_seg(".uart.const")
#pragma code_seg(".uart.text")
#pragma str_literal_override(".uart.text")
#endif /* #ifdef SUPPORT_MS_EXTENSIONS */

#include "includes.h"
#include "uart.h"
#include "gpio.h"

//uart时钟选择:
#define UART_CLK_SEL 				UART_CLK_RC16M   //由主系统gpcnt模块算出rc16m频率，rc16m频率跟pvdd有关，注意配置被覆盖。
/* #define UART_CLK_SEL 				UART_CLK_STD_12M //低功耗不能使用该时钟源 */

//uart波特率选择:
#define UART_TARGET_BAUD    		115200L
//#define UART_TARGET_BAUD    		1000000L

//调试串口选择: P11_UART0/P11_UART1
#define DEBUG_UART_SELECT 			P11_UART1

//输入选择. 只有UART0有输入功能
#define DEBUG_UART_RX_ENABLE 		0
#if DEBUG_UART_RX_ENABLE
#define DEBUG_UART_RX_IO 			IO_PORTB_05
#define DEBUG_UART_RX_OVERTIME 		100 //ms
#endif /* #if DEBUG_UART_RX_ENABLE */

static u8 init = 0;

___interrupt
static void uart0_isr(void)
{
    u8 rx = 0;

    if (UART_RX_PENDING_IS(P11_UART0)) {
        rx = UART_BUF_READ(P11_UART0);
        UART_RX_PENDING_CLEAR(P11_UART0);
        printf("RX pending, rx: 0x%x\n", rx);
    }

    if (UART_OT_PENDING_IS(P11_UART0)) {
        rx = UART_BUF_READ(P11_UART0);
        UART_OT_PENDING_CLEAR(P11_UART0);
        printf("OT pending, rx: 0x%x\n", rx);
    }
}

static u32 uart_src_clk_get(void)
{
    u32 freq = 0;

    if (UART_CLK_SEL == UART_CLK_STD_12M) {
        SFR(P11_CLOCK->CLK_CON1, 15, 2, 1); //MSYS_BT24M --> P11_BT24M
        SFR(P11_CLOCK->CLK_CON1, 17, 3, 1); //STD12M sel P11_BT24M input
        SFR(P11_CLOCK->CLK_CON1, 20, 2, 0); // div0
        SFR(P11_CLOCK->CLK_CON1, 22, 2, 1); // div1
        freq = 12000000;
    } else if (UART_CLK_SEL == UART_CLK_RC16M) {
        freq = 16000000;//(u32)((u32)m2p_message[M2P_RCH_FEQ_H] << 8 | m2p_message[M2P_RCH_FEQ_L]) * 1000;
    } else if (UART_CLK_SEL == UART_CLK_SYS_CLK) {
        freq = 16000000;
    }

    P11_UART_CLK_SEL(UART_CLK_SEL);

    return freq;
}

void debug_uart_init(u8 tx_port)
{
    u16 baud = (uart_src_clk_get() / UART_TARGET_BAUD) / 4 - 1;

    UART_CON0_CLEAR(DEBUG_UART_SELECT);
    UART_CON1_CLEAR(DEBUG_UART_SELECT);

    UART_BAUD_SET(DEBUG_UART_SELECT, baud);

    //tx io inita
    gpio_set_output_value(tx_port, 1);
    gpio_set_direction(tx_port, 0);

    if (DEBUG_UART_SELECT == P11_UART1) {
        gpio_set_fun_output_port(tx_port, P11_FO_UART1_TX, 1, 1);
    } else if (DEBUG_UART_SELECT == P11_UART0) {
        gpio_set_fun_output_port(tx_port, P11_FO_UART0_TX, 1, 1);
    }

#if DEBUG_UART_RX_ENABLE
    u32 ot_value = 0;
    if (DEBUG_UART_SELECT == P11_UART0) {
        //rx io init:
        gpio_set_direction(DEBUG_UART_RX_IO, 1);
        gpio_set_pull_up(DEBUG_UART_RX_IO, 1);
        gpio_set_pull_down(DEBUG_UART_RX_IO, 0);
        gpio_set_die(DEBUG_UART_RX_IO, 1);
        //iomc:
        gpio_set_fun_input_port(DEBUG_UART_RX_IO, PFI_UART0_RX);

        //rx config:
        UART_OT_PENDING_CLEAR(P11_UART0);
        UART_RX_PENDING_CLEAR(P11_UART0);
        ot_value = (uart_src_clk_get() * DEBUG_UART_RX_OVERTIME) / 1000;
        UART_OTCNT_SET(P11_UART0, ot_value);
        UART_OT_INT_ENABLE(P11_UART0);
        UART_RX_INT_ENABLE(P11_UART0);
        UART_RX_ENABLE(P11_UART0);
        P11_CLOCK->WKUP_EN |= BIT(IRQ_UART0_IDX);
        INTALL_HWI(IRQ_UART0_IDX, uart0_isr, IRQ_UART0_IP);
    }
#endif /* #if DEBUG_UART_RX_ENABLE */

    UART_TX_ENABLE(DEBUG_UART_SELECT);

    init = 1;
}

__WEAK__
void putchar(char a)
{
    if (init == 0) {
        return;
    }

    if (a == '\r') {
        return;
    }

    local_irq_disable();
    if (a == '\n') {
        UART_BUF_WRITE(DEBUG_UART_SELECT, '\r');

        __asm__ volatile("csync");
        while (!UART_TX_PENDING_IS(DEBUG_UART_SELECT));

        UART_TX_PENDING_CLEAR(DEBUG_UART_SELECT);

    }

    UART_BUF_WRITE(DEBUG_UART_SELECT, a);

    __asm__ volatile("csync");
    while (!UART_TX_PENDING_IS(DEBUG_UART_SELECT));

    UART_TX_PENDING_CLEAR(DEBUG_UART_SELECT);

    local_irq_enable();
}

__WEAK__
void putbyte(char a)
{
    putchar(a);
}

