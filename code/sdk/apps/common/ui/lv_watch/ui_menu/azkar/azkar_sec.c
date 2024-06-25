#include "azkar_sec.h"

#define Elem_Num (Azkar_Sec_Max)

static int8_t load_sidx;
static int8_t load_eidx;
static const uint8_t load_range = 3;

static bool hor_scroll;
static int16_t scroll_dela;
static int16_t scroll_offset;

static lv_point_t min_point = {0};
static lv_point_t max_point = {0};
static lv_point_t now_point = {0};
static lv_point_t start_point = {0};

static lv_obj_t *list_ctx_container;
static lv_obj_t *elem_container[Elem_Num];

static void elem_obj_dy_create(void);
static void elem_container_scroll(void);
static void __this_event_cb(lv_event_t *e);

void ClearScrollPara(void)
{
    scroll_dela = 0;
    scroll_offset = 0;
    hor_scroll = false;

    return;
}

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

static void hor_scroll_judge(void)
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

    if(LV_ABS(x_max) >= LV_ABS(y_max) && LV_ABS(x_max) >= 10)
        hor_scroll = true;
    else
        hor_scroll = false;

    return;
}

static void throw_anim_cb(void *var, int32_t val)
{   
    scroll_offset = val;

    elem_container_scroll();

    return;
}

static void throw_ready_cb(lv_anim_t *a)
{
    hor_scroll = false;

    elem_obj_dy_create();

    int16_t scroll = scroll_offset + scroll_dela;
    uint8_t SecIdx = LV_ABS(scroll)/Container_W;
    uint8_t SecNum = GetAzkarSecNum();
    SecIdx %= SecNum;
    SetAzkarSecIdx(SecIdx);

    lv_obj_t *tileview_obj = tileview_obj_container();
    if(SecIdx == 0)
        lv_obj_add_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);
    else
        lv_obj_clear_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = Container_W;
    widget_obj_para.obj_height = Container_H;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;
    list_ctx_container = common_widget_obj_create(&widget_obj_para);

    return;
}

static void elem_container_create(uint8_t menu_align)
{
    uint8_t SecNum = GetAzkarSecNum();

    widget_obj_para.obj_parent = list_ctx_container;
    widget_obj_para.obj_width = Container_W;
    widget_obj_para.obj_height = Container_H;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;

    for(uint8_t idx = 0; idx < SecNum; idx++)
    {
        if(menu_align == menu_align_right)
            widget_obj_para.obj_x = -1*idx*Container_W;
        else
            widget_obj_para.obj_x = idx*Container_W;
        widget_obj_para.obj_y = 0;
        elem_container[idx] = \
            common_widget_obj_create(&widget_obj_para);
        lv_obj_add_event_cb(elem_container[idx], __this_event_cb, LV_EVENT_ALL, NULL);
        
        if(widget_obj_para.obj_x >= Container_W || \
            (widget_obj_para.obj_x + Container_W - 1) < 0)
            lv_obj_add_flag(elem_container[idx], LV_OBJ_FLAG_HIDDEN);
    }

    return;
}

static void elem_container_scroll(void)
{
    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    int16_t obj_x = 0;
    uint8_t SecNum = GetAzkarSecNum();

    for(uint8_t i = 0; i < SecNum; i++)
    {
        if(menu_align == menu_align_right)
            obj_x = 0 - Container_W*i + scroll_offset + scroll_dela;
        else
            obj_x = 0 + Container_W*i + scroll_offset + scroll_dela;
            
        if(obj_x >= Container_W || (obj_x + Container_W - 1) < 0)
        {
            lv_obj_add_flag(elem_container[i], LV_OBJ_FLAG_HIDDEN);

            continue;
        }

        lv_obj_clear_flag(elem_container[i], LV_OBJ_FLAG_HIDDEN);

        lv_obj_set_x(elem_container[i], obj_x);
    }

    return;
}

