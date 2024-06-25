#include "sleep_main.h"

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = \
        p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t left_act_id = \
            ui_act_id_null;
        ui_act_id_t right_act_id = \
            ui_act_id_bo_sample;
        ui_act_id_t down_act_id = \
            ui_act_id_sleep_detail;
        tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
            left_act_id, right_act_id, ui_act_id_sleep_main);
    }else
    {
        ui_act_id_t prev_act_id = \
            ui_act_id_menu;
        ui_act_id_t down_act_id = \
            ui_act_id_sleep_detail;
        if(!lang_txt_is_arabic())
            tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
                prev_act_id, ui_act_id_null, ui_act_id_sleep_main);
        else
            tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
                ui_act_id_null, prev_act_id, ui_act_id_sleep_main);
    }

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

    uint8_t slp_am_or_pm = 0;
    uint8_t wkp_am_or_pm = 0;

    bool slp_valid = \
        Slp_Info.valid;
    u16 slp_total = \
        Slp_Info.slp_total;

    int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);

    char slp_hour_str[3];
    memset(slp_hour_str, 0, \
        sizeof(slp_hour_str));
    if(slp_valid)
        sprintf(slp_hour_str, "%02d", \
            slp_total/60);
    else
        memcpy(slp_hour_str, "00", 2);

    memset(&num_str_para, 0, \
        sizeof(widget_num_str_para_t));
    num_str_para.parent = \
        obj;
    num_str_para.num_obj_x = \
        60;
    num_str_para.num_obj_y = \
        80;
    num_str_para.p_num_str = \
        slp_hour_str;
    num_str_para.num_str_len = \
        2;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_46x78_ye_00_index;
    common_widget_num_str_create(&num_str_para);

    char slp_minute_str[3];
    memset(slp_minute_str, 0, \
        sizeof(slp_minute_str));
    if(slp_valid)
        sprintf(slp_minute_str, "%02d", \
            slp_total%60);
    else
        memcpy(slp_minute_str, "00", 2);
    num_str_para.num_obj_x = \
        204;
    num_str_para.p_num_str = \
        slp_minute_str;
    num_str_para.file_00_index = \
        comm_num_46x78_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_label_para.label_x = \
        60;
    widget_label_para.label_y = \
        170;
    widget_label_para.label_w = \
        (92);
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x808080);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_hours);
    common_widget_label_create(&widget_label_para);

    widget_label_para.label_x = \
        204;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_unit_mins);
    common_widget_label_create(&widget_label_para);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.img_x = \
        60;
    widget_img_para.img_y = \
        238;
    widget_img_para.file_img_dat = \
        sleep_icon_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = \
        220;
    widget_img_para.file_img_dat = \
        sleep_icon_01_index;
    common_widget_img_create(&widget_img_para, NULL);

    u16 slp_timestamp = \
        Slp_Info.slp_timestamp;
    u8 slp_time_hour;
    u8 slp_time_minute;
    if(slp_valid)
    {
        slp_time_hour = \
            slp_timestamp/60;
        slp_time_minute = \
            slp_timestamp%60;
        
        printf("slp_time_hour = %d, slp_time_minute = %d\n", \
            slp_time_hour, slp_time_minute);
    }else
    {
        slp_time_hour = 0;
        slp_time_minute = 0;
    }
    
    if(time_format == time_format_12)
    {
        if(slp_time_hour >= 12)
            slp_am_or_pm = 1;
        else
            slp_am_or_pm = 0;

        if(slp_time_hour > 12)
            slp_time_hour -= 12;
        else if(slp_time_hour == 0)
            slp_time_hour = 12;
    }
    
    char slp_time_str[6];
    memset(slp_time_str, 0, \
        sizeof(slp_time_str));
    sprintf(slp_time_str, "%02d:%02d", \
        slp_time_hour, slp_time_minute);
    num_str_para.parent = \
        obj;
    num_str_para.num_obj_x = \
        58;
    num_str_para.num_obj_y = \
        346;
    num_str_para.p_num_str = \
        slp_time_str;
    num_str_para.num_str_len = \
        5;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_20x32_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    u16 wkp_timestamp = \
        Slp_Info.wkp_timestamp;
    u8 wkp_time_hour;
    u8 wkp_time_minute;
    if(slp_valid)
    {
        wkp_time_hour = \
            wkp_timestamp/60;
        wkp_time_minute = \
            wkp_timestamp%60;
    }else
    {
        wkp_time_hour = 0;
        wkp_time_minute = 0;
    }
    
    if(time_format == time_format_12)
    {
        if(wkp_time_hour >= 12)
            wkp_am_or_pm = 1;
        else
            wkp_am_or_pm = 0;

        if(wkp_time_hour > 12)
            wkp_time_hour -= 12;
        else if(wkp_time_hour == 0)
            wkp_time_hour = 12;
    }

    char wkp_time_str[6];
    memset(wkp_time_str, 0, \
        sizeof(wkp_time_str));
    sprintf(wkp_time_str, "%02d:%02d", \
        wkp_time_hour, wkp_time_minute);
    num_str_para.num_obj_x = \
        220;
    num_str_para.p_num_str = \
        wkp_time_str;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.img_x = \
        85;
    widget_img_para.img_y = \
        390;
    if(slp_am_or_pm == 0)
        widget_img_para.file_img_dat = \
            comm_icon_24_index;
    else
        widget_img_para.file_img_dat = \
            comm_icon_25_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *slp_ampm_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(time_format == time_format_24)
        lv_obj_add_flag(slp_ampm_icon, LV_OBJ_FLAG_HIDDEN);

    widget_img_para.img_x = \
        248;
    if(wkp_am_or_pm == 0)
        widget_img_para.file_img_dat = \
            comm_icon_24_index;
    else
        widget_img_para.file_img_dat = \
            comm_icon_25_index;
    lv_obj_t *wkp_ampm_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(time_format == time_format_24)
        lv_obj_add_flag(wkp_ampm_icon, LV_OBJ_FLAG_HIDDEN);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_sleep_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_sleep_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
