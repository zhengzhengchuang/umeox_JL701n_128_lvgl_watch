#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "ui/res_config.h"
#include "ui/ui_sys_param.h"
#include "ui_action_alarm.h"
#include "rtc/alarm.h"
#include "bt/bt.h"
#include "sport_data/watch_common.h"
#include "ui/watch_syscfg_manage.h"

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_ALARM

#define STYLE_NAME  JL

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"




static u8 hour = 0, min = 0;
static u8 week;
static u8 sel_index = 0;
static int back_id = 0;
static u16 alarm_set_time[5] = {0};

static int __alarm_onff_touch(void *_ctrl, struct element_touch_event *e)
{
    u8 index = 0;
    struct ui_pic *_pic = (struct ui_pic *) _ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        switch (_pic->elm.id) {
        case ALARM_PIC_OFF_ON_ITEM1:
            index = 0;
            break;

        case ALARM_PIC_OFF_ON_ITEM2:
            index = 1;
            break;

        case ALARM_PIC_OFF_ON_ITEM3:
            index = 2;
            break;

        case ALARM_PIC_OFF_ON_ITEM4:
            index = 3;
            break;

        case ALARM_PIC_OFF_ON_ITEM5:
            index = 4;
            break;
        default:
            return false;
            break;

        }

        u8 h_index;
        get_alarm_number2table(index, &h_index);
        if (index > MAX_ALARM_ITEM) {
            break;
        }
        T_ALARM alarm;
        alarm_get_info(&alarm, h_index);
        alarm.sw = !alarm.sw;
        ui_pic_show_image_by_id(_pic->elm.id, alarm.sw);
        alarm_add(&alarm, h_index);
        return true;
        break;
    }
    return false;
}

static int alarm_vertlist_pic_ontouch(void *_ctrl, struct element_touch_event *e, void *arg)
{
    struct ui_pic *_pic = (struct ui_pic *) _ctrl;
    log_info("_pic->elm.id is 0x%x", _pic->elm.id);
    log_info("LAQ text e->event is %d", e->event);

    if (__alarm_onff_touch(_ctrl, e)) {
        return true;
    }

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        switch (_pic->elm.id) {
        case ALARM_ADD:
            ui_hide(ALARM_ADD_LAYOUT_1);
            if (alarm_get_total() >= MAX_ALARM_ITEM) {
                ui_show(ALARM_ADD_LAYOUT_4);
            } else {
                sel_index = alarm_get_total();
                hour = 0;
                min = 0;
                ui_show(ALARM_ADD_LAYOUT_2);
            }
            return true;
            break;
        case ALARM_BTN_ITEM1:
            sel_index = 0;
            ui_hide(ALARM_ADD_LAYOUT_1);
            ui_show(ALARM_ADD_LAYOUT_5);
            return true;
            break;
        case ALARM_BTN_ITEM2:
            sel_index = 1;
            ui_hide(ALARM_ADD_LAYOUT_1);
            ui_show(ALARM_ADD_LAYOUT_5);
            return true;
            break;
        case ALARM_BTN_ITEM3:
            sel_index = 2;
            ui_hide(ALARM_ADD_LAYOUT_1);
            ui_show(ALARM_ADD_LAYOUT_5);
            return true;
            break;
        case ALARM_BTN_ITEM4:
            sel_index = 3;
            ui_hide(ALARM_ADD_LAYOUT_1);
            ui_show(ALARM_ADD_LAYOUT_5);
            return true;
            break;
        case ALARM_BTN_ITEM5:
            sel_index = 4;
            ui_hide(ALARM_ADD_LAYOUT_1);
            ui_show(ALARM_ADD_LAYOUT_5);
            return true;
            break;
        }
        break;
    case ELM_EVENT_TOUCH_U_MOVE:
    case ELM_EVENT_TOUCH_D_MOVE:
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        break;
    default:
        break;
    }
    return false;
}

static int  alarm_vertlist_child_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm  = (struct element *) _ctrl;
    int type = ui_id2type(elm->id);
    log_info("LAQ text alarm type = %d", type);
    switch (type) {
    case CTRL_TYPE_PIC:
        alarm_vertlist_pic_ontouch(_ctrl, e, NULL);
    case CTRL_TYPE_NUMBER:
        break;
    }
    return false;
}

