#include "tasbih_days.h"
#include "tasbih_main.h"
#include "tasbih_inv_time.h"
#include "tasbih_end_time.h"
#include "tasbih_start_time.h"

static int16_t tasbih_main_scroll_y;
static lv_obj_t *tasbih_main_ctx_container;


static void tasbih_main_enable_event_cb(\
    lv_event_t *e)
{
    if(!e) return;

    bool *TasbihR_Enable = \
        &(TasbihR_info.TasbihR_Enable);
    if(*TasbihR_Enable == true)
        *TasbihR_Enable = false;
    else if(*TasbihR_Enable == false)
        *TasbihR_Enable = true;

    TasbihRInfoParaUpdate();

    ui_menu_jump(\
        p_ui_info_cache->cur_act_id);

    return;
}

static void tasbih_main_enable_create(lv_obj_t *obj, \
    menu_align_t menu_align)
{
    bool TasbihR_Enable = \
        TasbihR_info.TasbihR_Enable;

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_05_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *enable_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(enable_container, LV_ALIGN_TOP_MID, 0, 52);

    widget_label_para.label_parent = \
        enable_container;
    widget_label_para.label_w = \
        180;
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
    widget_label_para.user_text_font = \
        NULL;
    if(TasbihR_Enable)
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_reminders_on);
    else
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_reminders_off);
    lv_obj_t *enable_label = \
        common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_right)
        lv_obj_align(enable_label, LV_ALIGN_RIGHT_MID, -20, 0);
    else
        lv_obj_align(enable_label, LV_ALIGN_LEFT_MID, 20, 0);

    widget_img_para.img_parent = \
        enable_container;
    widget_img_para.file_img_dat = \
        comm_icon_00_index + TasbihR_Enable;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_main_enable_event_cb;
    widget_img_para.user_data = \
        NULL;  
    lv_obj_t *enable_switch = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align(enable_switch, LV_ALIGN_LEFT_MID, 20, 0);
    else
        lv_obj_align(enable_switch, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_set_ext_click_area(enable_switch, 10);

    return;
}

static void tasbih_main_ctx_container_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);
    tasbih_main_scroll_y = \
        lv_obj_get_scroll_y(obj);

    return;
}

static void tasbih_main_ctx_container_create(lv_obj_t *obj)
{
    bool TasbihR_Enable = \
        TasbihR_info.TasbihR_Enable;

    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        146;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT - 146;
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
    tasbih_main_ctx_container = \
        common_widget_obj_create(&widget_obj_para);
    // lv_obj_set_style_pad_bottom(tasbih_main_ctx_container, \
    //     0, LV_PART_MAIN);
    lv_obj_add_event_cb(tasbih_main_ctx_container, \
        tasbih_main_ctx_container_cb, LV_EVENT_SCROLL, NULL);
    if(!TasbihR_Enable)
        lv_obj_add_flag(tasbih_main_ctx_container, \
            LV_OBJ_FLAG_HIDDEN);

    return;
}

static void tasbih_main_time_range_cb(\
    lv_event_t *e)
{
    if(!e) return;

    u32 TasbihR_STimestamp = \
        TasbihR_info.TasbihR_STimestamp;
    uint8_t STimeHour = \
        TasbihR_STimestamp/3600;
    uint8_t STimeMinute = \
        (TasbihR_STimestamp%3600)/60;
    SetTasbih_stime_hour(STimeHour);
    SetTasbih_stime_minute(STimeMinute);

    u32 TasbihR_ETimestamp = \
        TasbihR_info.TasbihR_ETimestamp;
    uint8_t ETimeHour = \
        TasbihR_ETimestamp/3600;
    uint8_t ETimeMinute = \
        (TasbihR_ETimestamp%3600)/60;
    SetTasbih_etime_hour(ETimeHour);
    SetTasbih_etime_minute(ETimeMinute);

    ui_menu_jump(\
        ui_act_id_tasbih_start_time);

    return;
}

