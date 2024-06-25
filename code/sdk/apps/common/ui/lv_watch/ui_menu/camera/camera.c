#include "camera.h"

static void photos_cb(lv_event_t *e)
{
    if(!e) return;

    DevReqOpCameraHandle(\
        DevReqTakePhotos);

    return;
}

static void dly_photos_cb(lv_event_t *e)
{
    if(!e) return;

    DevReqOpCameraHandle(\
        DevReqDlyTakePhotos);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_camera);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_camera);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    bool OffScreen = \
        GetIsEnterOffScreen();
    if(OffScreen == false)
    {
        bool RemExit = \
            GetCameraIsRemoteExit();
        if(RemExit == false)
            le_task_post(Le_Req_Exit_Camera, NULL, 0);

        SetCameraUnlockExit(false);
        SetCameraIsRemoteExit(false);
    }else
    {
        SetCameraUnlockExit(true);
    }

    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    u8 BleBtStatus = \
        GetDevBleBtConnectStatus();
    if(BleBtStatus == 0 || \
        BleBtStatus == 2)
    {
        ui_act_id_t prev_act_id = \
            read_menu_return_level_id();
        ui_menu_jump(prev_act_id);
    }
        
    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        camera_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *camera_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(camera_icon, LV_ALIGN_TOP_MID, 0, 108);

    widget_img_para.img_x = 42;
    widget_img_para.img_y = 316;
    widget_img_para.file_img_dat = \
        camera_01_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        dly_photos_cb;
    widget_img_para.user_data = \
        NULL;
    common_widget_img_create(&widget_img_para, NULL);
    
    widget_img_para.img_x = 226;
    widget_img_para.file_img_dat = \
        camera_02_index;
    widget_img_para.event_cb = \
        photos_cb;
    common_widget_img_create(&widget_img_para, NULL);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_camera) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_camera,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
