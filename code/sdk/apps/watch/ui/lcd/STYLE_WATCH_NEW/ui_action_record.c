#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "app_main.h"
#include "init.h"
#include "key_event_deal.h"

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
#if TCFG_UI_ENABLE_RECORD

#define STYLE_NAME  JL

void record_no_dev_callback(int arg)
{
    ui_hide(RECORD_LAYOUT);
    ui_show(RECORD_NO_DEV_LAYOUT);
}

static int record_status(const char *type, u32 arg)
{
    int  status = arg;
#if TCFG_APP_RECORD_EN
    if (app_get_curr_task() == APP_RECORD_TASK) {
        if (status) {
            ui_pic_show_image_by_id(RECORD_PP_PIC, 1);
            ui_auto_shut_down_disable();
        } else {
            ui_pic_show_image_by_id(RECORD_PP_PIC, 0);
            ui_auto_shut_down_enable();
        }
    }
#endif /* #if TCFG_APP_RECORD_EN */
    return 0;
}
static const struct uimsg_handl ui_msg_handler[] = {
    { "record_status",        record_status     }, /* 录音状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};

static int record_win_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        printf("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
#if TCFG_APP_RECORD_EN
        /* ui_auto_shut_down_disable(); */
        if (app_get_curr_task() != APP_RECORD_TASK) {
            if (record_app_check() == false) {
                ui_set_call(record_no_dev_callback, 0);
            } else {
                app_task_switch_to(APP_RECORD_TASK);
            }
        }
        ui_register_msg_handler(window->elm.id, ui_msg_handler);//注册消息交互的回调
#endif
        break;
    case ON_CHANGE_RELEASE:
        printf("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
#if TCFG_APP_RECORD_EN
        if (app_get_curr_task() == APP_RECORD_TASK) {
            app_task_switch_back();
        }
        ui_auto_shut_down_enable();
#endif
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(ID_WINDOW_RECORD)
.onchange = record_win_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int record_pp_pic(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s %d\n", __FUNCTION__, e->event);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    case ELM_EVENT_TOUCH_UP:
#if TCFG_APP_RECORD_EN
        if (app_get_curr_task() == APP_RECORD_TASK) {
            printf("put KEY_ENC_START \n");
            app_task_put_key_msg(KEY_ENC_START, 0);
        }
#endif
        return true;
        break;
    default:
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(RECORD_PP_PIC)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = record_pp_pic,
};

#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

