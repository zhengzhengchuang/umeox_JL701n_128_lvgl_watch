#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "app_main.h"
#include "init.h"
#include "key_event_deal.h"
#include "device/device.h"
#include "app_power_manage.h"
#include "btstack/avctp_user.h"
#include "asm/charge.h"


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_PULLDOWN_MENU

/* #define STYLE_NAME  DIAL */
#define STYLE_NAME  JL

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



/* REGISTER_UI_STYLE(STYLE_NAME) */

static int refresh_timer;
static u8 bright_screen_on_off = 0;
static u8 undisturb_ensure_release_flag = 0;
static u8 percent_str[5];

extern u8 get_light_time_sel();
extern void screen_light_alway_switch(u8 on);
extern u8 get_all_day_undisturb_sel();
extern void set_all_day_undisturb_sel(u8 sel);

u8 get_undisturb_ensure_release_flag()
{
    return undisturb_ensure_release_flag;
}

void set_undistrub_ensure_release_flag(u8 flag)
{
    undisturb_ensure_release_flag = 0;
}

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

static void get_sys_time_weekday(struct sys_time *time)
{
    u8 weekday = 0;
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    weekday = dev_ioctl(fd, IOCTL_GET_WEEKDAY, (u32)time);
    log_info(">>>>>>>weekday:%d", weekday);
    dev_close(fd);
}

static void num2string(u8 num, u8 *buf)
{
    u8 len = 0;
    if ((num >= 100) && (num <= 999)) {
        buf[len++] = '0' + num / 100;
        buf[len++] = '0' + (num / 10) % 10;
    } else if (num < 10) {

    } else {
        buf[len++] = '0' + num / 10;
    }
    buf[len++] = '0' + num % 10;
    buf[len++] = ' ';
    buf[len++] = '\0';
    /* log_info("%s",buf); */
}

static void pulldown_menu_refresh(void *p)
{
    if (!refresh_timer) {
        return ;
    }
    struct sys_time s_time;
    struct utime time_r;
    struct unumber num;
    u8 percent = get_vbat_percent();//get_vbat_averge_percent();
    u8 weekday = 0;
    u8 pic_index;

    if (ui_get_disp_status_by_id(SIDEBAR_UNDISTURB_LAYOUT)) {
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        return;
    }

    num.type = TYPE_STRING;
    /* percent = rand32() % 100; */
    /* log_info("percent:%d",percent); */
    num2string(percent, percent_str);
    num.num_str = percent_str;
    ui_number_update_by_id(SIDEBAR_NUM_BATTERY_PERCENTAGE, &num);
    pic_index = percent / 20;
    /* log_info("%s %d %d",__FUNCTION__,__LINE__, get_charge_online_flag()); */
    if (get_charge_online_flag()) {
        pic_index += 5;
    }
    if (percent == 100) {
        pic_index--;
    }
    ui_pic_show_image_by_id(SIDEBAR_BATTERY_PIC, pic_index);

    get_sys_time(&s_time);
    time_r.year = s_time.year;
    time_r.month = s_time.month;
    time_r.day = s_time.day;
    /* log_info("year:%d month:%d day:%d",time_r.year,time_r.month,time_r.day); */
    ui_time_update_by_id(SIDEBAR_PULLDOWN_MENU_TIME, &time_r);

#if TCFG_APP_RTC_EN
    extern u8 rtc_calculate_week_val(struct sys_time * data_time);
    weekday = rtc_calculate_week_val(&s_time);
#endif /*TCFG_APP_RTC_EN*/
    ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_WEEK_TEXT, weekday);
}

static int pulldown_menu_time_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct window *window = (struct window *)_ctrl;
    struct ui_time *time = (struct ui_time *)_ctrl;
    struct sys_time s_time;
    struct utime time_r;

    switch (event) {
    case ON_CHANGE_INIT:
        switch (time->text.elm.id) {
        case SIDEBAR_PULLDOWN_MENU_TIME:
            get_sys_time(&s_time);
            time_r.year = s_time.year;
            time_r.month = s_time.month;
            time_r.day = s_time.day;
            log_info("year:%d month:%d day:%d", time_r.year, time_r.month, time_r.day);
            ui_time_update(time, &time_r);
            break;
        }
        if (!refresh_timer) {
            refresh_timer = sys_timer_add(NULL, pulldown_menu_refresh, 10000);
        }
        break;
    case ON_CHANGE_RELEASE:
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        if (refresh_timer) {
            /* log_info("%s %d", __FUNCTION__, __LINE__); */
            sys_timer_del(refresh_timer);
            refresh_timer = 0;
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_PULLDOWN_MENU_TIME)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = pulldown_menu_time_onchange,
};

