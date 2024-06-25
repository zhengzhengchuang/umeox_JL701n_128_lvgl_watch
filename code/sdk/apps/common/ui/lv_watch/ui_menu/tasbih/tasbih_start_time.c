#include "tasbih_start_time.h"

static uint8_t tasbih_stime_hour;
static uint8_t tasbih_stime_minute;

static lv_obj_t *tasbih_stime_hour_sel;
static uint16_t tasbih_stime_hour_dsc;
static lv_obj_t *tasbih_stime_minute_sel;
static uint16_t tasbih_stime_minute_dsc;

static lv_obj_t *tasbih_stime_hour_roller;
static lv_obj_t *tasbih_stime_minute_roller;

uint8_t GetTasbih_stime_hour(void)
{
    return tasbih_stime_hour;
}

void SetTasbih_stime_hour(uint8_t hour)
{
    tasbih_stime_hour = hour;

    return;
}

uint8_t GetTasbih_stime_minute(void)
{
    return tasbih_stime_minute;
}

void SetTasbih_stime_minute(uint8_t minute)
{
    tasbih_stime_minute = minute;

    return;
}

static void tasbih_stime_hour_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = \
        lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
    {
        lv_obj_set_style_text_color(tasbih_stime_hour_roller, \
            lv_color_hex(0x999999), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_stime_hour_roller, \
            lv_color_hex(0xF0D990), LV_PART_SELECTED);
        common_widget_img_replace_src(tasbih_stime_hour_sel, \
            comm_icon_37_index, tasbih_stime_hour_dsc);

        lv_obj_set_style_text_color(tasbih_stime_minute_roller, \
            lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_stime_minute_roller, \
            lv_color_hex(0x000000), LV_PART_SELECTED);          
        common_widget_img_replace_src(tasbih_stime_minute_sel, \
            comm_icon_38_index, tasbih_stime_minute_dsc);
    }else if(event_code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t roller_sel = \
            lv_roller_get_selected(tasbih_stime_hour_roller); 
        tasbih_stime_hour = roller_sel;
    }

    return;
}

static void tasbih_stime_minute_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = \
        lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
    {
        lv_obj_set_style_text_color(tasbih_stime_minute_roller, \
            lv_color_hex(0x999999), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_stime_minute_roller, \
            lv_color_hex(0xF0D990), LV_PART_SELECTED);
        common_widget_img_replace_src(tasbih_stime_minute_sel, \
            comm_icon_37_index, tasbih_stime_minute_dsc);

        lv_obj_set_style_text_color(tasbih_stime_hour_roller, \
            lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_stime_hour_roller, \
            lv_color_hex(0x000000), LV_PART_SELECTED);          
        common_widget_img_replace_src(tasbih_stime_hour_sel, \
            comm_icon_38_index, tasbih_stime_hour_dsc);
    }else if(event_code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t roller_sel = \
            lv_roller_get_selected(tasbih_stime_minute_roller);
        tasbih_stime_minute = roller_sel;
    }

    return;
}

static void tasbih_stime_confirm_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(\
        ui_act_id_tasbih_end_time);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    u32 TasbihR_STimestamp = \
        TasbihR_info.TasbihR_STimestamp;
    tasbih_stime_hour = \
        TasbihR_STimestamp/3600;
    tasbih_stime_minute = \
        (TasbihR_STimestamp%3600)/60;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_tasbih_start_time);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_tasbih_start_time);

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

    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_start_time);
    lv_obj_t *tasbih_stime_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(tasbih_stime_label, LV_ALIGN_TOP_MID, \
        0, 40);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_37_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    tasbih_stime_hour_sel = \
        common_widget_img_create(&widget_img_para, \
            &tasbih_stime_hour_dsc);
    lv_obj_align(tasbih_stime_hour_sel, LV_ALIGN_LEFT_MID, \
        32, 11);

    widget_img_para.file_img_dat = \
        comm_num_30x50_wh_10_index;
    lv_obj_t *tasbih_stime_colon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(tasbih_stime_colon, LV_ALIGN_LEFT_MID, \
        176, 10);

    widget_img_para.file_img_dat = \
        comm_icon_38_index;
    tasbih_stime_minute_sel = \
        common_widget_img_create(&widget_img_para, \
            &tasbih_stime_minute_dsc);
    lv_obj_align(tasbih_stime_minute_sel, LV_ALIGN_LEFT_MID, \
        216, 11);

    widget_roller_para.roller_parent = \
        obj;
    widget_roller_para.roller_width = \
        120;
    widget_roller_para.roller_height = \
        0;
    widget_roller_para.roller_row_count = \
        3;
    widget_roller_para.roller_options = \
        time_hour_str;
    widget_roller_para.roller_mode = \
        LV_ROLLER_MODE_NORMAL;
    widget_roller_para.roller_sel = \
        tasbih_stime_hour;
    widget_roller_para.roller_line_inv = \
        15;
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
        NULL;
    widget_roller_para.user_data = \
        NULL;
    tasbih_stime_hour_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(tasbih_stime_hour_roller, LV_ALIGN_LEFT_MID, \
        32, 10);
    lv_obj_add_event_cb(tasbih_stime_hour_roller, \
        tasbih_stime_hour_roller_cb, LV_EVENT_ALL, NULL);
    

    widget_roller_para.roller_options = \
        time_min_sec_str;
    widget_roller_para.roller_sel = \
        tasbih_stime_minute;
    widget_roller_para.roller_main_text_color = \
        lv_color_hex(0x000000);
    widget_roller_para.roller_selected_text_color = \
        lv_color_hex(0x000000);
    widget_roller_para.event_cb = \
        NULL;
    widget_roller_para.user_data = \
        NULL;
    tasbih_stime_minute_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(tasbih_stime_minute_roller, LV_ALIGN_LEFT_MID, \
        216, 10);
    lv_obj_add_event_cb(tasbih_stime_minute_roller, \
        tasbih_stime_minute_roller_cb, LV_EVENT_ALL, NULL);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_stime_confirm_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(confirm_button, LV_ALIGN_TOP_MID, \
        0, 376);
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
    menu_load_tasbih_start_time) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_tasbih_start_time,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
