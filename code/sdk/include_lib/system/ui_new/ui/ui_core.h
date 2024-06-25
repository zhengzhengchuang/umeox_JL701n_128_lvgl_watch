/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_core.h
 * @brief 杰理UI框架核心，注意：本文件修改会影响到UI资源结构，UI工具等内容，禁止随意改动
 * @author
 * @version V201
 * @date 2022-12-15
 */

#ifndef UI_ELEMENT_CORE_H
#define UI_ELEMENT_CORE_H

#include "typedef.h"
#include "rect.h"
#include "system/event.h"
// #include "fs/fs.h"
#include "res/resfile.h"
#include "ui/ui_effect.h"


#define UI_CTRL_BUTTON  0

struct element;


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 使能PSRAM标记
 */
/* ------------------------------------------------------------------------------------*/
extern const int ENABLE_PSRAM_UI_FRAME;


#ifdef offsetof
#undef offsetof
#endif
#ifdef container_of
#undef container_of
#endif

#define offsetof(type, memb) \
((unsigned long)(&((type *)0)->memb))

#define container_of(ptr, type, memb) \
((type *)((char *)ptr - offsetof(type, memb)))


enum ui_direction {
    UI_DIR_UP,
    UI_DIR_DOWN,
    UI_DIR_LEFT,
    UI_DIR_RIGHT,
};

enum ui_align {
    UI_ALIGN_LEFT = 0,
    UI_ALIGN_CENTER,
    UI_ALIGN_RIGHT,
};


enum {
    POSITION_ABSOLUTE = 0,
    POSITION_RELATIVE = 1,
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 触摸事件类型
 */
/* ------------------------------------------------------------------------------------*/
enum {
    ELM_EVENT_TOUCH_DOWN,
    ELM_EVENT_TOUCH_MOVE,
    ELM_EVENT_TOUCH_R_MOVE,
    ELM_EVENT_TOUCH_L_MOVE,
    ELM_EVENT_TOUCH_D_MOVE,
    ELM_EVENT_TOUCH_U_MOVE,
    ELM_EVENT_TOUCH_HOLD,
    ELM_EVENT_TOUCH_UP,
    ELM_EVENT_TOUCH_ENERGY,
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 按键事件类型
 */
/* ------------------------------------------------------------------------------------*/
enum {
    ELM_EVENT_KEY_CLICK,
    ELM_EVENT_KEY_LONG,
    ELM_EVENT_KEY_HOLD,
};

enum {
    ELM_STA_INITED,
    ELM_STA_READY,
    //ELM_STA_SHOW_PROBE,
    //ELM_STA_SHOW_POST,
    ELM_STA_HIDE,
    ELM_STA_SHOW,
    ELM_STA_PAUSE,
};

enum {
    ELM_FLAG_NORMAL,
    ELM_FLAG_HEAD,
};

enum {
    DC_DATA_FORMAT_OSD8 = 0,
    DC_DATA_FORMAT_YUV420 = 1,
    DC_DATA_FORMAT_OSD16 = 2,
    DC_DATA_FORMAT_OSD8A = 3,
    DC_DATA_FORMAT_MONO = 4,
};


struct element_touch_event {
    int event;
    int xoffset;
    int yoffset;
    u8  hold_up;
    u8  onfocus;
    u8  move_flag;
    struct position pos;
    struct position origin;
    void *private_data;
    int has_energy;
};

struct element_key_event {
    u8 event;
    u8 value;
    void *private_data;
};

#define ELM_KEY_EVENT(e) 		(0x0000 | (e->event) | (e->value << 8))
#define ELM_TOUCH_EVENT(e) 		(0x1000 | (e->event))
#define ELM_CHANGE_EVENT(e) 	(0x2000 | (e->event))


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ON_CHANGE事件类型
 */
/* ------------------------------------------------------------------------------------*/
enum element_change_event {
    ON_CHANGE_INIT_PROBE,
    ON_CHANGE_INIT,
    ON_CHANGE_TRY_OPEN_DC,
    ON_CHANGE_FIRST_SHOW,
    ON_CHANGE_SHOW_PROBE,
    ON_CHANGE_SHOW,
    ON_CHANGE_SHOW_POST,
    ON_CHANGE_HIDE,
    ON_CHANGE_HIGHLIGHT,
    ON_CHANGE_RELEASE_PROBE,
    ON_CHANGE_RELEASE,
    ON_CHANGE_ANIMATION_END,
    ON_CHANGE_SHOW_COMPLETED,
    ON_CHANGE_UPDATE_ITEM,
};


struct element_event_handler {
    int id;
    int (*ontouch)(void *, struct element_touch_event *);
    int (*onkey)(void *, struct element_key_event *);
    int (*onchange)(void *, enum element_change_event, void *);
};

struct jaction {
    u32 show;
    u32 hide;
};

enum {
    ELM_ACTION_HIDE = 0,
    ELM_ACTION_SHOW,
    ELM_ACTION_TOGGLE,
    ELM_ACTION_HIGHLIGHT,
};

struct event_action {
    u16 event;
    u16 action;
    int id;
    u8  argc;
    char argv[];
};

struct element_event_action {
    u16 num;
    struct event_action action[0];
};

struct image_preview {
    struct flash_file_info *file_info;
    RESFILE *file;
    int id;
    int page;
};


struct image {
    int x;
    int y;
    int id;
    int page;
    int en;
    struct rect rect;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 显示上下文句柄结构
 */
/* ------------------------------------------------------------------------------------*/
struct draw_context {
    u8 ref;
    u8 alpha;
    u8 align;
    u8 data_format;
    u8 prj;
    u8 page;
    u8 buf_num;
    u32 background_color;
    void *handl;
    struct element *elm;
    struct rect rect;
    struct rect draw;
    void *dc;

