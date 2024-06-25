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
#include "music/music.h"
#include "ui_draw/ui_wave.h"
#include "ui_draw/ui_bar_chart.h"
#include "sport/sport_api.h"
#include "sport_data/watch_common.h"

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
#if TCFG_UI_ENABLE_HEART


#define STYLE_NAME  JL

static c_wave_ctrl *p_wave;

static int hrtimer = 0;

static const unsigned char s_wave_data1[] = {
    0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
    0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
    0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
    0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7E, 0x7E, 0x7F, 0x81, 0x81, 0x82, 0x82, 0x83,	0x84, 0x84, 0x84, 0x84, 0x83, 0x82, 0x82, 0x81,
    0x81, 0x7F, 0x7F, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7C, 0x7B, 0x7A, 0x79, 0x7A, 0x82,
    0x89, 0x91, 0x98, 0x9F, 0xA2, 0x9B, 0x94, 0x8C,	0x85, 0x7E, 0x78, 0x79, 0x7A, 0x7B, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,
    0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D,	0x7E, 0x7F, 0x7F, 0x81, 0x81, 0x82, 0x83, 0x84,	0x84, 0x85, 0x86, 0x87, 0x87, 0x87, 0x88, 0x88,
    0x89, 0x89, 0x89, 0x8A, 0x8A, 0x8A, 0x8A, 0x89,	0x89, 0x88, 0x88, 0x87, 0x87, 0x85, 0x85, 0x85,	0x83, 0x82, 0x81, 0x81, 0x7F, 0x7E, 0x7E, 0x7D,	0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D, 0x7D
};

/* static const unsigned char s_wave_data1[] = { */
/* 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, */
/* 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, */
/* 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, */
/* 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, */
/* 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, */

/* 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, */
/* 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, */
/* 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, */
/* 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, */
/* 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, */

/* 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, */
/* 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, */
/* 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, */
/* 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, */
/* }; */
static const unsigned char s_wave_data2[] = {
    70, 80, 80, 90, 90, 95, 94, 92, 91, 93, 96, 99, 110, 110, 110, 130, 110, 100, 90, 90, 90
};
static void refresh_draw_wave(struct rect *rect, void *_dc)
{

    int point_wave_len = sizeof(s_wave_data2) - 1;
    if (point_wave_len == 0) {
        return ;
    }
    int step = (int)(rect->width / point_wave_len);
    log_info("height=%d", rect->height - s_wave_data2[0]);
    int offset = 0;
    for (int i = 0; i < point_wave_len; i++) {
        ui_draw_line(_dc, offset, rect->height - s_wave_data2[i] + 100, offset + step, rect->height - s_wave_data2[i + 1] + 100, 0xffff);
        offset += step;
    }
}

#define TEXT_POINT 20121

static void refresh_test(struct rect *rect)
{
    log_info("%s", __func__);
    int i = 0;
    int k = 0;
    int frame_len = 0;

    frame_len = TEXT_POINT / (rect->width);

    int offset_len = TEXT_POINT / sizeof(s_wave_data1);

    p_wave->m_frame_len = frame_len;
    log_info("offset_len%d %d %d %d ", offset_len, sizeof(s_wave_data1), TEXT_POINT, frame_len);

    for (i = 0; i < TEXT_POINT; i += frame_len) {
        log_info("%d %d %d ", i, TEXT_POINT, frame_len);
        for (int j = 0; j < frame_len; j++) {
            p_wave->write_wave_data(p_wave->m_wave, s_wave_data1[((i + j) / offset_len) % sizeof(s_wave_data1)]);
        }
        if (k <= rect->width) {
            if (k > 255) {
                return;
            }
            p_wave->refresh_wave(p_wave, k++);
        }
    }


}


static void redraw_the_hr_line(void)
{
    ui_redraw(WAVE_PIC);

}


static int DRAW_DEMO_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};
    int w, h;

    switch (event) {
    case ON_CHANGE_INIT:

        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动

        /* rect.left = rect.left + 12; */
        /* rect.width = rect.width - 25; */
        /* rect.height = rect.height - 55; */
        /* rect.top  = rect.top + 10; */

        /* p_wave =  c_wave_ctrl_create(&rect); */
        /* p_wave->set_wave_speed(p_wave, 1); */
        /* p_wave->set_wave_color(p_wave, GL_RGB(255, 255, 0)); */
        /* p_wave->set_wave_in_out_rate(p_wave, 180, 100); */
        /* p_wave->set_max_min(p_wave, 256, 00); */

        /* refresh_test(&rect); */

        //sys_timeout_add(NULL,redraw_the_hr_line,100);
        break;
    case ON_CHANGE_SHOW:
        /*根据需要调用以下三个接口:ui_remove_backcolor,ui_remove_backimage,ui_remove_border*/
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        /* ui_remove_border(elm);//移除控件边界 */
        break;
    case ON_CHANGE_SHOW_POST:

        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        //ui_draw_line(dc, rect.left, rect.top, rect.left + rect.width, rect.top + rect.height, 0xffff);
        //draw_line_test(dc);
        /* wave_set_screen_rect(p_wave,&rect); */
        refresh_draw_wave(&rect, dc);
        //draw_smooth_vline_redraw(p_wave, dc, 0);
        break;
    case ON_CHANGE_RELEASE:
        c_wave_ctrl_release(&p_wave);
        break;
    default:
        break;
    }
    return false;
}

