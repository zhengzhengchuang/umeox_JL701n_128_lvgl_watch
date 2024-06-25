#include "tasbih_days.h"

static uint8_t TasbihR_DaysTmp;
static int16_t tasbih_days_scroll_y = 0;
static lv_obj_t *tasbih_days_scroll_container;

static const uint8_t tasbih_days_idx[7] = {
    0, 1, 2, 3, 4, 5, 6,
};

uint8_t GetTasbihR_DaysTmp(void)
{
    return TasbihR_DaysTmp;
}

void SetTasbihR_DaysTmp(uint8_t Days)
{
    TasbihR_DaysTmp = Days;

    return;
}

static void tasbih_days_scroll_container_cb(\
    lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);
    tasbih_days_scroll_y = \
        lv_obj_get_scroll_y(obj);

    return;
}

static void tasbih_days_scroll_container_create(\
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
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        true;
    tasbih_days_scroll_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_set_style_pad_bottom(tasbih_days_scroll_container, \
        25, LV_PART_MAIN);
    lv_obj_add_event_cb(tasbih_days_scroll_container, \
        tasbih_days_scroll_container_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void tasbih_days_elem_container_cb(\
    lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    if(TasbihR_DaysTmp & (0x01 << idx))
        TasbihR_DaysTmp &= ~(0x01 << idx);
    else
        TasbihR_DaysTmp |= (0x01 << idx);

    ui_act_id_t cur_act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(cur_act_id);

    return;
}

static void tasbih_days_set_confirm_cb(lv_event_t *e)
{
    if(!e) return;

    TasbihR_info.TasbihR_Days = \
        TasbihR_DaysTmp;

    TasbihRInfoParaUpdate();

    ui_menu_jump(\
        ui_act_id_tasbih_main);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_tasbih_days);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_tasbih_days);

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

    tasbih_days_scroll_container_create(\
        obj);

    uint8_t i;
    
    for(i = 0; i < 7; i++)
    {
        widget_img_para.img_parent = \
            tasbih_days_scroll_container;
        if(TasbihR_DaysTmp & (0x01 << i))
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;
        widget_img_para.img_click_attr = \
            true;
        widget_img_para.event_cb = \
            tasbih_days_elem_container_cb;
        widget_img_para.user_data = \
            (void *)&tasbih_days_idx[i];
        lv_obj_t *tasbih_days_elem_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(tasbih_days_elem_container, LV_ALIGN_TOP_MID, \
            0, 32 + i*96);

        widget_label_para.label_parent = \
            tasbih_days_elem_container;
        widget_label_para.label_w = \
            (180);
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
        widget_label_para.label_text = \
            get_lang_txt_with_id(\
                lang_txtid_sunday + i);
        lv_obj_t *tasbih_days_week_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align(tasbih_days_week_label, LV_ALIGN_RIGHT_MID, \
                -25, 0);
        else
            lv_obj_align(tasbih_days_week_label, LV_ALIGN_LEFT_MID, \
                25, 0);

        widget_img_para.img_parent = \
            tasbih_days_elem_container;
        if(TasbihR_DaysTmp & (0x01 << i))
            widget_img_para.file_img_dat = \
                comm_icon_17_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_16_index;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = \
            NULL;
        lv_obj_t *comm_sel_icon = \
            common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(comm_sel_icon, LV_ALIGN_LEFT_MID, \
                25, 0);
        else
            lv_obj_align(comm_sel_icon, LV_ALIGN_RIGHT_MID, \
                -25, 0);
    }

    widget_img_para.img_parent = \
        tasbih_days_scroll_container;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        tasbih_days_set_confirm_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *confirm_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(confirm_button, LV_ALIGN_TOP_MID, \
        0, 32 + i*96 + 50);
    lv_obj_set_ext_click_area(confirm_button, 20);

    lv_obj_scroll_to_y(tasbih_days_scroll_container, \
        tasbih_days_scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_tasbih_days) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_tasbih_days,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
