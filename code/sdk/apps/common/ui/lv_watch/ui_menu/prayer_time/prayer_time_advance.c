#include "prayer_time_advance.h"

static int32_t PT_advance;
int32_t GetPT_advance(void)
{
    return PT_advance;
}

void SetPT_advance(int32_t val)
{
    PT_advance = val;
    return;
}

static void prayer_time_advance_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);
    int32_t val = \
        roller_sel - 30;
    if(val < -30) val = -30;
    if(val > 30) val = 30;
    SetPT_advance(val);

    return;
}

static void prayer_time_advance_confirm_cb(\
    lv_event_t *e)
{
    if(!e) return;

    int32_t val = \
        GetPT_advance();
    u8 PT_type = \
        GetPTimeType();
    PTimeRemindOffsetUpdate(\
        PT_type, val*60);

    ui_menu_jump(\
        ui_act_id_prayer_time_main);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_prayer_time_advance);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_prayer_time_advance);

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
        get_lang_txt_with_id(lang_txtid_reminder_in_advance);
    lv_obj_t *PT_reminder_advance_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(PT_reminder_advance_label, LV_ALIGN_TOP_MID, 0, 40);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_09_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *PT_advance_time_sel = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(PT_advance_time_sel, LV_ALIGN_CENTER, 0, 1);

    widget_label_para.label_w = \
        100;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        PT_advance_time_sel;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_unit_mins);
    lv_obj_t *unit_mins_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(unit_mins_label, LV_ALIGN_TOP_MID, 92, 35);

    int32_t tmp = 
        GetPT_advance();
    tmp += 30;
    if(tmp < 0) tmp = 0;
    if(tmp > 60) tmp = 60;
    widget_roller_para.roller_parent = \
        obj;
    widget_roller_para.roller_width = \
        120;
    widget_roller_para.roller_height = 0;
    widget_roller_para.roller_row_count = \
        3;
    widget_roller_para.roller_options = \
        time_ne30_po30_str;
    widget_roller_para.roller_mode = \
        LV_ROLLER_MODE_NORMAL;
    widget_roller_para.roller_sel = \
        tmp;
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
        prayer_time_advance_roller_cb;
    widget_roller_para.user_data = \
        NULL;
    lv_obj_t *prayer_time_advance_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(prayer_time_advance_roller, LV_ALIGN_CENTER, 0, 0);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        prayer_time_advance_confirm_cb;
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
    menu_load_prayer_time_advance) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_prayer_time_advance,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
