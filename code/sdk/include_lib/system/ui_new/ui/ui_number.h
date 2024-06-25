/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_number.h
 * @brief 数字控件API头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_NUMBER_H
#define UI_NUMBER_H


#include "ui/control.h"
#include "ui/ui_core.h"
#include "ui/p.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 数字控件参数 type 的类型定义，允许显示数字或字符串
 */
/* ------------------------------------------------------------------------------------*/
enum {
    TYPE_NUM,
    TYPE_STRING,
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief 数字控件显示参数
 */
/* ------------------------------------------------------------------------------------*/
struct unumber {
    u8 numbs;		// 参数数量
    u8 type;		// 参数类型

    // 参数类型（type）决定使用下面哪个参数用作显示，number[2]和num_str互斥，数字控件只会显示其中一个
    u32 number[2];	// 数字（允许显示两个数值，中间自动由间隔符隔开）
    u8 *num_str;	// 字符串（将通过ascii字库显示）
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 数字控件元素句柄
 */
/* ------------------------------------------------------------------------------------*/
struct ui_number {
    struct element_text text;
    char source[8];	// 数据源
    u16 number[2];
    u16 buf[20];

    int color;		// 普通颜色
    int hi_color;	// 高亮颜色
    u8 css_num;		// css属性个数
    u8 page;
    u8 nums: 6;
    u8 type: 2;
    u32 css[2];
    u8 *num_str;
    u8 *temp_str;
    u16 temp_str_len;
    struct element_luascript_t *lua;
    const struct ui_number_info *info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_number_enable 数字控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_number_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief new_ui_number 创建新的数字控件（内部调用）
 *
 * @param _info 数字控件信息
 * @param parent 父控件
 *
 * @return 数字控件元素句柄
 */
/* ------------------------------------------------------------------------------------*/
void *new_ui_number(const void *_info, struct element *parent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_number_update 更新数字控件内容（不带redraw）
 *
 * @param number 数字控件句柄
 * @param n 待显示的内容
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_number_update(struct ui_number *number, struct unumber *n);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_number_update_by_id 更新数字控件内容（带redraw）
 *
 * @param id 数字控件ID
 * @param n 待显示内容
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_number_update_by_id(int id, struct unumber *n);


#endif

