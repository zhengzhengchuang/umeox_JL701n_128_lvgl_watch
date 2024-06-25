#include "call_main.h"

static void call_log_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_call_log);

    return;
}

static void contacts_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_contacts);

    return;
}

static void call_dial_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_call_dial);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_call_main);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_call_main);
            
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

    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 32;
    widget_img_para.img_y = 80;
    widget_img_para.file_img_dat = \
        comm_icon_12_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = \
        call_log_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *call_log_container = \
        common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = 194;
    widget_img_para.event_cb = \
        contacts_cb;
    lv_obj_t *contacts_container = \
        common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = 134;
    widget_img_para.img_y = 320;
    widget_img_para.file_img_dat = \
        call_02_index;
    widget_img_para.event_cb = \
        call_dial_cb;
    common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_parent = \
        call_log_container;
    widget_img_para.file_img_dat = \
        call_00_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *call_log_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(call_log_icon, LV_ALIGN_CENTER, 0, 0);

    widget_img_para.img_parent = \
        contacts_container;
    widget_img_para.file_img_dat = \
        call_01_index;
    lv_obj_t *contacts_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(contacts_icon, LV_ALIGN_CENTER, 0, 0);

#if 0
    widget_label_para.label_w = \
        (130);
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
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = \
        call_log_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_call_log);
    lv_obj_t *call_log_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(call_log_label, LV_ALIGN_TOP_MID, 0, 117);

    widget_label_para.label_parent = \
        contacts_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_contacts);
    lv_obj_t *contacts_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(contacts_label, LV_ALIGN_TOP_MID, 0, 117);
#endif

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_call_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_call_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
