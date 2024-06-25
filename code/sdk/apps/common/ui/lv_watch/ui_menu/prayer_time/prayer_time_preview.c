#include "prayer_time_preview.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;

static const u16 ec_h = 200;
static lv_obj_t *PT_preview_container;
static lv_obj_t *PT_preview_elem_container[PT_Num - 1];

static void scroll_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void ptime_preview_container_create(\
    lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
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
    PT_preview_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(PT_preview_container, scroll_cb, \
        LV_EVENT_SCROLL, NULL);
 
    return;
}

static void ptime_preview_elem_container_create(void)
{
    widget_obj_para.obj_parent = \
        PT_preview_container;
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
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        false;

    for(u8 i = Fajr; i < PT_Num - 1; i++)
    {
        widget_obj_para.obj_y = i*ec_h;
        PT_preview_elem_container[i] = \
            common_widget_obj_create(&widget_obj_para);
    }

    return;
}

static void ptime_preview_elem_label_create(void)
{
    widget_label_para.label_w = \
        288;
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

    for(u8 i = Fajr; i < PT_Num; i++)
    {
        if(i == Sunset)
            continue;
        
        if(i > Sunset)
            widget_label_para.label_parent = \
                PT_preview_elem_container[i - 1];
        else
            widget_label_para.label_parent = \
                PT_preview_elem_container[i];

        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_Fajr + i);
        lv_obj_t *PT_type_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(PT_type_label, LV_ALIGN_TOP_MID, 0, 30);
    }

    return;
}

static void ptime_preview_elem_time_create(void)
{
    char PTNumStr[6];

    u8 am_pm = 0;
    u8 PT_type = \
        GetPTimeType();

    int time_format = \
        GetVmParaCacheByLabel(\
            vm_label_time_format);

    for(u8 i = Fajr; i < PT_Num; i++)
    {
        if(i == Sunset)
            continue;

        memset(PTNumStr, 0, sizeof(PTNumStr));

        uint8_t PTResultHour = \
            PT_Info.result_timestamp[i]/3600;
        uint8_t PTResultMinute = \
            (PT_Info.result_timestamp[i]%3600)/60;
        if(time_format == time_format_12)
        {
            if(PTResultHour >= 12)
                am_pm = 1;
            else
                am_pm = 0;

            if(PTResultHour > 12)
                PTResultHour -= 12;
            else if(PTResultHour == 0)
            PTResultHour = 12; 
        }

        sprintf(PTNumStr, "%02d:%02d", \
            PTResultHour, PTResultMinute);

        if(i > Sunset)
            num_str_para.parent = \
                PT_preview_elem_container[i - 1];
        else
            num_str_para.parent = \
                PT_preview_elem_container[i];
        num_str_para.num_obj_x = \
            103;
        num_str_para.num_obj_y = \
            114;
        num_str_para.p_num_str = \
            PTNumStr;
        num_str_para.num_str_len = \
            5;
        num_str_para.num_obj = \
            NULL;
        num_str_para.num_obj_max = \
            0;
        num_str_para.num_dsc_idx = \
            NULL;
        
        if(PT_type == i)
            num_str_para.file_00_index = \
                comm_num_34x56_ye_00_index;
        else
            num_str_para.file_00_index = \
                comm_num_34x56_wh_00_index;
        int16_t end_x = \
            common_widget_num_str_create(&num_str_para);

        if(i > Sunset)
            widget_img_para.img_parent = \
                PT_preview_elem_container[i - 1];
        else
            widget_img_para.img_parent = \
                PT_preview_elem_container[i];
        widget_img_para.img_x = \
            end_x + 10;
        widget_img_para.img_y = \
            140;
        if(am_pm == 0)
        {
            if(PT_type == i)
                widget_img_para.file_img_dat = \
                    comm_icon_22_index;
            else
               widget_img_para.file_img_dat = \
                    comm_icon_24_index; 
        }else
        {
            if(PT_type == i)
                widget_img_para.file_img_dat = \
                    comm_icon_23_index;
            else
               widget_img_para.file_img_dat = \
                    comm_icon_25_index;
        }
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = \
            NULL;
        lv_obj_t *comm_ampm_icon = \
            common_widget_img_create(&widget_img_para, NULL);
        if(time_format == time_format_24)
            lv_obj_add_flag(comm_ampm_icon, LV_OBJ_FLAG_HIDDEN);
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_prayer_time_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_prayer_time_preview);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_prayer_time_preview);

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

    ptime_preview_container_create(obj);
    ptime_preview_elem_container_create();
    ptime_preview_elem_label_create();
    ptime_preview_elem_time_create();

    lv_obj_scroll_to_y(PT_preview_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(PT_preview_container);
    scroll_top_y = lv_obj_get_scroll_top(PT_preview_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(PT_preview_container);
    scroll_y = scroll_y<scroll_top_y?scroll_top_y:scroll_y;
    scroll_y = scroll_y>scroll_bottom_y?scroll_bottom_y:scroll_y;
    lv_obj_scroll_to_y(PT_preview_container, scroll_y, LV_ANIM_OFF);

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
    if(lv_anim_get(PT_preview_container, NULL))
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
        lv_obj_scroll_to_y(PT_preview_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_prayer_time_preview) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_prayer_time_preview,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
