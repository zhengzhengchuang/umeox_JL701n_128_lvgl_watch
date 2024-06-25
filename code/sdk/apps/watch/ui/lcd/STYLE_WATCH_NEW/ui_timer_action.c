#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "tone_player.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_TIMER_ACTION

#define STYLE_NAME  JL


#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


/* REGISTER_UI_STYLE(STYLE_NAME) */
extern void ui_send_event(u16 event, u32 val);

extern u32 timer_get_ms(void);

#define CALCULAGRAPH_SEL        PAGE_34//
/* #define CALCULAGRAPH_FRONT      PAGE_35// */
#define CALCULAGRAPH_ADD        PAGE_36//
#define CALCULAGRAPH_DONE      	PAGE_35//

struct ui_timer {
    u8 countdown_hour;          //用来显示小时
    u8 countdown_min;
    u8 countdown_sec ;
    u8 countdown_flag;
    u8 countdown_customize_start;     //自定义设置好了时间，直接开始倒计时
    u16 contdown_show_num_id;         //倒计时时间刷新定时器id
    u16 countdown_count_num_id;       //计数定时器刷新id
    u16 countdown_time_id;            //页面上面刷新显示时间的定时器id
    u8 countdown_dis_buf[20];         //储存显示数据的buf
};

#define     MALLOC_SIZE    (sizeof(struct ui_timer))//64
struct ui_timer *__timer_countdown = NULL;
static u32 timer_countdown_num = 0;          // 倒计时计数
static u32 timer_background_prev_time = 0;
static u16 timer_background_cnt = 0;
static u16 timer_done_id = 0;
static u8  timer_done_cnt = 0;

#define ui_number_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? (struct ui_number *)elm: NULL; \
     })

/***************************************************************************************
                                页面处理部分
****************************************************************************************/
static int calculagraph_ui_switch_init(int id)
{
    if (timer_countdown_num) {
        ui_hide(LAYOUT_TIMER_SEL);
        ui_show(LAYOUT_TIMER_RUN);
    } else {
        ui_hide(LAYOUT_TIMER_RUN);
        ui_show(LAYOUT_TIMER_SEL);
    }
    return 0;
}

