#include "ui_translate.h"
#include "../lv_watch.h"
#include "../poc_modem/poc_modem_ui.h"
#include "../poc_modem/poc_modem_cache.h"

#if UI_USE_TRANSLATE

static lv_anim_t translate_anim;
static uint8_t translate_mode = 0;//0:obj 1:tileview
static const uint8_t up_menu_idx = 0;
static const uint8_t down_menu_idx = 1;
static const uint8_t left_menu_idx = 2;
static const uint8_t right_menu_idx = 3;
static const uint8_t center_menu_idx = 4;
static const int16_t translate_scroll_limit = 10;

void translate_info_clear(void)
{
    memset(&p_ui_info_cache->ui_translate_info, 0, sizeof(ui_translate_info_t));

    return;
}

static void translate_register_up_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].x = 0,
    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].y = -1;
    p_ui_info_cache->ui_translate_info.translate_id_cache[up_menu_idx] = act_id;
    p_ui_info_cache->ui_translate_info.translate_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_translate_info.translate_valid_cnt++;

    return;
}

static void translate_register_down_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].x = 0,
    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].y = 1;
    p_ui_info_cache->ui_translate_info.translate_id_cache[down_menu_idx] = act_id;
    p_ui_info_cache->ui_translate_info.translate_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_translate_info.translate_valid_cnt++;

    return;
}

static void translate_register_left_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].x = -1,
    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].y = 0;
    p_ui_info_cache->ui_translate_info.translate_id_cache[left_menu_idx] = act_id;
    p_ui_info_cache->ui_translate_info.translate_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_translate_info.translate_valid_cnt++;

    return;
}

static void translate_register_right_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].x = 1,
    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].y = 0;
    p_ui_info_cache->ui_translate_info.translate_id_cache[right_menu_idx] = act_id;
    p_ui_info_cache->ui_translate_info.translate_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_translate_info.translate_valid_cnt++;

    return;
}

static void translate_register_center_menu(ui_act_id_t act_id)
{
    ui_menu_load_info_t *menu_load_info = NULL;

    menu_load_info = ui_menu_load_info(act_id);
    if(!menu_load_info) return;

    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].x = 0,
    p_ui_info_cache->ui_translate_info.translate_dir_points[valid_cnt].y = 0;
    p_ui_info_cache->ui_translate_info.translate_id_cache[center_menu_idx] = act_id;
    p_ui_info_cache->ui_translate_info.translate_menu_load[valid_cnt] = menu_load_info;

    p_ui_info_cache->ui_translate_info.translate_valid_cnt++;

    return;
}

/*********如果那个方向没有的话，传入ui_act_id_null即可************/
void translate_register_all_menu(ui_act_id_t up, ui_act_id_t down, ui_act_id_t left, \
    ui_act_id_t right, ui_act_id_t center)
{
    if(ui_act_id_validity(up))
        translate_register_up_menu(up);

    if(ui_act_id_validity(down))
        translate_register_down_menu(down);

    if(ui_act_id_validity(left))
        translate_register_left_menu(left);

    if(ui_act_id_validity(right))
        translate_register_right_menu(right); 

    if(ui_act_id_validity(center))
        translate_register_center_menu(center);
  
    return;
}
/*
    0:上 
    1:下 
    2:左
    3:右
    4:中心
*/
static uint8_t translate_menu_cache_by_point(lv_point_t *dir_point)
{
    if(!dir_point) return 0xff;

    if(dir_point->x == 0 && dir_point->y == -1)
        return up_menu_idx;
    else if(dir_point->x == 0 && dir_point->y == 1)
        return down_menu_idx;
    else if(dir_point->x == -1 && dir_point->y == 0)
        return left_menu_idx;
    else if(dir_point->x == 1 && dir_point->y == 0)
        return right_menu_idx;
    else if(dir_point->x == 0 && dir_point->y == 0)
        return center_menu_idx;

    return 0xff;
}

static void translate_menu_scroll_judge(void)
{
    bool hor_scroll_en = false;
    bool ver_scroll_en = false;
    lv_point_t vert_point = {0};

    lv_indev_get_vect(lv_indev_get_act(), &vert_point);

    lv_point_t *translate_scroll_sum = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_sum;

    translate_scroll_sum->x += vert_point.x;
    translate_scroll_sum->y += vert_point.y;

    if(LV_ABS(translate_scroll_sum->x) > LV_ABS(translate_scroll_sum->y))
        hor_scroll_en = true;
    else
        ver_scroll_en = true;

    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    if(hor_scroll_en && LV_ABS(translate_scroll_sum->x) > translate_scroll_limit)
        *translate_scroll_dir = LV_DIR_HOR;
    else if(ver_scroll_en && LV_ABS(translate_scroll_sum->y) > translate_scroll_limit)
        *translate_scroll_dir = LV_DIR_VER;
    else 
        *translate_scroll_dir = LV_DIR_NONE;

    if(*translate_scroll_dir != LV_DIR_NONE)
        memset(translate_scroll_sum, 0, sizeof(lv_point_t));

    return;
}