    struct image_preview preview;

    struct rect page_draw;
    struct rect need_draw;
    struct rect disp;
    struct rect rect_orig;
    struct rect rect_ref;//referance of page rect
    u16 width;
    u16 height;
    u8 *fbuf;
    u32 fbuf_len;
    u8 *buf;
    u8 *buf0;
    u8 *buf1;
    u32 len;
    u16 lines;
    u16 colums;
    u8 col_align;
    u8 row_align;
    u8 index;
    u8 elm_index;
    u8 draw_state;
    u8 crop;

    struct image draw_img;

    u8 *mask;
    u8 refresh;

    u8 custom_color_en;
    u16 color_rgb565;

    /*********psram和特效相关变量*************/
    //滑动页面时,当新页第一次复制到psram时为1，其他时候为0
    //可用于滑动时,特效只需要处理一次的情况
    u8 new_copy;
    u8 new_page;
    u8 slider;
    u8 just_record;
    u8 lr_status;//0:left 1:right
    u8 copy_to_psram;
    u16 effect_mode;
    effect_cb effect_user;//用户自定义特效处理的回调函数
    void *effect_priv; //页面特效私有值,可以用来传递特效参数
    /********************************************/
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 边框属性
 */
/* ------------------------------------------------------------------------------------*/
struct css_border {
    u16 left: 4;
    u16 top: 4;
    u16 right: 4;
    u16 bottom: 4;
    u16 color: 16;
};

struct css_border1 {
    u8 left;
    u8 top;
    u8 right;
    u8 bottom;
    int color: 24;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 旋转属性
 */
/* ------------------------------------------------------------------------------------*/
struct css_rotate {
    u8 en;
    u8 chunk;
    int angle;
    int cent_x;
    int cent_y;
    int dx;
    int dy;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 缩放属性
 */
/* ------------------------------------------------------------------------------------*/
struct css_ratio {
    u8 en;
    float ratio_w;
    float ratio_h;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 元素属性结构
 */
/* ------------------------------------------------------------------------------------*/
struct element_css {
    u8  align: 2;
    u8  invisible: 1;
    u8  z_order: 5;
    int left/* : 16 */;
    int top/* : 16 */;
    int width/* : 16 */;
    int height/* : 16 */;
    u32 background_color: 24;
    u32 alpha: 8;
    int background_image: 24;
    int image_quadrant: 8;
    struct css_border border;
    struct css_rotate rotate;
    struct css_ratio ratio;
};

struct element_css1 {
    u8  align;
    u8  invisible;
    u8  z_order;
    int left;
    int top;
    int width;
    int height;
    u32 background_color: 24;
    u32 alpha: 8;
    int background_image: 24;
    int image_quadrant: 8;
    struct css_border1 border;
};

struct element_ops {
    int (*show)(struct element *);
    int (*redraw)(struct element *, struct rect *);
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 元素结构
 */
/* ------------------------------------------------------------------------------------*/
struct element {
    u32 highlight: 1;
    u32 state: 3;
    u32 ref: 5;
    u32 prj: 3;
    u32 hide_action: 1;
    u32 css_num : 2;
    u32 page: 8;
    u32 prior: 9;
    // u32 alive;
    int id;
    struct element *parent;
    struct list_head sibling;
    struct list_head child;
    struct element *focus;
    struct element_css css;
    struct draw_context *dc;
    // const struct element_ops *ops;
    const struct element_event_handler *handler;
    // const struct element_event_action *action;
    int group;
};

struct ui_style {
    const char *file;
    u32 version;
};

enum {
    UI_FTYPE_VIDEO = 0,
    UI_FTYPE_IMAGE,
    UI_FTYPE_AUDIO,
    UI_FTYPE_DIR,
    UI_FTYPE_UNKNOW = 0xff,
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief UI文件属性
 */
/* ------------------------------------------------------------------------------------*/
struct ui_file_attrs {
    char *format;
    char fname[128];
    struct vfs_attr attr;
    u8 ftype;
    u16 file_num;
    u32 film_len;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief UI图片属性
 */
/* ------------------------------------------------------------------------------------*/
struct ui_image_attrs {
    u16 width;
    u16 height;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief UI文本属性
 */
/* ------------------------------------------------------------------------------------*/
struct ui_text_attrs {
    const char *str;
    const char *format;
    int color;
    u16 strlen;
    u16 offset;
    u8  encode: 2;
    u8  endian: 1;
    u8  flags: 5;
    // u16  offset;
    // u8  displen;
    u16  displen;	// 显示长度类型改为u16，防止文本长度溢出导致显示异常
    const char *mulstr;
    u16 mulstr_len;
    u16 default_code;
    u8 x_interval;
    u8 charNumInSyllable;  //印地语单音节内字符数
    short word_space;
    short line_space;
    u16 width;
    u16 height;
    short scroll_cnt;
    short offsetx;
};

struct ui_file_browser {
    int file_number;
    u8  dev_num;
    void *private_data;
};



struct ui_effect_ctrl {
    u8 has_new_page;
    void *new_page_elm;