static int TIMER_CALCULAGRAPH_page_onchange(void *ctrl,  enum element_change_event e, void *arg)
{
    struct window *win = (struct window *)ctrl;
    switch (e) {
    case ON_CHANGE_INIT:
        ui_set_call(calculagraph_ui_switch_init, 0);
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(CALCULAGRAPH_SEL)
.ontouch = NULL,
 .onkey = NULL,
  .onchange = TIMER_CALCULAGRAPH_page_onchange,
};

static int TIMER_CALCULAGRAPH_onchange(void *ctrl,  enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        if (__timer_countdown == NULL) {
            log_info("malloc addr to __timer_countdown and size is %d", sizeof(struct ui_timer));
            __timer_countdown = (struct ui_timer *) malloc(MALLOC_SIZE);
        }
        memset(__timer_countdown, 0, MALLOC_SIZE);
        /* ui_auto_shut_down_disable(); */
        break;
    case ON_CHANGE_RELEASE:
        /* ui_auto_shut_down_enable(); */
        if (__timer_countdown->countdown_count_num_id) {
            sys_timer_del(__timer_countdown->countdown_count_num_id);
            __timer_countdown->countdown_count_num_id = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(LAYOUT_TIMER_SEL)
.ontouch = NULL,
 .onkey = NULL,
  .onchange = TIMER_CALCULAGRAPH_onchange,
};

static void child_id_to_change(struct element *elm)
{
    static u8 last_return_vaule = 0;
    switch (elm->id) {
    case TIMER_PIC_1_MIN_NUM:
    case TIMER_PIC_1_MIN_TEXT:
    case TIMER_PIC_1_MIN:
        timer_countdown_num = 60;
        break;
    case TIMER_PIC_3_MIN_NUM:
    case TIMER_PIC_3_MIN_TEXT:
    case TIMER_PIC_3_MIN:
        timer_countdown_num = 180;
        break;
    case TIMER_PIC_5_MIN_NUM:
    case TIMER_PIC_5_MIN_TEXT:
    case TIMER_PIC_5_MIN:
        timer_countdown_num = 300;
        break;
    case TIMER_PIC_10_MIN_NUM:
    case TIMER_PIC_10_MIN_TEXT:
    case TIMER_PIC_10_MIN:
        timer_countdown_num = 600;
        break;
    case TIMER_PIC_15_MIN_NUM:
    case TIMER_PIC_15_MIN_TEXT:
    case TIMER_PIC_15_MIN:
        timer_countdown_num = 900;
        break;
    case TIMER_PIC_30_MIN_NUM:
    case TIMER_PIC_30_MIN_TEXT:
    case TIMER_PIC_30_MIN:
        timer_countdown_num = 1800;
        break;
    case TIMER_PIC_1_HOUR_NUM:
    case TIMER_PIC_1_HOUR_TEXT:
    case TIMER_PIC_1_HOUR:
        timer_countdown_num = 3600;
        break;
    case TIMER_PIC_2_HOUR_NUM:
    case TIMER_PIC_2_HOUR_TEXT:
    case TIMER_PIC_2_HOUR:
        timer_countdown_num = 7200;
        break;
    case TIMER_ADD_BUTTN_TEXT:
        ui_send_event(KEY_CHANGE_PAGE, BIT(31) | CALCULAGRAPH_ADD);
        return ;
    default:
        return ;
    }
    timer_background_prev_time = 0;
    ui_hide(LAYOUT_TIMER_SEL);
    ui_show(LAYOUT_TIMER_RUN);
}

static int vlist_child_ontouch(void *ctrl, struct  element_touch_event *e)
{
    struct element *grid = (struct element *) ctrl;
    static u8 change_page_action = 0;
    log_info("LAQtext the id is 0x%x", grid->id);
    log_info("LAQ event is %d", e->event);
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (change_page_action == 1) {
            child_id_to_change(grid);
            change_page_action = 0;
        }
        break;
    case ELM_EVENT_TOUCH_MOVE:
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
    case ELM_EVENT_TOUCH_U_MOVE:
    case ELM_EVENT_TOUCH_D_MOVE:
    case ELM_EVENT_TOUCH_HOLD:
        change_page_action = 2;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        change_page_action = 1;
        break;
    default:
        break;
    }
    return false;
}

static int VLIST_onchange(void *ctrl, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        ui_set_default_handler(grid, vlist_child_ontouch, NULL, NULL);
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return false;
}

static int VLIST_ontouch(void *ctrl, struct  element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    struct rect *rect;
    int i;
    int sel_item = ui_grid_cur_item(grid);
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            break;
        }
        struct rect r;
        struct element *p, *n;
        struct element *elm = &grid->item[sel_item].elm;
        list_for_each_child_element_reverse(p, n, elm) {
            if (!p->css.invisible) {
                ui_core_get_element_abs_rect(p, &r);
                if (in_rect(&r, &e->pos)) {
                    p->handler->ontouch(p, e);
                    break;
                }
            }
        }
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(VLIST)
.ontouch = VLIST_ontouch,
 .onkey = NULL,
  .onchange = VLIST_onchange,
};

/***************************************************************************************
                                LAQ PAGE21 页面处理部分
****************************************************************************************/
static int __timer_countdown_tone_play(int param)
{
    if (!__timer_countdown || (UI_GET_WINDOW_ID() != CALCULAGRAPH_DONE)) {
        return 0;
    }
    tone_play_by_path(TONE_RING, 0);
    return 0;
}
static void __timer_countdown_tone_run(void *priv)
{
    if (timer_done_cnt) {
        timer_done_cnt--;
        if (timer_done_cnt == 0) {
            UI_WINDOW_PREEMPTION_POP(CALCULAGRAPH_DONE);
            sys_timer_del(timer_done_id);
            timer_done_id = 0;
            return ;
        }
    }
    int argv[3];
    argv[0] = (int)__timer_countdown_tone_play;
    argv[1] = 1;
    argv[2] = 0;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, argv);
}
static int timer_countdown_ui_entry(int page)
{
    timer_done_cnt = 10;
    if (timer_done_id == 0) {
        timer_done_id = sys_timer_add(NULL, __timer_countdown_tone_run, 1000);
    }
    return false;
}

static int timer_countdown_ui_exit(int page)
{
    if (timer_done_id) {
        sys_timer_del(timer_done_id);
        timer_done_id = 0;
    }
    return false;
}