static int __alarm_onff_onchange(void *ctrl, enum element_change_event e)
{
    struct ui_pic *_pic = (struct ui_pic *)ctrl;
    u8 index = 0;
    switch (e) {
    case ON_CHANGE_INIT:
        switch (_pic->elm.id) {
        case ALARM_PIC_OFF_ON_ITEM1:
            index = 0;
            break;

        case ALARM_PIC_OFF_ON_ITEM2:
            index = 1;
            break;

        case ALARM_PIC_OFF_ON_ITEM3:
            index = 2;
            break;

        case ALARM_PIC_OFF_ON_ITEM4:
            index = 3;
            break;

        case ALARM_PIC_OFF_ON_ITEM5:
            index = 4;

            break;
        default:
            return false;
            break;

        }
        u8 h_index;
        get_alarm_number2table(index, &h_index);
        if (index > MAX_ALARM_ITEM) {
            break;
        }
        T_ALARM alarm;
        alarm_get_info(&alarm, h_index);
        ui_pic_set_image_index(_pic, !!alarm.sw);

        return true;
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }

    return false;
}

static int  alarm_vertlist_pic_onchange(void *ctrl, enum element_change_event e)
{
    if (__alarm_onff_onchange(ctrl, e)) {
        return true;
    }
    return false;
}

/***************************************************************************************
  闹钟信息显示功能函数
 ****************************************************************************************/

/* #define ui_text_for_id(id) (struct ui_text *)ui_core_get_element_by_id(id) */
#define ui_text_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_text, elm): NULL; \
	 })
static int ALARM_TEXT_AM_PM_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_text *text = (struct ui_text *) ctrl;
    static u16 week_text_buf[MAX_ALARM_ITEM][7];
    switch (e) {
    case ON_CHANGE_INIT:
        switch (text->elm.id) {
        case ALARM_TEXT_AM_PM_INTM1:
            log_info("sub_min : %d\n", alarm_set_time[0]);
            if (alarm_set_time[0] < 720) {
                ui_text_set_index(text, 0);
            } else {
                ui_text_set_index(text, 1);
            }
            break;
        case ALARM_TEXT_AM_PM_INTM2:
            log_info("sub_min : %d\n", alarm_set_time[1]);
            if (alarm_set_time[1] < 720) {
                ui_text_set_index(text, 0);
            } else {
                ui_text_set_index(text, 1);
            }
            break;
        case ALARM_TEXT_AM_PM_INTM3:
            log_info("sub_min : %d\n", alarm_set_time[2]);
            if (alarm_set_time[2] < 720) {
                ui_text_set_index(text, 0);
            } else {
                ui_text_set_index(text, 1);
            }
            break;
        case ALARM_TEXT_AM_PM_INTM4:
            log_info("sub_min : %d\n", alarm_set_time[3]);
            if (alarm_set_time[3] < 720) {
                ui_text_set_index(text, 0);
            } else {
                ui_text_set_index(text, 1);
            }
            break;
        case ALARM_TEXT_AM_PM_INTM5:
            log_info("sub_min : %d\n", alarm_set_time[4]);
            if (alarm_set_time[4] < 720) {
                ui_text_set_index(text, 0);
            } else {
                ui_text_set_index(text, 1);
            }
            break;
        default:
            break;

        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_AM_PM_INTM1)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_AM_PM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_AM_PM_INTM2)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_AM_PM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_AM_PM_INTM3)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_AM_PM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_AM_PM_INTM4)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_AM_PM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_AM_PM_INTM5)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_AM_PM_onchange,
};
static int alarm_number_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_number *number = (struct ui_number *)_ctrl;
    struct unumber num;
    u8 record = 0;
    u8 index;
    switch (event) {
    case ON_CHANGE_INIT:
        switch (number->text.elm.id) {
        case ALARM_NUMB_PRE_ITEM1:
            index = 0;
            break;
        case ALARM_NUMB_PRE_ITEM2:
            index = 1;
            break;
        case ALARM_NUMB_PRE_ITEM3:
            index = 2;
            break;
        case ALARM_NUMB_PRE_ITEM4:
            index = 3;
            break;
        case ALARM_NUMB_PRE_ITEM5:
            index = 4;
            break;
        case ALAEM_SET_NUM:
            index = sel_index;
            break;
        case ALARM_TIME_NUMB:
            index = sel_index;
            record = 1;//这个时候可以记录信息
            break;
        default:
            return false;
            break;
        }
        u8 h_index;
        get_alarm_number2table(index, &h_index);
        if (index > MAX_ALARM_ITEM) {
            log_info("%s %d\n", __func__, __LINE__);
            break;
        }
        T_ALARM alarm;
        u8 info_err =  alarm_get_info(&alarm, h_index);
        if (record) {
            week = alarm.mode;
            if (week == 1) {
                week = 0xfe;
            }
            if (!info_err) {
                hour = alarm.time.hour;
                min = alarm.time.min;
            } else {
                alarm.time.hour = hour;
                alarm.time.min = min;
            }
        }
        if (number->text.elm.id == ALAEM_SET_NUM) { // 从当前时间开始设置闹钟
            struct sys_time current_time = {0};
            extern void watch_file_get_sys_time(struct sys_time * time);
            watch_file_get_sys_time(&current_time);
            hour = alarm.time.hour = current_time.hour;
            min = alarm.time.min	= current_time.min;
        }
        log_info(">>> hour : %d min : %d %d %d\n", hour, min, h_index, info_err);
        num.type = TYPE_NUM;
        num.numbs = 2;
        num.number[0] = alarm.time.hour;
        num.number[1] = alarm.time.min;

        ui_number_update(number, &num);
        //*
        switch (index) {
        case 0:
            alarm_set_time[0] = (u16)(alarm.time.hour * 60 + alarm.time.min);
            break;
        case 1:
            alarm_set_time[1] = (u16)(alarm.time.hour * 60 + alarm.time.min);
            break;
        case 2:
            alarm_set_time[2] = (u16)(alarm.time.hour * 60 + alarm.time.min);
            break;
        case 3:
            alarm_set_time[3] = (u16)(alarm.time.hour * 60 + alarm.time.min);
            break;
        case 4:
            alarm_set_time[4] = (u16)(alarm.time.hour * 60 + alarm.time.min);
            break;
        default:
            break;
        }
        //*/
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return false;
}




