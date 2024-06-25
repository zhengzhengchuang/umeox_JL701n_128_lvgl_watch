/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file control.h
 * @brief UI控件信息、类型定义。注意：本文件内容不可擅自改动，需配合UI工具，UI编译工具同步改动，否则影响UI框架运行
 * @author zhuhaifang@zh-jieli.com
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_CONTROL_H
#define UI_CONTROL_H

#include "ui/ui_core.h"

union ui_control_info;
struct layout_info;


/***************************************** 控件类型定义 *****************************************/
// 可通过 ui_id2type(ID) 获取，用于通过控件ID区分控件类型
#define CTRL_TYPE_WINDOW            2
#define CTRL_TYPE_LAYOUT            3
#define CTRL_TYPE_LAYER             4
#define CTRL_TYPE_GRID              5
#define CTRL_TYPE_LIST              6
#define CTRL_TYPE_BUTTON            7
#define CTRL_TYPE_PIC               8
#define CTRL_TYPE_BATTERY           9
#define CTRL_TYPE_TIME              10
#define CTRL_TYPE_CAMERA_VIEW       11
#define CTRL_TYPE_TEXT              12
#define CTRL_TYPE_ANIMATION         13
#define CTRL_TYPE_PLAYER            14
#define CTRL_TYPE_NUMBER            15

// 圆环进度条控件
#define CTRL_TYPE_PROGRESS          20
#define CTRL_PROGRESS_CHILD_BEGIN   (CTRL_TYPE_PROGRESS + 1)
#define CTRL_PROGRESS_CHILD_HIGHLIGHT   (CTRL_PROGRESS_CHILD_BEGIN) //21
#define CTRL_PROGRESS_CHILD_END     (CTRL_PROGRESS_CHILD_BEGIN + 1)

// 多重圆环进度条控件
#define CTRL_TYPE_MULTIPROGRESS          22
#define CTRL_MULTIPROGRESS_CHILD_BEGIN   (CTRL_TYPE_MULTIPROGRESS + 1)
#define CTRL_MULTIPROGRESS_CHILD_HIGHLIGHT   (CTRL_MULTIPROGRESS_CHILD_BEGIN)//23
#define CTRL_MULTIPROGRESS_CHILD_END     (CTRL_MULTIPROGRESS_CHILD_BEGIN + 1)

// watch(表盘)控件
#define CTRL_TYPE_WATCH             24
#define CTRL_WATCH_CHILD_BEGIN      (CTRL_TYPE_WATCH + 1)
#define CTRL_WATCH_CHILD_HOUR       (CTRL_WATCH_CHILD_BEGIN)//25
#define CTRL_WATCH_CHILD_MIN        (CTRL_WATCH_CHILD_BEGIN+1)//26
#define CTRL_WATCH_CHILD_SEC        (CTRL_WATCH_CHILD_BEGIN+2)//27
#define CTRL_WATCH_CHILD_END        (CTRL_WATCH_CHILD_BEGIN+3)

// 水平进度条控件
#define CTRL_TYPE_SLIDER            28

#define SLIDER_CHILD_BEGIN          (CTRL_TYPE_SLIDER+1)
#define SLIDER_CHILD_UNSELECT_PIC   (SLIDER_CHILD_BEGIN)//29
#define SLIDER_CHILD_SELECTED_PIC   (SLIDER_CHILD_BEGIN+1)//30
#define SLIDER_CHILD_SLIDER_PIC     (SLIDER_CHILD_BEGIN+2)//31
#define SLIDER_CHILD_PERSENT_TEXT   (SLIDER_CHILD_BEGIN+3)//32
#define SLIDER_CHILD_END            (SLIDER_CHILD_BEGIN+4)

// 垂直进度条控件
#define CTRL_TYPE_VSLIDER            33

#define VSLIDER_CHILD_BEGIN          (CTRL_TYPE_VSLIDER+1)
#define VSLIDER_CHILD_UNSELECT_PIC   (VSLIDER_CHILD_BEGIN)//34
#define VSLIDER_CHILD_SELECTED_PIC   (VSLIDER_CHILD_BEGIN+1)//35
#define VSLIDER_CHILD_SLIDER_PIC     (VSLIDER_CHILD_BEGIN+2)//36
#define VSLIDER_CHILD_PERSENT_TEXT   (VSLIDER_CHILD_BEGIN+3)//37
#define VSLIDER_CHILD_END            (VSLIDER_CHILD_BEGIN+4)

