#ifndef UI_POLY_H
#define UI_POLY_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ui_draw/ui_obj.h"

#if UI_USE_POLY != 0

/**
* @brief
* 1.点的顺序必须是顺时针,并且最少有三个点
* 2.只能是凸多边形，如果要画凹多边形，则需要用户分割成多个凸多边来画
*/
typedef struct {
    struct rect coords_rect;//相对背景的位置和范围,不会随着移动而变化
    const ui_point_t *pntarray;//每个点的坐标是相对coords_rect坐标来说的,该数组必须是全局或者静态
    unsigned cnt;//点的个数, 至少要有三个点
    ui_color_t bg_color;//填充多边形的颜色
    ui_opa_t opa;//alpha
} poly_info_t;



void draw_poly(const poly_info_t *info, u8 *buf, u32 buflen, struct rect *disp, struct rect *ctrl, int draw_id);



#endif /*UI_USE_POLY*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UI_POLY_H*/
