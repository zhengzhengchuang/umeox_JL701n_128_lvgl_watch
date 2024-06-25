#include "unit_switch.h"

#if 0
static const uint8_t unit_distance_idx[\
    Unit_Distance_Sel_Num] = {
    unit_distance_kilometre, unit_distance_mile,
};

static const uint16_t unit_distance_txtid[\
    Unit_Distance_Sel_Num] = {
    lang_txtid_kilometer, lang_txtid_mile,
};

static const uint8_t unit_temperature_idx[\
    Unit_Temperature_Sel_Num] = {
    unit_temperature_C, unit_temperature_F,
};

static const uint16_t unit_temperature_txtid[\
    Unit_Temperature_Sel_Num] = {
    lang_txtid_unit_C, lang_txtid_unit_F,
};

static unit_switch_ctx_t unit_switch_ctx = {0};

static void unit_switch_container_create(lv_obj_t *obj)
{
    lv_obj_t **unit_switch_container = \
        &(unit_switch_ctx.unit_switch_container);

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
    widget_obj_para.obj_is_scrollable = true;
    *unit_switch_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_set_style_pad_bottom(*unit_switch_container, \
        25, LV_PART_MAIN);

    return;
}

static void unit_switch_distance_label_create(void)
{
    lv_obj_t *unit_switch_container = \
        unit_switch_ctx.unit_switch_container;

    widget_label_para.label_x = 0;
    widget_label_para.label_y = 0;
    widget_label_para.label_w = \
        (LCD_WIDTH - 30);
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
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = \
        unit_switch_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_distance);
    lv_obj_t *distance_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(distance_label, LV_ALIGN_TOP_MID, \
        0, 20);

    return;
}

/*********************************************************************************
                                  距离单位选项事件回调                                 
*********************************************************************************/
static void unit_switch_distance_sel_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);
    lv_obj_t **unit_distance_img = \
        unit_switch_ctx.unit_distance_img;
    uint16_t *unit_distance_img_dsc = \
        unit_switch_ctx.unit_distance_img_dsc;

    int cur_unit_distance = \
        GetVmParaCacheByLabel(\
            vm_label_unit_distance);

    if(idx == cur_unit_distance)
        return;

    cur_unit_distance = (int)idx;

    SetVmParaCacheByLabel(\
        vm_label_unit_distance, cur_unit_distance);

    uint32_t file_img_dat;
    for(uint8_t i = 0; i < Unit_Distance_Sel_Num; i++)
    {
        if(i == cur_unit_distance)
            file_img_dat = \
                comm_icon_06_index;
        else
            file_img_dat = \
                comm_icon_05_index;

        common_widget_img_replace_src(\
            unit_distance_img[i], file_img_dat, \
                unit_distance_img_dsc[i]);
    }

    return;
}

/*********************************************************************************
                                  创建距离单位选项                                 
*********************************************************************************/
static void unit_switch_distance_sel_create(void)
{
    lv_obj_t *unit_switch_container = \
        unit_switch_ctx.unit_switch_container;
    lv_obj_t **unit_distance_img = \
        unit_switch_ctx.unit_distance_img;
    lv_obj_t **unit_distance_label = \
        unit_switch_ctx.unit_distance_label;
    uint16_t *unit_distance_img_dsc = \
        unit_switch_ctx.unit_distance_img_dsc;

    int cur_unit_distance = \
        GetVmParaCacheByLabel(\
            vm_label_unit_distance);

    widget_img_para.img_x = 24;
    widget_img_para.img_parent = \
        unit_switch_container;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        unit_switch_distance_sel_cb;

    for(uint8_t i = 0; i < Unit_Distance_Sel_Num; i++)
    {
        widget_img_para.img_y = \
            78 + 96*i;
        widget_img_para.user_data = \
            &unit_distance_idx[i];
        
        if(i == cur_unit_distance)
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;

        unit_distance_img[i] = \
            common_widget_img_create(&widget_img_para, \
                &unit_distance_img_dsc[i]);
    }

    widget_label_para.label_x = 0;
    widget_label_para.label_y = 0;
    widget_label_para.label_w = 240;
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
    widget_label_para.user_text_font = NULL;

    for(uint8_t i = 0; i < Unit_Distance_Sel_Num; i++)
    {
        widget_label_para.label_parent = \
            unit_distance_img[i];
        widget_label_para.label_text = \
            get_lang_txt_with_id(unit_distance_txtid[i]);
        unit_distance_label[i] = \
            common_widget_label_create(&widget_label_para);
        lv_obj_center(unit_distance_label[i]);
    }

    return;
}

/*********************************************************************************
                                  创建温度标签                                 
*********************************************************************************/
static void unit_switch_temperature_label_create(void)
{
    lv_obj_t *unit_switch_container = \
        unit_switch_ctx.unit_switch_container;

    widget_label_para.label_x = 0;
    widget_label_para.label_y = 0;
    widget_label_para.label_w = \
        (LCD_WIDTH - 30);
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
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = \
        unit_switch_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_temperature);
    lv_obj_t *temperature_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(temperature_label, LV_ALIGN_TOP_MID, \
        0, 310);

    return;
}

/*********************************************************************************
                                  温度单位选项事件回调                                 
*********************************************************************************/
static void unit_switch_temperature_sel_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);
    lv_obj_t **unit_temperature_img = \
        unit_switch_ctx.unit_temperature_img;
    uint16_t *unit_temperature_img_dsc = \
        unit_switch_ctx.unit_temperature_img_dsc;

    int cur_unit_temperature = \
        GetVmParaCacheByLabel(\
            vm_label_unit_temperature);

    if(idx == cur_unit_temperature)
        return;

    cur_unit_temperature = (int)idx;

    SetVmParaCacheByLabel(\
        vm_label_unit_temperature, cur_unit_temperature);

    uint32_t file_img_dat;
    for(uint8_t i = 0; i < Unit_Temperature_Sel_Num; i++)
    {
        if(i == cur_unit_temperature)
            file_img_dat = \
                comm_icon_06_index;
        else
            file_img_dat = \
                comm_icon_05_index;

        common_widget_img_replace_src(\
            unit_temperature_img[i], file_img_dat, \
                unit_temperature_img_dsc[i]);
    }

    return;
}

