#include "common.h"
#include "asm/irq.h"

static void (*timer_cb)(void) = NULL;
static volatile u32 tmr_2ms_cnt = 0;

u32 get_osc_clk_freq(void)
{
    return (24 * 1000 * 1000L);
}

static u32 get_pll_12M_clk_freq(void)
{
    return (12 * 1000 * 1000L);
}

u32 tmr_2ms_cnt_get(void)
{
    return tmr_2ms_cnt;
}

void delay_2ms(u32 ms)
{
    u32 wait_ms = tmr_2ms_cnt_get() + ms;
    while (wait_ms >= tmr_2ms_cnt_get());
}

SET_INTERRUPT
void timer1_isr()
{
    JL_TIMER1->CON |= BIT(14);

    tmr_2ms_cnt ++;

    if (!(tmr_2ms_cnt % 5)) { //10ms
        if (timer_cb) {
            timer_cb();         //systimer调度10ms一次
        }
    }

    if (!(tmr_2ms_cnt % 250)) { //500ms
        putchar('$');
    }
    /* JL_PORTA->DIE |= BIT(4);
    JL_PORTA->DIR &= ~BIT(4);
    JL_PORTA->OUT ^= BIT(4); */

}

void timer1_init(void (*cb)(void))
{
    timer_cb = cb;
    request_irq(IRQ_TIME1_IDX, 1, timer1_isr, 0);

    JL_TIMER1->CNT = 0;
    JL_TIMER1->PRD = get_osc_clk_freq() / 64 / 500; //2ms
    JL_TIMER1->CON = BIT(0) | BIT(4) | BIT(5) | BIT(10) | BIT(14); //开启timer时要清pending(BIT(14)),CLK_SRC:OSC
}

/* //AT(.volatile_ram_code) */
/* void udelay(u32 nus) */
/* { */
/*     //如果不分频跑的时钟超过sys时钟会等不到pending; */
/*     JL_TIMER3->CON = BIT(14); */
/*     JL_TIMER3->CNT = 0; */
/* #<{(| #if CRYSTAL_FREE_EN |)}># */
/* #<{(|     JL_IOMAP->CON1 |= BIT(27);		//bit27:0:pb6 1:pll_12m |)}># */
/* #<{(|     JL_TIMER3->PRD = get_pll_12M_clk_freq() / 4 / 1000000  * nus; //1us |)}># */
/* #<{(|     JL_TIMER3->CON = BIT(0) | BIT(2) | BIT(4); //io clk,4分频 |)}># */
/* #<{(| #else |)}># */
/*     JL_TIMER3->PRD = get_osc_clk_freq() / 4 / 1000000  * nus; //1us */
/*     JL_TIMER3->CON = BIT(0) | BIT(3) | BIT(4); //io clk,4分频 */
/* #<{(| #endif |)}># */
/*     while ((JL_TIMER3->CON & BIT(15)) == 0) { */
/*     } */
/*     JL_TIMER3->CON = BIT(14); */
/* } */
/*  */
/* //2020.4.8卡过20ms延时是准确的; */
/* void mdelay(u32 nms) */
/* { */
/*     u32 i; */
/*     for (i = 0; i < nms; i++) { */
/*         udelay(1000); */
/*     } */
/* } */
/*  */