static lv_obj_t *img_container_create(\
    uint8_t idx, int16_t x, int16_t y, uint16_t w, uint16_t h)
{ 
    widget_obj_para.obj_x = x;
    widget_obj_para.obj_y = y;
    widget_obj_para.obj_width = w;
    widget_obj_para.obj_height = h;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_parent = elem_container[idx];
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = true;
    lv_obj_t *obj = common_widget_obj_create(&widget_obj_para);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_style_pad_top(obj, 20, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(obj, 60, LV_PART_MAIN);

    return obj;
}

static void elem_ctx_create(uint8_t menu_align, int8_t sidx, int8_t eidx)
{
    char freq_str[8];
  
    int16_t sec_pos0 = 20;
    int16_t sec_pos1 = 55;
    int16_t sec_pos2 = 88;
    int16_t sec_pos3 = 124;

    uint8_t lang = GetAzkarTxtLang();

    for(int8_t i = sidx; i < eidx; i++)
    {
        int freq_num = GetAzkarFreq(i);
        if(freq_num > 0) 
        {
            memset(freq_str, 0, sizeof(freq_str));
            sprintf(freq_str, "[%d]", freq_num);
            widget_label_para.label_w = 60;
            widget_label_para.label_h = 28;
            widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
            widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
            widget_label_para.label_text_color = lv_color_hex(0xf0d790);
            widget_label_para.label_ver_center = false;
            widget_label_para.user_text_font = &font_common_num_24;
            widget_label_para.label_parent = elem_container[i];
            widget_label_para.label_text = freq_str;
            lv_obj_t *freq_label = common_widget_label_create(&widget_label_para);
            lv_obj_align(freq_label, LV_ALIGN_TOP_MID, 0, sec_pos0);
        }

        int title_ret = GetAzkarTitle(i);
        if(title_ret > 0)
        {
            widget_label_para.label_w = 320;
            widget_label_para.label_h = 33*2;
            widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
            widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
            widget_label_para.label_text_color = lv_color_hex(0xf0d790);
            widget_label_para.label_ver_center = true;
            widget_label_para.user_text_font = &font_common_24_no_ch;
            widget_label_para.label_parent = elem_container[i];
            widget_label_para.label_text = GetpTitle();
            lv_obj_t * title_label = common_widget_label_create(&widget_label_para);
            if(freq_num > 0)
                lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, sec_pos1);
            else
                lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, sec_pos0);
        }

        int16_t y;
        if(freq_num > 0)
        {
            if(title_ret > 0)
                y = sec_pos3;  
            else
                y = sec_pos1;  
        }else
        {
            if(title_ret > 0)
                y = sec_pos2;
            else
                y = sec_pos0;     
        }

        int ctx_ret = GetAzkarContent(i);
        if(ctx_ret > 0)
        {
            widget_textarea_para.width = 340;
            widget_textarea_para.height = Container_H - y;
            widget_textarea_para.font = &font_common_30_no_ch;
            widget_textarea_para.color = lv_color_hex(0xffffff);
            if(lang == AzkarTxtLangAr)
                widget_textarea_para.align = LV_TEXT_ALIGN_RIGHT;
            else
                widget_textarea_para.align = LV_TEXT_ALIGN_LEFT;
            widget_textarea_para.bg_opa_x = LV_OPA_0;
            widget_textarea_para.bg_color = lv_color_hex(0x000000);
            widget_textarea_para.border_width = 0;
            widget_textarea_para.border_opa_x = LV_OPA_0;
            widget_textarea_para.border_color = lv_color_hex(0x000000);
            widget_textarea_para.parent = elem_container[i];
            widget_textarea_para.txt = GetpContent();
            lv_obj_t *content_label = common_widget_textarea_create(&widget_textarea_para);
            lv_obj_align(content_label, LV_ALIGN_TOP_MID, 0, y);
            lv_obj_set_style_pad_top(content_label, 0, LV_PART_MAIN);
            lv_obj_set_style_pad_bottom(content_label, 40, LV_PART_MAIN);
            lv_obj_add_flag(content_label, LV_OBJ_FLAG_EVENT_BUBBLE);
        }else
        { 
            lv_obj_t *img_container = \
                img_container_create(i, 0, y, Container_W, Container_H - y);
 
            for(uint8_t i = 0; i < 3; i++)
            {
                widget_img_para.img_x = 14;
                widget_img_para.img_y = 0 + i*446;
                widget_img_para.img_parent = img_container;
                widget_img_para.file_img_dat = azkar_sec_00_index + i;
                widget_img_para.img_click_attr = false;
                widget_img_para.event_cb = NULL;
                common_widget_img_create(&widget_img_para, NULL);
            }
        }
    }
    
    return;
}

