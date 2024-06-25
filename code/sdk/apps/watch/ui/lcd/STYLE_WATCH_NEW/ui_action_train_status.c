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
#include "ui/ui_resource.h"
#include "sport_data/watch_common.h"
#include "sport/sport_api.h"

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_TRAIN

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"




#define STYLE_NAME  JL
#define get_stamina() (90)
#define get_load() (85)

static u16 timerid1 = 0;
static u16 timerid2 = 0;

static void refresh_elm(void *priv)
{
    if (!timerid1) {
        return ;
    }
    timerid1 = 0;
    struct element *elm = priv;
    log_info("%s", __func__);
    ui_update_source_by_elm(elm, 1);
}
static void refresh_text(void *priv)
{
    if (!timerid2) {
        return ;
    }
    timerid2 = 0;
    if (get_stamina() != 0) {
        ui_text_set_hide_by_id(TRAIN1_TEXT, 1);
        ui_text_set_hide_by_id(TRAIN2_TEXT, 1);
    }
    if (get_load() != 0) {
        ui_text_set_hide_by_id(TRAIN4_TEXT, 1);
    }
    struct watch_execise __execise_hd;
    struct motion_info __execise_info;
    watch_execise_handle_get(&__execise_hd);
    __execise_hd.execise_info_get(&__execise_info);
    if (__execise_info.recoverytime_hour * 60 + __execise_info.recoverytime_min == 0) {
        ui_text_show_index_by_id(TRAIN3_TEXT, 0);
    } else {
        ui_text_show_index_by_id(TRAIN3_TEXT, 1);
    }
    struct watch_blood_oxygen __bloodoxygen_hd;
    watch_blood_oxygen_handle_get(&__bloodoxygen_hd);
    if (__bloodoxygen_hd.blood_oxygen_get(LAST_DATA, NULL) != 0) {
        ui_text_set_hide_by_id(TRAIN5_TEXT, 1);
    }
}

static int LAYOUT_TRAIN_STATUS_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    log_info("%s", __func__);
    struct element *elm = (struct element *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_FIRST_SHOW:
        if (!timerid1) {
            timerid1 = sys_timeout_add(elm, refresh_elm, 10);
        }
        if (!timerid2) {
            timerid2 = sys_timeout_add(NULL, refresh_text, 10);
        }
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        if (timerid1) {
            sys_timeout_del(timerid1);
            timerid1 = 0;
        }
        if (timerid2) {
            sys_timeout_del(timerid2);
            timerid2 = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(LAYOUT_TRAIN_STATUS)
.onchange = LAYOUT_TRAIN_STATUS_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};



#endif
#endif