static alarm_vertlist_child_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct element *elm  = (struct element *) ctrl;
    int type = ui_id2type(elm->id);
    /* log_info("LAQ text alarm type = %d" , type); */
    switch (type) {
    case CTRL_TYPE_PIC:
        alarm_vertlist_pic_onchange(ctrl, e);
        break;
    case CTRL_TYPE_NUMBER:
        alarm_number_onchange(ctrl, e, arge);
        break;
    }
    return false;
}

static int ALARM_VERTLIST_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        u8 item_index = alarm_get_total() + 1;
        sel_index = item_index;
        week = 0;
        back_id = 0;
        log_info("LAQ text item_index is %d line = %d", item_index, __LINE__);
        ui_grid_set_item_num(grid, item_index);
        ui_set_default_handler(grid, alarm_vertlist_child_ontouch, NULL, alarm_vertlist_child_onchange);
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_INIT:

        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(grid, NULL, NULL, NULL);
        break;
    default:
        break;
    }
    return false;
}

static int ALARM_VERTLIST_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
    //LAQ 处理无up事件
    struct rect *rect;
    int i;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        struct rect r;
        struct element *p, *n;


        if (move_flag) {
            move_flag = 0;
            break;
        }
        int sel_item = ui_grid_cur_item(grid);
        struct element *elm = &grid->item[sel_item].elm;
        log_info("sel_item is %d", sel_item);
#if 0
        list_for_each_child_element_reverse(p, n, elm) {
            if (!p->css.invisible) {
                ui_core_get_element_abs_rect(p, &r);
                if (in_rect(&r, &e->pos)) {
                    p->handler->ontouch(p, e);
                    break;
                }
            }
        }
#endif
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    }


    return false;
}

REGISTER_UI_EVENT_HANDLER(ALARM_VERTLIST)
.ontouch = ALARM_VERTLIST_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_VERTLIST_onchange,
};

static u8 deal_week_data(u8 temp, u8 *index_buf)
{
    u8 temp_index = 0;
    u8 count = 0;
    log_info("LAQ the temp is 0x%x", temp);
    if (temp == E_ALARM_MODE_EVERY_DAY || temp == 0xfe) {
        log_info("LAQ alarm temp == every_day!");
        index_buf[0] = 8;
        count = 1;
    } else if (temp == E_ALARM_MODE_ONCE) {
        log_info("LAQ alarm temp == mode once!");
        index_buf[0] = 7;
        count = 1;
    } else {
        for (u8 i = 1 ; i < 8 ; i++) {
            if (temp & BIT(i)) {
                index_buf[count] = i - 1;
                count++;
            }
        }
        return count;
    }
    return count;
}

