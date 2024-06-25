#include "call_dial.h"

static const lv_point_t button_crd[12] = {
    {24, 352}, \
    {24, 100}, {137, 100}, {250, 100}, \
    {24, 184}, {137, 184}, {250, 184}, \
    {24, 268}, {137, 268}, {250, 268}, \
    {137, 352}, {250, 352},
};

static const uint8_t button_idx[12] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
};

#define Num_Max (15)
static uint8_t num_len;
char num_str[Num_Max + 1];

static lv_obj_t *num_label;


static void num_click_handle(uint8_t num)
{
    if(num_len >= Num_Max)
        return;

    if(!num_label)
        return;

    num_str[num_len] = num + '0';

    lv_label_set_text(num_label, \
        num_str);
  
    num_len += 1;

    return;
}

/*********************************************************************************
                                  拨号盘回退点击处理                                 
*********************************************************************************/
static void back_click_handle(void)
{
    if(!num_len) return;

    if(!num_label) return;

    num_str[num_len - 1] = '\0';

    lv_label_set_text(num_label, \
        num_str);
  
    num_len -= 1;

    return;
}

static void dial_click_handle(void)
{
    if(!num_len) return;

    CtrlCallOutByNum(num_str, \
        num_len);
    
    return;
}

static void click_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    if(idx >= 0 && idx <= 9)
        num_click_handle(idx);
    else if(idx == 10)
        back_click_handle();
    else if(idx == 11)
        dial_click_handle();

    return;
}

static lv_timer_t *lpb_timer = NULL;

static void lpb_timer_cb(lv_timer_t *timer)
{
    if(!lpb_timer)
        return;

    if(!num_len)
    {
        lv_timer_del(lpb_timer);
        lpb_timer = NULL;
        return;
    }

    back_click_handle();

    return;
}

static void lpb_pressed_cb(lv_event_t *e)
{
    if(!e) return;

    if(lpb_timer)
    {
        lv_timer_del(lpb_timer);
        lpb_timer = NULL;
    }

    lpb_timer = \
        lv_timer_create(lpb_timer_cb, 150, NULL);

    return;
}

static void lpb_release_cb(lv_event_t *e)
{
    if(!e) return;

    if(lpb_timer)
    {
        lv_timer_del(lpb_timer);
        lpb_timer = NULL;
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_call_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_call_dial);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_call_dial);

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

    
    uint8_t i = 0;

    for(i = 0; i < 10; i++)
    {
        widget_img_para.img_x = \
            button_crd[i].x;
        widget_img_para.img_y = \
            button_crd[i].y;
        widget_img_para.img_parent = \
            obj;
        widget_img_para.file_img_dat = \
            call_08_index;
        widget_img_para.img_click_attr = \
            true;
        widget_img_para.event_cb = \
            click_cb;
        widget_img_para.user_data = \
            (void *)&button_idx[i];
        lv_obj_t *button_container = \
            common_widget_img_create(&widget_img_para, \
                NULL);
        lv_obj_set_ext_click_area(button_container, 5);

        widget_img_para.img_parent = \
            button_container;
        widget_img_para.file_img_dat = \
            comm_num_20x32_wh_00_index + i;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = \
            NULL;
        lv_obj_t *button_num = \
            common_widget_img_create(&widget_img_para, \
                NULL);
        lv_obj_center(button_num);
    }

    uint32_t spec_button_file[2] = { 
        call_09_index, call_10_index,
    };

    for(; i < 12; i++)
    {
        widget_img_para.img_x = \
            button_crd[i].x;
        widget_img_para.img_y = \
            button_crd[i].y;
        widget_img_para.img_parent = \
            obj;
        widget_img_para.file_img_dat = \
            spec_button_file[i - 10];
        widget_img_para.img_click_attr = \
            true;
        widget_img_para.event_cb = \
            click_cb;
        widget_img_para.user_data = \
            (void *)&button_idx[i];
        lv_obj_t *button_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_set_ext_click_area(button_container, 5);

        if(i == 10)
        {
            lv_obj_add_event_cb(button_container, \
                lpb_pressed_cb, LV_EVENT_LONG_PRESSED, \
                    NULL);

            lv_obj_add_event_cb(button_container, \
                lpb_release_cb, LV_EVENT_RELEASED, \
                    NULL);      
        }
            
    }
  
    num_len = 0;
    num_label = NULL;
    memset(num_str, 0, \
        sizeof(num_str));

    widget_label_para.label_w = \
        (300);
    widget_label_para.label_h = \
        42;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        &font_common_num_36;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        num_str;
    num_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(num_label, LV_ALIGN_TOP_MID, \
        0, 44);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_call_dial) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_call_dial,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
