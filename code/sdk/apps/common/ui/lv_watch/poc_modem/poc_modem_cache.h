#ifndef __POC_MODEM_CACHE_H__
#define __POC_MODEM_CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../include/ui_act_id.h"
#include "../ui_cover/ui_cover.h"
#include "../ui_tileview/ui_tileview.h"
#include "../ui_translate/ui_translate.h"

/*********************************************************************************
                             菜单返回级数                           
*********************************************************************************/
#define Menu_Return_Level (10)

/*********************************************************************************
                             ui模式
                    表盘交互、菜单列表交互、工具盒子交互                          
*********************************************************************************/
enum
{
    ui_mode_watchface = 0x00,
    ui_mode_menu_list,
    ui_mode_tool_box,
};
typedef uint8_t ui_mode_t;

/*********************************************************************************
                             cache缓存信息，关机会被冲刷掉                           
*********************************************************************************/
typedef struct
{
    ui_mode_t ui_mode;

    ui_act_id_t cur_act_id;
    ui_act_id_t prev_act_id[\
        Menu_Return_Level];
    uint8_t menu_return_level;

    lv_obj_t *ui_menu_container;

#if UI_USE_COVER
    ui_cover_info_t ui_cover_info;
#endif

    ui_menu_load_info_t menu_load_info;
    ui_menu_load_info_t exit_menu_load_info;

#if UI_USE_TILEVIEW
    ui_tileview_info_t ui_tileview_info;
#endif

#if UI_USE_TRANSLATE
    ui_translate_info_t ui_translate_info;
#endif
}ui_info_cache_t;
extern ui_info_cache_t *p_ui_info_cache;

void ui_info_cache_init(ui_act_id_t act_id);
#ifdef __cplusplus
}
#endif

#endif
