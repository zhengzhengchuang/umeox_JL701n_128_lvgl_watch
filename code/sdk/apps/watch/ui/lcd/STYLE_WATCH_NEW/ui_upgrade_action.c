#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
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

#define STYLE_NAME  JL

/* #define STYLE_NAME  UPGRADE */
/*  */
/* REGISTER_UI_STYLE(STYLE_NAME) */
/*  */
extern void ui_send_event(u16 event, u32 val);



static int upgrade_handler(const char *type, u32 arg)
{
    static u32 process_record = -1;
    log_info("msg test %s %s %d \n", __FUNCTION__, type, arg);
    if (type && !strcmp(type, "start")) {
        ui_text_show_index_by_id(STYLE_UPGRADE_ID(UPGRADE_TEXT), 1);
    }

    if (type && !strcmp(type, "process")) {
        if (process_record != arg) {
            process_record = arg;
            ui_progress_set_persent_by_id(STYLE_UPGRADE_ID(PROGRESS), arg);
        }
    }

    if (type && !strcmp(type, "result")) {
        if (arg) { //升级成功
            ui_text_show_index_by_id(STYLE_UPGRADE_ID(UPGRADE_TEXT), 2);
        } else {
            ui_text_show_index_by_id(STYLE_UPGRADE_ID(UPGRADE_TEXT), 3);
        }
    }

    if (type && !strcmp(type, "wait")) {
        /* ui_text_show_index_by_id(STYLE_UPGRADE_ID(UPGRADE_TEXT), 0); */
        ui_text_show_index_by_id(STYLE_UPGRADE_ID(UPGRADE_TEXT), arg);
    }

    return 0;
}


static const struct uimsg_handl ui_msg_handler[] = {
    { "upgrade",        upgrade_handler     }, //
    { NULL, NULL},      /* 必须以此结尾！ */
};



extern void set_lcd_keep_open_flag(u8 flag);


static int PAGE_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    log_info("window call = %s %d id =%x \n", __FUNCTION__, __LINE__, window->elm.id);
    switch (e) {
    case ON_CHANGE_INIT:
        if (!ui_auto_shut_down_disable()) {
            set_lcd_keep_open_flag(1);
        }

        ui_register_msg_handler(window->elm.id, ui_msg_handler);//注册消息交互的回调
        break;
    case ON_CHANGE_RELEASE:
        set_lcd_keep_open_flag(0);
        ui_auto_shut_down_enable();
        break;
    default:
        return false;
    }
    return false;
}
static int upgrade_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s \n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
        return true; // 升级界面不响应左右滑动
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ID_WINDOW_UPGRADE)
.onchange = PAGE_mode_onchange,
 .onkey = NULL,
  .ontouch = upgrade_ontouch,
};

#endif /* #if (!TCFG_LUA_ENABLE) */