static void translate_menu_scroll_handle(void)
{
    lv_point_t vert_point = {0};

    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    lv_indev_get_vect(lv_indev_get_act(), &vert_point);

    lv_point_t *translate_scroll_sum = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_sum;

    translate_scroll_sum->x += vert_point.x;
    translate_scroll_sum->y += vert_point.y;

    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    lv_obj_t *translate_menu_cache[TRANSLATE_DIR_NUM];
    for(uint8_t i = 0; i < TRANSLATE_DIR_NUM; i++)
        translate_menu_cache[i] = \
            p_ui_info_cache->ui_translate_info.translate_menu_cache[i];

    lv_obj_t *translate_up_menu = translate_menu_cache[up_menu_idx];
    lv_obj_t *translate_down_menu = translate_menu_cache[down_menu_idx];
    lv_obj_t *translate_left_menu = translate_menu_cache[left_menu_idx];
    lv_obj_t *translate_right_menu = translate_menu_cache[right_menu_idx];

    int16_t translate_scroll_min = 0;
    int16_t translate_scroll_max = 0;

    if(*translate_scroll_dir == LV_DIR_HOR)
    {
        if(!translate_left_menu && !translate_right_menu)
            return;

        if(translate_left_menu)
            translate_scroll_max = lcd_w;

        if(translate_right_menu)
            translate_scroll_min = -lcd_w;

        translate_scroll_sum->x = translate_scroll_sum->x < translate_scroll_min? \
            translate_scroll_min:translate_scroll_sum->x;
        translate_scroll_sum->x = translate_scroll_sum->x > translate_scroll_max? \
            translate_scroll_max:translate_scroll_sum->x;

        if(translate_scroll_sum->x > 0)
        {
            if(translate_left_menu)
                lv_obj_set_style_translate_x(translate_left_menu, translate_scroll_sum->x, LV_PART_MAIN);

            if(translate_right_menu)
                lv_obj_set_style_translate_x(translate_right_menu, 0, LV_PART_MAIN);
        }else if(translate_scroll_sum->x < 0)
        {
            if(translate_right_menu)
                lv_obj_set_style_translate_x(translate_right_menu, translate_scroll_sum->x, LV_PART_MAIN);

            if(translate_left_menu)
                lv_obj_set_style_translate_x(translate_left_menu, 0, LV_PART_MAIN);
        }
    }else if(*translate_scroll_dir == LV_DIR_VER)
    {
        if(!translate_up_menu && !translate_down_menu)
            return;

        if(translate_up_menu)
            translate_scroll_max = lcd_h;

        if(translate_down_menu)
            translate_scroll_min = -lcd_h;

        translate_scroll_sum->y = translate_scroll_sum->y < translate_scroll_min? \
            translate_scroll_min:translate_scroll_sum->y;
        translate_scroll_sum->y = translate_scroll_sum->y > translate_scroll_max? \
            translate_scroll_max:translate_scroll_sum->y;

        if(translate_scroll_sum->y > 0)
        {
            if(translate_up_menu)
                lv_obj_set_style_translate_y(translate_up_menu, translate_scroll_sum->y, LV_PART_MAIN);

            if(translate_down_menu)
                lv_obj_set_style_translate_y(translate_down_menu, 0, LV_PART_MAIN);
        }else if(translate_scroll_sum->y < 0)
        {
            if(translate_down_menu)
                lv_obj_set_style_translate_y(translate_down_menu, translate_scroll_sum->y, LV_PART_MAIN);

            if(translate_up_menu)
                lv_obj_set_style_translate_y(translate_up_menu, 0, LV_PART_MAIN);
        }
    }

    return;
}

