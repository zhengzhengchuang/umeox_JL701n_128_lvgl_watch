#include "gcalendar_main.h"

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_menu;
    ui_act_id_t next_act_id = \
        ui_act_id_Hcalendar_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, next_act_id, ui_act_id_Gcalendar_main);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            next_act_id, prev_act_id, ui_act_id_Gcalendar_main);

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
 
    struct sys_time *Hutc_time = \
        &(HcalendarInfo.Hutc_time);
    comm_enum_week_t weekday = \
        GetUtcWeek(Hutc_time);
   
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
            lang_txtid_sunday + weekday);
    lv_obj_t *weekday_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(weekday_label, LV_ALIGN_TOP_MID, \
        0, 40);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        hijri_calendar_icon_00_index;
    widget_img_para.event_cb = \
        NULL;
    widget_img_para.img_click_attr = \
        false;
    lv_obj_t *Gcalendar_day_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(Gcalendar_day_container, LV_ALIGN_TOP_MID, \
        0, 100);

    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    lv_obj_t *comm_11_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, \
        0, 246);

    char GcalendarDayStr[3];
    memset(GcalendarDayStr, 0, \
        sizeof(GcalendarDayStr));
    sprintf(GcalendarDayStr, "%02d", \
        Hutc_time->day);
    num_str_para.parent = \
        Gcalendar_day_container;
    num_str_para.num_obj_x = \
        50;
    num_str_para.num_obj_y = \
        58;
    num_str_para.p_num_str = \
        GcalendarDayStr;
    num_str_para.num_str_len = \
        strlen(GcalendarDayStr);
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
            lang_txtid_January + Hutc_time->month - 1);
    lv_obj_t *Gcalendar_month_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(Gcalendar_month_label, LV_ALIGN_TOP_MID, \
        0, 316);

    char GcalendarYearStr[5];
    memset(GcalendarYearStr, 0, sizeof(GcalendarYearStr));
    sprintf(GcalendarYearStr, "%d", Hutc_time->year);
    num_str_para.parent = \
        obj;
    num_str_para.num_obj_x = \
        156;
    num_str_para.num_obj_y = \
        368;
    num_str_para.p_num_str = \
        GcalendarYearStr;
    num_str_para.num_str_len = \
        strlen(GcalendarYearStr);
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
            190;
        widget_img_para.file_img_dat = \
            comm_icon_07_index;
        common_widget_img_create(&widget_img_para, NULL);

        widget_img_para.img_x = \
            170;
        widget_img_para.file_img_dat = \
            comm_icon_08_index;
        common_widget_img_create(&widget_img_para, NULL);
    }else
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
    }

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_gcalendar_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_Gcalendar_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
