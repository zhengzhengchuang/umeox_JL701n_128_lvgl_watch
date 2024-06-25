#include "tasbih_end_time.h"

static uint8_t tasbih_etime_hour;
static uint8_t tasbih_etime_minute;

static lv_obj_t *tips_label_container;
static lv_timer_t *tips_label_hid_timer;

static lv_obj_t *tasbih_etime_hour_sel;
static uint16_t tasbih_etime_hour_dsc;
static lv_obj_t *tasbih_etime_minute_sel;
static uint16_t tasbih_etime_minute_dsc;

static lv_obj_t *tasbih_etime_hour_roller;
static lv_obj_t *tasbih_etime_minute_roller;

uint8_t GetTasbih_etime_hour(void)
{
    return tasbih_etime_hour;
}

void SetTasbih_etime_hour(uint8_t hour)
{
    tasbih_etime_hour = hour;

    return;
}

uint8_t GetTasbih_etime_minute(void)
{
    return tasbih_etime_minute;
}

void SetTasbih_etime_minute(uint8_t minute)
{
    tasbih_etime_minute = minute;

    return;
}

static void tasbih_etime_hour_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = \
        lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
    {
        lv_obj_set_style_text_color(tasbih_etime_hour_roller, \
            lv_color_hex(0x999999), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_etime_hour_roller, \
            lv_color_hex(0xF0D990), LV_PART_SELECTED);
        common_widget_img_replace_src(tasbih_etime_hour_sel, \
            comm_icon_37_index, tasbih_etime_hour_dsc);

        lv_obj_set_style_text_color(tasbih_etime_minute_roller, \
            lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_etime_minute_roller, \
            lv_color_hex(0x000000), LV_PART_SELECTED);          
        common_widget_img_replace_src(tasbih_etime_minute_sel, \
            comm_icon_38_index, tasbih_etime_minute_dsc);
    }else if(event_code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t roller_sel = \
            lv_roller_get_selected(tasbih_etime_hour_roller); 
        tasbih_etime_hour = roller_sel;
    }

    return;
}

static void tasbih_etime_minute_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = \
        lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
    {
        lv_obj_set_style_text_color(tasbih_etime_minute_roller, \
            lv_color_hex(0x999999), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_etime_minute_roller, \
            lv_color_hex(0xF0D990), LV_PART_SELECTED);
        common_widget_img_replace_src(tasbih_etime_minute_sel, \
            comm_icon_37_index, tasbih_etime_minute_dsc);

        lv_obj_set_style_text_color(tasbih_etime_hour_roller, \
            lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_set_style_text_color(tasbih_etime_hour_roller, \
            lv_color_hex(0x000000), LV_PART_SELECTED);          
        common_widget_img_replace_src(tasbih_etime_hour_sel, \
            comm_icon_38_index, tasbih_etime_hour_dsc);
    }else if(event_code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t roller_sel = \
            lv_roller_get_selected(tasbih_etime_minute_roller);
        tasbih_etime_minute = roller_sel;
    }

    return;
}

static void tips_label_hid_timeout_cb(lv_timer_t *timer)
{
    if(!timer) return;

    if(tips_label_hid_timer)
    {
        lv_timer_del(\
            tips_label_hid_timer);
        tips_label_hid_timer = NULL;
    }

    lv_obj_add_flag(tips_label_container, \
        LV_OBJ_FLAG_HIDDEN);

    return;
}