// 指南针控件
#define CTRL_TYPE_COMPASS            38

#define CTRL_COMPASS_CHILD_BEGIN          (CTRL_TYPE_COMPASS+1)
#define CTRL_COMPASS_CHILD_BKIMG          (CTRL_COMPASS_CHILD_BEGIN)//39
#define CTRL_COMPASS_CHILD_INDICATOR      (CTRL_COMPASS_CHILD_BEGIN+1)//40
#define CTRL_COMPASS_CHILD_END            (CTRL_COMPASS_CHILD_BEGIN+2)


// 通过ID号获取控件工程号(模式界面、表盘界面、侧边栏界面属于不同工程号，在UI编译工具上设置)
#define ui_id2prj(id)  	    ((((u32)id)>>26) & 0x3f)

// 通过ID号获取控件页面号(控件所在页面号)
#define ui_id2page(id)  	((((u32)id)>>17) & 0x1ff)

// 通过ID号获取控件类型
#define ui_id2type(id)  	((((u32)id)>>10) & 0x7f)


/***************************************** LUA相关 *****************************************/
// lua功能使能开关。注意：ui控件的属性不关，只是不会自动获取lua代码执行
extern const int ENABLE_LUA_VIRTUAL_MACHINE;

// lua事件类型
enum luascript_event_type {
    LUA_EVENT_ONLOAD = 0,
    LUA_EVENT_UNLOAD,
    LUA_EVENT_TOUCH_DOWN,
    LUA_EVENT_TOUCH_MOVE,
    LUA_EVENT_TOUCH_R_MOVE,
    LUA_EVENT_TOUCH_L_MOVE,
    LUA_EVENT_TOUCH_D_MOVE,
    LUA_EVENT_TOUCH_U_MOVE,
    LUA_EVENT_TOUCH_HOLD,
    LUA_EVENT_TOUCH_UP,
    LUA_EVENT_ONSHOW,
    LUA_EVENT_MAX,
};

// lua属性结构
struct luascript_code {
    u16 type;
    u16 argc;
    char argv[0];
};



/***************************************** UI资源相关 *****************************************/
// ui资源结构
struct element_luascript {
    u16 num;
    u16 nop; //FFFF
    struct luascript_code code[0];
};

struct element_luascript_t {
    u16 num;
    u16 nop; //FFFF
    struct luascript_code *code[LUA_EVENT_MAX];
};


// 控件信息头
struct ui_ctrl_info_head {
    u8 		type;
    u8 		ctrl_num;
    u8 		css_num;
    u8 		len;
    u8 		page;
    u8 		rev[3];
    int 	id;
    struct 	element_css1 *css;
};

// 图片列表结构
struct ui_image_list {
    u16 num;
    u16 image[0];
};

// 文本列表结构
struct ui_text_list {
    u16 num;
    char str[0];
};