static void translate_menu_anim_cb(void *var, int32_t anim_val)
{
    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    lv_point_t *translate_scroll_sum = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_sum;

    lv_obj_t *translate_menu_cache[TRANSLATE_DIR_NUM];
    for(uint8_t i = 0; i < TRANSLATE_DIR_NUM; i++)
        translate_menu_cache[i] = \
            p_ui_info_cache->ui_translate_info.translate_menu_cache[i];

    lv_obj_t *translate_up_menu = translate_menu_cache[up_menu_idx];
    lv_obj_t *translate_down_menu = translate_menu_cache[down_menu_idx];
    lv_obj_t *translate_left_menu = translate_menu_cache[left_menu_idx];
    lv_obj_t *translate_right_menu = translate_menu_cache[right_menu_idx];

    if(*translate_scroll_dir == LV_DIR_HOR)
    {
        if(translate_scroll_sum->x < 0)
        {
            if(translate_right_menu)
                lv_obj_set_style_translate_x(translate_right_menu, anim_val, LV_PART_MAIN);
        }else if(translate_scroll_sum->x > 0)
        {
            if(translate_left_menu)
                lv_obj_set_style_translate_x(translate_left_menu, anim_val, LV_PART_MAIN);
        }
    }else if(*translate_scroll_dir == LV_DIR_VER)
    {
        if(translate_scroll_sum->y < 0)
        {
            if(translate_down_menu)
                lv_obj_set_style_translate_y(translate_down_menu, anim_val, LV_PART_MAIN);
        }else if(translate_scroll_sum->y > 0)
        {
            if(translate_up_menu)
                lv_obj_set_style_translate_y(translate_up_menu, anim_val, LV_PART_MAIN);
        }
    }

    return;
}

static void translate_menu_anim_ready_cb(lv_anim_t *a)
{
    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    bool *translate_animing = \
        &p_ui_info_cache->ui_translate_info.translate_animing;

    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    ui_act_id_t *translate_id_cache = \
        p_ui_info_cache->ui_translate_info.translate_id_cache;

    int32_t anim_end_val = a->end_value;
    ui_act_id_t jump_act_id = ui_act_id_null;

    if(*translate_scroll_dir == LV_DIR_HOR)
    {
        if(anim_end_val == lcd_w)
            jump_act_id = translate_id_cache[left_menu_idx];
        else if(anim_end_val == -lcd_w)
            jump_act_id = translate_id_cache[right_menu_idx];
    }else if(*translate_scroll_dir == LV_DIR_VER)
    {
        if(anim_end_val == lcd_h)
            jump_act_id = translate_id_cache[up_menu_idx];
        else if(anim_end_val == -lcd_h)
            jump_act_id = translate_id_cache[down_menu_idx];
    }

    if(jump_act_id != ui_act_id_null)
        ui_menu_jump(jump_act_id);

    *translate_animing = false;

    return;
}

