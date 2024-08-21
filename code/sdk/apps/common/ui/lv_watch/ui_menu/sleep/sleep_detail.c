#include "sleep_detail.h"

static bool slp_valid;

static const s16 ec_sy = 20;
static const u16 ec_h = 102;
static lv_obj_t *list_ctx_container;

static const u32 txt_src[slp_state_num] = {
    lang_txtid_deep_sleep, lang_txtid_light_sleep, \
    lang_txtid_Rems, lang_txtid_wake_up,
};

static void list_ctx_container_create(lv_obj_t *obj)
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
    list_ctx_container = common_widget_obj_create(&widget_obj_para);
 
    return;
}


static void elem_ctx_create(menu_align_t menu_align)
{
    bool valid = SlpPara.valid;
    slp_valid = valid;

    u8 state_h[slp_state_num];
    u8 state_m[slp_state_num];
    
    u32 deep_dur = SlpPara.d_slp_dur;
    u32 light_dur = SlpPara.l_slp_dur;
    u32 rem_dur = SlpPara.r_slp_dur;
    u32 wake_dur = SlpPara.wake_dur;

    if(valid == true)
    {
        state_h[0] = deep_dur/60;
        state_m[0] = deep_dur%60;
        state_h[1] = light_dur/60;
        state_m[1] = light_dur%60;
        state_h[2] = rem_dur/60;
        state_m[2] = rem_dur%60;
        state_h[3] = wake_dur/60;
        state_m[3] = wake_dur%60;
    }else
    {
        memset(state_h, 0, sizeof(state_h));
        memset(state_m, 0, sizeof(state_m));
    }
    
    const u32 label_color[slp_state_num] = {
        0x6A01FC, 0x9B7BE8, 0xDCC5FB, 0xF0D990, 
    };

    widget_obj_para.obj_parent = list_ctx_container;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_width = LCD_WIDTH;
    widget_obj_para.obj_height = ec_h;  
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;

    char state_h_str[3];
    char state_m_str[3];
    for(u8 i = 0; i < slp_state_num; i++)
    {
        widget_obj_para.obj_y = ec_sy + ec_h*i;
        lv_obj_t *ec_container = common_widget_obj_create(&widget_obj_para);

        widget_img_para.img_parent = ec_container;
        widget_img_para.file_img_dat = sleep_state_00_index + i;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        lv_obj_t *state_icon = common_widget_img_create(&widget_img_para, NULL);
        if(menu_align == menu_align_right)
            lv_obj_align(state_icon, LV_ALIGN_TOP_RIGHT, -40, Label_Line_Height/2);
        else
            lv_obj_align(state_icon, LV_ALIGN_TOP_LEFT, 40, Label_Line_Height/2);

        widget_label_para.label_w = 280;
        widget_label_para.label_h = Label_Line_Height;
        widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
        if(menu_align == menu_align_right)
            widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
        else
            widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
        widget_label_para.label_text_color = lv_color_hex(label_color[i]);
        widget_label_para.label_ver_center = false;
        widget_label_para.user_text_font = NULL;
        widget_label_para.label_parent = ec_container;
        widget_label_para.label_text = get_lang_txt_with_id(txt_src[i]);
        lv_obj_t *state_label = common_widget_label_create(&widget_label_para);
        if(menu_align == menu_align_right)
            lv_obj_align_to(state_label, state_icon, LV_ALIGN_OUT_LEFT_MID, -20, 0);
        else
            lv_obj_align_to(state_label, state_icon, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

        memset(state_h_str, 0, sizeof(state_h_str));
        sprintf(state_h_str, "%02d", state_h[i]);
        num_str_para.parent = ec_container;
        num_str_para.num_obj_x = 70;
        num_str_para.num_obj_y = 62;
        num_str_para.p_num_str = state_h_str;
        num_str_para.num_str_len = 2;
        num_str_para.num_obj = NULL;
        num_str_para.num_obj_max = 0;
        num_str_para.num_dsc_idx = NULL;
        num_str_para.file_00_index = comm_num_24x40_wh_00_index;
        int16_t hour_end_x = common_widget_num_str_create(&num_str_para);

        memset(state_m_str, 0, sizeof(state_m_str));
        sprintf(state_m_str, "%02d", state_m[i]);
        num_str_para.num_obj_x = 216;
        num_str_para.p_num_str = state_m_str;
        int16_t min_end_x = common_widget_num_str_create(&num_str_para);

        widget_label_para.label_x = hour_end_x + 2;
        widget_label_para.label_y = 64;
        widget_label_para.label_w = 96;
        widget_label_para.label_h = Label_Line_Height;
        widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
        widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
        widget_label_para.label_text_color = lv_color_hex(0x808080);
        widget_label_para.label_ver_center = false;
        widget_label_para.user_text_font = NULL;
        widget_label_para.label_parent = ec_container;
        widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_hours);
        common_widget_label_create(&widget_label_para);

        widget_label_para.label_x = min_end_x + 2;
        widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_mins);
        common_widget_label_create(&widget_label_para);
    }
    
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t up_act_id = ui_act_id_sleep_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_sleep_detail);
    else
        tileview_register_all_menu(obj, up_act_id, ui_act_id_null, \
            ui_act_id_null, ui_act_id_null, ui_act_id_sleep_detail);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bool valid = SlpPara.valid;
    if(valid != slp_valid)
    {
        slp_valid = valid;
        ui_menu_jump(p_ui_info_cache->cur_act_id);
    }

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    list_ctx_container_create(obj);

    elem_ctx_create(menu_align);

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
   
    return;
}

register_ui_menu_load_info(\
    menu_load_sleep_detail) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_sleep_detail,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 200,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
