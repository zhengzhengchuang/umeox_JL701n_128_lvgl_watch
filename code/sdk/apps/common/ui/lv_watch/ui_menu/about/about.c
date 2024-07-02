#include "about.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;

static lv_obj_t *QR_code;
static lv_obj_t *about_container;

static void scroll_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void about_container_create(lv_obj_t *obj)
{
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
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        true;
    about_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_set_style_pad_bottom(about_container, 25, \
        LV_PART_MAIN);
    lv_obj_add_event_cb(about_container, scroll_cb, \
        LV_EVENT_SCROLL, NULL);

    return;
}

static void about_title_icon_create(void)
{
    widget_img_para.img_parent = \
        about_container;
    widget_img_para.file_img_dat = \
        about_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *title_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(title_icon, LV_ALIGN_TOP_MID, 0, 20);

    return;
}

static void about_title_label_create(void)
{
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
        about_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_quran_watch);
    lv_obj_t *title_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 88);

    return;
}

static void about_ble_name_elem_create(void)
{
    widget_img_para.img_parent = \
        about_container;
    widget_img_para.file_img_dat = \
        comm_icon_10_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *ble_name_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(ble_name_container, LV_ALIGN_TOP_MID, 0, 152);

    widget_label_para.label_w = \
        260;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x777777);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        ble_name_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_bluetooth_name);
    lv_obj_t *ble_name_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(ble_name_label, LV_ALIGN_TOP_MID, 0, 6);

    const char *ble_name_str = \
        GetDevBleName();
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_text = \
        (char *)ble_name_str;
    lv_obj_t *ble_name_str_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(ble_name_str_label, LV_ALIGN_BOTTOM_MID, 0, -6);

    return;
}

extern void swapX(const uint8_t *src, uint8_t *dst, int len);
static void about_ble_mac_elem_create(void)
{   
    widget_img_para.img_parent = \
        about_container;
    widget_img_para.file_img_dat = \
        comm_icon_10_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *ble_mac_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(ble_mac_container, LV_ALIGN_TOP_MID, 0, 272);

    widget_label_para.label_w = \
        260;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x777777);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        ble_mac_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_bluetooth_mac);
    lv_obj_t *ble_mac_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(ble_mac_label, LV_ALIGN_TOP_MID, 0, 6);

    uint8_t ble_mac_idx = 0;
    char ble_mac_str[18] = {0};
    const uint8_t *ble_mac = \
        GetDevBleMac();
    
    static u8 dst_ble_mac[6];
    memset(dst_ble_mac, 0, 6);
    swapX(ble_mac, dst_ble_mac, 6);

    for(uint8_t i = 0; i < 17; i++)
    {
        if(((i + 1) % 3) == 0)
		{
			ble_mac_idx++;
			ble_mac_str[i] = ':';
		}else if(((i + 1) % 3) == 1)
		{
			ble_mac_str[i] = ((dst_ble_mac[ble_mac_idx] >> 4) & 0x0f) > 9 ? \
			    ((dst_ble_mac[ble_mac_idx] >> 4) & 0x0f) + 0x37:\
                    ((dst_ble_mac[ble_mac_idx] >> 4) & 0x0f) + 0x30;
		}else if(((i + 1) % 3) == 2)
		{
			ble_mac_str[i] = (dst_ble_mac[ble_mac_idx] & 0x0f) > 9 ? \
			    ((dst_ble_mac[ble_mac_idx]) & 0x0f) + 0x37:\
                    (dst_ble_mac[ble_mac_idx] & 0x0f) + 0x30;
		}
    }
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_text = \
        ble_mac_str;
    lv_obj_t *ble_mac_str_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(ble_mac_str_label, LV_ALIGN_BOTTOM_MID, 0, -6);

    return;
}

static void about_version_elem_create(void)
{
    widget_img_para.img_parent = \
        about_container;
    widget_img_para.file_img_dat = \
        comm_icon_10_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *version_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(version_container, LV_ALIGN_TOP_MID, 0, 392);

    widget_label_para.label_w = \
        260;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x777777);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        version_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_version);
    lv_obj_t *version_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(version_label, LV_ALIGN_TOP_MID, 0, 6);

    char *version_str = \
        Version_Release_Str;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_text = \
        version_str;
    lv_obj_t *version_str_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(version_str_label, LV_ALIGN_BOTTOM_MID, 0, -6);

    return;
}

static void about_QR_code_elem_create(void)
{
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
        about_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_QR_code);
    lv_obj_t *QR_code_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(QR_code_label, LV_ALIGN_TOP_MID, \
        0, LCD_HEIGHT + 102);

    widget_obj_para.obj_parent = \
        about_container;
    widget_obj_para.obj_width = \
        230;
    widget_obj_para.obj_height = \
        230;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_100;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0xffffff);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 20;
    widget_obj_para.obj_is_scrollable = \
        false;
    lv_obj_t *QR_code_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_align_to(QR_code_container, QR_code_label, \
        LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_color_t dcolor;
    lv_color_t lcolor;
    uint16_t QR_code_size;
    uint16_t link_str_len;
    char *p = GetQRCodeLinkStrBuf();

    QR_code_size = 210;
    link_str_len = strlen(p);
    dcolor = lv_color_hex(0x000000);
    lcolor = lv_color_hex(0xffffff);
    
    QR_code = \
        lv_qrcode_create(QR_code_container, QR_code_size, dcolor, lcolor);
    lv_qrcode_update(QR_code, p, link_str_len);
    lv_obj_center(QR_code);

    return;
}

static void reset_button_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_reset);

    return;
}

static void about_reset_button_elem_create(void)
{
    widget_img_para.img_parent = \
        about_container;
    widget_img_para.file_img_dat = \
        about_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        reset_button_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *reset_button = \
        common_widget_img_create(&widget_img_para, \
            NULL);
    lv_obj_align(reset_button, LV_ALIGN_TOP_MID, \
        0, LCD_HEIGHT + 430);
    lv_obj_set_ext_click_area(reset_button, 20);

    widget_label_para.label_w = \
        200;
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
        reset_button;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_reset);
    lv_obj_t *reset_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_center(reset_label);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_set_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_about);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_about);

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

    about_container_create(obj);

    about_title_icon_create();

    about_title_label_create();

    about_ble_name_elem_create();

    about_ble_mac_elem_create();

    about_version_elem_create();

    about_QR_code_elem_create();

    about_reset_button_elem_create();

    lv_obj_scroll_to_y(about_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(about_container);
    scroll_top_y = lv_obj_get_scroll_top(about_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(about_container);
    scroll_y = scroll_y<scroll_top_y?scroll_top_y:scroll_y;
    scroll_y = scroll_y>scroll_bottom_y?scroll_bottom_y:scroll_y;
    lv_obj_scroll_to_y(about_container, scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

static void menu_rdec_cb(lv_obj_t *obj, int state)
{
    if(lv_anim_get(about_container, NULL))
        return;

    int16_t scroll_y_tmp = scroll_y;

    if(state == Rdec_Forward)
        scroll_y_tmp += LCD_HEIGHT/2;
    else if(state == Rdec_Backward)
        scroll_y_tmp -= LCD_HEIGHT/2;
    else
        return;

    scroll_y_tmp = scroll_y_tmp < scroll_top_y?scroll_top_y:scroll_y_tmp;
    scroll_y_tmp = scroll_y_tmp > scroll_bottom_y?scroll_bottom_y:scroll_y_tmp;
    if(scroll_y_tmp != scroll_y)
        lv_obj_scroll_to_y(about_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_about) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_about,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
