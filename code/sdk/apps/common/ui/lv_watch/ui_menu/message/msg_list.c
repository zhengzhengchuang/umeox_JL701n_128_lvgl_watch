#include "msg_list.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *list_ctx_container;

static const uint16_t ec_h = 214;

static const uint8_t ec_idx[Max_Elem_Num] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
};

static void scroll_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void delete_cb(lv_event_t *e)
{
    if(!e) return;

    VmMessageCtxClear();

    ui_act_id_t cur_act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(cur_act_id);

    return;
}

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    uint8_t num = \
        GetMessageNum();

    if(idx >= num)
        return;

    SetQueryId(idx);

    ui_menu_jump(\
        ui_act_id_msg_detail);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT - LCD_UI_Y_OFFSET;
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
    lv_obj_set_style_pad_bottom(list_ctx_container, \
        25, LV_PART_MAIN);
    lv_obj_add_event_cb(list_ctx_container, \
        scroll_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void no_message_menu_create(lv_obj_t *obj)
{
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        message_icon_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *no_msg_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(no_msg_icon, LV_ALIGN_TOP_MID, 0, 88);

    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    lv_obj_t *comm_11_icon =
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 266);

    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_no_message);
    lv_obj_t *label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 342);

    return;
}

static void has_message_menu_create(lv_obj_t *obj, \
    uint8_t msg_num, menu_align_t menu_align)
{
    if(msg_num >= Max_Elem_Num)
        msg_num = Max_Elem_Num;

    list_ctx_container_create(obj);

    uint8_t idx = 0;
    int16_t ec_sy = 28;
    static char time_str[6];
    
    for(uint8_t i = 0; i < msg_num; i++)
    { 
        bool ret = \
            GetVmFlashMessageCtx(i);
        if(ret == false)
            continue;
       
        widget_img_para.img_parent = \
            list_ctx_container;
        widget_img_para.file_img_dat = \
            message_icon_02_index; 
        widget_img_para.img_click_attr = \
            true;
        widget_img_para.event_cb = \
            elem_container_cb;
        widget_img_para.user_data = \
            (void *)&ec_idx[i];
        lv_obj_t *elem_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(elem_container, LV_ALIGN_TOP_MID, 0, ec_sy + ec_h*i);

        message_type_t type = \
            r_message.message_type;
        widget_img_para.img_parent = \
            elem_container;
        widget_img_para.file_img_dat = \
            message_type_00_index + type; 
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        widget_img_para.user_data = NULL;
        lv_obj_t *msg_type_icon = \
            common_widget_img_create(&widget_img_para, NULL); 
        if(menu_align == menu_align_right)
            lv_obj_align(msg_type_icon, LV_ALIGN_TOP_RIGHT, -20, 10);
        else
            lv_obj_align(msg_type_icon, LV_ALIGN_TOP_LEFT, 20, 10);

        uint8_t hour = \
            r_message.message_time.hour;
        uint8_t minute = \
            r_message.message_time.min;
        memset(time_str, 0, sizeof(time_str));
        sprintf(time_str, "%02d:%02d", hour, minute);
        widget_label_para.label_w = \
            80;
        widget_label_para.label_h = \
            Label_Line_Height;
        widget_label_para.long_mode = \
            LV_LABEL_LONG_CLIP;
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_CENTER;
        widget_label_para.label_text_color = \
            lv_color_hex(0xffffff);
        widget_label_para.label_ver_center = \
            false;
        widget_label_para.user_text_font = \
            NULL;
        widget_label_para.label_parent = \
            elem_container;
        widget_label_para.label_text = \
            time_str;
        lv_obj_t *time_str_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align(time_str_label, LV_ALIGN_TOP_LEFT, 20, 10);
        else
            lv_obj_align(time_str_label, LV_ALIGN_TOP_RIGHT, -20, 10); 

        char *ctx_str = \
            r_message.msg_ctx;
        uint32_t str_len = strlen(ctx_str);
        bool arabic_letter = \
            utf8_str_is_arabic(ctx_str, str_len);
        widget_label_para.label_w = \
            288;
        widget_label_para.label_h = \
            Label_Line_Height*2;
        widget_label_para.long_mode = \
            LV_LABEL_LONG_DOT;
        if(arabic_letter)
            widget_label_para.text_align = \
                LV_TEXT_ALIGN_RIGHT;
        else
            widget_label_para.text_align = \
                LV_TEXT_ALIGN_LEFT;
        widget_label_para.label_text_color = \
            lv_color_hex(0xffffff);
        widget_label_para.label_ver_center = \
            false;
        widget_label_para.user_text_font = \
            NULL;
        widget_label_para.label_parent = \
            elem_container;
        widget_label_para.label_text = \
            ctx_str;
        lv_obj_t *short_msg_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(short_msg_label, LV_ALIGN_TOP_MID, 0, 78);

        idx++;
    }

    widget_img_para.img_parent = \
        list_ctx_container;
    widget_img_para.file_img_dat = \
        comm_icon_14_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        delete_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *delete_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(delete_button, LV_ALIGN_TOP_MID, 0, ec_sy + ec_h*idx + 30);
    lv_obj_set_ext_click_area(delete_button, 20);

    lv_obj_scroll_to_y(list_ctx_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(list_ctx_container);
    scroll_top_y = lv_obj_get_scroll_top(list_ctx_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(list_ctx_container);
    scroll_y = scroll_y<scroll_top_y?scroll_top_y:scroll_y;
    scroll_y = scroll_y>scroll_bottom_y?scroll_bottom_y:scroll_y;
    lv_obj_scroll_to_y(list_ctx_container, scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = \
        p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t up_act_id = \
            ui_act_id_watchface;
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_msg_list);
    }else
    {
        ui_act_id_t prev_act_id = \
            ui_act_id_menu;
        if(!lang_txt_is_arabic())
            tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
                prev_act_id, ui_act_id_null, ui_act_id_msg_list);
        else
            tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
                ui_act_id_null, prev_act_id, ui_act_id_msg_list);
    }

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

    uint8_t num = \
        GetMessageNum();
    if(num == 0)
        no_message_menu_create(obj);
    else
        has_message_menu_create(obj, \
            num, menu_align);

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
    if(!obj) return;

    uint8_t num = \
        GetMessageNum();
    if(num == 0) return;

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
    menu_load_msg_list) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_msg_list,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
