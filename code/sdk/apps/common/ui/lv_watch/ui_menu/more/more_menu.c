#include "more_menu.h"

static lv_obj_t *title_container;
static lv_obj_t *list_ctx_container;
static lv_obj_t *elem_container[Elem_Num];

static const uint8_t ec_idx[Elem_Num] = {0, 1};

static void title_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = \
        Title_Container_W;
    widget_obj_para.obj_height = \
        Title_Container_H;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        true;
    title_container = \
        common_widget_obj_create(&widget_obj_para);
  
    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        Title_Container_H + LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = \
        List_Ctx_Container_W;
    widget_obj_para.obj_height = \
        List_Ctx_Container_H;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        true;
    list_ctx_container = \
        common_widget_obj_create(&widget_obj_para);

    return;
}

static void title_label_create(menu_align_t menu_align)
{
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
        title_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_more);
    lv_obj_t *title_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);

    return;
}

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);
    if(idx == 0)
    {
        bool exist = \
            Countdown_Info.exist;
        if(!exist)
            ui_menu_jump(\
                ui_act_id_countdown_time);
        else
            ui_menu_jump(\
                ui_act_id_countdown_state);
    }else if(idx == 1)
    {
        bool exist = \
            Stopwatch_Info.exist;
        if(!exist)
            ui_menu_jump(\
                ui_act_id_stopwatch_main);
        else
            ui_menu_jump(\
                ui_act_id_stopwatch_state);
    }

    return;
}

static void elem_container_create(menu_align_t menu_align)
{
    int16_t ec_sx = 0;
    int16_t ec_sy = 0;
    uint16_t ec_w = \
        LCD_WIDTH;
    uint16_t ec_h = \
        112;

    widget_obj_para.obj_parent = \
        list_ctx_container;
    widget_obj_para.obj_width = \
        ec_w;
    widget_obj_para.obj_height = \
        ec_h;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        false;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_obj_para.obj_x = \
            ec_sx;
        widget_obj_para.obj_y = \
            ec_sy + ec_h*idx;
        elem_container[idx] = \
            common_widget_obj_create(&widget_obj_para);
        lv_obj_add_event_cb(elem_container[idx], elem_container_cb, \
            LV_EVENT_SHORT_CLICKED, (void *)&ec_idx[idx]);
    }

    return;
}

static void elem_ctx_icon_create(menu_align_t menu_align)
{
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_img_para.img_parent = \
            elem_container[idx];
        widget_img_para.file_img_dat = \
            more_icon_01_index + idx;
        lv_obj_t *elem_ctx_icon = \
            common_widget_img_create(&widget_img_para, NULL);

        if(menu_align == menu_align_right)
            lv_obj_align(elem_ctx_icon, LV_ALIGN_RIGHT_MID, -20, 0);
        else
            lv_obj_align(elem_ctx_icon, LV_ALIGN_LEFT_MID, 20, 0);
    }

    return;
}

static void elem_ctx_label_create(menu_align_t menu_align)
{
    widget_label_para.label_w = \
        200;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    if(menu_align == menu_align_right)
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        true;
    widget_label_para.user_text_font = NULL;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_label_para.label_parent = \
            elem_container[idx];
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_timer + idx);
        lv_obj_t *elem_ctx_label = \
            common_widget_label_create(&widget_label_para);

        if(menu_align == menu_align_right)
            lv_obj_align(elem_ctx_label, LV_ALIGN_RIGHT_MID, -120, 0);
        else
            lv_obj_align(elem_ctx_label, LV_ALIGN_LEFT_MID, 120, 0);
    }

    return;
}

static void elem_ctx_arrow_create(menu_align_t menu_align)
{
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;

    if(menu_align == menu_align_right)
        widget_img_para.file_img_dat = \
            comm_icon_40_index;
    else
        widget_img_para.file_img_dat = \
            comm_icon_39_index;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_img_para.img_parent = \
            elem_container[idx];
        lv_obj_t *elem_ctx_arrow = \
            common_widget_img_create(&widget_img_para, NULL);

        if(menu_align == menu_align_right)
            lv_obj_align(elem_ctx_arrow, LV_ALIGN_LEFT_MID, 10, 0);
        else
            lv_obj_align(elem_ctx_arrow, LV_ALIGN_RIGHT_MID, -10, 0);
    }

    return;
}

static void menu_layout_create(void)
{
    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = \
            menu_align_right;

    title_label_create(menu_align);

    elem_container_create(menu_align);

    elem_ctx_icon_create(menu_align);

    elem_ctx_label_create(menu_align);

    elem_ctx_arrow_create(menu_align);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_more_menu);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_more_menu);

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

    title_container_create(obj);

    list_ctx_container_create(obj);

    menu_layout_create();

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_more_menu) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_more_menu,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