    u16 effect_mode;
    void *user_effect_cb;
    void *effect_priv;
    int stop_redraw;
};
extern struct ui_effect_ctrl ui_core_effcet;



#define ELEMENT_ALIVE 		0x53547a7b

#define element_born(elm) \
		elm->alive = ELEMENT_ALIVE

#define element_alive(elm) \
		(elm->alive == ELEMENT_ALIVE)


/* 扫描子控件链表 */
#define list_for_each_child_element(p, elm) \
	list_for_each_entry(p, &(elm)->child, sibling)

#define list_for_each_child_element_reverse(p, n, elm) \
	list_for_each_entry_reverse_safe(p, n, &(elm)->child, sibling)

#define list_for_each_child_element_safe(p, n, elm) \
	list_for_each_entry_safe(p, n, &(elm)->child, sibling)

struct ui_platform_api {
    void *(*malloc)(int);
    void (*free)(void *);

    int (*load_style)(struct ui_style *);

    void *(*load_window)(int id);
    void (*unload_window)(void *);

    int (*open_draw_context)(struct draw_context *);
    int (*get_draw_context)(struct draw_context *);
    int (*put_draw_context)(struct draw_context *);
    int (*set_draw_context)(struct draw_context *);
    int (*close_draw_context)(struct draw_context *);

