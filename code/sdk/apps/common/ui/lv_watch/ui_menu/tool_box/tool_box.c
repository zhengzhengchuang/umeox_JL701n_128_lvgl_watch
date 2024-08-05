#include "tool_box.h"

static bool bt_en;

static void bl_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_backlight);

    return;
}

static void bt_cb(lv_event_t *e)
{
    if(!e) return;

    bool en_state = GetBtEnableState();
    if(en_state == false)
        UserEnableBt();
    else
        UserDisableBt();
    
    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void tws_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_tws_main);

    return;
}

static void volume_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_sound_ctrl);

    return;
}

static void find_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t ble_bt_connect = \
        GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || \
        ble_bt_connect == 2)
    {
        ui_menu_jump(ui_act_id_not_conn); 
    }else
    {
        ui_menu_jump(ui_act_id_find_phone); 
    }

    return;
}

static void set_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_set_main);

    return;
}

static void dnd_cb(lv_event_t *e)
{
    if(!e) return;
 
    bool *main_sw = \
        &(Dnd_Info.main_sw);
    if(*main_sw == false)
        *main_sw = true;
    else if(*main_sw == true)
        *main_sw = false;
    DndManualUpdate();
    
    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);
    
    return;
}

static void camera_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t ble_bt_connect = \
        GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || \
        ble_bt_connect == 2)
    {
        ui_menu_jump(ui_act_id_not_conn); 
    }else
    {
        ui_menu_jump(ui_act_id_camera); 
    }

    return;
}

static void raise_cb(lv_event_t *e)
{
    if(!e) return;

    int raise = GetVmParaCacheByLabel(vm_label_raise);
    if(raise == 0)
        SetVmParaCacheByLabel(vm_label_raise, 1);
    else
        SetVmParaCacheByLabel(vm_label_raise, 0);

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;
    ui_menu_jump(act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t rm_act_id = \
        ui_act_id_remote_music;  
    ui_act_id_t down_act_id = \
        ui_act_id_watchface;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
            ui_act_id_null, rm_act_id, ui_act_id_tool_box);
    else
        tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
            rm_act_id, ui_act_id_null, ui_act_id_tool_box);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bool en_state = GetBtEnableState();
    if(bt_en != en_state)
    {
        bt_en = en_state;
        ui_act_id_t act_id = \
            p_ui_info_cache->cur_act_id;
        ui_menu_jump(act_id);
    }

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    /* 背光 */
    int bl_val = GetVmParaCacheByLabel(vm_label_backlight);
    uint8_t bl_level = (bl_val - TCFG_BACKLIGHT_MIN_VAL)/TCFG_BACKLIGHT_STEPS_VAL;
    bl_level = bl_level > TCFG_BACKLIGHT_MAX_LEVEL?TCFG_BACKLIGHT_MAX_LEVEL:bl_level;
    widget_img_para.img_x = 24;
    widget_img_para.img_y = 60;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_bl_00_index + bl_level;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = bl_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /*bt开关*/
    bool en_state = GetBtEnableState();
    bt_en = en_state;
    widget_img_para.img_x = 140;
    widget_img_para.img_y = 60;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_bt_00_index + en_state;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = bt_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /*tws耳机*/
    widget_img_para.img_x = 256;
    widget_img_para.img_y = 60;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_tws_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = tws_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /*音量控制*/
    widget_img_para.img_x = 24;
    widget_img_para.img_y = 172;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_vol_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = volume_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /*找手机*/
    widget_img_para.img_x = 140;
    widget_img_para.img_y = 172;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_find_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = find_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /* 设置 */
    widget_img_para.img_x = 256;
    widget_img_para.img_y = 172;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_set_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = set_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /* 勿扰 */
    int dnd_state = \
        GetVmParaCacheByLabel(vm_label_dnd_state);
    widget_img_para.img_x = 24;
    widget_img_para.img_y = 284;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_dnd_00_index + dnd_state;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = dnd_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /* 相机 */
    widget_img_para.img_x = 140;
    widget_img_para.img_y = 284;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_camera_00_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = camera_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /* 抬腕亮屏 */
    int raise = GetVmParaCacheByLabel(vm_label_raise);
    widget_img_para.img_x = 256;
    widget_img_para.img_y = 284;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tool_box_raise_00_index + raise;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = raise_cb;
    widget_img_para.user_data = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    /* 指示点 */
    if(lang_txt_is_arabic() == false)
    {
        widget_img_para.img_x = 164;
        widget_img_para.img_y = 408;
        widget_img_para.img_parent = obj;
        widget_img_para.file_img_dat = tool_box_other_01_index;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        widget_img_para.user_data = NULL;
        common_widget_img_create(&widget_img_para, NULL);

        widget_img_para.img_x = 188;
        widget_img_para.file_img_dat = tool_box_other_00_index;
        common_widget_img_create(&widget_img_para, NULL);
    }else
    {
        widget_img_para.img_x = 188;
        widget_img_para.img_y = 408;
        widget_img_para.img_parent = obj;
        widget_img_para.file_img_dat = tool_box_other_01_index;
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;
        widget_img_para.user_data = NULL;
        common_widget_img_create(&widget_img_para, NULL);

        widget_img_para.img_x = 164;
        widget_img_para.file_img_dat = tool_box_other_00_index;
        common_widget_img_create(&widget_img_para, NULL);
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
    menu_load_tool_box) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_tool_box,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
