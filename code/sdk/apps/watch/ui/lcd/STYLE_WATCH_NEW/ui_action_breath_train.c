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
#if TCFG_UI_ENABLE_BREATH_TRAIN

#define STYLE_NAME  JL

#define BREATH_TRAIN_PAGE       PAGE_57
#define BREATH_TIME_PAGE        PAGE_58
#define BREATH_RHYTHM_PAGE      PAGE_59
#define BREATH_RESULT_PAGE      PAGE_60
#define BREATH_ANIMATION_PAGE   PAGE_62

struct BREATH_ANIM {
    int breath_anim_timer;
    s32 breath_anim_total_time;
    u8 dir;
    s8 pic_index;
    u16 stop_cnt;
};

static u8 breath_time = 0;
static u8 last_breath_time = 0;
static u8 breath_rhythm = 0;
static u8 last_breath_rhythm = 0;
static struct BREATH_ANIM breath_anim;



static int breath_train_start_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(BREATH_ANIMATION_PAGE);
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(BREATH_TRAIN_START_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = breath_train_start_ontouch,
};
static int breath_train_time_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(BREATH_TIME_PAGE);
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(BREATH_TRAIN_TIME_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = breath_train_time_ontouch,
};
static int breath_train_rhythm_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(BREATH_RHYTHM_PAGE);
        break;
    }
    return false;

}

REGISTER_UI_EVENT_HANDLER(BREATH_TRAIN_RHYTHM_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = breath_train_rhythm_ontouch,
};

static int list_text_set_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:

        switch (text->elm.id) {
        case TEXT_BREATH_TIME:
        case TEXT_BREATH_TIME_RESLUT:
            log_info("%s %d:%d", __FUNCTION__, __LINE__, breath_time);
            ui_text_set_index(text, breath_time);
            break;
        case TEXT_BREATH_RHYTHM:
            log_info("%s %d:%d", __FUNCTION__, __LINE__, breath_rhythm);
            ui_text_set_index(text, breath_rhythm);
            break;
        case ON_CHANGE_HIGHLIGHT:
            break;
        default:
            break;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(TEXT_BREATH_TIME)
.onchange = list_text_set_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(TEXT_BREATH_RHYTHM)
.onchange = list_text_set_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(TEXT_BREATH_TIME_RESLUT)
.onchange = list_text_set_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int list_breath_time_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        ui_grid_set_hi_index(grid, breath_time);
        ui_grid_set_pix_scroll(grid, true);
        break;
    }
    return 0;
}

static int list_breath_time_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        sel_item = ui_grid_cur_item(grid);
        /* printf("sel_item %d, %d\n", sel_item, grid->hi_index); */
        if (sel_item >= 0) {
            breath_time = sel_item;
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_U_MOVE:
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(VLIST_BREATH_TIME)//呼吸时长-垂直列表
.onchange = list_breath_time_onchange,
 .onkey = NULL,
  .ontouch = list_breath_time_ontouch,
};


static int text_breath_time_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_text *text = (struct ui_text *)ctr;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        last_breath_time = breath_time;
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(BREATH_TRAIN_PAGE);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    case ELM_EVENT_TOUCH_U_MOVE:
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(TEXT_BREATH_TIME_SURE)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = text_breath_time_ontouch,
};

static int layout_breath_time_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct layout *layout = (struct layout *)_ctrl; */
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        breath_time = last_breath_time;
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(BREATH_TIME_PAGE)
.onchange = layout_breath_time_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int list_breath_rhythm_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        ui_grid_set_hi_index(grid, breath_rhythm);
        ui_grid_set_pix_scroll(grid, true);
        break;
    }
    return 0;
}

static int list_breath_rhythm_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        sel_item = ui_grid_cur_item(grid);
        /* printf("sel_item %d, %d\n", sel_item, grid->hi_index); */
        if (sel_item >= 0) {
            breath_rhythm = sel_item;
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_U_MOVE:
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(VLIST_BREATH_RHYTHM)//呼吸节奏-垂直列表
.onchange = list_breath_rhythm_onchange,
 .onkey = NULL,
  .ontouch = list_breath_rhythm_ontouch,
};