static void timer_countdown_func(void *priv)
{
    if (!__timer_countdown || !__timer_countdown->countdown_count_num_id) {
        return ;
    }
    log_info("timer_countdown_num = %d", timer_countdown_num);
    if (timer_countdown_num > 0) {
        timer_countdown_num--;
    } else if (timer_countdown_num <= 0) {
        timer_countdown_num = 0;
        UI_WINDOW_PREEMPTION_POSH(CALCULAGRAPH_DONE, timer_countdown_ui_entry, timer_countdown_ui_exit, UI_WINDOW_PREEMPTION_TYPE_TIMER_COUNTDOWN);
    }
}

static void timer_background_func(void *priv)
{
    u16 cnt = (u16)priv;
    /* printf("timeout :%d, %d, %d ", cnt, timer_background_cnt, timer_countdown_num); */
    if (cnt != timer_background_cnt) {
        return ;
    }
    /* printf("calculagraph done \n"); */
    timer_countdown_num = 0;
    UI_WINDOW_PREEMPTION_POSH(CALCULAGRAPH_DONE, timer_countdown_ui_entry, timer_countdown_ui_exit, UI_WINDOW_PREEMPTION_TYPE_TIMER_COUNTDOWN);
}

static void timer_countdown_display_string(void *priv);

static int TIMER_CALCULAGRAPH_FRONT_onchange(void *ctrl,  enum element_change_event e, void *arg)
{
    struct unumber countdown;
    switch (e) {
    case ON_CHANGE_INIT:
        timer_background_cnt++;
        if (timer_countdown_num && timer_background_prev_time) {
            u32 cur_time = timer_get_ms();
            u32 offset_time;
            if (cur_time >= timer_background_prev_time) {
                offset_time = cur_time - timer_background_prev_time;
            } else {
                offset_time = ((u32) - 1) - (timer_background_prev_time - cur_time);
            }
            /* printf("time:%d,%d,%d \n", timer_background_prev_time, cur_time, offset_time); */
            /* printf("cnt:%d \n", timer_countdown_num); */
            offset_time = offset_time / 1000;
            if (timer_countdown_num > offset_time) {
                timer_countdown_num -= offset_time;
                __timer_countdown->countdown_customize_start = 1;
            } else {
                timer_countdown_num = 0;
                timer_background_prev_time = 0;
            }
        }
        if (timer_countdown_num >= 3600) {
            __timer_countdown->countdown_hour = timer_countdown_num / 3600;
            __timer_countdown->countdown_min = (timer_countdown_num % 3600) / 60;
            __timer_countdown->countdown_sec = ((timer_countdown_num % 3600) % 60);
            countdown.type = TYPE_NUM;
            countdown.numbs = 1;
            countdown.number[0] = __timer_countdown->countdown_sec;
            ui_number_update(ui_number_for_id(BIG_S), &countdown);
            countdown.number[0] = __timer_countdown->countdown_min;
            ui_number_update(ui_number_for_id(BIG_M), &countdown);
            countdown.number[0] = __timer_countdown->countdown_hour;
            ui_number_update(ui_number_for_id(BIG_H), &countdown);
        } else {
            struct layout *layout = layout_for_id(BIG_ONE);  //默认隐藏
            layout->elm.css.invisible = 1;
            __timer_countdown->countdown_min = timer_countdown_num / 60;
            __timer_countdown->countdown_sec = timer_countdown_num % 60;
            countdown.type = TYPE_NUM;
            countdown.numbs = 1;
            countdown.number[0] = __timer_countdown->countdown_sec;
            ui_number_update(ui_number_for_id(SMALL_S), &countdown);
            countdown.number[0] = __timer_countdown->countdown_min;
            ui_number_update(ui_number_for_id(SMALL_M), &countdown);
        }
        if (__timer_countdown->contdown_show_num_id == 0) {
            log_info("creat the sys to display countdown");
            log_info("timer_countdown_num is %d", timer_countdown_num);
            __timer_countdown->contdown_show_num_id = sys_timer_add(NULL, timer_countdown_display_string, 500);

        }
        if (__timer_countdown->countdown_customize_start) {
            if (__timer_countdown->countdown_flag == 0) {
                if (!(__timer_countdown->countdown_count_num_id)) {
                    __timer_countdown->countdown_count_num_id = sys_timer_add(NULL, timer_countdown_func, 1000);
                    __timer_countdown->countdown_flag = 1;
                    struct ui_pic *pic = (struct ui_pic *)ui_core_get_element_by_id(TIMER_COUNTDOWM_SP);
                    if (pic) {
                        ui_pic_set_image_index(pic, 1);
                    }
                }
            }
        }
        /* ui_auto_shut_down_disable(); */
        break;
    case ON_CHANGE_RELEASE:
        /* ui_auto_shut_down_enable(); */
        if (__timer_countdown->countdown_flag) {
            timer_background_prev_time = timer_get_ms();
            timer_background_cnt ++;
            sys_timeout_add((void *)timer_background_cnt, timer_background_func, timer_countdown_num * 1000);
        } else {
            timer_countdown_num = 0;
            timer_background_prev_time = 0;
        }
        if (__timer_countdown->countdown_count_num_id) {
            sys_timer_del(__timer_countdown->countdown_count_num_id);
            __timer_countdown->countdown_count_num_id = 0;
            __timer_countdown->countdown_flag = 0;
        }
        if (__timer_countdown->countdown_customize_start) {
            __timer_countdown->countdown_customize_start = 0;
            __timer_countdown->countdown_flag = 0;
        }
        if (__timer_countdown->contdown_show_num_id) {
            sys_timer_del(__timer_countdown->contdown_show_num_id);
            __timer_countdown->contdown_show_num_id = 0;
        }
        break;
    default:
        break;
    }
    return false;
}
static int TIMER_CALCULAGRAPH_FRONT_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
        ui_hide(LAYOUT_TIMER_RUN);
        ui_show(LAYOUT_TIMER_SEL);
        return true;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(LAYOUT_TIMER_RUN)
