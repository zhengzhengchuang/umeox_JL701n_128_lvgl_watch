#include "quran_volume.h"
#include "audio_config.h"

static s8 media_vol_tmp;
static lv_obj_t *vol_slider;

#define Sys_Max_Vol (get_max_sys_vol() - 1)

//滑块增大倍数
static const uint8_t inc_mul = 5;

static void slider_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);
    int32_t slider_val = \
        lv_slider_get_value(obj);
    s8 media_vol_val = \
        slider_val/inc_mul;
    media_vol_val = media_vol_val < 0?0:media_vol_val;
    media_vol_val = media_vol_val > Sys_Max_Vol?Sys_Max_Vol:media_vol_val;

    if(media_vol_val != media_vol_tmp)
    {
        media_vol_tmp = media_vol_val;

        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, media_vol_val, 1);
    }

    return;
}

static void slider_anim_cb(void *var, int32_t val)
{
    lv_obj_t *obj = (lv_obj_t *)var;

    if(obj == NULL)
        return;

    lv_slider_set_value(obj, val, LV_ANIM_OFF);

    return;
}

static void slider_anim_create(int32_t s, int32_t e)
{
    lv_anim_t anim;

    int32_t inc_mul_s = s*inc_mul;
    int32_t inc_mul_e = e*inc_mul;

    uint32_t duration = \
        LV_ABS(inc_mul_e - inc_mul_s)*20;

    widget_anim_para.anim = &anim;
    widget_anim_para.anim_obj = vol_slider;
    widget_anim_para.anim_exec_xcb = slider_anim_cb;
    widget_anim_para.anim_duration = duration;
    widget_anim_para.anim_start_val = inc_mul_s;
    widget_anim_para.anim_end_val = inc_mul_e;
    widget_anim_para.is_start_anim = true;
    common_widget_anim_create(&widget_anim_para);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_quran_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_quran_volume);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_quran_volume);

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

    s8 cur_media_vol = \
        app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
    uint8_t min_media_vol = 0;
    uint8_t max_media_vol = Sys_Max_Vol;
    
    media_vol_tmp = cur_media_vol;

    int32_t slm_min_val = min_media_vol*inc_mul;
    int32_t slm_max_val = max_media_vol*inc_mul;
    int32_t slm_cur_val = cur_media_vol*inc_mul;

    widget_slider_para.slider_parent = obj;
    widget_slider_para.slider_width = 82;
    widget_slider_para.slider_height = 284;
    widget_slider_para.slider_min_value = slm_min_val;
    widget_slider_para.slider_max_value = slm_max_val;
    widget_slider_para.slider_cur_value = slm_cur_val;
    widget_slider_para.slider_main_color = lv_color_hex(0xB28146);
    widget_slider_para.slider_indic_color = lv_color_hex(0xF0D990);
    widget_slider_para.slider_knob_opax = LV_OPA_0;
    widget_slider_para.slider_knob_color = lv_color_hex(0x000000);
    widget_slider_para.event_cb = slider_cb;
    widget_slider_para.user_data = NULL;
    vol_slider = common_widget_slider_create(&widget_slider_para);
    lv_obj_set_style_radius(vol_slider, 26, LV_PART_MAIN);
    lv_obj_set_style_radius(vol_slider, 26, LV_PART_INDICATOR);
    lv_obj_align(vol_slider, LV_ALIGN_TOP_MID, 0, 80);

    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    widget_img_para.img_parent = vol_slider;
    widget_img_para.file_img_dat = comm_icon_44_index;
    widget_img_para.img_click_attr = false;
    lv_obj_t *vol_down = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(vol_down, LV_ALIGN_BOTTOM_MID, 0, -20);

    widget_img_para.file_img_dat = comm_icon_45_index;
    lv_obj_t *vol_up = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(vol_up, LV_ALIGN_TOP_MID, 0, 20);

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
    if(lv_anim_get(vol_slider, NULL))
        return;

    uint8_t min_val = 0;
    uint8_t max_val = Sys_Max_Vol;
    s8 s_val = \
        app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
    s8 e_val;

    if(state == Rdec_Forward)
    {
        if(s_val >= max_val)
            return;
        e_val = s_val + 1;
    }else if(state == Rdec_Backward)
    {
        if(s_val <= min_val)
            return;
        e_val = s_val - 1;
    }else
        return;

    e_val = e_val<min_val?min_val:e_val;
    e_val = e_val>max_val?max_val:e_val;

    slider_anim_create(s_val, e_val);

    media_vol_tmp = e_val;
    app_audio_set_volume(APP_AUDIO_STATE_MUSIC, e_val, 1);

    return;
}

register_ui_menu_load_info(\
    menu_load_quran_volume) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_quran_volume,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
