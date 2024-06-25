#include "scrollbar.h"

/****************通用滚动条参数内容****************/
static common_scrollbar_ctx_t \
    common_scrollbar_ctx = {0};


static void del_anim_with_var(void *var)
{
    if(!var) return;

    if(lv_anim_get(var, NULL))
        lv_anim_del(var, NULL);
        
    return;
}

static void scrollbar_hidden_anim_cb(void *var, int32_t val)
{
    val = val<LV_OPA_0?LV_OPA_0:val;
    val = val>LV_OPA_100?LV_OPA_100:val;
    
    lv_obj_t *scrollbar_0_obj = \
        common_scrollbar_ctx.scrollbar_0_obj;
    lv_obj_t *scrollbar_1_obj = \
        common_scrollbar_ctx.scrollbar_1_obj;
    lv_obj_set_style_bg_opa(scrollbar_0_obj, val, \
        LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scrollbar_1_obj, val, \
        LV_PART_MAIN);

    if(val <= LV_OPA_0)
        lv_obj_add_flag(scrollbar_0_obj, \
            LV_OBJ_FLAG_HIDDEN);

    return;
}

static void scrollbar_hidden_anim_add(lv_obj_t *obj)
{
    if(!obj) return;

    lv_anim_t hidden_anim;
    widget_anim_para.anim = &hidden_anim;
    widget_anim_para.anim_obj = obj;
    widget_anim_para.anim_exec_xcb = \
        scrollbar_hidden_anim_cb;
    widget_anim_para.anim_duration = 500;
    widget_anim_para.anim_start_val = \
        LV_OPA_100;
    widget_anim_para.anim_end_val = \
        LV_OPA_0;
    widget_anim_para.is_start_anim = true;
    common_widget_anim_create(&widget_anim_para);

    return;
}

void common_scrollbar_destroy(void)
{
    memset(&common_scrollbar_ctx, 0, \
        sizeof(common_scrollbar_ctx_t));

    return;
}

void common_scrollbar_create(lv_obj_t *obj, \
    int16_t scroll_offset, int16_t scroll_total, \
        menu_align_t menu_align)
{
    if(!obj) return;

    if(scroll_total == 0) 
        return;

    uint16_t scrollbar_0_w = 12;
    uint16_t scrollbar_0_h = 72;
    lv_obj_t **scrollbar_0_obj = \
        &common_scrollbar_ctx.scrollbar_0_obj;
    
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = 0;
    widget_obj_para.obj_width = \
        scrollbar_0_w;
    widget_obj_para.obj_height = \
        scrollbar_0_h;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x545454);
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_is_scrollable = false;
    widget_obj_para.obj_radius = \
        scrollbar_0_w/2;
    *scrollbar_0_obj = \
        common_widget_obj_create(&widget_obj_para);
    if(menu_align == menu_align_right)
        lv_obj_align(*scrollbar_0_obj, LV_ALIGN_TOP_LEFT, \
            40, 50);
    else
        lv_obj_align(*scrollbar_0_obj, LV_ALIGN_TOP_RIGHT, \
            -40, 50);

    uint16_t scrollbar_1_w = \
        scrollbar_0_w;
    uint16_t scrollbar_1_h = \
        scrollbar_0_w;
    int16_t *scrollbar_total = \
        &common_scrollbar_ctx.scrollbar_total;
    lv_obj_t **scrollbar_1_obj = \
        &common_scrollbar_ctx.scrollbar_1_obj;

    *scrollbar_total = scroll_total;
    int16_t obj_1_y = \
        (scroll_offset*(scrollbar_0_h - scrollbar_1_h))/ \
            (*scrollbar_total);

    widget_obj_para.obj_parent = \
        *scrollbar_0_obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        obj_1_y;
    widget_obj_para.obj_width = \
        scrollbar_1_w;
    widget_obj_para.obj_height = \
        scrollbar_1_h;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0xffffff);
    widget_obj_para.obj_radius = \
        scrollbar_1_w/2;
    *scrollbar_1_obj = \
        common_widget_obj_create(&widget_obj_para);

    lv_obj_add_flag(scrollbar_0_obj, \
        LV_OBJ_FLAG_HIDDEN);

    bool *scrollbar_add = \
        &common_scrollbar_ctx.scrollbar_add;
    *scrollbar_add = true;

    return;
}

void common_scrollbar_press_handle(int16_t scroll_offset)
{
    bool scrollbar_add = \
        common_scrollbar_ctx.scrollbar_add;
    if(!scrollbar_add)
        return;

    lv_obj_t *scrollbar_0_obj = \
        common_scrollbar_ctx.scrollbar_0_obj;
    lv_obj_t *scrollbar_1_obj = \
        common_scrollbar_ctx.scrollbar_1_obj;

    if(lv_obj_has_flag(scrollbar_0_obj, \
        LV_OBJ_FLAG_HIDDEN))
        lv_obj_clear_flag(scrollbar_0_obj, \
            LV_OBJ_FLAG_HIDDEN);

    lv_obj_set_style_bg_opa(scrollbar_0_obj, \
        LV_OPA_100, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scrollbar_1_obj, \
        LV_OPA_100, LV_PART_MAIN);

    del_anim_with_var(scrollbar_0_obj);

    int16_t scrollbar_total = \
        common_scrollbar_ctx.scrollbar_total;
    
    int16_t scrollbar_0_h = \
        lv_obj_get_height(scrollbar_0_obj);
    int16_t scrollbar_1_h = \
        lv_obj_get_height(scrollbar_1_obj);

    int16_t obj_1_y = \
        (scroll_offset*(scrollbar_0_h - scrollbar_1_h))/ \
            (scrollbar_total);

    obj_1_y = \
        obj_1_y<0?0:obj_1_y;
    obj_1_y = \
        obj_1_y>(scrollbar_0_h - scrollbar_1_h)?\
            (scrollbar_0_h - scrollbar_1_h):obj_1_y;
    
    lv_obj_set_y(scrollbar_1_obj, obj_1_y);

    return;
}

void common_scrollbar_released_handle(void)
{
    bool scrollbar_add = \
        common_scrollbar_ctx.scrollbar_add;
    if(!scrollbar_add)
        return;

    lv_obj_t *scrollbar_0_obj = \
        common_scrollbar_ctx.scrollbar_0_obj;       
    scrollbar_hidden_anim_add(scrollbar_0_obj);

    return;
}