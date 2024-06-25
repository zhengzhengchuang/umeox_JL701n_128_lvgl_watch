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
#include "sport_data/sport.h"
#include "sport_data/watch_common.h"
#include "sport/sport_api.h"

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_TRAIN

#define STYLE_NAME  JL

struct target_info {
    u8 type: 7;
    u8 user: 1;
    u8 value;
};

struct grid_set_info {
    u8  sel_item;
    u8  sport_set_index;//选择设置的运动类型
    u8  sport_index;
    u8  set_index;//选择目录、提醒
    u8  target_index;//目录的设置类型
    u8  user_target_index;//是否支持了自定义
    struct target_info target[13];//所有运动的目标消息，要和vm 挂钩
    u16 sport_common_timer;
    int prev_id;//最后一下页面的信息
    int current_id;//最后一下页面的信息
};

struct grid_set_info  info;// = NULL;
#define __this (&info)
#define sizeof_this   (sizeof(struct grid_set_info))

#define UI_NUMBER10(x) ((x)/10+M385,(x)%10+M385)

#define _RGB565(r,g,b)  (u16)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3))
#define UI_RGB565(c)  \
        _RGB565((c>>16)&0xff,(c>>8)&0xff,c&0xff)

#define DISTANCE_TARGET (1)
#define TIME_TARGET     (2)
#define HEAT_TARGET     (3)
#define NONE_TARGET     (0xff)

static u16 T_Main_ItemString[13][5] = {
    {M371, M385, M385, M372, 0}, //0恢复时间00小时
    {M386, M388, M414, 0}, //1  //13门课程
    {M385, M386, M395, 0}, //2
    {M385, M387, M395, 0}, //3
    {M385, M388, M395, 0}, //4
    {M385, M389, M395, 0}, //5
    {M385, M390, M395, 0}, //6
    {M385, M391, M395, 0}, //7
    {M385, M392, M395, 0}, //8
    {M385, M393, M395, 0}, //9
    {M385, M394, M395, 0}, //10
    {M391, M385, M395, 0}, //11
    {M391, M386, M395, 0}, //12
};


static const u16 T_SET_title_ItemString[] = {
    M419,//
    M420,//
    M421,//
    M422,//
    M423,//
    M424,//
    M425,//
    M426,//
    M427,//
    M428,//
    M429,//
};



static const u16 T_target_title_ItemString[] = {
    M401,//目标
    M430,//距离
    M431,//时间
    M432,//热量
    M433,//没有目标
};

static const u16 T_target_title_ItemPIC[] = {
    0,
    PAGE54_8f5d_ICON_86,
    PAGE54_e018_ICON_87,
    PAGE54_b6a9_ICON_88,
    PAGE54_d9ec_ICON_89,
};


static const u16 T_DISTANCE_ItemString[][4] = {
    {M435, M445, 0},//1公里
    {M437, M445, 0},//3公里
    {M439, M445, 0},//5公里
    {M435, M434, M445, 0},//10公里
    {M449, 0},//半马
    {M450, 0},//全马
    {M447, 0},//自定义
};


static const u16 N_DISTANCE_Itemvalue[] = {
    1,
    3,
    5,
    10,
    21,
    42,
};

static const u16 T_Big_number_ItemString[] = {
    M434,//0
    M435,//1
    M436,//2
    M437,//3
    M438,//4
    M439,//5
    M440,//6
    M441,//7
    M442,//8
    M443,//9
    M445,//公里
    M444,//分钟
    M446,//千卡
};


static const u16 T_Small_number_ItemString[] = {
    M385,//0
    M386,//1
    M387,//2
    M388,//3
    M389,//4
    M390,//5
    M391,//6
    M392,//7
    M393,//8
    M394,//9
    M396,//公里
    M395,//分钟
    M397,//千卡
};



const int __T_USER_DISTANCE_ItemString(int number, u16 *buf, u16 size, u8 big)
{
    u8 temp[10];
    snprintf(temp, size, "%d", number);
    int i = 0;
    for (i = 0; i < strlen(temp); i++) {
        if (big) {
            buf[i] =  T_Big_number_ItemString[temp[i] - '0'];
        } else {
            buf[i] =  T_Small_number_ItemString[temp[i] - '0'];
        }
    }

    if (big) {
        buf[i++] = T_Big_number_ItemString[10];
    } else {
        buf[i++] = T_Small_number_ItemString[10];
    }
    buf[i++] = 0;
    return i;
}

