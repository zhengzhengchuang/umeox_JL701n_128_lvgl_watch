#include "ota_update.h"

static u16 dsc_idx;
static lv_obj_t *upgrade_icon;
static lv_obj_t *upgrade_label;

static lv_timer_t *ugd_timer;
void upgrade_completed_timer_destory(void)
{
    if(ugd_timer)
        lv_timer_del(ugd_timer);
    ugd_timer = NULL;

    return;
}

static void timer_cb(lv_timer_t *timer)
{
    if(ugd_timer == NULL) 
        return;

    u8 state = GetOtaUpgradeState(); 
    if(state == upgrade_fail)
        OtaUpgradeFailHandle();
        
    return;
}

void upgrade_completed_timer_create(void)
{
    if(ugd_timer == NULL)
        ugd_timer = lv_timer_create(timer_cb, 5*1000, NULL);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
        ui_act_id_null, ui_act_id_null, ui_act_id_ota_update);

    OtaUpgradeHandle();

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    upgrade_completed_timer_destory();

    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    u32 file;
    u32 lang_txt;
    u8 state = GetOtaUpgradeState();
    if(state == upgrading)
    {
        file = remind_upgrade_00_index;
        lang_txt = lang_txtid_upgrading;
    }
    // else if(state == upgrade_succ)
    // {
    //     file = remind_upgrade_01_index;
    //     lang_txt = lang_txtid_upgrade_succ;
    // }
    else if(state == upgrade_fail)
    {
        file = remind_upgrade_02_index;
        lang_txt = lang_txtid_upgrade_fail;
    }else
    {
        file = remind_upgrade_00_index;
        lang_txt = lang_txtid_upgrading;
    }

    common_widget_img_replace_src(upgrade_icon, file, dsc_idx);
    lv_label_set_text(upgrade_label, get_lang_txt_with_id(lang_txt));

    if(/*state == upgrade_succ || */state == upgrade_fail)
        upgrade_completed_timer_create();

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    u32 file;
    u32 lang_txt;
    u8 state = GetOtaUpgradeState();
    if(state == upgrading)
    {
        file = remind_upgrade_00_index;
        lang_txt = lang_txtid_upgrading;
    }
    // else if(state == upgrade_succ)
    // {
    //     file = remind_upgrade_01_index;
    //     lang_txt = lang_txtid_upgrade_succ;
    // }
    else if(state == upgrade_fail)
    {
        file = remind_upgrade_02_index;
        lang_txt = lang_txtid_upgrade_fail;
    }else
    {
        file = remind_upgrade_00_index;
        lang_txt = lang_txtid_upgrading;
    }

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = file;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    widget_img_para.user_data = NULL;
    upgrade_icon = common_widget_img_create(&widget_img_para, &dsc_idx);
    lv_obj_align(upgrade_icon, LV_ALIGN_TOP_MID, 0, 32);

    widget_label_para.label_w = 330;
    widget_label_para.label_h = Label_Line_Height*2;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txt);
    upgrade_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(upgrade_label, LV_ALIGN_TOP_MID, 0, 332);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_ota_update) = 
{
    .menu_arg = NULL,
    .lock_flag = true,
    .return_flag = false,
    .menu_id = \
        ui_act_id_ota_update,
    .user_offscreen_time = Always_OnScreen,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
