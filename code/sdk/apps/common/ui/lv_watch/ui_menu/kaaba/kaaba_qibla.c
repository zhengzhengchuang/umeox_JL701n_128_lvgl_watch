#include "kaaba_qibla.h"

static bool kaaba_remind;
static int16_t kaaba_azimuth;
static lv_obj_t *kaaba_zoom;
static lv_obj_t *kaaba_pointer;
static lv_obj_t *kaaba_compass;
static lv_obj_t *kaaba_yaw_label;

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    EnableGmModule();

    kaaba_azimuth = GetKaabaQiblaAzimuth();

    ui_act_id_t prev_act_id = \
        ui_act_id_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_kaaba_qibla);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_kaaba_qibla);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    bool OffScr = GetIsEnterOffScreen();
    if(OffScr == false) DisableGmModule();

    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bool cali_succ = \
        GetGmCaliSucc();
    if(cali_succ == false)
    {
        ui_menu_jump(ui_act_id_gm_cali);

        return;
    }

    int16_t Yaw = GetGmYawAngle();
    int16_t compass_rotate = (-1)*Yaw*10;
    lv_img_set_angle(kaaba_compass, compass_rotate);

    char YawNumStr[10];
    memset(YawNumStr, 0, sizeof(YawNumStr));
    sprintf(YawNumStr, "%d°", Yaw);
    lv_label_set_text(kaaba_yaw_label, YawNumStr);

    int16_t zoom_rotate = (Yaw - kaaba_azimuth)*10;
    lv_img_set_angle(kaaba_zoom, zoom_rotate);

    int16_t min = kaaba_azimuth - 5;
    int16_t max = kaaba_azimuth + 5;
    min = min < 0?(360 + min):min;
    max = max < 0?(360 + max):max;
    if(max > min)
    {
        if(Yaw >= min && Yaw <= max)
        {
            if(kaaba_remind == false)
            {
                kaaba_remind = true;
                motor_run(1, def_motor_duty);
            }
        }else
        {
            kaaba_remind = false;
        }
    }else
    {
        if(Yaw >= min || Yaw <= max)
        {
            if(kaaba_remind == false)
            {
                kaaba_remind = true;
                motor_run(1, def_motor_duty);
            }
        }else
        {
            kaaba_remind = false;
        }
    }

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    kaaba_remind = false;

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        kaaba_01_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *kaaba_qibla_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(kaaba_qibla_container, LV_ALIGN_CENTER, 0, 0);

    int16_t Yaw = \
        GetGmYawAngle();
    int16_t compass_rotate = (-1)*Yaw*10;
    widget_img_para.img_parent = \
        kaaba_qibla_container;
    widget_img_para.file_img_dat = \
        kaaba_02_index;
    kaaba_compass = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_center(kaaba_compass);
    lv_img_set_angle(kaaba_compass, compass_rotate);

    int16_t zoom_rotate = \
        (Yaw - kaaba_azimuth)*10;
    widget_img_para.file_img_dat = \
        kaaba_03_index;
    kaaba_zoom = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(kaaba_zoom, LV_ALIGN_TOP_MID, 0, 0);
    lv_img_set_pivot(kaaba_zoom, 23, 168);
    lv_img_set_angle(kaaba_zoom, zoom_rotate);

    char YawNumStr[10];
    memset(YawNumStr, 0, sizeof(YawNumStr));
    sprintf(YawNumStr, "%d°", Yaw);
    widget_label_para.label_w = \
        60;
    widget_label_para.label_h = \
        37;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        &font_common_num_32;
    widget_label_para.label_parent = \
        kaaba_qibla_container;
    widget_label_para.label_text = \
        YawNumStr;
    kaaba_yaw_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_center(kaaba_yaw_label);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_kaaba_qibla) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_kaaba_qibla,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