const int __T_USER_TIME_ItemString(int number, u16 *buf, u16 size, u8 big)
{
    u8 temp[10];
    snprintf(temp, size, "%d", number);
    int i = 0;
    for (i = 0; i < strlen(temp); i++) {
        if (big) {
            buf[i] =  T_Big_number_ItemString[temp[i] - '0'];
        } else {
            buf[i] =  T_Small_number_ItemString[temp[i] - '0'];
        }
    }
    if (big) {
        buf[i++] = T_Big_number_ItemString[11];
    } else {
        buf[i++] = T_Small_number_ItemString[11];
    }
    buf[i++] = 0;
    return i;
}


const int __T_USER_HEAT_ItemString(int number, u16 *buf, u16 size, u8 big)
{
    u8 temp[10];
    snprintf(temp, size, "%d", number);
    int i = 0;
    for (i = 0; i < strlen(temp); i++) {
        if (big) {
            buf[i] =  T_Big_number_ItemString[temp[i] - '0'];
        } else {
            buf[i] =  T_Small_number_ItemString[temp[i] - '0'];
        }
    }
    if (big) {
        buf[i++] = T_Big_number_ItemString[12];
    } else {
        buf[i++] = T_Small_number_ItemString[12];
    }
    buf[i++] = 0;
    return i;
}


static const u16 T_TIME_ItemString[][5] = {
    {M435, M434, M444, 0}, //10分钟
    {M436, M434, M444, 0}, //20分钟
    {M437, M434, M444, 0}, //30分钟
    {M440, M434, M444, 0}, //60分钟
    {M435, M436, M434, M444, 0},//120分钟
    {M435, M442, M434, M444, 0},//180分钟
    {M447, 0},//自定义
};



static const u16 T_TIME_Itemvalue[] = {
    10,
    20,
    30,
    60,
    120,
    180,
};

static const u16 T_HEAT_ItemString[][5] = {
    {M435, M434, M434, M446, 0},//100千卡
    {M436, M434, M434, M446, 0},//200千卡
    {M437, M434, M434, M446, 0},//300千卡
    {M439, M434, M434, M446, 0},//500千卡
    {M440, M434, M434, M446, 0},//600千卡
    {M442, M434, M434, M446, 0},//800千卡
    {M447, 0},//自定义
};


static const u16 N_HEAT_Itemvalue[] = {
    100,
    200,
    300,
    500,
    600,
    800,
};

static const int __T_get_terget_ItemString(int index, u16 *buf, u16 size)
{
    int value = __this->target[index].value;
    switch (__this->target[index].type) {
    case DISTANCE_TARGET:
        if (__this->target[index].user) {
            return __T_USER_DISTANCE_ItemString(1 + value * 1, buf, size, 0);
        } else {
            return __T_USER_DISTANCE_ItemString(N_DISTANCE_Itemvalue[value], buf, size, 0);
        }
        break;
    case TIME_TARGET:
        if (__this->target[index].user) {
            return __T_USER_TIME_ItemString(10 + value * 5, buf, size, 0);
        } else {
            return __T_USER_TIME_ItemString(T_TIME_Itemvalue[value], buf, size, 0);
        }
        break;
    case HEAT_TARGET:
        if (__this->target[index].user) {
            return __T_USER_HEAT_ItemString(100 + value * 50, buf, size, 0);
        } else {
            return __T_USER_HEAT_ItemString(N_HEAT_Itemvalue[value], buf, size, 0);
        }
        break;
    default:
        buf[0] = M399;//无目标
        buf[1] = 0;//无目标
        break;
    }
    return 0;
}


struct TARGET_MENULIST {
    u8    type;
    u16   *ItemString;   //对应的字符串ID号
    u16   *ItemValue;    //字符串值
};