static void tr_icon_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t TxtLang = GetAzkarTxtLang();
    if(TxtLang == AzkarTxtLangEn)
        TxtLang = AzkarTxtLangTr;
    else if(TxtLang == AzkarTxtLangTr)
        TxtLang = AzkarTxtLangEn;
    SetAzkarTxtLang(TxtLang);

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void elem_tr_icon_create(uint8_t menu_align, int8_t sidx, int8_t eidx)
{
    uint8_t lang = GetAzkarTxtLang();

    for(int8_t i = sidx; i < eidx; i++)
    {
        widget_img_para.img_parent = \
            elem_container[i];
        if(lang == AzkarTxtLangEn)
            widget_img_para.file_img_dat = azkar_tr_00_index;
        else
            widget_img_para.file_img_dat = azkar_tr_01_index;
        widget_img_para.img_click_attr = true;
        widget_img_para.event_cb = tr_icon_cb;
        widget_img_para.user_data = NULL;
        lv_obj_t *tr_icon = common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(tr_icon, LV_ALIGN_BOTTOM_RIGHT, -40, -30);
        lv_obj_set_ext_click_area(tr_icon, 30);
        lv_obj_add_flag(tr_icon, LV_OBJ_FLAG_EVENT_BUBBLE);
        if(lang == AzkarTxtLangAr)
            lv_obj_add_flag(tr_icon, LV_OBJ_FLAG_HIDDEN);
    }

    return;
}

static void elem_obj_dy_create(void)
{
    uint8_t SecNum = GetAzkarSecNum();
    int16_t scroll = scroll_offset + scroll_dela;
    int8_t idx = LV_ABS(scroll/Container_W); 
    int8_t sidx = idx - load_range;
    int8_t eidx = idx + load_range;
    sidx = sidx<0?0:sidx;
    eidx = eidx>SecNum?SecNum:eidx;

    if(sidx < load_sidx)
    {
        //不在范围内，新的创建
        menu_align_t menu_align = \
            menu_align_left;
        if(lang_txt_is_arabic())
            menu_align = menu_align_right;

        printf("sidx = %d, load_sidx = %d\n", sidx, load_sidx);
        elem_ctx_create(menu_align, sidx, load_sidx);
        elem_tr_icon_create(menu_align, sidx, load_sidx);
        load_sidx = sidx;
    }

    if(eidx > load_eidx)
    {
        //不在范围内，新的创建
        menu_align_t menu_align = \
            menu_align_left;
        if(lang_txt_is_arabic())
            menu_align = menu_align_right;

        printf("load_eidx = %d, eidx = %d\n", load_eidx, eidx);
        elem_ctx_create(menu_align, load_eidx, eidx);
        elem_tr_icon_create(menu_align, load_eidx, eidx);
        load_eidx = eidx;
    }

    return;
}

static void pressed_cb(lv_event_t *e)
{
    scroll_dela = 0;

    hor_scroll = false;

    lv_indev_t *indev_act = lv_indev_get_act();

    lv_indev_get_point(indev_act, &start_point);

    memcpy(&now_point, &start_point, sizeof(lv_point_t));
    memcpy(&min_point, &start_point, sizeof(lv_point_t));
    memcpy(&max_point, &start_point, sizeof(lv_point_t));

    return;
}

