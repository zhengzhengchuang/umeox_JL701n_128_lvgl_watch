#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "cJSON.h"
#include "btstack/avctp_user.h"
#include "app_power_manage.h"
#include "asm/imb.h"
#include "ui_draw/ui_wave.h"
#include "ui/ui_measure.h"

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)

#define STYLE_NAME  JL
#define DIAL_SIDEBAR_START_THRESHOLD      (454 * 15 / 100)
#define DIAL_SIDEBAR_MAX_THRESHOLD      (454 * 70 / 100)//区域判断
#define DIAL_SIDEBAR_MIN_THRESHOLD      (454 * 30 / 100)//区域判断
#define DIAL_SIDEBAR_RUN_MAX_THRESHOLD  (454 * 85 / 100)//滑动距离判断
#define DIAL_SIDEBAR_RUN_MIN_THRESHOLD  (454 * 15 / 100)//滑动距离判断
#define DIAL_SIDEBAR_LEFT_MAX_THRESHOLD      (163 * 70 / 100)//区域判断
#define DIAL_SIDEBAR_LEFT_MIN_THRESHOLD      (163 * 30 / 100)//区域判断
#define DIAL_SIDEBAR_LEFT_RUN_MAX_THRESHOLD  (163 * 85 / 100)//滑动距离判断
#define DIAL_SIDEBAR_LEFT_RUN_MIN_THRESHOLD  (163 * 15 / 100)//滑动距离判断
#define DIAL_SIDEBAR_STEP               (454 * 20 / 100)//滑动步进


//#define SIDEBAR_X_EN//作废
#if TCFG_UI_ENABLE_LEFT_MENU
#define SIDEBAR_X_Y_EN
#endif
REGISTER_UI_STYLE(STYLE_NAME)


extern int watch_get_style();
extern char *watch_get_item(int style);
extern int res_get_picture_number(RESFILE *file, int page);
extern void ui_page_move_en_callback(u8(*move_en_query)(void));
extern u16 get_lcd_width_from_imd(void);
u8 is_ui_page_move_enable(void);
void ui_send_event(u16 event, u32 val);
//int PAGE_switch_ontouch(void *_layout, struct element_touch_event *e);

static u32 sidebar_root = 0;

static int watch_show_timer = 0;

int get_elapse_time(struct sys_time *time)
{
    static struct sys_time cur_time = {0};
    static u32 jiffies_offset = 0;
    static u32 jiffies_base = 0;
    int current_msec = jiffies_msec();

    int msec = current_msec + jiffies_offset - jiffies_base;

    if (msec < 0) {  //越界，清0处理
        jiffies_base = 0;
        cur_time.hour = 0;
        cur_time.min = 0;
        cur_time.sec = 0;
        msec = current_msec + jiffies_offset - jiffies_base;
    }

    if (msec >= 1000) {
        do {
            msec -= 1000;
            if (++cur_time.sec >= 60) {
                cur_time.sec = 0;
                if (++cur_time.min >= 60) {
                    cur_time.min = 0;
                    if (++cur_time.hour >= 99) {
                        cur_time.hour = 0;
                    }
                }
            }
        } while (msec >= 1000);

        jiffies_base = current_msec + jiffies_offset - msec;
    }

    memcpy(time, &cur_time, sizeof(struct sys_time));
    /* slog_info(time, "[%02d:%02d:%02d.%03d]", cur_time.hour, cur_time.min, */
    /* cur_time.sec, msec); */

    return 1;
}

static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        get_elapse_time(time);
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    /* log_info("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec); */
    dev_close(fd);
}

int watch_refresh(int id, struct ui_watch *watch)
{
    /* log_info("data source : %s\n", watch->source); */
    if (!strcmp(watch->source, "rtc")) {
        struct sys_time time;
        get_sys_time(&time);
        /* log_info("%04d-%02d-%02d %02d:%02d:%02d\n", time.year, time.month, time.day, time.hour, time.min, time.sec); */

        /* ui_watch_set_time_by_id(id, time.hour % 12, time.min, time.sec); */
        ui_watch_set_time((struct ui_watch *)ui_core_get_element_by_id(id), time.hour % 12, time.min, time.sec);
    }
    return 0;
}

int progress_refresh(int id, struct ui_progress *progress)
{
    if (!strcmp(progress->source, "step")) {
        int step = jiffies_msec() % 100000;
        u8 percent = 0;
        if (step < 10000) {
            percent = step * 100 / 10000;
        }
        if (step < 100000) {
            percent = step * 100 / 100000;
        } else {
            percent = 100;
        }
        /* ui_progress_set_persent_by_id(id, percent); */
    }
    return 0;
}


int multiprogress_refresh(int id, struct ui_multiprogress *multiprogress)
{

    return 0;
}

int text_refresh(int id, struct ui_text *text)
{
    if (!strcmp(text->source, "text1")) {
        /* ui_text_set_str_by_id(id, "ascii", "0123"); */
        ui_text_set_str((struct ui_text *)ui_core_get_element_by_id(id), "ascii", "0123", -1, FONT_DEFAULT);
    }
    return 0;
}

int number_refresh(int id, struct ui_number *number)
{
    struct unumber num;

    /* test */
    if (!strcmp(number->source, "number1")) {
        num.type = TYPE_STRING;
        num.num_str = (u8 *)"0123456789";
        /* ui_number_update_by_id(number->text.elm.id, &num); */
        ui_number_update(number, &num);
    } else if (!strcmp(number->source, "number2")) {
        num.type = TYPE_NUM;
        num.numbs = 2;
        num.number[0] = 88;
        num.number[1] = 55;
        /* ui_number_update_by_id(number->text.elm.id, &num); */
        ui_number_update(number, &num);
    } else if (!strcmp(number->source, "numb")) {
        static int test  = 0;

        struct unumber numb;
        test += 85;
        numb.type = TYPE_NUM;
        numb.numbs = 1;
        numb.number[0] = test % 1000;

        /* ui_number_update_by_id(id, &numb); */
        ui_number_update(number, &numb);

    } else if (!strcmp(number->source, "step")) {
        log_info("%x %x\n", id, (int)number);
        num.type = TYPE_NUM;
        num.numbs = 1;
        num.number[0] = jiffies_msec() % 100000;
        ui_number_update(number, &num);
    } else if (!strcmp(number->source, "heart")) {
        struct unumber numb;
        num.type = TYPE_NUM;
        num.numbs = 1;
        num.number[0] = jiffies_msec() % 1000;
        ui_number_update(number, &num);

    }
    return 0;
}

int time_refresh(int id, struct ui_time *time)
{
    if (!strcmp(time->source, "rtc")) {
        struct sys_time time_t;
        struct utime ui_time = {0};

        get_sys_time(&time_t);
        ui_time.hour = time_t.hour;
        ui_time.min = time_t.min;
        ui_time.sec = time_t.sec;
        /* ui_time_update_by_id(id, &ui_time); */
        ui_time_update((struct ui_time *)ui_core_get_element_by_id(id), &ui_time);
    } else if (!strcmp(time->source, "time1")) {
        struct sys_time time;
        struct utime time_r;

        get_sys_time(&time);
        time_r.month = time.month;
        time_r.day = time.day;
        time_r.hour = time.hour;
        time_r.min = time.min;
        time_r.sec = time.sec;

        /* ui_time_update_by_id(id, &time_r); */
        ui_time_update((struct ui_time *)ui_core_get_element_by_id(id), &time_r);
    } else if (!strcmp(time->source, "time2")) {
        struct sys_time time;
        struct utime time_r;

        get_sys_time(&time);
        time_r.month = time.month;
        time_r.day = time.day;
        time_r.hour = time.hour;
        time_r.min = time.min;
        time_r.sec = time.sec;

        /* ui_time_update_by_id(id, &time_r); */
        ui_time_update((struct ui_time *)ui_core_get_element_by_id(id), &time_r);
    } else if (!strcmp(time->source, "none")) { //test
        struct utime ui_time = {0};
        ui_time.hour = 12;
        ui_time.min = 34;
        ui_time.sec = 56;
        /* ui_time_update_by_id(time->text.elm.id, &ui_time); */
        ui_time_update((struct ui_time *)ui_core_get_element_by_id(id), &ui_time);
    }
    return 0;
}

