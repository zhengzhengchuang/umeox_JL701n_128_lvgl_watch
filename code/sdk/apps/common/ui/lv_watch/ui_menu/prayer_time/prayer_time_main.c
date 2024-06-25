#include "prayer_time_main.h"
#include "prayer_time_advance.h"

static lv_obj_t *ampm_icon;
static uint16_t ampm_icon_dsc_idx;

static lv_obj_t *PT_type_label;

static lv_obj_t *PTTimeNumObj[5];
static uint16_t PTTimeNumDscIdx[5];

static lv_obj_t *PTCountdownNumObj[8];
static uint16_t PTCountdownNumDscIdx[8];

static lv_obj_t *PT_countdown_mode_label;

static void prayer_time_num_container_cb(\
    lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(\
        ui_act_id_prayer_time_preview);

    return;
}

static void prayer_time_reminder_on_cb(\
    lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(\
        ui_act_id_prayer_time_remind);

    return;
}

static void prayer_time_reminder_offset_cb(\
    lv_event_t *e)
{
    if(!e) return;

    u8 PT_type = \
        GetPTimeType();
    int32_t val = 
        PT_Cfg.remind_offset[PT_type]/60;
    SetPT_advance(val);

    ui_menu_jump(\
        ui_act_id_prayer_time_advance);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = \
        p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t left_act_id = ui_act_id_null;
        ui_act_id_t right_act_id = ui_act_id_null;
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            left_act_id, right_act_id, ui_act_id_prayer_time_main);
    }else
    {
        ui_act_id_t prev_act_id = \
            ui_act_id_menu;
        if(!lang_txt_is_arabic())
            tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
                prev_act_id, ui_act_id_null, ui_act_id_prayer_time_main);
        else
            tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
                ui_act_id_null, prev_act_id, ui_act_id_prayer_time_main);
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

    u8 PT_type = \
        GetPTimeType();

    lv_label_set_text(PT_type_label, \
        get_lang_txt_with_id(lang_txtid_Fajr + PT_type));
    
    u8 am_pm = 0;
    uint8_t PTResultHour = \
        PT_Info.result_timestamp[PT_type]/3600;
    uint8_t PTResultMinute = \
        (PT_Info.result_timestamp[PT_type]%3600)/60;    
    int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(PTResultHour >= 12)
            am_pm = 1;
        else
            am_pm = 0;

        if(PTResultHour > 12)
            PTResultHour -= 12;
        else if(PTResultHour == 0)
           PTResultHour = 12; 
    }
    char PTNumStr[6];
    memset(PTNumStr, 0, sizeof(PTNumStr));
    sprintf(PTNumStr, "%02d:%02d", \
        PTResultHour, PTResultMinute);
    common_widget_num_str_refresh(PTTimeNumObj, 5, PTTimeNumDscIdx, \
        PTNumStr, 5, comm_num_46x78_ye_00_index);

    if(am_pm == 0)
        common_widget_img_replace_src(ampm_icon, \
            comm_icon_22_index, ampm_icon_dsc_idx);
    else
        common_widget_img_replace_src(ampm_icon, \
            comm_icon_23_index, ampm_icon_dsc_idx);
    if(time_format == time_format_24)
        lv_obj_add_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_clear_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);

    uint8_t PTCountdownMode = \
        GetPTimeCountdownMode(PT_type);
    lv_label_set_text(PT_countdown_mode_label, \
        get_lang_txt_with_id(lang_txtid_remaining_time + \
            PTCountdownMode));
    
    u32 PTCountdownTime = \
        GetPTimeCountdownTime(PT_type);
    char PTCountdownTimeStr[10];
    memset(PTCountdownTimeStr, 0, \
        sizeof(PTCountdownTimeStr));
    sprintf(PTCountdownTimeStr, "%02d:%02d:%02d", \
        PTCountdownTime/3600, (PTCountdownTime%3600)/60, \
            (PTCountdownTime%3600)%60);
    common_widget_num_str_refresh(PTCountdownNumObj, 8, \
        PTCountdownNumDscIdx, PTCountdownTimeStr, 8, \
            comm_num_16x28_wh_00_index);

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

    u8 am_pm = 0;
    u8 PT_type = \
        GetPTimeType();

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
        get_lang_txt_with_id(lang_txtid_Fajr + PT_type);
    PT_type_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(PT_type_label, LV_ALIGN_TOP_MID, 0, 40);

    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        140;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        78;
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
        false;
    lv_obj_t *PT_num_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(PT_num_container, \
        prayer_time_num_container_cb, LV_EVENT_SHORT_CLICKED, NULL);

    char PTNumStr[6];
    uint8_t PTResultHour = \
        PT_Info.result_timestamp[PT_type]/3600;
    uint8_t PTResultMinute = \
        (PT_Info.result_timestamp[PT_type]%3600)/60;    
    int time_format = \
        GetVmParaCacheByLabel(vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(PTResultHour >= 12)
            am_pm = 1;
        else
            am_pm = 0;

        if(PTResultHour > 12)
            PTResultHour -= 12;
        else if(PTResultHour == 0)
           PTResultHour = 12; 
    }

    memset(PTNumStr, 0, sizeof(PTNumStr));
    sprintf(PTNumStr, "%02d:%02d", \
        PTResultHour, PTResultMinute);
    num_str_para.parent = \
       PT_num_container;
    num_str_para.num_obj_x = \
        67;
    num_str_para.num_obj_y = \
        0;
    num_str_para.p_num_str = \
        PTNumStr;
    num_str_para.num_str_len = \
        5;
    num_str_para.num_obj = \
        PTTimeNumObj;
    num_str_para.num_obj_max = \
        5;
    num_str_para.num_dsc_idx = \
        PTTimeNumDscIdx;
    num_str_para.file_00_index = \
        comm_num_46x78_ye_00_index;
    int16_t end_x = \
        common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
        PT_num_container;
    widget_img_para.img_x = \
        end_x + 10;
    widget_img_para.img_y = \
        48;
    if(am_pm == 0)
        widget_img_para.file_img_dat = \
            comm_icon_22_index;
    else
        widget_img_para.file_img_dat = \
            comm_icon_23_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    ampm_icon = \
        common_widget_img_create(&widget_img_para, &ampm_icon_dsc_idx);
    if(time_format == time_format_24)
        lv_obj_add_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);

    uint8_t PTCountdownMode = \
        GetPTimeCountdownMode(PT_type);

    widget_label_para.label_w = \
        288;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x999999);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_remaining_time + \
            PTCountdownMode);
    PT_countdown_mode_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(PT_countdown_mode_label, LV_ALIGN_TOP_MID, 0, 258);

    u32 PTCountdownTime = \
        GetPTimeCountdownTime(PT_type);
    char PTCountdownTimeStr[10];
    memset(PTCountdownTimeStr, 0, \
        sizeof(PTCountdownTimeStr));
    sprintf(PTCountdownTimeStr, "%02d:%02d:%02d", \
        PTCountdownTime/3600, (PTCountdownTime%3600)/60, \
            (PTCountdownTime%3600)%60);
    num_str_para.parent = \
       obj;
    num_str_para.num_obj_x = \
        128;
    num_str_para.num_obj_y = \
        314;
    num_str_para.p_num_str = \
        PTCountdownTimeStr;
    num_str_para.num_str_len = \
        8;
    num_str_para.num_obj = \
        PTCountdownNumObj;
    num_str_para.num_obj_max = \
        8;
    num_str_para.num_dsc_idx = \
        PTCountdownNumDscIdx;
    num_str_para.file_00_index = \
        comm_num_16x28_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
        obj;
    if(menu_align == menu_align_left)
        widget_img_para.img_x = 44;
    else
        widget_img_para.img_x = 288;
    widget_img_para.img_y = 372;
    widget_img_para.file_img_dat = \
        prayer_time_01_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        prayer_time_reminder_offset_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *reminder_offset_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(reminder_offset_icon, 20);
 
    bool remind_en = \
        PT_Cfg.remind_en[PT_type];
    if(menu_align == menu_align_left)
        widget_img_para.img_x = 288;
    else
        widget_img_para.img_x = 44;   
    if(remind_en)
        widget_img_para.file_img_dat = \
            prayer_time_03_index;
    else
        widget_img_para.file_img_dat = \
            prayer_time_02_index;
    widget_img_para.event_cb = \
        prayer_time_reminder_on_cb;
    lv_obj_t *reminder_on_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(reminder_on_icon, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_prayer_time_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_prayer_time_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
