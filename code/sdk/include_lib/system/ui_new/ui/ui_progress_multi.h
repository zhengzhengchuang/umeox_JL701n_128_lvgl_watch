/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_progress_multi.h
 * @brief 多重圆弧进度条控件API接口头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_PROGRESS_MULTI_H
#define UI_PROGRESS_MULTI_H


#include "ui/control.h"
#include "ui/ui_core.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 多重圆弧进度条控件子控件数量定义
 */
/* ------------------------------------------------------------------------------------*/
#define MULTIPROGRESS_CHILD_NUM     (CTRL_MULTIPROGRESS_CHILD_END - CTRL_MULTIPROGRESS_CHILD_BEGIN)


struct multiprogress_highlight_info {
    struct ui_ctrl_info_head head;
    u16 number;
    u16 center_x;
    u16 center_y;
    u16 radius0_big;
    u16 radius0_small;
    u32 color0;
    u16 radius1_big;
    u16 radius1_small;
    u32 color1;
    u16 radius2_big;
    u16 radius2_small;
    u32 color2;
    u16 angle_begin;
    u16 angle_end;
    struct ui_image_list *img;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 多重圆弧进度条控件句柄结构定义
 */
/* ------------------------------------------------------------------------------------*/
struct ui_multiprogress {
    struct element elm;
    struct element child_elm[MULTIPROGRESS_CHILD_NUM];
    char source[8];	// 数据源
    u16 center_x;	// 中心x坐标
    u16 center_y;	// 中心y坐标
    u16 radius;		// 半径
    u16 angle_begin;	// 进度条的起始角度
    u16 angle_end;		// 进度条的结束角度
    u8 ctrl_num;	// 控件数量
    char percent[3];	// 百分比（三个进度条）
    u8 circle_num;
    u8 index;
    u8 *mask;
    u16 mask_len;
    void *timer;
    const struct layout_info *info;
    const struct multiprogress_highlight_info *pic_info[MULTIPROGRESS_CHILD_NUM];
    const struct element_event_handler *handler;
    const struct element_luascript_t *lua;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_enable 多重进度条控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_multiprogress_enable();
int ui_multiprogress_set_persent_by_id(int id, int persent);
int ui_multiprogress_set_second_persent_by_id(int id, int percent);
int ui_multiprogress_set_third_persent_by_id(int id, int percent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_set_persent_by_id 设置第一个进度条的百分比（待redraw）
 *
 * @param id 多重进度条控件ID
 * @param persent 第一个进度条显示的百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_multiprogress_set_persent_by_id(int id, int persent);		// 第一个进度条

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_set_second_persent_by_id 设置第二个进度条的百分比（待redraw）
 *
 * @param id 多重进度条控件ID
 * @param persent 第二个进度条显示的百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_multiprogress_set_second_persent_by_id(int id, int percent);	// 第二个进度条

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_set_third_persent_by_id 设置第三个进度条的百分比（待redraw）
 *
 * @param id 多重进度条控件ID
 * @param persent 第三个进度条显示的百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_multiprogress_set_third_persent_by_id(int id, int percent);	// 第三个进度条



/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_set_persent 设置多重圆弧进度条第一个的百分比（不带redraw）
 *
 * @param multiprogress 多重圆弧进度条句柄
 * @param percent 第一个圆弧进度条百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_multiprogress_set_persent(struct ui_multiprogress *multiprogress, int percent);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_set_second_persent 设置多重圆弧进度条第二个的百分比（不带redraw）
 *
 * @param multiprogress 多重圆弧进度条句柄
 * @param percent 第二个圆弧进度条百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_multiprogress_set_second_persent(struct ui_multiprogress *multiprogress, int percent);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_multiprogress_set_third_persent 设置多重圆弧进度条第三个的百分比（不带redraw）
 *
 * @param multiprogress 多重圆弧进度条句柄
 * @param percent 第三个圆弧进度条百分比
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_multiprogress_set_third_persent(struct ui_multiprogress *multiprogress, int percent);



#endif