static int progress_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct ui_progress *progress = (struct ui_progress *)_ctrl;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        break;
    }

    return 0;
}

static int multiprogress_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct ui_multiprogress *multiprogress = (struct ui_multiprogress *)_ctrl;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        break;
    }

    return 0;
}

static int text_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct ui_text *text = (struct ui_text *)_ctrl;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        break;
    }

    return 0;
}

static int time_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct ui_time *time = (struct ui_time *)_ctrl;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        break;
    }

    return 0;
}

static int number_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct ui_number *number = (struct ui_number *)_ctrl;

    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_UP:
        break;
    }

    return 0;
}

static int progress_onkey(void *_ctrl, struct element_key_event *event)
{
    struct ui_progress *progress = (struct ui_progress *)_ctrl;
    switch (event->value) {
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:
        break;
    case KEY_RIGHT:
        break;
    }

    return 0;
}

static int multiprogress_onkey(void *_ctrl, struct element_key_event *event)
{
    struct ui_multiprogress *multiprogress = (struct ui_multiprogress *)_ctrl;

    switch (event->value) {
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:
        break;
    case KEY_RIGHT:
        break;
    }

    return 0;
}

static int time_onkey(void *_ctrl, struct element_key_event *event)
{
    struct ui_time *time = (struct ui_time *)_ctrl;

    switch (event->value) {
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:
        break;
    case KEY_RIGHT:
        break;
    }

    return 0;
}

static int text_onkey(void *_ctrl, struct element_key_event *event)
{
    struct ui_text *text = (struct ui_text *)_ctrl;

    switch (event->value) {
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:
        break;
    case KEY_RIGHT:
        break;
    }

    return 0;
}

static int number_onkey(void *_ctrl, struct element_key_event *event)
{
    struct ui_number *number = (struct ui_number *)_ctrl;

    switch (event->value) {
    case KEY_UP:
        break;
    case KEY_DOWN:
        break;
    case KEY_LEFT:
        break;
    case KEY_RIGHT:
        break;
    }

    return 0;
}

static int multiprogress_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_multiprogress *multiprogress = (struct ui_multiprogress *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }

    return 0;
}

static int progress_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_progress *progress = (struct ui_progress *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }

    return 0;
}

static int text_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }

    return 0;
}

static int time_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_time *time = (struct ui_time *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        if (!strcmp(time->source, "rtc")) {
            struct sys_time time_t;
            struct utime ui_time = {0};

            get_sys_time(&time_t);
            ui_time.hour = time_t.hour;
            ui_time.min = time_t.min;
            ui_time.sec = time_t.sec;
            ui_time_update(time, &ui_time);
        } else if (!strcmp(time->source, "none")) {
            struct utime ui_time = {0};
            ui_time.hour = 12;
            ui_time.min = 34;
            ui_time.sec = 56;

            ui_time_update(time, &ui_time);
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return 0;
}

static int number_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_number *number = (struct ui_number *)_ctrl;
    struct unumber num;
    /* log_info("number->source : %s\n", number->source); */

    switch (event) {
    case ON_CHANGE_INIT:
        if (!strcmp(number->source, "number1")) {
            num.type = TYPE_STRING;
            num.num_str = (u8 *)"0123456789";

            ui_number_update(number, &num);
        } else if (!strcmp(number->source, "number2")) {
            num.type = TYPE_NUM;
            num.numbs = 2;
            num.number[0] = 88;
            num.number[1] = 55;

            ui_number_update(number, &num);
        } else if (!strcmp(number->source, "step")) {
            num.type = TYPE_NUM;
            num.numbs = 1;
            num.number[0] = jiffies_msec() % 10000;
            ui_number_update(number, &num);
        } else if (!strcmp(number->source, "heart")) {
            num.type = TYPE_NUM;
            num.numbs = 1;
            num.number[0] = jiffies_msec() % 200;
            ui_number_update(number, &num);
        }

        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }

    return 0;
}

/*
 * 表盘以及子控件ontouch响应
 * */
int watch_child_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;

    int type = ui_id2type(elm->id);


    switch (type) {
    case CTRL_TYPE_PROGRESS:
        progress_ontouch(_ctrl, e);
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        multiprogress_ontouch(_ctrl, e);
        break;
    case CTRL_TYPE_TEXT:
        text_ontouch(_ctrl, e);
        break;
    case CTRL_TYPE_NUMBER:
        number_ontouch(_ctrl, e);
        break;
    case CTRL_TYPE_TIME:
        time_ontouch(_ctrl, e);
        break;
    default:
        break;
    }

    return 0;
}

/*
 * 表盘以及子控件onkey响应
 * */
int watch_child_onkey(void *_ctrl, struct element_key_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);

    switch (type) {
    case CTRL_TYPE_PROGRESS:
        progress_onkey(_ctrl, e);
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        multiprogress_onkey(_ctrl, e);
        break;
    case CTRL_TYPE_TEXT:
        text_onkey(_ctrl, e);
        break;
    case CTRL_TYPE_NUMBER:
        number_onkey(_ctrl, e);
        break;
    case CTRL_TYPE_TIME:
        time_onkey(_ctrl, e);
        break;
    }

    return 0;
}

/*
 * 表盘以及子控件onchange响应
 * */
int watch_child_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    int type = ui_id2type(elm->id);

    switch (type) {
    case CTRL_TYPE_PROGRESS:
        progress_onchange(_ctrl, event, arg);
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        multiprogress_onchange(_ctrl, event, arg);
        break;
    case CTRL_TYPE_TEXT:
        text_onchange(_ctrl, event, arg);
        break;
    case CTRL_TYPE_NUMBER:
        number_onchange(_ctrl, event, arg);
        break;
    case CTRL_TYPE_TIME:
        time_onchange(_ctrl, event, arg);
        break;
    default:
        break;
    }

    return 0;
}


/*
 * 表盘以及子控件参数更新
 * */
int watch_child_cb(void *_ctrl, int id, int type)
{
    switch (type) {
    case CTRL_TYPE_WATCH:
        watch_refresh(id, (struct ui_watch *)_ctrl);
        break;
    case CTRL_TYPE_PROGRESS:
        progress_refresh(id, (struct ui_progress *)_ctrl);
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        multiprogress_refresh(id, (struct ui_multiprogress *)_ctrl);
        break;
    case CTRL_TYPE_TEXT:
        text_refresh(id, (struct ui_text *)_ctrl);
        break;
    case CTRL_TYPE_NUMBER:
        number_refresh(id, (struct ui_number *)_ctrl);
        break;
    case CTRL_TYPE_TIME:
        time_refresh(id, (struct ui_time *)_ctrl);
        break;
    case CTRL_TYPE_LAYOUT:
        struct element *p;
        list_for_each_child_element(p, (struct element *)_ctrl) {
            if (watch_child_cb(p, p->id, ui_id2type(p->id))) {
                break;
            }
        }
        break;
    }

    return 0;
}


static void WATCH_timer(void *priv)
{
    if (!watch_show_timer) {
        return ;
    }
    ui_get_child_by_id(STYLE_DIAL_ID(WATCH), watch_child_cb);

    struct element *elm = NULL;

    if (sidebar_root) {
        elm = ui_core_get_element_by_id(sidebar_root);
    }

    if (!elm) {
        elm = ui_core_get_element_by_id(STYLE_DIAL_ID(WATCH));
    }

    if (elm) {
        ui_io_set(IO_FRAME, HIGH);
        int a = jiffies_msec();
        ui_core_redraw(elm);
        int b = jiffies_msec();
        printf("watch redraw %d ms", b-a);
        ui_io_set(IO_FRAME, LOW);
    }
}