static int ALARM_TEXT_WEEK_ITEM_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_text *text = (struct ui_text *) ctrl;
    static u16 week_text_buf[MAX_ALARM_ITEM][7];
    u8 text_index_size = 0;
    u8 index = 0;
    switch (e) {
    case ON_CHANGE_INIT:
        switch (text->elm.id) {
        case ALARM_TEXT_WEEK_ITEM1:
            index = 0;
            break;
        case ALARM_TEXT_WEEK_ITEM2:
            index = 1;
            break;
        case ALARM_TEXT_WEEK_ITEM3:
            index = 2;
            break;
        case ALARM_TEXT_WEEK_ITEM4:
            index = 3;
            break;
        case ALARM_TEXT_WEEK_ITEM5:
            index = 4;
            break;
        case ALARM_MODE_REP_TEXT:
            index = sel_index;
            break;
        default:
            return false;
            break;

        }
        u8 h_index;
        get_alarm_number2table(index, &h_index);
        if (index > MAX_ALARM_ITEM) {
            break;
        }
        T_ALARM alarm;
        alarm_get_info(&alarm, h_index);
        u8 index_buf[6];
        text_index_size = deal_week_data(alarm.mode, index_buf);
        put_buf(index_buf, sizeof(index_buf));
        ui_text_set_combine_index(text, week_text_buf[index], index_buf, text_index_size);
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_WEEK_ITEM1)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_WEEK_ITEM2)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_WEEK_ITEM3)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_WEEK_ITEM4)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_WEEK_ITEM5)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_TEXT_WEEK_ITEM6)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};
REGISTER_UI_EVENT_HANDLER(ALARM_MODE_REP_TEXT)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = ALARM_TEXT_WEEK_ITEM_onchange,
};




static int ALARM_SET_NUM_OK_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_text *_text = (struct ui_text *) ctrl;
    //__alarm->move_invatil = 0;
    log_info("LAQ e->event is %d", e->event);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        // if(__alarm->move_invatil){
        //  实际切换日期选择页面， 暂时用于验证能否动态改变闹钟显示的条数
        log_info("LAQ 2 text %d : %d ", hour, min);
        //}
        return true;

    case ELM_EVENT_TOUCH_UP:
        log_info("LAQ 2 text %d : %d ", hour, min);
        ui_hide(ALARM_ADD_LAYOUT_2);
        if (back_id) {
            /**********获取当前闹钟VM中的name_len,赋值给即将写进VM的闹钟结构体***************/
            u8 h_index;
            get_alarm_number2table(sel_index, &h_index);
            T_ALARM alarm;
            alarm_get_info(&alarm, h_index);
            /**********获取当前闹钟VM中的name_len,赋值给即将写进VM的闹钟结构体***************/
            u8 index;
            T_ALARM set_alarm;
            get_alarm_number2table(sel_index, &index);
            set_alarm.mode = week;
            set_alarm.sw = 1;
            set_alarm.time.hour = hour;
            set_alarm.time.min = min;
            set_alarm.index = index;
            set_alarm.name_len = alarm.name_len;  /////////
            alarm_add(&set_alarm, index);
            ui_show(back_id);
        } else {
            ui_show(ALARM_ADD_LAYOUT_3);
        }
        return true;
        break;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(ALARM_SET_NUM_OK)
.ontouch = ALARM_SET_NUM_OK_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};


//加减时间控件
static int ALARM_HOUR_MIN_DEAL_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    case ELM_EVENT_TOUCH_UP:
        switch (elm->id) {
        case ALARM_ADD_HOUR:
            hour++;
            if (hour > 23) {
                hour = 0;
            }
            break;
        case ALARM_ADD_MIN:
            min++;
            if (min > 59) {
                min = 0;
            }
            break;
        case ALARM_CUT_HOUR:
            if (!(hour--)) {
                hour = 23;
            }
            break;
        case ALARM_CUT_MIN:
            if (!(min--)) {
                min = 59;
            }
            break;
        }
        struct unumber num;
        num.type = TYPE_NUM;
        num.numbs = 2;
        num.number[0] = hour;
        num.number[1] = min;
        ui_number_update_by_id(ALAEM_SET_NUM, &num);

        log_info("LAQ text %d : %d ", hour, min);

        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ALARM_ADD_HOUR)
