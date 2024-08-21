#include "watchface_10.h"

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

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = watchface_10_bg_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    widget_time_para.time_x = 132;
    widget_time_para.time_y = 130;
    widget_time_para.num_inv = 0;
    widget_time_para.time_parent = obj;
    widget_time_para.num_addr_index = watchface_10_52x78_00_index;
    common_time_widget_create(&widget_time_para, widget_time_mode_hh);

    widget_time_para.time_y = 260;
    common_time_widget_create(&widget_time_para, widget_time_mode_mm);

    widget_date_para.date_x = 148;
    widget_date_para.date_y = 223;
    widget_date_para.num_inv = 0;
    widget_date_para.date_parent = obj;
    widget_date_para.num_addr_index = watchface_06_14x22_00_index;
    common_date_widget_create(&widget_date_para, widget_date_mode_mm_dd);

    lang_id_t lang_id = get_comm_sys_language();

    comm_week_para.week_x = 226;
    comm_week_para.week_y = 214;
    comm_week_para.week_parent = obj;
    if(lang_id == lang_id_french)
        comm_week_para.week_addr_index = watchface_06_week_fr_00_index; 
    else if(lang_id == lang_id_arabic)
        comm_week_para.week_addr_index = watchface_06_week_ar_00_index; 
    else
        comm_week_para.week_addr_index = watchface_06_week_en_00_index;
    common_week_widget_create(&comm_week_para);

    uint8_t type;
    widget_img_para.img_x = 92;
    widget_img_para.img_y = 214;
    widget_img_para.img_parent = obj;
    if(Weather_Info.valid)
        type = weather_weather_type(0);
    else
        type = weather_type_unknown;
    widget_img_para.file_img_dat = watchface_11_weather_00_index + type;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);
    
    int16_t real_temper;
    if(Weather_Info.valid)
        real_temper = weather_real_temper();
    else
        real_temper = No_Weather;
    widget_data_para.data_x = 20;
    widget_data_para.data_y = 223;
    widget_data_para.num_inv = 0;
    widget_data_para.data_parent = obj;
    widget_data_para.num_addr_index = watchface_06_14x22_00_index;
    widget_data_para.data_align = widget_data_align_right;
    common_data_widget_create(&widget_data_para, widget_data_type_weather, &real_temper);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_watchface_10) = 
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