int get_watch_animation_path(int style, char *anim_path, int path_len)
{
    char *path = watch_get_item(style);
    char *p;
    if (path_len < (strlen(path) + 2)) {
        return -1;
    }

    strcpy(anim_path, path);
    p = &anim_path[strlen(anim_path)];
    while (*p-- != '.');
    *++p = '.';
    *++p = 'a';
    *++p = 'n';
    *++p = 'i';
    *++p = 'm';
    *++p = '\0';

    return 0;
}

struct watch_param {
    int curr_watch_style;
    RESFILE *view_file;
    struct flash_file_info view_file_info;
    int find_animation;
    int picture_num;
    int curr_picture;
};

enum {
    SHOW_BEGIN,
    SHOW_MID,
    SHOW_END,
};

int watch_show_positon(struct draw_context *dc)
{
    if ((dc->disp.top == dc->rect.top) && (dc->disp.width == dc->rect.width)) {
        return SHOW_BEGIN;
    } else if ((dc->disp.top + dc->disp.height) == (dc->rect.top + dc->rect.height)) {
        return SHOW_END;
    } else {
        return SHOW_MID;
    }
}



extern char *watch_bgp_get_related(u8 cur_watch);


extern char *watch_get_background();

struct watch_ex {
    int heart;//心率
    int battrty;//电量
    int weather;//天气
    int press;//压力
    int step;//步数
    int streng;//强度
    int active;//活动时间
    int calorie;//热量
    int baro;//气压
    int alti;//海拔
    int oxygen;//血氧
    int time;//时间
    int zone;//时区
    struct position pos[4];
};

static struct watch_ex watch_cfg = {0};
extern int watch_get_style();
char *watch_get_item(int style);

static int watch_load_cfg_file(void *arg)
{
    FILE *file;

    char *root_path = watch_get_item(watch_get_style());
    char *watch;//[64];// = {0};
    watch = zalloc(128);
    strcpy(watch, root_path);
    strcpy(watch + strlen(watch) - 4, ".cfg");

    file = fopen(watch, "r");
    if (!file) {
        log_info("open_cfg fail %s\n", watch);
        free(watch);
        return -1;
    }
    int len = 512;
    log_info("cfg size = %x \n", len);
    u8 *buf = zalloc(len);
    fread(file, buf, len);
    log_info("%s \n", buf);
    cJSON *root = NULL;
    cJSON *item = NULL;
    root = cJSON_Parse((const char *)buf);
    if (!root) {
        log_info("Error before: [%s]\n", cJSON_GetErrorPtr());
    } else {
        item = cJSON_GetObjectItem(root, "step");
        if (item) {
            watch_cfg.step = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "heart");
        if (item) {
            watch_cfg.heart = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "weather");
        if (item) {
            watch_cfg.weather = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "time");
        if (item) {
            watch_cfg.time = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "battrty");
        if (item) {
            watch_cfg.battrty = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "press");
        if (item) {
            watch_cfg.press = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "streng");
        if (item) {
            watch_cfg.streng = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "avtive");
        if (item) {
            watch_cfg.active = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "calorie");
        if (item) {
            watch_cfg.calorie = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "baro");
        if (item) {
            watch_cfg.baro = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "alti");
        if (item) {
            watch_cfg.alti = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "oxygen");
        if (item) {
            watch_cfg.oxygen = item->valueint;
        }
        cJSON_Delete(root);
    }

    put_buf((const u8 *)&watch_cfg, sizeof(watch_cfg));

    if (file) {
        fclose(file);
        file = NULL;
    }

    if (buf) {
        free(buf);
        buf = NULL;
    }


    watch_cfg.pos[0].x = 1250;
    watch_cfg.pos[0].y = 0;

    watch_cfg.pos[1].x = 0;
    watch_cfg.pos[1].y = 2500;

    watch_cfg.pos[2].x = 0;
    watch_cfg.pos[2].y = 5000;

    watch_cfg.pos[3].x = 1250;
    watch_cfg.pos[3].y = 7500;
    void *hdl;
    struct element *elm;

    strcpy(watch + strlen(watch) - 4, ".tab");

    hdl = load_control_info_by_id(watch, 0x2000000, 0X3F6ABD8);
    if (hdl)  {
        elm = (struct element *)ui_control_new(hdl, arg);
        if (elm) {
            elm->css.invisible = 0;
            elm->css.top = 0;
            elm->css.left = 0;
            log_info("create new widget succ!\n");
        }
    }

    hdl = load_control_info_by_id(watch, 0x2000000, 0X3F6ABD8);
    if (hdl)  {
        elm = (struct element *)ui_control_new(hdl, arg);
        if (elm) {
            elm->css.invisible = 0;
            elm->css.top = 2500;
            elm->css.left = 2500;
            log_info("create new widget succ!\n");
        }
    }
    if (watch) {
        free(watch);
    }
    return 0;
}

extern int pd_menu_bt_status_handler(const char *type, u32 arg);
extern int pd_menu_bat_status_handler(const char *type, u32 arg);
extern int notice_status_handler(const char *type, u32 arg);
int sidebar_bt_status_handler(const char *type, u32 arg)
{
    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case BT_STATUS_SECOND_CONNECTED:
        case BT_STATUS_FIRST_CONNECTED:
        case BT_STATUS_CONN_A2DP_CH:
        case BT_STATUS_FIRST_DISCONNECT:
        case BT_STATUS_SECOND_DISCONNECT:
#if TCFG_UI_ENABLE_PULLDOWN_MENU
            pd_menu_bt_status_handler(type, arg);
#endif /* #if TCFG_UI_ENABLE_PULLDOWN_MENU */
            break;
        }
    }

    return 0;
}

int sidebar_bat_status_handler(const char *type, u32 arg)
{
    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case POWER_EVENT_POWER_CHANGE:
        case POWER_EVENT_POWER_CHARGE:
#if TCFG_UI_ENABLE_PULLDOWN_MENU
            pd_menu_bat_status_handler(type, arg);
#endif /* #if TCFG_UI_ENABLE_PULLDOWN_MENU */
            break;
        }
    }

    return 0;
}

int sidebar_message_status_handler(const char *type, u32 arg)
{
    if (type && (!strcmp(type, "event"))) {
        switch (arg) {
        case 1: /* add */
        case 2: /* del */
        case 3: /* reset grid */
#if TCFG_UI_ENABLE_NOTICE
            notice_status_handler(type, arg);
#endif /* #if TCFG_UI_ENABLE_NOTICE */
            break;
        }
    }

    return 0;
}
static const struct uimsg_handl ui_pd_menu_msg_handler[] = {
    { "bt_status",        sidebar_bt_status_handler     }, /* 蓝牙状态 */
    { "bat_status",       sidebar_bat_status_handler     },
    { "message_status",   sidebar_message_status_handler     },
    { NULL, NULL},      /* 必须以此结尾！ */
};


#define UI_WATCH_EFFECT 0


#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
static struct effect_redraw_part redraw_part = {0};
static struct effect_sca_alpha redraw_sal = {0};
#endif

