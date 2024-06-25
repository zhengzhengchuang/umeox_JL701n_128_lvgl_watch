#include "tool_box.h"

static uint16_t dnd_state_icon_dsc;
static lv_obj_t *tool_box_container;
static lv_obj_t *tool_box_tips_label;
static lv_timer_t *tips_label_hd_timer;
static const uint16_t tips_label_duration = \
    3*1000;

static void tips_label_hd_timer_cb(lv_timer_t *timer)
{
    if(!tips_label_hd_timer)
        return;

    lv_timer_pause(tips_label_hd_timer);

    lv_label_set_text(tool_box_tips_label, \
        get_lang_txt_with_id(lang_txtid_no_reminder));

    return;
}

static void tips_label_hd_timer_resume(void)
{
    if(!tips_label_hd_timer)
        return;

    lv_timer_reset(tips_label_hd_timer);
    lv_timer_resume(tips_label_hd_timer);

    return;
}

static void tips_label_hd_timer_create(void)
{
    if(tips_label_hd_timer)
    {
        lv_timer_del(tips_label_hd_timer);
        tips_label_hd_timer = NULL;
    }

    tips_label_hd_timer = \
        lv_timer_create(tips_label_hd_timer_cb, \
            tips_label_duration, NULL);
    lv_timer_pause(tips_label_hd_timer);

    return;
}

static void tips_label_hd_timer_destory(void)
{
    if(tips_label_hd_timer)
    {
        lv_timer_del(tips_label_hd_timer);
        tips_label_hd_timer = NULL;
    }

    return;
}

/*************查找手机事件回调***************/
static void find_event_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t ble_bt_connect = \
        GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || \
        ble_bt_connect == 2)
    {
        lv_label_set_text(tool_box_tips_label, \
            get_lang_txt_with_id(lang_txtid_not_connected));  
    }else
    {
        lv_label_set_text(tool_box_tips_label, \
            get_lang_txt_with_id(lang_txtid_search_phone));

        FindPhoneHandle();
    }

    tips_label_hd_timer_resume();

    return;
}

/*************相机事件回调***************/
static void camera_event_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t ble_bt_connect = \
        GetDevBleBtConnectStatus();
    if(ble_bt_connect == 0 || \
        ble_bt_connect == 2)
    {
        lv_label_set_text(tool_box_tips_label, \
            get_lang_txt_with_id(lang_txtid_not_connected));

        tips_label_hd_timer_resume();  
    }else
    {
        DevReqOpCameraHandle(\
            DevReqEnterCamera);
        
        ui_menu_jump(ui_act_id_camera);
    }

    return;
}

/*************设置事件回调***************/
static void set_main_event_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_set_main);

    return;
}

/*************勿扰事件回调***************/
static void dnd_state_event_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);

    bool *main_sw = \
        &(Dnd_Info.main_sw);
    if(*main_sw == false)
        *main_sw = true;
    else if(*main_sw == true)
        *main_sw = false;
    DndManualUpdate();
    
    int dnd_state = \
        GetVmParaCacheByLabel(\
            vm_label_dnd_state);
    uint32_t file_img_dat = \
        tool_box_dnd_00_index + dnd_state;
    common_widget_img_replace_src(obj, \
        file_img_dat, dnd_state_icon_dsc);

    if(dnd_state == dnd_state_disable)
        lv_label_set_text(tool_box_tips_label, \
            get_lang_txt_with_id(lang_txtid_dnd_disable));
    else if(dnd_state == dnd_state_enable)
        lv_label_set_text(tool_box_tips_label, \
            get_lang_txt_with_id(lang_txtid_dnd_enable));
    tips_label_hd_timer_resume(); 

    return;
}

/*************背光事件回调***************/
static void backlight_event_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(\
        ui_act_id_backlight);

    return;
}

static void earphone_event_cb(lv_event_t *e)
{
    if(!e) return;


    return;
}

static void tool_box_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_width = \
        320;
    widget_obj_para.obj_height = \
        196;
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
    widget_obj_para.obj_is_scrollable = false;
    tool_box_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_align(tool_box_container, LV_ALIGN_TOP_MID, 0, 80);

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

    tips_label_hd_timer_create();

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    tips_label_hd_timer_destory();

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

    tool_box_container_create(obj);

    int cur_backlight_val = \
        GetVmParaCacheByLabel(\
            vm_label_backlight);
    uint8_t backlight_level = \
        (cur_backlight_val - TCFG_BACKLIGHT_MIN_VAL)/ \
            TCFG_BACKLIGHT_STEPS_VAL;
    backlight_level = backlight_level > TCFG_BACKLIGHT_MAX_LEVEL?\
        TCFG_BACKLIGHT_MAX_LEVEL:backlight_level;
    widget_img_para.img_parent = \
        tool_box_container;
    widget_img_para.file_img_dat = \
        tool_box_light_00_index + backlight_level;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = \
        backlight_event_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *backlight_icon = 
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(backlight_icon, LV_ALIGN_TOP_LEFT, 0, 0);

    widget_img_para.file_img_dat = \
        tool_box_find_00_index;
    widget_img_para.event_cb = \
        find_event_cb;
    lv_obj_t *find_icon = 
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(find_icon, LV_ALIGN_TOP_MID, 0, 0);

    widget_img_para.file_img_dat = \
        tool_box_camera_00_index;
    widget_img_para.event_cb = \
        camera_event_cb;
    lv_obj_t *camera_icon = 
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(camera_icon, LV_ALIGN_TOP_RIGHT, 0, 0);

    int dnd_state = \
        GetVmParaCacheByLabel(\
            vm_label_dnd_state);
    widget_img_para.file_img_dat = \
        tool_box_dnd_00_index + dnd_state;
    widget_img_para.event_cb = \
        dnd_state_event_cb;
    lv_obj_t *dnd_state_icon = 
        common_widget_img_create(&widget_img_para, \
            &dnd_state_icon_dsc);
    lv_obj_align(dnd_state_icon, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    int earphone_state = \
        GetVmParaCacheByLabel(vm_label_earphone_state);
    widget_img_para.file_img_dat = \
        tool_box_ear_00_index + earphone_state;
    widget_img_para.event_cb = \
        earphone_event_cb;
    lv_obj_t *earphone_state_icon = 
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(earphone_state_icon, LV_ALIGN_BOTTOM_MID, 0, 0);

    widget_img_para.file_img_dat = \
        tool_box_setting_00_index;
    widget_img_para.event_cb = \
        set_main_event_cb;
    lv_obj_t *setting_icon = 
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(setting_icon, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_13_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *tips_label_container = 
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(tips_label_container, LV_ALIGN_TOP_MID, 0, 308);
    
    widget_label_para.label_parent = \
        tips_label_container;
    widget_label_para.label_w = \
        280;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0x000000);
    widget_label_para.label_ver_center = \
        true;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_no_reminder);
    tool_box_tips_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_center(tool_box_tips_label);

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
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};

