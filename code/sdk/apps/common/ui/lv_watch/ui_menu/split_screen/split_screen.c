#include "split_screen.h"

#if UI_USE_SPLIT_SCREEN
/**************分屏锁定**************/
static bool split_screen_lock = false;

/**************分屏平移值**************/
static int16_t split_screen_translate = 0;
static int16_t split_screen_translate_tmp = 0;

/**************分屏动画进行**************/
static bool split_screen_anim_progress = false;

/**************分屏容器**************/
static lv_obj_t *split_screen_container = NULL;

/**************按压点相关**************/
static lv_point_t split_screen_min_point = {0};
static lv_point_t split_screen_max_point = {0};
static lv_point_t split_screen_now_point = {0};
static lv_point_t split_screen_start_point = {0};

static void get_cur_anim_progress(void)
{
    if(lv_anim_get(split_screen_container, NULL))
        split_screen_anim_progress = true;
    else
        split_screen_anim_progress = false;

    return;
}

static void split_screen_anim_cb(void *var, int32_t val)
{
    lv_obj_t *obj = (lv_obj_t *)var;

    if(!obj) return;

    split_screen_translate = val;

    lv_obj_set_style_translate_x(obj, val, LV_PART_MAIN);

    return;
}

static void split_screen_anim_ready_cb(lv_anim_t *anim)
{
    lv_obj_t *tileview_obj = tileview_obj_container();
    if(!tileview_obj) return;
        
    if(!split_screen_translate)
        lv_obj_add_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);    
    else
        lv_obj_clear_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);

    return;
}

static void split_screen_menu_pressed_cb(lv_event_t *e)
{
    get_cur_anim_progress();

    if(split_screen_anim_progress)
        return;

    split_screen_translate_tmp = 0;

    lv_indev_t *indev_act = lv_indev_get_act();

    lv_indev_get_point(indev_act, &split_screen_start_point);

    memcpy(&split_screen_min_point, &split_screen_start_point, \
        sizeof(lv_point_t));
    memcpy(&split_screen_max_point, &split_screen_start_point, \
        sizeof(lv_point_t));
    memcpy(&split_screen_now_point, &split_screen_start_point, \
        sizeof(lv_point_t));

    lv_obj_t *tileview_obj = tileview_obj_container();
    if(!tileview_obj) return;
    if(!lv_obj_has_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE))
        split_screen_lock = true;
    else
        split_screen_lock = false;

    return;
}