static int WATCH_onchange(void *ctr, enum element_change_event e, void *arg)
{
    u32 rets;//

    struct ui_watch *watch = (struct ui_watch *)ctr;
    struct draw_context *dc = (struct draw_context *)arg;
    struct element *elm = (struct element *)ctr;
    struct sys_time time;
    char animpath[46];
    static struct watch_param param = {0};
    RESFILE *view_file;
    u32 flag;
    char *bg_path;
    int timer_interval = 0;
    union ui_control_info info;
    void *hdl;
    struct element_css *css;
    u8 slow_sec = 0;

    switch (e) {
    case ON_CHANGE_INIT:

        get_sys_time(&time);

        ui_watch_set_time(watch, time.hour % 12, time.min, time.sec);
        ui_set_default_handler(watch, watch_child_ontouch, watch_child_onkey, watch_child_onchange);

        /*****************************************************************/
        /* 匀速表盘设置，默认是跳秒也就是不打开，如果打开 slow_sec = 1,
         * 定时器间隔不能修改，必须是166ms定时，因为 360度 / 60 = 6度每秒，然后
         * 1000ms / 6度 = 166ms,也就是每转动1度需要166ms */
        /*****************************************************************/
        /* slow_sec = 1; */
        ui_watch_slow_sec_by_id(STYLE_DIAL_ID(WATCH), slow_sec);
        if (slow_sec == 1) {
            timer_interval = 166;
        } else {
            timer_interval = 500;
        }

        param.curr_watch_style = -1;
        param.find_animation = false;
        param.curr_picture = 0;
        param.view_file = NULL;

        /* bg_path = watch_get_background(); */
        bg_path = watch_bgp_get_related(watch_get_style());
        log_info("\n\n\n\ntest %d, %s\n\n\n\n\n", watch_get_style(), bg_path);
        if (bg_path) {
            param.view_file = res_fopen(bg_path, "r");
            ui_res_flash_info_get(&param.view_file_info, bg_path, "res");
        } else {
            get_watch_animation_path(watch_get_style(), animpath, sizeof(animpath));
            param.view_file = res_fopen(animpath, "r");
            ui_res_flash_info_get(&param.view_file_info, animpath, "res");
        }
        if (param.view_file) {
            res_fread(param.view_file, &flag, sizeof(flag));
            if (flag == 0x30335552) {
                param.find_animation = true;
                param.picture_num = res_get_picture_number(param.view_file, 0);
                param.curr_picture = 0;
                timer_interval = 200;
            } else if (flag == 0x31325552) {
                log_info("file version not support, please check it!\n");
            }
        }
        if (!watch_show_timer) {
            watch_show_timer = sys_timer_add(NULL, WATCH_timer, timer_interval);
        }



        int root = 0;
        if (elm && elm->parent) {
            elm = elm->parent;
            if (elm->parent) {
                root = elm->parent->id;
            }
        }

        ui_page_move_en_callback(is_ui_page_move_enable);

#ifdef SIDEBAR_X_EN
        if (root && create_control_by_id("storage/virfat_flash/C/sidebar/sidebar.tab", SIDEBAR_PAGE_0, SIDEBAR_SIDEBAR, root)) {
            sidebar_root = root;
            log_info("[5]crteate control succ! %x\n", root);
        } else {
            sidebar_root = 0;

            log_info("[5]crteate control fail! %x\n", root);
        }
#else
#ifdef SIDEBAR_X_Y_EN
        if (root && create_control_by_id("storage/virfat_flash/C/sidebar/sidebar.tab", SIDEBAR_PAGE_0,  SIDEBAR_SIDEBAR, root)) {
            sidebar_root = root;
            log_info("[5]crteate control succ! %x\n", root);
        } else {
            sidebar_root = 0;

            log_info("[5]crteate control fail! %x\n", root);
        }

#endif
        if (root && create_control_by_id("storage/virfat_flash/C/sidebar/sidebar.tab", SIDEBAR_PAGE_1, SIDEBAR_PULLDONW_MENU_LAYOUT, root)) {
            sidebar_root = root;
            log_info("[5]crteate control succ! %x\n", root);
        } else {
            sidebar_root = 0;

            log_info("[5]crteate control fail! %x\n", root);
        }

        if (root && create_control_by_id("storage/virfat_flash/C/sidebar/sidebar.tab", SIDEBAR_PAGE_1, SIDEBAR_UNDISTURB_LAYOUT, root)) {
            sidebar_root = root;
            log_info("[5]crteate control succ! %x\n", root);
        } else {
            sidebar_root = 0;

            log_info("[5]crteate control fail! %x\n", root);
        }

        if (root && create_control_by_id("storage/virfat_flash/C/sidebar/sidebar.tab", SIDEBAR_PAGE_2, SIDEBAR_NOTICE_LAYOUT, root)) {
            sidebar_root = root;
            log_info("[5]crteate control succ! %x, %d\n", root, __LINE__);
        } else {
            sidebar_root = 0;

            log_info("[5]crteate control fail! %x, %d\n", root, __LINE__);
        }

        if (sidebar_root) {
            ui_register_msg_handler(DIAL_PAGE_0, ui_pd_menu_msg_handler);//注册消息交互的回调
        }
#endif
        break;
    case ON_CHANGE_SHOW:
        if (param.find_animation) {
            if (SHOW_BEGIN == watch_show_positon(dc)) {
                param.curr_picture++;
                if (param.curr_picture > param.picture_num) {
                    param.curr_picture = 1;
                }
            }

            watch->elm.css.background_image = 1;
            dc->preview.file = param.view_file;
            dc->preview.file_info = &param.view_file_info;
            dc->preview.id = param.curr_picture;
            dc->preview.page = 0;
        }
        break;
    case ON_CHANGE_RELEASE:
        sidebar_root = 0;
        if (watch_show_timer) {
            sys_timer_del(watch_show_timer);
            watch_show_timer = 0;
        }
        if (param.view_file) {
            printf("%s ON_CHANGE_RELEASE", __func__);
            res_fclose(param.view_file);
            ui_res_flash_info_free(&param.view_file_info, "res");
            param.view_file = NULL;
        }
        ui_set_default_handler(watch, NULL, NULL, NULL);
#ifdef SIDEBAR_X_EN
        if (ui_core_get_element_by_id(SIDEBAR_SIDEBAR)) {
            ui_hide_set(SIDEBAR_SIDEBAR, HIDE_WITHOUT_REDRAW);
            delete_control_by_id(SIDEBAR_SIDEBAR);
        }
#else
#if 1
        if (ui_core_get_element_by_id(SIDEBAR_SIDEBAR)) {
            ui_hide_set(SIDEBAR_SIDEBAR, HIDE_WITHOUT_REDRAW);
            delete_control_by_id(SIDEBAR_SIDEBAR);
        }
#endif
        if (ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT)) {
            ui_hide_set(SIDEBAR_PULLDONW_MENU_LAYOUT, HIDE_WITHOUT_REDRAW);
            delete_control_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT);
        }
        if (ui_core_get_element_by_id(SIDEBAR_UNDISTURB_LAYOUT)) {
            ui_hide_set(SIDEBAR_UNDISTURB_LAYOUT, HIDE_WITHOUT_REDRAW);
            delete_control_by_id(SIDEBAR_UNDISTURB_LAYOUT);
        }
        if (ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT)) {
            ui_hide_set(SIDEBAR_NOTICE_LAYOUT, HIDE_WITHOUT_REDRAW);
            delete_control_by_id(SIDEBAR_NOTICE_LAYOUT);
        }
#endif
        break;
    default:
        return FALSE;
    }
    return FALSE;
}

enum SIDERBAR_STATUS {
    SIDEBAR_HIDE,
    SIDEBAR_MOVE_X,
    SIDEBAR_MOVE_Y,
    SIDEBAR_SHOWING_TOP,
    SIDEBAR_SHOWING_BOTTOM,
    SIDEBAR_SHOWING_LEFT,
    SIDEBAR_SHOW_TOP,
    SIDEBAR_SHOW_BOTTOM,
    SIDEBAR_SHOW_LEFT,
};

struct sidebar_priv {
    struct position pos;
    int first_x_offset;
    int x_offset;
    int first_y_offset;
    int y_offset;
    int step;
    int timer;
    u8 stop;
    enum SIDERBAR_STATUS sidebar_status;
};

