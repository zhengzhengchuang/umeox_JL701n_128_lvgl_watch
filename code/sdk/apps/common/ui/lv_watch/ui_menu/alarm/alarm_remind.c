#include "alarm_remind.h"

static void alarm_remind_exit_cb(lv_event_t *e)
{
    if(!e) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    ui_menu_jump(prev_act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_alarm_remind);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_alarm_remind);

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
        alarm_01_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *alarm_remind_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(alarm_remind_icon, LV_ALIGN_TOP_MID, \
        0, 60);

    int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);

    uint8_t am_or_pm;
    uint8_t alarm_id = \
        GetAlarmIsOnId();
    uint8_t alarm_time_hour = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_hour;
    uint8_t alarm_time_minute = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_minute;
    if(time_format == time_format_12)
    {
        if(alarm_time_hour >= 12)
            am_or_pm = 1;
        else
            am_or_pm = 0;

        if(alarm_time_hour > 12)
            alarm_time_hour -= 12;
        else if(alarm_time_hour == 0)
            alarm_time_hour = 12;
    }
    char alarm_time_str[6];
    memset(alarm_time_str, 0 ,\
        sizeof(alarm_time_str));
    sprintf(alarm_time_str, "%02d:%02d", \
        alarm_time_hour, alarm_time_minute);

    memset(&num_str_para, 0, \
        sizeof(widget_num_str_para_t));
    num_str_para.parent = \
        obj;
    num_str_para.num_obj_x = \
        104;
    num_str_para.num_obj_y = \
        184;
    num_str_para.p_num_str = \
        alarm_time_str;
    num_str_para.num_str_len = \
        5;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_34x56_wh_00_index;
    int16_t end_y = \
        common_widget_num_str_create(&num_str_para);
  
    widget_img_para.img_parent = \
        obj;
    if(menu_align == menu_align_left)
        widget_img_para.img_x = \
            end_y + 10;
    else
        widget_img_para.img_x = \
            104 - 36 - 10;
    widget_img_para.img_y = \
        210;
    widget_img_para.file_img_dat = \
        comm_icon_24_index + am_or_pm;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *ampm_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(time_format == time_format_24)
            lv_obj_add_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);

    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    lv_obj_t *comm_11_icon =
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, \
        0, 266);

    widget_img_para.file_img_dat = \
        comm_icon_26_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        alarm_remind_exit_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *exit_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(exit_button, LV_ALIGN_TOP_MID, \
        0, 376);
    lv_obj_set_ext_click_area(exit_button, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_alarm_remind) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_alarm_remind,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
