#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "app_main.h"
#include "init.h"
#include "key_event_deal.h"
#include "weather/weather.h"
#include "res/font_ascii.h"


#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"




#if TCFG_UI_ENABLE_WEATHER

#ifndef abs
#define abs(x)  ((x)>0?(x):-(x))
#endif

#if 0
static u8 weather_type[][38] = {
    "晴",
    "少云",
    "晴间多云",
    "多云",
    "阴",
    "有风",
    "平静",
    "大风",
    "飓风",
    "风暴",
    "霾",
    "阵雨",
    "雷阵雨",
    "雷阵雨并伴有冰雹",
    "小雨",
    "中雨",
    "大雨",
    "暴雨",
    "大暴雨",
    "特大暴雨",
    "强阵雨",
    "强雷阵雨",
    "极端阵雨",
    "雨夹雪",
    "雪",
    "阵雪",
    "小雪",
    "中雪",
    "大雪",
    "暴雪",
    "浮尘",
    "扬沙",
    "沙尘暴",
    "强沙尘暴",
    "龙卷风",
    "雾",
    "热",
    "冷",
    "未知",
};
#endif

int update_weather_info_handler(const char *type, u32 arg);

static struct __WEATHER_INFO weather_info;
static u8 temperature_str[5];

struct __WEATHER_INFO *get_weather_info()
{
    return &weather_info;
}

void data_func_attr_weather_set(void *priv, u8 attr, u8 *data, u16 len)
{
    log_info("->%s\n", __FUNCTION__);
    /* g_log_info("============="); */
    /* put_buf(data, len); */
    /* g_log_info("============="); */
    u8 province_name_len;
    u8 city_name_len;
    u8 offset = 0;
    u32 timestamp;

    province_name_len = data[offset];
    /* log_info("province_name_len:%d", province_name_len); */
    offset++;
    if (weather_info.province) {
        free(weather_info.province);
        weather_info.province = 0;
    }
    weather_info.province_name_len = province_name_len + 1;
    weather_info.province = zalloc(province_name_len + 1);
    if (!weather_info.province) {
        log_info("weather_info.province malloc fail");
        return;
    }
    memcpy(weather_info.province, data + offset, province_name_len);
    log_info("province_name_len : %d", weather_info.province_name_len);
    log_info("province:%s\n", weather_info.province);
    put_buf(weather_info.province, weather_info.province_name_len);

    offset += province_name_len;
    city_name_len = data[offset];
    /* log_info("city_name_len:%d", city_name_len); */
    offset++;
    if (weather_info.city) {
        free(weather_info.city);
        weather_info.city = 0;
    }

    weather_info.city_name_len = city_name_len + 1;
    weather_info.city = zalloc(city_name_len + 1);
    if (!weather_info.city) {
        log_info("weather_info.city malloc fail");
        return;
    }
    memcpy(weather_info.city, data + offset, city_name_len);
    /* log_info("city:%s", weather_info.city); */

    offset += city_name_len;
    weather_info.weather = data[offset];
    /* log_info("weather_info.weather:%d", weather_info.weather); */

    offset++;
    weather_info.temperature = data[offset];
    /* log_info("weather_info.temperature:%d", weather_info.temperature); */

    offset++;
    weather_info.humidity = data[offset];
    /* log_info("weather_info.humidity:%d", weather_info.humidity); */

    offset++;
    weather_info.wind_direction = data[offset];
    /* log_info("weather_info.wind_direction:%d", weather_info.wind_direction); */

    offset++;
    weather_info.wind_power = data[offset];
    /* log_info("weather_info.wind_power:%d", weather_info.wind_power); */

    offset++;
    weather_info.update_time = (*(data + offset) << 24) | (*(data + offset + 1) << 16) | (*(data + offset + 2) << 8) | (*(data + offset + 3) << 0);
    timestamp = weather_info.update_time;
    /* log_info("%d年%d月%d日%d时%d分%d秒", ((timestamp >> 26) & 0x3f) + 2010, (timestamp >> 22) & 0xf, (timestamp >> 17) & 0x1f, (timestamp >> 12) & 0x1f, (timestamp >> 6) & 0x3f, timestamp & 0x3f); */

    UI_MSG_POST("weather_info:event=%4", 0);
}


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_SPI_LCD_ENABLE