/* struct TARGET_MENULIST target_table[] */
/* { */
/*     {}, */
/*     {}, */
/*     {}, */
/*     {}, */
/* }; */
/*  */
static int grid_child_cb(int parent, void *_ctrl, int id, int type, int index)
{
    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        switch (parent) {
        case V_TRAIN_LIST:
            if (!strcmp(text->source, "content") && index < ARRAY_SIZE(T_Main_ItemString)) {
                if (index < 2) {
                    text->attrs.str = (char *)(T_Main_ItemString[index]);
                } else {
                    __T_get_terget_ItemString(index - 2, T_Main_ItemString[index], 5);
                    text->attrs.str = (char *)(T_Main_ItemString[index]);
                }
                text->attrs.format = "mulstr";
            }
            break;

        case VLIST_TRAIN_SET_COMMON_LIST:
            if (!strcmp(text->source, "content") && !index) {
                u8 sel =  __this->sport_set_index;
                if (sel < ARRAY_SIZE(T_SET_title_ItemString)) {
                    text->attrs.str = (char *)(&T_SET_title_ItemString[sel]);
                    text->attrs.format = "strpic";
                }
            } else if (!strcmp(text->source, "content")) {
                __T_get_terget_ItemString(__this->sport_set_index, T_Main_ItemString[2], 5);
                //这里借用了T_Main_ItemString[2]的空间
                text->attrs.str = (char *)(T_Main_ItemString[2]);
                text->attrs.format = "mulstr";
            }
            break;
        }
        break;

    case CTRL_TYPE_NUMBER:
        struct ui_number *number = (struct ui_number *)_ctrl;
        number->type = TYPE_STRING;
        break;
    case CTRL_TYPE_TIME:
        break;
    case CTRL_TYPE_PIC:
        struct ui_pic *pic = (struct ui_pic *)_ctrl;

        break;
    }
    return 0;
}




static int grid_children_init(struct ui_grid *grid)
{
    struct element *k;
    if (!grid) {
        return 0;
    }
    int parent = grid->elm.id;
    for (int i = 0; i < grid->avail_item_num; i++) {
        list_for_each_child_element(k, &grid->item[i].elm) {
            grid_child_cb(parent, k, k->id, ui_id2type(k->id), i);
        }
    }
    return 0;
}


static int  __muenu_in(u8 index)
{
    int ret = false;
    switch (index) {
    case 0:
        break;
    case 1:
        break;

    default:
        ui_hide(ID_WINDOW_TRAIN);
        __this->prev_id = ID_WINDOW_TRAIN;
        UI_SHOW_WINDOW(ID_WINDOW_SPORT_SHOW);
        __this->current_id = ID_WINDOW_SPORT_SHOW;
        ret = true;
        break;
    }
    __this->sel_item = index;
    return ret;
}


static int TRAIN_VERTLIST_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(grid, NULL, NULL, NULL);
        break;
    case ON_CHANGE_FIRST_SHOW:
        grid_children_init(grid);
        break;
    default:
        break;
    }
    return false;
}

static int TARIN_VERTLIST_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
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
        if (sel_item == 1) {
            ui_send_event(KEY_CHANGE_PAGE, BIT(31) | ID_WINDOW_SPORT_COURSE);
        }
        set_sport_mode(sel_item - 1);
        struct element *elm = &grid->item[sel_item].elm;
        log_info("sel_item is %d", sel_item);
        list_for_each_child_element_reverse(p, n, elm) {
            if (!p->css.invisible) {
                ui_core_get_element_abs_rect(p, &r);
                if (in_rect(&r, &e->pos)) {
                    u8 count = 0;
                    switch (p->id) {
                    case V12_TRAIN_PIC:
                        count++;
                    case V11_TRAIN_PIC:
                        count++;
                    case V10_TRAIN_PIC:
                        count++;
                    case V9_TRAIN_PIC:
                        count++;
                    case V8_TRAIN_PIC:
                        count++;
                    case V7_TRAIN_PIC:
                        count++;
                    case V6_TRAIN_PIC:
                        count++;
                    case V5_TRAIN_PIC:
                        count++;
                    case V4_TRAIN_PIC:
                        count++;
                    case V3_TRAIN_PIC:
                        count++;
                    case V2_TRAIN_PIC:
                        __this->sport_set_index = count;
                        ui_hide(V_TRAIN_LAYOUT);
                        ui_show(V_TRAIN_LAYOUT_2);
                        __this->prev_id = V_TRAIN_LAYOUT;
                        __this->current_id = V_TRAIN_LAYOUT_2;
                        return true;
                        break;

                    }
                    break;
                }
            }
        }
        printf("sel_item : %d\n", sel_item);
        return __muenu_in(sel_item);
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

