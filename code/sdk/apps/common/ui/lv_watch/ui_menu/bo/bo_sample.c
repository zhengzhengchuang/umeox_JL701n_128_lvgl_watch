#include "bo_sample.h"

static uint8_t bo_GIF_cnt;
static lv_obj_t *bo_GIF_icon;
static uint16_t bo_GIF_dsc_idx;

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *bo_sample_container;

static bool bo_sample_user_enable = false;

static lv_obj_t *bo_sample_start_icon;

static lv_timer_t *bo_sample_timer;
static const uint32_t bo_sample_duration = \
    45*1000;

static void scroll_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void bo_sample_container_create(\
    lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        0;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT;
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
    widget_obj_para.obj_is_scrollable = \
        true;
    bo_sample_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(bo_sample_container, \
        scroll_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void bo_sample_timeout_cb(lv_timer_t *timer)
{
    if(!timer) return;

    if(bo_sample_timer)
        lv_timer_del(bo_sample_timer);
    bo_sample_timer = NULL;

    DisablePpgModule();

    bo_sample_user_enable = false;

    lv_obj_clear_flag(bo_sample_start_icon, \
        LV_OBJ_FLAG_HIDDEN);

    return;
}

static void bo_sample_start_event_cb(lv_event_t *e)
{
    if(!e) return;

    common_offscreen_timer_restart();

    lv_obj_t *obj = \
        lv_event_get_target(e);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);

    SetBoRealVal(0);

    EnablePpgModule(\
        PpgWorkBo, PpgModeManual);

    bo_sample_user_enable = true;

    if(!bo_sample_timer)
        bo_sample_timer = \
            lv_timer_create(bo_sample_timeout_cb, \
                bo_sample_duration, NULL);

    return;
}

#define CHART_PRIX_MAJOR_CNT 5
static const char *chart_prix_label_str[\
    CHART_PRIX_MAJOR_CNT] = {
    "00:00", "06:00", "12:00", "18:00", "00:00",
};

#define CHART_PRIY_MAJOR_CNT 5

#define CHART_SERIES_NUM 1
static int16_t bo_sample_chart_points[\
    CHART_SERIES_NUM][24] = {
    LV_CHART_POINT_NONE
};

static lv_color_t bo_sample_chart_color[\
    CHART_SERIES_NUM];
static lv_chart_series_t *bo_sample_chart_series[\
    CHART_SERIES_NUM];

static void bo_sample_range_chart_create(\
    lv_obj_t *obj)
{
    const uint8_t bo_chart_min = \
        80;
    const uint8_t bo_chart_max = \
        100;
    const uint8_t bo_pixel_per = \
        14;

    uint16_t chart_h = \
        (bo_chart_max - bo_chart_min)*bo_pixel_per;

    lv_obj_set_style_pad_bottom(obj, \
        (LCD_HEIGHT - chart_h)/2, LV_PART_MAIN);

    bo_sample_chart_color[0] = \
        lv_color_hex(0x000000);

    widget_chart_para.chart_parent = \
        obj;
    widget_chart_para.chart_width = \
        300;
    widget_chart_para.chart_height = \
        chart_h;
    widget_chart_para.chart_type = \
        LV_CHART_TYPE_NONE;
    widget_chart_para.chart_item_cnt = \
        24;
    widget_chart_para.chart_update_mode = \
        LV_CHART_UPDATE_MODE_CIRCULAR;
    widget_chart_para.chart_hor_div = \
        CHART_PRIY_MAJOR_CNT;
    widget_chart_para.chart_ver_div = \
        0;
    widget_chart_para.chart_priy_range_min = \
        bo_chart_min;
    widget_chart_para.chart_priy_range_max = \
        bo_chart_max;
    widget_chart_para.chart_prix_major_cnt = \
        CHART_PRIX_MAJOR_CNT;
    widget_chart_para.chart_priy_major_cnt = \
        CHART_PRIY_MAJOR_CNT;
    widget_chart_para.chart_prix_label_str = \
        chart_prix_label_str;
    widget_chart_para.chart_priy_label_str = \
        NULL;
    widget_chart_para.chart_series_num = \
        CHART_SERIES_NUM;
    widget_chart_para.chart_ext_y_array = \
        bo_sample_chart_points[0];
    widget_chart_para.chart_series_color = \
        bo_sample_chart_color;
    widget_chart_para.chart_series = \
        bo_sample_chart_series;
    widget_chart_para.chart_main_opax = \
        LV_OPA_0;
    widget_chart_para.chart_main_color = \
        lv_color_hex(0x000000);
    lv_obj_t *bo_sample_chart = \
        common_widget_chart_create(&widget_chart_para);
    lv_obj_align(bo_sample_chart, LV_ALIGN_TOP_MID, \
        0, (3*LCD_HEIGHT - chart_h)/2);
    lv_obj_clear_flag(bo_sample_chart, LV_OBJ_FLAG_SCROLLABLE);


    const uint8_t bo_sample_bar_w = \
        8;
    const uint8_t bo_sample_bar_inv = \
        4;
    
    widget_obj_para.obj_parent = \
        bo_sample_chart;
    widget_obj_para.obj_width = \
        bo_sample_bar_w;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_100;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0xc86e32);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = \
        bo_sample_bar_w/2;
    widget_obj_para.obj_is_scrollable = \
        false;
    
    uint8_t bo_sample_min;
    uint8_t bo_sample_max;
    uint16_t bo_sample_bar_h;

    u8 HIdx = \
       (w_bo.CurIdx)/(60/Bo_Inv_Dur);

    for(uint8_t i = 0; i < (HIdx + 1); i++)
    {
        bo_sample_min = \
            w_bo.min_data[i];
        bo_sample_max = \
            w_bo.max_data[i];

        if(bo_sample_max < \
            bo_sample_min)
            continue;
        
        if(bo_sample_max == 0 && \
            bo_sample_min == 0)
            continue;

        if(bo_sample_max > bo_chart_max)
            bo_sample_max = bo_chart_max;
        if(bo_sample_min < bo_chart_min)
            bo_sample_min = bo_chart_min;
        
        bo_sample_bar_h = \
            (bo_sample_max - bo_sample_min)*bo_pixel_per;
        if(bo_sample_bar_h < bo_pixel_per)
            bo_sample_bar_h = bo_pixel_per;
        widget_obj_para.obj_height = \
            bo_sample_bar_h;

        lv_obj_t *bo_sample_bar = \
            common_widget_obj_create(&widget_obj_para);
        lv_obj_align(bo_sample_bar, LV_ALIGN_TOP_LEFT, \
            (bo_sample_bar_w + bo_sample_bar_inv)*i, \
                (bo_chart_max - bo_sample_max)*bo_pixel_per);
    }
    
    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = \
        p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t left_act_id = \
            ui_act_id_sleep_main;
        ui_act_id_t right_act_id = \
            ui_act_id_hr_sample;
        ui_act_id_t down_act_id = \
            ui_act_id_null;
        tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
            left_act_id, right_act_id, ui_act_id_bo_sample);
    }else
    {
        ui_act_id_t prev_act_id = \
            ui_act_id_menu;
        ui_act_id_t down_act_id = \
            ui_act_id_null;
        if(!lang_txt_is_arabic())
            tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
                prev_act_id, ui_act_id_null, ui_act_id_bo_sample);
        else
            tileview_register_all_menu(obj, ui_act_id_null, down_act_id, \
                ui_act_id_null, prev_act_id, ui_act_id_bo_sample);
    }

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    if(!bo_sample_user_enable)
        return;

    if(bo_sample_timer)
        lv_timer_del(bo_sample_timer);
    bo_sample_timer = NULL;

    DisablePpgModule();
        
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    if(!bo_sample_user_enable)
        return;

    bool wear_status = \
        GetPpgWearStatus();
    if(!wear_status)
    {
        ui_menu_jump(\
            ui_act_id_off_wrist);
        return;
    }
        
    bo_GIF_cnt++;
    bo_GIF_cnt %= 1;
    common_widget_img_replace_src(bo_GIF_icon, \
        bo_GIF_00_index + bo_GIF_cnt, bo_GIF_dsc_idx);

    common_offscreen_timer_restart();
        
    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    bo_GIF_cnt = 0;
    bo_sample_user_enable = \
        false;

    bo_sample_container_create(obj);

    widget_img_para.img_parent = \
        bo_sample_container;
    widget_img_para.file_img_dat = \
        bo_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *bo_sample_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(bo_sample_icon, LV_ALIGN_TOP_LEFT, \
        24, 72);

    u8 bo_sample_val = \
        GetBoRealVal();
    widget_data_para.data_x = \
        136;
    widget_data_para.data_y = \
        96;
    widget_data_para.num_inv = \
        3;
    widget_data_para.data_parent = \
        bo_sample_container;
    widget_data_para.num_addr_index = \
        comm_num_24x40_wh_00_index;
    widget_data_para.data_align = \
        widget_data_align_center;
    common_data_widget_create(&widget_data_para, \
        widget_data_type_bo, &bo_sample_val);

    widget_img_para.img_parent = \
        bo_sample_container;
    widget_img_para.img_x = \
        136;
    widget_img_para.img_y = \
        152;
    widget_img_para.file_img_dat = \
        comm_icon_32_index;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        bo_sample_start_event_cb;
    widget_img_para.user_data = \
        NULL;
    bo_sample_start_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(bo_sample_start_icon, 20);

    widget_img_para.img_parent = \
        bo_sample_container;
    widget_img_para.file_img_dat = \
        bo_GIF_00_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    bo_GIF_icon = \
        common_widget_img_create(&widget_img_para, &bo_GIF_dsc_idx);
    lv_obj_align(bo_GIF_icon, LV_ALIGN_TOP_MID, 0, 224);

    widget_img_para.file_img_dat = \
        bo_01_index;
    lv_obj_t *bo_01_icon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(bo_01_icon, LV_ALIGN_TOP_MID, 0, 392);

    bo_sample_range_chart_create(bo_sample_container);

    scroll_y = 0;
    lv_obj_scroll_to_y(bo_sample_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(bo_sample_container);
    scroll_top_y = lv_obj_get_scroll_top(bo_sample_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(bo_sample_container);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

static void menu_rdec_cb(lv_obj_t *obj, int state)
{
    if(lv_anim_get(bo_sample_container, NULL))
        return;

    int16_t scroll_y_tmp = scroll_y;

    if(state == Rdec_Forward)
        scroll_y_tmp += LCD_HEIGHT/2;
    else if(state == Rdec_Backward)
        scroll_y_tmp -= LCD_HEIGHT/2;
    else
        return;

    scroll_y_tmp = scroll_y_tmp < scroll_top_y?scroll_top_y:scroll_y_tmp;
    scroll_y_tmp = scroll_y_tmp > scroll_bottom_y?scroll_bottom_y:scroll_y_tmp;
    if(scroll_y_tmp != scroll_y)
        lv_obj_scroll_to_y(bo_sample_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_bo_sample) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_bo_sample,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
