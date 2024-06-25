#include "screen_sleep.h"

static const char roller_str[] = {
    "10\n20\n30\n60"
};

static void roller_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \  
        lv_event_get_target(e);
    uint16_t roller_sel = \
        lv_roller_get_selected(obj);
    
    int time = 30;
    if(roller_sel == 0)
        time = 10;
    else if(roller_sel == 1)
        time = 20;
    else if(roller_sel == 2)
        time = 30;
    else if(roller_sel == 3)
        time = 60;
    
    time *= 1000;
    SetVmParaCacheByLabel(\
        vm_label_offscreen_time, time);

    printf("time = %d\n", time);

    return;
}

static void title_label_create(lv_obj_t *obj)
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
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_screen_sleep);
    lv_obj_t *title_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 40);
 
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t bht_act_id = \
        ui_act_id_backlight;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            bht_act_id, ui_act_id_null, ui_act_id_screen_sleep);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, bht_act_id, ui_act_id_screen_sleep);

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

    title_label_create(obj);

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
    lv_obj_align(sel_container, LV_ALIGN_CENTER, 0, 10);

    widget_img_para.img_parent = \
        sel_container;
    widget_img_para.file_img_dat = \
        disp_ctrl_02_index;
    lv_obj_t *sec_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(sec_icon, LV_ALIGN_TOP_MID, 45, 46);

    u8 tmp = 2;
    int scr_slp_time = \
        GetVmParaCacheByLabel(\
            vm_label_offscreen_time);
    scr_slp_time = scr_slp_time/1000;
    if(scr_slp_time == 10)
        tmp = 0;
    else if(scr_slp_time == 20)
        tmp = 1;
    else if(scr_slp_time == 30)
        tmp = 2;
    else if(scr_slp_time == 60)
        tmp = 3;

    widget_roller_para.roller_parent = \
        obj;
    widget_roller_para.roller_width = \
        120;
    widget_roller_para.roller_height = \
        0;
    widget_roller_para.roller_row_count = \
        3;
    widget_roller_para.roller_options = \
        roller_str;
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
        roller_cb;
    widget_roller_para.user_data = \
        NULL;
    lv_obj_t *roller = \
        common_widget_roller_create(&widget_roller_para);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 10);

    widget_img_para.img_x = 170;
    widget_img_para.img_y = 408;
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_08_index;
    common_widget_img_create(&widget_img_para, \
        NULL);

    widget_img_para.img_x = 190;
    widget_img_para.file_img_dat = \
        comm_icon_07_index;
    common_widget_img_create(&widget_img_para, \
        NULL);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_screen_sleep) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_screen_sleep,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