int battery_pic_init(int percent)
{
    u8 pic_index;
    pic_index = percent / 20;
    if (get_charge_online_flag()) {
        pic_index += 5;
    }
    if (percent == 100) {
        pic_index--;
    }
    ui_pic_show_image_by_id(SIDEBAR_BATTERY_PIC, pic_index);
    return 0;
}

static int pulldown_menu_bat_pic_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    u8 percent = get_vbat_percent();//get_vbat_averge_percent();
    u8 pic_index;

    switch (event) {
    case ON_CHANGE_INIT:
        pic_index = percent / 20;
        if (get_charge_online_flag()) {
            pic_index += 5;
        }
        if (percent == 100) {
            pic_index--;
        }
        ui_pic_set_image_index(pic, pic_index);
        break;
    case ON_CHANGE_RELEASE:
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_BATTERY_PIC)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = pulldown_menu_bat_pic_onchange,
};

int pd_menu_bat_status_handler(const char *type, u32 arg)
{
    log_info("_func_  == %s %d\n", __func__, __LINE__);
    struct unumber num;
    u8 percent = get_vbat_percent();//get_vbat_averge_percent();

    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case POWER_EVENT_POWER_CHANGE:
        case POWER_EVENT_POWER_CHARGE:
            log_info("_func_  == %s %d\n", __func__, __LINE__);
            num.type = TYPE_STRING;
            num2string(percent, percent_str);
            num.num_str = percent_str;
            ui_number_update_by_id(SIDEBAR_NUM_BATTERY_PERCENTAGE, &num);
            battery_pic_init(percent);
            break;
        }
    }

    return 0;
}

static int pulldown_menu_bat_percent_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_number *number = (struct ui_number *)_ctrl;
    struct unumber num;
    u8 percent = get_vbat_percent();//get_vbat_averge_percent();

    switch (event) {
    case ON_CHANGE_INIT:
        switch (number->text.elm.id) {
        case SIDEBAR_NUM_BATTERY_PERCENTAGE:
            num.type = TYPE_STRING;
            num2string(percent, percent_str);
            num.num_str = percent_str;
            ui_number_update(number, &num);
            ui_set_call(battery_pic_init, percent);
            break;
        }
        break;
    case ON_CHANGE_RELEASE:
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_NUM_BATTERY_PERCENTAGE)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = pulldown_menu_bat_percent_onchange,
};

static int pulldown_menu_week_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    struct sys_time s_time;
    struct utime time_r;
    u8 weekday = 0;

    switch (event) {
    case ON_CHANGE_INIT:
        switch (text->elm.id) {
        case SIDEBAR_PULLDOWN_MENU_WEEK_TEXT:
#if TCFG_APP_RTC_EN
            get_sys_time(&s_time);
            extern u8 rtc_calculate_week_val(struct sys_time * data_time);
            weekday = rtc_calculate_week_val(&s_time);
            /* log_info(">>>>>>>weekday:%d", weekday); */
            ui_text_set_index(text, weekday);
            /* get_sys_time_weekday(&s_time); */
#endif
            break;
        }
        break;
    case ON_CHANGE_RELEASE:
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_PULLDOWN_MENU_WEEK_TEXT)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = pulldown_menu_week_onchange,
};

static int pulldown_menu_text_hide_timer = 0;
static void pulldown_menu_text_hide(void *p)
{
    if (pulldown_menu_text_hide_timer) {
        pulldown_menu_text_hide_timer = 0;
    }
    /* y_log_info(">>>>>>>>>>>%s %d", __FUNCTION__, __LINE__); */
    if (ui_get_disp_status_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1) == 1) {
        /* log_info("%s %d %d", __FUNCTION__, __LINE__, ui_get_disp_status_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1)); */
        ui_hide(SIDEBAR_PULLDOWN_MENU_TEXT1);
    }
    if (ui_get_disp_status_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2) == 1) {
        /* log_info("%s %d %d", __FUNCTION__, __LINE__, ui_get_disp_status_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2)); */
        ui_hide(SIDEBAR_PULLDOWN_MENU_TEXT2);
    }
}

