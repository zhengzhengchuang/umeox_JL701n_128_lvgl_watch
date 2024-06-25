
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

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_CALENDAR
#define STYLE_NAME  JL

#define NO_MONTH_TEXT_COLOR 0x8410
#define MONTH_TEXT_COLOR 0xffff
#define DAY_BG_COLOR 0xb63b
#define NO_DAY_BG_COLOR 0x0000


struct ui_calendar {
    struct sys_time list_start_date;//表格开始日期
    struct sys_time list_point_date;
    struct sys_time list_month_start;
    u16 item_height;//项高+间距
    u8 list_num;//项数
    u8 status;//初始化状态
    u8 month;//当前月份
    u8 *datestr;//用于刷新
} __calendar;
#define __this (&__calendar)

#define ABS(x) ((x>0)?(x):(-x))
#define DIR(x) ((x>0)?(1):(-1))
/*
   		表格 6*7 index
	00 01 02 03 04 05 06
	07 08 09 10 11 12 13
	14 15 16 17 18 19 20
	21 22 23 24 25 26 27
	28 29 30 31 32 33 34
	35 36 37 38 39 40 41
 */
/***************************日期工具函数*************************************/
u8 month_list_nonleap[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
u8 month_list_leap[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//格里历最早不超过1582年10月4日，这里习惯上取1900.1.1为起点
#define YEAR_S 1900
#define MONTH_S 1
#define DAY_S 1
//获取系统当前日期
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
//日期转成距离起始时间的天数
static int __time2int(struct sys_time *time)
{
    if (time->year < YEAR_S) {
        return -1;
    }
//	 printf("%syyyy=%d mm=%d dd=%d ",__func__,time->year,time->month,time->day);
    u8 *month_list = (time->year % 4 == 0) ? month_list_leap : month_list_nonleap;
    int day = (time->year - YEAR_S) * 365 + (time->year - YEAR_S - 1) / 4;
    for (int i = 0; i < (time->month - MONTH_S); i++) {
        day += month_list[i];
    }
    day += (time->day - DAY_S);
//	 printf("%s day=%d",__func__,day);
    return day;
}
//天数转日期
static int __int2time(struct sys_time *time, int day)
{
//	 printf("%s day=%d",__func__,day);
    if (day < 0) {
        return -1;
    }
    time->year = YEAR_S + ((float)day / 365);
    if (((time->year - YEAR_S) * 365 + (time->year - YEAR_S - 1) / 4) > day) {
        time->year--;
    }
    day = day - ((time->year - YEAR_S) * 365 + (time->year - YEAR_S - 1) / 4);
//	 printf("day1=%d y_d+leap=%d",day,((time->year-YEAR_S)*365+ (time->year-YEAR_S-1)/4 ));
    u8 *month_list = (time->year % 4 == 0) ? month_list_leap : month_list_nonleap;
    for (int i = 0; i < 12; i++) {
        if (day - month_list[i] < 0) {
            time->month = i + MONTH_S;
            time->day = day + DAY_S;
            break;
        } else {
            day -= month_list[i];
        }
    }
//	printf("%syyyy=%d mm=%d dd=%d ",__func__,time->year,time->month,time->day);
    return 0;
}
//日期增
static void day_up(struct sys_time *time, int day)
{
    int ret = __int2time(time, (__time2int(time) + day));
    if (ret == -1) {
//		printf("%s Date=%04d-%02d-%02d",__func__,time->year,time->month,time->day);
    }
}
//日期减
static void day_down(struct sys_time *time, int day)
{
    int ret = __int2time(time, (__time2int(time) - day));
    if (ret == -1) {
//		printf("%s Date=%04d-%02d-%02d",__func__,time->year,time->month,time->day);
    }
}
//日期转字符//******注意配字符串长度********
static void __time2str(u8 *str, struct sys_time *time)
{
    if (str) {
        sprintf(str, "%02d", time->day);
    } else {
        printf("str ==NULL");
    }
}
//蔡勒公式:日期转星期
static int __date2week(struct sys_time *ntime)
{
    struct sys_time time;
    memcpy(&time, ntime, sizeof(struct sys_time));
    if (time.month == 1 || time.month == 2) {
        time.year--;
        time.month += 12;
    }
    int centary = time.year / 100;
    int yy = time.year - centary * 100;
    int week = yy + yy / 4 + centary / 4 - 2 * centary + 26 * (time.month + 1) / 10 + time.day - 1;
    while (week < 0) {
        week += 7;
    }
    week %= 7;
    /* printf("%s=%d",__func__,week); */
    return week;
}
/***************************数据刷新*************************************/
//刷新日历
//index为控件序号
static void calendar_date_get(int index)
{
    __this->list_point_date = __this->list_start_date;
    day_up(&__this->list_point_date, index);
}
//获取月份//可用于单独刷到控件上显示
static int calendar_month_get(void)
{
    struct sys_time time = __this->list_start_date;
    day_up(&time, __this->list_num / 2); //以中间位置的月份为准
    __this->month = time.month;
    /* printf("%s %d",__func__,time.month); */
    return time.month;
}
static int calendar_year_get(void)
{
    struct sys_time time = __this->list_start_date;
    day_up(&time, __this->list_num / 2); //以中间位置的月份为准
    /* printf("%s %d",__func__,time.month); */
    return time.year;
}
//刷新文本
static int grid_child_cb(void *_ctrl, int id, int type, int index)
{
    switch (type) {
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->_format, "text")) {
            __time2str((__this->datestr + (index * 2)), &__this->list_point_date);
            text->elm.css.background_color = NO_DAY_BG_COLOR; //默认背景
            if (__this->list_point_date.month == calendar_month_get()) { //是否为当月
                text->attrs.color = MONTH_TEXT_COLOR; //修改“当月”字体颜色
                struct sys_time today_time;
                get_sys_time(&today_time);
                if ((__this->list_point_date.year == today_time.year) && ((__this->list_point_date.month == today_time.month)) && (__this->list_point_date.day == today_time.day)) { //是否为当天
                    text->elm.css.background_color = DAY_BG_COLOR; //修改背景
                }
            } else {
                text->attrs.color = NO_MONTH_TEXT_COLOR;	//修改“非当月”字体颜色
            }
            ui_text_set_utf8_str(text, text->_format, (__this->datestr + (index * 2)), 2, FONT_DEFAULT);

        }
        break;
    default:
        break;
    }
    return 0;
}
int calendar_date_show(void)
{
    static struct unumber num1, num2;
    num1.type = TYPE_NUM;
    num1.number[0] = calendar_year_get();
    num1.numbs = 1;
    num2.type = TYPE_NUM;
    num2.number[0] = calendar_month_get();
    num2.numbs = 1;
    int ret = 0;
    ret += ui_number_update_by_id(CALENDAR_YEAR, &num1);
    ret += ui_number_update_by_id(CALENDAR_MONTH, &num2);
    return ret;
}

