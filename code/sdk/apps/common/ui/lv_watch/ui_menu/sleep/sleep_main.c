#include "sleep_main.h"

static bool slp_valid;

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t left_act_id = ui_act_id_null;
        ui_act_id_t right_act_id = ui_act_id_bo_sample;
        ui_act_id_t down_act_id = ui_act_id_sleep_detail;
        tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
            left_act_id, right_act_id, ui_act_id_sleep_main);
    }else
    {
        ui_act_id_t prev_act_id = ui_act_id_menu;
        ui_act_id_t down_act_id = ui_act_id_sleep_detail;
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

    bool valid = SlpPara.valid;
    if(valid != slp_valid)
    {
        slp_valid = valid;
        ui_menu_jump(p_ui_info_cache->cur_act_id);
    }

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    uint8_t slp_am_or_pm = 0;
    uint8_t wkp_am_or_pm = 0;

    bool valid = SlpPara.valid;
    slp_valid = valid;

    u32 slp_dur = (u32)(SlpPara.slp_dur);

    int time_format = \
        GetVmParaCacheByLabel(vm_label_time_format);

    char hour_str[3];
    memset(hour_str, 0, sizeof(hour_str));
    if(valid == true)
        sprintf(hour_str, "%02d", slp_dur/60);
    else
        memcpy(hour_str, "00", 2);

    memset(&num_str_para, 0, sizeof(widget_num_str_para_t));
    num_str_para.parent = obj;
    num_str_para.num_obj_x = 60;
    num_str_para.num_obj_y = 80;
    num_str_para.p_num_str = hour_str;
    num_str_para.num_str_len = 2;
    num_str_para.num_obj = NULL;
    num_str_para.num_obj_max = 0;
    num_str_para.num_dsc_idx = NULL;
    num_str_para.file_00_index = comm_num_46x78_ye_00_index;
    common_widget_num_str_create(&num_str_para);

    char minute_str[3];
    memset(minute_str, 0, sizeof(minute_str));
    if(valid == true)
        sprintf(minute_str, "%02d", slp_dur%60);
    else
        memcpy(minute_str, "00", 2);

    num_str_para.num_obj_x = 204;
    num_str_para.p_num_str = minute_str;
    num_str_para.file_00_index = comm_num_46x78_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_label_para.label_x = 60;
    widget_label_para.label_y = 170;
    widget_label_para.label_w = (92);
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0x808080);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_hours);
    common_widget_label_create(&widget_label_para);

    widget_label_para.label_x = 204;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_mins);
    common_widget_label_create(&widget_label_para);

    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 60;
    widget_img_para.img_y = 238;
    widget_img_para.file_img_dat = sleep_icon_00_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = 220;
    widget_img_para.file_img_dat = sleep_icon_01_index;
    common_widget_img_create(&widget_img_para, NULL);

    struct sys_time slp_time;

    u32 start_ts = SlpPara.slp_start_ts;
    SecToUtcTime(start_ts, &slp_time);
    u8 start_hour;
    u8 start_minute;
    if(valid == true)
    {
        start_hour = slp_time.hour;
        start_minute = slp_time.min;
        
        printf("sh = %d, sm = %d\n", start_hour, start_minute);
    }else
    {
        start_hour = 0;
        start_minute = 0;
    }
    
    if(time_format == time_format_12)
    {
        if(start_hour >= 12)
            slp_am_or_pm = 1;
        else
            slp_am_or_pm = 0;

        if(start_hour > 12)
            start_hour -= 12;
        else if(start_hour == 0)
            start_hour = 12;
    }
    
    char st_str[6];
    memset(st_str, 0, sizeof(st_str));
    sprintf(st_str, "%02d:%02d", start_hour, start_minute);
    num_str_para.parent = obj;
    num_str_para.num_obj_x = 58;
    num_str_para.num_obj_y = 346;
    num_str_para.p_num_str = st_str;
    num_str_para.num_str_len = 5;
    num_str_para.num_obj = NULL;
    num_str_para.num_obj_max = 0;
    num_str_para.num_dsc_idx = NULL;
    num_str_para.file_00_index = comm_num_20x32_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    u32 end_ts = SlpPara.slp_end_ts;
    SecToUtcTime(end_ts, &slp_time);
    u8 end_hour;
    u8 end_minute;
    if(valid == true)
    {
        end_hour = slp_time.hour;
        end_minute = slp_time.min;
    }else
    {
        end_hour = 0;
        end_minute = 0;
    }
    
    if(time_format == time_format_12)
    {
        if(end_hour >= 12)
            wkp_am_or_pm = 1;
        else
            wkp_am_or_pm = 0;

        if(end_hour > 12)
            end_hour -= 12;
        else if(end_hour == 0)
            end_hour = 12;
    }

    char et_str[6];
    memset(et_str, 0, sizeof(et_str));
    sprintf(et_str, "%02d:%02d", end_hour, end_minute);
    num_str_para.num_obj_x = 220;
    num_str_para.p_num_str = et_str;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 85;
    widget_img_para.img_y = 390;
    if(slp_am_or_pm == 0)
        widget_img_para.file_img_dat = comm_icon_24_index;
    else
        widget_img_para.file_img_dat = comm_icon_25_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *st_ampm_icon = common_widget_img_create(&widget_img_para, NULL);
    if(time_format == time_format_24)
        lv_obj_add_flag(st_ampm_icon, LV_OBJ_FLAG_HIDDEN);

    widget_img_para.img_x = 248;
    if(wkp_am_or_pm == 0)
        widget_img_para.file_img_dat = comm_icon_24_index;
    else
        widget_img_para.file_img_dat = comm_icon_25_index;
    lv_obj_t *et_ampm_icon = common_widget_img_create(&widget_img_para, NULL);
    if(time_format == time_format_24)
        lv_obj_add_flag(et_ampm_icon, LV_OBJ_FLAG_HIDDEN);

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
    .user_refresh_time_inv = 200,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