int undisturb_pic_init(int arg)
{
    ui_pic_show_image_by_id(SIDEBAR_UNDISTURB_PIC, get_all_day_undisturb_sel());
    return 0;
}

static int undisturb_button_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct window *window = (struct window *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        ui_set_call(undisturb_pic_init, 0);
        break;
    case ON_CHANGE_RELEASE:
        /* log_info("%s %d", __FUNCTION__, __LINE__); */
        break;
    }
    return false;
}

static int undisturb_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        log_info("%s %d", __FUNCTION__, __LINE__);
        if (get_all_day_undisturb_sel()) {
            /* log_info("%s %d", __FUNCTION__, __LINE__); */
            set_all_day_undisturb_sel(0);
            ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1, 3);
            ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2, 5);
            ui_pic_show_image_by_id(SIDEBAR_UNDISTURB_PIC, get_all_day_undisturb_sel());
            if (!pulldown_menu_text_hide_timer) {
                pulldown_menu_text_hide_timer = sys_timeout_add(NULL, pulldown_menu_text_hide, 3000);
            } else {
                sys_timer_re_run(pulldown_menu_text_hide_timer);
            }
        } else {
            /* log_info("%s %d", __FUNCTION__, __LINE__); */
            ui_hide_set(SIDEBAR_PULLDONW_MENU_LAYOUT, HIDE_WITHOUT_REDRAW);
            ui_hide(SIDEBAR_PULLDONW_MENU_LAYOUT);
            ui_show(SIDEBAR_UNDISTURB_LAYOUT);
        }
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(SIDEBAR_UNDISTURB_BUTTON)
.onchange = undisturb_button_onchange,
 .onkey = NULL,
  .ontouch = undisturb_button_ontouch,
};

int undisturb_ensure_release(int arg)
{
    if (get_all_day_undisturb_sel()) {
        ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1, 2);
        ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2, 4);
        ui_pic_show_image_by_id(SIDEBAR_UNDISTURB_PIC, get_all_day_undisturb_sel());
        if (!pulldown_menu_text_hide_timer) {
            pulldown_menu_text_hide_timer = sys_timeout_add(NULL, pulldown_menu_text_hide, 3000);
        } else {
            sys_timer_re_run(pulldown_menu_text_hide_timer);
        }
    } else {
        /* if (pulldown_menu_text_hide_timer) { */
        /*     ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1, 3); */
        /*     ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2, 5); */
        /* } */
    }
    return 0;
}

static int layout_undisturb_ensure_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct layout *layout = (struct layout *)_ctrl; */
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        log_info("%s %d", __FUNCTION__, __LINE__);
        undisturb_ensure_release_flag = 1;
        ui_set_call(undisturb_ensure_release, 0);
        break;
    }
    return 0;
}

REGISTER_UI_EVENT_HANDLER(SIDEBAR_UNDISTURB_LAYOUT)
.onchange = layout_undisturb_ensure_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int undisturb_ensure_button_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct button *button = (struct button *)ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        log_info("%s %d", __FUNCTION__, __LINE__);
        switch (button->elm.id) {
        case SIDEBAR_UNDISTURB_NO_BUTTON:
            set_all_day_undisturb_sel(0);
            ui_hide_set(SIDEBAR_UNDISTURB_LAYOUT, HIDE_WITHOUT_REDRAW);
            ui_hide(SIDEBAR_UNDISTURB_LAYOUT);
            ui_show(SIDEBAR_PULLDONW_MENU_LAYOUT);
            break;
        case SIDEBAR_UNDISTURB_YES_BUTTON:
            set_all_day_undisturb_sel(1);
            ui_hide_set(SIDEBAR_UNDISTURB_LAYOUT, HIDE_WITHOUT_REDRAW);
            ui_hide(SIDEBAR_UNDISTURB_LAYOUT);
            ui_show(SIDEBAR_PULLDONW_MENU_LAYOUT);
            break;
        }
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(SIDEBAR_UNDISTURB_NO_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = undisturb_ensure_button_ontouch,
};

REGISTER_UI_EVENT_HANDLER(SIDEBAR_UNDISTURB_YES_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = undisturb_ensure_button_ontouch,
};

void set_bright_screen_on_off(u8 on_off)
{
    bright_screen_on_off = on_off;
}

