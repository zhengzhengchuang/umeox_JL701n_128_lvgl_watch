#include "ui_tileview.h"
#include "../lv_watch.h"
#include "../poc_modem/poc_modem_ui.h"
#include "../poc_modem/poc_modem_cache.h"

#if UI_USE_TILEVIEW

void tileview_info_clear(void)
{
    memset(&p_ui_info_cache->ui_tileview_info, \
        0, sizeof(ui_tileview_info_t));

    return;
}

lv_obj_t *tileview_obj_container(void)
{
    return (p_ui_info_cache->ui_tileview_info.tileview_obj);
}

lv_obj_t *tileview_get_center_menu(void)
{
    return (p_ui_info_cache->ui_tileview_info.tileview_center_menu);
}

static void tileview_register_up_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;
  
    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].x = 1;
    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].y = 0;
    p_ui_info_cache->ui_tileview_info.tileview_id_cache[valid_cnt] = act_id; 
    p_ui_info_cache->ui_tileview_info.tileview_valid_dir |= (0x01<<2);
    p_ui_info_cache->ui_tileview_info.tileview_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_tileview_info.tileview_valid_cnt++;

    return;
}

static void tileview_register_down_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;

    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].x = 1;
    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].y = 2;
    p_ui_info_cache->ui_tileview_info.tileview_id_cache[valid_cnt] = act_id;
    p_ui_info_cache->ui_tileview_info.tileview_valid_dir |= (0x01<<3);
    p_ui_info_cache->ui_tileview_info.tileview_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_tileview_info.tileview_valid_cnt++;

    return;
}

static void tileview_register_left_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;

    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].x = 0;
    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].y = 1;
    p_ui_info_cache->ui_tileview_info.tileview_id_cache[valid_cnt] = act_id;
    p_ui_info_cache->ui_tileview_info.tileview_valid_dir |= (0x01<<0);
    p_ui_info_cache->ui_tileview_info.tileview_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_tileview_info.tileview_valid_cnt++;

    return;
}

static void tileview_register_right_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;

    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].x = 2;
    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].y = 1;
    p_ui_info_cache->ui_tileview_info.tileview_id_cache[valid_cnt] = act_id;
    p_ui_info_cache->ui_tileview_info.tileview_valid_dir |= (0x01<<1);
    p_ui_info_cache->ui_tileview_info.tileview_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_tileview_info.tileview_valid_cnt++;

    return;
}

static void tileview_register_center_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;

    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].x = 1;
    p_ui_info_cache->ui_tileview_info.tileview_dir_points[valid_cnt].y = 1; 
    p_ui_info_cache->ui_tileview_info.tileview_id_cache[valid_cnt] = act_id;
    p_ui_info_cache->ui_tileview_info.tileview_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_tileview_info.tileview_valid_cnt++;

    return;
}

static void tileview_event_cb(lv_event_t *e)
{
    if(!e) return;

    static uint8_t event_filter = 0;
    event_filter++;
    event_filter %= 2;
    if(event_filter) return;

    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *tile_act = lv_tileview_get_tile_act(obj);
    if(tile_act == p_ui_info_cache->ui_tileview_info.tileview_center_menu)
        return;

    ui_act_id_t new_act_id = ui_act_id_null;
    lv_obj_t *menu_load_container = NULL;
    uint8_t tileview_valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;
    for(uint8_t i = 0; i < tileview_valid_cnt; i++)
    {
        menu_load_container = p_ui_info_cache->ui_tileview_info.tileview_side_menu_cache[i];
        if(menu_load_container && tile_act == menu_load_container)
        {
            new_act_id = p_ui_info_cache->ui_tileview_info.tileview_id_cache[i];
            if(ui_act_id_validity(new_act_id))
                ui_menu_jump(new_act_id);
        }       
    }

    return;
}

static void tileview_menu_create(lv_obj_t *obj)
{
    if(!obj) return;

    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    lv_obj_t *tileview_menu = lv_tileview_create(obj);
    lv_obj_set_size(tileview_menu, lcd_w, lcd_h);
    lv_obj_center(tileview_menu);
    lv_obj_set_style_bg_color(tileview_menu, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_remove_style(tileview_menu, NULL, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_remove_style(tileview_menu, NULL, LV_PART_SCROLLBAR|LV_STATE_PRESSED);

    p_ui_info_cache->ui_tileview_info.tileview_obj = tileview_menu;
    
    lv_point_t tileview_dir_point = {0};
    lv_obj_t *menu_load_container = NULL;
    ui_menu_load_info_t *menu_load_info = NULL;
    uint8_t tileview_valid_dir = p_ui_info_cache->ui_tileview_info.tileview_valid_dir;
    uint8_t tileview_valid_cnt = p_ui_info_cache->ui_tileview_info.tileview_valid_cnt;

    for(uint8_t i = 0; i < tileview_valid_cnt; i++)
    {
        menu_load_info = p_ui_info_cache->ui_tileview_info.tileview_menu_load[i];
        tileview_dir_point.x = p_ui_info_cache->ui_tileview_info.tileview_dir_points[i].x;
        tileview_dir_point.y = p_ui_info_cache->ui_tileview_info.tileview_dir_points[i].y;
        menu_load_container = lv_tileview_add_tile(tileview_menu, tileview_dir_point.x, \
            tileview_dir_point.y, tileview_valid_dir);
        if(menu_load_info)
        {
            if(menu_load_info->display_func_cb)
                menu_load_info->display_func_cb(menu_load_container);
        }

        if(!(tileview_dir_point.x == 1 && tileview_dir_point.y == 1))
            p_ui_info_cache->ui_tileview_info.tileview_side_menu_cache[i] = menu_load_container;
        else
            p_ui_info_cache->ui_tileview_info.tileview_center_menu = menu_load_container;
    }
    
    lv_obj_set_tile_id(tileview_menu, 1, 1, LV_ANIM_OFF);

    lv_obj_add_event_cb(tileview_menu, tileview_event_cb, LV_EVENT_SCROLL_END, NULL);

    return;
}

/*********如果那个方向没有的话，传入ui_act_id_null即可************/
void tileview_register_all_menu(lv_obj_t *obj, ui_act_id_t up, ui_act_id_t down, \
    ui_act_id_t left, ui_act_id_t right, ui_act_id_t center)
{
    if(!obj) return;

    if(ui_act_id_validity(up))
        tileview_register_up_menu(up);
    
    if(ui_act_id_validity(down))
        tileview_register_down_menu(down);

    if(ui_act_id_validity(left))
        tileview_register_left_menu(left);

    if(ui_act_id_validity(right))
        tileview_register_right_menu(right); 

    if(ui_act_id_validity(center))
        tileview_register_center_menu(center); 

    tileview_menu_create(obj);

    return;
}
#endif