struct ui_image_list_t {
    u16 num;
    u16 image[128];
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief BR28手表部分手表客户开发150种运动功能，需扩大多国语言可读取数量
 */
/* ------------------------------------------------------------------------------------*/
#define UI_TEXT_STRPIC_MAX_INDEX	255


#define UI_TEXT_LIST_MAX_NUM    	3
struct ui_text_list_t {
    u16 num;
    u16 str[UI_TEXT_STRPIC_MAX_INDEX];
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief button控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_button_info {
    struct ui_ctrl_info_head 	head;
    struct element_event_action *action;
    struct element_luascript_t *lua;
};



struct ui_camera_info {
    struct ui_ctrl_info_head head;
    char device[8];
    struct element_event_action *action;
};

// player控件信息
struct ui_player_info {
    struct ui_ctrl_info_head head;
    char device[8];
    struct element_event_action *action;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief time控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_time_info {
    struct ui_ctrl_info_head head;
    char source[8];
    u8 auto_cnt;
    u8 rev[3];
    char format[16];
    int color;
    int hi_color;
    u16 number[10];
    u16 delimiter[10];
    struct element_event_action *action;
    struct element_luascript_t *lua;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief number控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_number_info {
    struct ui_ctrl_info_head head;
    char source[8];
    char format[16];
    int color;
    int hi_color;
    u16 number[10];
    u16 delimiter[10];
    u16 space[2];
    struct element_event_action *action;
    struct element_luascript_t *lua;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief pic控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_pic_info {
    struct ui_ctrl_info_head head;
    char source[8];
    u8 highlight;
    u16 cent_x;
    u16 cent_y;
    u8 play_mode;
    u16 play_interval;
    struct ui_image_list *normal_img;
    struct ui_image_list *highlight_img;
    struct element_event_action *action;
    struct element_luascript_t *lua;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief battery控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_battery_info {
    struct ui_ctrl_info_head head;
    struct ui_image_list *normal_image;
    struct ui_image_list *charge_image;
    struct element_event_action *action;
    struct element_luascript_t *lua;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief text控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_text_info {
    struct ui_ctrl_info_head head;
    char source[8];
    char code[8];
    int color;
    int highlight_color;
    struct ui_text_list *str;
    struct element_event_action *action;
    struct element_luascript_t *lua;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief grid控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_grid_info {
    struct ui_ctrl_info_head head;
    u8 page_mode;
    char highlight_index;
    struct element_event_action *action;
    struct element_luascript_t *lua;
    struct layout_info *info;
};

// animation控件信息
struct ui_animation_info {
    struct ui_ctrl_info_head head;
    u16 loop_num;
    u32 interval;
    struct ui_image_list *img;
    struct element_event_action *action;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief slider控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_slider_info {
    struct ui_ctrl_info_head head;
    u8 step;
    struct element_luascript_t *lua;
    struct ui_ctrl_info_head *ctrl;
    // struct element_event_action *action;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief vslider控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_vslider_info {
    struct ui_ctrl_info_head head;
    u8 step;
    struct element_luascript_t *lua;
    struct ui_ctrl_info_head *ctrl;
    // struct element_event_action *action;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief watch控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_watch_info {
    struct ui_ctrl_info_head head;
    char source[8];
    struct element_event_action *action;
    struct element_luascript_t *lua;
    struct ui_ctrl_info_head *ctrl;
};

// compass控件信息
struct ui_compass_info {
    struct ui_ctrl_info_head head;
    char source[8];
    struct element_event_action *action;
    struct element_luascript_t *lua;
    struct ui_ctrl_info_head *ctrl;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief progress控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_progress_info {
    struct ui_ctrl_info_head head;
    char source[8];
    struct element_event_action *action;
    struct element_luascript_t *lua;
    struct ui_ctrl_info_head *ctrl;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief multiprogress控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct ui_multiprogress_info {
    struct ui_ctrl_info_head head;
    char source[8];
    struct element_event_action *action;
    struct element_luascript_t *lua;
    struct ui_ctrl_info_head *ctrl;
};

// browser控件信息
struct ui_browser_info {
    struct ui_ctrl_info_head head;
    u8 row;
    u8 column;
    u8 interval;
    u8 scroll;
    u8 auto_highlight;
    struct element_event_action *action;
    struct ui_ctrl_info_head *ctrl;
};

// fattrs控件信息
struct ui_fattrs_info {
    struct ui_ctrl_info_head head;
    struct element_event_action *action;
    struct ui_ctrl_info_head *ctrl;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct layout_info {
    struct ui_ctrl_info_head head;
    struct element_event_action *action;
    struct element_luascript_t *lua;
    union ui_control_info *ctrl;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer控件信息
 */
/* ------------------------------------------------------------------------------------*/
struct layer_info {
    struct ui_ctrl_info_head head;
    u8 format;
    struct element_event_action *action;
    struct element_luascript_t *lua;
    struct layout_info *layout;
};


union ui_control_info {
    struct ui_ctrl_info_head head;//16 bytes
    struct ui_button_info   button;//20 bytes
    struct ui_camera_info   camera;//28 bytes
    struct ui_time_info     time;//84 bytes
    struct ui_number_info   number;
    struct ui_pic_info      pic;//36 bytes
    struct ui_battery_info  battery;//28 bytes
    struct ui_text_info     text;//40 bytes
    struct ui_grid_info     grid;//28 bytes
    struct layer_info       layer;
    struct layout_info      layout;
    struct ui_watch_info    watch;
    struct ui_progress_info progress;
    struct ui_multiprogress_info multiprogress;
    struct ui_slider_info   slider;
    struct ui_vslider_info  vslider;
};//84 bytes

// union ui_control_info {
// struct ui_ctrl_info_head head;
// struct ui_button_info 	button;
// struct ui_camera_info 	camera;
// struct ui_time_info 	time;
// struct ui_number_info   number;
// struct ui_pic_info 		pic;
// struct ui_battery_info 	battery;
// struct ui_text_info 	text;
// struct ui_grid_info  	grid;
// };


struct window_info {
    u8 		type;
    u8 		ctrl_num;
    u8 		css_num;
    u8 		len;
    u8 		rev[4];
    struct rect rect;
    struct layer_info *layer;
//	struct element_event_action *action;
};

struct control_ops {
    int type;
    void *(*new)(const void *, struct element *);
    /*int (*delete)(void *);*/
};

extern const struct control_ops control_ops_begin[];
extern const struct control_ops control_ops_end[];


#define REGISTER_CONTROL_OPS(_type)  \
	static const struct control_ops control_ops_##_type sec(.control_ops) __attribute__((used)) = { \
		.type = _type,



#define get_control_ops_by_type(_type) \
		({  \
		 	const struct control_ops *ops, *ret=NULL; \
			for (ops = control_ops_begin; ops < control_ops_end; ops++) { \
		 			if (ops->type == _type) { \
		 				ret = ops; \
						break; \
		 			} \
		 	}\
		 	ret; \
		 })


/*
 * 通过事件找到对应控件的lua code
 * */
static inline int luascript_code_find(struct element_luascript_t *elm_code, u8 event_type, struct luascript_code **code)
{
    if (!ENABLE_LUA_VIRTUAL_MACHINE) {
        return 0;
    }
    struct luascript_code *lua_code = NULL;

    for (u8 i = 0; i < elm_code->num; i++) {
        lua_code = elm_code->code[i];
        if (lua_code->type == event_type) {
            // printf("layout luascript_code type:%d\n",lua_code->type);
            // printf("layout luascript_code len:%d\n",lua_code->argc);
            // printf("layout luascript_code code:\n%s\n",&lua_code->argv);
            *code = lua_code;
            return 1;
        }
        // lua_code = (struct luascript_code *)((u32)&lua_code->argv + lua_code->argc);
    }
    return 0;
}

// 默认有lua被执行就将touch消息截断，后续不让继续跑。
// 由run_lua_string函数控制touch消息是否截断，如果没有lua代码被执行则不截断。
static inline int lua_touch_event_run(struct element_luascript_t *code, struct element_touch_event *e)
{
    if (!ENABLE_LUA_VIRTUAL_MACHINE) {
        return 0;
    }
    // printf("======lua_touch_event_run:%d\n", e->event);
    struct luascript_code *lua_code = NULL;
    int type = -1;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        type = LUA_EVENT_TOUCH_DOWN;
        break;
    case ELM_EVENT_TOUCH_MOVE:
        type = LUA_EVENT_TOUCH_MOVE;
        break;
    case ELM_EVENT_TOUCH_U_MOVE:
        type = LUA_EVENT_TOUCH_U_MOVE;
        break;
    case ELM_EVENT_TOUCH_D_MOVE:
        type = LUA_EVENT_TOUCH_D_MOVE;
        break;
    case ELM_EVENT_TOUCH_L_MOVE:
        type = LUA_EVENT_TOUCH_L_MOVE;
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        type = LUA_EVENT_TOUCH_R_MOVE;
        break;
    case ELM_EVENT_TOUCH_HOLD:
        type = LUA_EVENT_TOUCH_HOLD;
        break;
    case ELM_EVENT_TOUCH_UP:
        type = LUA_EVENT_TOUCH_UP;
        break;
    }
    if (luascript_code_find(code, type, &lua_code)) {
        // printf("func: %s %d find lua code\n",__func__,__LINE__ );
        return run_lua_string(lua_code->argc, (const char *) &lua_code->argv);
    } else {
        // printf("not find lua code\n");
        return 0;
    }

    // return 1; //err
}


#if 0
struct control_event_header {
    int id;
    int len;
};

extern struct control_event_header control_event_handler_begin[];
extern struct control_event_header control_event_handler_end[];


#define REGISTER_CONTROL_EVENT_HANDLER(control, _id) \
	static const struct control##_event_handler __##control##_event_handler_##_id \
			sec(.control_event_handler) = { \
				.header = { \
					.id = _id, \
					.len = sizeof(struct control##_event_handler), \
				}, \




static inline void *control_event_handler_for_id(int id)
{
    struct control_event_header *p;

    for (p = control_event_handler_begin; p < control_event_handler_end;) {
        if (p->id == id) {
            return p;
        }
        p = (u8 *)p + p->len;
    }

    return NULL;
}
#endif








#endif


