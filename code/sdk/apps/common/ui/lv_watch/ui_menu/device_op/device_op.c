#include "device_op.h"

static void shutdown_event_cb(lv_event_t *e)
{
    if(!e) return;

    DevOpShutdownHandle();

    return;
}

static void restore_event_cb(lv_event_t *e)
{
    if(!e) return;

    DevOpRestartHandle();

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_device_op);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_device_op);

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
  
    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 108;
    widget_img_para.img_y = 32;
    widget_img_para.file_img_dat = \
        shutdown_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = \
        shutdown_event_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = 20;
    widget_img_para.img_y = 224;
    widget_img_para.file_img_dat = \
        restore_00_index;
    widget_img_para.event_cb = \
        restore_event_cb;
    common_widget_img_create(&widget_img_para, NULL);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_device_op) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_device_op,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
