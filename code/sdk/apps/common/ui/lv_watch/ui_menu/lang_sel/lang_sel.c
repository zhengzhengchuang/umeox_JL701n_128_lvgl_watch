#include "lang_sel.h"

static const char *text_src[Elem_Num] = 
{
#if Conf_Lang_Ar
    "العربية",
#endif

#if Conf_Lang_En
    "English",
#endif

#if Conf_Lang_Fr
    "Français",
#endif

#if Conf_Lang_Ur
    "اردو",
#endif

#if Conf_Lang_Tu
    "Türk",
#endif

#if Conf_Lang_Ch
    "简体中文",
#endif

#if Conf_Lang_In
    "Bahasa Indonesia",
#endif

#if Conf_Lang_Pe
    "فارسی",
#endif

#if Conf_Lang_Ge
    "Deutsch",
#endif

#if Conf_Lang_Th
    "ภาษาไทย",
#endif

#if Conf_Lang_Ru
    "Pусский",
#endif

#if Conf_Lang_Hi
    "Hindi",//需要翻译
#endif

#if Conf_Lang_Ma
    "Bahasa Malaysia",//需要翻译
#endif

#if Conf_Lang_Uz
    "Ўзбек тили",//需要翻译
#endif
};

static const u16 ec_h = 112;
static const uint8_t ec_idx[Elem_Num] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};


static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *title_container;
static lv_obj_t *list_ctx_container;
static lv_obj_t *elem_container[Elem_Num];

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
        false;
    title_container = \
        common_widget_obj_create(&widget_obj_para);
  
    return;
}

static void scroll_cb(lv_event_t *e)
{
    if(!e) return;

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
    lv_obj_add_event_cb(list_ctx_container, scroll_cb, \
        LV_EVENT_SCROLL, NULL);

    return;
}

static void title_label_create(menu_align_t menu_align)
{
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
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        title_container;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_language);
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
    int cur_lang = \
        GetVmParaCacheByLabel(vm_label_sys_language);
    if(idx == cur_lang)
        return;

    cur_lang = idx;
    SetVmParaCacheByLabel(vm_label_sys_language, cur_lang);

    ui_act_id_t act_id = p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);
    
    return;
}

static void elem_container_create(menu_align_t menu_align)
{
    widget_obj_para.obj_parent = \
        list_ctx_container;
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

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_obj_para.obj_x = 0;
        widget_obj_para.obj_y = \
            0 + ec_h*idx;
        elem_container[idx] = \
            common_widget_obj_create(&widget_obj_para);
        lv_obj_add_event_cb(elem_container[idx], elem_container_cb, \
            LV_EVENT_SHORT_CLICKED, (void *)&ec_idx[idx]);
    }

    return;
}

static void elem_ctx_all_create(menu_align_t menu_align)
{
    int cur_lang = \
        GetVmParaCacheByLabel(vm_label_sys_language);

    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;

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

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_img_para.img_parent = \
            elem_container[idx];
        if(idx == cur_lang)
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;
        lv_obj_t *sel_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_center(sel_container);

        widget_label_para.label_parent = \
            sel_container;
        widget_label_para.label_text = \
            text_src[idx];
        lv_obj_t *elem_ctx_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_center(elem_ctx_label);
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

    elem_ctx_all_create(menu_align);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_set_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_lang_sel);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_lang_sel);

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
    menu_load_lang_sel) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_lang_sel,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
