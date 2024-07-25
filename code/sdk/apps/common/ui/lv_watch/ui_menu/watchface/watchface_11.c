#include "watchface_11.h"

static lv_obj_t *PTTimeHObj[2];
static uint16_t PTTimeHDscIdx[2];

static lv_obj_t *PTTimeMObj[2];
static uint16_t PTTimeMDscIdx[2];

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    wf_tileview_register_all_menu(obj);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    u8 PT_type = GetPTimeType();

    uint8_t PTResultHour = \
        PT_Info.result_timestamp[PT_type]/3600;
    uint8_t PTResultMinute = \
        (PT_Info.result_timestamp[PT_type]%3600)/60;    
    int time_format = \
        GetVmParaCacheByLabel(vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(PTResultHour > 12)
            PTResultHour -= 12;
        else if(PTResultHour == 0)
           PTResultHour = 12; 
    }

    char PTHStr[3];
    memset(PTHStr, 0, sizeof(PTHStr));
    sprintf(PTHStr, "%02d", PTResultHour);
    common_widget_num_str_refresh(PTTimeHObj, 2, PTTimeHDscIdx, \
        PTHStr, 2, watchface_11_12x20_dark_00_index);

    char PTMStr[3];
    memset(PTMStr, 0, sizeof(PTMStr));
    sprintf(PTMStr, "%02d", PTResultMinute);
    common_widget_num_str_refresh(PTTimeMObj, 2, PTTimeMDscIdx, \
        PTMStr, 2, watchface_11_12x20_dark_00_index);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = \
        watchface_11_bg_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    widget_time_para.time_x = 132;
    widget_time_para.time_y = 137;
    widget_time_para.num_inv = 0;
    widget_time_para.time_parent = \
        obj;
    widget_time_para.num_addr_index = \
        watchface_11_52x78_00_index;
    common_time_widget_create(\
        &widget_time_para, widget_time_mode_hh);

    widget_time_para.time_y = 227;
    common_time_widget_create(\
        &widget_time_para, widget_time_mode_mm);

    widget_date_para.date_x = 154;
    widget_date_para.date_y = 109;
    widget_date_para.num_inv = 0;
    widget_date_para.date_parent = obj;
    widget_date_para.num_addr_index = \
        watchface_11_12x20_light_00_index;
    common_date_widget_create(\
        &widget_date_para, widget_date_mode_mm_dd);

    lang_id_t lang_id = \
        get_comm_sys_language();
    comm_week_para.week_x = 145;
    comm_week_para.week_y = 313;
    comm_week_para.week_parent = obj;
    if(lang_id == lang_id_french)
        comm_week_para.week_addr_index = \
            watchface_11_week_fr_00_index; 
    else if(lang_id == lang_id_arabic)
        comm_week_para.week_addr_index = \
            watchface_11_week_ar_00_index; 
    else
        comm_week_para.week_addr_index = \
            watchface_11_week_en_00_index;
    common_week_widget_create(&comm_week_para);

    u32 step = PedoData.steps;
    widget_data_para.data_x = 21;
    widget_data_para.data_y = 332;
    widget_data_para.num_inv = 0;
    widget_data_para.data_parent = obj;
    widget_data_para.num_addr_index = \
        watchface_11_12x20_dark_00_index;
    widget_data_para.data_align = \
        widget_data_align_center;
    common_data_widget_create(&widget_data_para, \
        widget_data_type_step, &step);

    int hr_val = \
        GetHrRealVal();
    widget_data_para.data_x = 33;
    widget_data_para.data_y = 185;
    common_data_widget_create(&widget_data_para, \
        widget_data_type_hr, &hr_val);

    u8 PT_type = GetPTimeType();

    uint8_t PTResultHour = \
        PT_Info.result_timestamp[PT_type]/3600;
    uint8_t PTResultMinute = \
        (PT_Info.result_timestamp[PT_type]%3600)/60;    
    int time_format = \
        GetVmParaCacheByLabel(vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(PTResultHour > 12)
            PTResultHour -= 12;
        else if(PTResultHour == 0)
           PTResultHour = 12; 
    }

    char PTHStr[3];
    memset(PTHStr, 0, sizeof(PTHStr));
    sprintf(PTHStr, "%02d", PTResultHour);
    num_str_para.parent = \
       obj;
    num_str_para.num_obj_x = \
        268;
    num_str_para.num_obj_y = \
        185;
    num_str_para.p_num_str = \
        PTHStr;
    num_str_para.num_str_len = \
        2;
    num_str_para.num_obj = \
        PTTimeHObj;
    num_str_para.num_obj_max = \
        2;
    num_str_para.num_dsc_idx = \
        PTTimeHDscIdx;
    num_str_para.file_00_index = \
        watchface_11_12x20_dark_00_index;
    common_widget_num_str_create(&num_str_para);

    char PTMStr[3];
    memset(PTMStr, 0, sizeof(PTMStr));
    sprintf(PTMStr, "%02d", PTResultMinute);
    num_str_para.parent = \
       obj;
    num_str_para.num_obj_x = \
        314;
    num_str_para.num_obj_y = \
        185;
    num_str_para.p_num_str = \
        PTMStr;
    num_str_para.num_str_len = \
        2;
    num_str_para.num_obj = \
        PTTimeMObj;
    num_str_para.num_obj_max = \
        2;
    num_str_para.num_dsc_idx = \
        PTTimeMDscIdx;
    num_str_para.file_00_index = \
        watchface_11_12x20_dark_00_index;
    common_widget_num_str_create(&num_str_para);

    uint8_t type;
    widget_img_para.img_x = 297;
    widget_img_para.img_y = 264;
    widget_img_para.img_parent = obj;
    if(Weather_Info.valid)
        type = weather_weather_type(0);
    else
        type = weather_type_unknown;
    widget_img_para.file_img_dat = \
        watchface_11_weather_00_index + type;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    int16_t real_temper;
    if(Weather_Info.valid)
        real_temper = weather_real_temper();
    else
        real_temper = No_Weather;
    widget_data_para.data_x = 290;
    widget_data_para.data_y = 332;
    widget_data_para.num_inv = 0;
    widget_data_para.data_parent = obj;
    widget_data_para.num_addr_index = \
        watchface_11_12x20_dark_00_index;
    widget_data_para.data_align = \
        widget_data_align_center;
    common_data_widget_create(&widget_data_para, \
        widget_data_type_weather, &real_temper);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_watchface_11) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_watchface,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
