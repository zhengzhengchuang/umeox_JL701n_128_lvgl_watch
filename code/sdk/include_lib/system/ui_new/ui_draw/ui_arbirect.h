#ifndef UI_ARBIRECT_H
#define UI_ARBIRECT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ui_draw/ui_obj.h"

#if UI_USE_ARBIRECT != 0

typedef struct {

    struct rect coords_rect;//相对背景的位置和范围,不会随着移动而变化
    ui_style_int_t radius;//如果为0，则是普通矩形，否则圆角矩形

    ui_color_t bg_color;//填充矩形的颜色
    struct rect *bg_limit_rect; //(必须静态或者全局)填充矩形限制画的区域,可用于局部显示背景，一般用做矩形百分比显示,如果为空则表示不限制

    ui_color_t border_color;//边框颜色
    ui_style_int_t border_width;//边框宽度,如果border_en使能，则该值不能小于2

    uint8_t bg_en : 1;//填充矩形使能
    uint8_t border_en : 1;//边框矩形使能

} arbirect_info_t;





typedef struct {
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    ui_style_int_t radius;

    ui_color_t bg_color;//背景的颜色
    ui_opa_t bg_opa;
    struct rect *bg_limit_rect; //背景限制画的区域,可用于局部显示背景，一般用做矩形百分比显示,如果为空则表示不限制

    ui_color_t border_color;
    ui_style_int_t border_width;
    ui_opa_t border_opa;

} ui_arbirect_ext_t;




/* ------------------------------------------------------------------------------------*/
/**
 * @brief draw_arbirect 圆角矩形
 *
 * @param info		矩形info
 * @param buf		屏显buf
 * @param buflen	屏显buflen
 * @param disp		屏显区域
 * @param ctrl		绘制区域
 * @param draw_id	圆角矩形id
 */
/* ------------------------------------------------------------------------------------*/
void draw_arbirect(const arbirect_info_t *info, u8 *buf, u32 buflen, struct rect *disp, struct rect *ctrl, int draw_id);







#endif /*UI_USE_ARBIRECT*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UI_ARBIRECT_H*/



