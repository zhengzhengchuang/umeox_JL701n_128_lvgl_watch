#include "remote_music.h"

#define Slider_Range_Inc (6)

static bool ble_connected;

static uint8_t last_remote_music_vol;
static Rmusic_state_t last_music_state;
static char last_music_title[Rmusic_Title_Len];

static uint16_t music_state_icon_dsc;
static lv_obj_t *remote_music_state_icon;

static lv_obj_t *remote_music_vol_slider;
static lv_obj_t *remote_music_title_label;


static void prev_cb(lv_event_t *e)
{
    if(!e) return;

    DevReqOpRmusicHandle(Cmd_Set_Rmusic_Prev, NULL);

    return;
}

static void next_cb(lv_event_t *e)
{
    if(!e) return;

    DevReqOpRmusicHandle(Cmd_Set_Rmusic_Next, NULL);

    return;
}

static void state_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);

    Rmusic_state_t *music_state = &(Rmusic_Info.state);
    if(*music_state == Rmusic_puase)
        *music_state = Rmusic_play;
    else if(*music_state == Rmusic_play)
        *music_state = Rmusic_puase;

    last_music_state = *music_state;
    
    uint32_t file_img_dat = remote_music_02_index + *music_state;
    common_widget_img_replace_src(obj, file_img_dat, music_state_icon_dsc);

    DevReqOpRmusicHandle(Cmd_Set_Rmusic_State, NULL);

    return;
}

static void volume_slider_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);

    uint8_t *volume = &(Rmusic_Info.volume);
    *volume = lv_slider_get_value(obj)/Slider_Range_Inc;

    if(*volume != last_remote_music_vol)
    {
        last_remote_music_vol = *volume;
        DevReqOpRmusicHandle(Cmd_Set_Rmusic_Volume, volume);
    }

    return;
}

static void disc_menu_create(lv_obj_t *obj)
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
    widget_obj_para.obj_is_scrollable = false;
    lv_obj_t *menu_cover_container = common_widget_obj_create(&widget_obj_para);

    widget_img_para.img_parent = menu_cover_container;
    widget_img_para.file_img_dat = comm_icon_13_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *disc_tips_label_container = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(disc_tips_label_container, LV_ALIGN_TOP_MID, 0, 308);
    
    widget_label_para.label_parent = disc_tips_label_container;
    widget_label_para.label_w = 280;
    widget_label_para.label_h = Label_Line_Height*2;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0x000000);
    widget_label_para.label_ver_center = true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_connect_and_open);
    lv_obj_t *disc_tips_label = common_widget_label_create(&widget_label_para);
    lv_obj_center(disc_tips_label);

    return;
}