.ontouch = ALARM_HOUR_MIN_DEAL_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};
REGISTER_UI_EVENT_HANDLER(ALARM_ADD_MIN)
.ontouch = ALARM_HOUR_MIN_DEAL_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};
REGISTER_UI_EVENT_HANDLER(ALARM_CUT_HOUR)
.ontouch = ALARM_HOUR_MIN_DEAL_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};
REGISTER_UI_EVENT_HANDLER(ALARM_CUT_MIN)
.ontouch = ALARM_HOUR_MIN_DEAL_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};

/***************************************************************************************
  LAQ PAGE26 页面处理部分
 ****************************************************************************************/
/***************************************************************************************
  LAQ PAGE28 页面处理部分
 ****************************************************************************************/
static int ALARM_SET_WEEK_OK_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_pic *_pic = (struct ui_pic *) ctrl;
    T_ALARM set_alarm;
    static u8 flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        log_info("LAQ e->event is %d", e->event);
        if (flag) {
            if (week == 0xfe) {
                week = E_ALARM_MODE_EVERY_DAY;
            }
            /**********获取当前闹钟VM中的name_len,赋值给即将写进VM的闹钟结构体***************/
            u8 h_index;
            get_alarm_number2table(sel_index, &h_index);
            T_ALARM alarm;
            alarm_get_info(&alarm, h_index);
            /**********获取当前闹钟VM中的name_len,赋值给即将写进VM的闹钟结构体***************/
            u8 index;
            get_alarm_number2table(sel_index, &index);
            /* log_info(">>>>>>>>>>>>>>>>>>>>> %d %d %d\n",__LINE__,index,sel_index); */
            set_alarm.mode = week;
            set_alarm.sw = 1;
            set_alarm.time.hour = hour;
            set_alarm.time.min = min;
            set_alarm.index = index;
            set_alarm.name_len = alarm.name_len;
            alarm_add(&set_alarm, index);
            /* log_info(">>>>>>>>>>>>>>>>>>>>> %d %d\n",__LINE__,alarm_get_total()); */
            ui_hide(ALARM_ADD_LAYOUT_3);
            if (back_id) {
                ui_show(back_id);
            } else {
                /* hour = 0; */
                /* min  = 0; */
                ui_show(ALARM_ADD_LAYOUT_1);
            }
            return true;
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
        flag = 0;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        flag = 1;
        return true;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(ALARM_SET_WEEK_OK)
.ontouch = ALARM_SET_WEEK_OK_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};
// E_ALARM_MODE_EVERY_MONDAY    = 0x02,
// E_ALARM_MODE_EVERY_TUESDAY   = 0x04,
// E_ALARM_MODE_EVERY_WEDNESDAY = 0x08,
// E_ALARM_MODE_EVERY_THURSDAY  = 0x10,
// E_ALARM_MODE_EVERY_FRIDAY    = 0x20,
// E_ALARM_MODE_EVERY_SATURDAY  = 0x40,
// E_ALARM_MODE_EVERY_SUNDAY    = 0x80,
//
static int ALARM_WEEK_PIC_onchange(void *ctrl, enum element_change_event e, void *arge)
{

    struct ui_pic *pic = (struct ui_pic *) ctrl;
    u8 index = 0;
    switch (e) {
    case ON_CHANGE_INIT:
        switch (pic->elm.id) {
        case WEEK_PIC_1:
            if (week & E_ALARM_MODE_EVERY_MONDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        case WEEK_PIC_2:
            if (week & E_ALARM_MODE_EVERY_TUESDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        case WEEK_PIC_3:
            if (week & E_ALARM_MODE_EVERY_WEDNESDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        case WEEK_PIC_4:
            if (week & E_ALARM_MODE_EVERY_THURSDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        case WEEK_PIC_5:
            if (week & E_ALARM_MODE_EVERY_FRIDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        case WEEK_PIC_6:
            if (week & E_ALARM_MODE_EVERY_SATURDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        case WEEK_PIC_7:
            if (week & E_ALARM_MODE_EVERY_SUNDAY) {
                ui_pic_set_image_index(pic, 1);
            }
            break;
        default:
            break;
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return false;
}

static WEEK_PIC_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_pic *_pic = (struct ui_pic *) ctrl;
    static u8 key_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (key_flag) {
            switch (_pic->elm.id) {
            case WEEK_PIC_1:
                if (week & E_ALARM_MODE_EVERY_MONDAY) {
                    week &= ~E_ALARM_MODE_EVERY_MONDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_MONDAY)) {
                    week |= E_ALARM_MODE_EVERY_MONDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            case WEEK_PIC_2:
                if (week & E_ALARM_MODE_EVERY_TUESDAY) {
                    week &= ~ E_ALARM_MODE_EVERY_TUESDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_TUESDAY)) {
                    week |= E_ALARM_MODE_EVERY_TUESDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            case WEEK_PIC_3:
                if (week & E_ALARM_MODE_EVERY_WEDNESDAY) {
                    week &= ~ E_ALARM_MODE_EVERY_WEDNESDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_WEDNESDAY)) {
                    week |= E_ALARM_MODE_EVERY_WEDNESDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            case WEEK_PIC_4:
                if (week & E_ALARM_MODE_EVERY_THURSDAY) {
                    week &= ~ E_ALARM_MODE_EVERY_THURSDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_THURSDAY)) {
                    week |= E_ALARM_MODE_EVERY_THURSDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            case WEEK_PIC_5:
                if (week & E_ALARM_MODE_EVERY_FRIDAY) {
                    week &= ~ E_ALARM_MODE_EVERY_FRIDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_FRIDAY)) {
                    week |= E_ALARM_MODE_EVERY_FRIDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            case WEEK_PIC_6:
                if (week & E_ALARM_MODE_EVERY_SATURDAY) {
                    week &= ~ E_ALARM_MODE_EVERY_SATURDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_SATURDAY)) {
                    week |= E_ALARM_MODE_EVERY_SATURDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            case WEEK_PIC_7:
                if (week & E_ALARM_MODE_EVERY_SUNDAY) {
                    week &= ~ E_ALARM_MODE_EVERY_SUNDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 0);
                } else if (!(week & E_ALARM_MODE_EVERY_SUNDAY)) {
                    week |= E_ALARM_MODE_EVERY_SUNDAY;
                    ui_pic_show_image_by_id(_pic->elm.id, 1);
                }
                break;
            default:
                break;
            }
        }
        log_info("LAQ text pic up event and week is 0x%x", week);
        break;
    case ELM_EVENT_TOUCH_HOLD:
    case ELM_EVENT_TOUCH_MOVE:
        key_flag = 0;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        key_flag = 1;
        return true;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_1)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_2)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_3)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_4)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_5)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_6)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(WEEK_PIC_7)
.ontouch = WEEK_PIC_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_WEEK_PIC_onchange,
};

