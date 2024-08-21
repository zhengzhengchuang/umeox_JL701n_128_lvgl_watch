#include "sound_ctrl.h"
#include "audio_config.h"
#include "../../../../../../include_lib/btstack/avctp_user.h"

static s8 call_vol_tmp;
static s8 media_vol_tmp;

#define Sys_Max_Vol (get_max_sys_vol() - 1)

//滑块增大倍数
static const uint8_t inc_mul = 5;


static void switch_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);

    int sound_state = GetVmParaCacheByLabel(vm_label_sys_sound);
    sound_state = !sound_state;
    SetVmParaCacheByLabel(vm_label_sys_sound, sound_state);

    if(sound_state == 0)
    {
        SetVmParaCacheByLabel(vm_label_call_vol_b, call_vol_tmp);
        SetVmParaCacheByLabel(vm_label_media_vol_b, media_vol_tmp);

        //设置媒体音量值为0
        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, 0, 1);

        //设置通话音量值为0
        s8 call_vol_val = 0;
        app_audio_set_volume(APP_AUDIO_STATE_CALL, call_vol_val, 1);
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME, 1, &call_vol_val);
    }else
    {
        s8 call_vol_b = GetVmParaCacheByLabel(vm_label_call_vol_b);
        s8 media_vol_b = GetVmParaCacheByLabel(vm_label_media_vol_b);

        //恢复媒体音量值
        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, media_vol_b, 1);

        //恢复通话音量值
        app_audio_set_volume(APP_AUDIO_STATE_CALL, call_vol_b, 1);
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME, 1, &call_vol_b);
    }
    
    ui_act_id_t act_id = p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);
        
    return;
}

static void call_vol_slider_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    int32_t slider_val = lv_slider_get_value(obj);
    s8 call_vol_val = slider_val/inc_mul;
    call_vol_val = call_vol_val < 0?0:call_vol_val;
    call_vol_val = call_vol_val > Sys_Max_Vol?Sys_Max_Vol:call_vol_val;

    if(call_vol_val != call_vol_tmp)
    {
        call_vol_tmp = call_vol_val;
        app_audio_set_volume(APP_AUDIO_STATE_CALL, call_vol_val, 1);
        user_send_cmd_prepare(USER_CTRL_HFP_CALL_SET_VOLUME, 1, &call_vol_val);
    }

    return;
}

