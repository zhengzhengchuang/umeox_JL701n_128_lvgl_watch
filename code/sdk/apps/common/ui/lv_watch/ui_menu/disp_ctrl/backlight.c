#include "backlight.h"

static lv_obj_t *bl_slider;

//滑块增大倍数
static const uint8_t inc_mul = 3;

static void slider_anim_cb(void *var, int32_t val)
{
    lv_obj_t *obj = (lv_obj_t *)var;

    if(obj == NULL)
        return;

    lv_slider_set_value(obj, val, LV_ANIM_OFF);

    int bl_val = val/inc_mul;
    AppSetSysBacklight(bl_val);

    return;
}

static void slider_anim_create(int32_t s, int32_t e)
{
    lv_anim_t anim;

    int32_t inc_mul_s = s*inc_mul;
    int32_t inc_mul_e = e*inc_mul;
  
    uint32_t duration = \
        LV_ABS(inc_mul_e - inc_mul_s)*10;

    widget_anim_para.anim = &anim;
    widget_anim_para.anim_obj = bl_slider;
    widget_anim_para.anim_exec_xcb = slider_anim_cb;
    widget_anim_para.anim_duration = duration;
    widget_anim_para.anim_start_val = inc_mul_s;
    widget_anim_para.anim_end_val = inc_mul_e;
    widget_anim_para.is_start_anim = true;
    common_widget_anim_create(&widget_anim_para);

    return;
}

static void slider_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);
    int cur_val = \
        lv_slider_get_value(obj)/inc_mul;

    int min_val = \
        TCFG_BACKLIGHT_MIN_VAL;
    int max_val = \
        TCFG_BACKLIGHT_MAX_VAL;
    cur_val = \
        cur_val < min_val?min_val:cur_val;
    cur_val = \
        cur_val > max_val?max_val:cur_val;

    SetVmParaCacheByLabel(\
        vm_label_backlight, cur_val);
    
    AppSetSysBacklight(cur_val);
  
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = \
        p_ui_info_cache->ui_mode;
    ui_act_id_t prev_act_id = \
        ui_act_id_set_main;
    if(ui_mode == ui_mode_tool_box)
        prev_act_id = \
            ui_act_id_tool_box;
    ui_act_id_t scr_slp_act_id = \
        ui_act_id_screen_sleep;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, scr_slp_act_id, ui_act_id_backlight);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            scr_slp_act_id, prev_act_id, ui_act_id_backlight);

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

    int cur_val;
    int min_val; 
    int max_val;

    cur_val = \
        GetVmParaCacheByLabel(vm_label_backlight)*inc_mul;
    min_val = \
        TCFG_BACKLIGHT_MIN_VAL*inc_mul;
    max_val = \
        TCFG_BACKLIGHT_MAX_VAL*inc_mul;

    widget_slider_para.slider_parent = obj;
    widget_slider_para.slider_width = 82;
    widget_slider_para.slider_height = 284;
    widget_slider_para.slider_min_value = \ 
        min_val;
    widget_slider_para.slider_max_value = \
        max_val;
    widget_slider_para.slider_cur_value = \
        cur_val;
    widget_slider_para.slider_main_color = \
        lv_color_hex(0xB28146);
    widget_slider_para.slider_indic_color = \
        lv_color_hex(0xF0D990);
    widget_slider_para.slider_knob_opax = \
        LV_OPA_0;
    widget_slider_para.slider_knob_color = \
        lv_color_hex(0x000000);
    widget_slider_para.event_cb = \
        slider_cb;
    widget_slider_para.user_data = NULL;
    bl_slider = \
        common_widget_slider_create(&widget_slider_para);
    lv_obj_align(bl_slider, LV_ALIGN_TOP_MID, 0, 80);

    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_parent = \
        bl_slider;
    widget_img_para.file_img_dat = \
        disp_ctrl_00_index;
    widget_img_para.img_click_attr = \
        false;
    lv_obj_t *down_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(down_icon, LV_ALIGN_BOTTOM_MID, 0, -20);

    widget_img_para.file_img_dat = \
        disp_ctrl_01_index;
    lv_obj_t *up_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(up_icon, LV_ALIGN_TOP_MID, 0, 20);

    widget_img_para.img_x = 170;
    widget_img_para.img_y = 408;
    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_07_index;
    common_widget_img_create(&widget_img_para, NULL);

    widget_img_para.img_x = 190;
    widget_img_para.file_img_dat = \
        comm_icon_08_index;
    common_widget_img_create(&widget_img_para, NULL);

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
    if(lv_anim_get(bl_slider, NULL))
        return;

    int min_val = \
        TCFG_BACKLIGHT_MIN_VAL;
    int max_val = \
        TCFG_BACKLIGHT_MAX_VAL;
    int s_val = \
        GetVmParaCacheByLabel(vm_label_backlight);
    int e_val;

    if(state == Rdec_Forward)
    {
        if(s_val >= max_val)
            return;
        e_val = s_val + 7;
    }else if(state == Rdec_Backward)
    {
        if(s_val <= min_val)
            return;
        e_val = s_val - 7;
    }else
        return;

    e_val = e_val<min_val?min_val:e_val;
    e_val = e_val>max_val?max_val:e_val;

    slider_anim_create(s_val, e_val);

    SetVmParaCacheByLabel(vm_label_backlight, e_val);

    return;
}

register_ui_menu_load_info(\
    menu_load_backlight) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_backlight,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