REGISTER_UI_EVENT_HANDLER(ALARM_TIME_NUMB)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = alarm_number_onchange,
};

REGISTER_UI_EVENT_HANDLER(ALAEM_SET_NUM)
.ontouch = NULL,
 .onkey = NULL,
  .onchange  = alarm_number_onchange,
};






static int ALAEM_DEL_TEXT_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct ui_text *text = (struct ui_text *) _ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        u8 h_index;
        get_alarm_number2table(sel_index, &h_index);
        if (sel_index > MAX_ALARM_ITEM) {
            break;
        }
        log_info("%s alarm delete = %d sel_index : %d h_index : %d\n", __func__, __LINE__, sel_index, h_index);
        alarm_delete(h_index);
        ui_hide(ALARM_ADD_LAYOUT_5);
        ui_show(ALARM_ADD_LAYOUT_1);
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(ALAEM_DEL_TEXT)
.ontouch = ALAEM_DEL_TEXT_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};



static int ALARM_MODE_SHOW_LIST_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
    static struct draw_context dc = {0};
    /* struct fb_map_user map; */
    //LAQ 处理无up事件
    struct rect *rect;
    int i;
    int sel_item = ui_grid_cur_item(grid);
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        struct rect r;
        struct element *p, *n;
        struct element *elm = &grid->item[sel_item].elm;


        if (move_flag) {
            move_flag = 0;
            break;
        }

        log_info("sel_item is %d", sel_item);

        switch (sel_item) {
        case 0:
            return false;
            break;
        case 1:
            ui_hide(ALARM_ADD_LAYOUT_5);
            ui_show(ALARM_ADD_LAYOUT_2);
            back_id = ALARM_ADD_LAYOUT_5;
            return true;
            break;
        case 2:
            ui_hide(ALARM_ADD_LAYOUT_5);
            ui_show(ALARM_ADD_LAYOUT_3);
            back_id = ALARM_ADD_LAYOUT_5;
            return true;
            break;
        }
