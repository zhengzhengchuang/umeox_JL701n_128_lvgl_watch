#include "call_volume.h"
#include "audio_config.h"
#include "../../../../../../include_lib/btstack/avctp_user.h"

static s8 call_vol_tmp;
static const uint8_t inc_mul = 5;
static lv_obj_t *call_vol_slider;

static lv_timer_t *return_timer;
static const uint16_t return_duration = 5*1000;

#define Sys_Max_Vol (get_max_sys_vol() - 1)

static void return_timer_cb(lv_timer_t *timer)
{
    if(!return_timer)
        return;

    ui_menu_jump(ui_act_id_call_online);

    return;
}

static void return_timer_resume(void)
{
    if(!return_timer)
        return;

    lv_timer_reset(return_timer);
 
    return;
}

static void return_timer_create(void)
{
    if(return_timer)
    {
        lv_timer_del(return_timer);
        return_timer = NULL;
    }

    return_timer = lv_timer_create(return_timer_cb, return_duration, NULL);

    return;
}

static void return_timer_delete(void)
{
    if(return_timer)
    {
        lv_timer_del(return_timer);
        return_timer = NULL;
    }

    return;
}

static void slider_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    int32_t slider_val = lv_slider_get_value(obj);
    s8 call_vol = slider_val/inc_mul;
    call_vol = call_vol < 0?0:call_vol;
    call_vol = call_vol > Sys_Max_Vol?Sys_Max_Vol:call_vol;

    if(call_vol != call_vol_tmp)
    {
        call_vol_tmp = call_vol;
        app_audio_set_volume(APP_AUDIO_STATE_CALL, call_vol, 1);
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME, 1, &call_vol);
    }

    return_timer_resume();

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = ui_act_id_call_online;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_call_volume);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_call_volume);

    return_timer_create();

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return_timer_delete();

    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    /*如果说当前是设备更新音量，就不要同步手机的音量*/
    bool sd = lv_slider_is_dragged(call_vol_slider);
    if(sd == true) return;

    s8 cur_call_vol = app_audio_get_volume(APP_AUDIO_STATE_CALL);
    if(cur_call_vol == call_vol_tmp)
        return;

    call_vol_tmp = cur_call_vol;

    int32_t cur_val = cur_call_vol*inc_mul;
    lv_slider_set_value(call_vol_slider, cur_val, LV_ANIM_OFF);

    return_timer_resume();

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    s8 cur_call_vol = app_audio_get_volume(APP_AUDIO_STATE_CALL);
    uint8_t min_call_vol = 0;
    uint8_t max_call_vol = Sys_Max_Vol;
    
    call_vol_tmp = cur_call_vol;

    int32_t slc_min_val = min_call_vol*inc_mul;
    int32_t slc_max_val = max_call_vol*inc_mul;
    int32_t slc_cur_val = cur_call_vol*inc_mul;

    widget_slider_para.slider_parent = obj;
    widget_slider_para.slider_width = 82;
    widget_slider_para.slider_height = 284;
    widget_slider_para.slider_min_value = slc_min_val;
    widget_slider_para.slider_max_value = slc_max_val;
    widget_slider_para.slider_cur_value = slc_cur_val;
    widget_slider_para.slider_main_color = lv_color_hex(0xB28146);
    widget_slider_para.slider_indic_color = lv_color_hex(0xF0D990);
    widget_slider_para.slider_knob_opax = LV_OPA_0;
    widget_slider_para.slider_knob_color = lv_color_hex(0x000000);
    widget_slider_para.event_cb = slider_cb;
    widget_slider_para.user_data = NULL;
    call_vol_slider = common_widget_slider_create(&widget_slider_para);
    lv_obj_set_style_radius(call_vol_slider, 26, LV_PART_MAIN);
    lv_obj_set_style_radius(call_vol_slider, 26, LV_PART_INDICATOR);
    lv_obj_center(call_vol_slider);

    widget_img_para.img_parent = call_vol_slider;
    widget_img_para.file_img_dat = comm_icon_44_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    lv_obj_t *call_vol_down = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(call_vol_down, LV_ALIGN_BOTTOM_MID, 0, -20);

    widget_img_para.file_img_dat = comm_icon_45_index;
    lv_obj_t *call_vol_up = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(call_vol_up, LV_ALIGN_TOP_MID, 0, 20);

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

    s8 call_vol = app_audio_get_volume(APP_AUDIO_STATE_CALL);

    if(state == Rdec_Forward)
    {
        if(call_vol >= Sys_Max_Vol)
            return;
        call_vol++;
    }else if(state == Rdec_Backward)
    {
        if(call_vol <= 0)
            return;
        call_vol--;
    }else
        return;

    call_vol = call_vol<0?0:call_vol;
    call_vol = call_vol>Sys_Max_Vol?Sys_Max_Vol:call_vol;

    app_audio_set_volume(APP_AUDIO_STATE_CALL, call_vol, 1);
    user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME, 1, &call_vol);

    common_refresh_timer_re_run();
    ui_menu_refresh_func_cb refresh_func_cb = \
        p_ui_info_cache->menu_load_info.refresh_func_cb;
    if(refresh_func_cb)
        refresh_func_cb(obj);

    return;
}

register_ui_menu_load_info(\
    menu_load_call_volume) = 
{
    .menu_arg = NULL,
    .lock_flag = true,
    .return_flag = false,
    .menu_id = \
        ui_act_id_call_volume,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
