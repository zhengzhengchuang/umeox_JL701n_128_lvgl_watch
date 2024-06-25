/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_progress.h
 * @brief 圆弧进度条控件API接口头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_PROGRESS_H
#define UI_PROGRESS_H


#include "ui/control.h"
#include "ui/ui_core.h"

/* ------------------------------------------------------------------------------------*/
/**
 * @brief 圆弧进度条控件子控件数量定义
 */
/* ------------------------------------------------------------------------------------*/
#define PROGRESS_CHILD_NUM     (CTRL_PROGRESS_CHILD_END - CTRL_PROGRESS_CHILD_BEGIN)


struct progress_highlight_info {
    struct ui_ctrl_info_head head;
    u16 center_x;
    u16 center_y;
    u16 radius_big;
    u16 radius_small;
    u32 color;
    u16 angle_begin;
    u16 angle_end;
    struct ui_image_list *img;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 圆弧进度条控件句柄结构定义
 */
/* ------------------------------------------------------------------------------------*/
struct ui_progress {
    struct element elm;
    struct element child_elm[PROGRESS_CHILD_NUM];
    char source[8];	// 数据源
    u16 center_x;	// 旋转中心x坐标
    u16 center_y;	// 旋转中心y坐标
    u16 radius;		// 半径
    u16 angle_begin;	// 起始角度
    u16 angle_end;		// 结束角度
    u8 ctrl_num;	// 控件数量
    char percent;	// 百分比
    u8 *mask;
    u16 mask_len;
    void *timer;
    const struct layout_info *info;
    const struct progress_highlight_info *pic_info[PROGRESS_CHILD_NUM];
    const struct element_event_handler *handler;
    const struct element_luascript_t *lua;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_progress_enable 圆弧进度条控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_progress_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_progress_set_persent_by_id 设置圆弧进度条百分比（带redraw）
 *
 * @param id 圆弧进度条控件ID
 * @param persent 圆弧进度条显示的百分比
 *
 * @return 0 正常，-22控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_progress_set_persent_by_id(int id, int persent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_progress_set_persent 设置圆弧进度条百分比（不带redraw）
 *
 * @param progress 圆弧进度条句柄
 * @param percent 圆弧进度条显示的百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_progress_set_persent(struct ui_progress *progress, int percent);



#endif


