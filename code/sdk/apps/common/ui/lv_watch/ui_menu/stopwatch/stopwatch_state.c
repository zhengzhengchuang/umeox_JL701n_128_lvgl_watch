#include "stopwatch_state.h"

static lv_obj_t *time_obj[8];
static uint16_t time_dsc_idx[8];

static uint16_t state_dsc_idx;

static void reset_cb(lv_event_t *e)
{
    if(!e) return;

    UserStopwatchReset();

    ui_menu_jump(\
        ui_act_id_more_menu);

    return;
}

static void state_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = \
        lv_event_get_target(e);

    stopwatch_state_t state = \
        Stopwatch_Info.state;

    if(state == stopwatch_state_start)
    {
        UserStopwatchPause();

        common_widget_img_replace_src(obj, \
            comm_icon_28_index, state_dsc_idx);
    }else if(state == stopwatch_state_pause)
    {
        UserStopwatchResume();

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
            prev_act_id, ui_act_id_null, ui_act_id_stopwatch_state);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_stopwatch_state);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    stopwatch_state_t state = \
        Stopwatch_Info.state;
    if(state == stopwatch_state_pause)
        return;

    stopwatch_ctx_t *s_ctx = \
        &(Stopwatch_Info.ctx);
    uint8_t minute = s_ctx->minute;
    uint8_t second = s_ctx->second;
    uint8_t m_second = s_ctx->m_second;

    char time_str[10];
    memset(time_str, 0, \
        sizeof(time_str));
    sprintf(time_str, "%02d:%02d.%02d", \
        minute, second, m_second);

    uint32_t file_img_dat;
    for(uint8_t i = 0; i < 8; i++)
    {
        if(time_str[i] == ':')
            file_img_dat = \
                comm_num_34x56_wh_10_index;
        else if(time_str[i] == '.')
            file_img_dat = \
                comm_num_34x56_wh_14_index;
        else
            file_img_dat = \
                comm_num_34x56_wh_00_index + \
                    (time_str[i] - '0');

        common_widget_img_replace_src(time_obj[i], \
            file_img_dat, time_dsc_idx[i]);
    }

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    stopwatch_ctx_t *s_ctx = \
        &(Stopwatch_Info.ctx);
    uint8_t minute = s_ctx->minute;
    uint8_t second = s_ctx->second;
    uint8_t m_second = s_ctx->m_second;

    bool exist = \
        Stopwatch_Info.exist;
    stopwatch_state_t state = \
        Stopwatch_Info.state;
    
    char time_str[10];
    memset(time_str, 0, \
        sizeof(time_str));
    sprintf(time_str, "%02d:%02d.%02d", \
        minute, second, m_second);
    num_str_para.parent = \ 
        obj;
    num_str_para.num_obj_x = \
        58;
    num_str_para.num_obj_y = \
        184;
    num_str_para.p_num_str = \
        time_str;
    num_str_para.num_str_len = \
        8;
    num_str_para.num_obj = \
        time_obj;
    num_str_para.num_obj_max = \
        8;
    num_str_para.num_dsc_idx = \
        time_dsc_idx;
    num_str_para.file_00_index = \
        comm_num_34x56_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_11_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *comm_11_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, 0, 266);

    widget_img_para.img_parent = \
        obj;
    widget_img_para.img_x = \
        60;
    widget_img_para.img_y = \
        376;
    widget_img_para.file_img_dat = \
        comm_icon_29_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        reset_cb;
    widget_img_para.user_data = \
        NULL;
    lv_obj_t *reset_button = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(reset_button, 20);

    widget_img_para.img_x = 214;
    if(state == stopwatch_state_start)
        widget_img_para.file_img_dat = \
            comm_icon_27_index;
    else if(state == stopwatch_state_pause)
        widget_img_para.file_img_dat = \
            comm_icon_28_index;   
    widget_img_para.event_cb = \
        state_cb; 
    lv_obj_t *state_button = \
        common_widget_img_create(&widget_img_para, \
            &state_dsc_idx);
    lv_obj_set_ext_click_area(state_button, 20);

    if(state == stopwatch_state_start && \
        exist == false)
        UserStopwatchStart();

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_stopwatch_state) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_stopwatch_state,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 60,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
