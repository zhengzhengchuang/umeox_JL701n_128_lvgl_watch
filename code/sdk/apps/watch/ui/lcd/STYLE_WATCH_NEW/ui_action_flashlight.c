#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "ui/res_config.h"

#include "app_task.h"
#include "system/timer.h"
#include "key_event_deal.h"
#include "device/device.h"


#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_FLASHLIGHT

#define STYLE_NAME  JL

static u16 timer = 0;

static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    /* log_info("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec); */
    dev_close(fd);
}



static void refresh()
{
    if (!timer) {
        return ;
    }
    struct sys_time time_t;
    struct utime ui_time = {0};
    get_sys_time(&time_t);
    ui_time.hour = time_t.hour;
    ui_time.min = time_t.min;
    ui_time.sec = time_t.sec;
    ui_time_update_by_id(FLASHLIGHT_OFF_TIME, &ui_time);
    /* ui_time_update(ui_core_get_element_by_id(id), &ui_time); */
}


static time_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_time *time = (struct ui_time *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        struct sys_time time_t;
        struct utime ui_time = {0};

        get_sys_time(&time_t);
        ui_time.hour = time_t.hour;
        ui_time.min = time_t.min;
        ui_time.sec = time_t.sec;
        ui_time_update(time, &ui_time);
        if (!timer) {
            timer = sys_timer_add(NULL, refresh, 500);
        }
        break;
    case ON_CHANGE_RELEASE:
        if (timer) {
            sys_timer_del(timer);
            timer = 0;
        }
        break;
    }
    return 0;
}

REGISTER_UI_EVENT_HANDLER(FLASHLIGHT_OFF_TIME)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = time_onchange,
};



static int FLASHLIGHT_OFF_LAYOUT_onchane(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        /* ui_auto_shut_down_disable(); */
        break;
    case ON_CHANGE_RELEASE:
        /* ui_auto_shut_down_enable(); */
        break;
    case ON_CHANGE_FIRST_SHOW:

        break;
    default:
        return false;
    }
    return false;
}


static int FLASHLIGHT_OFF_LAYOUT_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        ui_hide(FLASHLIGHT_OFF_LAYOUT);
        ui_show(FLASHLIGHT_ON_LAYOUT);
        return true;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(FLASHLIGHT_OFF_LAYOUT)
.ontouch = FLASHLIGHT_OFF_LAYOUT_ontouch,
 .onkey = NULL,
  .onchange  = FLASHLIGHT_OFF_LAYOUT_onchane,
};



static int FLASHLIGHT_ON_LAYOUT_onchane(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        ui_auto_shut_down_disable();
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    default:
        return false;
    }
    return false;
}


static int FLASHLIGHT_ON_LAYOUT_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        ui_hide(FLASHLIGHT_ON_LAYOUT);
        ui_show(FLASHLIGHT_OFF_LAYOUT);
        return true;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(FLASHLIGHT_ON_LAYOUT)
.ontouch = FLASHLIGHT_ON_LAYOUT_ontouch,
 .onkey = NULL,
  .onchange  = FLASHLIGHT_ON_LAYOUT_onchane,
};

#endif /* #if TCFG_UI_ENABLE_FLASHLIGHT */
#endif /* #if (!TCFG_LUA_ENABLE) */


