#include "countdown_time.h"

static uint8_t time_hour;
static uint8_t time_minute;
static uint8_t time_second;

void ClearTimeSet(void)
{
    time_hour = 0;
    time_minute = 0;
    time_second = 0;
}

static void time_hour_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);

    time_hour = roller_sel;

    return;
}

static void time_minute_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);

    time_minute = roller_sel;

    return;
}

static void time_second_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);

    time_second = roller_sel;

    return;
}

static void time_confirm_cb(\
    lv_event_t *e)
{
    if(!e) return;

    uint32_t time_total = \
        time_hour*3600 + time_minute*60 + time_second;

    if(time_total == 0)
        return;

    ClearTimeSet();

    UserCountdownCreate(time_total);

    ui_menu_jump(\
        ui_act_id_countdown_state);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_more_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_countdown_time);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_countdown_time);

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
        comm_icon_09_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *sel_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(sel_container, LV_ALIGN_CENTER, 0, -25);

    widget_img_para.img_parent = \
       sel_container;
     widget_img_para.file_img_dat = \
        comm_num_30x50_ye_10_index;
    lv_obj_t *num_10_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(num_10_icon, LV_ALIGN_LEFT_MID, 106, 0);

    num_10_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(num_10_icon, LV_ALIGN_RIGHT_MID, -106, 0);

    widget_roller_para.roller_parent = \
        obj;
    widget_roller_para.roller_width = 80;
    widget_roller_para.roller_height = 0;
    widget_roller_para.roller_row_count = 3;
    widget_roller_para.roller_options = \
        time_hour_str;
    widget_roller_para.roller_mode = \
        LV_ROLLER_MODE_NORMAL;
    widget_roller_para.roller_sel = \
        time_hour;
    widget_roller_para.roller_line_inv = \
        25;
    widget_roller_para.roller_main_bg_opax = \
        LV_OPA_0;
    widget_roller_para.roller_main_bg_color = \
        lv_color_hex(0x000000);
    widget_roller_para.roller_selected_bg_opax = \
        LV_OPA_0;
    widget_roller_para.roller_selected_bg_color = \
        lv_color_hex(0x000000);
    widget_roller_para.roller_border_width = 0;
    widget_roller_para.roller_border_opax = \
        LV_OPA_0;
    widget_roller_para.roller_border_color = \
        lv_color_hex(0x000000);
    widget_roller_para.roller_main_text_font = \
        &font_common_num_64;
    widget_roller_para.roller_main_text_color = \
        lv_color_hex(0x999999);
    widget_roller_para.roller_selected_text_font = \
        &font_common_num_64;
    widget_roller_para.roller_selected_text_color = \
        lv_color_hex(0xF0D990);
    widget_roller_para.event_cb = \
        time_hour_roller_cb;
    widget_roller_para.user_data = \
        NULL;
    lv_obj_t *time_hour_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(time_hour_roller, LV_ALIGN_LEFT_MID, \
        50, -25);

    widget_roller_para.roller_options = \
        time_min_sec_str;
    widget_roller_para.roller_sel = \
        time_minute;
    widget_roller_para.event_cb = \
        time_minute_roller_cb;
    lv_obj_t *time_minute_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(time_minute_roller, LV_ALIGN_CENTER, \
        0, -25);

    widget_roller_para.roller_options = \
        time_min_sec_str;
    widget_roller_para.roller_sel = \
        time_second;
    widget_roller_para.event_cb = \
        time_second_roller_cb;
    lv_obj_t *time_second_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(time_second_roller, LV_ALIGN_RIGHT_MID, \
        -50, -25);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        time_confirm_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(confirm_button, LV_ALIGN_TOP_MID, 0, 376);
    lv_obj_set_ext_click_area(confirm_button, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_countdown_time) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_countdown_time,
    .user_offscreen_time = 0,\
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