    int (*show_element)(struct draw_context *);
    int (*hide_element)(struct draw_context *);
    int (*delete_element)(struct draw_context *);
    int (*set_element_prior)(struct draw_context *);

    int (*fill_rect)(struct draw_context *, u32 color);
    int (*draw_rect)(struct draw_context *, struct css_border *border);
    int (*draw_image)(struct draw_context *, u32 src, u8 quadrant, u8 *mask);
    int (*draw_point)(struct draw_context *, u16 x, u16 y, u32 color);
    u32(*read_point)(struct draw_context *dc, u16 x, u16 y);
    int (*invert_rect)(struct draw_context *, u32 color);

    void *(*load_widget_info)(void *_head, u8 page);
    void *(*load_css)(u8 page, void *_css);
    void *(*load_image_list)(u8 page, void *_list);
    void *(*load_text_list)(u8 page, void *__list);

    //int (*highlight)(struct draw_context *);
    int (*show_text)(struct draw_context *, struct ui_text_attrs *);
    int (*read_image_info)(struct draw_context *, u32, u8, struct ui_image_attrs *);

    int (*open_device)(struct draw_context *, const char *device);
    int (*close_device)(int);

    void *(*set_timer)(void *, void (*callback)(void *), u32 msec);
    int (*del_timer)(void *);

    struct ui_file_browser *(*file_browser_open)(struct rect *r,
            const char *path, const char *ftype, int show_mode);

    int (*get_file_attrs)(struct ui_file_browser *, struct ui_file_attrs *attrs);

    int (*set_file_attrs)(struct ui_file_browser *, struct ui_file_attrs *attrs);

    int (*clear_file_preview)(struct ui_file_browser *, struct rect *r);

    int (*show_file_preview)(struct ui_file_browser *, struct rect *r, struct ui_file_attrs *attrs);

    int (*flush_file_preview)(struct ui_file_browser *);

    void *(*open_file)(struct ui_file_browser *, struct ui_file_attrs *attrs);
    int (*delete_file)(struct ui_file_browser *, struct ui_file_attrs *attrs);

    int (*move_file_preview)(struct ui_file_browser *_bro, struct rect *dst, struct rect *src);

