#include "msg_detail.h"

static void delete_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t id = GetQueryId();
    uint8_t num = GetMessageNum();
    if(id >= num)
        return;

    id = (num - 1) - id;
    VmMessageDelByIdx(id);

    ui_act_id_t prev_act_id = ui_act_id_msg_list;
    ui_menu_jump(prev_act_id);
    
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = read_menu_return_level_id();
    if(prev_act_id == ui_act_id_null)
        prev_act_id = ui_act_id_watchface;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_msg_detail);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_msg_detail);

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

    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = menu_align_right;

    uint8_t query_id = GetQueryId();
    GetVmFlashMessageCtx(query_id);

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = message_icon_03_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *msg_detail_container = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(msg_detail_container, LV_ALIGN_TOP_MID, 0, 48);

    widget_img_para.img_parent = msg_detail_container;
    widget_img_para.file_img_dat = message_type_00_index + r_message.message_type; 
    lv_obj_t *msg_type_icon = common_widget_img_create(&widget_img_para, NULL); 
    if(menu_align == menu_align_right)
        lv_obj_align(msg_type_icon, LV_ALIGN_TOP_RIGHT, -20, 10);
    else
        lv_obj_align(msg_type_icon, LV_ALIGN_TOP_LEFT, 20, 10);

    struct sys_time time;
    SecToUtcTime(r_message.timestamp, &time);
    uint8_t hour = time.hour;
    uint8_t minute = time.min;
    static char time_str[6];
    memset(time_str, 0, sizeof(time_str));
    sprintf(time_str, "%02d:%02d", hour, minute);
    memset(&num_str_para, 0, sizeof(widget_num_str_para_t));
    num_str_para.parent = msg_detail_container;
    if(menu_align == menu_align_right)
        num_str_para.num_obj_x = 20;
    else
        num_str_para.num_obj_x = 240;
    num_str_para.num_obj_y = 20;
    num_str_para.p_num_str = time_str;
    num_str_para.num_str_len = 5;
    num_str_para.num_obj = NULL;
    num_str_para.num_obj_max = 0;
    num_str_para.num_dsc_idx = NULL;
    num_str_para.file_00_index = comm_num_14x22_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    char *ctx_str = r_message.msg_ctx;
    uint32_t str_len = strlen(ctx_str);
    bool arabic_letter = utf8_str_is_arabic(ctx_str, str_len);

    widget_textarea_para.parent = msg_detail_container;
    widget_textarea_para.width = 310;
    widget_textarea_para.height = 210;
    widget_textarea_para.txt = ctx_str;
    widget_textarea_para.font = NULL;
    widget_textarea_para.color = lv_color_hex(0xffffff);
    if(arabic_letter)
        widget_textarea_para.align = LV_TEXT_ALIGN_RIGHT;
    else
        widget_textarea_para.align = LV_TEXT_ALIGN_LEFT;
    widget_textarea_para.bg_opa_x = LV_OPA_0;
    widget_textarea_para.bg_color = lv_color_hex(0x000000);
    widget_textarea_para.border_width = 0;
    widget_textarea_para.border_opa_x = LV_OPA_0;
    widget_textarea_para.border_color = lv_color_hex(0x000000);
    lv_obj_t *md_textarea = common_widget_textarea_create(&widget_textarea_para);
    lv_obj_align(md_textarea, LV_ALIGN_TOP_MID, 0, 68);

    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = comm_icon_14_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = delete_cb;
    widget_img_para.user_data = NULL;
    lv_obj_t *delete_button = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(delete_button, LV_ALIGN_TOP_MID, 0, 376);
    lv_obj_set_ext_click_area(delete_button, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_msg_detail) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_msg_detail,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
