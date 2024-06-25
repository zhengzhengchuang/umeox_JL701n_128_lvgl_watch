#include "ui_cover.h"
#include "../lv_watch.h"
#include "../poc_modem/poc_modem_ui.h"
#include "../poc_modem/poc_modem_cache.h"

#if UI_USE_COVER

static lv_anim_t cover_anim;
static const int16_t cover_scroll_limit = 10;

void cover_info_clear(void)
{
    memset(&p_ui_info_cache->ui_cover_info, 0, sizeof(ui_cover_info_t));

    return;
}

static void cover_register_bg_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    p_ui_info_cache->ui_cover_info.cover_id_cache[0] = act_id;
    p_ui_info_cache->ui_cover_info.cover_menu_load[0] = menu_load_info;

    return;
}

static void cover_register_fg_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    p_ui_info_cache->ui_cover_info.cover_id_cache[1] = act_id;
    p_ui_info_cache->ui_cover_info.cover_menu_load[1] = menu_load_info;

    return;
}

static void cover_menu_scroll_judge(void)
{
    bool hor_scroll_en = false;
    bool ver_scroll_en = false;
    lv_point_t vert_point = {0};

    lv_indev_get_vect(lv_indev_get_act(), &vert_point);

    lv_point_t *cover_scroll_sum = \
        &p_ui_info_cache->ui_cover_info.cover_scroll_sum;

    cover_scroll_sum->x += vert_point.x;
    cover_scroll_sum->y += vert_point.y;

    if(LV_ABS(cover_scroll_sum->x) > LV_ABS(cover_scroll_sum->y))
        hor_scroll_en = true;
    else
        ver_scroll_en = true;

    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    if(hor_scroll_en && LV_ABS(cover_scroll_sum->x) > cover_scroll_limit)
    {
        if(cover_scroll_sum->x < 0)
            *cover_candidate_scroll_dir = LV_DIR_LEFT;
        else
           *cover_candidate_scroll_dir = LV_DIR_RIGHT; 
    }else if(ver_scroll_en && LV_ABS(cover_scroll_sum->y) > cover_scroll_limit)
    {
        if(cover_scroll_sum->y < 0)
            *cover_candidate_scroll_dir = LV_DIR_TOP;
        else
           *cover_candidate_scroll_dir = LV_DIR_BOTTOM;
    }else
        *cover_candidate_scroll_dir = LV_DIR_NONE;

    if(*cover_candidate_scroll_dir != LV_DIR_NONE)
        memset(cover_scroll_sum, 0, sizeof(lv_point_t));

    return;
}

static void cover_menu_scroll_handle(void)
{
    lv_point_t vert_point = {0};

    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    lv_indev_get_vect(lv_indev_get_act(), &vert_point);

    lv_point_t *cover_scroll_sum = \
        &p_ui_info_cache->ui_cover_info.cover_scroll_sum;

    cover_scroll_sum->x += vert_point.x;
    cover_scroll_sum->y += vert_point.y;

    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    lv_obj_t *cover_menu_cache[COVER_MENU_NUM];
    for(uint8_t i = 0; i < COVER_MENU_NUM; i++)
        cover_menu_cache[i] = \
            p_ui_info_cache->ui_cover_info.cover_menu_cache[i];

    lv_obj_t *cover_fg_menu = cover_menu_cache[1];
    if(!cover_fg_menu) return;

    int16_t cover_scroll_min = 0;
    int16_t cover_scroll_max = 0;

    if(*cover_candidate_scroll_dir == LV_DIR_LEFT)
    {
        cover_scroll_min = -lcd_w;
        cover_scroll_max = 0;
    }else if(*cover_candidate_scroll_dir == LV_DIR_RIGHT)
    {
        cover_scroll_min = 0;
        cover_scroll_max = lcd_w;
    }else if(*cover_candidate_scroll_dir == LV_DIR_TOP)
    {
        cover_scroll_min = -lcd_h;
        cover_scroll_max = 0;
    }else if(*cover_candidate_scroll_dir == LV_DIR_BOTTOM)
    {
        cover_scroll_min = 0;
        cover_scroll_max = lcd_h;
    }else
        return;

    if(*cover_candidate_scroll_dir == LV_DIR_LEFT || \
        *cover_candidate_scroll_dir == LV_DIR_RIGHT)
    {
        cover_scroll_sum->x = cover_scroll_sum->x < cover_scroll_min? \
            cover_scroll_min:cover_scroll_sum->x;
        cover_scroll_sum->x = cover_scroll_sum->x > cover_scroll_max? \
            cover_scroll_max:cover_scroll_sum->x;

        lv_obj_set_style_translate_x(cover_fg_menu, cover_scroll_sum->x, LV_PART_MAIN);
    }else
    {
        cover_scroll_sum->y = cover_scroll_sum->y < cover_scroll_min? \
            cover_scroll_min:cover_scroll_sum->y;
        cover_scroll_sum->y = cover_scroll_sum->y > cover_scroll_max? \
            cover_scroll_max:cover_scroll_sum->y;

        lv_obj_set_style_translate_y(cover_fg_menu, cover_scroll_sum->y, LV_PART_MAIN);
    }

    return;
}

