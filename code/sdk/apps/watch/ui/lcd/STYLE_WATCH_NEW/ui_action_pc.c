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
#if TCFG_UI_ENABLE_PC

#define STYLE_NAME  JL

int pc_no_dev_callback(int arg)
{
    ui_show(PC_NO_DEV_LAYOUT);
    return true;
}

static int pc_layout_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_text *text = (struct ui_text *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
#if TCFG_APP_PC_EN
        if (app_get_curr_task() != APP_PC_TASK) {
            if (pc_app_check() == false) {
                ui_set_call(pc_no_dev_callback, 0);
            } else {
                app_task_switch_to(APP_PC_TASK);
            }
        }
#endif
        break;
    case ON_CHANGE_SHOW_PROBE:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
        break;
    case ON_CHANGE_FIRST_SHOW:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
        break;
    case ON_CHANGE_RELEASE:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(PC_LAYOUT)
.onchange = pc_layout_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int pc_no_dev_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        log_info("%s %d", __FUNCTION__, __LINE__);
        break;
    case ON_CHANGE_RELEASE:
        log_info("%s %d", __FUNCTION__, __LINE__);
        break;
    default:
        return FALSE;
    }
    return FALSE;
}

REGISTER_UI_EVENT_HANDLER(PC_NO_DEV_LAYOUT)
.onchange = pc_no_dev_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int pc_err_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, e->event);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        log_info("%s %d", __FUNCTION__, __LINE__);
        ui_send_event(KEY_CHANGE_PAGE, 1);
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(PC_OK_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = pc_err_ontouch,
};

#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