REGISTER_UI_EVENT_HANDLER(V_TRAIN_LIST)
.ontouch = TARIN_VERTLIST_ontouch,
 .onkey = NULL,
  .onchange  = TRAIN_VERTLIST_onchange,
};



int SPORT_COMMON_LAYOUT_child_cb(void *_ctrl, int id, int type)
{
    struct watch_heart_rate __heartrate_hd;//心率为0显示--
    watch_heart_rate_handle_get(&__heartrate_hd);
    int heart = __heartrate_hd.heart_rate_data_get(LAST_DATA, NULL);
    switch (type) {
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->source, "heat")) {
            static char heat[4];
            sprintf(heat, "%3d", heart);
            ui_text_set_text_attrs(text, heat, strlen(heat), FONT_ENCODE_ANSI, 0, 0);
        }
        break;
    case CTRL_TYPE_NUMBER:
        break;
    case CTRL_TYPE_TIME:
        break;
    case CTRL_TYPE_LAYOUT:
        break;
    }

    return 0;
}



static void SPORT_COMMON_LAYOUT_timer(void *priv)
{
    if (!__this->sport_common_timer) {
        return ;
    }
    ui_get_child_by_id(SPORT_COMMON_LAYOUT, SPORT_COMMON_LAYOUT_child_cb);

    struct element *elm = NULL;
    ui_update_source_by_elm(elm, 0);
    elm = ui_core_get_element_by_id(SPORT_COMMON_LAYOUT);
    extern void heart_null_show(u32 pic_id, u32 num_id, u8 type);
    heart_null_show(HRNULL_4, SPORT_COMMON_HR, LAST_DATA);
    if (elm) {
        ui_core_redraw(elm);
    }
}


static int SPORT_COMMON_LAYOUT_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        break;
    case ON_CHANGE_INIT:
        if (!__this->sport_common_timer) {
            __this->sport_common_timer = sys_timer_add(NULL, SPORT_COMMON_LAYOUT_timer, 500);
        }
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(ctrl, NULL, NULL, NULL);
        if (__this->sport_common_timer) {
            sys_timer_del(__this->sport_common_timer);
            __this->sport_common_timer = 0;
        }
        break;
    case ON_CHANGE_FIRST_SHOW:
        ui_get_child_by_id(SPORT_COMMON_LAYOUT, SPORT_COMMON_LAYOUT_child_cb);
        break;
    default:
        break;
    }
    return false;
}



static int SPORT_COMMON_LAYOUT_ontouch(void *ctrl, struct element_touch_event *e)
{
    int i;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:

        if (move_flag) {
            move_flag = 0;
            break;
        }
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_SPORT_INFO);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        return true;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    }


    return false;

}



static int SPORT_COMMON_LAYOUT_onkey(void *ctr, struct element_key_event *e)
{
    printf("%s %d %x\n", __FUNCTION__, e->value, e->event);
    switch (e->value) {
    case KEY_UI_SHORTCUT:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_SPORT_INFO);
        break;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(SPORT_COMMON_LAYOUT)
.ontouch = SPORT_COMMON_LAYOUT_ontouch,
 .onkey = SPORT_COMMON_LAYOUT_onkey,
  .onchange  = SPORT_COMMON_LAYOUT_onchange,
};



static int VLIST_TRAIN_SET_COMMON_LIST_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(grid, NULL, NULL, NULL);
        break;
    case ON_CHANGE_FIRST_SHOW:
        grid_children_init(grid);
        break;
    default:
        break;
    }
    return false;
}

static int VLIST_TRAIN_SET_COMMON_LIST_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
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

        switch (sel_item) {
        case 1://目标
            ui_hide(V_TRAIN_LAYOUT_2);
            ui_show(TRAIN_TARGET_LAYOUT);
            __this->prev_id = V_TRAIN_LAYOUT_2;
            __this->current_id = TRAIN_TARGET_LAYOUT;
            __this->set_index = 1;
            return true;
            break;
        case 2://提醒

            break;
        }

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
    case ELM_EVENT_TOUCH_L_MOVE:
        ui_hide(__this->current_id);
        ui_show(V_TRAIN_LAYOUT);
        __this->prev_id = __this->current_id;
        __this->current_id = V_TRAIN_LAYOUT;
        return true;
        break;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(VLIST_TRAIN_SET_COMMON_LIST)