static void split_screen_menu_pressing_cb(lv_event_t *e)
{
    if(split_screen_anim_progress)
        return;

    lv_point_t vert_point;
    lv_indev_t *indev_act = lv_indev_get_act();

    lv_indev_get_vect(indev_act, &vert_point);
    lv_indev_get_point(indev_act, &split_screen_now_point);

    int16_t lcd_w = lv_disp_get_hor_res(NULL);

    if(split_screen_now_point.x > split_screen_max_point.x)
        split_screen_max_point.x = split_screen_now_point.x;

    if(split_screen_now_point.x < split_screen_min_point.x)
        split_screen_min_point.x = split_screen_now_point.x;

    if(split_screen_now_point.y > split_screen_max_point.y)
        split_screen_max_point.y = split_screen_now_point.y;

    if(split_screen_now_point.y < split_screen_min_point.y)
        split_screen_min_point.y = split_screen_now_point.y;

    int16_t x_max_diff = split_screen_max_point.x - split_screen_min_point.x;
    int16_t y_max_diff = split_screen_max_point.y - split_screen_min_point.y;
    if(LV_ABS(x_max_diff) < LV_ABS(y_max_diff))
        return;
    
    split_screen_translate_tmp += vert_point.x;

    if(split_screen_translate_tmp > 0 && !split_screen_lock)
    {
        split_screen_lock = true;
        lv_obj_t *tileview_obj = tileview_obj_container();
        if(!tileview_obj) return;
        lv_obj_clear_flag(tileview_obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    if(!split_screen_lock) 
        return;
        
    split_screen_translate += vert_point.x;
    split_screen_translate = split_screen_translate < 0? \
        0:split_screen_translate;
    split_screen_translate = split_screen_translate > lcd_w/2? \
        lcd_w/2:split_screen_translate;

    if(split_screen_container)
        lv_obj_set_style_translate_x(split_screen_container, \
            split_screen_translate, LV_PART_MAIN);
 
    return;
}

static void split_screen_menu_released_cb(lv_event_t *e)
{
    if(split_screen_anim_progress)
        return;

    if(!split_screen_lock) return;

    lv_point_t vert_point = {0};
    lv_anim_t split_screen_anim;
    lv_indev_t *indev_act = lv_indev_get_act();

    lv_indev_get_vect(indev_act, &vert_point);

    int16_t lcd_w = lv_disp_get_hor_res(NULL);

    widget_anim_para.anim = &split_screen_anim;
    widget_anim_para.anim_obj = (void *)split_screen_container;
    widget_anim_para.anim_exec_xcb = split_screen_anim_cb;
    widget_anim_para.is_start_anim = false;
    widget_anim_para.anim_start_val = split_screen_translate;

    uint16_t anim_translate;
    if(vert_point.x > 0)
    {
        if(split_screen_translate >= lcd_w/2)
        {
            split_screen_anim_ready_cb(NULL);
            return;
        }
            
        widget_anim_para.anim_end_val = lcd_w/2;
        anim_translate = \
            LV_ABS(widget_anim_para.anim_end_val - \
                split_screen_translate);
    }else if(vert_point.x < 0)
    {
        if(split_screen_translate <= 0)
        {
            split_screen_anim_ready_cb(NULL);
            return;
        }
        
        widget_anim_para.anim_end_val = 0;
        anim_translate = \
            LV_ABS(widget_anim_para.anim_end_val - \
                split_screen_translate);
    }else
    {
        if(split_screen_translate >= lcd_w/4)
            widget_anim_para.anim_end_val = lcd_w/2;
        else
            widget_anim_para.anim_end_val = 0;

        anim_translate = \
            LV_ABS(widget_anim_para.anim_end_val - \
                split_screen_translate);
    }

    widget_anim_para.anim_duration = anim_translate*2;
    common_widget_anim_create(&widget_anim_para);
    lv_anim_set_ready_cb(widget_anim_para.anim, \
        split_screen_anim_ready_cb);
    lv_anim_start(widget_anim_para.anim);

    return;
}

static void split_screen_menu_event_cb(lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = \
        lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        split_screen_menu_pressed_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        split_screen_menu_pressing_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        split_screen_menu_released_cb(e);

    return;
}

void menu_split_screen_create(lv_obj_t *obj)
{
    if(!obj) return;

    split_screen_lock = false;
    split_screen_translate = 0;
    split_screen_translate_tmp = 0;

    int16_t lcd_w = lv_disp_get_hor_res(NULL);
    int16_t lcd_h = lv_disp_get_ver_res(NULL);

    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = -lcd_w/2;
    widget_obj_para.obj_y = 0;
    widget_obj_para.obj_width = lcd_w/2;
    widget_obj_para.obj_height = lcd_h;
    widget_obj_para.obj_bg_opax = LV_OPA_80;
    widget_obj_para.obj_bg_color = lv_color_hex(0x363636);
    widget_obj_para.obj_border_opax = LV_OPA_100;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    split_screen_container = common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(split_screen_container, split_screen_menu_event_cb, \
        LV_EVENT_ALL, NULL);

    lv_obj_t *split_screen_menu = tileview_get_center_menu();
    lv_obj_add_event_cb(split_screen_menu, split_screen_menu_event_cb, \
        LV_EVENT_ALL, NULL);

    return;
}
#endif