struct sidebar_priv sidebar = {0};
void ui_core_redraw_area(struct rect *rect);
static int ui_x_move(struct element *elm, int x_offset)
{
    struct element_css *parent_css;
    struct element_css *css;
    struct rect rect;
    struct rect parent_rect;
    int css_left;
    int percent;
    if (!elm) {
        return 0;
    }

    ui_core_get_element_abs_rect(elm->parent, &parent_rect);
    ui_core_get_element_abs_rect(elm, &rect);
    rect.left = 0;

    css_left = x_offset * 10000 / parent_rect.width;

    css = ui_core_get_element_css(elm);
    css->left += css_left;
    percent = (css->left + css->width) * 100 / css->width;
    percent = (percent > 100) ? 100 : percent;
    percent = (percent < 0) ? 0 : percent;
    //percent = (css->left - css->width) * 100 / css->height;
    /* printf("percent%d cssright=%d css->width=%d", percent, (css->left + css->width) * 454 / 10000, css->width * 454 / 10000); */

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
    elm->css.alpha = percent;

#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
    redraw_sal.sca = 1.0 - 0.3 * percent / 100;
    redraw_sal.alpha = 255;

    redraw_part.reflesh = 1;
    redraw_part.part1_effect_mode = EFFECT_MODE_SCA_ALPHA;
    redraw_part.part1_effect_priv = &redraw_sal;

    if ((percent > 0) && (percent <= 100)) {
        redraw_part.part1_one_kick = 1;
    } else {
        redraw_part.part1_one_kick = 0;
        redraw_part.part1_kick_buf = NULL;
    }
    ui_window_effect(elm->parent->id, EFFECT_MODE_REDRAW_PART, NULL, &redraw_part);
#endif

#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
    ui_window_stop_redraw(0);
#endif

    ui_core_redraw_area(&rect);
    ui_core_redraw(elm->parent);

#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
    if ((percent > 0) && (percent < 100)) {
        ui_window_stop_redraw(1);
    } else {
        ui_window_stop_redraw(0);
    }
#endif


    return 1;
}

static int ui_y_move(struct element *elm, int y_offset, u8 dir)
{
    struct element_css *parent_css;
    struct element_css *css;
    struct rect rect;
    struct rect parent_rect;
    int css_top;
    u32 percent;
    int ret = 1;

    if (!elm) {
        return 0;
    }

    ui_core_get_element_abs_rect(elm->parent, &parent_rect);
    ui_core_get_element_abs_rect(elm, &rect);
    rect.top = 0;

    css_top = y_offset * 10000 / parent_rect.height;

    css = ui_core_get_element_css(elm);
    css->top += css_top;
    if (dir) {
        if (css->top > 0) {
            css->top = 0;
            ret = 0;
            /* ui_core_redraw_area(&rect); */
            /* ui_core_redraw(elm->parent); */
            /* return 0; */
        } else if (css->top < -css->height) {
            css->top = -css->height;
            ret = 0;
            /* ui_core_redraw_area(&rect); */
            /* ui_core_redraw(elm->parent); */
            /* return 0; */
        }
        percent = (css->top + css->height) * 100 / css->height;

    } else {
        if (css->top < 0) {
            css->top = 0;
            ret = 0;
            /* ui_core_redraw_area(&rect); */
            /* ui_core_redraw(elm->parent); */
            /* return 0; */
        } else if (css->top > css->height) {
            css->top = css->height;
            ret = 0;
            /* ui_core_redraw_area(&rect); */
            /* ui_core_redraw(elm->parent); */
            /* return 0; */
        }
        percent = (css->height - css->top) * 100 / css->height;

    }

    /* printf("\npercent %d\n\n", percent); */
    elm->css.alpha = percent;

#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
    redraw_sal.sca = 1.0 - 0.3 * percent / 100;
    redraw_sal.alpha = 255;
    redraw_part.reflesh = 1;
    redraw_part.part1_effect_mode = EFFECT_MODE_SCA_ALPHA;
    redraw_part.part1_effect_priv = &redraw_sal;

    if ((percent > 0) && (percent < 100)) {
        redraw_part.part1_one_kick = 1;
    } else {
        redraw_part.part1_one_kick = 0;
        redraw_part.part1_kick_buf = NULL;
    }
    ui_window_effect(elm->parent->id, EFFECT_MODE_REDRAW_PART, NULL, &redraw_part);
#endif


#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
    ui_window_stop_redraw(0);
#endif

    ui_core_redraw_area(&rect);
    ui_core_redraw(elm->parent);

#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT
    if ((percent > 0) && (percent < 100)) {
        ui_window_stop_redraw(1);
    } else {
        ui_window_stop_redraw(0);
    }
#endif


    return ret;
}

u8 is_ui_page_move_enable(void)
{
    return (sidebar.stop && (sidebar.sidebar_status == SIDEBAR_HIDE));
}

#define abs(x)  ((x)>0?(x):-(x) )
enum {
    SLIDER_MOVE_NONE,
    SLIDER_MOVE_UD,
    SLIDER_MOVE_LF,
};
static int WATCH_ontouch(void *_ctrl, struct element_touch_event *e)
{
    static int step = 0;
    static int id = 0;
    static int page_id = 0;
    static int parent_id = 0;
    static struct position pos = {0};
    static struct position move_pos = {0};
    struct rect rect;
    static int move_dir = SLIDER_MOVE_NONE;
    struct element_css *css_top;
    struct element_css *css_bottom;
    struct element_css *css_left;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
#if 0
        if (step == 0) {
            page_id = STYLE_DIAL_ID(PAGE_0);
            id = STYLE_DIAL_ID(PIC);
            /* page_id = STYLE_DIAL_ID(PAGE_1); */
            /* id = STYLE_DIAL_ID(PIC_TEST); */
            parent_id = STYLE_DIAL_ID(WATCH);
            create_control_by_id("storage/virfat_flash/C/watch/watch.tab", page_id, id, parent_id);
            step++;
        } else if (step == 1) {
            delete_control_by_id(id);
            step = 0;
        }
#endif

        if (sidebar_root == 0) {
            return true;
        }
        memcpy(&sidebar.pos, &e->pos, sizeof(struct position));
#ifdef SIDEBAR_X_EN
        sidebar.first_x_offset = e->pos.x;
#else
#ifdef SIDEBAR_X_Y_EN
        sidebar.first_x_offset = e->pos.x;
#endif
        sidebar.first_y_offset = e->pos.y;
#endif
        move_dir = SLIDER_MOVE_NONE;
        move_pos.x = 0;
        move_pos.y = 0;
        if (sidebar.stop == false) {
            return true;
        }
        css_left = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_SIDEBAR));
        css_top = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT));
        css_bottom = ui_core_get_element_css(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT));
        if ((css_top->top == -css_top->height) && (css_bottom->top == css_bottom->height) && ((css_left->left == -css_left->width))) {
            sidebar.sidebar_status = SIDEBAR_HIDE;
        }
        sidebar.stop = true;
        sidebar.step = 0;
        return true;

    case ELM_EVENT_TOUCH_MOVE:
        if (sidebar_root == 0) {
            break;
        }

#ifdef SIDEBAR_X_EN
        sidebar.x_offset = e->pos.x - sidebar.pos.x;
        memcpy(&sidebar.pos, &e->pos, sizeof(struct position));
        if (sidebar.stop) {
            ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
        }
#else
#ifdef SIDEBAR_X_Y_EN
        int x_move_ret = 0;
        sidebar.x_offset = e->pos.x - sidebar.pos.x;
        sidebar.y_offset = e->pos.y - sidebar.pos.y;
        move_pos.x += sidebar.x_offset;
        move_pos.y += sidebar.y_offset;
        if ((move_dir == SLIDER_MOVE_NONE) && (abs(move_pos.x) > DIAL_SIDEBAR_START_THRESHOLD) || (abs(move_pos.y) > DIAL_SIDEBAR_START_THRESHOLD)) {
            move_dir = (abs(move_pos.x) > abs(move_pos.y)) ? SLIDER_MOVE_LF : SLIDER_MOVE_UD;
        }
        memcpy(&sidebar.pos, &e->pos, sizeof(struct position));
        if (move_dir == SLIDER_MOVE_LF) {
            if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_x_offset <= DIAL_SIDEBAR_LEFT_MIN_THRESHOLD)) {
                sidebar.stop = false;
                x_move_ret = ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
            } else if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_x_offset >= DIAL_SIDEBAR_LEFT_MAX_THRESHOLD)) {
                sidebar.stop = false;
                x_move_ret = ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_LEFT) {
                sidebar.stop = false;
                x_move_ret = ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
            } else {
                sidebar.stop = false;
            }
            /* printf("status=%d xoffset=%d fxoffset=%d moveret=%d MIN=%d MAX=%d", \ */
            /* sidebar.sidebar_status, sidebar.x_offset, sidebar.first_x_offset, x_move_ret, DIAL_SIDEBAR_LEFT_MIN_THRESHOLD, DIAL_SIDEBAR_LEFT_MAX_THRESHOLD); */
        }
