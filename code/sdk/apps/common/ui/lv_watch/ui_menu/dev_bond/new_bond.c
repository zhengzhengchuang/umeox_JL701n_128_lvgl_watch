#include "new_bond.h"

static lv_timer_t *exit_timer;

static void exit_timer_destroy(void)
{
    if(exit_timer)
    {
        lv_timer_del(exit_timer);
        exit_timer = NULL;
    }

    return;
}

static void timer_cb(lv_timer_t *timer)
{
    if(exit_timer == NULL)
        return;

    exit_timer_destroy();

    ui_act_id_t act_id = \
        ui_act_id_watchface;
    ui_menu_jump(act_id);

    return;
}

static void exit_timer_create(void)
{
    exit_timer_destroy();

    exit_timer = \
        lv_timer_create(timer_cb, 5*1000, NULL);

    return;
}

static void exit_cb(lv_event_t *e)
{
    ui_act_id_t act_id = ui_act_id_watchface;
    ui_menu_jump(act_id);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = ui_act_id_watchface;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_new_bond);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_new_bond);

    exit_timer_create();

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    exit_timer_destroy();

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

    widget_label_para.label_w = 300;
    widget_label_para.label_h = Label_Line_Height*2;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_new_bond);
    lv_obj_t *reset_tips_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(reset_tips_label, LV_ALIGN_TOP_MID, 0, 132);

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = comm_icon_11_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *comm_11_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 264);

    widget_img_para.file_img_dat = comm_icon_26_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = exit_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *exit_button = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(exit_button, LV_ALIGN_TOP_MID, 0, 376);
    lv_obj_set_ext_click_area(exit_button, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_new_bond) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_new_bond,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