static void cover_menu_anim_cb(void *var, int32_t anim_val)
{
    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    lv_obj_t *cover_menu_cache[COVER_MENU_NUM];
    for(uint8_t i = 0; i < COVER_MENU_NUM; i++)
        cover_menu_cache[i] = \
            p_ui_info_cache->ui_cover_info.cover_menu_cache[i];

    lv_obj_t *cover_fg_menu = cover_menu_cache[1];
    if(!cover_fg_menu) return;

    if(*cover_candidate_scroll_dir == LV_DIR_LEFT || \
        *cover_candidate_scroll_dir == LV_DIR_RIGHT)
        lv_obj_set_style_translate_x(cover_fg_menu, anim_val, LV_PART_MAIN);
    else
        lv_obj_set_style_translate_y(cover_fg_menu, anim_val, LV_PART_MAIN);

    return;
}

static void cover_menu_anim_ready_cb(lv_anim_t *a)
{  
    int32_t anim_end_val = a->end_value;
    ui_act_id_t jump_act_id = ui_act_id_null;

    ui_act_id_t *cover_id_cache = \
        &p_ui_info_cache->ui_cover_info.cover_id_cache;

    bool *cover_animing = \
        &p_ui_info_cache->ui_cover_info.cover_animing;

    if(anim_end_val != 0)
       jump_act_id = cover_id_cache[0];

    if(jump_act_id != ui_act_id_null)
        ui_menu_jump(jump_act_id);

    *cover_animing = false;
    
    return;
}

static void cover_menu_released_handle(void)
{
    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    bool *cover_animing = \
        &p_ui_info_cache->ui_cover_info.cover_animing;
  
    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    lv_point_t *cover_scroll_sum = \
        &p_ui_info_cache->ui_cover_info.cover_scroll_sum;

    widget_anim_para.anim_obj = NULL;
    widget_anim_para.anim = &cover_anim;
    widget_anim_para.anim_exec_xcb = cover_menu_anim_cb;

    if(*cover_candidate_scroll_dir == LV_DIR_LEFT)
    {
        widget_anim_para.anim_start_val = cover_scroll_sum->x;

        if(LV_ABS(cover_scroll_sum->x) < lcd_w/4)
            widget_anim_para.anim_end_val = 0;
        else 
            widget_anim_para.anim_end_val = -lcd_w;
    }else if(*cover_candidate_scroll_dir == LV_DIR_RIGHT)
    {
        widget_anim_para.anim_start_val = cover_scroll_sum->x;

        if(LV_ABS(cover_scroll_sum->x) < lcd_w/4)
            widget_anim_para.anim_end_val = 0;
        else 
            widget_anim_para.anim_end_val = lcd_w;
    }else if(*cover_candidate_scroll_dir == LV_DIR_TOP)
    {
        widget_anim_para.anim_start_val = cover_scroll_sum->y;

        if(LV_ABS(cover_scroll_sum->y) < lcd_h/4)
            widget_anim_para.anim_end_val = 0;
        else 
            widget_anim_para.anim_end_val = -lcd_h;
    }else if(*cover_candidate_scroll_dir == LV_DIR_BOTTOM)
    {
        widget_anim_para.anim_start_val = cover_scroll_sum->y;

        if(LV_ABS(cover_scroll_sum->y) < lcd_h/4)
            widget_anim_para.anim_end_val = 0;
        else 
            widget_anim_para.anim_end_val = lcd_h;
    }

    uint16_t anim_diff = LV_ABS(widget_anim_para.anim_start_val - \
                    widget_anim_para.anim_end_val);
    uint32_t anim_duration = anim_diff*2;
    anim_duration = anim_duration == 0?2:anim_duration;
    anim_duration = anim_duration > 300?300:anim_duration;
    widget_anim_para.anim_duration = anim_duration;
    widget_anim_para.is_start_anim = false;
    common_widget_anim_create(&widget_anim_para);
    lv_anim_set_path_cb(widget_anim_para.anim, lv_anim_path_ease_out);
    lv_anim_set_ready_cb(widget_anim_para.anim, cover_menu_anim_ready_cb);
    lv_anim_start(widget_anim_para.anim);

    *cover_animing = true;

    return;
}

