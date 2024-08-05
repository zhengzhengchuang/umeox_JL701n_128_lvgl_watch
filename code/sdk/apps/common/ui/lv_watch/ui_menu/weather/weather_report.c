#include "weather_report.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;

static const u16 ec_h = 102;

static lv_obj_t *list_ctx_container;

static void scroll_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = LCD_WIDTH;
    widget_obj_para.obj_height = LCD_HEIGHT - LCD_UI_Y_OFFSET;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = true;
    list_ctx_container = common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(list_ctx_container, scroll_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void elem_ctx_create(menu_align_t menu_align)
{
    int16_t ec_w = LCD_WIDTH;

    struct sys_time time;
    SecToUtcTime(Weather_Info.timestamp, &time);
    u8 week = GetUtcWeek(&time);
    
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_parent = list_ctx_container;
    widget_obj_para.obj_width = ec_w;
    widget_obj_para.obj_height = ec_h;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_is_scrollable = false;
    widget_obj_para.obj_radius = 0;

    for(uint8_t i = 0; i < Weather_Sync_Days; i++)
    {
        weather_type_t type = weather_weather_type(i);
        if(type == weather_type_unknown)
            continue;

        widget_obj_para.obj_y = 0 + ec_h*i;
        lv_obj_t *ec_container = common_widget_obj_create(&widget_obj_para);

        widget_img_para.img_parent = ec_container;
        widget_img_para.file_img_dat = weather_72x68_00_index + type;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        lv_obj_t *type_icon = common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(type_icon, LV_ALIGN_RIGHT_MID, -24, 0);
        else
            lv_obj_align(type_icon, LV_ALIGN_LEFT_MID, 24, 0);

        widget_label_para.label_w = 220;
        widget_label_para.label_h = Label_Line_Height;
        widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
        widget_label_para.label_text_color = lv_color_hex(0xffffff);
        widget_label_para.label_ver_center = false;
        widget_label_para.user_text_font = NULL;
        widget_label_para.label_parent = ec_container;
        widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_sunday + week);
        if(menu_align == menu_align_right)
            widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
        else
            widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
        lv_obj_t *week_label = common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align_to(week_label, type_icon, LV_ALIGN_OUT_LEFT_TOP, -20, -10);
        else
            lv_obj_align_to(week_label, type_icon, LV_ALIGN_OUT_RIGHT_TOP, 20, -10);
        week += 1;
        week %= Comm_Enum_Week_Max;

        int16_t min_tempera = weather_min_temper(i); 
        widget_data_para.data_x = 116;
        widget_data_para.data_y = 62;
        widget_data_para.num_inv = 0;
        widget_data_para.data_parent = ec_container;
        widget_data_para.num_addr_index = comm_num_14x22_wh_00_index;
        widget_data_para.data_align = widget_data_align_left;
        int16_t min_end_x = \
            common_data_widget_create(&widget_data_para, widget_data_type_min0_weather + (i*2), &min_tempera); 

        int16_t max_tempera = weather_max_temper(i);
        widget_data_para.data_x = 189;
        widget_data_para.data_align = widget_data_align_right;
        int16_t max_end_x = \
            common_data_widget_create(&widget_data_para, widget_data_type_max0_weather + (i*2), &max_tempera); 

        widget_img_para.img_parent = ec_container;
        widget_img_para.img_x = min_end_x + 4;
        widget_img_para.img_y = 60;
        widget_img_para.file_img_dat = weather_other_00_index;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        common_widget_img_create(&widget_img_para, NULL);

        widget_img_para.img_x = max_end_x + 4;
        widget_img_para.file_img_dat = weather_other_01_index;
        common_widget_img_create(&widget_img_para, NULL);
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t up_act_id = \
        ui_act_id_weather_data;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_weather_report);
    else
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_weather_report);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bool data_valid = \
        Weather_Info.valid;
    if(!data_valid)
        ui_menu_jump(ui_act_id_weather_data);

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
    menu_load_weather_report) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_weather_report,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