static int DRAW_DEMO_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    struct rect rect;
    static struct position mem_pos = {0};
    int x_offset;
    int y_offset;
    int css_left;
    int css_top;
    struct element_css *css;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        /* memcpy(&mem_pos, &e->pos, sizeof(struct position)); */
        /* return true; */

        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
#if 0
        x_offset = e->pos.x - mem_pos.x;
        y_offset = e->pos.y - mem_pos.y;
        memcpy(&mem_pos, &e->pos, sizeof(struct position));
        ui_core_get_element_abs_rect(elm->parent, &rect);
        css_left = x_offset * 10000 / rect.width;
        css_top = y_offset * 10000 / rect.height;
        css = ui_core_get_element_css(elm);
        css->left += css_left;
        css->top += css_top;
        ui_core_redraw(elm->parent);
#endif
        break;
    case ELM_EVENT_TOUCH_UP:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(BASEFORM_999)
.onchange = DRAW_DEMO_onchange,
 .onkey = NULL,
  .ontouch = DRAW_DEMO_ontouch,
};
#define ui_number_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? (struct ui_number *)elm: NULL; \
     })
#define ui_pic_for_id(id) \
		(struct ui_pic *)ui_core_get_element_by_id(id)

void heart_null_show(u32 pic_id, u32 num_id, u8 type)
{
    struct watch_heart_rate __heartrate_hd;//心率为0显示--
    watch_heart_rate_handle_get(&__heartrate_hd);
    int heart = __heartrate_hd.heart_rate_data_get(type, NULL);
    if (heart) {
        ui_core_hide(ui_pic_for_id(pic_id));
        /* ui_pic_set_hide_by_id(pic_id, 1); */
        ui_core_redraw(ui_pic_for_id(pic_id));
        ui_core_show(ui_number_for_id(num_id), false);
        ui_core_redraw(ui_number_for_id(num_id));

    } else {
        ui_core_hide(ui_number_for_id(num_id));
        ui_core_redraw(ui_number_for_id(num_id));
        ui_core_show(ui_pic_for_id(pic_id), false);
        /* ui_pic_set_hide_by_id(pic_id, 0); */
        ui_core_redraw(ui_pic_for_id(pic_id));
    }
}
static void ui_heart_rate_data(void *priv)
{
    if (!hrtimer) {
        return ;
    }
    struct element *elm = priv;
    /* log_info("%s", __func__); */
    ui_update_source_by_elm(elm, 1);
    heart_null_show(HRNULL_5, CUR_HEATRRATE_NUM, LAST_DATA);
    heart_null_show(HRNULL_6, MAX_HEARTRATE_NUM, MAX_DATA);
    heart_null_show(HRNULL_7, MIN_HEARTRATE_NUM, MIN_DATA);
    heart_null_show(HRNULL_8, CALM_HEARTRATE_NUM, RESTING_DATA);

}

static int LAYOUT_DEMO_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    /* log_info("%s", __func__); */
    struct element *elm = (struct element *)_ctrl;
    struct watch_algo __watch_algo;
    watch_algo_handle_get(&__watch_algo);
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_FIRST_SHOW:
        if (!hrtimer) {
#if TCFG_HR_SENSOR_ENABLE
            __watch_algo.algo_heart_rate_task();
#endif
            ui_auto_shut_down_disable();
            hrtimer = sys_timer_add(elm, ui_heart_rate_data, 1000);
        }
        ui_heart_rate_data(elm);
        break;
    case ON_CHANGE_SHOW_POST:
        break;
    case ON_CHANGE_RELEASE:

        if (hrtimer) {
#if TCFG_HR_SENSOR_ENABLE
            __watch_algo.algo_heart_rate_task_exit();
#endif
            sys_timer_del(hrtimer);
            hrtimer = 0;
            ui_auto_shut_down_enable();

        }

        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(HEART_RATE_MAIN)
.onchange = LAYOUT_DEMO_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};














#endif /* #if TCFG_UI_ENABLE_HEAT */
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