static void pressing_cb(lv_event_t *e)
{
    bool anim_progress = \
        anim_progress_with_var(list_ctx_container);
    if(anim_progress == true)
        return;

    if(!hor_scroll)
    {
        hor_scroll_judge();

        if(hor_scroll)
            del_anim_with_var(list_ctx_container);
    }

    return;
}

static void release_cb(lv_event_t *e)
{
    if(!hor_scroll)
        return;

    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    uint8_t SecNum = GetAzkarSecNum();

    lv_point_t throw;
    lv_indev_t *indev_act = lv_indev_get_act();
    lv_indev_get_throw(indev_act, &throw);

    int16_t scroll_abs_min;
    int16_t scroll_abs_max;
    if(menu_align == menu_align_right)
    {
        scroll_abs_min = 0;
        scroll_abs_max = (SecNum - 1)*Container_W;
    }else
    {
        scroll_abs_min = 0;
        scroll_abs_max = (-1)*(SecNum - 1)*Container_W;
    }

    scroll_dela = 0;

    int16_t throw_start;
    int16_t throw_end;
    throw_start = scroll_offset;
    if(LV_ABS(throw.x) >= 10)
    {
        if(throw.x < 0)   
            throw_end = scroll_offset - Container_W;
        else
            throw_end = scroll_offset + Container_W;
    }else
        return;

    if(menu_align == menu_align_right)
    {
        if(throw_end > scroll_abs_max || throw_end < scroll_abs_min)
            return;
    }else
    {
        if(throw_end > scroll_abs_min || throw_end < scroll_abs_max)
            return;
    }

    uint32_t anim_duration = 200;

    lv_anim_t throw_anim;

    widget_anim_para.anim = &throw_anim;
    widget_anim_para.anim_obj = list_ctx_container;
    widget_anim_para.anim_exec_xcb = throw_anim_cb; 
    widget_anim_para.anim_duration = anim_duration;
    widget_anim_para.anim_start_val = throw_start;
    widget_anim_para.anim_end_val = throw_end;
    widget_anim_para.is_start_anim = false;
    common_widget_anim_create(&widget_anim_para);
    lv_anim_set_path_cb(widget_anim_para.anim, \
        lv_anim_path_ease_out);
    lv_anim_set_ready_cb(widget_anim_para.anim, \
        throw_ready_cb);
    lv_anim_start(widget_anim_para.anim);

    return;
}

static void __this_event_cb(lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        pressed_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        pressing_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        release_cb(e);

    return;
}

static void layout_create(void)
{
    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    elem_container_create(menu_align);

    uint8_t SecNum = GetAzkarSecNum();
    int16_t scroll = scroll_offset + scroll_dela;
    int8_t idx = LV_ABS(scroll)/Container_W;
    load_sidx = idx - load_range;
    load_eidx = idx + load_range;
    load_sidx = load_sidx<0?0:load_sidx;
    load_eidx = load_eidx>SecNum?SecNum:load_eidx;

    elem_ctx_create(menu_align, load_sidx, load_eidx);

    elem_tr_icon_create(menu_align, load_sidx, load_eidx);

    elem_container_scroll();

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    SetUsrWaitTe(0);

    ui_act_id_t prev_act_id = \
        ui_act_id_azkar_list;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_azkar_sec);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_azkar_sec);

    uint8_t SecIdx = GetAzkarSecIdx();
    lv_obj_t *tileview_obj = tileview_obj_container();
    if(SecIdx == 0)
        lv_obj_add_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);
    else
        lv_obj_clear_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    SetUsrWaitTe(1);

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

    for(uint8_t i = 0; i < Elem_Num; i++)
        elem_container[i] = NULL;

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

register_ui_menu_load_info(\
    menu_load_azkar_sec) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .disable_te = true,
    .menu_id = \
        ui_act_id_azkar_sec,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