#else

        sidebar.y_offset = e->pos.y - sidebar.pos.y;
        memcpy(&sidebar.pos, &e->pos, sizeof(struct position));
        move_dir = SLIDER_MOVE_UD;
#endif

        if (move_dir == SLIDER_MOVE_UD) {
            if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_y_offset <= DIAL_SIDEBAR_MIN_THRESHOLD)) {
                sidebar.stop = false;
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), sidebar.y_offset, 1);
            } else if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_y_offset >= DIAL_SIDEBAR_MAX_THRESHOLD)) {
                sidebar.stop = false;
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), sidebar.y_offset, 0);
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_TOP) {
                sidebar.stop = false;
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), sidebar.y_offset, 1);
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_BOTTOM) {
                sidebar.stop = false;
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), sidebar.y_offset, 0);
            }
            /* printf("dir=%doffset=%d status=%d fyoffset=%d MIN=%d MAX=%d", move_dir, sidebar.y_offset, sidebar.sidebar_status, sidebar.first_y_offset, DIAL_SIDEBAR_MIN_THRESHOLD, DIAL_SIDEBAR_MAX_THRESHOLD); */
        }
#endif
        break;

    case ELM_EVENT_TOUCH_HOLD:
        ui_send_event(KEY_CHANGE_PAGE, BIT(31) | PAGE_16);
        return true;

    case ELM_EVENT_TOUCH_UP:
        log_info("ELM_EVENT_TOUCH_UP\n");
        if (sidebar_root == 0) {
            return true;
        }
#ifdef SIDEBAR_X_EN
        if (abs(e->pos.x - sidebar.first_x_offset) > 0) {
            if (e->pos.x > sidebar.first_x_offset) {
                sidebar.step = 20;
            } else {
                sidebar.step = -20;
            }
            sidebar.stop = false;
        } else if (abs(e->pos.x - sidebar.first_x_offset) == 0) {
            sidebar.step = -20;
            sidebar.stop = false;
        }
#else
#ifdef SIDEBAR_X_Y_EN
        if (move_dir == SLIDER_MOVE_LF) {
            if (sidebar.sidebar_status == SIDEBAR_SHOWING_LEFT) {
                return true;
            }
            sidebar.x_offset = e->pos.x - sidebar.pos.x;
            /* printf("x_offset=%d fxoffset=%d", sidebar.x_offset, sidebar.first_x_offset); */
            memcpy(&sidebar.pos, &e->pos, sizeof(struct position));
            if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_x_offset <= DIAL_SIDEBAR_LEFT_MIN_THRESHOLD)) {
                ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
            } else if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_y_offset >= DIAL_SIDEBAR_LEFT_MAX_THRESHOLD)) {
                ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_LEFT) {
                ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.x_offset);
            }
            ui_core_get_element_abs_rect(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), &rect);
            if (sidebar.sidebar_status == SIDEBAR_HIDE) {//表盘显示状态
                //如果右边界大于最小滑动偏移量，则完全滑出侧边栏
                if (rect.left + rect.width >= DIAL_SIDEBAR_LEFT_RUN_MIN_THRESHOLD) {
                    sidebar.step = DIAL_SIDEBAR_STEP;
                    sidebar.stop = false;
                    sidebar.sidebar_status = SIDEBAR_SHOWING_LEFT;
                } else {//否则收回
                    sidebar.step = -DIAL_SIDEBAR_STEP;
                    sidebar.stop = false;
                    sidebar.sidebar_status = SIDEBAR_SHOWING_LEFT;
                }
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_LEFT) {//左侧边栏显示状态
                //如果右边界小于最大滑动偏移量，则完全收回侧边栏
                if (rect.left + rect.width <= DIAL_SIDEBAR_LEFT_RUN_MAX_THRESHOLD) {
                    sidebar.step = -DIAL_SIDEBAR_STEP;
                    sidebar.stop = false;
                    sidebar.sidebar_status = SIDEBAR_SHOWING_LEFT;
                } else {
                    sidebar.step = DIAL_SIDEBAR_STEP;
                    sidebar.stop = false;
                    sidebar.sidebar_status = SIDEBAR_SHOWING_LEFT;
                }
            }
        }
#endif
        if (move_dir == SLIDER_MOVE_UD) {
            if ((sidebar.sidebar_status == SIDEBAR_SHOWING_TOP) || (sidebar.sidebar_status == SIDEBAR_SHOWING_BOTTOM)) {
                return true;
            }

            sidebar.y_offset = e->pos.y - sidebar.pos.y;
            memcpy(&sidebar.pos, &e->pos, sizeof(struct position));
            /* printf("offset=%d status=%d fyoffset=%d MIN=%d MAX=%d", sidebar.y_offset, sidebar.sidebar_status, sidebar.first_y_offset, DIAL_SIDEBAR_MIN_THRESHOLD, DIAL_SIDEBAR_MAX_THRESHOLD); */
            if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_y_offset <= DIAL_SIDEBAR_MIN_THRESHOLD)) {
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), sidebar.y_offset, 1);
            } else if ((sidebar.sidebar_status == SIDEBAR_HIDE) && (sidebar.first_y_offset >= DIAL_SIDEBAR_MAX_THRESHOLD)) {
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), sidebar.y_offset, 0);
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_TOP) {
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), sidebar.y_offset, 1);
            } else if (sidebar.sidebar_status == SIDEBAR_SHOW_BOTTOM) {
                ui_y_move(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), sidebar.y_offset, 0);
            }
            if (1) {
                if (sidebar.sidebar_status == SIDEBAR_HIDE) {
                    if (sidebar.first_y_offset <= DIAL_SIDEBAR_MIN_THRESHOLD) {
                        ui_core_get_element_abs_rect(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), &rect);
                        if (rect.top + rect.height >= DIAL_SIDEBAR_RUN_MIN_THRESHOLD) {
                            sidebar.step = DIAL_SIDEBAR_STEP;
                            sidebar.stop = false;
                            sidebar.sidebar_status = SIDEBAR_SHOWING_TOP;
                        } else {
                            sidebar.step = -DIAL_SIDEBAR_STEP;
                            sidebar.stop = false;
                            sidebar.sidebar_status = SIDEBAR_SHOWING_TOP;
                        }
                    } else if (sidebar.first_y_offset >= DIAL_SIDEBAR_MAX_THRESHOLD) {
                        ui_core_get_element_abs_rect(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), &rect);
                        if (rect.top <= DIAL_SIDEBAR_RUN_MAX_THRESHOLD) {
                            sidebar.step = -DIAL_SIDEBAR_STEP;
                            sidebar.stop = false;
                            sidebar.sidebar_status = SIDEBAR_SHOWING_BOTTOM;
                        } else {
                            sidebar.step = DIAL_SIDEBAR_STEP;
                            sidebar.stop = false;
                            sidebar.sidebar_status = SIDEBAR_SHOWING_BOTTOM;
                        }
                    }
                } else if (sidebar.sidebar_status == SIDEBAR_SHOW_TOP) {
                    ui_core_get_element_abs_rect(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), &rect);
                    if (rect.top + rect.height <= DIAL_SIDEBAR_RUN_MAX_THRESHOLD) {
                        sidebar.step = -DIAL_SIDEBAR_STEP;
                        sidebar.stop = false;
                        sidebar.sidebar_status = SIDEBAR_SHOWING_TOP;
                    } else {
                        sidebar.step = DIAL_SIDEBAR_STEP;
                        sidebar.stop = false;
                        sidebar.sidebar_status = SIDEBAR_SHOWING_TOP;
                    }
                } else if (sidebar.sidebar_status == SIDEBAR_SHOW_BOTTOM) {
                    ui_core_get_element_abs_rect(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), &rect);
                    if (rect.top >= DIAL_SIDEBAR_MIN_THRESHOLD) {
                        sidebar.step = DIAL_SIDEBAR_STEP;
                        sidebar.stop = false;
                        sidebar.sidebar_status = SIDEBAR_SHOWING_BOTTOM;
                    } else {
                        sidebar.step = -DIAL_SIDEBAR_STEP;
                        sidebar.stop = false;
                        sidebar.sidebar_status = SIDEBAR_SHOWING_BOTTOM;
                    }
                }
            }
        }
