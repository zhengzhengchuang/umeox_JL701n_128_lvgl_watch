/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_text.h
 * @brief 文本控件API接口头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_TEXT_H
#define UI_TEXT_H


#include "ui/ui_core.h"
#include "ui/control.h"
#include "font/font_all.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 文本控件句柄结构定义
 */
/* ------------------------------------------------------------------------------------*/
struct ui_text {
    struct element elm;
    struct ui_text_attrs attrs;	// 文本属性
    char source[8];	// 数据源
    u16 timer;	// 定时器id，用作文本滚动显示
    u16 _str[UI_TEXT_LIST_MAX_NUM];
    char _format[7];	// 编码格式
    u8 str_num;
    u8 index;	// 文本索引
    u8 page;
    // u8 str_num:4;
    // u8 css_num:4;
    // u32 css[2];
    // u16 attr_color;
    // u16 attr_highlight_color;
    // struct ui_text_attrs attrs;
    struct element_luascript_t *lua;
    const struct ui_text_info *info;
    const struct element_event_handler *handler;
    u8 scroll;				//滚动使能
    u8 scroll_start_cnt;	//滚动前计数：计数值*滚动间隔=滚动前停止时间
    u8 scroll_end_cnt;		//滚动后计数：同上
    u8 scroll_real_cnt;		//滚动实时计数值
};



/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_enable 文本控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_text_enable();

/* ------------------------------------------------------------------------------------*/
/**
 * @brief new_ui_text 创建文本控件（内部调用）
 *
 * @param _info 文本控件信息
 * @param parent 父控件元素
 *
 * @return 文本控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void *new_ui_text(const void *_info, struct element *parent);



/****************************** api of format 'ascii' ******************************/

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_str 文本控件显示ASCII编码字符串
 *
 * @param text 文本控件句柄
 * @param format 文本编码格式
 * @param str 文本字符串(注意：字符串buf生命周期必须与控件一致)
 * @param strlen 文本字符串长度
 * @param flags 显示标志
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_str(struct ui_text *text, const char *format, const char *str, int strlen, u32 flags);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_utf8_str 设置文本控件显示UTF-8编码字符串
 *
 * @param text 文本控件句柄
 * @param format 文本编码格式
 * @param str 文本字符串(注意：字符串buf生命周期必须与控件一致)
 * @param strlen 文本字符串长度
 * @param flags 显示标志
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_utf8_str(struct ui_text *text, const char *format, const char *str, int strlen, u32 flags);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_str_by_id 设置文本控件显示字符串（带redraw）
 *
 * @param id 文本控件ID
 * @param format 文本编码格式
 * @param str 文本字符串指针(注意：字符串buf生命周期必须与控件一致)
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_str_by_id(int id, const char *format, const char *str);





/****************************** api of format 'strpic' ******************************/

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_index 设置文本控件显示的文本索引
 *
 * @param text 文本控件句柄
 * @param index 文本在“文字列表”中的索引
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_index(struct ui_text *text, int index);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_show_index_by_id 设置文本控件显示的文本索引
 *
 * @param id 文本控件ID
 * @param index 文本在“文字列表”中的索引
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_show_index_by_id(int id, int index);




/****************************** api of format 'text' ******************************/

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_text_attrs 设置文本控件属性
 *
 * @param text 文本控件句柄
 * @param str 文本字符串指针(注意：字符串buf生命周期必须与控件一致)
 * @param strlen 文本字符串长度
 * @param encode 文本字符串编码格式
 * @param endian 文本字符串大、小端
 * @param flags 显示标志
 */
/* ------------------------------------------------------------------------------------*/
void ui_text_set_text_attrs(struct ui_text *text, const char *str, int strlen, u8 encode, u8 endian, u32 flags);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_text_by_id 设置指定ID的文本控件显示内容
 *
 * @param id 文本控件ID
 * @param str 字符串指针(注意：字符串buf生命周期必须与控件一致)
 * @param strlen 字符串长度
 * @param flags 显示标志
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_text_by_id(int id, const char *str, int strlen, u32 flags);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_textw_by_id 设置指定ID的文本控件显示宽字节字符串
 *
 * @param id 文本控件ID
 * @param str 宽字节字符串(注意：字符串buf生命周期必须与控件一致)
 * @param strlen 字符串长度
 * @param endian 存储格式（大端、小端）
 * @param flags 显示标志
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_textw_by_id(int id, const char *str, int strlen, int endian, u32 flags);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_textu_by_id 设置指定ID的文本控件显示UTF8编码字符串
 *
 * @param id 文本控件ID
 * @param str UTF8编码字符串(注意：字符串buf生命周期必须与控件一致)
 * @param strlen 字符串长度
 * @param flags 显示标志
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_textu_by_id(int id, const char *str, int strlen, u32 flags);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief text_release 释放文本控件
 *
 * @param text 文本控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void text_release(struct ui_text *text);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_combine_index 设置文本控件显示多个excel文本拼接字符串
 *
 * @param text 文本控件句柄
 * @param store_buf 必须是全局或者静态，不能是局部buf，大小为index_num+1
 * @param index_buf 表示当前文本控件字符串ID的序号，从0开始
 * @param index_num 表示有多少个字符串ID拼接起来
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_combine_index(struct ui_text *text, u16 *store_buf, u8 *index_buf, int index_num);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_multi_text_index 设置文本控件拼接显示excel文本
 *
 * @param text 文本控件指针
 * @param index_buf 当前文本控件显示的文本索引列表
 * @param index_num 文本列表中有多少个文本拼接显示
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_multi_text_index(struct ui_text *text, u8 *index_buf, int index_num);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_hide_by_id 设置为文本控件的隐藏状态
 *
 * @param id 文本控件ID
 * @param hide 是否设置为隐藏状态（true 隐藏，false 显示）
 *
 * @return 0 正常， -22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_text_set_hide_by_id(int id, int hide);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief text_set_strpic_scroll_interval 设置strpic文本滚动时间间隔
 *
 * @param ms 默认250ms
 */
/* ------------------------------------------------------------------------------------*/
void text_set_strpic_scroll_interval(u16 ms);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief text_set_font_scroll_interval 设置字库文本滚动间隔
 *
 * @param ms 默认1000ms
 */
/* ------------------------------------------------------------------------------------*/
void text_set_font_scroll_interval(u16 ms);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_get_strpic_scroll_interval 获取strpic文本滚动时间间隔
 *
 * @return 间隔
 */
/* ------------------------------------------------------------------------------------*/
u16 ui_text_get_strpic_scroll_interval();

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_get_font_scroll_interval 获取字库文本滚动间隔
 *
 * @return 间隔
 */
/* ------------------------------------------------------------------------------------*/
u16 ui_text_get_font_scroll_interval();

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_text_set_default_scroll_cnt 设置文本滚动前和滚动结束的静止等待时间
 *
 * @param strpic_start_cnt
 * @param strpic_end_cnt
 * @param font_start_cnt
 * @param font_end_cnt
 *
 * @note 0不等待
 */
/* ------------------------------------------------------------------------------------*/
void ui_text_set_default_scroll_cnt(u8 strpic_start_cnt, u8 strpic_end_cnt, u8 font_start_cnt, u8 font_end_cnt);


#endif
