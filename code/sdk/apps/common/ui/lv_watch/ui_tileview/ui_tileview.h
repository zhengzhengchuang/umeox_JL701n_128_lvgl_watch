#ifndef __UI_TILEVIEW_H__
#define __UI_TILEVIEW_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_conf.h"
#include "../include/ui_menu.h"

#if UI_USE_TILEVIEW

/********上、下、左、右、中心********/
#define Tileview_Dir_Num (5)

typedef struct
{
    /*tileview对象*/
    lv_obj_t *tileview_obj;

    uint8_t tileview_valid_dir;

    /*可视化个数*/
    uint8_t tileview_valid_cnt;

    /*中心菜单容器*/
    lv_obj_t *tileview_center_menu;

    /*可视化菜单容器*/
    ui_act_id_t tileview_id_cache[Tileview_Dir_Num];
    lv_point_t tileview_dir_points[Tileview_Dir_Num];
    lv_obj_t *tileview_side_menu_cache[Tileview_Dir_Num-1];//tileview四边容器 上、下、左、右
    ui_menu_load_info_t *tileview_menu_load[Tileview_Dir_Num]; 
}ui_tileview_info_t;

void tileview_info_clear(void);
lv_obj_t *tileview_obj_container(void);
lv_obj_t *tileview_get_center_menu(void);
void tileview_register_all_menu(lv_obj_t *obj, ui_act_id_t up, ui_act_id_t down, \
    ui_act_id_t left, ui_act_id_t right, ui_act_id_t center);
#endif

#ifdef __cplusplus
}
#endif

#endif