static void tasbih_etime_confirm_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t stime_hour = \
        GetTasbih_stime_hour();
    uint8_t stime_minute = \
        GetTasbih_stime_minute();
    uint8_t etime_hour = \
        GetTasbih_etime_hour();
    uint8_t etime_minute = \
        GetTasbih_etime_minute();

    uint32_t TasbihR_STimestamp = \
        stime_hour*3600 + stime_minute*60;
    uint32_t TasbihR_ETimestamp = \
        etime_hour*3600 + etime_minute*60;
 
    uint32_t ST = \
        TasbihR_STimestamp;
    uint32_t ET = \
        TasbihR_ETimestamp;

    u32 TasbihR_TotalInvTime;
    if(ET >= ST)
        TasbihR_TotalInvTime = \
            ET - ST;
    else
        TasbihR_TotalInvTime = \
            ET + 86400 - ST; 

    uint32_t TasbihR_SetInvTime = \
        TasbihR_info.TasbihR_SetInvTime;

    if(TasbihR_SetInvTime > \
        TasbihR_TotalInvTime)
    {
        lv_obj_clear_flag(tips_label_container, \
            LV_OBJ_FLAG_HIDDEN);

        if(!tips_label_hid_timer)
            tips_label_hid_timer = \
                lv_timer_create(tips_label_hid_timeout_cb, 3*1000, NULL);

        return;
    }

    TasbihR_info.TasbihR_STimestamp = \
        TasbihR_STimestamp;
    TasbihR_info.TasbihR_ETimestamp = \
        TasbihR_ETimestamp;
    TasbihR_info.TasbihR_TotalInvTime = \
        TasbihR_TotalInvTime;

    TasbihRInfoParaUpdate();

    ui_menu_jump(\
        ui_act_id_tasbih_main);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_tasbih_end_time);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_tasbih_end_time);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    if(tips_label_hid_timer)
    {
        lv_timer_del(\
            tips_label_hid_timer);
        tips_label_hid_timer = NULL;
    }

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
        get_lang_txt_with_id(lang_txtid_end_time);
    lv_obj_t *tasbih_etime_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(tasbih_etime_label, LV_ALIGN_TOP_MID, \
        0, 40);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_37_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    tasbih_etime_hour_sel = \
        common_widget_img_create(&widget_img_para, \
            &tasbih_etime_hour_dsc);
    lv_obj_align(tasbih_etime_hour_sel, LV_ALIGN_LEFT_MID, \
        32, 11);

    widget_img_para.file_img_dat = \
        comm_num_30x50_wh_10_index;
    lv_obj_t *tasbih_etime_colon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(tasbih_etime_colon, LV_ALIGN_LEFT_MID, \
        176, 10);

    widget_img_para.file_img_dat = \
        comm_icon_38_index;
    tasbih_etime_minute_sel = \
        common_widget_img_create(&widget_img_para, \
            &tasbih_etime_minute_dsc);
    lv_obj_align(tasbih_etime_minute_sel, LV_ALIGN_LEFT_MID, \
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
        tasbih_etime_hour;
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
    widget_roller_para.roller_border_width = \
        0;
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
    tasbih_etime_hour_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(tasbih_etime_hour_roller, LV_ALIGN_LEFT_MID, \
        32, 10);
    lv_obj_add_event_cb(tasbih_etime_hour_roller, \
        tasbih_etime_hour_roller_cb, LV_EVENT_ALL, NULL);

    widget_roller_para.roller_options = \
        time_min_sec_str;
    widget_roller_para.roller_sel = \
        tasbih_etime_minute;
    widget_roller_para.roller_main_text_color = \
        lv_color_hex(0x000000);
    widget_roller_para.roller_selected_text_color = \
        lv_color_hex(0x000000);
    widget_roller_para.event_cb = \
        NULL;
    widget_roller_para.user_data = \
        NULL;
    tasbih_etime_minute_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(tasbih_etime_minute_roller, LV_ALIGN_LEFT_MID, \
        216, 10);
    lv_obj_add_event_cb(tasbih_etime_minute_roller, \
        tasbih_etime_minute_roller_cb, LV_EVENT_ALL, NULL);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_etime_confirm_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(confirm_button, LV_ALIGN_TOP_MID, \
        0, 376);
    lv_obj_set_ext_click_area(confirm_button, 20);

    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT - LCD_UI_Y_OFFSET;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_90;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        false;
    tips_label_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_add_flag(tips_label_container, LV_OBJ_FLAG_HIDDEN);

    widget_label_para.label_w = \
        (280);
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xFFDF8B);
    widget_label_para.label_ver_center = \
        true;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        tips_label_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_inv_exceeds_range);
    lv_obj_t *tips_label_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(tips_label_label, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_tasbih_end_time) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_tasbih_end_time,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
