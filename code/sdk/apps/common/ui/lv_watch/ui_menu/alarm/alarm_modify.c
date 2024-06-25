#include "alarm_main.h"
#include "alarm_time.h"
#include "alarm_repeat.h"
#include "alarm_modify.h"

static void alarm_modify_edit_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t alarm_id = \
        GetAlarmEditId();

    uint8_t alarm_hour = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_hour;
    uint8_t alarm_minute = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_minute;
    SetAlarmTimeTmp(alarm_hour, alarm_minute);

    uint8_t alarm_repeat = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_repeat;
    SetAlarmRepeatTmp(alarm_repeat);

    ui_menu_jump(ui_act_id_alarm_time);

    return;
}

static void alarm_modify_delete_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t alarm_id = \
        GetAlarmEditId();

    UserAlarmDeleteOne(alarm_id);

    ui_menu_jump(ui_act_id_alarm_main);

    return;
}

static void alarm_modify_enable_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t alarm_id = \
        GetAlarmEditId();
    alarm_union_t alarm_union_tmp;
    alarm_union_tmp.info = \
        Alarm_Info.alarm_union[alarm_id].info;
    if(alarm_union_tmp.bit_field.alarm_enable)
        alarm_union_tmp.bit_field.alarm_enable = 0;
    else
        alarm_union_tmp.bit_field.alarm_enable = 1;
    UserAlarmEnableModify(alarm_union_tmp.info);

    ui_act_id_t cur_act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(cur_act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_alarm_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_alarm_modify);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_alarm_modify);

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

    uint8_t alarm_id = \
        GetAlarmEditId();

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_05_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *alarm_modify_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(alarm_modify_container, LV_ALIGN_TOP_MID, \
        0, 52);

    /************闹钟时间num************/
     int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);

    uint8_t am_or_pm;
    char alarm_time_str[6];
    uint8_t alarm_time_hour;
    uint8_t alarm_time_minute;
    memset(alarm_time_str, 0, \
        sizeof(alarm_time_str));
    alarm_time_hour = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_hour;
    alarm_time_minute = \
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
    sprintf(alarm_time_str, "%02d:%02d", \
        alarm_time_hour, alarm_time_minute);

    widget_label_para.label_parent = \
        alarm_modify_container;
    widget_label_para.label_w = \
        70;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_CLIP;
    if(menu_align == menu_align_right)
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text = \
        alarm_time_str;
    widget_label_para.label_text_color = \
        lv_color_hex(0xF0D990);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        &font_common_num_32;
    lv_obj_t *alarm_time_label = \
        common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_right)
        lv_obj_align(alarm_time_label, LV_ALIGN_TOP_RIGHT, \
            -25, 10);
    else
        lv_obj_align(alarm_time_label, LV_ALIGN_TOP_LEFT, \
            25, 10);

    widget_img_para.img_parent = \
        alarm_modify_container;
    widget_img_para.file_img_dat = \
        comm_icon_22_index + am_or_pm;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *ampm_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align_to(ampm_icon, alarm_time_label, \
            LV_ALIGN_OUT_LEFT_MID, 0, 0);
    else
        lv_obj_align_to(ampm_icon, alarm_time_label, \
            LV_ALIGN_OUT_RIGHT_MID, 0, 0);
    if(time_format == time_format_24)
        lv_obj_add_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);

    uint8_t alarm_enable = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_enable;
    widget_img_para.img_parent = \
        alarm_modify_container;
    widget_img_para.file_img_dat = \
        comm_icon_00_index + alarm_enable;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        alarm_modify_enable_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *alarm_enable_sw = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align(alarm_enable_sw, LV_ALIGN_LEFT_MID, \
            16, 0);
    else
        lv_obj_align(alarm_enable_sw, LV_ALIGN_RIGHT_MID, \
            -16, 0);
    lv_obj_set_ext_click_area(alarm_enable_sw, 16);

    uint8_t alarm_repeat = \
        Alarm_Info.alarm_union[alarm_id].bit_field.alarm_repeat;
    widget_img_para.img_parent = \
        alarm_modify_container;
    widget_img_para.img_y = \
        52;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    
    int16_t total_width = 0;
    lv_obj_t *alarm_repeat_sw;
    for(uint8_t j = 0; j < 7; j++)
    {
        if(menu_align == menu_align_right)
        {
            widget_img_para.img_x = \
                296 - (j + 1)*24;
            
            if(alarm_repeat & (0x01 << j))
                widget_img_para.file_img_dat = \
                    alarm_repeat_00_index + (j*2) + 1;
            else
                widget_img_para.file_img_dat = \
                    alarm_repeat_00_index + (j*2) + 0;
            alarm_repeat_sw = \
                common_widget_img_create(&widget_img_para, NULL);
        }else
        {
            widget_img_para.img_x = \
                24 + total_width;
            if(alarm_repeat & (0x01 << j))
                widget_img_para.file_img_dat = \
                    alarm_repeat_00_index + (j*2) + 1;
            else
                widget_img_para.file_img_dat = \
                    alarm_repeat_00_index + (j*2) + 0;
                
            alarm_repeat_sw = \
                common_widget_img_create(&widget_img_para, NULL);
            total_width += 24;
        }
    }

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_14_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        alarm_modify_delete_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *delete_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align_to(delete_button, alarm_modify_container, \
        LV_ALIGN_OUT_BOTTOM_LEFT, 40, 80);
    lv_obj_set_ext_click_area(delete_button, 20);

    widget_img_para.file_img_dat = \
        comm_icon_15_index;
    widget_img_para.event_cb = \
        alarm_modify_edit_cb;
    lv_obj_t *edit_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align_to(edit_button, alarm_modify_container, \
        LV_ALIGN_OUT_BOTTOM_RIGHT, -40, 80);
    lv_obj_set_ext_click_area(delete_button, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_alarm_modify) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_alarm_modify,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
