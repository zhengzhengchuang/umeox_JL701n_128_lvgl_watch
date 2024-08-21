#include "prayer_time_popup.h"

static void prayer_time_cancel_cb(lv_event_t *e)
{
    if(!e) return;

    ui_act_id_t *prev_act_id = read_menu_return_level_id();
    ui_menu_jump(prev_act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_prayer_time_popup);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_prayer_time_popup);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    tone_play_stop();
    
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

    u8 PT_type = GetPTimeProcessType();
    u8 advance_flag = GetPTimeAdvanceFlag();

    widget_label_para.label_w = 300;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_Fajr + PT_type);
    lv_obj_t *PT_type_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(PT_type_label, LV_ALIGN_TOP_MID, 0, 40);

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = prayer_time_04_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *PT_icon = common_widget_img_create(&widget_img_para, NULL);
    if(advance_flag == 1)
        lv_obj_align(PT_icon, LV_ALIGN_TOP_MID, 0, 90);
    else
        lv_obj_align(PT_icon, LV_ALIGN_TOP_MID, 0, 98);

    widget_img_para.file_img_dat = comm_icon_11_index;
    lv_obj_t *comm_11_icon = common_widget_img_create(&widget_img_para, NULL);
    if(advance_flag == 1)
        lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 250);
    else
        lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 266);

    if(advance_flag == 1)
    {
        //提前提醒
        s8 val = PT_Cfg.remind_offset[PT_type]/60;
        const char *pword;
        if(val > 0)
            pword = (const char *)get_lang_txt_with_id(lang_txtid_minutes_ago);
        else
            pword = (const char *)get_lang_txt_with_id(lang_txtid_minutes_later);

        val = LV_ABS(val);
        char num_str[4];
        memset(num_str, 0, sizeof(num_str));
        sprintf(num_str, "%d", val);
        u8 num_len = strlen(num_str);

        u16 i;
        u16 len = strlen(pword);
        for(i = 0; i < len; i++)
        {
            if(pword[i] == '0')
                break;
        }

        u16 idx = 0;
        char buf[80] = {0};
        memcpy(&buf[idx], &pword[0], i);
        idx += i;
        memcpy(&buf[idx], num_str, num_len);
        idx += num_len;
        memcpy(&buf[idx], &pword[i + 1], len - i - 1);

        widget_label_para.label_w = 300;
        widget_label_para.label_h = Label_Line_Height;
        widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
        widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
        widget_label_para.label_text_color = lv_color_hex(0xffffff);
        widget_label_para.label_ver_center = false;
        widget_label_para.user_text_font = NULL;
        widget_label_para.label_parent = obj;
        widget_label_para.label_text = buf;
        lv_obj_t *advance_label = common_widget_label_create(&widget_label_para);
        lv_obj_align(advance_label, LV_ALIGN_TOP_MID, 0, 320);
    }

    widget_img_para.file_img_dat = comm_icon_26_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = prayer_time_cancel_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *comm_26_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_26_icon, LV_ALIGN_TOP_MID, 0, 376);
    lv_obj_set_ext_click_area(comm_26_icon, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_prayer_time_popup) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .offscreen_unlock = true,
    .menu_id = \
        ui_act_id_prayer_time_popup,
    .user_offscreen_time = 20*1000,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