#if 0
        list_for_each_child_element_reverse(p, n, elm) {
            if (!p->css.invisible) {
                ui_core_get_element_abs_rect(p, &r);
                if (in_rect(&r, &e->pos)) {
                    p->handler->ontouch(p, e);
                    break;
                }
            }
        }
#endif
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        log_info("line:%d", __LINE__);
        ui_hide(ALARM_ADD_LAYOUT_5);
        ui_show(ALARM_ADD_LAYOUT_1);
        return true;
        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        log_info("line:%d", __LINE__);
        ui_hide(ALARM_ADD_LAYOUT_5);
        ui_show(ALARM_ADD_LAYOUT_1);
        return true;
        break;
    }

    return false;
}

static int ALARM_MODE_SHOW_LIST_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        /* ui_set_default_handler(alarm_vertlist_child_ontouch , NULL  ,alarm_vertlist_child_onchange); */
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_INIT:

        break;
    case ON_CHANGE_RELEASE:
        /* ui_set_default_handler(NULL , NULL , NULL); */
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ALARM_MODE_SHOW_LIST)
.ontouch = ALARM_MODE_SHOW_LIST_ontouch,
 .onkey = NULL,
  .onchange  = ALARM_MODE_SHOW_LIST_onchange,
};


static int ALARM_ADD_LAYOUT_3_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        if (back_id == ALARM_ADD_LAYOUT_5) {
            ui_hide(ALARM_ADD_LAYOUT_3);
            ui_show(back_id);
            back_id = 0;
        } else {
            log_info("%s %d %x\n", __func__, __LINE__, back_id);
            back_id = 0;
            ui_hide(ALARM_ADD_LAYOUT_3);
            ui_show(ALARM_ADD_LAYOUT_2);
        }
        return true;

        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ALARM_ADD_LAYOUT_3)
.ontouch = ALARM_ADD_LAYOUT_3_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};




//上限提醒
static int ALARM_ADD_LAYOUT_4_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        ui_hide(ALARM_ADD_LAYOUT_4);
        ui_show(ALARM_ADD_LAYOUT_1);
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

REGISTER_UI_EVENT_HANDLER(ALARM_ADD_LAYOUT_4)
.ontouch = ALARM_ADD_LAYOUT_4_ontouch,
 .onkey = NULL,
  .onchange  = NULL,
};

//------------------------------闹钟响应界面-------------------------------//
extern void set_lcd_keep_open_flag(u8 flag);

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

static int show_pic_am_pm(int arg)
{
    ui_pic_show_image_by_id(PIC_AM_PM, 1);
    return 0;
}

static int time_alarm_response_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_time *time = (struct ui_time *)ctrl;
    struct sys_time s_time;
    struct utime time_r;

    switch (e) {
    case ON_CHANGE_INIT:
        if (!ui_auto_shut_down_disable()) {
            set_lcd_keep_open_flag(1);
        }
        get_sys_time(&s_time);
        time_r.year = s_time.year;
        time_r.month = s_time.month;
        time_r.day = s_time.day;
        time_r.hour = s_time.hour;;
        time_r.min = s_time.min;
        time_r.sec = s_time.sec;
        if (s_time.hour > 12) {
            time_r.hour = s_time.hour - 12;
            ui_set_call(show_pic_am_pm, 0);
        }
        ui_time_update(time, &time_r);
        UI_MOTO_RUN(1);
        break;
    case ON_CHANGE_RELEASE:
        UI_MOTO_RUN(0);
        set_lcd_keep_open_flag(0);
        ui_auto_shut_down_enable();
        UI_MOTO_RUN(0);
        struct watch_execise __execise_hd;
        watch_execise_handle_get(&__execise_hd);
        u8 sport_status = __execise_hd.execise_ctrl_status_get();
        if ((sport_status != 4) & (sport_status != 0)) {//运动过程中回到运动页面
            printf("%s %d", __func__, __LINE__);
            bt_task_set_window_id(ID_WINDOW_SPORT_INFO);
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(TIME_ALARM_RESPONSE)
.onchange = time_alarm_response_onchange,
 .onkey = NULL,
  .ontouch  = NULL,
};

static int button_alarm_response_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct button *button = (struct button *)ctrl;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (alarm_active_flag_get()) {
            alarm_stop(0);
        }
        switch (button->elm.id) {
        case BUTTON_ALARM_CLOSE:
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_ALARM_RING_STOP);
            break;
        case BUTTON_SNOOZE_MODE:
            extern void alarm_snooze();
            alarm_snooze();
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_ALARM_RING_SOON);
            break;
        }
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(BUTTON_ALARM_CLOSE)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_alarm_response_ontouch,
};
REGISTER_UI_EVENT_HANDLER(BUTTON_SNOOZE_MODE)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_alarm_response_ontouch,
};