static int text_breath_rhythm_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_text *text = (struct ui_text *)ctr;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        last_breath_rhythm = breath_rhythm;
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(BREATH_TRAIN_PAGE);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    case ELM_EVENT_TOUCH_U_MOVE:
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(TEXT_BREATH_RHYTHM_SURE)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = text_breath_rhythm_ontouch,
};

static int layout_breath_rhythm_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct layout *layout = (struct layout *)_ctrl; */
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        breath_rhythm = last_breath_rhythm;
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(BREATH_RHYTHM_PAGE)
.onchange = layout_breath_rhythm_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int list_breath_result_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        ui_grid_set_pix_scroll(grid, true);
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(VLIST_BREATH_RESULT)//呼吸结果-垂直列表
.onchange = list_breath_result_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int breath_train_heart_rate_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_number *number = (struct ui_number *)_ctrl;
    struct unumber num;

    switch (event) {
    case ON_CHANGE_INIT:
        switch (number->text.elm.id) {
        case BREATH_TRAIN_BEFORE:
        case BREATH_TRAIN_AFTER:
            num.type = TYPE_NUM;
            num.numbs = 1;
            num.number[0] = rand32() % 100;
            log_info("number[0]:%d", num.number[0]);
            ui_number_update(number, &num);
            break;
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(BREATH_TRAIN_BEFORE)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = breath_train_heart_rate_onchange,
};

REGISTER_UI_EVENT_HANDLER(BREATH_TRAIN_AFTER)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = breath_train_heart_rate_onchange,
};

static void breath_anim_refresh(void *p)
{
    if (breath_anim.breath_anim_timer) {
        breath_anim.breath_anim_timer = 0;
    }

    if (breath_anim.breath_anim_total_time <= 0) {
        breath_anim.dir = 0;
        breath_anim.pic_index = 0;
        breath_anim.stop_cnt = 0;
        breath_anim.breath_anim_total_time = (breath_time + 1) * 60 * 1000;//单位：ms
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(BREATH_RESULT_PAGE);
        return;
    }

    breath_anim.breath_anim_total_time -= 128;
    if (breath_anim.stop_cnt) {
        breath_anim.stop_cnt--;
        breath_anim.breath_anim_timer = sys_timeout_add(NULL, breath_anim_refresh, 128);
        return;
    }

    ui_pic_show_image_by_id(BREATH_ANIMATION_PIC, breath_anim.pic_index);
    if (breath_anim.dir) {
        ui_text_show_index_by_id(BREATH_ANIMATION_TEXT, breath_anim.dir);
        breath_anim.pic_index--;
        if (breath_anim.pic_index == -1) {
            breath_anim.dir = 0;
            breath_anim.stop_cnt = 8;
            breath_anim.pic_index++;
        }
    } else {
        ui_text_show_index_by_id(BREATH_ANIMATION_TEXT, breath_anim.dir);
        breath_anim.pic_index++;
        if (breath_anim.pic_index == 31) {
            breath_anim.dir = 1;
            breath_anim.stop_cnt = 8;
            breath_anim.pic_index--;
        }
    }
    breath_anim.breath_anim_timer = sys_timeout_add(NULL, breath_anim_refresh, 128);
}

static int breath_anim_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        ui_auto_shut_down_disable();
        breath_anim.dir = 0;
        breath_anim.pic_index = 0;
        breath_anim.stop_cnt = 0;
        breath_anim.breath_anim_total_time = (breath_time + 1) * 60 * 1000;//单位：ms
        if (!breath_anim.breath_anim_timer) {
            breath_anim.breath_anim_timer = sys_timeout_add(NULL, breath_anim_refresh, 128);
        }
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        if (breath_anim.breath_anim_timer) {
            sys_timeout_del(breath_anim.breath_anim_timer);
            breath_anim.breath_anim_timer = 0;
        }
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(BREATH_ANIMATION_PAGE)
.onchange = breath_anim_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

