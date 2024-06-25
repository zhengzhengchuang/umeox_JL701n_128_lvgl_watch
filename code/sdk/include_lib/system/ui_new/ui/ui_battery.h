/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_battery.h
 * @brief 电池控件
 * @author
 * @version V201
 * @date 2022-12-15
 */

#ifndef UI_BATTERY_H
#define UI_BATTERY_H


#include "ui/control.h"
#include "list.h"



/* ------------------------------------------------------------------------------------*/
/**
 * @brief 电池控件句柄结构
 */
/* ------------------------------------------------------------------------------------*/
struct ui_battery {
    struct element elm;
    int src;
    u8 index;
    u8 page;
    struct ui_image_list *charge_image;
    struct ui_image_list *normal_image;
    struct list_head entry;
    struct element_luascript_t *lua;
    const struct ui_battery_info *info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_battery_enable 电池控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_battery_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_battery_level_change 设置所有电池控件状态
 *
 * @param persent 电量百分比
 * @param incharge 是否正在充电（true 充电状态，false 显示电量百分比）
 */
/* ------------------------------------------------------------------------------------*/
void ui_battery_level_change(int persent, int incharge);//改变所有电池控件


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_battery_set_level_by_id 设置指定电池控件状态（带redraw）
 *
 * @param id 电池控件ID
 * @param persent 电量百分比
 * @param incharge 是否正在充电
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_battery_set_level_by_id(int id, int persent, int incharge);//修改指定id


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_battery_set_level 设置指点电池控件状态（不带redraw）
 *
 * @param battery 电池控件句柄
 * @param persent 电量百分比
 * @param incharge 充电状态
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_battery_set_level(struct ui_battery *battery, int persent, int incharge);//初始化使用

#endif