.ontouch = TIMER_CALCULAGRAPH_FRONT_ontouch,
 .onkey = NULL,
  .onchange = TIMER_CALCULAGRAPH_FRONT_onchange,
};
static void timer_countdown_display_string(void *priv)
{
    if (__timer_countdown->contdown_show_num_id == 0) {
        return ;
    }
    struct unumber countdown_buf;
    if (timer_countdown_num >= 3600) {
        struct element *layout = ui_core_get_element_by_id(SMALL_ONE);
        if (!layout->css.invisible) {
            ui_hide(SMALL_ONE);
        }
        layout = ui_core_get_element_by_id(BIG_ONE);
        if (layout->css.invisible) {
            ui_show(BIG_ONE);
        }


        __timer_countdown->countdown_hour = timer_countdown_num / 3600;
        __timer_countdown->countdown_min = (timer_countdown_num % 3600) / 60;
        __timer_countdown->countdown_sec = ((timer_countdown_num % 3600) % 60);
        /* countdown_buf.type = TYPE_STRING; */
        countdown_buf.type = TYPE_NUM;
        countdown_buf.numbs = 1;
        countdown_buf.number[0] = __timer_countdown->countdown_sec;
        /* ui_number_update_by_id(BIG_S, &countdown_buf); */
        ui_number_update(ui_core_get_element_by_id(BIG_S), &countdown_buf);
        countdown_buf.number[0] = __timer_countdown->countdown_min;
        /* ui_number_update_by_id(BIG_M, &countdown_buf); */
        ui_number_update(ui_core_get_element_by_id(BIG_M), &countdown_buf);
        countdown_buf.number[0] = __timer_countdown->countdown_hour;
        /* ui_number_update_by_id(BIG_H, &countdown_buf); */
        ui_number_update(ui_core_get_element_by_id(BIG_H), &countdown_buf);

        struct element *elm = ui_core_get_element_by_id(BIG_S);
        ui_core_redraw(elm->parent);
    } else {
        struct element *layout = ui_core_get_element_by_id(BIG_ONE);
        if (!layout->css.invisible) {
            ui_hide(BIG_ONE);
        }
        layout = ui_core_get_element_by_id(SMALL_ONE);
        if (layout->css.invisible) {
            ui_show(SMALL_ONE);
        }
        __timer_countdown->countdown_min = timer_countdown_num / 60;
        __timer_countdown->countdown_sec = timer_countdown_num % 60;
        countdown_buf.type = TYPE_NUM;
        countdown_buf.numbs = 1;
        countdown_buf.number[0] = __timer_countdown->countdown_sec;
        /* ui_number_update_by_id(SMALL_S, &countdown_buf); */
        ui_number_update(ui_core_get_element_by_id(SMALL_S), &countdown_buf);
        countdown_buf.number[0] = __timer_countdown->countdown_min;
        /* ui_number_update_by_id(SMALL_M, &countdown_buf); */
        ui_number_update(ui_core_get_element_by_id(SMALL_M), &countdown_buf);

        struct element *elm = ui_core_get_element_by_id(SMALL_S);
        ui_core_redraw(elm->parent);
    }
    /* countdown_buf.num_str = &(__timer_countdown->countdown_dis_buf); */
    /* ui_number_update_by_id(TIMER_COUNTDOWM, &countdown_buf); */
}

