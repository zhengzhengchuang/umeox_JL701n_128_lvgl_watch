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

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



#define STYLE_NAME  JL


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_STOPWATCH

extern u32 timer_get_ms(void);

static u16 updata_time_id = 0;
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
//LAQ 时间控件
static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    /* printf("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec); */
    dev_close(fd);
}

static void stopwatch_display_time(void *priv)
{
}

static int stopwatch_time_onchange(void *ctrl,  enum element_change_event e, void *arg)
{
    struct ui_time *time = (struct ui_time *)ctrl;
    switch (e) {
    case ON_CHANGE_INIT:
        if (!strcmp(time->source, "rtc")) {
            //stopwatch_display_time();
            struct sys_time s_time;
            struct utime u_time;
            get_sys_time(&s_time);
            u_time.hour = s_time.hour;
            u_time.min = s_time.min;
            ui_time_update(time, &u_time);
            /* if (updata_time_id == 0) { */
            /* log_info("LAQ creat the "); */
            /* updata_time_id = sys_timer_add(NULL, stopwatch_display_time, 250); */
            /* } */
        }
        break;
    case ON_CHANGE_RELEASE:
        log_info("LAQ stopwatch time release");
        /* if (updata_time_id) { */
        /* sys_timer_del(updata_time_id); */
        /* updata_time_id = 0; */
        /* } */
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(STPOWATCH_TIME)
.onchange = stopwatch_time_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
//LAQ 数字控件
#define STOPWATCH_COUNT_MAX			(1000*60*100 - 1)
static u8 stopwatch_num_buf[10] = {0};
static u16 stopwatch_count_start_id = 0;
static u16 stopwatch_reflash_id = 0;
static u32 stopwatch_background_prev_time = 0;
static volatile u32 stopwatch_count = 0;//65310;
static struct unumber *unum  = NULL;

#define ui_number_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? (struct ui_number *)elm: NULL; \
     })