#define STYLE_NAME  JL

#define ui_text_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_text, elm): NULL; \
	 })

#define WEATHER_PAGE        PAGE_66


static const struct uimsg_handl ui_weather_handler[] = {
    { "weather_info",       update_weather_info_handler     },
    { NULL, NULL},      /* 必须以此结尾！ */
};

static void num2string(int num, u8 *buf)
{
    u8 len = 0;
    if (num < 0) {
        buf[len++] = '-';
    }
    num = abs(num);
    if ((num >= 100) && (num <= 999)) {
        buf[len++] = '0' + num / 100;
        buf[len++] = '0' + (num / 10) % 10;
    } else if (num < 10) {

    } else {
        buf[len++] = '0' + num / 10;
    }
    buf[len++] = '0' + num % 10;
    /* buf[len++] = ' '; */
    buf[len++] = '\0';
    /* g_log_info("%s",buf); */
}

int update_weather_info_handler(const char *type, u32 arg)
{
    struct ui_text *text;
    u32 timestamp;
    u8 index_buf;
    static u16 store_buf[2];// ALIGNED(4);
    struct utime time_r;
    struct unumber num;
    int width;
    int height;
    int total_width = 0;
    struct element_css *css;
    struct rect rect;
    struct rect parent_rect;
    char *str;

    css = ui_core_get_element_css(ui_core_get_element_by_id(PIC_WAIT_WEATHER_UPDATE));
    if (weather_info.update_time) {
        css->invisible = true;
    } else {
        css->invisible = false;
    }

    //province
    text = ui_text_for_id(TEXT_PROVINCE);
    ui_text_set_text_attrs(text, weather_info.province, strlen(weather_info.province), FONT_ENCODE_UTF8, 0, FONT_DEFAULT);

    //city
    text = ui_text_for_id(TEXT_CITY);
    ui_text_set_text_attrs(text, weather_info.city, strlen(weather_info.city), FONT_ENCODE_UTF8, 0, FONT_DEFAULT);

    //weather
    text = ui_text_for_id(TEXT_WEATHER);
    index_buf = weather_info.weather;
    ui_text_set_combine_index(text, store_buf, &index_buf, 1);

    //temperature
    num2string(weather_info.temperature, temperature_str);
    num.type = TYPE_STRING;
    num.num_str = temperature_str;
    ui_number_update_by_id(NUM_TEMPERATURE, &num);
    str = temperature_str;
    while (*str) {
        font_ascii_get_width_and_height(*str, &height, &width);
        total_width += width;
        str++;
    }
    ui_core_get_element_abs_rect(ui_core_get_element_by_id(NUM_TEMPERATURE), &rect);
    ui_core_get_element_abs_rect(ui_core_get_element_by_id(PIC_CENTIGRADE)->parent, &parent_rect);
    css = ui_core_get_element_css(ui_core_get_element_by_id(PIC_CENTIGRADE));
    css->left = (rect.left + total_width) * 10000 / parent_rect.width;

    //humidity
    num.type = TYPE_NUM;
    num.numbs = 1;
    num.number[0] = weather_info.humidity;
    ui_number_update_by_id(NUM_HUMIDITY, &num);

    //wind_direction
    ui_text_show_index_by_id(TEXT_WIND_DIRECTION, weather_info.wind_direction);

    //wind_power
    num.type = TYPE_NUM;
    num.numbs = 1;
    num.number[0] = weather_info.wind_power;
    ui_number_update_by_id(NUM_WIND_POWER, &num);

    //update_time
    timestamp = weather_info.update_time;
    time_r.year = ((timestamp >> 26) & 0x3f) + 2010;
    time_r.month = (timestamp >> 22) & 0xf;
    time_r.day = (timestamp >> 17) & 0x1f;
    time_r.hour = (timestamp >> 12) & 0x1f;
    time_r.min = (timestamp >> 6) & 0x3f;
    time_r.sec = timestamp & 0x3f;
    ui_time_update_by_id(TIMER_UPDATE_TIME, &time_r);

    ui_core_redraw(ui_core_get_element_by_id(LAYOUT_WEATHER));

    return 0;
}