static void cover_menu_pressed_cb(lv_event_t *e)
{
    bool *cover_animing = \
        &p_ui_info_cache->ui_cover_info.cover_animing;

    if(*cover_animing) return;

    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    *cover_candidate_scroll_dir = LV_DIR_NONE;

    lv_point_t *cover_scroll_sum = \
        &p_ui_info_cache->ui_cover_info.cover_scroll_sum;

    memset(cover_scroll_sum, 0, sizeof(lv_point_t));

    return;
}

static void cover_menu_pressing_cb(lv_event_t *e)
{
    bool *cover_animing = \
        &p_ui_info_cache->ui_cover_info.cover_animing;

    if(*cover_animing) return;

    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    lv_dir_t *cover_register_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_register_scroll_dir;

    if(*cover_candidate_scroll_dir != LV_DIR_NONE && \
        *cover_candidate_scroll_dir != *cover_register_scroll_dir)
        return;

    if(*cover_candidate_scroll_dir == LV_DIR_NONE)
        cover_menu_scroll_judge();
    else
        cover_menu_scroll_handle();

    return;
}

static void cover_menu_released_cb(lv_event_t *e)
{
    bool *cover_animing = \
        &p_ui_info_cache->ui_cover_info.cover_animing;

    if(*cover_animing) return;

    lv_dir_t *cover_candidate_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_candidate_scroll_dir;

    if(*cover_candidate_scroll_dir == LV_DIR_NONE)
        return;

    cover_menu_released_handle();

    return;
}

static void cover_menu_event_cb(lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        cover_menu_pressed_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        cover_menu_pressing_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        cover_menu_released_cb(e);

    return;
}

static void cover_menu_create(lv_obj_t *obj)
{
    if(!obj) return;
    
    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);
    ui_menu_load_info_t *cover_menu_load = NULL;

    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = 0;
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_width = lcd_w;
    widget_obj_para.obj_height = lcd_h;
    widget_obj_para.obj_bg_opax = LV_OPA_100;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;

    /*******************背景菜单创建*******************/
    cover_menu_load = p_ui_info_cache->ui_cover_info.cover_menu_load[0];
    lv_obj_t *cover_bg_container = common_widget_obj_create(&widget_obj_para);  
    if(cover_menu_load)
    {
        if(cover_menu_load->display_func_cb)
            cover_menu_load->display_func_cb(cover_bg_container);
    }
    p_ui_info_cache->ui_cover_info.cover_menu_cache[0] = cover_bg_container;

    /*******************前景菜单创建*******************/
    cover_menu_load = p_ui_info_cache->ui_cover_info.cover_menu_load[1];
    lv_obj_t *cover_fg_container = common_widget_obj_create(&widget_obj_para);
    if(cover_menu_load)
    {
        if(cover_menu_load->display_func_cb)
            cover_menu_load->display_func_cb(cover_fg_container);
    }
    p_ui_info_cache->ui_cover_info.cover_menu_cache[1] = cover_fg_container;

    lv_obj_add_event_cb(cover_fg_container, cover_menu_event_cb, LV_EVENT_ALL, NULL);

    return;
}

void cover_register_all_menu(lv_obj_t *obj, ui_act_id_t bg, \
    ui_act_id_t fg, lv_dir_t dir)
{
    if(ui_act_id_validity(bg))
        cover_register_bg_menu(bg);

    if(ui_act_id_validity(fg))
        cover_register_fg_menu(fg);

    if(dir == LV_DIR_NONE) return;

    lv_dir_t *cover_register_scroll_dir = \
        &p_ui_info_cache->ui_cover_info.cover_register_scroll_dir;

    *cover_register_scroll_dir = dir;

    cover_menu_create(obj);

    return;
}
#endif