static void media_vol_slider_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    int32_t slider_val = lv_slider_get_value(obj);
    s8 media_vol_val = slider_val/inc_mul;
    media_vol_val = media_vol_val < 0?0:media_vol_val;
    media_vol_val = media_vol_val > Sys_Max_Vol?Sys_Max_Vol:media_vol_val;
    if(media_vol_val != media_vol_tmp)
    {
        media_vol_tmp = media_vol_val;
        app_audio_set_volume(APP_AUDIO_STATE_MUSIC, media_vol_val, 1);
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_sound_ctrl);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_sound_ctrl);
            
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

    menu_align_t menu_align = menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = comm_icon_05_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *switch_container = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(switch_container, LV_ALIGN_TOP_MID, 0, 52);

    widget_label_para.label_parent = switch_container;
    widget_label_para.label_w = 180;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    if(menu_align == menu_align_right)
        widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_sound);
    lv_obj_t *sound_label = common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_right)
        lv_obj_align(sound_label, LV_ALIGN_RIGHT_MID, -20, 0);
    else
        lv_obj_align(sound_label, LV_ALIGN_LEFT_MID, 20, 0);

    int sound_state = GetVmParaCacheByLabel(vm_label_sys_sound);
    widget_img_para.event_cb = switch_cb;
    widget_img_para.user_data = NULL;
    widget_img_para.img_parent = switch_container;
    widget_img_para.file_img_dat = comm_icon_00_index + sound_state;
    widget_img_para.img_click_attr = true;
    lv_obj_t *switch_button = common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align(switch_button, LV_ALIGN_LEFT_MID, 20, 0);
    else
        lv_obj_align(switch_button, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_set_ext_click_area(switch_button, 20);

    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = 160;
    widget_obj_para.obj_width = LCD_WIDTH;
    widget_obj_para.obj_height = LCD_HEIGHT - 160;
    widget_obj_para.obj_bg_opax = LV_OPA_0;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;
    lv_obj_t *ctx_container = common_widget_obj_create(&widget_obj_para);
    if(!sound_state) lv_obj_add_flag(ctx_container, LV_OBJ_FLAG_HIDDEN);

    widget_label_para.label_parent = ctx_container;
    widget_label_para.label_w = 320;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    if(menu_align == menu_align_right)
        widget_label_para.text_align = LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_call_volumn);
    lv_obj_t *call_volumn_label = common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_right)
        lv_obj_align(call_volumn_label, LV_ALIGN_TOP_RIGHT, -24, 0);
    else
        lv_obj_align(call_volumn_label, LV_ALIGN_TOP_LEFT, 24, 0);

    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_media_volumn);
    lv_obj_t *media_volumn_label = common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_right)
        lv_obj_align(media_volumn_label, LV_ALIGN_TOP_RIGHT, -24, 145);
    else
        lv_obj_align(media_volumn_label, LV_ALIGN_TOP_LEFT, 24, 145);

    widget_img_para.img_parent = ctx_container;
    widget_img_para.file_img_dat = comm_icon_43_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *call_vol_container = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align_to(call_vol_container, call_volumn_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_obj_t *media_vol_container = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align_to(media_vol_container, media_volumn_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    widget_img_para.img_parent = call_vol_container;
    widget_img_para.file_img_dat = comm_icon_44_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *call_vol_down = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(call_vol_down, LV_ALIGN_LEFT_MID, 12, 0);

    widget_img_para.file_img_dat = comm_icon_45_index;
    lv_obj_t *call_vol_up = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(call_vol_up, LV_ALIGN_RIGHT_MID, -12, 0);

    widget_img_para.img_parent = media_vol_container;
    widget_img_para.file_img_dat = comm_icon_44_index;
    lv_obj_t *media_vol_down = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(media_vol_down, LV_ALIGN_LEFT_MID, 12, 0);

    widget_img_para.file_img_dat = comm_icon_45_index;
    lv_obj_t *media_vol_up = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(media_vol_up, LV_ALIGN_RIGHT_MID, -12, 0);
    
    s8 cur_call_vol = app_audio_get_volume(APP_AUDIO_STATE_CALL);
    uint8_t min_call_vol = 0;
    uint8_t max_call_vol = Sys_Max_Vol;
    
    call_vol_tmp = cur_call_vol;

    int32_t slc_min_val = min_call_vol*inc_mul;
    int32_t slc_max_val = max_call_vol*inc_mul;
    int32_t slc_cur_val = cur_call_vol*inc_mul;

    widget_slider_para.slider_parent = call_vol_container;
    widget_slider_para.slider_width = 200;
    widget_slider_para.slider_height = 25;
    widget_slider_para.slider_min_value = slc_min_val;
    widget_slider_para.slider_max_value = slc_max_val;
    widget_slider_para.slider_cur_value = slc_cur_val;
    widget_slider_para.slider_main_color = lv_color_hex(0x666666);
    widget_slider_para.slider_indic_color = lv_color_hex(0xF0D990);
    widget_slider_para.slider_knob_opax = LV_OPA_0;
    widget_slider_para.slider_knob_color = lv_color_hex(0x000000);
    widget_slider_para.event_cb = call_vol_slider_cb;
    widget_slider_para.user_data = NULL;
    lv_obj_t *call_vol_slider = common_widget_slider_create(&widget_slider_para);
    lv_obj_center(call_vol_slider);
    lv_obj_set_ext_click_area(call_vol_slider, 20);

    s8 cur_media_vol = app_audio_get_volume(APP_AUDIO_STATE_MUSIC);
    uint8_t min_media_vol = 0;
    uint8_t max_media_vol = Sys_Max_Vol;
    
    media_vol_tmp = cur_media_vol;

    int32_t slm_min_val = min_media_vol*inc_mul;
    int32_t slm_max_val = max_media_vol*inc_mul;
    int32_t slm_cur_val = cur_media_vol*inc_mul;

    widget_slider_para.slider_parent = media_vol_container;
    widget_slider_para.slider_min_value = slm_min_val;
    widget_slider_para.slider_max_value = slm_max_val;
    widget_slider_para.slider_cur_value = slm_cur_val;
    widget_slider_para.event_cb = media_vol_slider_cb;
    lv_obj_t *media_vol_slider = common_widget_slider_create(&widget_slider_para);
    lv_obj_center(media_vol_slider);
    lv_obj_set_ext_click_area(media_vol_slider, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_sound_ctrl) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_sound_ctrl,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
