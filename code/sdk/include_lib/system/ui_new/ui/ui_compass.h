/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_compass.h
 * @brief 指南针控件API接口头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */

#ifndef UI_COMPASS_H
#define UI_COMPASS_H


#include "ui/control.h"
#include "ui/ui_core.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 指南针控件的子控件数量
 */
/* ------------------------------------------------------------------------------------*/
#define COMPASS_CHILD_NUM     (CTRL_COMPASS_CHILD_END - CTRL_COMPASS_CHILD_BEGIN)


struct compass_pic_info {
    struct ui_ctrl_info_head head;
    u16 cent_x;
    u16 cent_y;
    struct ui_image_list *img;
};

struct compass_css_info {
    int left: 16;
    int top: 16;
    int width: 16;
    int height: 16;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief 指南针控件句柄结构
 */
/* ------------------------------------------------------------------------------------*/
struct ui_compass {
    struct element elm;
    struct element child_elm[COMPASS_CHILD_NUM];
    struct compass_css_info child_css[COMPASS_CHILD_NUM];
    char source[8];
    int bk_angle : 16;
    int indicator_angle : 16;
    int last_bk_angle : 16;
    int last_indicator_angle : 16;
    u8 updata;
    u8 ctrl_num;
    void *timer;
    const struct layout_info *info;
    const struct compass_pic_info *pic_info[COMPASS_CHILD_NUM];
    const struct element_event_handler *handler;
    const struct element_luascript_t *lua;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_compass_enable 指南针控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_compass_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_compass_update 刷新指南针控件
 *
 * @param compass 指南针控件句柄
 * @param refresh 是否redraw(true redraw, false not redraw)
 */
/* ------------------------------------------------------------------------------------*/
void ui_compass_update(struct ui_compass *compass, u8 refresh);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_compass_set_angle_by_id 设置指南针角度(带redraw)
 *
 * @param id 指南针控件ID
 * @param bk_angle 背景角度
 * @param indicator_angle 指针角度
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_compass_set_angle_by_id(int id, int bk_angle, int indicator_angle);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_compass_set_angle 设置指南针角度(不带redraw)
 *
 * @param compass 指南针控件句柄
 * @param bk_angle 背景图角度
 * @param indicator_angle 指针角度
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_compass_set_angle(struct ui_compass *compass, int bk_angle, int indicator_angle);

#endif