//初始化
static int calendar_children_init(struct ui_grid *grid)
{
    struct element *k;
    if (!grid) {
        return 0;
    }
    if (!__this->status) {
        __this->list_num = grid->avail_item_num;
        get_sys_time(&__this->list_start_date);
        __this->list_start_date.day = 1; //从当月的1号开始显示
        __this->list_month_start = __this->list_start_date;
        //第一行七个控件用于下拉时显示过渡效果
        //int date_offset=7+__date2week(&__this->list_start_date);
        int date_offset = __date2week(&__this->list_start_date);
        day_down(&__this->list_start_date, date_offset);

        if (!__this->datestr) {
            __this->datestr = zalloc(__this->list_num * 10);
        }
        __this->status = 1;
    }
    struct rect rect, parent_rect;
    ui_core_get_element_abs_rect(&grid->item[grid->hi_index].elm, &rect);
    ui_core_get_element_abs_rect(&grid->elm, &parent_rect);
    int y_interval = ((float)(grid->y_interval * parent_rect.height) / 10000 + 0.5);//间隔
    __this->item_height = rect.height + y_interval; //行高+间隔

    for (int i = 0; i < grid->avail_item_num; i++) {
        calendar_date_get(i);
        list_for_each_child_element(k, &grid->item[i].elm) {
            grid_child_cb(k, k->id, ui_id2type(k->id), i);
        }
    }
    return 0;
}
//滑动刷新
static int calendar_children_redraw(struct ui_grid *grid)
{
    struct element *k;
    if (!grid) {
        return 0;
    }
    if (!__this->status) {
//		printf("%s error",__func__);
        return 0;
    }
    for (int i = 0; i < grid->avail_item_num; i++) {
        calendar_date_get(i);
        list_for_each_child_element(k, &grid->item[i].elm) {
            grid_child_cb(k, k->id, ui_id2type(k->id), i);
        }
    }
    return 0;
}


static int calendar_child_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);
    if (event == ON_CHANGE_UPDATE_ITEM) {
        int index = (u32)arg;
        grid_child_cb(elm, elm->id, type, index);
    }
    return 0;
}

