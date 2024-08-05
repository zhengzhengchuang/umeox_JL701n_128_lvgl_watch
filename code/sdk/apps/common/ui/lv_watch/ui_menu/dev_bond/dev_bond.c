#include "dev_bond.h"

static lv_obj_t *QR_code;

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    tileview_register_all_menu(obj, ui_act_id_null, \
        ui_act_id_null, ui_act_id_null, ui_act_id_null, \
            ui_act_id_dev_bond);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    lv_qrcode_delete(QR_code);

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

    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_width = 230;
    widget_obj_para.obj_height = 230;
    widget_obj_para.obj_bg_opax = LV_OPA_100;
    widget_obj_para.obj_bg_color = lv_color_hex(0xffffff);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 20;
    widget_obj_para.obj_is_scrollable = false;
    lv_obj_t *QR_code_container = common_widget_obj_create(&widget_obj_para);
    lv_obj_align(QR_code_container, LV_ALIGN_TOP_MID, 0, 52);

    lv_color_t dcolor;
    lv_color_t lcolor;
    uint16_t QR_code_size;
    uint16_t link_str_len;
    char *p = GetQRCodeLinkStrBuf();

    QR_code_size = 210;
    link_str_len = strlen(p);
    dcolor = lv_color_hex(0x000000);
    lcolor = lv_color_hex(0xffffff);
    QR_code = lv_qrcode_create(QR_code_container, QR_code_size, dcolor, lcolor);
    lv_qrcode_update(QR_code, p, link_str_len);
    lv_obj_center(QR_code);

    bool ar_lang = lang_txt_is_arabic();
    widget_label_para.label_w = 320;
    widget_label_para.label_h = Label_Line_Height*3 - 5;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    if(ar_lang == true)
        widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_dev_bond);
    lv_obj_t *dev_bond_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(dev_bond_label, LV_ALIGN_TOP_MID, 0, 292);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_dev_bond) = 
{
    .menu_arg = NULL,
    .lock_flag = true,
    .return_flag = true,
    .menu_id = \
        ui_act_id_dev_bond,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
