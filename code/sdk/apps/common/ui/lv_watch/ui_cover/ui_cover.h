#ifndef __UI_COVER_H__
#define __UI_COVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_conf.h"
#include "../include/ui_menu.h"
#include "../include/ui_act_id.h"

#if UI_USE_COVER

#define COVER_MENU_NUM (2)//0:背景层 1:前景层

typedef struct
{
    bool cover_animing;
    lv_point_t cover_scroll_sum;
    lv_dir_t cover_register_scroll_dir;//ui页面注册的滚动方向
    lv_dir_t cover_candidate_scroll_dir;//实际操作的候选滚动方向
    ui_act_id_t cover_id_cache[COVER_MENU_NUM];
    lv_obj_t *cover_menu_cache[COVER_MENU_NUM];
    ui_menu_load_info_t *cover_menu_load[COVER_MENU_NUM];
}ui_cover_info_t;

void cover_info_clear(void);
void cover_register_all_menu(lv_obj_t *obj, ui_act_id_t bg, \
    ui_act_id_t fg, lv_dir_t dir);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