    void (*file_browser_close)(struct ui_file_browser *);

};

extern /* const */ struct ui_platform_api *platform_api;

extern /* const */ struct element_event_handler dumy_handler;

struct janimation {
    u8  persent[5];
    u8  direction;
    u8  play_state;
    u8  iteration_count;
    u16 delay;
    u16 duration;
    struct element_css css[0];
};


extern struct element_event_handler *elm_event_handler_begin;
extern struct element_event_handler *elm_event_handler_end;


#define ___REGISTER_UI_EVENT_HANDLER(style, _id) \
    static const struct element_event_handler element_event_handler_##_id \
			sec(.elm_event_handler_##style) __attribute__((used)) = { \
				.id = _id,

#define __REGISTER_UI_EVENT_HANDLER(style, _id) \
		___REGISTER_UI_EVENT_HANDLER(style, _id)

#define REGISTER_UI_EVENT_HANDLER(id) \
	__REGISTER_UI_EVENT_HANDLER(STYLE_NAME, id)



struct ui_style_info {
    const char *name;
    struct element_event_handler *begin;
    struct element_event_handler *end;
};

extern struct ui_style_info ui_style_begin[];
extern struct ui_style_info ui_style_end[];

#define __REGISTER_UI_STYLE(style_name) \
	extern struct element_event_handler elm_event_handler_begin_##style_name[]; \
	extern struct element_event_handler elm_event_handler_end_##style_name[]; \
    static const struct ui_style_info ui_style_##style_name sec(.ui_style) __attribute__((used)) = { \
		.name 	= #style_name, \
		.begin 	= elm_event_handler_begin_##style_name, \
		.end 	= elm_event_handler_end_##style_name, \
	};

#define REGISTER_UI_STYLE(style_name) \
	__REGISTER_UI_STYLE(style_name)



extern struct element_event_handler *element_event_handler_for_id(u32 id);
// {
//     struct element_event_handler *p;
//
//     for (p = elm_event_handler_begin; p < elm_event_handler_end; p++) {
//         if (p->id == id) {
//             return p;
//         }
//     }
//
//     return NULL;
// }



/* 获取控件的CSS属性 */
#define ui_core_get_element_css(elm)   \
	&((struct element *)(elm))->css

/* 设置控件的invisible属性 */
#define ui_core_element_invisable(elm, i)  \
		((struct element *)(elm))->css.invisible = i


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_init 杰理UI框架核心初始化
 *
 * @param api UI框架API
 * @param rect UI显示区域
 *
 * @return 0 正常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_init(const struct ui_platform_api *api, struct rect *rect);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_set_style 设置UI风格
 *
 * @param style UI资源路径
 *
 * @return 0 正常, -22 失败
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_set_style(const char *style);

void ui_core_set_rotate(int _rotate);

int ui_core_get_rotate();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_malloc UI内存申请
 *
 * @param size 待申请内存大小
 *
 * @return 申请的内存指针
 */
/* ------------------------------------------------------------------------------------*/
void *ui_core_malloc(int size);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_free UI内存释放
 *
 * @param buf 待释放内存指针
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_free(void *buf);

void ui_core_element_init(struct element *,
                          u32 id,
                          u8 page,
                          u8 prj,
                          u8 css_num,
                          struct element_css1 *,
                          const struct element_event_handler *,
                          const struct element_event_action *);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_element_abs_rect 获取控件绝对位置(UI工具上设置的位置)
 *
 * @param elm 控件元素句柄
 * @param rect 绝对位置缓存指针（输出）
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_get_element_abs_rect(struct element *elm, struct rect *rect);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_append_child 添加子元素，按照z_order从小到大排序，也就是从底层往上层排列。
 * 重绘时从前往后遍历，onkey和ontouch时从后往前遍历
 *
 * @param _child 待添加的子元素句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_append_child(void *_child);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_first_child 获取第一个子控件元素句柄
 *
 * @return 元素句柄
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_first_child();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_remove_element 从链表中移除子控件
 *
 * @param _child 待移除的子控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_remove_element(void *_child);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_open_draw_context 打开显示上下文
 *
 * @param dc 显示上下文
 * @param elm 控件元素
 *
 * @return 0 正常，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_open_draw_context(struct draw_context *dc, struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_close_draw_context 关闭显示上下文
 *
 * @param dc 显示上下文指针
 *
 * @return 0 正常，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_close_draw_context(struct draw_context *dc);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_show 显示控件元素
 *
 * @param _elm 控件元素
 * @param init 是否已经初始化(true 是，false 否)
 *
 * @return 0 正常，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_show(void *_elm, int init);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_hide 隐藏控件元素
 *
 * @param _elm 控件元素
 *
 * @return 0 正常，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_hide(void *_elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief get_element_by_id 通过ID获取子控件元素
 *
 * @param elm 父控件元素
 * @param id 子控件ID
 *
 * @return 子控件元素
 */
/* ------------------------------------------------------------------------------------*/
struct element *get_element_by_id(struct element *elm, u32 id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_element_by_id 通过ID获取控件元素
 *
 * @param id 控件ID
 *
 * @return 控件元素
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_element_by_id(u32 id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_disp_status_by_id 获取指定ID的控件显示状态
 *
 * @param id 控件ID
 *
 * @return 0 隐藏，1 显示，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_get_disp_status_by_id(u32 id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_up_element 获取指定元素上方的元素
 *
 * @param elm 指定元素
 *
 * @return 指定元素上方的元素
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_up_element(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_down_element 获取指定元素下方的元素
 *
 * @param elm 指定元素
 *
 * @return 指定元素下方的元素
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_down_element(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_left_element 获取指定元素左边的元素
 *
 * @param elm 指定元素
 *
 * @return 指定元素左边的元素
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_left_element(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_right_element 获取指定元素右边的元素
 *
 * @param elm 指定元素
 *
 * @return 指定元素右边的元素
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_right_element(struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_put_element
 *
 * @param elm
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_put_element(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_element
 *
 * @param elm
 *
 * @return 其他正常，-22异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_get_element(struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_element_ontouch 触发元素触摸回调
 *
 * @param elm 控件元素
 * @param e 触摸事件
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_element_ontouch(struct element *, struct element_touch_event *e);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_ontouch 触摸事件分发处理
 *
 * @param e 触摸事件
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_ontouch(struct element_touch_event *e);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_element_onkey 元素按键事件响应
 *
 * @param elm 控件元素
 * @param e 按键事件
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_element_onkey(struct element *elm, struct element_key_event *e);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_onkey 按键事件分发处理
 *
 * @param e 按键事件
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_onkey(struct element_key_event *e);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_element_append_child 添加子元素，按照z_order从小到大排序，也就是从底层往上层排列，重绘时从前往后遍历，onkey和ontouch是从后往前遍历
 *
 * @param parent 父控件元素
 * @param child 子控件元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_element_append_child(struct element *parent, struct element *child);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_set_element_css 设置控件元素的CSS属性
 *
 * @param _elm 控件元素
 * @param css 待设置的CSS属性
 *
 * @return 设置后的CSS属性
 */
/* ------------------------------------------------------------------------------------*/
struct element_css *ui_core_set_element_css(void *_elm, const struct element_css1 *css);

int ui_core_invert_rect(struct draw_context *dc);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_release_child_probe
 *
 * @param elm
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_release_child_probe(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_release_child 释放子控件
 *
 * @param elm 待释放的子控件元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_release_child(struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_redraw 重绘控件元素
 *
 * @param _elm 待重绘的控件元素
 *
 * @return 0 正常，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_redraw(void *_elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_highlight_element 设置控件元素的高亮状态
 *
 * @param elm 待高亮的控件元素
 * @param yes 是否设置为为高亮状态
 *
 * @return 0 正常，其他异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_highlight_element(struct element *elm, int yes);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_element_on_focus 控件元素焦点设置
 *
 * @param elm 控件元素
 * @param yes 是否设置为焦点
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_element_on_focus(struct element *elm, int yes);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_element_ontouch_focus 控件元素触摸事件聚焦
 *
 * @param level 等级（一般为 int level = 0; &level 进行设置）
 * @param elm 控件元素
 * @param e 触摸事件
 *
 * @return true正常，false异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_element_ontouch_focus(int level, struct element *elm, struct element_touch_event *e);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_ontouch_lose_focus 控件元素失去焦点
 *
 * @param elm 控件元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_ontouch_lose_focus(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_ontouch_lock 控件触摸事件锁定(如果设置了锁定，触摸事件会发送到该控件)
 *
 * @param elm 控件元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_ontouch_lock(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_ontouch_unlock 控件触摸事件解锁
 *
 * @param elm 控件元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_core_ontouch_unlock(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_draw_context 获取绘制上下文句柄
 *
 * @param dc 显示上下文
 * @param elm 控件元素
 * @param draw 绘制区域
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int ui_core_get_draw_context(struct draw_context *dc, struct element *elm, struct rect *draw);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_screen_draw_rect 获取屏幕绘制区域
 *
 * @return rect 结构体，表示屏幕绘制区域
 */
/* ------------------------------------------------------------------------------------*/
struct rect *ui_core_get_screen_draw_rect();

void ui_core_set_group(struct element *elm, int group, int chile_redo);

// ~~~~~~~~~~~ 以下为lua相关接口
extern int run_lua_string(int len, const char *string);

void *ui_core_load_lua(u8 page, void *_lua);

void ui_core_free_lua(void *_lua);



#endif