static void menu_create(lv_obj_t *obj)
{
    uint8_t volume = Rmusic_Info.volume;
    uint8_t min_volume =  Rmusic_Min_Vol;
    uint8_t max_volume =  Rmusic_Max_Vol;
  
    last_remote_music_vol = volume;

    int32_t slider_cur_val = volume*Slider_Range_Inc;
    int32_t slider_min_val = min_volume*Slider_Range_Inc;
    int32_t slider_max_val = max_volume*Slider_Range_Inc;

    widget_slider_para.slider_parent = obj;
    widget_slider_para.slider_width = 304;
    widget_slider_para.slider_height = 72;
    widget_slider_para.slider_min_value = slider_min_val;
    widget_slider_para.slider_max_value = slider_max_val;
    widget_slider_para.slider_cur_value = slider_cur_val;
    widget_slider_para.slider_main_color = lv_color_hex(0xB28146);
    widget_slider_para.slider_indic_color = lv_color_hex(0xF0D990);
    widget_slider_para.slider_knob_opax = LV_OPA_0;
    widget_slider_para.slider_knob_color = lv_color_hex(0x000000);
    widget_slider_para.event_cb = volume_slider_cb;
    widget_slider_para.user_data = NULL;
    remote_music_vol_slider = common_widget_slider_create(&widget_slider_para);
    lv_obj_align(remote_music_vol_slider, LV_ALIGN_TOP_MID, 0, 70);

    widget_img_para.event_cb = NULL;
    widget_img_para.img_parent = remote_music_vol_slider;
    widget_img_para.file_img_dat = comm_icon_44_index;
    widget_img_para.img_click_attr = false;
    lv_obj_t *volume_down_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(volume_down_icon, LV_ALIGN_LEFT_MID, 20, 0);

    widget_img_para.file_img_dat = comm_icon_45_index;
    lv_obj_t *volume_up_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(volume_up_icon, LV_ALIGN_RIGHT_MID, -20, 0);

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = remote_music_04_index;
    lv_obj_t *remote_music_container = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(remote_music_container, LV_ALIGN_TOP_MID, 0, 166);


    widget_img_para.img_x = 24;
    widget_img_para.img_y = 124;
    widget_img_para.img_parent = remote_music_container;
    widget_img_para.file_img_dat = remote_music_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = prev_cb;
    lv_obj_t *remote_music_prev = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(remote_music_prev, 10);

    widget_img_para.img_x = 222;
    widget_img_para.file_img_dat = remote_music_01_index;
    widget_img_para.event_cb = next_cb;
    lv_obj_t *remote_music_next = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(remote_music_next, 10);

    Rmusic_state_t music_state;
    music_state = Rmusic_Info.state;
    last_music_state = music_state;
    widget_img_para.img_x = 123;
    widget_img_para.file_img_dat = remote_music_02_index + music_state;
    widget_img_para.event_cb = state_cb;
    remote_music_state_icon = common_widget_img_create(&widget_img_para, &music_state_icon_dsc);
    lv_obj_set_ext_click_area(remote_music_state_icon, 10);


    memcpy(last_music_title, Rmusic_Info.title, Rmusic_Title_Len);

    widget_label_para.label_x = 42;
    widget_label_para.label_y = 30;
    widget_label_para.label_w = 220;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = remote_music_container;
    widget_label_para.label_text = Rmusic_Info.title;
    remote_music_title_label = common_widget_label_create(&widget_label_para);
    
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    DevReqOpRmusicHandle(Cmd_Get_Rmusic_Para, NULL);

    ui_act_id_t act_id = ui_act_id_tool_box;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            act_id, ui_act_id_null, ui_act_id_remote_music);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, act_id, ui_act_id_remote_music);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    memset(last_music_title, 0, Rmusic_Title_Len);

    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    uint8_t ble_bt_connect = GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || ble_bt_connect == 2)
    {
        if(ble_connected == true)
        {
            ui_act_id_t act_id = p_ui_info_cache->cur_act_id;
            ui_menu_jump(act_id);

            return;
        }
    }else
    {
        if(ble_connected == false)
        {
            ui_act_id_t act_id = p_ui_info_cache->cur_act_id;
            ui_menu_jump(act_id);

            return;
        }
    }
        
    /*如果说当前是设备更新音量，就不要同步手机的音量*/
    bool slider_dragged = lv_slider_is_dragged(remote_music_vol_slider);
    if(slider_dragged == true)
        goto __refr_music;

    uint8_t volume = Rmusic_Info.volume;
    if(volume == last_remote_music_vol)
        goto __refr_music;

    last_remote_music_vol = volume;

    int32_t slider_cur_value = volume*Slider_Range_Inc;
    lv_slider_set_value(remote_music_vol_slider, slider_cur_value, LV_ANIM_OFF);

__refr_music:
    Rmusic_state_t music_state;
    music_state = Rmusic_Info.state;
    if(music_state == last_music_state)
        goto __refr_title;
    last_music_state = music_state;
    uint32_t file_img_dat = remote_music_02_index + music_state;
    common_widget_img_replace_src(remote_music_state_icon, \
        file_img_dat, music_state_icon_dsc);

__refr_title:
    int cmp_ret = strncmp(last_music_title, Rmusic_Info.title, Rmusic_Title_Len);
    if(cmp_ret == 0) 
        return;

    memcpy(last_music_title, Rmusic_Info.title, Rmusic_Title_Len);
    lv_label_set_text(remote_music_title_label, Rmusic_Info.title);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    menu_create(obj);
    ble_connected = true;

    uint8_t ble_bt_connect = GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || ble_bt_connect == 2)
    {
        ble_connected = false;
        disc_menu_create(obj);
    }
         
    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_remote_music) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_remote_music,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