static int TIMER_COUNTDOWM_SP_BUT_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct button *bton = (struct button *) ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        log_info("LAQ test __timer_countdown->countdown_flag = %d", __timer_countdown->countdown_flag);
        /* printf("flag=%d id=%d",__timer_countdown->countdown_flag,__timer_countdown->countdown_count_num_id); */
        if (bton->elm.id == TIMER_COUNTDOWM_SP_BUT) {
            if (__timer_countdown->countdown_flag == 0) {
                ui_pic_show_image_by_id(TIMER_COUNTDOWM_SP, 1);
                if (!(__timer_countdown->countdown_count_num_id)) {
                    __timer_countdown->countdown_count_num_id = sys_timer_add(NULL, timer_countdown_func, 1000);
                    __timer_countdown->countdown_flag = 1;
                }
            } else if (__timer_countdown->countdown_flag == 1) {
                if (__timer_countdown->countdown_count_num_id) {
                    ui_pic_show_image_by_id(TIMER_COUNTDOWM_SP, 0);
                    sys_timer_del(__timer_countdown->countdown_count_num_id);
                    __timer_countdown->countdown_count_num_id = 0;
                    __timer_countdown->countdown_flag = 0;
                }
            }
        } else if (bton->elm.id == TIMER_COUNTDOWM_CANCLE) {
            if (__timer_countdown->countdown_count_num_id) {
                sys_timer_del(__timer_countdown->countdown_count_num_id);
                __timer_countdown->countdown_flag = 0;
                __timer_countdown->countdown_count_num_id = 0;
            }
            ui_send_event(KEY_CHANGE_PAGE, BIT(31) | CALCULAGRAPH_SEL);
        }
        return true;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(TIMER_COUNTDOWM_SP_BUT)
.ontouch = TIMER_COUNTDOWM_SP_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(TIMER_COUNTDOWM_CANCLE)
.ontouch = TIMER_COUNTDOWM_SP_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};


/***************************************************************************************
                                LAQ PAGE23 页面处理部分
****************************************************************************************/

