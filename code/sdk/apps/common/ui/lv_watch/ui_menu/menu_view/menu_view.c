#include "menu_view.h"

static lv_obj_t *elem_container[\
    ui_menu_view_num];
static ui_menu_view_t menu_view_cache = \
    ui_menu_view_num;
    
static const uint8_t ex_idx[ui_menu_view_num] = {
    ui_menu_view_00, ui_menu_view_01, \
    ui_menu_view_02,
};

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t menu_view_sel = \
        *(uint8_t *)lv_event_get_user_data(e);

    if(menu_view_sel == menu_view_cache)
        return;

    menu_view_cache = menu_view_sel;

    SetVmParaCacheByLabel(vm_label_menu_view, \
        menu_view_cache);

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void confirm_button_cb(lv_event_t *e)
{
    if(!e) return;

    int menu_view_vm_cache = \
        (int)menu_view_cache;
    
    SetVmParaCacheByLabel(\
        vm_label_menu_view, menu_view_vm_cache);

    ui_menu_jump(ui_act_id_set_main);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_set_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_menu_view);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_menu_view);

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

    int cur_menu_view = \
        GetVmParaCacheByLabel(\
            vm_label_menu_view);

    menu_view_cache = \
        (ui_menu_view_t)cur_menu_view;
    
    widget_img_para.img_parent = \
        obj;
    widget_img_para.img_x = 24;

    for(uint8_t i = ui_menu_view_00; i < \
        ui_menu_view_num; i++)
    {
        widget_img_para.img_y = 52 + \
            96*i;
        if(i == menu_view_cache)
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;
        widget_img_para.img_click_attr = \
            true;
        widget_img_para.event_cb = \
            elem_container_cb;
        widget_img_para.user_data = \
            (void *)&ex_idx[i];
        elem_container[i] = \
            common_widget_img_create(&widget_img_para, NULL);
    }

    for(uint8_t i = ui_menu_view_00; i < \
        ui_menu_view_num; i++)
    {
        widget_img_para.img_parent = \
            elem_container[i];
        widget_img_para.file_img_dat = \
            menu_view_00_index + i;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        lv_obj_t *menu_view_icon = \
            common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(menu_view_icon, LV_ALIGN_RIGHT_MID, \
                -44, 0);
        else
            lv_obj_align(menu_view_icon, LV_ALIGN_LEFT_MID, \
                44, 0);
    }

    widget_label_para.label_x = 0;
    widget_label_para.label_y = 0;
    widget_label_para.label_w = 180;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    if(menu_align == menu_align_right)
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = NULL;
    for(uint8_t i = ui_menu_view_00; i < \
        ui_menu_view_num; i++)
    {
        widget_label_para.label_parent = \
            elem_container[i];
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_view_list + i);
        lv_obj_t *menu_view_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align(menu_view_label, LV_ALIGN_RIGHT_MID, \
                -104, 0);
        else
            lv_obj_align(menu_view_label, LV_ALIGN_LEFT_MID, \
                104, 0);
    }

    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 137;
    widget_img_para.img_y = 376;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.event_cb = \
        confirm_button_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para,  NULL);
    lv_obj_set_ext_click_area(confirm_button, 20);
    
    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_menu_view) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_menu_view,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
