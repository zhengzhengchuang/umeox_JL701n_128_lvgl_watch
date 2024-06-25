#include "alarm_time.h"

static uint8_t alarm_time_hour_tmp;
static uint8_t alarm_time_minute_tmp;

void SetAlarmTimeTmp(uint8_t hour, \
    uint8_t minute)
{
    alarm_time_hour_tmp = \
        hour;
    alarm_time_minute_tmp = \
        minute;

    return;
}

void GetAlarmTimeTmp(uint8_t *hour, \
    uint8_t *minute)
{
    if(!hour || !minute)
        return;
    
    *hour = \
        alarm_time_hour_tmp;
    *minute = \
        alarm_time_minute_tmp;

    return;
}

static void alarm_time_next_step_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(\
        ui_act_id_alarm_repeat);

    return;
}

static void alarm_time_hour_roller_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);

    alarm_time_hour_tmp = \
        roller_sel;

    return;
}

static void alarm_time_minute_roller_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);

    alarm_time_minute_tmp = \
        roller_sel;

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_alarm_time);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_alarm_time);

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
    lv_obj_t *alarm_time_sel = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(alarm_time_sel, LV_ALIGN_CENTER, \
        0, -25);

    widget_img_para.img_parent = \
       alarm_time_sel;
     widget_img_para.file_img_dat = \
        comm_num_30x50_ye_10_index;
    lv_obj_t *comm_num_10_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_num_10_icon, LV_ALIGN_CENTER, \
        0, 0);

    widget_roller_para.roller_parent = \
        obj;
    widget_roller_para.roller_width = 100;
    widget_roller_para.roller_height = 0;
    widget_roller_para.roller_row_count = 3;
    widget_roller_para.roller_options = \
        time_hour_str;
    widget_roller_para.roller_mode = \
        LV_ROLLER_MODE_NORMAL;
    widget_roller_para.roller_sel = \
        alarm_time_hour_tmp;
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
        alarm_time_hour_roller_cb;
    widget_roller_para.user_data = \
        NULL;
    lv_obj_t *alarm_time_hour_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(alarm_time_hour_roller, LV_ALIGN_LEFT_MID, \
        90, -25);

    widget_roller_para.roller_options = \
        time_min_sec_str;
    widget_roller_para.roller_sel = \
        alarm_time_minute_tmp;
    widget_roller_para.event_cb = \
        alarm_time_minute_roller_cb;
    lv_obj_t *alarm_time_minute_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(alarm_time_minute_roller, LV_ALIGN_RIGHT_MID, \
        -90, -25);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_03_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        alarm_time_next_step_cb;
    widget_img_para.user_data = 
        NULL;
    lv_obj_t *next_step_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(next_step_icon, LV_ALIGN_TOP_MID, \
        0, 376);
    lv_obj_set_ext_click_area(next_step_icon, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_alarm_time) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_alarm_time,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
