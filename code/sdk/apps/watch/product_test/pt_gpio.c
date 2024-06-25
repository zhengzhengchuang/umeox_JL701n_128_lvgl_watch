#include "product_test.h"
#include "pt_gpio.h"
#include "asm/gpio.h"
#include "ui/ui_api.h"

#if PT_GPIO_ENABLE

#define GPIO_CHECK_DELAY_US			10

// 记录PCBA中没有外接设备的GPIO。
// 相邻的写在一起，和其他的块之间以IO_PORT_MAX间隔
// 此处以7013为例，PCBA不接屏，不做屏幕测试
static const u8 idle_gpio_list[] = {
    IO_PORTB_09, IO_PORT_MAX, // KEY
    IO_PORTB_08, IO_PORT_MAX, // null
    IO_PORTB_01, IO_PORTB_00, IO_PORT_MAX, // KEY
    IO_PORTC_07, IO_PORTC_06, IO_PORT_MAX, // KEY
#if PT_GPIO_CHECK_LCD_TP
    IO_PORTA_15, IO_PORTA_14, IO_PORTA_13, IO_PORT_MAX, // LCD D0/D1/D2
    IO_PORTA_12, IO_PORTA_11, IO_PORTA_10, IO_PORTA_09, IO_PORTA_08, IO_PORT_MAX, // LCD D3/D4/D5/D6/D7
    IO_PORTA_07, IO_PORTA_06, IO_PORTA_05, IO_PORT_MAX, // LCD CLK/CS/WR
    IO_PORTA_03, IO_PORTA_02, IO_PORT_MAX, // LCD TE/RST
    IO_PORTG_06, IO_PORTG_05, IO_PORTG_04, IO_PORTG_03, IO_PORT_MAX, // TP RST/INT/SDA/SCL
#endif /* #if PT_GPIO_CHECK_LCD_TP */
};
static const u32 gpio_regs[] = {
    (u32) JL_PORTA,
    (u32) JL_PORTB,
    (u32) JL_PORTC,
    (u32) JL_PORTD,
    (u32) JL_PORTE,
    (u32) JL_PORTG,
    (u32) JL_PORTP,
};
struct pt_gpio {
    unsigned int out;
    unsigned int dir;
    unsigned int pu;
    unsigned int pd;
};
struct pt_gpio pt_gpio1;
struct pt_gpio pt_gpio2;
struct gpio_reg *pt_gpio_status;
void gpio_status_config(u32 gpio, u32 mode)
{
    pt_gpio_status = (struct gpio_reg *)gpio_regs[gpio / IO_GROUP_NUM];
    u32 pt_pin = gpio % 16;
    switch (mode & 0xf) {
    case 0://保存gpio1的状态
        pt_gpio1.out = pt_gpio_status->out & BIT(pt_pin);
        pt_gpio1.dir = pt_gpio_status->dir & BIT(pt_pin);
        pt_gpio1.pu = pt_gpio_status->pu & BIT(pt_pin);
        pt_gpio1.pd = pt_gpio_status->pd & BIT(pt_pin);
        break;
    case 1://恢复gpio1的状态
        pt_gpio_status->out |= pt_gpio1.out;
        pt_gpio_status->dir |= pt_gpio1.dir;
        pt_gpio_status->pu |= pt_gpio1.pu;
        pt_gpio_status->pd |= pt_gpio1.pd;
        break;
    case 2://保存gpio2的状态
        pt_gpio2.out = pt_gpio_status->out & BIT(pt_pin);
        pt_gpio2.dir = pt_gpio_status->dir & BIT(pt_pin);
        pt_gpio2.pu = pt_gpio_status->pu & BIT(pt_pin);
        pt_gpio2.pd = pt_gpio_status->pd & BIT(pt_pin);
        break;
    case 3://恢复gpio2的状态
        pt_gpio_status->out |= pt_gpio2.out;
        pt_gpio_status->dir |= pt_gpio2.dir;
        pt_gpio_status->pu |= pt_gpio2.pu;
        pt_gpio_status->pd |= pt_gpio2.pd;
        break;
    }

}

static u8 pt_gpio_busy = 0; // 忙碌标记
static u8 pt_gpio_res = PT_E_SYS_UNREADY;      		// 测试结果

