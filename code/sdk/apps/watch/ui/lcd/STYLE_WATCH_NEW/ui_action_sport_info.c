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
#include "system/includes.h"
#include "audio_config.h"
#include "asm/mcpwm.h"
#include "sport/sport_api.h"
#include "sport_data/watch_common.h"
#include "sport_info_sync.h"

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
#if TCFG_UI_ENABLE_SPORT_INFO

#define STYLE_NAME  JL

static struct watch_execise *__execise_hd = NULL;

static u8 sport_status = 0;//0未运动，1运动，2暂停，3继续，4停止
static u16 timer = 0;
static u8 icon_timer = 0;
static u8 lock_status = 1;

extern void heart_null_show(u32 pic_id, u32 num_id, u8 type);

static void layout_refresh(void *p)
{
    if (!timer) {
        return ;
    }
    if (p) {
        struct element *elm = (struct element *)p;
        ui_update_source_by_elm(elm, 1);
        heart_null_show(HRNULL_1, S_HEART, LAST_DATA);
        heart_null_show(HRNULL_2, S_HEART2, LAST_DATA);

    }

}

static u32 item_memory = 0;
static void sport_info_item_get(struct ui_grid *grid)
{
    item_memory = ui_grid_get_hindex(grid);
    if (item_memory >= grid->avail_item_num) {
        item_memory = 0;
    }
}
static void sport_info_item_set(struct ui_grid *grid, int item)
{
    ui_grid_set_hi_index(grid, item);
}

static int TRAIN_VERTLIST_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    struct element *k;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);

        break;
    case ON_CHANGE_INIT:
        sport_info_item_set(grid, item_memory);
        break;
    case ON_CHANGE_FIRST_SHOW:

        if (__execise_hd == NULL) {
            __execise_hd = zalloc(sizeof(struct watch_execise));
            watch_execise_handle_get(__execise_hd);
        }
        ui_update_source_by_elm(&grid->elm, 0);
        if (!timer) {
            timer =  sys_timer_add(&grid->elm, layout_refresh, 1000);
        }
        sport_status = __execise_hd->execise_ctrl_status_get();
        if (sport_status == 0) {
            __execise_hd->execise_ctrl_status_set(__execise_hd->execise_mode_get(), SPORT_STATUS_START);
            sport_info_sync_start_exercise_resp(NULL, NULL);
            watch_sport_start(__execise_hd->execise_mode_get());//记录运动数据
            ui_auto_goto_dial_disable();//关闭自动回到表盘功能
        }
        if (sport_status == 2) {
            __execise_hd->execise_ctrl_status_set(-1, SPORT_STATUS_CONTINNUE);
            sport_info_sync_keep_exercise_resp();
            watch_sport_restart();
        }
        if (sport_status == 3) {
        }
        break;
    case ON_CHANGE_RELEASE:
        sport_info_item_get(grid);
        if (timer) {
            sys_timer_del(timer);
            timer = 0;
        }

        if (__execise_hd == NULL) {
            free(__execise_hd);
            __execise_hd = NULL;
        }
        ui_set_default_handler(grid, NULL, NULL, NULL);
        break;

    default:
        break;
    }
    return false;
}

static int TARIN_VERTLIST_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    sport_status = __execise_hd->execise_ctrl_status_get();
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            break;
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        return lock_status;
    /* break; */
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        return lock_status;
    case ELM_EVENT_TOUCH_R_MOVE:
        if (!lock_status) {
            if ((sport_status == 1) | (sport_status == 3)) {
                __execise_hd->execise_ctrl_status_set(-1, SPORT_STATUS_PAUSE);
                sport_info_sync_pause_exercise_resp();
                watch_sport_pause();
            }
        }
        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        if (!lock_status) {
            if ((sport_status == 1) | (sport_status == 3)) {
                __execise_hd->execise_ctrl_status_set(-1, SPORT_STATUS_PAUSE);
                sport_info_sync_pause_exercise_resp();
                watch_sport_pause();
            }
        }
        break;
    }
    return true;
}
REGISTER_UI_EVENT_HANDLER(BASEFORM_886)
.ontouch = TARIN_VERTLIST_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};
REGISTER_UI_EVENT_HANDLER(SPORT_INFO_GRID)
.ontouch = TARIN_VERTLIST_ontouch,
 .onkey = NULL,
  .onchange  = TRAIN_VERTLIST_onchange,
};
static int LOCK_ICON_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_pic *pic = (struct ui_pic *) ctrl;
    struct element *k;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        break;
    case ON_CHANGE_INIT:
        printf("%s lock_status=%d invis %d", __func__, lock_status, pic->elm.css.invisible);
        pic->elm.css.invisible = !lock_status;
        if (!lock_status) {
            ui_core_hide(pic);
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(LOCK_ICON)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = LOCK_ICON_onchange,
};