.ontouch = VLIST_TRAIN_SET_COMMON_LIST_ontouch,
 .onkey = NULL,
  .onchange  = VLIST_TRAIN_SET_COMMON_LIST_onchange,
};




static int VLIST_TRAIN_LIST_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        break;
    case ON_CHANGE_INIT:
        int row = ARRAY_SIZE(T_target_title_ItemString);
        int col = 1;
        ui_grid_init_dynamic(grid, &row, &col);
        printf("dynamic_grid %d X %d\n", row, col);
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(grid, NULL, NULL, NULL);
        break;
    case ON_CHANGE_FIRST_SHOW:
        /* grid_children_init(grid); */
        break;
    default:
        break;
    }
    return false;
}

static int VLIST_TRAIN_LIST_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
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

        int index = ui_grid_cur_item_dynamic(grid);
        if (index) {
            ui_hide(TRAIN_TARGET_LAYOUT);
            __this->prev_id = TRAIN_TARGET_LAYOUT;
            if (index == ARRAY_SIZE(T_target_title_ItemString) - 1) {
                __this->target[__this->sport_set_index].type = NONE_TARGET; //0xff
                __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                __this->target[__this->sport_set_index].value = 0;
                ui_show(V_TRAIN_LAYOUT_2);
                __this->target_index = index;
                __this->current_id = V_TRAIN_LAYOUT_2;
                return true;
            }
            ui_show(LAYOUT_SPORT_SET_COMMON);
            __this->current_id = LAYOUT_SPORT_SET_COMMON;
            __this->user_target_index = 0;
            __this->target_index = index;
            return true;
        }
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
        ui_hide(__this->current_id);
        ui_show(V_TRAIN_LAYOUT_2);
        __this->prev_id = __this->current_id;
        __this->current_id = V_TRAIN_LAYOUT_2;
        return true;
    case ELM_EVENT_TOUCH_L_MOVE:
        break;
    }

    return false;
}

REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_LIST)
.ontouch = VLIST_TRAIN_LIST_ontouch,
 .onkey = NULL,
  .onchange  = VLIST_TRAIN_LIST_onchange,
};



static int list_target_pic_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    int index;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_HIGHLIGHT://接管高亮
        return true;
        break;
    case ON_CHANGE_UPDATE_ITEM:
        index = (u32)arg;
        pic->elm.css.invisible = !index;
        pic->elm.css.background_image = T_target_title_ItemPIC[index] & 0xff;
        break;
    }
    return FALSE;
}



REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_PIC0)//
.onchange = list_target_pic_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_PIC1)//
.onchange = list_target_pic_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_PIC2)//
.onchange = list_target_pic_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_PIC3)//
.onchange = list_target_pic_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};



static int list_target_right_pic_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    int index;

    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_HIGHLIGHT://接管高亮
        return true;
        break;
    case ON_CHANGE_UPDATE_ITEM:
        index = (u32)arg;
        pic->elm.css.invisible = !index;
        break;
    }
    return FALSE;
}


REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_RIGHT_PIC)//
.onchange = list_target_right_pic_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};





static int list_target_text_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    int index;
    struct ui_text *text = (struct ui_text *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_HIGHLIGHT:
        return true;
        break;
    case ON_CHANGE_UPDATE_ITEM:
        index = (u32)arg;

        if (!strcmp(text->source, "content") && index < ARRAY_SIZE(T_target_title_ItemString)) {
            text->elm.css.invisible = !index;
            text->attrs.str = (char *)(&T_target_title_ItemString[index]);
        }

        if (!strcmp(text->source, "title") && index < ARRAY_SIZE(T_target_title_ItemString)) {
            text->elm.css.invisible = index;
            text->attrs.str = (char *)(&T_target_title_ItemString[index]);
        }

        break;
    }
    return FALSE;
}


REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_TEXT0)//
.onchange = list_target_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_TEXT1)//
.onchange = list_target_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_TEXT2)//
.onchange = list_target_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_TEXT3)//
.onchange = list_target_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


