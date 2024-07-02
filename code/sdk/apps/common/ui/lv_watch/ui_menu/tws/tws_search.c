#include "tws_search.h"

static uint8_t LSn;

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    TwsBtEmitterStart();

    ui_act_id_t prev_act_id = \
        ui_act_id_tws_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_tws_search);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_tws_search);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    bool Sn = GetTwsScanComplete();
    if(LSn == 0 && Sn == false)
        bt_emitter_stop_search_device();
        
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bool Sn = GetTwsScanComplete();
    if(Sn == true)
    {
        ui_menu_jump(ui_act_id_tws_not_found);
        return;
    }

    if(LSn != TwsScan.num)
    {
        LSn = TwsScan.num;
        ui_menu_jump(ui_act_id_tws_list);
    }
        
    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    LSn = TwsScan.num;

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = tws_02_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *search_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(search_icon, LV_ALIGN_TOP_MID, 0, 88);

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = comm_icon_11_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *comm_11_icon = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 264);

    widget_label_para.label_w = 300;
    widget_label_para.label_h = Label_Line_Height*2;
    widget_label_para.long_mode = LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = true;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = get_lang_txt_with_id(lang_txtid_searching);
    lv_obj_t *search_label = common_widget_label_create(&widget_label_para);
    lv_obj_align(search_label, LV_ALIGN_TOP_MID, 0, 342);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_tws_search) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_tws_search,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
