#include "system/includes.h"
#include "media/includes.h"
#include "app_config.h"
#include "tone_player.h"
#include "asm/charge.h"
#include "app_charge.h"
#include "app_main.h"
#include "ui_manage.h"
#include "vm.h"
#include "app_chargestore.h"
#include "user_cfg.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "key_event_deal.h"

#if TCFG_UI_ENABLE
#if UI_UPGRADE_RES_ENABLE
#endif /* #if TCFG_UI_ENABLE */
#include "smartbox_extra_flash_opt.h"
#endif /* #if UI_UPGRADE_RES_ENABLE */

#define LOG_TAG_CONST       APP_IDLE
#define LOG_TAG             "[APP_IDLE]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#if (TCFG_SPI_LCD_ENABLE)
#include "ui/ui_style.h"
#endif

static u8 poweron_idle = 0;


//映射id
int ui_id2remap(int id)
{
#if TCFG_UI_ENABLE
#if UI_UPGRADE_RES_ENABLE
    static int i = 0;
    if (ui_id2type(id) == CTRL_TYPE_WINDOW) {
        if (smartbox_eflash_flag_get() || smartbox_eflash_update_flag_get()) {
            printf("id remap %x\n", ID_WINDOW_UPGRADE);
            return ID_WINDOW_UPGRADE;
        }
    }
#endif
#endif /* #if TCFG_UI_ENABLE */
    return id;
}


#if (TCFG_UI_ENABLE && TCFG_SPI_LCD_ENABLE)
static int logo_time = 0;

static void  lcd_ui_show_probe()
{
    UI_SHOW_WINDOW(ID_WINDOW_POWER_ON);
    sys_key_event_enable();
    logo_time = timer_get_ms();
}

static void  lcd_ui_show_end()
{
    while (timer_get_ms() - logo_time <= 2 * 1000) { //显示开机logo
        os_time_dly(2);
#if PT_GPIO_CHECK_LCD_TP
        extern int product_test_check_run(void);
        if (product_test_check_run()) {
            break;
        }
#endif /* #if PT_GPIO_CHECK_LCD_TP */
    }
    UI_HIDE_WINDOW(ID_WINDOW_POWER_ON);

    extern void ui_auto_shut_down_enable(void);
    ui_auto_shut_down_enable();
}

#endif


static void  lcd_ui_power_on()
{
#if (TCFG_UI_ENABLE && TCFG_SPI_LCD_ENABLE)
    int logo_time = 0;

    UI_SHOW_WINDOW(ID_WINDOW_POWER_ON);
    sys_key_event_enable();
    logo_time = timer_get_ms();
    while (timer_get_ms() - logo_time <= 2 * 1000) { //显示开机logo
        os_time_dly(2);
#if PT_GPIO_CHECK_LCD_TP
        extern int product_test_check_run(void);
        if (product_test_check_run()) {
            break;
        }
#endif /* #if PT_GPIO_CHECK_LCD_TP */
    }
    UI_HIDE_WINDOW(ID_WINDOW_POWER_ON);

    extern void ui_auto_shut_down_enable(void);
    ui_auto_shut_down_enable();
    /* return 0; */
#endif
}

static int power_on_init(void)
{
#if 0
    UI_SHOW_WINDOW(PAGE_3);
    return 0;
#endif

    ///有些需要在开机提示完成之后再初始化的东西， 可以在这里初始化
#if (TCFG_UI_ENABLE && TCFG_SPI_LCD_ENABLE)

    extern int ui_watch_poweron_update_check();
    if (ui_watch_poweron_update_check() != 0) {
        return -1;
    }

    /* lcd_ui_power_on();//由ui决定切换的模式 */
    lcd_ui_show_end();
    /* return 0; */
#endif

    printf("----->%s, %d\n", __FUNCTION__, __LINE__);
#if TCFG_APP_BT_EN
    app_task_switch_to(APP_BT_TASK);
#else
    app_task_switch_to(APP_MUSIC_TASK);
#endif

    return 0;
}

static int power_on_unint(void)
{
    UI_SHOW_WINDOW(ID_WINDOW_DIAL);

    tone_play_stop();
    /* UI_HIDE_CURR_WINDOW(); */
    return 0;
}


static int poweron_sys_event_handler(struct sys_event *event)
{
    switch (event->type) 
    {
        case SYS_KEY_EVENT:
            break;
        case SYS_BT_EVENT:
            break;
        case SYS_DEVICE_EVENT:
            break;
        default:
            return false;
    }
    return false;
}


static void  tone_play_end_callback(void *priv, int flag)
{
    int index = (int)priv;

    if (APP_POWERON_TASK != app_get_curr_task()) {
        log_error("tone callback task out \n");
        return;
    }

    switch (index) 
    {
        case IDEX_TONE_POWER_ON:
            power_on_init();
            break;
    }
}


void app_poweron_task()
{
    int msg[32];

    //UI_SHOW_MENU(MENU_POWER_UP, 0, 0, NULL);

#if (TCFG_UI_ENABLE && TCFG_SPI_LCD_ENABLE)
    extern int ui_watch_poweron_update_check();
    if (ui_watch_poweron_update_check() == 0) {
        lcd_ui_show_probe();//由ui决定切换的模式
    }
#endif

    int err =  tone_play_with_callback_by_name(\
        (char *)tone_table[IDEX_TONE_POWER_ON], 1, \
            tone_play_end_callback, (void *)IDEX_TONE_POWER_ON);
    if (err) { //提示音没有,播放失败，直接init流程
        power_on_init();
    }


    while (1) {
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);
        switch (msg[0]) {
        case APP_MSG_SYS_EVENT:
            if (poweron_sys_event_handler((struct sys_event *)(msg + 1)) == false) {
                app_default_event_deal((struct sys_event *)(&msg[1]));    //由common统一处理
            }
            break;
        default:
            break;
        }

        if (app_task_exitting()) 
        {
            power_on_unint();
            poweron_idle = 1;
            return;
        }
    }

}

static u8 poweron_idle_query(void)
{
    return poweron_idle;
}

REGISTER_LP_TARGET(poweron_lp_target) = {
    .name = "power_on",
    .is_idle = poweron_idle_query,
};

