#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "music/music.h"
#include "ui/ui_wave.h"
#include "ui/ui_bar_chart.h"
#include "sport/sport_api.h"
#include "sport_data/watch_common.h"

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
#if TCFG_UI_ENABLE_OXYGEN

#define STYLE_NAME  JL


static u16 spo2timer = 0;


static int watch_child_cb(void *_ctrl, int id, int type, u8 oxygen, u8 valid)
{
    switch (type) {
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->source, "Soxygen")) {
            if (valid && oxygen) {
                ui_text_set_index(text, 1);
                /* if (spo2timer) { */
                /* sys_timer_del(spo2timer); */
                /* spo2timer = 0; */
                /* ui_auto_shut_down_enable(); */

                /* } */
            } else if (valid) {
                ui_text_set_index(text, 0);
            } else {
                ui_text_set_index(text, 2);
            }
            ui_core_redraw(text);
        }

        break;
    case CTRL_TYPE_NUMBER:
        struct ui_number *number = (struct ui_number *)_ctrl;
        struct unumber num;
        if (!strcmp(number->source, "Roxygen")) {
            if (valid && oxygen) {
                num.type = TYPE_NUM;
                num.numbs =  1;
                num.number[0] = oxygen;
            } else {
                num.type = TYPE_NUM;
                num.numbs =  1;
                num.number[0] = 0;//
                /* num.type = TYPE_STRING; */
                /* num.num_str = (u8*)"---"; */
            }
            ui_number_update(number, &num);
            ui_core_redraw(number);
        }
        if (!strcmp(number->source, "Rheart")) {
            struct watch_heart_rate __heartrate_hd;//心率为0显示--
            watch_heart_rate_handle_get(&__heartrate_hd);
            int heart = __heartrate_hd.heart_rate_data_get(LAST_DATA, NULL);
            num.type = TYPE_NUM;
            num.numbs =  1;
            num.number[0] = heart;
            ui_number_update(number, &num);
            ui_core_redraw(number);
        }

        break;
    }
    return 0;
}

#define ui_number_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? (struct ui_number *)elm: NULL; \
     })
#define ui_pic_for_id(id) \
		(struct ui_pic *)ui_core_get_element_by_id(id)
void blood_null_show(u32 pic_id, u32 num_id, u8 type)
{
    struct watch_blood_oxygen __bloodoxygen_hd;//血氧为0显示--
    watch_blood_oxygen_handle_get(&__bloodoxygen_hd);
    int spo2 = __bloodoxygen_hd.blood_oxygen_get(type, NULL);
    printf("%s %d", __func__, spo2);
    if (spo2) {
        ui_core_hide(ui_pic_for_id(pic_id));
        ui_core_redraw(ui_pic_for_id(pic_id));
        ui_core_show(ui_number_for_id(num_id), false);
        ui_core_redraw(ui_number_for_id(num_id));
    } else {
        ui_core_hide(ui_number_for_id(num_id));
        ui_core_redraw(ui_number_for_id(num_id));
        ui_core_show(ui_pic_for_id(pic_id), false);
        ui_core_redraw(ui_pic_for_id(pic_id));
    }
}
static void layout_refresh(void *ctrl)
{
    if (!spo2timer) {
        return ;
    }
    if (ctrl) {
        struct element *elm = (struct element *)ctrl;
        u8 valid = 0;

#if TCFG_SPO2_SENSOR_ENABLE
        struct watch_blood_oxygen __bloodoxygen_hd;
        watch_blood_oxygen_handle_get(&__bloodoxygen_hd);
        u8 blood_oxygen = __bloodoxygen_hd.blood_oxygen_get(LAST_DATA, &valid);

#else
        valid = 1;
        u8 blood_oxygen =  99;
#endif
        struct element *p;
        list_for_each_child_element(p, elm) {
            if (watch_child_cb(p, p->id, ui_id2type(p->id), blood_oxygen, valid)) {
                return;
            }
        }
        extern void heart_null_show(u32 pic_id, u32 num_id, u8 type);
        heart_null_show(HRNULL_3, OXYGEN_HR, LAST_DATA);
        blood_null_show(SPO2NULL_1, REAL_OXYGEN, LAST_DATA);
    }
}





static int LAYOUT_BLOOD_OXYGEN_MAIN_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* log_info("%s %d", __func__,event); */
    struct element *elm = (struct element *)_ctrl;
    struct watch_algo __watch_algo;
    watch_algo_handle_get(&__watch_algo);
    switch (event) {
    case ON_CHANGE_INIT:
        heart_null_show(HRNULL_3, OXYGEN_HR, LAST_DATA);
        blood_null_show(SPO2NULL_1, REAL_OXYGEN, LAST_DATA);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_FIRST_SHOW:
        if (!spo2timer) {
#if TCFG_SPO2_SENSOR_ENABLE
            __watch_algo.algo_blood_oxygen_task();
#endif
            ui_auto_shut_down_disable();
            spo2timer =  sys_timer_add(elm, layout_refresh, 1000);
        }
        /* layout_refresh(elm); */
        log_info("%s,spo2timer=%d", __func__, spo2timer);
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        log_info("%s,timer=%d", __func__, spo2timer);
        if (spo2timer) {
            sys_timer_del(spo2timer);
            spo2timer = 0;
            ui_auto_shut_down_enable();
#if TCFG_SPO2_SENSOR_ENABLE
            __watch_algo.algo_blood_oxygen_task_exit();
#endif
        }

        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(BLOOD_OXYGEN_MAIN)
.onchange = LAYOUT_BLOOD_OXYGEN_MAIN_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int start_timer = 0;
static int start_sec = 3;
static void hide_cur_layout(void *priv)
{
    log_info("%s", __func__);
    struct unumber n;
    n.type = TYPE_NUM;
    n.numbs =  1;
    n.number[0] = start_sec;
    ui_number_update_by_id(BASEFORM_1023, &n);
    if (!start_sec) {
        layout_hide(BLOOD_OXYGEN_START);
        layout_show(BLOOD_OXYGEN_MAIN);
        if (start_timer) {
            sys_timer_del(start_timer);
            start_timer = 0;
            start_sec = 3;
            return;
        }
    }
    start_sec--;
}

static int LAYOUT_BLOOD_OXYGEN_START_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* log_info("%s", __func__); */
    struct element *elm = (struct element *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:

        struct unumber n;
        n.type = TYPE_NUM;
        n.numbs =  1;
        n.number[0] = start_sec ;
        ui_number_update(ui_number_for_id(BASEFORM_1023), &n);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_FIRST_SHOW:
        if (!start_timer) {
            start_timer = sys_timer_add(NULL, hide_cur_layout, 1000);
            start_sec--;
        }
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:

        if (start_timer) {
            sys_timer_del(start_timer);
            start_timer = 0;
            start_sec = 4;
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(BLOOD_OXYGEN_START)
.onchange = LAYOUT_BLOOD_OXYGEN_START_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};




#endif /* #if TCFG_UI_ENABLE_OXYGEN */
#endif
#endif

