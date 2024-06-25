#include "countdown_over.h"

static void exit_cb(lv_event_t *e)
{
    if(!e) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    ui_menu_jump(prev_act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_countdown_over);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_countdown_over);

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

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        countdown_01_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *countdown_over_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(countdown_over_icon, LV_ALIGN_TOP_MID, \
        0, 52);

    char over_time_str[10];
    memset(over_time_str, 0, \
        sizeof(over_time_str));
    sprintf(over_time_str, "%02d:%02d:%02d", \
        0, 0, 0);
    num_str_para.parent = \ 
        obj;
    num_str_para.num_obj_x = \
        58;
    num_str_para.num_obj_y = \
        184;
    num_str_para.p_num_str = \
        over_time_str;
    num_str_para.num_str_len = \
        8;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_34x56_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *comm_11_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 266);
    
    widget_img_para.file_img_dat = \
        comm_icon_26_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        exit_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *exit_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(exit_button, LV_ALIGN_TOP_MID, 0, 376);
    lv_obj_set_ext_click_area(exit_button, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_countdown_over) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_countdown_over,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