void stopwatch_reflash(void *priv)
{
    u16 min = 0;
    u8 sec = 0;
    u8 tem = 0;
    u8 j = 0;
    struct unumber num;
    min = stopwatch_count / 6000;
    sec = (stopwatch_count % 6000) / 100;
    tem = (stopwatch_count % 6000) % 100;
    /* if (min < 60) { */
    /*     sprintf((char *)stopwatch_num_buf, "%2d%c%2d%c%2d", min, ':', sec, '.', tem); */
    /* } */
    num.type = TYPE_NUM;
    num.numbs = 1;
    num.number[0] = tem;
    /* unum->num_str = (char *)stopwatch_num_buf; */
    /* ui_number_update_by_id(DATA_W, &num); */
    ui_number_update(ui_core_get_element_by_id(DATA_W), &num);
    num.number[0] = sec;
    /* ui_number_update_by_id(DATA_S, &num); */
    ui_number_update(ui_core_get_element_by_id(DATA_S), &num);
    num.number[0] = min;
    /* ui_number_update_by_id(DATA_M, &num); */
    ui_number_update(ui_core_get_element_by_id(DATA_M), &num);

    struct sys_time s_time;
    struct utime u_time;
    get_sys_time(&s_time);
    u_time.hour = s_time.hour;
    u_time.min = s_time.min;

    struct ui_time *time = ui_core_get_element_by_id(STPOWATCH_TIME);
    if ((time->hour != u_time.hour) || (time->min != u_time.min)) {
        ui_time_update(&time->text.elm, &u_time);
        /* ui_time_update_by_id(STPOWATCH_TIME, &u_time); */
        ui_core_redraw(time->text.elm.parent);
    } else {
        struct element *elm = ui_core_get_element_by_id(DATA_W);
        ui_core_redraw(elm->parent);
    }
}
static int stopwatch_num_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_number *num = (struct ui_number *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT:
        /* if (!(strcmp(num->source, "none"))) { */
        log_info("LAQ test num ctrl");
        unum = (struct unumber *)malloc(sizeof(struct unumber));
        if (stopwatch_reflash_id == 0) {
            stopwatch_reflash_id = sys_timer_add(NULL, stopwatch_reflash, 90);
        }
        /* } */

        break;
    case ON_CHANGE_RELEASE:
        if (unum) {
            free(unum);
            unum = NULL;
        }
        if (stopwatch_reflash_id) {
            sys_timer_del(stopwatch_reflash_id);
            stopwatch_reflash_id = 0;
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(BASEFORM_363)
.onchange = stopwatch_num_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
//LAQ 按键控件

static void stopwatch_num_updata(void *priv)
{
    if (stopwatch_count < STOPWATCH_COUNT_MAX) {
        stopwatch_count++;
    } else {
        stopwatch_count = STOPWATCH_COUNT_MAX;
    }
}

static int stopwatch_butt_pp_ontouch(void *ctrl, struct element_touch_event *e)
{
    log_info("LAQ PP button test ！");
    struct button *button = (struct button *)ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        /* code */
        break;
    case ELM_EVENT_TOUCH_UP:
        log_info("button ->elm.id = %d", button->elm.id);
        if ((stopwatch_count_start_id == 0) && (button->elm.id == STOPWATCH_PP)) {
            log_info("LAQ start ");
            ui_pic_show_image_by_id(STOPWATCH_PIC_PP, 1);
            stopwatch_count_start_id = usr_timer_add(NULL, stopwatch_num_updata, 10, 1);

        } else if ((stopwatch_count_start_id != 0) && (button->elm.id == STOPWATCH_PP)) {
            log_info("LAQ puse ");
            ui_pic_show_image_by_id(STOPWATCH_PIC_PP, 0);
            usr_timer_del(stopwatch_count_start_id);
            stopwatch_count_start_id = 0;
        } else if ((button->elm.id == STOPWATCH_RESTART) && (stopwatch_count_start_id == 0)) {
            stopwatch_count = 0;
            log_info("LAQ restart ");
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(STOPWATCH_PP)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = stopwatch_butt_pp_ontouch,
};

REGISTER_UI_EVENT_HANDLER(STOPWATCH_RESTART)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = stopwatch_butt_pp_ontouch,
};

static int stopwatch_pic_pp_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;
    /* printf("pp evt:%d \n", e); */
    switch (e) {
    case ON_CHANGE_INIT:
        //printf("ON_CHANGE_INIT \n");
        if (stopwatch_count_start_id) {
            //y_printf("%s,%d \n", __func__, __LINE__);
            ui_pic_set_image_index(pic, 1);
        }
        break;
    case ON_CHANGE_RELEASE:
        //printf("ON_CHANGE_RELEASE \n");
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(STOPWATCH_PIC_PP)
.onchange = stopwatch_pic_pp_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*  laq PAGE onchange 函数   */
static int PAGE_stopwatch_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    // printf("window call = %s id =%x \n", __FUNCTION__, window->elm.id);
    log_info("laq PAGE laq onchange func");
    switch (e) {
    case ON_CHANGE_INIT:
        /* key_ui_takeover(1); */
        log_info("laq on change init ");
        /* ui_auto_shut_down_disable(); */
        if (stopwatch_background_prev_time) {
            u32 cur_time = timer_get_ms();
            u32 offset_time;
            if (cur_time >= stopwatch_background_prev_time) {
                offset_time = cur_time - stopwatch_background_prev_time;
            } else {
                offset_time = ((u32) - 1) - (stopwatch_background_prev_time - cur_time);
            }
            /* printf("time:%d,%d,%d \n", stopwatch_background_prev_time, cur_time, offset_time); */
            /* printf("cnt:%d \n", stopwatch_count); */
            stopwatch_count += (offset_time / 10);
            if (stopwatch_count > STOPWATCH_COUNT_MAX) {
                stopwatch_count = STOPWATCH_COUNT_MAX;

            }
            stopwatch_count_start_id = usr_timer_add(NULL, stopwatch_num_updata, 10, 1);
        }
        break;
    case ON_CHANGE_RELEASE:
        /* key_ui_takeover(0); */
        log_info("laq on change release ");
        /* ui_auto_shut_down_enable(); */
        /* stopwatch_count = 0; */
        if (stopwatch_count_start_id) { //run
            usr_timer_del(stopwatch_count_start_id);
            stopwatch_count_start_id = 0;
            stopwatch_background_prev_time = timer_get_ms();
        } else {//pause
            stopwatch_background_prev_time = 0;
        }
        break;
    default:
        return false;
    }
    return false;
}

/*  laq 注册 page   */
REGISTER_UI_EVENT_HANDLER(ID_WINDOW_STOPWATCH)
.onchange = PAGE_stopwatch_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