#endif
        return true;
    default:
        break;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(STYLE_DIAL_ID(WATCH))
.onchange = WATCH_onchange,
 .onkey = NULL,
  .ontouch = WATCH_ontouch,
};


#if TCFG_PSRAM_UI_EFFECT && UI_WATCH_EFFECT

static int WATCH_LAYER_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layer *layer = (struct layer *)ctr;
    struct element *win_elm = &layer->elm;
    static u8 group_init = 0;
    u8 group1 = 1, group2 = 2;

    switch (e) {
    case ON_CHANGE_INIT:

#if 1
        redraw_sal.sca = 1;
        redraw_sal.alpha = 255;

        redraw_part.reflesh = 1;
        redraw_part.part1_effect_mode = EFFECT_MODE_SCA_ALPHA;
        redraw_part.part1_effect_priv = &redraw_sal;
        redraw_part.part1.group = group1;
        redraw_part.part1.bg_addr = NULL;

        redraw_part.part2.group = group2;
        redraw_part.part2.bg_addr = NULL;
        ui_window_effect(win_elm->id, EFFECT_MODE_REDRAW_PART, NULL, &redraw_part);
#endif
        /* redraw_sal.sca = 0.8; */
        /* redraw_sal.alpha = 255; */
        /* ui_window_effect(win_elm->id, EFFECT_MODE_SCA_ALPHA, NULL, &redraw_sal); */

        break;
    case ON_CHANGE_SHOW:
        if (group_init == 0) {
            group_init = 1;

            ui_core_set_group(ui_core_get_element_by_id(DIAL_BASEFORM_2), group1, 0);
            ui_core_set_group(ui_core_get_element_by_id(DIAL_WATCH_LAYOUT), group1, 1);

#ifdef SIDEBAR_X_Y_EN
            ui_core_set_group(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), group2, 1);
#endif
            ui_core_set_group(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), group2, 1);
            ui_core_set_group(ui_core_get_element_by_id(SIDEBAR_UNDISTURB_LAYOUT), group2, 1);
            ui_core_set_group(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), group2, 1);
        }

        break;
    case ON_CHANGE_RELEASE:
        group_init = 0;
        ui_window_stop_redraw(0);
        break;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(DIAL_BASEFORM_2)
.onchange = WATCH_LAYER_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif



static void SIDEBAR_timer(void *priv)
{
    if (!sidebar.timer) {
        return ;
    }
    /* log_info("stop:%d, step:%d, sidebar_status:%d\n", sidebar.stop, sidebar.step, sidebar.sidebar_status); */
    if (!sidebar.stop && sidebar.step) {
#ifdef SIDEBAR_X_EN
        if (!ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.step)) {
            sidebar.stop = true;
        }
#else
#ifdef SIDEBAR_X_Y_EN
        if (sidebar.sidebar_status == SIDEBAR_SHOWING_LEFT) {
            if (!ui_x_move(ui_core_get_element_by_id(SIDEBAR_SIDEBAR), sidebar.step)) {
                if (sidebar.step > 0) {
                    sidebar.sidebar_status = SIDEBAR_SHOW_LEFT;
                } else if (sidebar.step < 0) {
                    sidebar.sidebar_status = SIDEBAR_HIDE;
                }
                sidebar.stop = true;
                sidebar.step = 0;
            }

        }
#endif
        if (sidebar.sidebar_status == SIDEBAR_SHOWING_TOP) {
            if (!ui_y_move(ui_core_get_element_by_id(SIDEBAR_PULLDONW_MENU_LAYOUT), sidebar.step, 1)) {
                if (sidebar.step > 0) {
                    sidebar.sidebar_status = SIDEBAR_SHOW_TOP;
                } else if (sidebar.step < 0) {
                    sidebar.sidebar_status = SIDEBAR_HIDE;
                }
                sidebar.stop = true;
                sidebar.step = 0;
            }
        } else if (sidebar.sidebar_status == SIDEBAR_SHOWING_BOTTOM) {
            if (!ui_y_move(ui_core_get_element_by_id(SIDEBAR_NOTICE_LAYOUT), sidebar.step, 0)) {
                if (sidebar.step > 0) {
                    sidebar.sidebar_status = SIDEBAR_HIDE;
                } else if (sidebar.step < 0) {
                    sidebar.sidebar_status = SIDEBAR_SHOW_BOTTOM;
                }
                sidebar.stop = true;
                sidebar.step = 0;
            }
        }
#endif
    }
}

