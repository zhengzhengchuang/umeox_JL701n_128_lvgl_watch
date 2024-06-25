#include "call_log.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *list_ctx_container;

static const u16 ec_h = 118;
static const uint8_t click_idx[\
    Call_log_Max_Num] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
};

static void no_call_log_menu_create(lv_obj_t *obj)
{
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        call_04_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *no_call_log_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(no_call_log_icon, LV_ALIGN_TOP_MID, 0, 88);

    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    lv_obj_t *comm_11_icon = \
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
        get_lang_txt_with_id(lang_txtid_no_record);
    lv_obj_t *no_record_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(no_record_label, LV_ALIGN_TOP_MID, 0, 342);

    return;
}

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
    widget_obj_para.obj_y = 0;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT;
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

static uint32_t sel_state_icon(call_log_state_t state)
{
    uint32_t file_img_dat = \
        call_05_index;

    if(state == call_log_state_in)
        file_img_dat = call_07_index;
    else if(state == call_log_state_out)
        file_img_dat = call_06_index;

    return file_img_dat;
}

static void elem_click_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    bool ret = \
        VmCallLogCtxByIdx(idx);
    if(ret == false) return;

    char *number = \
        r_call_log.number_str;
    uint8_t len = \
        strlen(r_call_log.number_str);

    CtrlCallOutByNum(number, len);

    return;
}

static void has_call_log_menu_create(lv_obj_t *obj, \
    uint8_t num, menu_align_t menu_align)
{
    list_ctx_container_create(obj);

    widget_obj_para.obj_parent = \
        list_ctx_container;
    widget_obj_para.obj_width = \
        LCD_WIDTH;   
    widget_obj_para.obj_height = \
        ec_h;
    widget_obj_para.obj_is_scrollable = \
        false;

    widget_img_para.img_click_attr = \
            false;
    widget_img_para.event_cb = NULL;

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

    uint8_t am_or_pm = 0;
    static char time_str[6];

    for(uint8_t i = 0; i < num; i++)
    {
        bool ret = \
            VmCallLogCtxByIdx(i);
        if(ret == false) continue;

        widget_obj_para.obj_y = 40 + ec_h*i;
        lv_obj_t *elem_container = \
            common_widget_obj_create(&widget_obj_para);
        lv_obj_add_event_cb(elem_container, \
            elem_click_cb, LV_EVENT_SHORT_CLICKED, \
                (void *)&click_idx[i]);

        widget_img_para.img_parent = \
            elem_container;
        widget_img_para.file_img_dat = \
            sel_state_icon(r_call_log.state);
        lv_obj_t *elem_icon = \
             common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(elem_icon, LV_ALIGN_RIGHT_MID, -24, 0);
        else
            lv_obj_align(elem_icon, LV_ALIGN_LEFT_MID, 24, 0);

        uint8_t hour = \
            r_call_log.time.hour;
        uint8_t minute = \
            r_call_log.time.min;

        memset(time_str, 0, sizeof(time_str));
        int time_format = \
            GetVmParaCacheByLabel(vm_label_time_format);
        if(time_format == time_format_12)
        {
           if(hour >= 12)
                am_or_pm = 1;
            else
                am_or_pm = 0;

            if(hour > 12)
                hour -= 12;
            else if(hour == 0)
                hour = 12; 
        }
        sprintf(time_str, "%02d:%02d", hour, minute);

        widget_label_para.label_w = 80;
        widget_label_para.label_h = \
            Label_Line_Height;
        widget_label_para.label_text_color = \
            lv_color_hex(0xffffff);
        widget_label_para.label_parent = \
            elem_container;
        widget_label_para.label_text = \
            time_str;
        lv_obj_t *time_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align_to(time_label, elem_icon, LV_ALIGN_OUT_LEFT_TOP, -15, 4);
        else
            lv_obj_align_to(time_label, elem_icon, LV_ALIGN_OUT_RIGHT_TOP, 15, 4);

        widget_img_para.img_parent = \
            elem_container;
        widget_img_para.file_img_dat = \
            am_or_pm + comm_icon_20_index;
        lv_obj_t *ampm_icon = \
             common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align_to(ampm_icon, time_label, LV_ALIGN_OUT_LEFT_BOTTOM, 0, -5);
        else
            lv_obj_align_to(ampm_icon, time_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 0, -5);
        if(time_format == time_format_24)
            lv_obj_add_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);
        
        char *ctx_str;
        if(strlen(r_call_log.name_str))
            ctx_str = \
                r_call_log.name_str;
        else
            ctx_str = \
                r_call_log.number_str;

        widget_label_para.label_w = 220;
        widget_label_para.label_h = \
            Label_Line_Height;
        widget_label_para.label_text_color = \
            lv_color_hex(0x666666);
        widget_label_para.label_text = ctx_str;
        lv_obj_t *ctx_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align_to(ctx_label, elem_icon, LV_ALIGN_OUT_LEFT_BOTTOM, -15, 0);
        else
            lv_obj_align_to(ctx_label, elem_icon, LV_ALIGN_OUT_RIGHT_BOTTOM, 15, 0);
    }

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

    ui_act_id_t prev_act_id = \
        ui_act_id_call_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_call_log);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_call_log);

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

    uint8_t call_log_num = \
        VmCallLogItemNum();

    if(!call_log_num)
        no_call_log_menu_create(obj);
    else
    {
        menu_align_t menu_align = \
            menu_align_left;
        if(lang_txt_is_arabic())
            menu_align = menu_align_right;

        has_call_log_menu_create(obj, \
            call_log_num, menu_align);
    }
        
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
    uint8_t num = \
        VmCallLogItemNum();
    if(num == 0)
        return;

    if(lv_anim_get(list_ctx_container, NULL))
        return;

    int16_t scroll_y_tmp = scroll_y;

    if(state == Rdec_Forward)
        scroll_y_tmp += ec_h*2;
    else if(state == Rdec_Backward)
        scroll_y_tmp -= ec_h*2;
    else
        return;

    scroll_y_tmp = scroll_y_tmp < scroll_top_y?scroll_top_y:scroll_y_tmp;
    scroll_y_tmp = scroll_y_tmp > scroll_bottom_y?scroll_bottom_y:scroll_y_tmp;
    if(scroll_y_tmp != scroll_y)
        lv_obj_scroll_to_y(list_ctx_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_call_log) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_call_log,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