REGISTER_UI_EVENT_HANDLER(TRAIN_TARGET_TITLE)//
.onchange = list_target_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int LIST_SPORT_SET_COMMON_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_grid *grid = (struct ui_grid *) ctrl;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        break;
    case ON_CHANGE_INIT:
        int row = 1;
        if (__this->target_index == 1) {
            row =  ARRAY_SIZE(T_DISTANCE_ItemString);
        }
        if (__this->target_index == 2) {
            row =  ARRAY_SIZE(T_TIME_ItemString);
        }
        if (__this->target_index == 3) {
            row =  ARRAY_SIZE(T_HEAT_ItemString);
        }

        if (__this->user_target_index) {
            row = 100;
        }

        int col = 1;
        ui_grid_init_dynamic(grid, &row, &col);
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        break;
    case ON_CHANGE_RELEASE:
        ui_set_default_handler(grid, NULL, NULL, NULL);
        break;
    case ON_CHANGE_FIRST_SHOW:
        /* grid_children_init(grid); */
        break;
    default:
        break;
    }
    return false;
}

static int LIST_SPORT_SET_COMMON_ontouch(void *ctrl, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctrl;
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

REGISTER_UI_EVENT_HANDLER(LIST_SPORT_SET_COMMON)
.ontouch = LIST_SPORT_SET_COMMON_ontouch,
 .onkey = NULL,
  .onchange  = LIST_SPORT_SET_COMMON_onchange,
};



static int SPORT_SET_COMMON_TITLE_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    int index;
    struct ui_text *text = (struct ui_text *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        text->attrs.str = (char *)(&T_target_title_ItemString[__this->target_index]);
        break;
    case ON_CHANGE_HIGHLIGHT:
        return true;
        break;
    case ON_CHANGE_UPDATE_ITEM:
        break;
    }
    return FALSE;
}


REGISTER_UI_EVENT_HANDLER(SPORT_SET_COMMON_TITLE)//
.onchange = SPORT_SET_COMMON_TITLE_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int list_target_set_text_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    int index;
    struct ui_text *text = (struct ui_text *)_ctrl;
    static u16 buf[4][6];
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_HIGHLIGHT:
        if (arg) {
            text->attrs.color = UI_RGB565(0x00aaff);    //蓝色
        } else {
            text->attrs.color = UI_RGB565(0xffffff);    //白色
        }
        return true;
        break;
    case ON_CHANGE_UPDATE_ITEM:
        index = (u32)arg;
        text->elm.css.invisible = 0;

        if (__this->target_index == 1) {
            if (!__this->user_target_index) {
                text->attrs.str = (char *)(&T_DISTANCE_ItemString[index]);
            } else {
                __T_USER_DISTANCE_ItemString(1 + index * 1, buf[index % 4], 6, 1);
                text->attrs.str = (char *)(&buf[index % 4]);
            }
        }

        if (__this->target_index == 2) {
            if (!__this->user_target_index) {
                text->attrs.str = (char *)(&T_TIME_ItemString[index]);
            } else {
                __T_USER_TIME_ItemString(10 + index * 5, buf[index % 4], 6, 1);
                text->attrs.str = (char *)(&buf[index % 4]);
            }
        }

        if (__this->target_index == 3) {
            if (!__this->user_target_index) {
                text->attrs.str = (char *)(&T_HEAT_ItemString[index]);
            } else {
                __T_USER_HEAT_ItemString(100 + index * 50, buf[index % 4], 6, 1);
                text->attrs.str = (char *)(&buf[index % 4]);
            }
        }

        text->attrs.format = "mulstr";
        break;
    }
    return FALSE;
}


REGISTER_UI_EVENT_HANDLER(LIST_SPORT_SET_TEXT0)//
.onchange = list_target_set_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

REGISTER_UI_EVENT_HANDLER(LIST_SPORT_SET_TEXT1)//
.onchange = list_target_set_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

REGISTER_UI_EVENT_HANDLER(LIST_SPORT_SET_TEXT2)//
.onchange = list_target_set_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

REGISTER_UI_EVENT_HANDLER(LIST_SPORT_SET_TEXT3)//
.onchange = list_target_set_text_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};