//按月份增加
int calendar_month_up()
{
    int month = calendar_month_get();
    if ((month + 1) > 12) {
        __this->list_month_start.month = 1;
        __this->list_month_start.year++;
        __this->list_month_start.day = 1;
    } else {
        __this->list_month_start.month = month + 1;
        __this->list_month_start.day = 1;
    }
//    printf("%s %d %d %d",__func__,__this->list_month_start.year,__this->list_month_start.month,__this->list_month_start.day);
    //int date_offset=7+__date2week(&__this->list_month_start);
    int date_offset = __date2week(&__this->list_month_start);
    __this->list_start_date = __this->list_month_start;
    day_down(&__this->list_start_date, date_offset);
//	printf("%d %d %d",__this->list_start_date.year,__this->list_start_date.month,__this->list_start_date.day);
    struct ui_grid *grid = (struct ui_grid *)ui_core_get_element_by_id(GRID_CALENDAR);
    calendar_children_redraw(grid);
    ui_core_redraw(&grid->elm);
    calendar_date_show();
    return 0;
}
int calendar_month_down()
{
    int month = calendar_month_get();
    if ((month - 1) < 1) {
        __this->list_month_start.month = 12;
        __this->list_month_start.year--;
        __this->list_month_start.day = 1;
    } else {
        __this->list_month_start.month = month - 1;
        __this->list_month_start.day = 1;
    }
//    printf("%s %d %d %d",__func__,__this->list_month_start.year,__this->list_month_start.month,__this->list_month_start.day);
    //int date_offset=7+__date2week(&__this->list_month_start);
    int date_offset = __date2week(&__this->list_month_start);
    __this->list_start_date = __this->list_month_start;
    day_down(&__this->list_start_date, date_offset);
//	printf("%d %d %d",__this->list_start_date.year,__this->list_start_date.month,__this->list_start_date.day);
    struct ui_grid *grid = (struct ui_grid *)ui_core_get_element_by_id(GRID_CALENDAR);
    calendar_children_redraw(grid);
    ui_core_redraw(&grid->elm);
    calendar_date_show();
    return 0;
}
//表格
static int grid_calendar_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
//	printf("%s event==%d",__func__,e->event);
    switch (e->event) {
    case ELM_EVENT_TOUCH_U_MOVE:
        calendar_month_up();
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        calendar_month_down();
        break;
    case ELM_EVENT_TOUCH_MOVE:
        return true;
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        UI_WINDOW_BACK_SHOW(2);
        break;
    default:
        break;
    }
    return true;//接管消息
}

static int grid_calendar_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        ui_auto_shut_down_enable();
        __this->list_num = grid->avail_item_num;
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        calendar_children_init(grid);
        break;
    case ON_CHANGE_RELEASE:
        if (__this->datestr) {
            free(__this->datestr);
            __this->datestr = NULL;
        }
        __this->status = 0;
        break;
    case ON_CHANGE_SHOW_COMPLETED:
        break;
    default:
        return false;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(GRID_CALENDAR)
.onchange = grid_calendar_onchange,
 .onkey = NULL,
  .ontouch = grid_calendar_ontouch,
};
static int MONTH_CTRL_ontouch(void *ctr, struct element_touch_event *e)
{
    struct button *ubutton = (struct button *)ctr;
    static u8 flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_MOVE:
        flag = 0;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        flag = 1;
        break;
    case ELM_EVENT_TOUCH_UP:
        if (flag) {
            switch (ubutton->elm.id) {
            case MONTH_UP:
                calendar_month_up();
                break;
            case MONTH_DOWN:
                calendar_month_down();
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
    return true;//接管消息
//	return false;
}
REGISTER_UI_EVENT_HANDLER(MONTH_UP)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = MONTH_CTRL_ontouch,
};
REGISTER_UI_EVENT_HANDLER(MONTH_DOWN)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = MONTH_CTRL_ontouch,
};
static int calendar_date_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_number *ui_num = (struct ui_number *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        struct sys_time date;
        get_sys_time(&date);
        struct unumber num;
        num.type = TYPE_NUM;
        num.numbs = 1;
        num.number[0] = (ui_num->text.elm.id == CALENDAR_YEAR) ? date.year : date.month;
        ui_number_update(ui_num, &num);
        break;

    case ON_CHANGE_RELEASE:
        break;
    default:
        return false;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(CALENDAR_YEAR)
.onchange = calendar_date_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(CALENDAR_MONTH)
.onchange = calendar_date_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif

#endif
