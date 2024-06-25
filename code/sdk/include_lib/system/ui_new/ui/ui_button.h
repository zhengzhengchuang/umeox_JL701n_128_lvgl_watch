/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_button.h
 * @brief 按钮控件API头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_BUTTON_H
#define UI_BUTTON_H


#include "ui/control.h"
#include "ui/ui_core.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 按钮控件句柄结构
 */
/* ------------------------------------------------------------------------------------*/
struct button {
    struct element elm;
    u8 image_index;
    u8 css_num;
    u8 page;
    u32 css[2];
    struct element_luascript_t *lua;
    const struct ui_button_info *info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_button_enable 按钮控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_button_enable();

#endif

