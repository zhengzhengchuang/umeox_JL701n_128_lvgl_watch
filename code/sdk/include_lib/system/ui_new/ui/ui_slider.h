/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_slider.h
 * @brief 水平进度条控件API接口头文件
 * @author zhuhaifang@zh-jieli.com
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_SLIDER_H
#define UI_SLIDER_H

#include "ui/ui_core.h"
#include "ui/control.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 定义水平进度条控件子控件数量
 */
/* ------------------------------------------------------------------------------------*/
#define SLIDER_CHILD_NUM 	 (SLIDER_CHILD_END - SLIDER_CHILD_BEGIN)


struct slider_text_info {
    u8 move;
    int min_value;
    int max_value;
    int text_color;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 水平进度条控件句柄结构
 */
/* ------------------------------------------------------------------------------------*/
struct ui_slider {
    struct element elm;
    struct element child_elm[SLIDER_CHILD_NUM];
    u8 step;	// 步进量
    u8 move;	// 划动使能（true使能，false不使能）
    u8 follow_pic;	// right_pic和left_pic分界跟随滑块中线（注意，使能它将进度0%和100%时不被填满，适用滑块有图片的场景）
    char persent;	// 百分比
    s16 left;	// selected的left
    s16 width;	// selected的width
    s16 min_value;	// 最小值
    s16 max_value;	// 最大值
    u16 text_color;
    struct element_luascript_t *lua;
    const struct ui_slider_info *info;
    const struct slider_text_info *text_info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_slider_enable 水平进度条控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_slider_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_slider_set_persent_by_id 设置水平进度条百分比（待redraw）
 *
 * @param id 水平进度条控件ID
 * @param persent 水平进度条显示百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_slider_set_persent_by_id(int id, int persent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_slider_set_persent 设置水平进度条控件百分比（不带redraw）
 *
 * @param slider 水平进度条控件句柄
 * @param persent 水平进度条显示百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_slider_set_persent(struct ui_slider *slider, int persent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief slider_touch_slider_move 水平进度条控件划动（在ON_TOUCH_MOVE事件没有return true时，将move标志设置为真，底层会自动调用该接口）
 *
 * @param slider 水平进度条控件句柄
 * @param e 触摸事件
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int slider_touch_slider_move(struct ui_slider *slider, struct element_touch_event *e);//触摸滑动功能


/* ------------------------------------------------------------------------------------*/
/**
 * @brief slider_get_percent 获取水平进度条控件百分比
 *
 * @param slider 水平进度条控件句柄
 *
 * @return 水平进度条控件百分比值
 */
/* ------------------------------------------------------------------------------------*/
int slider_get_percent(struct ui_slider *slider);




#endif

