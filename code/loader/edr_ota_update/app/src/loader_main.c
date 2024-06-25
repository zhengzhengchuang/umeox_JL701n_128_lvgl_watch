#include "common.h"
#include "asm/clock.h"
#include "asm/irq.h"
#include "asm/wdt.h"
#include "asm/uart.h"
#include "generic/printf.h"
#include "asm/setup.h"
#include "dec.h"
#include "jlfs.h"
#include "asm/delay.h"
#include "upgrade.h"
#include "user.h"
#include "sys_timer.h"
#include "asm/debug.h"
#include "update_main.h"
#include "norflash.h"
#include "asm/power/power_reset.h"

#define LOG_TAG_CONST       LOADER_MAIN
#define LOG_TAG             "[LOADER_MAIN]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

/* #define IO_N 3 */
/* void test_io(int x) */
/* { */
/*     JL_PORTB->DIR &= ~BIT(IO_N); */
/*     #<{(| mdelay(20); |)}># */
/*     for (int i = 0; i < x; i++) */
/*     { */
/*         JL_PORTB->OUT ^= BIT(IO_N); */
/*     } */
/*     JL_PORTB->OUT |= BIT(IO_N); */
/* } */


#include "asm/power/p33.h"
#include "asm/gpio.h"
extern u32 get_power_pin_multi(u32 *gpio, u8 *level, u32 cnt);
#define PP_N  3//最多记录power_pin脚数
typedef struct  {
    u32 gpio[PP_N];
    u8 level[PP_N];
} PowerPin;
static PowerPin pp_handle = {0};

//================================================//
//          不可屏蔽中断使能配置(UNMASK_IRQ)      //
//          	Only For AC701N                   //
//================================================//
const int CONFIG_CPU_UNMASK_IRQ_ENABLE = 0;

void pinr_tmr_restart(void)
{
    /* putchar('K'); */
    u8 reg;
    reg = P33_CON_GET(P3_PINR_CON1);
    //开启LDO5V_DET长按复位
    if ((reg & BIT(0)) && ((reg & BIT(3)) == 0)) {
        P33_CON_SET(P3_PINR_CON1, 2, 1, 0);
        P33_CON_SET(P3_PINR_CON1, 2, 1, 1);
        P33_CON_SET(P3_PINR_CON1, 4, 3, 3); //设置长按8s复位。
    }
}

void  soft_irq_handler(void)
{
    printf("%s\n", __func__);
}

u32 stack_magic[4] sec(.stack_magic);
u32 stack_magic0[4] sec(.stack_magic0);
void flash_fs_v2_update_init(void);

int main(void)
{
    u8 val[16];
    u8 pll_scr = 0;


    /* JL_CLOCK->CLK_CON0 |= BIT(8); //OSC24M -> STD24M  */
    /* uart_init("PA05", 2000000); //debug串口 */
    norflash_init(2, 3, 0);

    flash_fs_v2_update_init(); //存在偏移情况，先提前获取。
#ifdef __PRINTF_DEBUG
    u8 *ptr = jlfs_get_isd_cfg_ptr();
    memset(val, 0, sizeof(val));
    dec_isd_cfg_ini("PLL_SRC", val, ptr);
    if (strcmp((char *)val, "LRC") == 0) {
        pll_scr = 2;
    }
    u32 ut_buad = 0;
    char uttx[8] = {0};
    memset(uttx, 0, sizeof(uttx));
    dec_isd_cfg_ini("UTTX", uttx, ptr);
    dec_isd_cfg_ini("UTBD", &ut_buad, ptr);

    uart_init(uttx, ut_buad);

    reset_source_dump();
#endif

    /* uart_init("USBDP", 1000000); //debug串口 */
    log_info("\n******************  Hello Ota loader DATE:%s TIME:%s *****************\n\n", __DATE__, __TIME__);


    //不关闭长按复位,打印复位寄存器值方便查看  2023_2_15 by phewlee
    printf(">>>[test]:P3_PINR_CON = 0x%x, P3_PINR_CON1 = 0x%x\n", P3_PINR_CON, P3_PINR_CON1);
    /* power_reset_close(); */

    extern void lrc_init(void);
    lrc_init();

    wdt_init(WDT_4s);

#if defined(CONFIG_CPU_BR28) && EDR_UPDATA_SUPPORT_CONNECT
    extern void irq_ipmask_reg_enable(void);
    irq_ipmask_reg_enable();
#endif

    /*初始化异常中断*/
    request_irq(IRQ_EXCEPTION_IDX, 2, exception_irq_handler, 0);
    debug_init();

    /* request_irq(IRQ_SOFT0_IDX, 2, soft_irq_handler, 0); */
    /*  */
    /* irq_set_pending(IRQ_SOFT0_IDX); */
    /* while(1); */
    /*初始化sys_timer*/
    sys_timer_init();

    //ldoin长按复位功能如果开启，定时出现启动
    sys_timer_add(NULL, pinr_tmr_restart, 500);


    //获取POWER_PIN参数, 防止部分触摸ic掉电一半再上电状态异常
#if (0 == UART_UPDATE_ONLY_TEST_MODE)
    if (get_power_pin_multi(&pp_handle.gpio[0], &pp_handle.level[0], PP_N)) {
        for (int i = 0; i < PP_N; i++) {
            printf(">>>[test]:gpio[%d] = %d\n", i, pp_handle.gpio[i]);
            if (pp_handle.gpio[i] == 0) {
                break;
            }
            gpio_set_direction(pp_handle.gpio[i], 0);
            gpio_set_die(pp_handle.gpio[i], 1);
            //先输出低是为了释放该io的电容的电，达到快速拉低目的,防止部分触摸ic掉电一半再上电状态异常;
            gpio_set_output_value(pp_handle.gpio[i], 0);
            udelay(100);
            gpio_set_output_value(pp_handle.gpio[i], pp_handle.level[i]);
        }
    }
#endif

    arch_init();

#if defined(CONFIG_CPU_BR28)
    extern void norflash_write_protect(void);
    norflash_write_protect();
#endif

    update_main();

    while (1) {
        putchar('o');
        udelay(1000 * 1000);
    };

    return 0;
}

