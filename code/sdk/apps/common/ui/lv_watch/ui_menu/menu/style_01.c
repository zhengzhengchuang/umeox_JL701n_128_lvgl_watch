#include "style_01.h"

static lv_obj_t *list_ctx_container;
static lv_obj_t *elem_container[Elem_Num];

static bool ver_scroll;
static int16_t scroll_dela;
static int16_t scroll_offset;
static lv_point_t min_point = {0};
static lv_point_t max_point = {0};
static lv_point_t now_point = {0};
static lv_point_t start_point = {0};

static const uint16_t ec_h = (Container_H/2);
static const uint16_t ec_w = (Container_W/2);
static const uint8_t visual_line = 2;

static const uint8_t ec_idx[Elem_Num] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, \
    10, 11, 12, 13, 14, 15, 16,
};

static const ui_act_id_t act_id[Elem_Num] = {
    ui_act_id_call_main, ui_act_id_msg_list, ui_act_id_null, \
    ui_act_id_quran_main, ui_act_id_prayer_time_main, ui_act_id_azkar_list, \
    ui_act_id_tasbih_main, ui_act_id_al_name_list, ui_act_id_Gcalendar_main, \
    ui_act_id_null, ui_act_id_sleep_main, ui_act_id_weather_data, \
    ui_act_id_hr_sample, ui_act_id_bo_sample, ui_act_id_alarm_main, \
    ui_act_id_more_menu, ui_act_id_set_main, 
};

static void elem_container_scroll(void);
static void list_ctx_container_cb(lv_event_t *e);

static void del_anim_with_var(void *var)
{
    if(!var) return;

    if(lv_anim_get(var, NULL))
        lv_anim_del(var, NULL);
        
    return;
}

static bool anim_progress_with_var(void *var)
{
    if(lv_anim_get(var, NULL))
        return true;

    return false;
}

static void ver_scroll_judge(void)
{
    lv_indev_t *indev_act = lv_indev_get_act();

    lv_indev_get_point(indev_act, &now_point);

    if(now_point.x > max_point.x)
        max_point.x = now_point.x;

    if(now_point.x < min_point.x)
        min_point.x = now_point.x;

    if(now_point.y > max_point.y)
        max_point.y = now_point.y;

    if(now_point.y < min_point.y)
        min_point.y = now_point.y;

    int16_t x_max = max_point.x - min_point.x;
    int16_t y_max = max_point.y - min_point.y;

    if(LV_ABS(y_max) >= LV_ABS(x_max) && \
        LV_ABS(y_max) >= 10)
        ver_scroll = true;
    else
        ver_scroll = false;

    return;
}

static int16_t calc_throw_sum(int16_t throw_vert)
{
    int16_t throw_vert_sum = 0;
    int16_t throw_vert_tmp = throw_vert;
    
    while(throw_vert_tmp)
    {
        throw_vert_sum += throw_vert_tmp;
        throw_vert_tmp = throw_vert_tmp * 90 / 100;
    }
    
    return throw_vert_sum;
}

static void throw_anim_cb(void *var, int32_t val)
{
    scroll_offset = val;

    elem_container_scroll();

    common_scrollbar_press_handle(\
        scroll_offset);

    return;
}

static void throw_anim_ready_cb(lv_anim_t *anim)
{
    ver_scroll = false;

    common_scrollbar_released_handle();

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = \
        Container_W;
    widget_obj_para.obj_height = \
        Container_H;
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
    widget_obj_para.obj_is_scrollable = false;
    list_ctx_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(list_ctx_container, \
        list_ctx_container_cb, LV_EVENT_ALL, NULL);

    return;
}

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    bool anim_progress = \
        anim_progress_with_var(list_ctx_container);
    if(anim_progress == true) return;
 
    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    if(idx == 2)
    {
        bool cali_succ = \
            GetGmCaliSucc();
        if(cali_succ == false)
            ui_menu_jump(ui_act_id_gm_cali);
        else
        {
            if(!(ll_info.position_valid))
                ui_menu_jump(ui_act_id_kaaba_position);   
            else
                ui_menu_jump(ui_act_id_kaaba_qibla);
        }
    }else
    {
        ui_menu_jump(act_id[idx]);
    }

    return;
}

