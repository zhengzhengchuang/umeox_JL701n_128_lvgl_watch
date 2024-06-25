#include "weather_data.h"

static bool data_valid;

/*********************************************************************************
                                  创建无天气数据页面                                 
*********************************************************************************/
static void no_weather_data_menu_create(lv_obj_t *obj)
{
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        weather_no_data_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *no_data_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(no_data_icon, LV_ALIGN_TOP_MID, \
        0, 82);

    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    lv_obj_t *comm_11_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, \
        0, 264);

    
    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;    
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_no_data);
    lv_obj_t *no_data_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(no_data_label, LV_ALIGN_TOP_MID, \
        0, 352);

    return;
}

/*********************************************************************************
                                  创建有天气数据页面                                 
*********************************************************************************/
static void has_weather_data_menu_create(lv_obj_t *obj)
{
    int16_t data_sym_x = 50;

    int16_t min0_temper = \
        weather_min_temper(0);

    widget_data_para.data_x = \
        data_sym_x;
    widget_data_para.data_y = 94;
    widget_data_para.num_inv = 0;
    widget_data_para.data_parent = \
        obj;
    widget_data_para.num_addr_index = \
        comm_num_14x22_wh_00_index;
    widget_data_para.data_align = \
        widget_data_align_left;
    int16_t data_end_x = \
         common_data_widget_create(&widget_data_para, \
            widget_data_type_min0_weather, &min0_temper);

    widget_img_para.img_x = \
        data_end_x + 4;
    widget_img_para.img_y = 92;
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        weather_other_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    int16_t max0_temper = \
        weather_max_temper(0);
    widget_data_para.data_x = \
        data_end_x + 30;
    data_end_x = \
        common_data_widget_create(&widget_data_para, \
            widget_data_type_max0_weather, &max0_temper);

    widget_img_para.img_x = \
        data_end_x + 4;
    widget_img_para.img_y = 92;
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        weather_other_01_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    int16_t real_temper = \
        weather_real_temper();
    widget_data_para.data_x = \
        (LCD_WIDTH - data_sym_x) - \
            (4*24 + 12);
    widget_data_para.data_y = 85;
    widget_data_para.num_inv = 0;
    widget_data_para.data_parent = \
        obj;
    widget_data_para.num_addr_index = \
        comm_num_24x40_wh_00_index;
    widget_data_para.data_align = \
        widget_data_align_right;
    common_data_widget_create(&widget_data_para, \
        widget_data_type_weather, &real_temper);

    weather_type_t weather_type = \
        weather_weather_type(0);
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        weather_148x140_00_index + weather_type;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *weather_type_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(weather_type_icon, LV_ALIGN_TOP_MID, \
        0, 165);

    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_sunny + \
            weather_type);
    lv_obj_t *weather_type_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(weather_type_label, LV_ALIGN_TOP_MID, \
        0, 332);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    data_valid = \
        Weather_Info.valid;
    ui_mode_t ui_mode = \
        p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t left_act_id = \
            ui_act_id_hr_sample;
        ui_act_id_t right_act_id = \
            ui_act_id_watchface;
        ui_act_id_t down_act_id;
        if(data_valid)
            down_act_id = \
                ui_act_id_weather_report;
        else
            down_act_id = \
                ui_act_id_null;
        tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
            left_act_id, right_act_id, ui_act_id_weather_data);
    }else
    {
        ui_act_id_t prev_act_id = \
            ui_act_id_menu;
        ui_act_id_t down_act_id;
        if(data_valid)
            down_act_id = \
                ui_act_id_weather_report;
        else
            down_act_id = \
                ui_act_id_null;
        if(!lang_txt_is_arabic())
            tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
                prev_act_id, ui_act_id_null, ui_act_id_weather_data);
        else
            tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
                ui_act_id_null, prev_act_id, ui_act_id_weather_data);
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

    bool data_valid_tmp = \
        Weather_Info.valid;
    if(data_valid != data_valid_tmp)
        ui_menu_jump(\
            p_ui_info_cache->cur_act_id);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    if(!data_valid)
        no_weather_data_menu_create(obj);
    else
        has_weather_data_menu_create(obj);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_weather_data) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_weather_data,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 200,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
