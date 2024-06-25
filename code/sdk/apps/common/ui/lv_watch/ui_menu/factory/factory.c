#include "factory.h"

static char latitude_str[28];
static char longitude_str[28];

static lv_obj_t *latitude_label;
static lv_obj_t *longitude_label;

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_about;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_factory);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_factory);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    float longitude = ll_info.vm_longitude;
    int longitude_0 = (int)longitude;
    int longitude_tmp = (int)(longitude*1000000);
    int longitude_1 = LV_ABS(longitude_tmp)%1000000;
    memset(longitude_str, 0, sizeof(longitude_str));
    sprintf(longitude_str, "经度:%d.%d", longitude_0, longitude_1);
    lv_label_set_text(longitude_label, longitude_str);

    float latitude = ll_info.vm_latitude;
    int latitude_0 = (int)latitude;
    int latitude_tmp = (int)(latitude*1000000);
    int latitude_1 = LV_ABS(latitude_tmp)%1000000;
    memset(latitude_str, 0, sizeof(latitude_str));
    sprintf(latitude_str, "纬度:%d.%d", latitude_0, latitude_1);
    lv_label_set_text(latitude_label, latitude_str);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    float longitude = ll_info.vm_longitude;
    int longitude_0 = (int)longitude;
    int longitude_tmp = (int)(longitude*1000000);
    int longitude_1 = LV_ABS(longitude_tmp)%1000000;
    memset(longitude_str, 0, sizeof(longitude_str));
    sprintf(longitude_str, "经度:%d.%d", longitude_0, longitude_1);
    widget_label_para.label_w = 200;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = longitude_str;
    longitude_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(longitude_label, LV_ALIGN_TOP_MID, 0, 44);

    float latitude = ll_info.vm_latitude;
    int latitude_0 = (int)latitude;
    int latitude_tmp = (int)(latitude*1000000);
    int latitude_1 = LV_ABS(latitude_tmp)%1000000;
    memset(latitude_str, 0, sizeof(latitude_str));
    sprintf(latitude_str, "纬度:%d.%d", latitude_0, latitude_1);
    widget_label_para.label_w = 200;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = latitude_str;
    latitude_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(latitude_label, LV_ALIGN_TOP_MID, 0, 88);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_factory) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_factory,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
