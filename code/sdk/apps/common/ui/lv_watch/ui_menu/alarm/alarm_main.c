#include "alarm_main.h"
#include "alarm_time.h"
#include "alarm_repeat.h"

static int16_t scroll_y = 0;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *alarm_main_container;

static const uint16_t ec_h = 96;
static const uint8_t alarm_idx[Alarm_Max_Num] = {
    0, 1, 2, 3, 4,
};

static uint8_t alarm_edit_id;
uint8_t GetAlarmEditId(void)
{
    return alarm_edit_id;
}

void SetAlarmEditId(uint8_t id)
{
    alarm_edit_id = id;

    return;
}

static uint8_t alarm_op_mode;
uint8_t GetAlarmOpMode(void)
{
    return alarm_op_mode;
}

void SetAlarmOpMode(uint8_t mode)
{
    alarm_op_mode = mode;

    return;
}

static void no_alarm_add_cb(lv_event_t *e)
{
    if(!e) return;

    struct sys_time utc_time;
    GetUtcTime(&utc_time);
    SetAlarmTimeTmp(utc_time.hour, utc_time.min);
    SetAlarmRepeatTmp(0x00);

    SetAlarmOpMode(Op_Alarm_Add);

    ui_menu_jump(ui_act_id_alarm_time);

    return;
}

static void no_alarm_menu_create(lv_obj_t *obj, menu_align_t menu_align)
{
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = alarm_00_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *no_alarm_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(no_alarm_icon, LV_ALIGN_TOP_MID, 0, 88);

    widget_img_para.file_img_dat = comm_icon_11_index;
    lv_obj_t *comm_11_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 266);

    widget_img_para.file_img_dat = comm_icon_18_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = no_alarm_add_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *alarm_add_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(alarm_add_icon, LV_ALIGN_TOP_MID, 0, 376);
    lv_obj_set_ext_click_area(alarm_add_icon, 20);

    return;
}

static void has_alarm_add_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t alarm_num = Alarm_Info.alarm_num;
    if(alarm_num >= Alarm_Max_Num)
        return;

    struct sys_time utc_time;
    GetUtcTime(&utc_time);
    SetAlarmTimeTmp(utc_time.hour, utc_time.min);
    SetAlarmRepeatTmp(0x00);

    SetAlarmOpMode(Op_Alarm_Add);

    ui_menu_jump(ui_act_id_alarm_time);

    return;
}

static void has_alarm_modify_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = *(uint8_t *)lv_event_get_user_data(e);
    uint8_t alarm_num = Alarm_Info.alarm_num;
    if(idx >= alarm_num)
        return;

    SetAlarmEditId(idx);
    SetAlarmOpMode(Op_Alarm_Modify);

    ui_menu_jump(ui_act_id_alarm_modify);

    return;
}

static void has_alarm_enable_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t idx = *(uint8_t *)lv_event_get_user_data(e);
    alarm_union_t alarm_union_tmp;
    alarm_union_tmp.info = Alarm_Info.alarm_union[idx].info;
    if(alarm_union_tmp.bit_field.alarm_enable)
        alarm_union_tmp.bit_field.alarm_enable = 0;
    else
        alarm_union_tmp.bit_field.alarm_enable = 1;
    UserAlarmEnableModify(alarm_union_tmp.info);

    ui_act_id_t cur_act_id = p_ui_info_cache->cur_act_id;
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

