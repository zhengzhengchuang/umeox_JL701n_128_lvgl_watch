#include "alarm_main.h"
#include "alarm_time.h"
#include "alarm_repeat.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *list_ctx_container;

static uint8_t alarm_repeat_tmp;

static const uint16_t ec_h = 96;
static const uint8_t repeat_idx[7] = {
    0, 1, 2, 3, 4, 5, 6,
};

uint8_t GetAlarmRepeatTmp(void)
{
    return alarm_repeat_tmp;
}

void SetAlarmRepeatTmp(uint8_t repeat)
{
    alarm_repeat_tmp = repeat;
    scroll_y = 0;

    return;
}

static void alarm_set_confirm_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t id = \
        GetAlarmEditId();
    uint8_t alarm_num = \
        Alarm_Info.alarm_num;
    uint8_t op_mode = \
        GetAlarmOpMode();

    uint8_t alarm_hour;
    uint8_t alarm_minute;
    GetAlarmTimeTmp(&alarm_hour, \
        &alarm_minute);

    alarm_union_t alarm_union_tmp;
    alarm_union_tmp.bit_field.alarm_hour = \
        alarm_hour;
    alarm_union_tmp.bit_field.alarm_minute = \
        alarm_minute;
    alarm_union_tmp.bit_field.alarm_repeat = \
        alarm_repeat_tmp;

    if(op_mode == Op_Alarm_Add)
    {
        if(alarm_num >= Alarm_Max_Num)
            goto __end;

        alarm_union_tmp.bit_field.alarm_id = alarm_num;
        alarm_union_tmp.bit_field.alarm_enable = true;
        UserAlarmAdd(alarm_union_tmp.info);
    }else if(op_mode == Op_Alarm_Modify)
    {
        if(id >= alarm_num)
            goto __end;

        alarm_union_tmp.bit_field.alarm_id = id;
        UserAlarmTimeModify(alarm_union_tmp.info);
        UserAlarmRepeatModify(alarm_union_tmp.info);
    }

__end:  
    ui_menu_jump(ui_act_id_alarm_main);

    return;
}

static void elem_container_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = \
        *(uint8_t *)lv_event_get_user_data(e);

    if(alarm_repeat_tmp & (0x01 << idx))
        alarm_repeat_tmp &= ~(0x01 << idx);
    else
        alarm_repeat_tmp |= (0x01 << idx);

    ui_act_id_t cur_act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(cur_act_id);

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
    lv_obj_add_event_cb(list_ctx_container, \
        scroll_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_null;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_alarm_repeat);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_alarm_repeat);

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

    uint8_t i;
    for(i = 0; i < 7; i++)
    {
        widget_img_para.img_parent = \
            list_ctx_container;
        if(alarm_repeat_tmp & (0x01 << i))
            widget_img_para.file_img_dat = \
                comm_icon_06_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_05_index;
        widget_img_para.img_click_attr = \
            true;
        widget_img_para.event_cb = \
            elem_container_cb;
        widget_img_para.user_data = \
            (void *)&repeat_idx[i];
        lv_obj_t *elem_container = \
            common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(elem_container, LV_ALIGN_TOP_MID, 0, 32 + i*ec_h);

        widget_label_para.label_parent = \
            elem_container;
        widget_label_para.label_w = \
            180;
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
        lv_obj_t *week_label = \
            common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align(week_label, LV_ALIGN_RIGHT_MID, -25, 0);
        else
            lv_obj_align(week_label, LV_ALIGN_LEFT_MID, 25, 0);

        widget_img_para.img_parent = \
            elem_container;
        if(alarm_repeat_tmp & (0x01 << i))
            widget_img_para.file_img_dat = \
                comm_icon_17_index;
        else
            widget_img_para.file_img_dat = \
                comm_icon_16_index;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = \
            NULL;
        lv_obj_t *sel_icon = \
            common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(sel_icon, LV_ALIGN_LEFT_MID, 25, 0);
        else
            lv_obj_align(sel_icon, LV_ALIGN_RIGHT_MID, -25, 0);
    }

    widget_img_para.img_parent = \
        list_ctx_container;
    widget_img_para.file_img_dat = \
        comm_icon_02_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        alarm_set_confirm_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *alarm_set_confirm = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(alarm_set_confirm, LV_ALIGN_TOP_MID, \
        0, 32 + i*ec_h + 50);
    lv_obj_set_ext_click_area(alarm_set_confirm, 20);

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
    menu_load_alarm_repeat) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_alarm_repeat,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
