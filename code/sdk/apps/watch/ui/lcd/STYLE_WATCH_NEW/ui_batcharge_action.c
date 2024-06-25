#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "asm/chargestore.h"
#include "app_power_manage.h"
#include "asm/charge.h"

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"




#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE

#define STYLE_NAME  JL

static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    dev_close(fd);
}


struct progress_bat_priv {
    u8 hour;//0~23
    u8 min;//0~59
    /* u8 sec; */
    u8 bat;//0~100
    /* int sec; */
    u8 show_temp;
    int timer;
};

static struct progress_bat_priv *__this = NULL;

static void PROGRESS_CHARGE_timer(void *priv)
{
    if (!__this) {
        return;
    }

    struct sys_time time;
    struct utime time_r;

    u8 percent = get_vbat_percent();
    if (get_charge_online_flag()) { //充电时候图标动态效果
        if (__this->show_temp > percent) {
            __this->show_temp = 0;
        }
        ui_progress_set_persent_by_id(BATCHARGE_PROGRESS, __this->show_temp);
        __this->show_temp += 10;
    } else {
        ui_progress_set_persent_by_id(BATCHARGE_PROGRESS, percent);
    }

    struct unumber numb_hour;
    numb_hour.type = TYPE_NUM;
    numb_hour.numbs = 1;
    numb_hour.number[0] = percent;
    ui_number_update_by_id(BATCHARGE_POWER, &numb_hour);

    get_sys_time(&time);
    time_r.hour = time.hour;
    time_r.min = time.min;
    time_r.sec = time.sec;
    ui_time_update_by_id(BATCHARGE_TIME, &time_r);

}


static int PROGRESS_CHARGE_init(int p)
{
    struct sys_time time;
    struct utime time_r;
    u8 percent = get_vbat_percent();

    struct unumber numb_hour;
    numb_hour.type = TYPE_NUM;
    numb_hour.numbs = 1;
    numb_hour.number[0] = percent;
    ui_number_update_by_id(BATCHARGE_POWER, &numb_hour);

    get_sys_time(&time);
    time_r.hour = time.hour;
    time_r.min = time.min;
    time_r.sec = time.sec;
    ui_time_update_by_id(BATCHARGE_TIME, &time_r);
    return 0;
}

static int PROGRESS_CHARGE_onchange(void *ctr, enum element_change_event e, void *arg)
{
    int index;
    struct ui_progress *progress = (struct ui_progress *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        if (!__this) {
            __this = zalloc(sizeof(struct progress_bat_priv));
        }

        if (!__this->timer) {
            __this->timer = sys_timer_add(NULL, PROGRESS_CHARGE_timer, 1000);
        }
        break;
    case ON_CHANGE_FIRST_SHOW:
        ui_set_call(PROGRESS_CHARGE_init, 0);
        break;

    case ON_CHANGE_RELEASE:
        if (__this && __this->timer) {
            sys_timer_del(__this->timer);
        }

        if (__this) {
            free(__this);
            __this = NULL;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE;
}


REGISTER_UI_EVENT_HANDLER(BATCHARGE_PROGRESS)
.onchange = PROGRESS_CHARGE_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