static void alarm_main_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = LCD_UI_Y_OFFSET;
    widget_obj_para.obj_width = LCD_WIDTH;
    widget_obj_para.obj_height = LCD_HEIGHT - LCD_UI_Y_OFFSET;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = true;
    alarm_main_container = common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(alarm_main_container, scroll_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void has_alarm_menu_create(lv_obj_t *obj, \
    uint8_t alarm_num, menu_align_t menu_align)
{
    if(alarm_num > Alarm_Max_Num)
        alarm_num = Alarm_Max_Num;

    alarm_main_container_create(obj);

    int time_format = GetVmParaCacheByLabel(vm_label_time_format);

    uint8_t i;
    uint8_t am_or_pm;
    uint8_t alarm_enable;
    uint8_t alarm_repeat;
    char alarm_time_str[6];
    uint8_t alarm_time_hour;
    uint8_t alarm_time_minute;
    int16_t elem_container_sy = 32;
    for(i = 0; i < alarm_num; i++)
    {
        widget_img_para.img_parent = alarm_main_container;
        widget_img_para.file_img_dat = comm_icon_05_index;
        widget_img_para.img_click_attr = true;
        widget_img_para.event_cb = has_alarm_modify_cb;
        widget_img_para.user_data = (void *)&alarm_idx[i];
        lv_obj_t *elem_container = common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(elem_container, LV_ALIGN_TOP_MID, 0, elem_container_sy + i*ec_h);

        /************闹钟时间num************/
        memset(alarm_time_str, 0, sizeof(alarm_time_str));
        alarm_time_hour = Alarm_Info.alarm_union[i].bit_field.alarm_hour;
        alarm_time_minute = Alarm_Info.alarm_union[i].bit_field.alarm_minute;
        if(time_format == time_format_12)
        {
            if(alarm_time_hour >= 12)
                am_or_pm = 1;
            else
                am_or_pm = 0;

            if(alarm_time_hour > 12)
                alarm_time_hour -= 12;
            else if(alarm_time_hour == 0)
                alarm_time_hour = 12;
        }
        sprintf(alarm_time_str, "%02d:%02d", alarm_time_hour, alarm_time_minute);

        widget_label_para.label_parent = elem_container;
        widget_label_para.label_w = 70;
        widget_label_para.label_h = Label_Line_Height;
        widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
        if(menu_align == menu_align_right)
            widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
        else
            widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
        widget_label_para.label_text = alarm_time_str;
        widget_label_para.label_text_color = lv_color_hex(0xF0D990);
        widget_label_para.label_ver_center = false;
        widget_label_para.user_text_font = &font_common_num_32;
        lv_obj_t *alarm_time_label = common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align(alarm_time_label, LV_ALIGN_TOP_RIGHT, -25, 10);
        else
            lv_obj_align(alarm_time_label, LV_ALIGN_TOP_LEFT, 25, 10);
        
        widget_img_para.img_parent = elem_container;
        widget_img_para.file_img_dat = comm_icon_22_index + am_or_pm;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        lv_obj_t *ampm_icon = common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align_to(ampm_icon, alarm_time_label, LV_ALIGN_OUT_LEFT_MID, 0, 0);
        else
            lv_obj_align_to(ampm_icon, alarm_time_label, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        if(time_format == time_format_24)
            lv_obj_add_flag(ampm_icon, LV_OBJ_FLAG_HIDDEN);

        alarm_enable = Alarm_Info.alarm_union[i].bit_field.alarm_enable;
        widget_img_para.img_parent = elem_container;
        widget_img_para.file_img_dat = comm_icon_00_index + alarm_enable;
        widget_img_para.img_click_attr = true;
        widget_img_para.event_cb = has_alarm_enable_cb;
        widget_img_para.user_data = (void *)&alarm_idx[i];
        lv_obj_t *alarm_enable_sw = common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(alarm_enable_sw, LV_ALIGN_LEFT_MID, 16, 0);
        else
            lv_obj_align(alarm_enable_sw, LV_ALIGN_RIGHT_MID, -16, 0);
        lv_obj_set_ext_click_area(alarm_enable_sw, 16);

        alarm_repeat = Alarm_Info.alarm_union[i].bit_field.alarm_repeat;
        widget_img_para.img_parent = elem_container;
        widget_img_para.img_y = 52;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        
        int16_t total_width = 0;
        lv_obj_t *alarm_repeat_sw;
        for(uint8_t j = 0; j < 7; j++)
        {
            if(menu_align == menu_align_right)
            {
                widget_img_para.img_x = 296 - (j + 1)*24;
                
                if(alarm_repeat & (0x01 << j))
                    widget_img_para.file_img_dat = alarm_repeat_00_index + (j*2) + 1;
                else
                   widget_img_para.file_img_dat = alarm_repeat_00_index + (j*2) + 0;
                alarm_repeat_sw = common_widget_img_create(&widget_img_para, NULL);
            }else
            {
                widget_img_para.img_x = 24 + total_width;
                if(alarm_repeat & (0x01 << j))
                    widget_img_para.file_img_dat = alarm_repeat_00_index + (j*2) + 1;
                else
                   widget_img_para.file_img_dat = alarm_repeat_00_index + (j*2) + 0;
                 
                alarm_repeat_sw = common_widget_img_create(&widget_img_para, NULL);
                    
                total_width += 24;
            }
        }
    }

    if(alarm_num < Alarm_Max_Num)
    {
        widget_img_para.img_parent = alarm_main_container;
        widget_img_para.file_img_dat = comm_icon_18_index;
        widget_img_para.img_click_attr = true;
        widget_img_para.event_cb = has_alarm_add_cb;
        widget_img_para.user_data = NULL;
        lv_obj_t *alarm_add_icon = common_widget_img_create(&widget_img_para, NULL);
        lv_obj_align(alarm_add_icon, LV_ALIGN_TOP_MID, 0, elem_container_sy + i*ec_h + 52);
        lv_obj_set_ext_click_area(alarm_add_icon, 20);
    }else
    {
        lv_obj_set_style_pad_bottom(alarm_main_container, 25, LV_PART_MAIN);
    }

    lv_obj_scroll_to_y(alarm_main_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(alarm_main_container);
    scroll_top_y = lv_obj_get_scroll_top(alarm_main_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(alarm_main_container);
    scroll_y = scroll_y<scroll_top_y?scroll_top_y:scroll_y;
    scroll_y = scroll_y>scroll_bottom_y?scroll_bottom_y:scroll_y;
    lv_obj_scroll_to_y(alarm_main_container, scroll_y, LV_ANIM_OFF);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = ui_act_id_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_alarm_main);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_alarm_main);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bool Update = GetAlarmRemoteUpdate();
    if(Update == false) return;
    SetAlarmRemoteUpdate(false);

    ui_act_id_t act_id = p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    SetAlarmRemoteUpdate(false);

    menu_align_t menu_align = menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    uint8_t alarm_num = Alarm_Info.alarm_num;
    if(alarm_num == 0)
        no_alarm_menu_create(obj, menu_align);
    else
        has_alarm_menu_create(obj, alarm_num, menu_align);

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
    uint8_t alarm_num = Alarm_Info.alarm_num;
    if(alarm_num == 0)
        return;

    if(lv_anim_get(alarm_main_container, NULL))
        return;

    int16_t scroll_y_tmp = scroll_y;

    if(state == Rdec_Forward)
        scroll_y_tmp += ec_h;
    else if(state == Rdec_Backward)
        scroll_y_tmp -= ec_h;
    else
        return;

    scroll_y_tmp = scroll_y_tmp < scroll_top_y?scroll_top_y:scroll_y_tmp;
    scroll_y_tmp = scroll_y_tmp > scroll_bottom_y?scroll_bottom_y:scroll_y_tmp;
    if(scroll_y_tmp != scroll_y)
        lv_obj_scroll_to_y(alarm_main_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_alarm_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_alarm_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
