#include "Hcalendar_main.h"
#include "Hcalendar_modify.h"

static void Hcalendar_modify_cb(\
    lv_event_t *e)
{
    if(!e) return;

    SetHcalendar_modify(\
        HcalendarInfo.Hcalendar_modify);

    ui_menu_jump(\
        ui_act_id_Hcalendar_modify);

    return;
}

static void Hcalendar_festival_preview_cb(\
    lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(\
        ui_act_id_Hcalendar_festivals);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_Gcalendar_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_Hcalendar_main);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_Hcalendar_main);

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

    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = \
            menu_align_right;

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        hijri_calendar_icon_01_index;
    widget_img_para.event_cb = \
        NULL;
    widget_img_para.img_click_attr = \
        false;
    lv_obj_t *Hcalendar_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(Hcalendar_icon, LV_ALIGN_TOP_MID, \
        0, 40);

    widget_img_para.file_img_dat = \
        hijri_calendar_icon_00_index;
    lv_obj_t *Hcalendar_day_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(Hcalendar_day_container, LV_ALIGN_TOP_MID, \
        0, 100);

    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    lv_obj_t *comm_11_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, \
        0, 246);

    char HcalendarDayStr[3];
    memset(HcalendarDayStr, 0, \
        sizeof(HcalendarDayStr));
    sprintf(HcalendarDayStr, "%02d", \
        HcalendarInfo.Hcalendar_day);
    num_str_para.parent = \
        Hcalendar_day_container;
    num_str_para.num_obj_x = \
        50;
    num_str_para.num_obj_y = \
        58;
    num_str_para.p_num_str = \
        HcalendarDayStr;
    num_str_para.num_str_len = \
        strlen(HcalendarDayStr);
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_30x50_ye_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x666666);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(\
            lang_txtid_Muharram + HcalendarInfo.Hcalendar_month - 1);
    lv_obj_t *Hcalendar_month_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(Hcalendar_month_label, LV_ALIGN_TOP_MID, \
        0, 316);

    char HcalendarYearStr[5];
    memset(HcalendarYearStr, 0, sizeof(HcalendarYearStr));
    sprintf(HcalendarYearStr, "%d", HcalendarInfo.Hcalendar_year);
    num_str_para.parent = \
        obj;
    num_str_para.num_obj_x = \
        156;
    num_str_para.num_obj_y = \
        368;
    num_str_para.p_num_str = \
        HcalendarYearStr;
    num_str_para.num_str_len = \
        strlen(HcalendarYearStr);
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_14x22_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_y = \
        408;
    widget_img_para.img_parent = \
        obj;
    widget_img_para.event_cb = \
        NULL;
    widget_img_para.img_click_attr = \
        false;
    if(menu_align == menu_align_right)
    {
        widget_img_para.img_x = \
            170;
        widget_img_para.file_img_dat = \
            comm_icon_07_index;
        common_widget_img_create(&widget_img_para, NULL);

        widget_img_para.img_x = \
            190;
        widget_img_para.file_img_dat = \
            comm_icon_08_index;
        common_widget_img_create(&widget_img_para, NULL);
    }else
    {
        widget_img_para.img_x = \
            190;
        widget_img_para.file_img_dat = \
            comm_icon_07_index;
        common_widget_img_create(&widget_img_para, NULL);

        widget_img_para.img_x = \
            170;
        widget_img_para.file_img_dat = \
            comm_icon_08_index;
        common_widget_img_create(&widget_img_para, NULL);
    }

    widget_img_para.img_parent = \
        obj;
    if(menu_align == menu_align_right)
        widget_img_para.img_x = \
            288;
    else
        widget_img_para.img_x = \
            44;
    widget_img_para.img_y = \
        388;
    widget_img_para.file_img_dat = \
        hijri_calendar_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        Hcalendar_festival_preview_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *Hcalendar_festival_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(Hcalendar_festival_icon, 30);

    if(menu_align == menu_align_right)
        widget_img_para.img_x = \
            44;
    else
        widget_img_para.img_x = \
            288;
    widget_img_para.file_img_dat = \
        hijri_calendar_icon_03_index;
    widget_img_para.event_cb = \
        Hcalendar_modify_cb;
    lv_obj_t *Hcalendar_modify_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(Hcalendar_modify_icon, 30);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_Hcalendar_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_Hcalendar_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