static int BUTTON_CSPORT_START_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            break;
        }
        UI_HIDE_CURR_WINDOW();
        UI_WINDOW_BACK_SPEC_SHOW(ID_WINDOW_SPORT_INFO);
        UI_WINDOW_BACK_DEL(ID_WINDOW_SPORT_CTRL);
        UI_WINDOW_PREEMPTION_DEL(ID_WINDOW_SPORT_CTRL);
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    }
    return false;
}

static int BUTTON_CSPORT_STOP_ontouch(void *_ctrl, struct element_touch_event *e)
{
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    sport_status = __execise_hd->execise_ctrl_status_get();
    struct button *elm = (struct button *)_ctrl;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:

        if (move_flag) {
            move_flag = 0;
            break;
        }
        if (sport_status != 0) {
            __execise_hd->execise_ctrl_status_set(-1, SPORT_STATUS_STOP);
            watch_sport_stop();
            sport_info_sync_end_exercise_by_fw();
            __execise_hd->execise_ctrl_status_clr();
        }
        ui_auto_goto_dial_enable();
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_SPORT_RECORD);
        UI_WINDOW_BACK_DEL(ID_WINDOW_SPORT_INFO);
        UI_WINDOW_BACK_DEL(ID_WINDOW_SPORT_CTRL);
        UI_WINDOW_PREEMPTION_DEL(ID_WINDOW_SPORT_INFO);
        UI_WINDOW_PREEMPTION_DEL(ID_WINDOW_SPORT_CTRL);
        break;
    case ELM_EVENT_TOUCH_DOWN:
        if (move_flag) {
            move_flag = 0;
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    }
    return false;
}

static int BUTTON_CSPORT_LOCK_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct button *button = (struct button *)_ctrl;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            break;
        }
        if (lock_status == 0) {
            lock_status = 1;
        } else {
            lock_status = 0;
        }
        if (!ui_core_highlight_element(&button->elm, lock_status)) {//切换图标显示
            ui_core_redraw(&button->elm);
        }
        return true;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    }
    return false;

}
void lock_status_show(void *p)
{
    if (!ui_core_highlight_element(p, lock_status)) {//切换图标显示
        ui_core_redraw(p);
    }
}
static button_timer = 0;
static int BUTTON_CSPORT_LOCK_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct button *button = (struct button *)ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        break;
    case ON_CHANGE_HIGHLIGHT:
        break;
    case ON_CHANGE_FIRST_SHOW:
        if (!button_timer) {
            button_timer = sys_timeout_add(&button->elm, lock_status_show, 50);
        }
        break;
    case ON_CHANGE_RELEASE:
        if (button_timer) {
            sys_timeout_del(button_timer);
            button_timer = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(BUTTON_CSPORT_START)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = BUTTON_CSPORT_START_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BUTTON_CSPORT_STOP)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = BUTTON_CSPORT_STOP_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BUTTON_CSPORT_LOCK)
.onchange = BUTTON_CSPORT_LOCK_onchange,
 .onkey = NULL,
  .ontouch = BUTTON_CSPORT_LOCK_ontouch,
};

#endif /* #if TCFG_UI_ENABLE_SPORT_INFO */
#endif
#endif