static void translate_menu_released_handle(void)
{
    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    bool *translate_animing = \
        &p_ui_info_cache->ui_translate_info.translate_animing;
  
    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    lv_point_t *translate_scroll_sum = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_sum;

    widget_anim_para.anim_obj = NULL;
    widget_anim_para.anim = &translate_anim;
    widget_anim_para.anim_exec_xcb = translate_menu_anim_cb;

    if(*translate_scroll_dir == LV_DIR_HOR)
    {
        if(translate_scroll_sum->x == 0)
            return;

        widget_anim_para.anim_start_val = translate_scroll_sum->x;

        if(LV_ABS(translate_scroll_sum->x) < lcd_w/3)
            widget_anim_para.anim_end_val = 0;
        else
        {
            if(translate_scroll_sum->x > 0)
                widget_anim_para.anim_end_val = lcd_w;
            else
                widget_anim_para.anim_end_val = -lcd_w;
        }
    }else if(*translate_scroll_dir == LV_DIR_VER)
    {
        if(translate_scroll_sum->y == 0)
            return;

        widget_anim_para.anim_start_val = translate_scroll_sum->y;

        if(LV_ABS(translate_scroll_sum->y) < lcd_h/3)
            widget_anim_para.anim_end_val = 0;
        else
        {
            if(translate_scroll_sum->y > 0)
                widget_anim_para.anim_end_val = lcd_h;
            else
                widget_anim_para.anim_end_val = -lcd_h;
        }
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
    lv_anim_set_ready_cb(widget_anim_para.anim, translate_menu_anim_ready_cb);
    lv_anim_start(widget_anim_para.anim);

    *translate_animing = true;

    return;
}

static void translate_menu_pressed_cb(lv_event_t *e)
{
    bool *translate_animing = \
        &p_ui_info_cache->ui_translate_info.translate_animing;

    if(*translate_animing) return;

    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    *translate_scroll_dir = LV_DIR_NONE;

    lv_point_t *translate_scroll_sum = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_sum;

    memset(translate_scroll_sum, 0, sizeof(lv_point_t));

    return;
}

static void translate_menu_pressing_cb(lv_event_t *e)
{
    bool *translate_animing = \
        &p_ui_info_cache->ui_translate_info.translate_animing;

    if(*translate_animing) return;

    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    if(*translate_scroll_dir == LV_DIR_NONE)
        translate_menu_scroll_judge();
    else
        translate_menu_scroll_handle();

    return;
}

static void translate_menu_released_cb(lv_event_t *e)
{
    bool *translate_animing = \
        &p_ui_info_cache->ui_translate_info.translate_animing;

    if(*translate_animing) return;

    lv_dir_t *translate_scroll_dir = \
        &p_ui_info_cache->ui_translate_info.translate_scroll_dir;

    if(*translate_scroll_dir == LV_DIR_NONE)
        return;

    translate_menu_released_handle();

    return;
}

static void translate_menu_event_cb(lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        translate_menu_pressed_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        translate_menu_pressing_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        translate_menu_released_cb(e);

    return;
}

void translate_menu_create_by_tv(lv_obj_t *obj)
{
    if(!obj) return;

    translate_mode = 1;

    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    /*************平移页面容器*************/
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

    lv_point_t translate_dir_point = {0};
    lv_obj_t *translate_menu_container = NULL;
    ui_menu_load_info_t *menu_load_info = NULL;
    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    for(uint8_t i = 0; i < valid_cnt; i++)
    {
        menu_load_info = p_ui_info_cache->ui_translate_info.translate_menu_load[i];
        translate_dir_point.x = p_ui_info_cache->ui_translate_info.translate_dir_points[i].x;
        translate_dir_point.y = p_ui_info_cache->ui_translate_info.translate_dir_points[i].y;

        if(translate_dir_point.x == 0 && translate_dir_point.y == 0)
            continue;

        widget_obj_para.obj_x = translate_dir_point.x*lcd_w;
        widget_obj_para.obj_y = translate_dir_point.y*lcd_h;
        translate_menu_container = common_widget_obj_create(&widget_obj_para);
        
        if(menu_load_info)
        {
            if(menu_load_info->display_func_cb)
                menu_load_info->display_func_cb(translate_menu_container);
        } 

        uint8_t idx = translate_menu_cache_by_point(&translate_dir_point);
        if(idx != 0xff)
            p_ui_info_cache->ui_translate_info.translate_menu_cache[idx] = translate_menu_container;

        printf("%s:idx=%d\n", __func__, idx);
    }

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(obj, translate_menu_event_cb, LV_EVENT_ALL, (void *)&translate_mode);

    return;
}

void translate_menu_create_by_obj(lv_obj_t *obj)
{
    if(!obj) return;

    translate_mode = 0;

    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    /*************平移页面容器*************/
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

    lv_point_t translate_dir_point = {0};
    lv_obj_t *translate_side_menu = NULL;
    lv_obj_t *translate_center_menu = NULL;  
    ui_menu_load_info_t *menu_load_info = NULL;
    uint8_t valid_cnt = p_ui_info_cache->ui_translate_info.translate_valid_cnt;

    /*先创建中心页面*/
    for(uint8_t i = 0; i < valid_cnt; i++)
    {
        translate_dir_point.x = p_ui_info_cache->ui_translate_info.translate_dir_points[i].x;
        translate_dir_point.y = p_ui_info_cache->ui_translate_info.translate_dir_points[i].y;

        if(translate_dir_point.x == 0 && translate_dir_point.y == 0)
        {
            menu_load_info = p_ui_info_cache->ui_translate_info.translate_menu_load[i];

            widget_obj_para.obj_x = translate_dir_point.x*lcd_w;
            widget_obj_para.obj_y = translate_dir_point.y*lcd_h;
            translate_center_menu = common_widget_obj_create(&widget_obj_para);
            if(menu_load_info)
            {
                if(menu_load_info->display_func_cb)
                    menu_load_info->display_func_cb(translate_center_menu);
            }
            break;
        }
    }

    for(uint8_t i = 0; i < valid_cnt; i++)
    {
        menu_load_info = p_ui_info_cache->ui_translate_info.translate_menu_load[i];
        translate_dir_point.x = p_ui_info_cache->ui_translate_info.translate_dir_points[i].x;
        translate_dir_point.y = p_ui_info_cache->ui_translate_info.translate_dir_points[i].y;

        if(translate_dir_point.x == 0 && translate_dir_point.y == 0)
            continue;

        widget_obj_para.obj_x = translate_dir_point.x*lcd_w;
        widget_obj_para.obj_y = translate_dir_point.y*lcd_h;
        translate_side_menu = common_widget_obj_create(&widget_obj_para);
    
        if(menu_load_info)
        {
            if(menu_load_info->display_func_cb)
                menu_load_info->display_func_cb(translate_side_menu);
        } 

        uint8_t idx = translate_menu_cache_by_point(&translate_dir_point);
        if(idx != 0xff)
            p_ui_info_cache->ui_translate_info.translate_menu_cache[idx] = translate_side_menu;

        printf("%s:idx=%d, %p\n", __func__, idx, translate_side_menu);
    }

    if(translate_center_menu)
        lv_obj_add_event_cb(translate_center_menu, translate_menu_event_cb, LV_EVENT_ALL, (void *)&translate_mode);

    return;
}
#endif
