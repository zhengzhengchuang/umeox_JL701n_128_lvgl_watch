#include "sleep_detail.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *list_ctx_container;

static const int16_t ec_sy = 20;
static const uint16_t ec_h = 136;

static void scroll_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        LCD_UI_Y_OFFSET + ec_sy;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT - (LCD_UI_Y_OFFSET + ec_sy);
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
    lv_obj_add_event_cb(list_ctx_container, scroll_cb, \
        LV_EVENT_SCROLL, NULL);
    
    return;
}

static void elem_ctx_create(menu_align_t menu_align)
{
    bool slp_valid = Slp_Info.valid;

    uint8_t slp_state_h[sl_state_num];
    uint8_t slp_state_m[sl_state_num];

    if(slp_valid)
    {
        slp_state_h[0] = \
            Slp_Info.wakeup_total/60;
        slp_state_m[0] = \
            Slp_Info.wakeup_total%60;
        slp_state_h[1] = \
            Slp_Info.r_slp_total/60;
        slp_state_m[1] = \
            Slp_Info.r_slp_total%60;
        slp_state_h[2] = \
            Slp_Info.l_slp_total/60;
        slp_state_m[2] = \
            Slp_Info.l_slp_total%60;
        slp_state_h[3] = \
            Slp_Info.d_slp_total/60;
        slp_state_m[3] = \
            Slp_Info.d_slp_total%60;
    }else
    {
        memset(slp_state_h, 0, \
            sizeof(slp_state_h));
        memset(slp_state_m, 0, \
            sizeof(slp_state_m));
    }
    
    const u32 label_color[sl_state_num] = {
        0xF0D990, 0xDCC5FB, 0x9B7BE8, 0x6A01FC,
    };

    widget_obj_para.obj_parent = \
        list_ctx_container;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        ec_h;  
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        false;

    char slp_state_h_str[3];
    char slp_state_m_str[3];
    for(uint8_t i = 0; i < sl_state_num; i++)
    {
        widget_obj_para.obj_y = \
            0 + ec_h*i;
        lv_obj_t *ec_container = \
            common_widget_obj_create(&widget_obj_para);

        widget_img_para.img_parent = \
            ec_container;
        widget_img_para.file_img_dat = \
            sleep_state_00_index + i;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = NULL;
        lv_obj_t *slp_state_icon = \
            common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(slp_state_icon, LV_ALIGN_TOP_RIGHT, -30, 16);
        else
            lv_obj_align(slp_state_icon, LV_ALIGN_TOP_LEFT, 30, 16);

        widget_label_para.label_w = \
            220;
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
            lv_color_hex(label_color[i]);
        widget_label_para.label_ver_center = \
            false;
        widget_label_para.user_text_font = \
            NULL;
        widget_label_para.label_parent = \
            ec_container;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_wake_up + i);
        lv_obj_t *slp_state_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align_to(slp_state_label, slp_state_icon, \
                LV_ALIGN_OUT_LEFT_MID, -20, 0);
        else
            lv_obj_align_to(slp_state_label, slp_state_icon, \
                LV_ALIGN_OUT_RIGHT_MID, 20, 0);

        memset(slp_state_h_str, 0, \
            sizeof(slp_state_h_str));
        sprintf(slp_state_h_str, "%02d", \
            slp_state_h[i]);

        num_str_para.parent = \
            ec_container;
        num_str_para.num_obj_x = \
            60;
        num_str_para.num_obj_y = \
            60;
        num_str_para.p_num_str = \
            slp_state_h_str;
        num_str_para.num_str_len = \
            2;
        num_str_para.num_obj = \
            NULL;
        num_str_para.num_obj_max = \
            0;
        num_str_para.num_dsc_idx = \
            NULL;
        num_str_para.file_00_index = \
            comm_num_24x40_wh_00_index;
        int16_t hour_end_x = \
            common_widget_num_str_create(&num_str_para);

        memset(slp_state_m_str, 0, \
            sizeof(slp_state_m_str));
        sprintf(slp_state_m_str, "%02d", \
            slp_state_m[i]);
        num_str_para.num_obj_x = \
            216;
        num_str_para.p_num_str = \
            slp_state_m_str;
        int16_t min_end_x = \
            common_widget_num_str_create(&num_str_para);

        widget_label_para.label_x = \
            hour_end_x + 2;
        widget_label_para.label_y = \
            64;
        widget_label_para.label_w = \
            (95);
        widget_label_para.label_h = \
            Label_Line_Height;
        widget_label_para.long_mode = \
            LV_LABEL_LONG_SCROLL;
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
        widget_label_para.label_text_color = \
            lv_color_hex(0x808080);
        widget_label_para.label_ver_center = \
            false;
        widget_label_para.user_text_font = \
            NULL;
        widget_label_para.label_parent = \
            ec_container;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_hours);
        common_widget_label_create(&widget_label_para);
  
        widget_label_para.label_x = \
            min_end_x + 2;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_unit_mins);
        common_widget_label_create(&widget_label_para);
    }
    
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t up_act_id = \
        ui_act_id_sleep_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_sleep_detail);
    else
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_sleep_detail);

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

    list_ctx_container_create(obj);

    elem_ctx_create(menu_align);

    lv_obj_scroll_to_y(list_ctx_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(list_ctx_container);
    scroll_top_y = lv_obj_get_scroll_top(list_ctx_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(list_ctx_container);
    scroll_y = scroll_y<scroll_top_y?scroll_top_y:scroll_y;
    scroll_y = scroll_y>scroll_bottom_y?scroll_bottom_y:scroll_y;
    lv_obj_scroll_to_y(list_ctx_container, scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

static void menu_rdec_cb(lv_obj_t *obj, int state)
{
    if(lv_anim_get(list_ctx_container, NULL))
        return;

    int16_t scroll_y_tmp = scroll_y;

    if(state == Rdec_Forward)
        scroll_y_tmp += ec_h;
    else if(state == Rdec_Backward)
        scroll_y_tmp -= ec_h;
    else
        return;

    scroll_y_tmp = scroll_y_tmp < scroll_top_y?scroll_top_y:scroll_y_tmp;
    scroll_y_tmp = scroll_y_tmp > scroll_bottom_y?scroll_bottom_y:scroll_y_tmp;
    if(scroll_y_tmp != scroll_y)
        lv_obj_scroll_to_y(list_ctx_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_sleep_detail) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_sleep_detail,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