int bright_screen_pic_init(int arg)
{
    if (bright_screen_on_off) {
        ui_pic_show_image_by_id(SIDEBAR_BRIGHT_SCREEN_PIC, bright_screen_on_off);
    }
    return 0;
}

static int bright_screen_button_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct window *window = (struct window *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(bright_screen_pic_init, 0);
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return false;
}

static int bright_screen_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        if (bright_screen_on_off) {
            bright_screen_on_off = 0;
            screen_light_alway_switch(0);
            ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1, 1);
            ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2, 5);
            ui_pic_show_image_by_id(SIDEBAR_BRIGHT_SCREEN_PIC, bright_screen_on_off);
            if (!pulldown_menu_text_hide_timer) {
                pulldown_menu_text_hide_timer = sys_timeout_add(NULL, pulldown_menu_text_hide, 3000);
            } else {
                sys_timer_re_run(pulldown_menu_text_hide_timer);
            }
        } else {
            bright_screen_on_off = 1;
            screen_light_alway_switch(1);
            ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT1, 0);
            ui_text_show_index_by_id(SIDEBAR_PULLDOWN_MENU_TEXT2, get_light_time_sel());
            ui_pic_show_image_by_id(SIDEBAR_BRIGHT_SCREEN_PIC, bright_screen_on_off);
            if (!pulldown_menu_text_hide_timer) {
                pulldown_menu_text_hide_timer = sys_timeout_add(NULL, pulldown_menu_text_hide, 3000);
            } else {
                sys_timer_re_run(pulldown_menu_text_hide_timer);
            }
        }
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(SIDEBAR_BRIGHT_SCREEN_BUTTON)
.onchange = bright_screen_button_onchange,
 .onkey = NULL,
  .ontouch = bright_screen_button_ontouch,
};

static int pulldown_menu_button_ontouch(void *ctr, struct element_touch_event *e)
{
    struct button *button = (struct button *)ctr;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        switch (button->elm.id) {
        case SIDEBAR_LOOK_PHONE_BUTTON:
            ui_pic_show_image_by_id(SIDEBAR_LOOK_PHONE_PIC, 1);
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_FINDPHONE);
            break;
        case SIDEBAR_ALARM_BUTTON:
            ui_pic_show_image_by_id(SIDEBAR_ALARM_PIC, 1);
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_ALARM);
            break;
        case SIDEBAR_SETUP_BUTTON:
            ui_pic_show_image_by_id(SIDEBAR_SETUP_PIC, 1);
            extern int setpage_enter_check(int window);
            setpage_enter_check(ID_WINDOW_SET);
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_SET);
            break;
        }
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(SIDEBAR_LOOK_PHONE_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = pulldown_menu_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_ALARM_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = pulldown_menu_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(SIDEBAR_SETUP_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = pulldown_menu_button_ontouch,
};

int pd_menu_bt_status_handler(const char *type, u32 arg)
{
    log_info("_func_  == %s %d\n", __func__, __LINE__);
    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case BT_STATUS_SECOND_CONNECTED:
        case BT_STATUS_FIRST_CONNECTED:
        case BT_STATUS_CONN_A2DP_CH:
            ui_pic_show_image_by_id(SIDEBAR_BLUETOOTH_PIC, 1);
            break;
        case BT_STATUS_FIRST_DISCONNECT:
        case BT_STATUS_SECOND_DISCONNECT:
            log_info("%s %d\n", __func__, __LINE__);
            ui_pic_show_image_by_id(SIDEBAR_BLUETOOTH_PIC, 0);
            break;
        case BT_STATUS_A2DP_MEDIA_START:
            log_info("%s %d\n", __func__, __LINE__);
            break;
        case BT_STATUS_A2DP_MEDIA_STOP:
            log_info("%s %d\n", __func__, __LINE__);
            break;
        }
    }

    return 0;
}

int bluetooth_pic_init(int arg)
{
    if (get_bt_connect_status() !=  BT_STATUS_WAITINT_CONN) {
        ui_pic_show_image_by_id(SIDEBAR_BLUETOOTH_PIC, 1);
    }
    return 0;
}

static int pulldown_menu_bluetooth_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct window *window = (struct window *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(bluetooth_pic_init, 0);
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_BLUETOOTH_PIC)
.onchange  = pulldown_menu_bluetooth_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