static int TIMER_CALCULAGRAPH_ADD_onchange(void *ctrl,  enum element_change_event e, void *arg)
{
    struct window *win = (struct window *)ctrl;
    switch (e) {
    case ON_CHANGE_INIT:
        timer_countdown_num = 0;
        timer_background_prev_time = 0;
        __timer_countdown->countdown_hour = 0;
        __timer_countdown->countdown_min = 0;
        __timer_countdown->countdown_sec = 0;
        /* ui_auto_shut_down_disable(); */
        break;
    case ON_CHANGE_RELEASE:
        // if(__timer_countdown->countdown_num_id){
        //     sys_timer_del(__timer_countdown->countdown_num_id);
        //     __timer_countdown->countdown_num_id =0;
        // }
        /* ui_auto_shut_down_enable(); */
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(CALCULAGRAPH_ADD)
.ontouch = NULL,
 .onkey = NULL,
  .onchange = TIMER_CALCULAGRAPH_ADD_onchange,
};

static void reflash_set_num(void *priv)
{
    if ((__timer_countdown->contdown_show_num_id) == 0) {
        return ;
    }
    struct unumber set_num;
    /* memset(__timer_countdown->countdown_dis_buf, 0, sizeof(__timer_countdown->countdown_dis_buf)); */
    /* set_num.type = TYPE_STRING; */
    /* sprintf(__timer_countdown->countdown_dis_buf, "%02d%c%02d%c%02d", __timer_countdown->countdown_hour, ':', __timer_countdown->countdown_min, ':', __timer_countdown->countdown_sec); */
    /* set_num.num_str = &(__timer_countdown->countdown_dis_buf); */
    /* ui_number_update_by_id(TIMER_SET_NUM_, &set_num); */
    log_info("reflash_set_num\n");
    set_num.type = TYPE_NUM;
    set_num.numbs = 1;
    set_num.number[0] = __timer_countdown->countdown_sec;
    /* ui_number_update_by_id(TIMER_S, &set_num); */
    ui_number_update(ui_core_get_element_by_id(TIMER_S), &set_num);
    set_num.number[0] = __timer_countdown->countdown_min;
    /* ui_number_update_by_id(TIMER_M, &set_num); */
    ui_number_update(ui_core_get_element_by_id(TIMER_M), &set_num);
    set_num.number[0] = __timer_countdown->countdown_hour;
    /* ui_number_update_by_id(TIMER_H, &set_num); */
    ui_number_update(ui_core_get_element_by_id(TIMER_H), &set_num);

    struct element *elm = ui_core_get_element_by_id(TIMER_H);
    ui_core_redraw(elm->parent);
}

static int TIMER_SET_NUM_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_number *numbr = (struct ui_number *) ctrl;

    switch (e) {
    case ON_CHANGE_INIT:
        if ((__timer_countdown->contdown_show_num_id) == 0) {
            __timer_countdown->contdown_show_num_id = sys_timer_add(NULL, reflash_set_num, 500);
        }
        break;
    case ON_CHANGE_RELEASE:
        if (__timer_countdown->contdown_show_num_id) {
            sys_timer_del(__timer_countdown->contdown_show_num_id);
            __timer_countdown->contdown_show_num_id = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

/*
 * 设置时间操作部分 *
*/

static void timer_set_func(int id)
{
    switch (id) {
    case TIMER_HOUR_ADD:
        if (__timer_countdown->countdown_hour < 23) {
            __timer_countdown->countdown_hour ++ ;
        } else if (__timer_countdown->countdown_hour == 23) {
            __timer_countdown->countdown_hour = 0;
        }
        break;
    case TIMER_MIN_ADD:
        if (__timer_countdown->countdown_min < 59) {
            __timer_countdown->countdown_min ++ ;
        } else if (__timer_countdown->countdown_min == 59) {
            __timer_countdown->countdown_min = 0;
        }
        break;
    case TIMER_SEC_ADD:
        if (__timer_countdown->countdown_sec < 59) {
            __timer_countdown->countdown_sec  ++ ;
        } else if (__timer_countdown->countdown_sec  == 59) {
            __timer_countdown->countdown_sec  = 0;
        }
        break;
    case TIMER_HOUR_CUT:
        if (__timer_countdown->countdown_hour > 0) {
            __timer_countdown->countdown_hour -- ;
        } else if (__timer_countdown->countdown_hour == 0) {
            __timer_countdown->countdown_hour = 23;
        }
        break;
    case TIMER_MIN_CUT:
        if (__timer_countdown->countdown_min > 0) {
            __timer_countdown->countdown_min -- ;
        } else if (__timer_countdown->countdown_min == 0) {
            __timer_countdown->countdown_min = 59;
        }
        break;
    case TIMER_SEC_CUT:
        if (__timer_countdown->countdown_sec > 0) {
            __timer_countdown->countdown_sec  -- ;
        } else if (__timer_countdown->countdown_sec  == 0) {
            __timer_countdown->countdown_sec  = 59;
        }
        break;
    /* case TIMER_SET_BUTT: */
    case BASEFORM_751:
        timer_countdown_num = (__timer_countdown->countdown_hour * 3600 + __timer_countdown->countdown_min * 60 + __timer_countdown->countdown_sec);
        log_info("timer_countdown num is %d", __timer_countdown->countdown_hour);
        if (timer_countdown_num) {
            //全部为0的时候，按键无效
            __timer_countdown->countdown_customize_start = 0;
            /* ui_send_event(KEY_CHANGE_PAGE, BIT(31) | CALCULAGRAPH_FRONT); */
            ui_send_event(KEY_CHANGE_PAGE, BIT(31) | CALCULAGRAPH_SEL);
        }
        break;
    default:
        break;
    }
}

static int TIMER_SET_BUT_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct button *btn = (struct button *) ctrl;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        log_info("btn->elm.id is 0x%x", btn->elm.id);
        timer_set_func(btn->elm.id);
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(TIMER_HOUR_ADD)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(TIMER_MIN_ADD)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(TIMER_SEC_ADD)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(TIMER_HOUR_CUT)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(TIMER_MIN_CUT)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(TIMER_SEC_CUT)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(BASEFORM_751)
.ontouch = TIMER_SET_BUT_ontouch,
 .onkey = NULL,
  .onchange = NULL,
};

REGISTER_UI_EVENT_HANDLER(BASEFORM_478)
.ontouch = NULL,
 .onkey = NULL,
  .onchange = TIMER_SET_NUM_onchange,
};

#endif /* #if TCFG_UI_ENABLE_TIMER_ACTION */
#endif /* #if (!TCFG_LUA_ENABLE) */