static void tasbih_main_time_range_create(\
    menu_align_t menu_align)
{
    widget_img_para.img_parent = \
        tasbih_main_ctx_container;
    widget_img_para.file_img_dat = \
        tasbih_bg_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_main_time_range_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *time_range_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(time_range_container, LV_ALIGN_TOP_MID, \
        0, 146*0);
  
    widget_img_para.img_parent = \
        time_range_container;
    widget_img_para.file_img_dat = \
        tasbih_icon_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *time_range_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)  
        lv_obj_align(time_range_icon, LV_ALIGN_RIGHT_MID, \
            -24, 0);
    else
        lv_obj_align(time_range_icon, LV_ALIGN_LEFT_MID, \
            24, 0);

    widget_img_para.file_img_dat = \
        tasbih_line_00_index;
    lv_obj_t *time_range_line = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align_to(time_range_line, time_range_icon, \
            LV_ALIGN_OUT_LEFT_MID, -16, 0);
    else
        lv_obj_align_to(time_range_line, time_range_icon, \
            LV_ALIGN_OUT_RIGHT_MID, 16, 0);

    widget_label_para.label_w = \
        150;
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
        time_range_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_time_range);
    lv_obj_t *time_range_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align_to(time_range_label, time_range_line, \
        LV_ALIGN_OUT_TOP_MID, 0, 0);
    
    int16_t line_x = \
        lv_obj_get_x(time_range_line);
    u32 TasbihR_STimestamp = \
        TasbihR_info.TasbihR_STimestamp;
    u32 TasbihR_ETimestamp = \
        TasbihR_info.TasbihR_ETimestamp;
    char TasbihRTimeStr[12];
    memset(TasbihRTimeStr, 0, \
        sizeof(TasbihRTimeStr));
    sprintf(TasbihRTimeStr, "%02d:%02d-%02d:%02d", \
        TasbihR_STimestamp/3600, (TasbihR_STimestamp%3600)/60, \
            TasbihR_ETimestamp/3600, (TasbihR_ETimestamp%3600)/60);
    num_str_para.parent = \
       time_range_container;
    num_str_para.num_obj_x = \
        line_x + 20;
    num_str_para.num_obj_y = \
        80;
    num_str_para.p_num_str = \
        TasbihRTimeStr;
    num_str_para.num_str_len = \
        11;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_12x20_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    return;
};

static void tasbih_main_interval_time_cb(\
    lv_event_t *e)
{
    if(!e) return;

    u32 TasbihR_SetInvTime = \
        TasbihR_info.TasbihR_SetInvTime;
    uint8_t TimeHour = \
        TasbihR_SetInvTime/3600;
    uint8_t TimeMinute = \
        (TasbihR_SetInvTime%3600)/60;
    SetTasbih_inv_time_hour(TimeHour);
    SetTasbih_inv_time_minute(TimeMinute);

    ui_menu_jump(\
        ui_act_id_tasbih_inv_time);

    return;
}

static void tasbih_main_interval_time_create(\
    menu_align_t menu_align)
{
    widget_img_para.img_parent = \
        tasbih_main_ctx_container;
    widget_img_para.file_img_dat = \
        tasbih_bg_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_main_interval_time_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *interval_time_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(interval_time_container, LV_ALIGN_TOP_MID, \
        0, 146*1);

    widget_img_para.img_parent = \
        interval_time_container;
    widget_img_para.file_img_dat = \
        tasbih_icon_01_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *interval_time_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)  
        lv_obj_align(interval_time_icon, LV_ALIGN_RIGHT_MID, \
            -24, 0);
    else
        lv_obj_align(interval_time_icon, LV_ALIGN_LEFT_MID, \
            24, 0);

    widget_img_para.file_img_dat = \
        tasbih_line_00_index;
    lv_obj_t *interval_time_line = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align_to(interval_time_line, interval_time_icon, \
            LV_ALIGN_OUT_LEFT_MID, -16, 0);
    else
        lv_obj_align_to(interval_time_line, interval_time_icon, \
            LV_ALIGN_OUT_RIGHT_MID, 16, 0);

    widget_label_para.label_w = \
        150;
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
        interval_time_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_interval_time);
    lv_obj_t *interval_time_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align_to(interval_time_label, interval_time_line, \
        LV_ALIGN_OUT_TOP_MID, 0, 0);

    int16_t line_x = \
        lv_obj_get_x(interval_time_line);
    u32 TasbihR_SetInvTime = \
        TasbihR_info.TasbihR_SetInvTime;
    char IntervalTimeStr[3];
    memset(IntervalTimeStr, 0, \
        sizeof(IntervalTimeStr));
    sprintf(IntervalTimeStr, "%02d", \
        TasbihR_SetInvTime/3600);
    num_str_para.parent = \
       interval_time_container;
    num_str_para.num_obj_x = \
        line_x + 40;
    num_str_para.num_obj_y = \
        80;
    num_str_para.p_num_str = \
        IntervalTimeStr;
    num_str_para.num_str_len = \
        2;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_12x20_wh_00_index;
    int16_t h_end_x = \
        common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
       interval_time_container;
    widget_img_para.img_x = \
        h_end_x;
    widget_img_para.img_y = \
        80;
    widget_img_para.file_img_dat = \
        comm_icon_41_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    common_widget_img_create(&widget_img_para, NULL);

    memset(IntervalTimeStr, 0, \
        sizeof(IntervalTimeStr));
    sprintf(IntervalTimeStr, "%02d", \
        (TasbihR_SetInvTime%3600)/60);
    num_str_para.num_obj_x = \
        h_end_x + 12;
    int16_t m_end_x = \
        common_widget_num_str_create(&num_str_para);

    widget_img_para.img_x = \
        m_end_x;
    widget_img_para.file_img_dat = \
        comm_icon_42_index;
    common_widget_img_create(&widget_img_para, NULL);

    return;
}