static int alarm_layout_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
        printf("%s,%d \n", __func__, __LINE__);
        UI_WINDOW_PREEMPTION_POP(ID_WINDOW_ALARM_RING_START);
        return true;
        break;
    default :
        break;
    }
    return false;

}
/* REGISTER_UI_EVENT_HANDLER(ID_WINDOW_ALARM_RING_START) */
/* .onchange = NULL, */
/* .onkey = NULL, */
/* .ontouch = alarm_layout_ontouch, */
/* }; */

static int layout_alarm_timer_handler;

void return_watch_page(void *p)
{
    printf("%s %d", __func__, __LINE__);
    if (layout_alarm_timer_handler) {
        /* UI_HIDE_CURR_WINDOW(); */
        /* UI_SHOW_WINDOW(ID_WINDOW_BT); */
        /* struct watch_execise __execise_hd; */
        /* watch_execise_handle_get(&__execise_hd); */
        /* u8 sport_status = __execise_hd.execise_ctrl_status_get(); */
        /* if ((sport_status != 4) & (sport_status != 0)) {//运动过程中回到运动页面 */
        /* printf("%s %d",__func__,__LINE__); */
        /* bt_task_set_window_id(ID_WINDOW_SPORT_INFO); */
        /* } */
        UI_WINDOW_PREEMPTION_POP(ID_WINDOW_ALARM_RING_START);
        /* u8 app = app_get_curr_task(); */
        /* if (app == APP_RTC_TASK) { */
        /* app_task_switch_back(); */
        /* } */
        layout_alarm_timer_handler = 0;
    }
}

static int layout_alarm_close_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    switch (e) {
    case ON_CHANGE_INIT:
        if (layout_alarm_timer_handler == 0) {
            layout_alarm_timer_handler = sys_timeout_add(NULL, return_watch_page, 3000);
        }
        break;
    case ON_CHANGE_RELEASE:
        if (layout_alarm_timer_handler) {
            sys_timeout_del(layout_alarm_timer_handler);
            layout_alarm_timer_handler = 0;
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(LAYOUT_ALARM_CLOSE)
.onchange = layout_alarm_close_onchange,
 .onkey = NULL,
  .ontouch = alarm_layout_ontouch,
};

static int layout_alarm_snooze_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    switch (e) {
    case ON_CHANGE_INIT:
        if (layout_alarm_timer_handler == 0) {
            layout_alarm_timer_handler = sys_timeout_add(NULL, return_watch_page, 3000);
        }
        break;
    case ON_CHANGE_RELEASE:
        if (layout_alarm_timer_handler) {
            sys_timeout_del(layout_alarm_timer_handler);
            layout_alarm_timer_handler = 0;
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(LAYOUT_ALARM_SNOOZE)
.onchange = layout_alarm_close_onchange,
 .onkey = NULL,
  .ontouch = alarm_layout_ontouch,
};


static int write_alarm_to_vm(void *priv)
{
    if ((int)priv == (int)SYSCFG_WRITE_ERASE_STATUS) {
        alarm_delete_all();
    }
    return 0;
}

REGISTER_WATCH_SYSCFG(alarm_ops) = {
    .name = "alarm",
    .read = NULL,
    .write = write_alarm_to_vm,
};

#endif /*TCFG_APP_RTC_EN*/
#endif /* #if (!TCFG_LUA_ENABLE) */





