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
#if TCFG_UI_ENABLE_ALTIMETER

#define STYLE_NAME  JL
#define REFRESH_CNT 24  //一天采24个点

static u32 first_point = 0;
static u16 altimeter_x = 0;
static u16 altimeter_y = 0;
static u16 barometer_x = 0;
static u16 barometer_y = 0;
static u16 max_x = 0;
static u16 min_x = 0;
static u16 max_y = 0;
static u16 min_y = 0xffff;
static u16 draw_tmep_buf[REFRESH_CNT];

extern void ui_core_redraw_area(struct rect *rect);

static int ui_move(struct element *elm, int x_offset, int y_offset)
{
    struct element_css *parent_css;
    struct element_css *css;
    struct rect rect;
    struct rect parent_rect;
    int css_left;
    int css_top;

    if (!elm) {
        return 0;
    }

    ui_core_get_element_abs_rect(elm->parent, &parent_rect);
    /* ui_core_get_element_abs_rect(elm, &rect); */
    /*  */
    /* rect.left = 0; */
    /* rect.top = 0; */

    css_left = x_offset * 10000 / parent_rect.width;
    css_top = y_offset * 10000 / parent_rect.height;

    css = ui_core_get_element_css(elm);
    css->left += css_left;
    css->top += css_top;
#if 0
    if (css->left > 0) {
        css->left = 0;
        ui_core_redraw_area(&rect);
        ui_core_redraw(elm->parent);
        return 0;
    }

    if (css->left < -css->width) {
        css->left = -css->width;
        ui_core_redraw_area(&rect);
        ui_core_redraw(elm->parent);
        return 0;
    }

    if (css->top > 0) {
        css->top = 0;
        ui_core_redraw_area(&rect);
        ui_core_redraw(elm->parent);
        return 0;
    }

    if (css->top < -css->height) {
        css->top = -css->height;
        ui_core_redraw_area(&rect);
        ui_core_redraw(elm->parent);
        return 0;
    }
#endif
    /* ui_core_redraw_area(&rect); */
    ui_core_redraw(elm->parent);

    return 1;
}

static int altimeter_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}

static int altimeter_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_D_MOVE:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(PAGE_64);//跳转气压计界面
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PAGE_63)
.onchange = altimeter_onchange,
 .onkey = NULL,
  .ontouch = altimeter_ontouch,
};

int refresh_max_height(int arg)
{
    struct element *elm_coordinate_axis = ui_core_get_element_by_id(ALTIMETER_COORDINATE_PIC);
    struct element *elm_max_dot = ui_core_get_element_by_id(MAX_HEIGHT_PIC);
    struct rect rect_coordinate_axis = {0};
    struct rect rect_max_dot = {0};
    int x_offset, y_offset;

    ui_core_get_element_abs_rect(elm_coordinate_axis, &rect_coordinate_axis);
    ui_core_get_element_abs_rect(elm_max_dot, &rect_max_dot);
    x_offset = rect_coordinate_axis.left + (rect_coordinate_axis.width / REFRESH_CNT) * (max_x + 1) - (rect_max_dot.left + rect_max_dot.width / 2);
    y_offset = draw_tmep_buf[max_x] - (rect_max_dot.top + rect_max_dot.height / 3);
    ui_move(elm_max_dot, x_offset, y_offset);
    return 0;
}

