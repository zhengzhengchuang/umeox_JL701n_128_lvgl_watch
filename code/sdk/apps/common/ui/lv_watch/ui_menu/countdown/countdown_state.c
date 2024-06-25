#include "countdown_time.h"
#include "countdown_state.h"

static lv_obj_t *percent_arc;
static uint16_t state_dsc_idx;
static lv_obj_t *time_obj[8];
static uint16_t time_dsc_idx[8];

static void reset_cb(lv_event_t *e)
{
    if(!e) return;

    UserCountdownReset();

    ui_menu_jump(\
        ui_act_id_more_menu);

    return;
}

static void state_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);

    countdown_state_t state = \
        Countdown_Info.state;
    if(state == countdown_state_start)
    {
        UserCountdownPause();

        common_widget_img_replace_src(obj, \
            comm_icon_28_index, state_dsc_idx);
    }else if(state == countdown_state_puase)
    {
        UserCountdownResume();

        common_widget_img_replace_src(obj, \
            comm_icon_27_index, state_dsc_idx);
    }
        
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_more_menu;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_countdown_state);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_countdown_state);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    countdown_state_t state = \
        Countdown_Info.state;
    if(state == countdown_state_puase)
        return;

    countdown_ctx_t *c_ctx = \
        &(Countdown_Info.ctx);
    int16_t percent = c_ctx->percent;
    uint32_t cur_cnt = c_ctx->cur_cnt;
    uint32_t total_cnt = c_ctx->total_cnt;
    
    uint8_t time_hour = cur_cnt/3600;
    uint8_t time_minute = (cur_cnt%3600)/60;
    uint8_t time_second = (cur_cnt%3600)%60;
    char cur_time_str[10];
    memset(cur_time_str, 0, \
        sizeof(cur_time_str));
    sprintf(cur_time_str, "%02d:%02d:%02d", \
        time_hour, time_minute, time_second);

    uint32_t file_img_dat;
    for(uint8_t i = 0; i < 8; i++)
    {
        if(cur_time_str[i] == ':')
            file_img_dat = \
                comm_num_30x50_wh_10_index;
        else
            file_img_dat = \
                comm_num_30x50_wh_00_index + \
                    (cur_time_str[i] - '0');

        common_widget_img_replace_src(time_obj[i], \
            file_img_dat, time_dsc_idx[i]);
    }
    
    lv_arc_set_value(percent_arc, percent);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    menu_align_t menu_align = \
        menu_align_left;
    if(lang_txt_is_arabic())
        menu_align = \
            menu_align_right;

    countdown_ctx_t *c_ctx = \
        &(Countdown_Info.ctx);
    int16_t percent = c_ctx->percent;
    uint32_t cur_cnt = c_ctx->cur_cnt;
    uint32_t total_cnt = c_ctx->total_cnt;
    
    bool exist = \
        Countdown_Info.exist;
    countdown_state_t state = \
        Countdown_Info.state;
    
    widget_arc_para.arc_parent = \
        obj;
    widget_arc_para.arc_bg_width = \
        320;
    widget_arc_para.arc_bg_height = \
        320;
    widget_arc_para.arc_bg_opax = \
        LV_OPA_0;
    widget_arc_para.arc_main_start_angle = \
        0;
    widget_arc_para.arc_main_end_angle = \
        360;
    widget_arc_para.arc_indic_start_angle = \
        0;
    widget_arc_para.arc_indic_end_angle = \
        360;
    widget_arc_para.arc_rotation_angle = \
        270;
    widget_arc_para.arc_min_value = \
        0;
    widget_arc_para.arc_max_value = \
        GetPercentMax();
    widget_arc_para.arc_cur_value = \
        percent;
    widget_arc_para.arc_main_line_width = \
        10;
    widget_arc_para.arc_indic_line_width = \
        10;
    widget_arc_para.arc_main_line_color = \
        lv_color_hex(0x13130b);
    widget_arc_para.arc_indic_line_color = \
        lv_color_hex(0xD0BD84);
    widget_arc_para.arc_main_is_rounded = \
        true;
    widget_arc_para.arc_indic_is_rounded = \
        true;
    widget_arc_para.arc_click_is_clear = \
        true;
    percent_arc = \
        common_widget_arc_create(&widget_arc_para);
    lv_obj_align(percent_arc, LV_ALIGN_TOP_MID, 0, 40);

    widget_img_para.img_parent = \
        obj;
    if(menu_align == menu_align_right)
        widget_img_para.img_x = \
            215;
    else
        widget_img_para.img_x = \
            113;
    widget_img_para.img_y = \
        123;
    widget_img_para.file_img_dat = \
        countdown_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    common_widget_img_create(&widget_img_para, NULL);

    uint8_t total_time_hour = \
        total_cnt/3600;
    uint8_t total_time_minute = \
        (total_cnt%3600)/60;
    uint8_t total_time_second = \
        (total_cnt%3600)%60;
    char total_time_str[10];
    memset(total_time_str, 0, \
        sizeof(total_time_str));
    sprintf(total_time_str, "%02d:%02d:%02d", \
        total_time_hour, total_time_minute, total_time_second);
    num_str_para.parent = \ 
        obj;
    if(menu_align == menu_align_right)
        num_str_para.num_obj_x = \
            105;
    else
        num_str_para.num_obj_x = \
            160;
    num_str_para.num_obj_y = \
        130;
    num_str_para.p_num_str = \
        total_time_str;
    num_str_para.num_str_len = \
        8;
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_14x22_gr_00_index;
    common_widget_num_str_create(&num_str_para);

    uint8_t cur_time_hour = \
        cur_cnt/3600;
    uint8_t cur_time_minute = \
        (cur_cnt%3600)/60;
    uint8_t cur_time_second = \
        (cur_cnt%3600)%60;
    char cur_time_str[10];
    memset(cur_time_str, 0, \
        sizeof(cur_time_str));
    sprintf(cur_time_str, "%02d:%02d:%02d", \
        cur_time_hour, cur_time_minute, cur_time_second);
    num_str_para.parent = \ 
        obj;
    num_str_para.num_obj_x = \
        79;
    num_str_para.num_obj_y = \
        190;
    num_str_para.p_num_str = \
        cur_time_str;
    num_str_para.num_str_len = \
        8;
    num_str_para.num_obj = \
        time_obj;
    num_str_para.num_obj_max = \
        8;
    num_str_para.num_dsc_idx = \
        time_dsc_idx;
    num_str_para.file_00_index = \
        comm_num_30x50_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.img_x = \
        60;
    widget_img_para.img_y = \
        376;
    widget_img_para.file_img_dat = \
        comm_icon_26_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        reset_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *reset_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(reset_button, 20);

    widget_img_para.img_x = \
        214;
    if(state == countdown_state_start)
        widget_img_para.file_img_dat = \
            comm_icon_27_index;
    else if(state == countdown_state_puase)
        widget_img_para.file_img_dat = \
            comm_icon_28_index;
    widget_img_para.event_cb = \
        state_cb;
    lv_obj_t *state_button = \
    common_widget_img_create(&widget_img_para, \
        &state_dsc_idx);
    lv_obj_set_ext_click_area(state_button, 20);

    if(state == countdown_state_start && \
        exist == false)
        UserCountdownStart();

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_countdown_state) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_countdown_state,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
