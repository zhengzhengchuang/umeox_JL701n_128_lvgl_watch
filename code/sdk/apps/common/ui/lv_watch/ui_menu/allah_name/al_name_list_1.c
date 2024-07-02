#include "al_name_list.h"

static bool ver_scroll;
static int16_t scroll_dela;
static int16_t scroll_offset;

static lv_point_t min_point;
static lv_point_t max_point;
static lv_point_t now_point;
static lv_point_t start_point;

static const uint16_t ec_w = LCD_WIDTH;
static const uint16_t ec_h = 108;
static const uint16_t ec_sx = 0;
static const uint16_t ec_sy = 0;
static const uint8_t visual_line = 4;

static int8_t load_sidx;
static int8_t load_eidx;
static const uint8_t load_range = 10;

static void elem_obj_dy_create(void);
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

    elem_obj_dy_create();

    return;
}

static void throw_anim_ready_cb(lv_anim_t *anim)
{
    ver_scroll = false;

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
    widget_obj_para.obj_is_scrollable = \
        false;
    al_list_ctx_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(al_list_ctx_container, list_ctx_container_cb, \
        LV_EVENT_ALL, NULL);

    return;
}

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    bool anim_progress = \
        anim_progress_with_var(al_list_ctx_container);

    if(ver_scroll || anim_progress)
        return;

    lv_point_t p_act;
    lv_indev_get_point(lv_indev_get_act(), &p_act);

    int16_t sx = LCD_WIDTH - 120;
    int16_t ex = LCD_WIDTH;
    int16_t sy = LCD_UI_Y_OFFSET;
    int16_t ey = LCD_HEIGHT;
    if((p_act.x >= sx && p_act.x < ex) && \
        (p_act.y >= sy && p_act.y < ey))
    {
        SetVmParaCacheByLabel(\
            vm_label_al_name_list_mode, al_name_list_mode0);

        ui_menu_jump(ui_act_id_al_name_list);

        return;
    }

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    printf("idx = %d\n", idx);

    SetAlNameExId(idx);

    ui_menu_jump(ui_act_id_al_name_ex);

    return;
}