static int max_height_onchange(void *_ctrl, enum element_change_event event, void *arg)
{

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(refresh_max_height, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(MAX_HEIGHT_PIC)
.onchange = max_height_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int refresh_min_height(int arg)
{
    struct element *elm_coordinate_axis = ui_core_get_element_by_id(ALTIMETER_COORDINATE_PIC);
    struct element *elm_min_dot = ui_core_get_element_by_id(MIN_HEIGHT_PIC);
    struct rect rect_coordinate_axis = {0};
    struct rect rect_min_dot = {0};
    int x_offset, y_offset;

    ui_core_get_element_abs_rect(elm_coordinate_axis, &rect_coordinate_axis);
    ui_core_get_element_abs_rect(elm_min_dot, &rect_min_dot);
    x_offset = rect_coordinate_axis.left + (rect_coordinate_axis.width / REFRESH_CNT) * (min_x + 1) - (rect_min_dot.left + rect_min_dot.width / 2);
    y_offset = draw_tmep_buf[min_x] - (rect_min_dot.top + rect_min_dot.height / 2);
    ui_move(elm_min_dot, x_offset, y_offset);
    return 0;
}

static int min_height_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct element *elm = (struct element *)_ctrl; */
    /* y_printf("et:%d el:%d ew:%d eh:%d",elm->css.top,elm->css.left,elm->css.width,elm->css.height); */

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(refresh_min_height, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(MIN_HEIGHT_PIC)
.onchange = min_height_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int refresh_altimeter_pointer(int arg)
{
    struct element *elm_coordinate_axis = ui_core_get_element_by_id(ALTIMETER_COORDINATE_PIC);
    struct element *elm_height_point = ui_core_get_element_by_id(ALTIMETER_POINTER_PIC);
    struct rect rect_coordinate_axis = {0};
    struct rect rect_height_point = {0};
    int x_offset;

    ui_core_get_element_abs_rect(elm_coordinate_axis, &rect_coordinate_axis);
    ui_core_get_element_abs_rect(elm_height_point, &rect_height_point);
    x_offset = rect_coordinate_axis.left + (rect_coordinate_axis.width / REFRESH_CNT) * REFRESH_CNT - rect_height_point.left;//(rect_height_point.left + rect_height_point.width / 2);
    ui_move(elm_height_point, x_offset, 0);
    return 0;
}

static int altimeter_point_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct element *elm = (struct element *)_ctrl; */
    /* y_printf("et:%d el:%d ew:%d eh:%d",elm->css.top,elm->css.left,elm->css.width,elm->css.height); */

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(refresh_altimeter_pointer, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(ALTIMETER_POINTER_PIC)
.onchange = altimeter_point_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int show_altimeter_max_min_height(int arg)
{
    struct unumber num;
    struct rect rect = {0};
    struct element *elm = ui_core_get_element_by_id(ALTIMETER_COORDINATE_PIC);
    ui_core_get_element_abs_rect(elm, &rect);

    num.type = TYPE_NUM;
    num.numbs = 1;
    num.number[0] = max_y;
    ui_number_update_by_id(HEIGHT_MAX_NUM, &num);
    num.number[0] = min_y;
    ui_number_update_by_id(HEIGHT_MIN_NUM, &num);
    num.number[0] = rect.top + rect.height - draw_tmep_buf[REFRESH_CNT - 1];
    ui_number_update_by_id(CUR_HEIGHT_NUM, &num);
    return 0;
}

static int altimeter_coordinate_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};
    u8 i;

    ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动

    switch (event) {
    case ON_CHANGE_INIT:
        first_point = rand32() % 48;
        log_info("first_point:%d", first_point);
        for (i = 0; i < REFRESH_CNT; i++) {
            draw_tmep_buf[i] = rect.top + rect.height - (rand32() % 48) - 5; //减5为了使数据超过坐标轴
            if ((rect.top + rect.height - draw_tmep_buf[i]) >= max_y) {
                max_y = rect.top + rect.height - draw_tmep_buf[i];
                max_x = i;
            }
            if ((rect.top + rect.height - draw_tmep_buf[i]) <= min_y) {
                min_y = rect.top + rect.height - draw_tmep_buf[i];
                min_x = i;
            }
        }
        log_info("min_x:%d min_y:%d max_x:%d max_y:%d", min_x, min_y, max_x, max_y);
        ui_set_call(show_altimeter_max_min_height, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        /* y_printf("t:%d l:%d w:%d h:%d",rect.top,rect.left,rect.width,rect.height); */
        if (first_point > 25) {
            altimeter_x = rect.left;
            altimeter_y = rect.top + rect.height / 2;
        } else {
            altimeter_x = rect.left;
            altimeter_y = rect.top + rect.height - first_point - 5; //减5为了使数据超过坐标轴
        }
        /* ui_draw_line(dc, rect.left, rect.top, rect.left + rect.width, rect.top, 0xffff); */
        for (i = 1; i <= REFRESH_CNT; i++) {
            ui_draw_line(dc, altimeter_x, altimeter_y, altimeter_x + (rect.width / REFRESH_CNT), draw_tmep_buf[i - 1], 0xffff);
            altimeter_x = altimeter_x + (rect.width / REFRESH_CNT);
            altimeter_y = draw_tmep_buf[i - 1];
            /* ui_draw_line(dc, altimeter_x, altimeter_y, altimeter_x, altimeter_y - 15, 0xffff); */
            /* r_printf("ax:%d ay:%d ",altimeter_x,altimeter_y); */
        }
        break;
    case ON_CHANGE_RELEASE:
        max_x = 0;
        max_y = 0;
        min_x = 0;
        min_y = 0xffff;
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(ALTIMETER_COORDINATE_PIC)
.onchange = altimeter_coordinate_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


//=====================================================================================================

static int barometer_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}

static int barometer_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_U_MOVE:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(PAGE_63);//跳转海拔高度计界面
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PAGE_64)
.onchange = barometer_onchange,
 .onkey = NULL,
  .ontouch = barometer_ontouch,
};

int refresh_max_pressure(int arg)
{
    struct element *elm_coordinate_axis = ui_core_get_element_by_id(BAROMETER_COORDINATE_PIC);
    struct element *elm_max_dot = ui_core_get_element_by_id(MAX_PRESSURE_PIC);
    struct rect rect_coordinate_axis = {0};
    struct rect rect_max_dot = {0};
    int x_offset, y_offset;

    ui_core_get_element_abs_rect(elm_coordinate_axis, &rect_coordinate_axis);
    ui_core_get_element_abs_rect(elm_max_dot, &rect_max_dot);
    x_offset = rect_coordinate_axis.left + (rect_coordinate_axis.width / REFRESH_CNT) * (max_x + 1) - (rect_max_dot.left + rect_max_dot.width / 2);
    y_offset = draw_tmep_buf[max_x] - (rect_max_dot.top + rect_max_dot.height / 3);
    ui_move(elm_max_dot, x_offset, y_offset);
    return 0;
}

static int max_pressure_onchange(void *_ctrl, enum element_change_event event, void *arg)
{

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(refresh_max_pressure, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(MAX_PRESSURE_PIC)
.onchange = max_pressure_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int refresh_min_pressure(int arg)
{
    struct element *elm_coordinate_axis = ui_core_get_element_by_id(BAROMETER_COORDINATE_PIC);
    struct element *elm_min_dot = ui_core_get_element_by_id(MIN_PRESSURE_PIC);
    struct rect rect_coordinate_axis = {0};
    struct rect rect_min_dot = {0};
    int x_offset, y_offset;

    ui_core_get_element_abs_rect(elm_coordinate_axis, &rect_coordinate_axis);
    ui_core_get_element_abs_rect(elm_min_dot, &rect_min_dot);
    x_offset = rect_coordinate_axis.left + (rect_coordinate_axis.width / REFRESH_CNT) * (min_x + 1) - (rect_min_dot.left + rect_min_dot.width / 2);
    y_offset = draw_tmep_buf[min_x] - (rect_min_dot.top + rect_min_dot.height / 2);
    ui_move(elm_min_dot, x_offset, y_offset);
    return 0;
}

static int min_pressure_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct element *elm = (struct element *)_ctrl; */
    /* y_printf("et:%d el:%d ew:%d eh:%d",elm->css.top,elm->css.left,elm->css.width,elm->css.height); */

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(refresh_min_pressure, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(MIN_PRESSURE_PIC)
.onchange = min_pressure_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int refresh_barometer_pointer(int arg)
{
    struct element *elm_coordinate_axis = ui_core_get_element_by_id(BAROMETER_COORDINATE_PIC);
    struct element *elm_pressure_point = ui_core_get_element_by_id(BAROMETER_POINTER_PIC);
    struct rect rect_coordinate_axis = {0};
    struct rect rect_pressure_point = {0};
    int x_offset;

    ui_core_get_element_abs_rect(elm_coordinate_axis, &rect_coordinate_axis);
    ui_core_get_element_abs_rect(elm_pressure_point, &rect_pressure_point);
    x_offset = rect_coordinate_axis.left + (rect_coordinate_axis.width / REFRESH_CNT) * REFRESH_CNT - rect_pressure_point.left;//(rect_height_point.left + rect_height_point.width / 2);
    ui_move(elm_pressure_point, x_offset, 0);
    return 0;
}

static int barometer_point_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* struct element *elm = (struct element *)_ctrl; */
    /* y_printf("et:%d el:%d ew:%d eh:%d",elm->css.top,elm->css.left,elm->css.width,elm->css.height); */

    switch (event) {
    case ON_CHANGE_INIT:
        ui_set_call(refresh_barometer_pointer, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(BAROMETER_POINTER_PIC)
.onchange = barometer_point_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int show_barometer_max_min_pressure(int arg)
{
    struct unumber num;
    struct rect rect = {0};
    struct element *elm = ui_core_get_element_by_id(BAROMETER_COORDINATE_PIC);
    ui_core_get_element_abs_rect(elm, &rect);

    num.type = TYPE_NUM;
    num.numbs = 1;
    num.number[0] = max_y;
    ui_number_update_by_id(PRESSURE_MAX_NUM, &num);
    num.number[0] = min_y;
    ui_number_update_by_id(PRESSURE_MIN_NUM, &num);
    num.number[0] = rect.top + rect.height - draw_tmep_buf[REFRESH_CNT - 1];
    ui_number_update_by_id(CUR_PRESSURE_NUM, &num);
    return 0;
}

static int barometer_coordinate_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};
    u8 i;

    ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动

    switch (event) {
    case ON_CHANGE_INIT:
        first_point = rand32() % 48;
        log_info("first_point:%d", first_point);
        for (i = 0; i < REFRESH_CNT; i++) {
            draw_tmep_buf[i] = rect.top + rect.height - (rand32() % 48) - 5; //减5为了使数据超过坐标轴
            if ((rect.top + rect.height - draw_tmep_buf[i]) >= max_y) {
                max_y = rect.top + rect.height - draw_tmep_buf[i];
                max_x = i;
            }
            if ((rect.top + rect.height - draw_tmep_buf[i]) <= min_y) {
                min_y = rect.top + rect.height - draw_tmep_buf[i];
                min_x = i;
            }
        }
        log_info("min_x:%d min_y:%d max_x:%d max_y:%d", min_x, min_y, max_x, max_y);
        ui_set_call(show_barometer_max_min_pressure, 0);
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:
        /* y_printf("t:%d l:%d w:%d h:%d",rect.top,rect.left,rect.width,rect.height); */
        if (first_point > 25) {
            barometer_x = rect.left - dc->rect_orig.left;
            barometer_y = rect.top + rect.height / 2 - dc->rect_orig.top;
        } else {
            barometer_x = rect.left - dc->rect_orig.left;
            barometer_y = rect.top + rect.height - first_point - 5 - dc->rect_orig.top; //减5为了使数据超过坐标轴
        }
        /* ui_draw_line(dc, rect.left, rect.top, rect.left + rect.width, rect.top, 0xffff); */
        for (i = 1; i <= REFRESH_CNT; i++) {
            ui_draw_line(dc, barometer_x, barometer_y, barometer_x + (rect.width / REFRESH_CNT), draw_tmep_buf[i - 1], 0xffff);
            barometer_x = barometer_x + (rect.width / REFRESH_CNT);
            barometer_y = draw_tmep_buf[i - 1];
            /* ui_draw_line(dc, barometer_x, barometer_y, barometer_x, barometer_y - 15, 0xffff); */
            /* r_printf("ax:%d ay:%d ",barometer_x,barometer_y); */
        }
        break;
    case ON_CHANGE_RELEASE:
        max_x = 0;
        max_y = 0;
        min_x = 0;
        min_y = 0xffff;
        break;
    }
    return 0;
}
REGISTER_UI_EVENT_HANDLER(BAROMETER_COORDINATE_PIC)
.onchange = barometer_coordinate_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

