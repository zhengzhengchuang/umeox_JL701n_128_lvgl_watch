#include "watchface_05.h"

static const common_clock_pointer_para_t \
    clk_p_para[] = 
{
    [0] = 
    {
        .clk_p_width = 34,
        .clk_p_height = 178,
        .clk_p_center = 148,
        .clk_p_type = \
            clk_pointer_type_hour,
        .clk_p_file_dat = \
            watchface_05_clock_hour_index,
    },

    [1] = 
    {
        .clk_p_width = 34,
        .clk_p_height = 236,
        .clk_p_center = 204,
        .clk_p_type = \
            clk_pointer_type_minute,
        .clk_p_file_dat = \
            watchface_05_clock_min_index,
    },

    [2] = 
    {
        .clk_p_width = 24,
        .clk_p_height = 223,
        .clk_p_center = 190,
        .clk_p_type = \
            clk_pointer_type_second,
        .clk_p_file_dat = \
            watchface_05_clock_sec_index,
    },
};

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    wf_tileview_register_all_menu(obj);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = \
        watchface_05_bg_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    uint8_t clk_p_num = sizeof(clk_p_para)/ \
        sizeof(common_clock_pointer_para_t);
    common_clock_pointer_create(obj, &clk_p_para, \
        clk_p_num);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_watchface_05) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_watchface,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
