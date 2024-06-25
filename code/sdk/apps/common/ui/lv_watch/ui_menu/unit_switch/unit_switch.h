#ifndef __UNIT_SWITCH_H__
#define __UNIT_SWITCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#if 0
/****************单位距离选项数量****************/
#define Unit_Distance_Sel_Num (unit_distance_num)

/****************单位距离选项数量****************/
#define Unit_Temperature_Sel_Num (unit_temperature_num)


/****************单位切换参数内容****************/
typedef struct
{
    lv_obj_t *unit_switch_container;
 
    lv_obj_t *unit_distance_img[\
        Unit_Distance_Sel_Num];
    lv_obj_t *unit_distance_label[\
        Unit_Distance_Sel_Num];
    uint16_t unit_distance_img_dsc[\
        Unit_Distance_Sel_Num];

    lv_obj_t *unit_temperature_img[\
        Unit_Temperature_Sel_Num];
    lv_obj_t *unit_temperature_label[\
        Unit_Temperature_Sel_Num];
    uint16_t unit_temperature_img_dsc[\
        Unit_Temperature_Sel_Num];
}unit_switch_ctx_t;
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