static void elem_container_create(menu_align_t menu_align)
{
    int16_t ec_sx = 0;
    int16_t ec_sy = 0;

    widget_obj_para.obj_parent = \
        list_ctx_container;
    widget_obj_para.obj_width = \
        ec_w;
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
    widget_obj_para.obj_is_scrollable = false;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_obj_para.obj_x = ec_sx + ec_w*(idx%2);
        widget_obj_para.obj_y = ec_sy + ec_h*(idx/2) + scroll_offset + scroll_dela;
        elem_container[idx] = \
            common_widget_obj_create(&widget_obj_para);
        lv_obj_add_flag(elem_container[idx], LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_event_cb(elem_container[idx], elem_container_cb, \
            LV_EVENT_SHORT_CLICKED, (void *)&ec_idx[idx]);

        if(widget_obj_para.obj_y >= Container_H || \
            (widget_obj_para.obj_y + ec_h) < 0)
            lv_obj_add_flag(elem_container[idx], LV_OBJ_FLAG_HIDDEN);
    }

    return;
}

static void elem_container_scroll(void)
{
    int16_t obj_y = 0;
    int16_t ec_sy = 0;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        obj_y = ec_sy + ec_h*(idx/2) + scroll_offset + scroll_dela;

        if(obj_y >= Container_H || (obj_y + ec_h) < 0)
        {
            lv_obj_add_flag(elem_container[idx], LV_OBJ_FLAG_HIDDEN);
            continue;
        }
            
        lv_obj_clear_flag(elem_container[idx], LV_OBJ_FLAG_HIDDEN);

        lv_obj_set_y(elem_container[idx], obj_y);
    }

    return;
}

static void elem_icon_create(menu_align_t menu_align)
{
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_img_para.img_parent = \
            elem_container[idx];
        widget_img_para.file_img_dat = \
            menu_100_100_icon_00_index + idx;
        lv_obj_t *elem_icon = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(elem_icon, LV_ALIGN_TOP_MID, 0, 20);
    }

    return;
}

static void elem_label_create(menu_align_t menu_align)
{
    widget_label_para.label_w = \
        ec_w - 10;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = NULL;

    for(uint8_t idx = 0; idx < Elem_Num; idx++)
    {
        widget_label_para.label_parent = \
            elem_container[idx];
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_phone + idx);
        lv_obj_t *elem_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(elem_label, LV_ALIGN_TOP_MID, 0, 130);
    }

    return;
}

static void pressed_cb(lv_event_t *e)
{
    scroll_dela = 0;

    ver_scroll = false;

    lv_indev_t *indev_act = lv_indev_get_act();

    lv_indev_get_point(indev_act, &start_point);

    memcpy(&now_point, &start_point, \
        sizeof(lv_point_t));
    memcpy(&min_point, &start_point, \
        sizeof(lv_point_t));
    memcpy(&max_point, &start_point, \
        sizeof(lv_point_t));

    return;
}

static void pressing_cb(lv_event_t *e)
{
    if(!ver_scroll)
    {
        ver_scroll_judge();

        if(ver_scroll)
            del_anim_with_var(list_ctx_container);
    }else
    {
        lv_point_t scroll_vert;
        lv_indev_t *indev_act = lv_indev_get_act();
        lv_indev_get_vect(indev_act, &scroll_vert);
        scroll_dela += scroll_vert.y;

        int16_t scroll_top_val = 0;
        int16_t scroll_bottom_val = \
            (-1)*(((Elem_Num+1)/2) - visual_line)*ec_h;

        if(scroll_offset + scroll_dela > scroll_top_val + ec_h)
            scroll_dela = scroll_top_val + ec_h - scroll_offset;
        else if(scroll_offset + scroll_dela < scroll_bottom_val - ec_h)
            scroll_dela = scroll_bottom_val - ec_h - scroll_offset;
        
        elem_container_scroll();

        common_scrollbar_press_handle(scroll_offset + scroll_dela);
    }

    return;
}

