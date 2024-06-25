#include "call_disc.h"

static void confirm_cb(lv_event_t *e)
{
    if(!e) return;

    ui_act_id_t act_id = \
        read_menu_return_level_id();
    ui_menu_jump(act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_call_disc);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_call_disc);
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
        Label_Line_Height*3;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_call_disc_tips);
    lv_obj_t *tips_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(tips_label, LV_ALIGN_TOP_MID, 0, 112);

    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 32;
    widget_img_para.img_y = 266;
    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = \
        NULL;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = 137;
    widget_img_para.img_y = 376;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.event_cb = \
        confirm_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para,  NULL);
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
    menu_load_call_disc) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_call_disc,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