static int idle_gpio_test(void)
{
    int ret = true;
    for (int i = 0; i < sizeof(idle_gpio_list) - 1; i++) {
        if (idle_gpio_list[i] == IO_PORT_MAX) {
            continue;
        }
        local_irq_disable();
        // 保存idle_gpio_list[i]状态
        u32 mode = 0;
        gpio_status_config(idle_gpio_list[i], mode);
        if (idle_gpio_list[i + 1] != IO_PORT_MAX) {
            // 保存idle_gpio_list[i+1]状态
            mode = 2;
            gpio_status_config(idle_gpio_list[i + 1], mode);

            // idle_gpio_list[i+1]输出高电平
            // idle_gpio_list[i]下拉输入
            // delay
            // 检测idle_gpio_list[i]，高电平标记出错并退出
            gpio_set_output_value(idle_gpio_list[i + 1], 1);
            gpio_set_direction(idle_gpio_list[i], 1);
            gpio_set_die(idle_gpio_list[i], 1);
            gpio_set_pull_down(idle_gpio_list[i], 1);
            gpio_set_pull_up(idle_gpio_list[i], 0);
            udelay(100);
            if (gpio_read(idle_gpio_list[i])) {
                local_irq_enable();
                printf("%s,%d, i:%d, io:%d \n", __func__, __LINE__, i, idle_gpio_list[i]);
                return false;
            }

            // idle_gpio_list[i+1]输出低电平
            // idle_gpio_list[i]上拉输入
            // delay
            // 检测idle_gpio_list[i]，低电平标记出错并退出
            gpio_set_output_value(idle_gpio_list[i + 1], 0);
            gpio_set_direction(idle_gpio_list[i], 1);
            gpio_set_die(idle_gpio_list[i], 1);
            gpio_set_pull_up(idle_gpio_list[i], 1);
            gpio_set_pull_down(idle_gpio_list[i], 0);
            udelay(100);
            if (gpio_read(idle_gpio_list[i]) == 0) {
                local_irq_enable();
                printf("%s,%d, i:%d, io:%d \n", __func__, __LINE__, i, idle_gpio_list[i]);
                return false;
            }

            // 恢复idle_gpio_list[i+1]状态
            mode = 3;
            gpio_status_config(idle_gpio_list[i + 1], mode);
        } else {
            // idle_gpio_list[i]下拉输入
            // delay
            // 检测idle_gpio_list[i]，高电平标记出错并退出
            gpio_set_direction(idle_gpio_list[i], 1);
            gpio_set_pull_down(idle_gpio_list[i], 1);
            gpio_set_pull_up(idle_gpio_list[i], 0);
            gpio_set_die(idle_gpio_list[i], 1);
            udelay(100);
            if (gpio_read(idle_gpio_list[i])) {
                local_irq_enable();
                printf("%s,%d, i:%d, io:%d \n", __func__, __LINE__, i, idle_gpio_list[i]);
                return false;
            }

            // idle_gpio_list[i]上拉输入
            // delay
            // 检测idle_gpio_list[i]，低电平标记出错并退出
            gpio_set_direction(idle_gpio_list[i], 1);
            gpio_set_pull_up(idle_gpio_list[i], 1);
            gpio_set_pull_down(idle_gpio_list[i], 0);
            gpio_set_die(idle_gpio_list[i], 1);
            udelay(100);
            if (gpio_read(idle_gpio_list[i]) == 0) {
                local_irq_enable();
                printf("%s,%d, i:%d, io:%d \n", __func__, __LINE__, i, idle_gpio_list[i]);
                return false;
            }

        }

        // 恢复idle_gpio_list[i]状态
        mode = 1;
        gpio_status_config(idle_gpio_list[i], mode);

        local_irq_enable();
    }
    return true;
}

static int pt_gpio_test(int priv)
{
    u32 result = PT_E_OK;
    if (idle_gpio_test() == false) {
        result = PT_E_MOD_ERROR;
    }
    pt_gpio_busy = 0;

    pt_gpio_res = result;
    /* product_test_push_data(PT_ORDER_M_SET(PT_M_GPIO) | PT_ORDER_C_SET(PT_N_C_PUSH_RESULT), 4, &result); */
    /* product_test_push_data(PT_ORDER_M_SET(PT_M_SYSTEM) | PT_ORDER_C_SET(PT_S_C_HOLD), 4, &result); */

    return 0;
}

int pt_gpio_start(void)
{
    if (pt_gpio_busy) {
        return PT_E_MOD_RUN;
    }
#if PT_GPIO_CHECK_LCD_TP
    ui_set_shut_down_time_ms(10);
    ui_auto_shut_down_modify();
    u8 cnt = 100;
    while (!lcd_sleep_status()) {
        os_time_dly(1);
        if (cnt == 0) {
            break;
        }
        cnt --;
    }
    y_printf("lcd_sleep_status:%d \n", lcd_sleep_status());
    void imd_sfr_poweroff(void);
    imd_sfr_poweroff();
#endif /* #if PT_GPIO_CHECK_LCD_TP */

    pt_gpio_res = PT_E_MOD_RUN;
    int msg[3] = {0};
    msg[0] = (int)pt_gpio_test;
    msg[1] = 1;
    msg[2] = (int)0;
    do {
        int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
        if (os_err == OS_ERR_NONE) {
            break;
        }
        if (os_err != OS_Q_FULL) {
            pt_gpio_res = PT_E_SYS_ERROR;
            return PT_E_SYS_ERROR;
        }
        os_time_dly(1);
    } while (1);
    /* pt_gpio_busy = 1; */

    return 0;
}

int pt_gpio_stop(void)
{
    if (pt_gpio_busy) {
        return PT_E_MOD_CANT_STOP;
    }
    if (pt_gpio_res == PT_E_MOD_RUN) {
        pt_gpio_res = PT_E_MOD_STOP_NO_END;
    }
    return 0;
}

int pt_gpio_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:
        result = pt_gpio_start();
        break;
    case PT_N_C_STOP:
        result = pt_gpio_stop();
        break;
    case PT_N_C_GET_RESULT:
        result = pt_gpio_res;
        break;
    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}

REGISTER_PT_MODULE(gpio) = {
    .module = PT_M_GPIO,
    .attr	= PT_ATTR_SELF,
    .init	= NULL,
    .ioctrl	= pt_gpio_ioctrl,
};


#endif /* #if PT_GPIO_ENABLE */