static int SPORT_COMMON_TEXT_ontouch(void *ctrl, struct element_touch_event *e)
{
    int i;
    static u8 move_flag = 0;
    int id_temp;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:

        if (move_flag) {
            move_flag = 0;
            break;
        }
        int index = ui_grid_cur_item_dynamic((struct ui_grid *)ui_core_get_element_by_id(LIST_SPORT_SET_COMMON));
        switch (__this->target_index) {
        case 1://距离
            if (!__this->user_target_index) {
                if (index  ==  ARRAY_SIZE(T_DISTANCE_ItemString) - 1) {
                    ui_hide(LAYOUT_SPORT_SET_COMMON);
                    ui_show(LAYOUT_SPORT_SET_COMMON);
                    /* __this->current_id = LAYOUT_SPORT_SET_COMMON; */
                    /* __this->prev_id = LAYOUT_SPORT_SET_COMMON; */
                    __this->user_target_index = 1;
                    return true;
                } else {
                    __this->target[__this->sport_set_index].type = 1;
                    __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                    __this->target[__this->sport_set_index].value = index;
                }

            } else {
                __this->target[__this->sport_set_index].type = 1;
                __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                __this->target[__this->sport_set_index].value = index;
            }
            ui_hide(__this->current_id);
            id_temp = V_TRAIN_LAYOUT_2;
            __this->prev_id = __this->current_id;;
            __this->current_id = id_temp;
            ui_show(id_temp);
            return true;
            break;
        case 2://时间
            if (!__this->user_target_index) {
                if (index  ==  ARRAY_SIZE(T_TIME_ItemString) - 1) {
                    ui_hide(LAYOUT_SPORT_SET_COMMON);
                    ui_show(LAYOUT_SPORT_SET_COMMON);
                    /* __this->current_id = LAYOUT_SPORT_SET_COMMON; */
                    /* __this->prev_id = LAYOUT_SPORT_SET_COMMON; */
                    __this->user_target_index = 2;
                    return true;
                } else {
                    __this->target[__this->sport_set_index].type = 2;
                    __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                    __this->target[__this->sport_set_index].value = index;

                }
            } else {
                __this->target[__this->sport_set_index].type = 2;
                __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                __this->target[__this->sport_set_index].value = index;

            }
            ui_hide(__this->current_id);
            id_temp = V_TRAIN_LAYOUT_2;
            __this->prev_id = __this->current_id;;
            __this->current_id = id_temp;
            ui_show(id_temp);
            return true;
            break;
        case 3://热量
            if (!__this->user_target_index) {
                if (index  ==  ARRAY_SIZE(T_HEAT_ItemString) - 1) {
                    ui_hide(LAYOUT_SPORT_SET_COMMON);
                    ui_show(LAYOUT_SPORT_SET_COMMON);
                    /* __this->current_id = LAYOUT_SPORT_SET_COMMON; */
                    /* __this->prev_id = LAYOUT_SPORT_SET_COMMON; */
                    __this->user_target_index = 3;
                    return true;
                } else {
                    __this->target[__this->sport_set_index].type = 3;
                    __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                    __this->target[__this->sport_set_index].value = index;
                }
            } else {
                __this->target[__this->sport_set_index].type = 3;
                __this->target[__this->sport_set_index].user = !!__this->user_target_index;
                __this->target[__this->sport_set_index].value = index;
            }
            ui_hide(__this->current_id);
            id_temp = V_TRAIN_LAYOUT_2;
            __this->prev_id = __this->current_id;;
            __this->current_id = id_temp;
            ui_show(id_temp);
            return true;
            break;
        }

        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        return true;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        ui_hide(__this->current_id);
        ui_show(TRAIN_TARGET_LAYOUT);
        __this->prev_id = __this->current_id;
        __this->current_id = TRAIN_TARGET_LAYOUT;
        return true;
        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        break;
    }


    return false;
}

REGISTER_UI_EVENT_HANDLER(SPORT_SET_COMMON_ENTER)//
.onchange = NULL,
 .onkey = NULL,
  .ontouch = SPORT_COMMON_TEXT_ontouch,
};


static int LAYOUT_TRAIN_BOOK_ontouch(void *ctrl, struct element_touch_event *e)
{
    int i;
    static u8 move_flag = 0;
    int id_temp;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        break;

    case ELM_EVENT_TOUCH_HOLD:
        break;

    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_DOWN:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(LAYOUT_TRAIN_BOOK)//
.onchange = NULL,
 .onkey = NULL,
  .ontouch = LAYOUT_TRAIN_BOOK_ontouch,
};



#endif /* #if TCFG_UI_ENABLE_TRAIN */
#endif /* #if (!TCFG_LUA_ENABLE) */