/*********************************************************************************
                                  创建温度单位选项                                 
*********************************************************************************/
static void unit_switch_temperature_sel_create(void)
{
    lv_obj_t *unit_switch_container = \
        unit_switch_ctx.unit_switch_container;
    lv_obj_t **unit_temperature_img = \
        unit_switch_ctx.unit_temperature_img;
    lv_obj_t **unit_temperature_label = \
        unit_switch_ctx.unit_temperature_label;
    uint16_t *unit_temperature_img_dsc = \
        unit_switch_ctx.unit_temperature_img_dsc;

    int cur_unit_temperature = \
        GetVmParaCacheByLabel(\
            vm_label_unit_temperature);

    widget_img_para.img_x = 24;
    widget_img_para.img_parent = \
        unit_switch_container;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        unit_switch_temperature_sel_cb;

    for(uint8_t i = 0; i < Unit_Temperature_Sel_Num; i++)
    {
        widget_img_para.img_y = \
            368 + 96*i;
        widget_img_para.user_data = \
            &unit_temperature_idx[i];
        
        if(i == cur_unit_temperature)
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;

        unit_temperature_img[i] = \
            common_widget_img_create(&widget_img_para, \
                &unit_temperature_img_dsc[i]);
    }

    widget_label_para.label_x = 0;
    widget_label_para.label_y = 0;
    widget_label_para.label_w = 240;
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
    widget_label_para.user_text_font = NULL;

    for(uint8_t i = 0; i < Unit_Temperature_Sel_Num; i++)
    {
        widget_label_para.label_parent = \
            unit_temperature_img[i];
        widget_label_para.label_text = \
            get_lang_txt_with_id(unit_temperature_txtid[i]);
        unit_temperature_label[i] = \
            common_widget_label_create(&widget_label_para);
        lv_obj_center(unit_temperature_label[i]);
    }

    return;
}
#endif

static int16_t scroll_y;
static lv_obj_t *list_ctx_container;

static const uint8_t unit_0_idx[\
    unit_distance_num] = {
    unit_distance_kilometre, unit_distance_mile,
};
static const uint8_t unit_1_idx[\
    unit_temperature_num] = {
    unit_temperature_C, unit_temperature_F,
};

static void scroll_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);
    scroll_y = \
        lv_obj_get_scroll_y(obj);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
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
    list_ctx_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_set_style_pad_bottom(list_ctx_container, 25, \
        LV_PART_MAIN);
    lv_obj_add_event_cb(list_ctx_container, scroll_cb, \
        LV_EVENT_SCROLL, NULL);

    return;
}

static void unit_0_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);
    int unit_dis = \
        GetVmParaCacheByLabel(vm_label_unit_distance);
    if(unit_dis == idx)
        return;

    unit_dis = idx;
    SetVmParaCacheByLabel(vm_label_unit_distance, unit_dis);

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void elem_ctx_unit_0_create(menu_align_t menu_align)
{
    int unit_dis = \
        GetVmParaCacheByLabel(vm_label_unit_distance);

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
        list_ctx_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_distance);
    lv_obj_t *title_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);

    widget_img_para.img_parent = \
        list_ctx_container;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        unit_0_cb;

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
    widget_label_para.user_text_font = NULL;

    for(uint8_t idx = unit_distance_kilometre; idx < \
        unit_distance_num; idx++)
    {
        if(idx == unit_dis)
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;
        widget_img_para.user_data = \
            (void *)&unit_0_idx[idx];
        lv_obj_t *sel_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(sel_container, LV_ALIGN_TOP_MID, 0, 84 + 95*idx);

        widget_label_para.label_parent = \
            sel_container;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_kilometer + idx);
        lv_obj_t *elem_ctx_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_center(elem_ctx_label);
    }

    return;
}

static void unit_1_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);
    int unit_temper = \
        GetVmParaCacheByLabel(vm_label_unit_temperature);
    if(unit_temper == idx)
        return;

    unit_temper = idx;
    SetVmParaCacheByLabel(vm_label_unit_temperature, unit_temper);

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void elem_ctx_unit_1_create(menu_align_t menu_align)
{
    int unit_temper = \
        GetVmParaCacheByLabel(vm_label_unit_temperature);

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
        list_ctx_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_temperature);
    lv_obj_t *title_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 304);

    widget_img_para.img_parent = \
        list_ctx_container;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        unit_1_cb;

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
    widget_label_para.user_text_font = NULL;

    for(uint8_t idx = unit_temperature_C; idx < \
        unit_temperature_num; idx++)
    {
        if(idx == unit_temper)
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;
        widget_img_para.user_data = \
            (void *)&unit_1_idx[idx];
        lv_obj_t *sel_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(sel_container, LV_ALIGN_TOP_MID, 0, 368 + 95*idx);

        widget_label_para.label_parent = \
            sel_container;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_unit_C + idx);
        lv_obj_t *elem_ctx_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_center(elem_ctx_label);
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_set_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_unit_switch);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_unit_switch);

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

    elem_ctx_unit_0_create(menu_align);

    elem_ctx_unit_1_create(menu_align);

    lv_obj_scroll_to_y(list_ctx_container, \
        scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_unit_switch) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_unit_switch,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