static int text_weather_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    u8 index_buf;
    static u16 store_buf[2];// ALIGNED(4);

    switch (event) {
    case ON_CHANGE_INIT:
        switch (text->elm.id) {
        case TEXT_PROVINCE:
            if (weather_info.province == 0) {
                ui_text_set_text_attrs(text, "---", strlen("---"), FONT_ENCODE_UTF8, 0, FONT_DEFAULT);
            } else {
                ui_text_set_text_attrs(text, weather_info.province, strlen(weather_info.province), FONT_ENCODE_UTF8, 0, FONT_DEFAULT);
            }
            break;
        case TEXT_CITY:
            if (weather_info.city == 0) {
                ui_text_set_text_attrs(text, "---", strlen("---"), FONT_ENCODE_UTF8, 0, FONT_DEFAULT);
            } else {
                ui_text_set_text_attrs(text, weather_info.city, strlen(weather_info.city), FONT_ENCODE_UTF8, 0, FONT_DEFAULT);
            }
            break;
        case TEXT_WEATHER:
            index_buf = weather_info.weather;
            ui_text_set_combine_index(text, store_buf, &index_buf, 1);
            break;
        case TEXT_WIND_DIRECTION:
            ui_text_set_index(text, weather_info.wind_direction);
            break;
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(TEXT_PROVINCE)
.onchange = text_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(TEXT_CITY)
.onchange = text_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(TEXT_WEATHER)
.onchange = text_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(TEXT_WIND_DIRECTION)
.onchange = text_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int num_weather_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_number *number = (struct ui_number *)_ctrl;
    struct unumber num;

    switch (event) {
    case ON_CHANGE_INIT:
        switch (number->text.elm.id) {
        case NUM_TEMPERATURE:
            num.type = TYPE_STRING;
            num2string(weather_info.temperature, temperature_str);
            num.num_str = temperature_str;
            ui_number_update(number, &num);
            break;
        case NUM_HUMIDITY:
            num.type = TYPE_NUM;
            num.numbs = 1;
            num.number[0] = weather_info.humidity;
            ui_number_update(number, &num);
            break;
        case NUM_WIND_POWER:
            num.type = TYPE_NUM;
            num.numbs = 1;
            num.number[0] = weather_info.wind_power;
            ui_number_update(number, &num);
            break;
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(NUM_TEMPERATURE)
.onchange = num_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(NUM_HUMIDITY)
.onchange = num_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
REGISTER_UI_EVENT_HANDLER(NUM_WIND_POWER)
.onchange = num_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int time_weather_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_time *time = (struct ui_time *)_ctrl;
    struct utime time_r;
    u32 timestamp;

    switch (event) {
    case ON_CHANGE_INIT:
        timestamp = weather_info.update_time;
        time_r.year = ((timestamp >> 26) & 0x3f) + 2010;
        time_r.month = (timestamp >> 22) & 0xf;
        time_r.day = (timestamp >> 17) & 0x1f;
        time_r.hour = (timestamp >> 12) & 0x1f;
        time_r.min = (timestamp >> 6) & 0x3f;
        time_r.sec = timestamp & 0x3f;
        ui_time_update(time, &time_r);
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(TIMER_UPDATE_TIME)
.onchange = time_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int pic_weather_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    struct element_css *css;
    struct rect rect;
    struct rect parent_rect;
    int total_width = 0;
    int width;
    int height;
    char *str;

    switch (event) {
    case ON_CHANGE_INIT:
        num2string(weather_info.temperature, temperature_str);
        str = temperature_str;
        while (*str) {
            font_ascii_get_width_and_height(*str, &height, &width);
            total_width += width;
            str++;
        }
        ui_core_get_element_abs_rect(ui_core_get_element_by_id(NUM_TEMPERATURE), &rect);
        ui_core_get_element_abs_rect(pic->elm.parent, &parent_rect);
        css = ui_core_get_element_css(&pic->elm);
        css->left = (rect.left + total_width) * 10000 / parent_rect.width;
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(PIC_CENTIGRADE)
.onchange = pic_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int pic_wait_weather_update_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    struct element_css *css;

    switch (event) {
    case ON_CHANGE_INIT:
        css = ui_core_get_element_css(&pic->elm);
        if (weather_info.update_time) {
            css->invisible = true;
        } else {
            css->invisible = false;
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(PIC_WAIT_WEATHER_UPDATE)
.onchange = pic_wait_weather_update_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#define LINEMETER_DEMO   	0
#define ARBIRECT_DEMO		0
#define NEWLINE_DEMO		0
#define POLY_DEMO			0


#if (LINEMETER_DEMO || ARBIRECT_DEMO || NEWLINE_DEMO || POLY_DEMO)

#include "ui_draw/ui_poly.h"
#include "ui_draw/ui_arbirect.h"
#include "ui_draw/ui_linemeter.h"
#include "ui_draw/ui_line.h"
#include "ui_draw/ui_figure.h"
#include "asm/math_fast_function.h"


/* static u32 n = 3; */
/* static void poly_timer(void *priv) */
/* { */
/* struct layout *layout = (struct layout *)priv; */
/* static u32 count = 0; */

/* if ((count++) % 5 == 0) { */
/* n++; */
/* } */
/* if ((n < 3) || (n > 32)) { */
/* n = 3; */
/* } */

/* ui_core_redraw(&layout->elm); */
/* } */

static u32 cnt = 0;
static void lmeter_timer(void *priv)
{
    struct layout *layout = (struct layout *)priv;

    cnt++;
    cnt = (cnt > 60) ? 0 : cnt;

    ui_core_redraw(&layout->elm);
}


static int layout_lmeter_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct layout *layout = (struct layout *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    static u32 timer_id = 0;
    /* static u32 poly_timer_id = 0; */

    switch (event) {
    case ON_CHANGE_INIT:
#if LINEMETER_DEMO
        timer_id = sys_timer_add(layout, lmeter_timer, 500);
#endif

        /* #if POLY_DEMO */
        /* timer_id = sys_timer_add(layout, poly_timer, 500); */
        /* #endif */

        break;
    case ON_CHANGE_RELEASE:

        if (timer_id) {
            sys_timer_del(timer_id);
            timer_id = 0;
        }

        /* if (poly_timer_id) { */
        /* sys_timer_del(poly_timer_id); */
        /* poly_timer_id = 0; */
        /* } */

        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_SHOW_POST:

        struct rect rect;

        ui_core_get_element_abs_rect(&layout->elm, &rect);
        /* printf("rect (%d, %d, %d, %d)\n\n\n", rect.left, rect.left + rect.width - 1, rect.top, rect.top + rect.height - 1); */
        if (rect.left != 0) {//这里只是想在滑动时不增加进度条数目,减少滑动时的压力,具体逻辑用户自己控制
            break;
        }

        struct rect *coords_rect;
        ui_style_list_t *style;

#if POLY_DEMO
        poly_info_t poly_info;
        static const ui_point_t pntarray[] = {{203, 290}, {33, 244}, {24, 68}, {188, 5}, {299, 142}}; //这里必须静态或者全局
        /* static const ui_point_t pntarray[] = {{0, 20}, {20, 0}, {120, 80}, {100, 100}}; //这里必须静态或者全局 */
        /* static const ui_point_t pntarray[] = {{20, 20}, {23, 17}, {123, 117}, {120, 120}}; //这里必须静态或者全局 */
        /* static const ui_point_t pntarray[] = {{0, 200}, {150, 0}, {300, 200}}; //这里必须静态或者全局 */

        coords_rect = &poly_info.coords_rect;

        /******* 头文件有注释 ********/
        /**
         * @brief
         * 1.点的顺序必须是顺时针,并且最少有三个点
         * 2.只能是凸多边形，如果要画凹多边形，则需要用户分割成多个凸多边来画
         */

        /* static ui_point_t pntarray[32]; */
        /* int i, r = 150; */
        /* for (i = 0; i < n; i++) { */
        /* int angle = i * 360 / n + ((jiffies % 36000) / 2); */
        /* pntarray[i].x = 150 + r * cos_float(angle / 180.f);  */
        /* pntarray[i].y = 150 + r * sin_float(angle / 180.f);  */
        /* } */
        /* poly_info.cnt =	n;//点的个数, 至少要有三个点 */

        poly_info.cnt =	sizeof(pntarray) / sizeof(pntarray[0]);//点的个数, 至少要有三个点
        poly_info.pntarray = &pntarray;//每个点的坐标是相对coords_rect坐标来说的,该数组必须是全局或者静态
        poly_info.bg_color = UI_COLOR_LB;
        poly_info.opa = 0xff;

        coords_rect->left = 100;//相对背景的绝对坐标
        coords_rect->top = 100;//相对背景的绝对坐标
        coords_rect->width = 454 - 100;//该控件范围
        coords_rect->height = 454 - 100;//该控件范围

        /*背景不是屏幕，一个页面只有一个背景，是最底的那一层，*/
        /*滑动时当前页面的背景在移动，但背景内的控件,相对背景是没有动的*/
        ui_draw(dc,
                NULL,
                coords_rect->left,//这里要写相对背景的绝对坐标
                coords_rect->top,//这里要写相对背景的绝对坐标
                coords_rect->width,
                coords_rect->height,
                ui_draw_default,
                (void *)&poly_info,
                sizeof(poly_info_t),
                DRAW_POLY);

#endif

#if NEWLINE_DEMO
        newline_info_t line_info;
        static const ui_point_t point_a[] = {{100, 100}, {200, 200}}; //这里必须静态或者全局
        /* static const ui_point_t point_a[] = {{100,100}, {180, 220}, {250, 50}};//这里必须静态或者全局 */

        coords_rect = &line_info.coords_rect;
        style = &line_info.style;

        /******* 头文件有注释 ********/
        line_info.point_array = &point_a;//在画线框内的点坐标数组，每个点的坐标是相对coords_rect坐标来说的,该数组必须是全局或者静态
        line_info.point_num = sizeof(point_a) / sizeof(point_a[0]);//点的个数, 至少要有两个点
        style->line_width = 10;//画线时最小值可以为1
        /* style->line_color = UI_COLOR_LB; */
        style->line_color = UI_COLOR_ZI;
        coords_rect->left = 0;//相对背景的绝对坐标
        coords_rect->top = 0;//相对背景的绝对坐标
        coords_rect->width = 454;//该控件范围
        coords_rect->height = 454;//该控件范围

        /* printf("point_num %d\n", line_info.point_num); */
        /*背景不是屏幕，一个页面只有一个背景，是最底的那一层，*/
        /*滑动时当前页面的背景在移动，但背景内的控件,相对背景是没有动的*/
        ui_draw(dc,
                NULL,
                coords_rect->left,//这里要写相对背景的绝对坐标
                coords_rect->top,//这里要写相对背景的绝对坐标
                coords_rect->width,
                coords_rect->height,
                ui_draw_default,
                (void *)&line_info,
                sizeof(newline_info_t),
                DRAW_NEWLINE);
#endif
#if ARBIRECT_DEMO
        arbirect_info_t rinfo;
        static struct rect limit_rect;//这里必须静态或者全局
        u8 percent = 60;

        coords_rect = &rinfo.coords_rect;

        /******* 头文件有注释 ********/
        coords_rect->left = 150;//相对背景的绝对坐标
        coords_rect->top = 150;//相对背景的绝对坐标
        coords_rect->width = 200;//该控件范围
        coords_rect->height = 100;//该控件范围

        /* memcpy(&limit_rect, coords_rect, sizeof(struct rect)); */
        /* limit_rect.width = coords_rect->width * percent / 100; */
        /* limit_rect.height = coords_rect->height * percent / 100; */

        rinfo.radius = UI_RADIUS_CIRCLE;//如果为0，则是普通矩形，否则圆角矩形
        /* rinfo.radius = 30; */
        rinfo.bg_color = UI_COLOR_YELLOW;//填充矩形的颜色
        rinfo.bg_limit_rect = NULL;//(必须静态或者全局)填充矩形限制画的区域,可用于局部显示背景，一般用做矩形百分比显示,如果为空则表示不限制
        /* rinfo.bg_limit_rect = &limit_rect; */
        rinfo.border_color = UI_COLOR_LZ;//边框颜色
        rinfo.border_width = 10;//边框宽度,如果border_en使能，则该值不能小于2
        rinfo.bg_en = 1;//填充矩形使能
        rinfo.border_en = 1;//边框矩形使能

        /*背景不是屏幕，一个页面只有一个背景，是最底的那一层，*/
        /*滑动时当前页面的背景在移动，但背景内的控件,相对背景是没有动的*/
        ui_draw(dc,
                NULL,
                coords_rect->left,//这里要写相对背景的绝对坐标
                coords_rect->top,//这里要写相对背景的绝对坐标
                coords_rect->width,
                coords_rect->height,
                ui_draw_default,
                (void *)&rinfo,
                sizeof(arbirect_info_t),
                DRAW_AIRECT);

#endif



#if LINEMETER_DEMO
        linemeter_info_t info;

        coords_rect = &info.coords_rect;
        style = &info.style;
        /******* 头文件有注释 ********/

        info.min_value = 0;
        info.max_value = 60;
        info.cur_value = cnt;
        info.angle_ofs = 0;
        info.line_cnt = 60;//根数不能太多，不然刷新不过来!!! 最大80, 默认18
        info.scale_angle = 360;
        /* coords_rect->left = 70;//相对背景的绝对坐标 */
        /* coords_rect->top = 70;//相对背景的绝对坐标 */
        /* coords_rect->width = 240;//该控件范围 */
        /* coords_rect->height = 240;//该控件范围 */
        coords_rect->left = 0;//相对背景的绝对坐标
        coords_rect->top = 0;//相对背景的绝对坐标
        coords_rect->width = 454;//该控件范围
        coords_rect->height = 454;//该控件范围
        style->scale_width = 20;
        style->line_width = 8;
        style->line_color = UI_COLOR_LB;
        style->scale_grad_color = UI_COLOR_ZI;
        style->scale_end_color = UI_COLOR_YELLOW;


        /*背景不是屏幕，一个页面只有一个背景，是最底的那一层，*/
        /*滑动时当前页面的背景在移动，但背景内的控件,相对背景是没有动的*/
        ui_draw(dc,
                NULL,
                coords_rect->left,//这里要写相对背景的绝对坐标
                coords_rect->top,//这里要写相对背景的绝对坐标
                coords_rect->width,
                coords_rect->height,
                ui_draw_default,
                (void *)&info,
                sizeof(linemeter_info_t),
                DRAW_LINEMETER);
#endif




        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(LMETER_TEST)
.onchange = layout_lmeter_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif //#if (LINEMETER_DEMO || ARBIRECT_DEMO || NEWLINE_DEMO)




static int layout_weather_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        ui_register_msg_handler(WEATHER_PAGE, ui_weather_handler);//注册消息交互的回调
        break;
    case ON_CHANGE_RELEASE:
        break;
    }

    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(LAYOUT_WEATHER)
.onchange = layout_weather_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

#endif /* #if TCFG_UI_ENABLE_WEATHER */


