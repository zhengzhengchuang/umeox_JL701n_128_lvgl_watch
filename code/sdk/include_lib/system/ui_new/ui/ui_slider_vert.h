/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_slider_vert.h
 * @brief 垂直进度条控件API接口头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_SLIDER_VERT_H
#define UI_SLIDER_VERT_H


#include "ui/ui_core.h"
#include "ui/control.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 定义垂直进度条控件子控件数量
 */
/* ------------------------------------------------------------------------------------*/
#define VSLIDER_CHILD_NUM     (VSLIDER_CHILD_END - VSLIDER_CHILD_BEGIN)


struct vslider_text_info {
    u8 move;
    int min_value;
    int max_value;
    int text_color;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 垂直进度条控件句柄结构定义
 */
/* ------------------------------------------------------------------------------------*/
struct ui_vslider {
    struct element elm;
    struct element child_elm[VSLIDER_CHILD_NUM];
    u8 step;	// 步进
    u8 move;	// 滑动使能标志
    char persent;	// 百分比
    s16 top;	// selected的top
    s16 height;	// selected的height
    u16 min_value;	// 最小值
    u16 max_value;	// 最大值
    u16 text_color;
    struct element_luascript_t *lua;
    const struct ui_slider_info *info;
    const struct vslider_text_info *text_info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_vslider_enable 垂直进度条控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_vslider_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_vslider_set_persent_by_id 设置垂直进度条百分比（待redraw）
 *
 * @param id 垂直进度条控件ID
 * @param persent 垂直进度条控件显示百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_vslider_set_persent_by_id(int id, int persent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_vslider_set_persent 设置垂直进度条控件百分比（不带redraw）
 *
 * @param vslider 垂直进度条控件
 * @param persent 垂直进度条控件显示百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_vslider_set_persent(struct ui_vslider *vslider, int persent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief vslider_touch_slider_move 垂直进度条控件划动回调（如果应用层ON_TOUCH_MOVE没有return true，并且把vslider->move设置为true，底层会自动调用该接口）
 *
 * @param vslider 垂直进度条控件句柄
 * @param e 触摸事件
 *
 * @return false百分比无变化，true百分比改变并已经redraw
 */
/* ------------------------------------------------------------------------------------*/
int vslider_touch_slider_move(struct ui_vslider *vslider, struct element_touch_event *e);//触摸滑动功能


/* ------------------------------------------------------------------------------------*/
/**
 * @brief vslider_get_percent 获取垂直进度条百分比
 *
 * @param vslider 垂直进度条控件句柄
 *
 * @return 垂直进度条当前百分比
 */
/* ------------------------------------------------------------------------------------*/
int vslider_get_percent(struct ui_vslider *vslider);




#endif