static void release_cb(lv_event_t *e, int *state)
{
    if(!ver_scroll) return;

    int16_t throw_sum;

    if(e != NULL)
    {
        lv_point_t throw_vert;
        lv_indev_t *indev_act = lv_indev_get_act();
        lv_indev_get_throw(indev_act, &throw_vert);
        throw_sum = calc_throw_sum(throw_vert.y);
    }else
    {
        if(*state == Rdec_Forward)
            throw_sum = -1*(ec_h);
        else if(*state == Rdec_Backward)
            throw_sum = 1*(ec_h);
        else
            throw_sum = 0;
    }
    
    scroll_offset += scroll_dela;
    scroll_dela = 0;

    int16_t scroll_top_val = 0;
    int16_t scroll_bottom_val = \
        (-1)*(((Elem_Num+1)/2) - visual_line)*ec_h;

    int16_t throw_start = scroll_offset;
    int16_t throw_end = scroll_offset + throw_sum;

    int16_t throw_adjust = 0;
    uint32_t anim_duration = 0;
    uint32_t anim_min_duration = 300;
    uint32_t anim_max_duration = 700;

    //printf("%d, %d\n", throw_end, scroll_bottom_val);

    if(throw_end > scroll_bottom_val && throw_end < scroll_top_val)
    {
        throw_adjust = throw_end%ec_h;
		if(throw_adjust <= (-1)*(ec_h/2))
            throw_end -= (ec_h + throw_adjust);	
		else
            throw_end -= throw_adjust;

        anim_duration = LV_ABS(throw_sum)*2;    
    }else if(throw_end >= scroll_top_val)
    {
        throw_end = scroll_top_val;

        if(throw_start != scroll_top_val)
            anim_duration = \
                LV_ABS(scroll_top_val - throw_start)*2;
        else
            anim_duration = anim_min_duration;
    }else if(throw_end <= scroll_bottom_val)
    {
        throw_end = scroll_bottom_val;

        if(throw_start != scroll_top_val)
            anim_duration = \
                LV_ABS(scroll_bottom_val - throw_start)*2;
        else
            anim_duration = anim_min_duration;
    }

    //printf("anim_duration = %d\n", anim_duration);

    if(throw_start == throw_end)
        return;

    if(anim_duration < anim_min_duration) \
        anim_duration = anim_min_duration;
    if(anim_duration > anim_max_duration) \
        anim_duration = anim_max_duration;

    lv_anim_t throw_anim;

    widget_anim_para.anim = &throw_anim;
    widget_anim_para.anim_obj = \
        list_ctx_container;
    widget_anim_para.anim_exec_xcb = \
        throw_anim_cb; 
    widget_anim_para.anim_duration = anim_duration;
    widget_anim_para.anim_start_val = throw_start;
    widget_anim_para.anim_end_val = throw_end;
    widget_anim_para.is_start_anim = false;
    common_widget_anim_create(&widget_anim_para);
    lv_anim_set_path_cb(widget_anim_para.anim, \
        lv_anim_path_ease_out);
    lv_anim_set_ready_cb(widget_anim_para.anim, \
        throw_anim_ready_cb);
    lv_anim_start(widget_anim_para.anim);

    return;
}

static void rdec_scroll(int state)
{
    scroll_dela = 0;
    ver_scroll = true;
    del_anim_with_var(list_ctx_container);

    release_cb(NULL, &state);

    return;
}

static void list_ctx_container_cb(lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        pressed_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        pressing_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        release_cb(e, NULL);

    return;
} 

static void layout_create(void)
{
    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    elem_container_create(menu_align);

    elem_icon_create(menu_align);

    elem_label_create(menu_align);

    int16_t scroll_bottom_val = \
        (-1)*(((Elem_Num+1)/2) - visual_line)*ec_h;
    common_scrollbar_create(list_ctx_container, \
        scroll_offset, scroll_bottom_val, menu_align);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    tileview_register_all_menu(obj, ui_act_id_null, \
        ui_act_id_null, ui_act_id_null, ui_act_id_null, \
            ui_act_id_menu);

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

static void menu_rdec_cb(lv_obj_t *obj, int state)
{
    if(!obj) return;

    rdec_scroll(state);

    return;
}

register_ui_menu_load_info(\
    menu_load_style_01) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_menu,
    .disable_te = true,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
