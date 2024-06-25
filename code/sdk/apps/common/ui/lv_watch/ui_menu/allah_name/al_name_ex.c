#include "al_name_ex.h"
#include "al_name_list.h"

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_al_name_list;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_al_name_ex);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_al_name_ex);

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

    bool IsArabic = \
        AllahNameIsArabic();

    uint8_t ExId = GetAlNameExId();

    widget_label_para.label_w = 330;
    widget_label_para.label_h = Label_Line_Height*2;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xf0d790);
    widget_label_para.label_ver_center = true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = GetAllahNameTableFollowLang(ExId);
    lv_obj_t *al_name_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(al_name_label, LV_ALIGN_TOP_MID, 0, 40);

    widget_textarea_para.parent = obj;
    widget_textarea_para.width = 330;
    widget_textarea_para.height = LCD_HEIGHT - 130;
    widget_textarea_para.txt = GetAllahNameExTableFollowLang(ExId);
    widget_textarea_para.font = &font_common_30_no_ch;
    widget_textarea_para.color = lv_color_hex(0xffffff);
    if(IsArabic)
        widget_textarea_para.align = LV_TEXT_ALIGN_RIGHT;
    else
        widget_textarea_para.align = LV_TEXT_ALIGN_LEFT;
    widget_textarea_para.bg_opa_x = LV_OPA_0;
    widget_textarea_para.bg_color = lv_color_hex(0x000000);
    widget_textarea_para.border_width = 0;
    widget_textarea_para.border_opa_x = LV_OPA_0;
    widget_textarea_para.border_color = lv_color_hex(0x000000);
    lv_obj_t *al_name_ex_textarea = common_widget_textarea_create(&widget_textarea_para);
    lv_obj_align(al_name_ex_textarea, LV_ALIGN_TOP_MID, 0, 130);
    lv_obj_set_style_pad_top(al_name_ex_textarea, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(al_name_ex_textarea, 20, LV_PART_MAIN);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_al_name_ex) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_al_name_ex,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