static void elem_container_create(menu_align_t menu_align)
{
    widget_obj_para.obj_parent = \
        al_list_ctx_container;
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
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        false;

    for(uint8_t i = 0; i < Elem_Num; i++)
    {
        widget_obj_para.obj_x = \
            ec_sx;
        widget_obj_para.obj_y = \
            ec_sy + ec_h*i + scroll_offset + scroll_dela;
        al_elem_container[i] = common_widget_obj_create(&widget_obj_para);
        lv_obj_add_flag(al_elem_container[i], LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_event_cb(al_elem_container[i], elem_container_cb, \
            LV_EVENT_SHORT_CLICKED, (void *)&al_ec_idx[i]);

        if(widget_obj_para.obj_y >= Container_H || \
            (widget_obj_para.obj_y + ec_h - 1) < 0)
        {
            lv_obj_add_flag(al_elem_container[i], LV_OBJ_FLAG_HIDDEN);
        }
    }

    return;
}

static void elem_container_scroll(void)
{
    int16_t obj_y = 0;

    for(uint8_t i = 0; i < Elem_Num; i++)
    {
        obj_y = ec_sy + ec_h*i + scroll_offset + scroll_dela;

        if(obj_y >= Container_H || (obj_y + ec_h - 1) < 0)
        {
            lv_obj_add_flag(al_elem_container[i], LV_OBJ_FLAG_HIDDEN);

            continue;
        }
            
        lv_obj_clear_flag(al_elem_container[i], LV_OBJ_FLAG_HIDDEN);

        lv_obj_set_y(al_elem_container[i], obj_y);
    }

    return;
}

static void elem_00_create(uint8_t menu_align, int8_t sidx, int8_t eidx)
{
    widget_label_para.label_w = \
        50;
    widget_label_para.label_h = \
        37;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_RIGHT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        &font_common_num_32;
    
    char elem_00_str[3];
    lv_obj_t *elem_00_obj;
    for(int8_t i = sidx; i < eidx; i++)
    {
        memset(elem_00_str, 0 , sizeof(elem_00_str));
        sprintf(elem_00_str, "%02d", i + 1);
        widget_label_para.label_parent = \
            al_elem_container[i];
        widget_label_para.label_text = \
            elem_00_str;
        elem_00_obj = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(elem_00_obj, LV_ALIGN_RIGHT_MID, -40, 5);
    }

    return;
}

static void elem_01_create(uint8_t menu_align, int8_t sidx, int8_t eidx)
{
    widget_label_para.label_w = \
        240;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_RIGHT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;

    lv_obj_t *elem_01_obj;
    for(int8_t i = sidx; i < eidx; i++)
    {
        widget_label_para.label_parent = \
            al_elem_container[i];
        widget_label_para.label_text = \
            GetAllahNameTableAr(lang_txt_al_name_00 + i);
        elem_01_obj = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(elem_01_obj, LV_ALIGN_TOP_RIGHT, -120, 15);
    }

    return;
}

static void elem_02_create(uint8_t menu_align, int8_t sidx, int8_t eidx)
{
    widget_label_para.label_w = \
        240;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_RIGHT;
    widget_label_para.label_text_color = \
        lv_color_hex(0x666666);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;

    lv_obj_t *elem_02_obj;
    for(int8_t i = sidx; i < eidx; i++)
    {
        widget_label_para.label_parent = \
            al_elem_container[i];
        widget_label_para.label_text = \
            GetAllahNameTableEnFr(lang_txt_al_name_00 + i);
        elem_02_obj = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(elem_02_obj, LV_ALIGN_BOTTOM_RIGHT, -120, 0);
    }

    return;
}

static void elem_obj_dy_create(void)
{
    int16_t scroll = scroll_offset + scroll_dela;
    int8_t idx = LV_ABS(scroll/ec_h); 
    int8_t sidx = idx - load_range;
    int8_t eidx = idx + load_range;
    sidx = sidx<0?0:sidx;
    eidx = eidx>Elem_Num?Elem_Num:eidx;
    if(sidx < load_sidx)
    {
        //不在范围内，新的创建
        menu_align_t menu_align = \
            menu_align_left;
        if(AllahNameIsArabic())
            menu_align = menu_align_right;

        printf("sidx = %d, load_sidx = %d\n", sidx, load_sidx);
        elem_00_create(menu_align, sidx, load_sidx);
        elem_01_create(menu_align, sidx, load_sidx);
        elem_02_create(menu_align, sidx, load_sidx);
        load_sidx = sidx;
    }

    if(eidx > load_eidx)
    {
        //不在范围内，新的创建
        menu_align_t menu_align = \
            menu_align_left;
        if(AllahNameIsArabic())
            menu_align = menu_align_right;

        printf("load_eidx = %d, eidx = %d\n", load_eidx, eidx);
        elem_00_create(menu_align, load_eidx, eidx);
        elem_01_create(menu_align, load_eidx, eidx);
        elem_02_create(menu_align, load_eidx, eidx);
        load_eidx = eidx;
    }

    return;
}

static void pressed_cb(lv_event_t *e)
{
    scroll_dela = 0;

    ver_scroll = false;

    lv_indev_t *indev_act = lv_indev_get_act();
    lv_indev_get_point(indev_act, &start_point);

    memcpy(&now_point, &start_point, sizeof(lv_point_t));
    memcpy(&min_point, &start_point, sizeof(lv_point_t));
    memcpy(&max_point, &start_point, sizeof(lv_point_t));

    return;
}

static void pressing_cb(lv_event_t *e)
{
    if(!ver_scroll)
    {
        ver_scroll_judge();

        if(ver_scroll)
            del_anim_with_var(al_list_ctx_container);
    }else
    {
        lv_point_t scroll_vert;
        lv_indev_t *indev_act = lv_indev_get_act();
        lv_indev_get_vect(indev_act, &scroll_vert);
        scroll_dela += scroll_vert.y;

        int16_t scroll_top_val = 0;
        int16_t scroll_bottom_val = \
            (-1)*(Elem_Num - visual_line) * ec_h;

        if(scroll_offset + scroll_dela > scroll_top_val + ec_h)
            scroll_dela = scroll_top_val + ec_h - scroll_offset;
        else if(scroll_offset + scroll_dela < scroll_bottom_val - ec_h)
            scroll_dela = scroll_bottom_val - ec_h - scroll_offset;
        
        elem_container_scroll();

        elem_obj_dy_create();
    }
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
            throw_sum = -2*(ec_h);
        else if(*state == Rdec_Backward)
            throw_sum = 2*(ec_h);
        else
            throw_sum = 0;
    }
    
    scroll_offset += scroll_dela;
    scroll_dela = 0;

    int16_t scroll_top_val = 0;
    int16_t scroll_bottom_val = \
        (-1)*(Elem_Num - visual_line) * ec_h;

    int16_t throw_start = scroll_offset;
    int32_t throw_end = scroll_offset + throw_sum;

    int16_t throw_adjust = 0;
    uint32_t anim_duration = 0;
    uint32_t anim_min_duration = 300;
    uint32_t anim_max_duration = 700;

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

    if(throw_start == throw_end)
        return;

    if(anim_duration < anim_min_duration) \
        anim_duration = anim_min_duration;
    if(anim_duration > anim_max_duration) \
        anim_duration = anim_max_duration;

    lv_anim_t throw_anim;

    widget_anim_para.anim = &throw_anim;
    widget_anim_para.anim_obj = \
        al_list_ctx_container;
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
    del_anim_with_var(al_list_ctx_container);

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
    if(AllahNameIsArabic())
        menu_align = menu_align_right;

    int16_t scroll = scroll_offset + scroll_dela;
    int8_t idx = LV_ABS(scroll/ec_h);
    load_sidx = idx - load_range;
    load_eidx = idx + load_range;
    load_sidx = load_sidx<0?0:load_sidx;
    load_eidx = load_eidx>Elem_Num?Elem_Num:load_eidx;
    printf("sidx = %d, eidx = %d\n", load_sidx, load_eidx);

    elem_container_create(menu_align);

    elem_00_create(menu_align, load_sidx, load_eidx);
    elem_01_create(menu_align, load_sidx, load_eidx);
    elem_02_create(menu_align, load_sidx, load_eidx);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_al_name_list);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_al_name_list);

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

    al_list_ctx_container = NULL;
    for(uint8_t i = 0; i < Elem_Num; i++)
        al_elem_container[i] = NULL;

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
    menu_load_al_name_list1) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_al_name_list,
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
