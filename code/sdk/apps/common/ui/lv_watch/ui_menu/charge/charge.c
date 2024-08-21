#include "charge.h"

static lv_obj_t *power_arc;
static lv_obj_t *charge_label;

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
        ui_act_id_null, ui_act_id_null, ui_act_id_charge);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

#if 1
    u8 charge_power = GetChargePowerFlag();
    if(charge_power == 0)
    {
        u8 power_per = GetBatteryPower();
        lv_arc_set_value(power_arc, power_per);
    }
    
    u32 txt_id;
    u8 charge_full = GetChargeFullFlag();
    if(charge_full == 1)
        txt_id = lang_txtid_charge_full;
    else
        txt_id = lang_txtid_charging;
    lv_label_set_text(charge_label, get_lang_txt_with_id(txt_id));
#endif

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    u8 power_per;
    u8 charge_power = GetChargePowerFlag();
    if(charge_power == 1)
        power_per = 100;
    else
        power_per = GetBatteryPower();
    widget_arc_para.arc_parent = obj;
    widget_arc_para.arc_bg_width = 280;
    widget_arc_para.arc_bg_height = 280;
    widget_arc_para.arc_bg_opax = LV_OPA_0;
    widget_arc_para.arc_main_start_angle = 0;
    widget_arc_para.arc_main_end_angle = 360;
    widget_arc_para.arc_indic_start_angle = 0;
    widget_arc_para.arc_indic_end_angle = 360;
    widget_arc_para.arc_rotation_angle = 270;
    widget_arc_para.arc_min_value = 0;
    widget_arc_para.arc_max_value = 100;
    widget_arc_para.arc_cur_value = power_per;
    widget_arc_para.arc_main_line_width = 16;
    widget_arc_para.arc_indic_line_width = 16;
    widget_arc_para.arc_main_line_color = lv_color_hex(0x333333);
    widget_arc_para.arc_indic_line_color = lv_color_hex(0xF0D990);
    widget_arc_para.arc_main_is_rounded = true;
    widget_arc_para.arc_indic_is_rounded = true;
    widget_arc_para.arc_click_is_clear = true;
    power_arc = common_widget_arc_create(&widget_arc_para);
    lv_obj_align(power_arc, LV_ALIGN_TOP_MID, 0, 46);

    widget_img_para.img_parent = power_arc;
    widget_img_para.file_img_dat = charge_00_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.user_data = NULL;
    lv_obj_t *charge_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_center(charge_icon);

    u32 txt_id;
    u8 charge_full = GetChargeFullFlag();
    if(charge_full == 1)
        txt_id = lang_txtid_charge_full;
    else
        txt_id = lang_txtid_charging;
    widget_label_para.label_w = 330;
    widget_label_para.label_h = Label_Line_Height*2;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(txt_id);
    charge_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(charge_label, LV_ALIGN_TOP_MID, 0, 344);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_charge) = 
{
    .menu_arg = NULL,
    .lock_flag = true,
    .return_flag = false,
    .menu_id = \
        ui_act_id_charge,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
