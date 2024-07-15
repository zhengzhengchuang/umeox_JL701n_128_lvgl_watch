#include "tws_list.h"

static uint8_t LSn;
static bool SnUpdate;
static lv_obj_t *list_ctx_container;
static lv_obj_t *elem_container[Elem_Num];

static uint8_t conn_dev;
static uint16_t conn_angle;
static lv_obj_t *tws_icon[Elem_Num];
static lv_obj_t *conn_icon[Elem_Num];

static uint8_t gif;
static lv_obj_t *gif_icon;
static uint16_t gif_dsc_idx;

static const uint16_t ec_h = 84;
static const uint8_t ec_idx[Elem_Num] = {
    0, 1, 2, 3, 4, 5, 6, 7, 
};
static void title_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = LCD_WIDTH;
    widget_obj_para.obj_height = 94;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;
    lv_obj_t *title_container = common_widget_obj_create(&widget_obj_para);
 
    widget_img_para.img_parent = title_container;
    widget_img_para.file_img_dat = tws_gif_00_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    gif_icon = common_widget_img_create(&widget_img_para, &gif_dsc_idx);
    lv_obj_center(gif_icon);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = LCD_UI_Y_OFFSET + 94;
    widget_obj_para.obj_width = LCD_WIDTH;
    widget_obj_para.obj_height = LCD_HEIGHT - LCD_UI_Y_OFFSET - 94;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = true;
    list_ctx_container = common_widget_obj_create(&widget_obj_para);
    lv_obj_set_style_pad_bottom(list_ctx_container, 25, LV_PART_MAIN);
    
    return;
}

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = *(uint8_t *)lv_event_get_user_data(e);

    uint8_t num = TwsScan.num;
    if(idx >= num) return;

    if(TwsConn.state == TWS_CONNECTING || \
        TwsConn.state == TWS_CONNECTED)
        return;

    conn_dev = idx;

    lv_obj_add_flag(tws_icon[idx], LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(conn_icon[idx], LV_OBJ_FLAG_HIDDEN);

    uint8_t *mac = TwsScan.Para[idx].mac;
    TwsBtEmitterConnByMac(mac);

    return;
}

static void elem_container_create(uint8_t menu_align)
{
    int16_t ec_sx = 0;
    int16_t ec_sy = 0;
    uint16_t ec_w = LCD_WIDTH;

    widget_obj_para.obj_parent = list_ctx_container;
    widget_obj_para.obj_width = ec_w;
    widget_obj_para.obj_height = ec_h;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;

    for(uint8_t idx = 0; idx < LSn; idx++)
    {
        widget_obj_para.obj_x = ec_sx;
        widget_obj_para.obj_y = ec_sy + ec_h*idx;
        elem_container[idx] = common_widget_obj_create(&widget_obj_para);
        lv_obj_add_event_cb(elem_container[idx], elem_container_cb, LV_EVENT_SHORT_CLICKED, (void *)&ec_idx[idx]);
    }

    return;
}

static void elem_ctx_create(uint8_t menu_align)
{
    for(uint8_t i = 0; i < LSn; i++)
    {
        widget_img_para.img_parent = elem_container[i];
        widget_img_para.file_img_dat = tws_01_index;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        tws_icon[i] = common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(tws_icon[i], LV_ALIGN_RIGHT_MID, -20, 0);
        else
            lv_obj_align(tws_icon[i], LV_ALIGN_LEFT_MID, 20, 0);

        widget_img_para.file_img_dat = tws_02_index;
        conn_icon[i] = common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(conn_icon[i], LV_ALIGN_RIGHT_MID, -20, 0);
        else
            lv_obj_align(conn_icon[i], LV_ALIGN_LEFT_MID, 20, 0);
        lv_obj_add_flag(conn_icon[i], LV_OBJ_FLAG_HIDDEN);

        widget_label_para.label_parent = elem_container[i];
        widget_label_para.label_w = 260;
        widget_label_para.label_h = Label_Line_Height;
        widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
        if(menu_align == menu_align_right)
            widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
        else
            widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
        widget_label_para.label_text_color = lv_color_hex(0xffffff);
        widget_label_para.label_ver_center = false;
        widget_label_para.user_text_font = NULL;
        widget_label_para.label_text = TwsScan.Para[i].name;
        lv_obj_t *name_label = common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align(name_label, LV_ALIGN_RIGHT_MID, -86, 0);
        else
            lv_obj_align(name_label, LV_ALIGN_LEFT_MID, 86, 0);
    }

    return;
}

static void layout_create(void)
{
    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    elem_container_create(menu_align);

    elem_ctx_create(menu_align);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_tws_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_tws_list);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_tws_list);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    if(SnUpdate == false)
        bt_emitter_stop_search_device();
    
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    if(TwsScan.state == TWS_SCANNING)
    {
        gif++;
        gif %= 3;
        uint32_t file = tws_gif_00_index + gif;
        common_widget_img_replace_src(gif_icon, file, gif_dsc_idx);

        common_offscreen_timer_restart();
    }else if(TwsScan.state == TWS_SCAN_TIMEOUT && LSn == 0)
    {
        ui_menu_jump(ui_act_id_scan_timeout);
        goto __end;
    }

    if(TwsConn.state == TWS_CONNECTED)
    {
        ui_menu_jump(ui_act_id_tws_main);
        goto __end; 
    }else if(TwsConn.state == TWS_CONNECTING)
    {
        if(conn_dev != 0xff)
        {
            conn_angle += 360;
            conn_angle %= 3600;
            lv_img_set_angle(conn_icon[conn_dev], conn_angle);
        }

        common_offscreen_timer_restart();
        goto __end;
    }

    if(LSn != TwsScan.num)
    {
        LSn = TwsScan.num;
        SnUpdate = true;
        
        ui_act_id_t act_id = \
            p_ui_info_cache->cur_act_id;
        ui_menu_jump(act_id);
        goto __end;
    }

__end:
    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    LSn = TwsScan.num;
    SnUpdate = false;
    conn_dev = 0xff;
    conn_angle = 0;

    for(uint8_t i = 0; i < Elem_Num; i++)
        elem_container[i] = NULL;

    title_create(obj);

    list_ctx_container_create(obj);

    layout_create();

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_tws_list) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_tws_list,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 200,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
