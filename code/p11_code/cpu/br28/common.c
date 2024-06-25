#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".app.bss")
#pragma data_seg(".app.data")
#pragma const_seg(".app.const")
#pragma code_seg(".app.text")
#pragma str_literal_override(".app.text")
#endif /* #ifdef SUPPORT_MS_EXTENSIONS */

#include "includes.h"
#include "gpio.h"


#define DELAY_US_CNT 3
void udelay(u32 us)//不准
{
    u32 temp = DELAY_US_CNT * us - 90;
    while (temp--) {
        asm("nop");
    }
}

void mdelay(u32 ms)//不准
{
    while (ms--) {
        udelay(1000);
    }
}
void delay_measurement()
{
    gpio_set_output_value(IO_PORTB_01, 1);
    gpio_set_direction(IO_PORTB_01, 0);
    while (1) {
        /* udelay(1000); */
        mdelay(50);
        P11_PORT->PB_OUT ^= BIT(1);
    }
}

#define P11_DEBUG_IO_INDEX 		5

__INLINE__
void debug_io_1(void)
{
    P11_PORT->PB_SEL |= BIT(P11_DEBUG_IO_INDEX); 	//P11 Control
    P11_PORT->PB_DIR &= ~BIT(P11_DEBUG_IO_INDEX); 	//dir output
    P11_PORT->PB_OUT |= BIT(P11_DEBUG_IO_INDEX); 	//output 1
}


__INLINE__
void debug_io_0(void)
{
    P11_PORT->PB_SEL |= BIT(P11_DEBUG_IO_INDEX); 	//P11 Control
    P11_PORT->PB_DIR &= ~BIT(P11_DEBUG_IO_INDEX); 	//dir output
    P11_PORT->PB_OUT &= ~BIT(P11_DEBUG_IO_INDEX); 	//output 0
}

void debug_io(void)
{
    while (1) {
        debug_io_1();
        debug_io_0();
    }
}

