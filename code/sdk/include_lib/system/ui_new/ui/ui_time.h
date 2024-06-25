/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_time.h
 * @brief 时间控件API接口头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_TIME_H
#define UI_TIME_H


#include "ui/control.h"
#include "ui/ui_core.h"
#include "ui/p.h"

struct utime {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 min;
    u8 sec;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief 时间控件句柄结构定义
 */
/* ------------------------------------------------------------------------------------*/
struct ui_time {
    struct element_text text;
    char source[8];	// 数据源
    u16 year: 12;	// 年
    u16 month: 4;	// 月
    u8 day;		// 日
    u8 hour;	// 时
    u8 min;		// 分
    u8 sec;		// 秒
    u8 css_num;
    u8 auto_cnt;	// 自动计时标志
    u8 page;
    u32 css[2];
    int color;		// 颜色
    int hi_color;	// 高亮颜色
    u16 buf[20];
    void *timer;
    struct element_luascript_t *lua;
    const struct ui_time_info *info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_time_enable 时间控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_time_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief new_ui_time 创建时间控件（内部调用）
 *
 * @param _info 时间控件信息
 * @param parent 父控件句柄
 *
 * @return 时间控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void *new_ui_time(const void *_info, struct element *parent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_time_update 刷新时间控件的时间（不带redraw）
 *
 * @param time 时间控件句柄
 * @param t 待显示的时间
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_time_update(struct ui_time *time, struct utime *t);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_time_update_by_id 设置时间控件显示的时间（带redraw）
 *
 * @param id 时间控件ID
 * @param time 待显示的时间
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_time_update_by_id(int id, struct utime *time);



#endif
