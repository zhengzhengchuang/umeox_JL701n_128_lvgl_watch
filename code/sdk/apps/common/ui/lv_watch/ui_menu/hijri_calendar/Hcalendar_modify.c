#include "Hcalendar_modify.h"

static const char Hcalendar_modify_days[] = {
    "-3\n-2\n-1\n00\n+1\n+2\n+3"
};

static int8_t Hcalendar_modify;
int8_t GetHcalendar_modify(void)
{
    return Hcalendar_modify;
}

void SetHcalendar_modify(int8_t val)
{
    Hcalendar_modify = \
        val;
    return;
}

static void Hcalendar_modify_roller_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);
    int8_t tmp = \
        roller_sel - 3;
    if(tmp < -3) tmp = -3;
    if(tmp > 3) tmp = 3;
    SetHcalendar_modify(tmp);

    return;
}

static void Hcalendar_modify_confirm_cb(\
    lv_event_t *e)
{
    if(!e) return;

    int8_t tmp = \
        GetHcalendar_modify();
    HcalendarInfo.Hcalendar_modify = \
        tmp;
    HcalendarInfoParaUpdate();

    ui_menu_jump(\
        ui_act_id_Hcalendar_main);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_Hcalendar_modify);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_Hcalendar_modify);

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
        get_lang_txt_with_id(\
            lang_txtid_Hcalendar_modify);
    lv_obj_t *Hcalendar_modify_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(Hcalendar_modify_label, LV_ALIGN_TOP_MID, \
        0, 40);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_09_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *Hcalendar_modify_sel = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(Hcalendar_modify_sel, LV_ALIGN_CENTER, 0, 10);
    
    widget_label_para.label_w = \
        80;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_parent = \
        Hcalendar_modify_sel;
    widget_label_para.label_text = \
        get_lang_txt_with_id(\
            lang_txtid_days);
    lv_obj_t *days_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(days_label, LV_ALIGN_TOP_MID, 80, 40);

    int8_t tmp = \
        GetHcalendar_modify();
    tmp += 3;
    if(tmp < 0) tmp = 0;
    if(tmp > 6) tmp = 6;
    widget_roller_para.roller_parent = \
        obj;
    widget_roller_para.roller_width = \
        120;
    widget_roller_para.roller_height = \
        0;
    widget_roller_para.roller_row_count = \
        3;
    widget_roller_para.roller_options = \
        Hcalendar_modify_days;
    widget_roller_para.roller_mode = \
        LV_ROLLER_MODE_NORMAL;
    widget_roller_para.roller_sel = \
        tmp;
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
        Hcalendar_modify_roller_cb;
    widget_roller_para.user_data = \
        NULL;
    lv_obj_t *Hcalendar_modify_roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(Hcalendar_modify_roller, LV_ALIGN_CENTER, 0, 10);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        Hcalendar_modify_confirm_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(confirm_button, LV_ALIGN_TOP_MID, 0, 390);
    lv_obj_set_ext_click_area(confirm_button, 10);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_Hcalendar_modify) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_Hcalendar_modify,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