#if 0
static void tasbih_main_days_cb(\
    lv_event_t *e)
{
    if(!e) return;

    uint8_t TasbihR_Days = \
        TasbihR_info.TasbihR_Days;
    SetTasbihR_DaysTmp(TasbihR_Days);

    ui_menu_jump(\
        ui_act_id_tasbih_days);

    return;
}

static void tasbih_main_days_create(\
    menu_align_t menu_align)
{
    widget_img_para.img_parent = \
        tasbih_main_ctx_container;
    widget_img_para.file_img_dat = \
        tasbih_bg_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_main_days_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *days_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(days_container, LV_ALIGN_TOP_MID, \
        0, 146*2);

    widget_img_para.img_parent = \
        days_container;
    widget_img_para.file_img_dat = \
        tasbih_icon_02_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *days_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)  
        lv_obj_align(days_icon, LV_ALIGN_RIGHT_MID, \
            -24, 0);
    else
        lv_obj_align(days_icon, LV_ALIGN_LEFT_MID, \
            24, 0);

    widget_img_para.file_img_dat = \
        tasbih_line_00_index;
    lv_obj_t *days_line = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align_to(days_line, days_icon, \
            LV_ALIGN_OUT_LEFT_MID, -16, 0);
    else
        lv_obj_align_to(days_line, days_icon, \
            LV_ALIGN_OUT_RIGHT_MID, 16, 0);

    widget_label_para.label_w = \
        150;
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
        days_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_days);
    lv_obj_t *days_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align_to(days_label, days_line, \
        LV_ALIGN_OUT_TOP_MID, 0, 0);

    uint8_t TasbihR_Days = \
        TasbihR_info.TasbihR_Days;

    widget_img_para.img_parent = \
        days_container;
    widget_img_para.img_y = \
        80;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;

    int16_t total_width = 0;
    int16_t line_x = \
        lv_obj_get_x(days_line);
    for(uint8_t j = 0; j < 7; j++)
    {
        if(menu_align == menu_align_right)
        {
            widget_img_para.img_x = \
                line_x + (-3) + (7 - j - 1)*22;
            
            if(TasbihR_Days & (0x01 << j))
                widget_img_para.file_img_dat = \
                    tasbih_repeat_00_index + (j*2) + 1;
            else
                widget_img_para.file_img_dat = \
                    tasbih_repeat_00_index + (j*2) + 0;
                    
            common_widget_img_create(&widget_img_para, NULL);
        }else
        {
            widget_img_para.img_x = \
                line_x + (-3) + total_width;
            if(TasbihR_Days & (0x01 << j))
                widget_img_para.file_img_dat = \
                    tasbih_repeat_00_index + (j*2) + 1;
            else
                widget_img_para.file_img_dat = \
                    tasbih_repeat_00_index + (j*2) + 0;
                
            common_widget_img_create(&widget_img_para, NULL);

            total_width += 22;
        }
    }

    return;
}
#endif

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_tasbih_main);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_tasbih_main);

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

    tasbih_main_enable_create(obj, \
        menu_align);
    
    tasbih_main_ctx_container_create(obj);

    tasbih_main_time_range_create(menu_align);

    tasbih_main_interval_time_create(menu_align);

    //tasbih_main_days_create(menu_align);

    lv_obj_scroll_to_y(tasbih_main_ctx_container, \
        tasbih_main_scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_tasbih_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_tasbih_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
