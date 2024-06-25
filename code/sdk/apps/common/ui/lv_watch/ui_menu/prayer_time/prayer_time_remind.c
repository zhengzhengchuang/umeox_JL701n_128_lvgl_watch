#include "prayer_time_remind.h"

static lv_obj_t *PT_type_label;

static void prayer_time_reminder_on_cb(lv_event_t *e)
{
    if(!e) return;

    u8 PT_type = \
        GetPTimeType();

    bool *remind_en = \
        &(PT_Cfg.remind_en[PT_type]);
    if(*remind_en == false)
        *remind_en = true;
    else
        *remind_en = false; 
    PTimeCfgParaUpdate();

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void prayer_time_voice_on_cb(lv_event_t *e)
{
    if(!e) return;

    u8 PT_type = \
        GetPTimeType();

    bool remind_en = \
        PT_Cfg.remind_en[PT_type];
    bool *voice_en = \
        &(PT_Cfg.voice_en[PT_type]);

    if(remind_en == false) 
        return;

    if(*voice_en == false)
        *voice_en = true;
    else
        *voice_en = false; 
    PTimeCfgParaUpdate();

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_prayer_time_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_prayer_time_remind);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_prayer_time_remind);

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

    u8 PT_type = \
        GetPTimeType();

    bool remind_en = \
        PT_Cfg.remind_en[PT_type];
    bool voice_en = \
        PT_Cfg.voice_en[PT_type];

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
    
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_05_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *PT_reminder_on_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(PT_reminder_on_container, LV_ALIGN_TOP_MID, 0, 136);

    widget_label_para.label_w = \
        180;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    if(menu_align == menu_align_left)
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
    else
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_RIGHT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        PT_reminder_on_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_reminder_on);
    lv_obj_t *PT_reminder_on_label = \
        common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_left)
        lv_obj_align(PT_reminder_on_label, LV_ALIGN_LEFT_MID, 16, 0);
    else
        lv_obj_align(PT_reminder_on_label, LV_ALIGN_RIGHT_MID, -16, 0);

    widget_img_para.img_parent = \
        PT_reminder_on_container;
    if(remind_en)
        widget_img_para.file_img_dat = \
            comm_icon_01_index;
    else
        widget_img_para.file_img_dat = \
            comm_icon_00_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        prayer_time_reminder_on_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *PT_reminder_on_button = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_left)
        lv_obj_align(PT_reminder_on_button, LV_ALIGN_LEFT_MID, 218, 0);
    else
        lv_obj_align(PT_reminder_on_button, LV_ALIGN_RIGHT_MID, -218, 0);
    lv_obj_set_ext_click_area(PT_reminder_on_button, 16);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_05_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *PT_voice_on_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(PT_voice_on_container, LV_ALIGN_TOP_MID, \
        0, 232);

    widget_label_para.label_w = \
        180;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    if(menu_align == menu_align_left)
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
    else
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_RIGHT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        PT_voice_on_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_voice_on);
    lv_obj_t *PT_voice_on_label = \
        common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_left)
        lv_obj_align(PT_voice_on_label, LV_ALIGN_LEFT_MID, 16, 0);
    else
        lv_obj_align(PT_voice_on_label, LV_ALIGN_RIGHT_MID, -16, 0);

    widget_img_para.img_parent = \
        PT_voice_on_container;
    if(remind_en)
    {
        if(voice_en)
            widget_img_para.file_img_dat = comm_icon_01_index;
        else
            widget_img_para.file_img_dat = comm_icon_00_index;
    }else
    {
        if(voice_en)
            widget_img_para.file_img_dat = comm_icon_34_index;
        else
            widget_img_para.file_img_dat = comm_icon_35_index;
    }
    
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        prayer_time_voice_on_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *PT_voice_on_button = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_left)
        lv_obj_align(PT_voice_on_button, LV_ALIGN_LEFT_MID, 218, 0);
    else
        lv_obj_align(PT_voice_on_button, LV_ALIGN_RIGHT_MID, -218, 0);
    lv_obj_set_ext_click_area(PT_voice_on_button, 16);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_prayer_time_remind) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_prayer_time_remind,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