#if TCFG_UI_ENABLE_PULLDOWN_MENU
static int SIDEBAR_TOP_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;
    struct element_css *css;

    switch (e) {
    case ON_CHANGE_INIT:
        css = ui_core_get_element_css(&layout->elm);
        /* log_info("%d, %d, %d, %d\n", css->left, css->top, css->width, css->height); */
#ifdef SIDEBAR_X_EN
        css->left = -css->width;
#else

#if TCFG_UI_ENABLE_PULLDOWN_MENU
        extern u8 get_undisturb_ensure_release_flag();
        extern void set_undistrub_ensure_release_flag(u8 flag);
        /* r_log_info("undisturb_ensure_release_flag:%d", get_undisturb_ensure_release_flag()); */
        if (!get_undisturb_ensure_release_flag()) {
            css->top = -css->height;
        } else {
            sidebar.sidebar_status = SIDEBAR_SHOW_TOP;
        }
        set_undistrub_ensure_release_flag(0);
#endif /* #if TCFG_UI_ENABLE_PULLDOWN_MENU */
#endif
        if (!sidebar.timer) {
            sidebar.timer = sys_timer_add(NULL, SIDEBAR_timer, 50);
        }
        sidebar.stop = true;
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        sidebar.sidebar_status = SIDEBAR_HIDE;
        if (sidebar.timer) {
            sys_timer_del(sidebar.timer);
            sidebar.timer = 0;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE;
}

#ifdef SIDEBAR_X_EN
REGISTER_UI_EVENT_HANDLER(SIDEBAR_SIDEBAR)
#else
REGISTER_UI_EVENT_HANDLER(SIDEBAR_PULLDONW_MENU_LAYOUT)
#endif
.onchange = SIDEBAR_TOP_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif /* #if TCFG_UI_ENABLE_PULLDOWN_MENU */
#ifdef SIDEBAR_X_Y_EN
static int SIDEBAR_LEFT_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;
    struct element_css *css;

    switch (e) {
    case ON_CHANGE_INIT:
        css = ui_core_get_element_css(&layout->elm);
        log_info("%d, %d, %d, %d\n", css->left, css->top, css->width, css->height);
        css->left = -css->width;
        if (!sidebar.timer) {
            sidebar.timer = sys_timer_add(NULL, SIDEBAR_timer, 50);
        }
        sidebar.stop = true;
        break;
    case ON_CHANGE_SHOW:
        css = ui_core_get_element_css(&layout->elm);
        break;
    case ON_CHANGE_RELEASE:
        sidebar.sidebar_status = SIDEBAR_HIDE;
        if (sidebar.timer) {
            sys_timer_del(sidebar.timer);
            sidebar.timer = 0;
        }
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_SIDEBAR)
.onchange = SIDEBAR_LEFT_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#endif
#if TCFG_UI_ENABLE_NOTICE
extern u8 is_create_control_by_menu();
static int SIDEBAR_BOTTOM_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;
    struct element_css *css;

#if TCFG_UI_ENABLE_NOTICE
    if (is_create_control_by_menu()) {
        return FALSE;
    }
#endif /* #if TCFG_UI_ENABLE_NOTICE */

    switch (e) {
    case ON_CHANGE_INIT:
        css = ui_core_get_element_css(&layout->elm);
        /* r_log_info("%d, %d, %d, %d\n", css->left, css->top, css->width, css->height); */
        css->top = css->height;
        sidebar.stop = true;
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        sidebar.sidebar_status = SIDEBAR_HIDE;
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(SIDEBAR_NOTICE_LAYOUT)
.onchange = SIDEBAR_BOTTOM_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#endif /* #if TCFG_UI_ENABLE_NOTICE */
static int PAGE_switch_ontouch(void *_layout, struct element_touch_event *e)
{
    struct layout *layout = (struct layout *)_layout;

    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
        /* ui_return_page_pop(1); */
        return false;
    case ELM_EVENT_TOUCH_L_MOVE:
        /* ui_return_page_pop(0); */
        return false;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(STYLE_DIAL_ID(PAGE_0))
.onchange = NULL,
 .onkey = NULL,
  .ontouch = PAGE_switch_ontouch,
};


// test
#if 0

extern const u8 icon_48x49_rgb565[4704];
void draw_rect_1(u8 *pdispbuf, struct rect *rect, struct rect *draw_r, struct rect *rectangle, u16 color);
static void ui_draw_cb(int id, u8 *dst_buf, struct rect *dst_r, struct rect *src_r, u8 bytes_per_pixel, void *priv)
{
    int w, h;
    struct rect r;
    int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
    int src_stride = (src_r->width * bytes_per_pixel + 3) / 4 * 4;
    if (id == 123) {
        u8 *src_buf = icon_48x49_rgb565;
        if (get_rect_cover(dst_r, src_r, &r)) {
            for (h = 0; h < r.height; h++) {
                memcpy(&dst_buf[(r.top + h - dst_r->top) * dst_stride + (r.left - dst_r->left) * bytes_per_pixel],
                       &src_buf[(r.top + h - src_r->top)*src_stride + (r.left - src_r->left) * bytes_per_pixel],
                       r.width * bytes_per_pixel);
            }
        }
    } else if (id == 124) {
        if (get_rect_cover(dst_r, src_r, &r)) {
            draw_rect_1(dst_buf, dst_r, &r, src_r, 0xf800);
        }
    } else if (id == 125) {
        if (get_rect_cover(dst_r, src_r, &r)) {
            draw_rect_1(dst_buf, dst_r, &r, src_r, 0x07e0);
        }
    }
}


static const unsigned char s_wave_data1[] = {
    50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
    75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75, 75,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
    150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150, 150,
    175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175, 175,
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
    200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
    225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225, 225,
    250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
    250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250, 250,
};


#define TEXT_POINT 12334

static c_wave_ctrl *p_wave;

static void refresh_test(struct rect *rect)
{
    int i = 0;
    int k = 0;
    int frame_len = 0;

    frame_len = TEXT_POINT / (rect->width);

    int offset_len = TEXT_POINT / sizeof(s_wave_data1);

    p_wave->m_frame_len = frame_len;

    for (i = 0; i < TEXT_POINT; i += frame_len) {
        for (int j = 0; j < frame_len; j++) {
            p_wave->write_wave_data(p_wave->m_wave, s_wave_data1[((i + j) / offset_len) % sizeof(s_wave_data1)]);
        }
        if (k <= rect->width - 10) {
            p_wave->refresh_wave(p_wave, k++);
        }
    }

}







static void *draw_test_cb(void *priv)
{
    struct draw_context dc = {0};
    struct imb_task_out *out = (struct imb_task_out *)priv;
    struct imb_task *task = out->task;
    /* struct draw_context *dc = (struct draw_context *)out->priv; */
    if (out->format == OUTPUT_FORMAT_RGB888) {
        task->cur_format = LAYER_FORMAT_RGB888;
    } else if (out->format == OUTPUT_FORMAT_RGB565) {
        task->cur_format = LAYER_FORMAT_RGB565;
    } else {
        ASSERT(0, "curr format is not support!\n");
    }
    task->x_offset = 0;
    task->y_offset = 0;
    task->src_w = out->rect.width;
    task->src_h = out->rect.height;
    task->des_w = out->rect.width;
    task->des_h = out->rect.height;
    task->dat_src_adr = (u32)out->outbuf;
    task->quote = 1;

    struct rect draw = {0};
    draw.left = 0;
    draw.top = 0;
    draw.width = out->rect.width;
    draw.height = out->rect.height;
    //log_info(">>>>>>>>>>>>>>>>>>>>>%d,%d, w=%d,h=%d line =%d \n",out->rect.left,out->rect.top,out->rect.width,out->rect.height,__LINE__);
    draw_rgb565_on_sram(&out->rect, &draw, out->outbuf, icon_48x49_rgb565, 0, 0, 240, 240); //不带alpha的显示


    return NULL;
    dc.buf = out->outbuf;
    dc.len = out->rect.width * out->rect.height * 2;
    memcpy(&dc.rect, &out->rect, sizeof(struct rect));
    memcpy(&dc.draw, &out->rect, sizeof(struct rect));
    memcpy(&dc.disp, &out->rect, sizeof(struct rect));


    struct rect rect = {0};
    rect.left = 20;
    rect.top = 20;
    rect.width  = 200;
    rect.height  = 200;
    //画圆(顺序针方向,12点钟方向为0度)
    ui_draw_circle(&dc, 0 + 91, 0 + 91, 80, 85, 225, 225 + 360, 0x07c0, 75);

    //ui_draw_line(&dc, rect.left, rect.top, rect.left + rect.width, rect.top + rect.height, 0xffff);
    //ui_draw_line(&dc, rect.left, rect.top + rect.height, rect.left + rect.width, rect.top, 0xffff);
    /* ui_draw_line_by_angle(&dc, rect.left + 100, rect.top + 100, 100, 0, 0xffff); */
    /* ui_draw_line_by_angle(&dc, rect.left + 100, rect.top + 100, 100, 90, 0xffff); */
    /* ui_draw_line_by_angle(&dc, rect.left + 100, rect.top + 100, 100, 180, 0xffff); */
    /* ui_draw_line_by_angle(&dc, rect.left + 100, rect.top + 100, 100, 270, 0xffff); */



    draw_smooth_vline_redraw(p_wave, &dc, 0);
    return NULL;
}


void br28_draw_in_sram(struct draw_context *dc, void *(*cb)(void *priv), void *priv);
#if 0
static int DRAW_TEST_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;
    struct draw_context *dc = (struct draw_context *)arg;

    struct rect rect = {0};
    switch (e) {
    case ON_CHANGE_INIT:

        ui_core_get_element_abs_rect(&layout->elm, &rect); //

        rect.left = rect.left + 12;
        rect.width = rect.width - 25;
        rect.height = rect.height - 55;
        rect.top  = rect.top + 10;

        log_info(">>>>>>>>>>>>>>%s %d ,%d,%d,%d,%d\n", __FUNCTION__, __LINE__, rect.left, rect.width, rect.height, rect.top);

        p_wave =  c_wave_ctrl_create(&rect);
        p_wave->set_wave_speed(p_wave, 1);
        p_wave->set_wave_color(p_wave, GL_RGB(255, 255, 0));
        p_wave->set_wave_in_out_rate(p_wave, 180, 100);
        p_wave->set_max_min(p_wave, 256, 00);
        refresh_test(&rect);

        break;
    case ON_CHANGE_SHOW:
        ui_remove_backcolor(&layout->elm);
        break;
    case ON_CHANGE_SHOW_POST:
        ui_draw(dc, NULL, 96, 96, 48, 49, ui_draw_cb, NULL, 0, 123);
        ui_draw(dc, NULL, 96, 40, 48, 49, ui_draw_cb, NULL, 0, 124);
        ui_draw(dc, NULL, 1, 1, 238, 238, ui_draw_cb, NULL, 0, 125);
        ui_draw(dc, icon_48x49_rgb565, 42, 96, 48, 49, NULL, NULL, 0, 0);
        ui_draw(dc, icon_48x49_rgb565, 150, 96, 48, 49, NULL, NULL, 0, 0);
        break;
    case ON_CHANGE_RELEASE:
        c_wave_ctrl_release(&p_wave);
        break;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(STYLE_DIAL_ID(DRAW_TEST))
.onchange = DRAW_TEST_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
#endif /* #if 0 */

#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

