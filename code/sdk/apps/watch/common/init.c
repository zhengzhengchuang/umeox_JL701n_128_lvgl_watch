
#include "app_config.h"
#include "system/includes.h"
#include "asm/charge.h"
#include "app_power_manage.h"
#include "update.h"
#include "app_main.h"
#include "app_charge.h"
#include "update_loader_download.h"
#include "audio_config.h"
#include "../../device/motor/motor.h"


extern void setup_arch();
extern int audio_dec_init();
extern int audio_enc_init();
extern void clr_wdt(void);

static void do_initcall()
{
    __do_initcall(initcall);
}

static void do_early_initcall()
{
    __do_initcall(early_initcall);
}

static void do_late_initcall()
{
    __do_initcall(late_initcall);
}

static void do_platform_initcall()
{
    __do_initcall(platform_initcall);
}

static void do_module_initcall()
{
    __do_initcall(module_initcall);
}

void __attribute__((weak)) board_init()
{

}
void __attribute__((weak)) board_early_init()
{

}

int eSystemConfirmStopStatus(void)
{
    /* 系统进入在未来时间里，无任务超时唤醒，可根据用户选择系统停止，或者系统定时唤醒(100ms) */
    //1:Endless Sleep
    //0:100 ms wakeup
    if (IS_CHARGE_EN()) {
        return 1;
    }
    if (get_charge_full_flag()) {
        log_i("Endless Sleep");
        power_set_soft_poweroff();
        return 1;
    } else {
        log_i("100 ms wakeup");
        return 0;
    }
}

static bool power_on = true;
bool GetPowerOnStatus(void)
{
    return power_on;
}

void SetPowerOnStatus(bool status)
{
    power_on = status;

    return;
}

static void check_power_on_key(void)
{
    u32 delay_10ms_cnt = 0;
#if PRODUCT_TEST_ENABLE
    u8 pt_result;
    int r = syscfg_read(PT_TEST_RESULT, (u8 *)&pt_result, sizeof(pt_result));
    if (r <= 0) {
        return;
        // log_info("vm read pt_test_result faild\n");
    }
#endif
    while (1) 
    {
        clr_wdt();
        os_time_dly(1);

        extern u8 get_power_on_status(void);  
        if(get_power_on_status()) 
        {
            putchar('+');
            delay_10ms_cnt++;
            SetPowerOnStatus(true);
            if(delay_10ms_cnt > 100) 
                return;
        }else 
        {
            putchar('-');
            delay_10ms_cnt = 0;
            puts("enter softpoweroff\n");
            power_set_soft_poweroff();
        }
    }
}

static void app_init()
{
    int update = 0;

    do_early_initcall();
    do_platform_initcall();

    board_init();
    
    do_initcall();

    do_module_initcall();
    do_late_initcall();

    audio_enc_init();
    audio_dec_init();

//user add 升级完成越过开机按键监测
#if 0
    if (!UPDATE_SUPPORT_DEV_IS_NULL()) {
        update = update_result_deal();
        printf("%s:update = %d\n", __func__, update);
    }
#endif

    app_var.play_poweron_tone = 1;

    if(!get_charge_online_flag()) 
    {
        check_power_on_voltage();

#if TCFG_POWER_ON_NEED_KEY
        /*充电拔出,CPU软件复位, 不检测按键，直接开机*/
        extern u8 get_alarm_wkup_flag(void);
#if TCFG_CHARGE_OFF_POWERON_NE
        if ((!update && cpu_reset_by_soft()) || is_ldo5v_wakeup() || get_alarm_wkup_flag()) {
#else
    
        if((!update && cpu_reset_by_soft()) || get_alarm_wkup_flag() || \
            (is_reset_source(P33_VDDIO_LVD_RST) | is_reset_source(P33_VDDIO_POR_RST))) {
#endif
            app_var.play_poweron_tone = 0;
        } else {
            check_power_on_key();
        }
#endif
    }

#if (TCFG_MC_BIAS_AUTO_ADJUST == MC_BIAS_ADJUST_POWER_ON)
    u8 por_flag = 0;
    u8 cur_por_flag = 0;
#if defined(CONFIG_CPU_BR23)
    extern u8 power_reset_src;
    /*
     *1.update
     *2.power_on_reset(BIT0:上电复位)
     *3.pin reset(BIT4:长按复位)
     */
    if (update || (power_reset_src & BIT(0)) || (power_reset_src & BIT(4))) {
        //log_info("reset_flag:0x%x",power_reset_src);
        cur_por_flag = 0xA5;
    }
#else /* #if defined(CONFIG_CPU_BR23) */
    u32 reset_src = get_reset_source_value();
    if (update || (reset_src & BIT(MSYS_POWER_RETURN)) || (reset_src & BIT(P33_PPINR_RST))) {
        //log_info("reset_flag:0x%x",reset_src);
        cur_por_flag = 0xA5;
    }
#endif /* #if defined(CONFIG_CPU_BR23) */

    int ret = syscfg_read(CFG_POR_FLAG, &por_flag, 1);
    if ((cur_por_flag == 0xA5) && (por_flag != cur_por_flag)) {
        //log_info("update POR flag");
        ret = syscfg_write(CFG_POR_FLAG, &cur_por_flag, 1);
    }
#endif

#if (TCFG_CHARGE_ENABLE && TCFG_CHARGE_POWERON_ENABLE)
#if 0 //user add
    if(is_ldo5v_wakeup()) 
    {   //LDO5V唤醒
        extern u8 get_charge_online_flag(void);
        if (get_charge_online_flag()) { //关机时，充电插入

        } else { //关机时，充电拔出
            power_set_soft_poweroff();
        }
    }
#endif
#endif

    motor_run(1, def_motor_duty);

    nor_flash_vm_init();

#if (TCFG_CHARGE_BOX_ENABLE)
    /* clock_add_set(CHARGE_BOX_CLK); */
    chgbox_init_app();
#endif

#if PRODUCT_TEST_ENABLE
    int product_test_open(void);
    product_test_open();
#endif /* #if PRODUCT_TEST_ENABLE */
}

static void app_task_handler(void *p)
{
    app_init();
    app_main();
}

__attribute__((used)) int *__errno()
{
    static int err;
    return &err;
}

int main()
{
#if defined(CONFIG_CPU_BR23)
    clock_set_sfc_max_freq(104 * 1000000, 104 * 1000000);
    clock_reset_lsb_max_freq(80 * 1000000);
#elif defined(CONFIG_CPU_BR28)
    clock_set_sfc_max_freq(104 * 1000000, 104 * 1000000);
#endif
    wdt_close();

    os_init();

    setup_arch();

    board_early_init();

    task_create(app_task_handler, NULL, "app_core");

    os_start();

    local_irq_enable();

    while (1) {
        asm("idle");
    }

    return 0;
}

