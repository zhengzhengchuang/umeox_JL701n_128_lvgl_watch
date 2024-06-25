#include "watchface_06.h"

static lv_obj_t *PTCTimeArc;

static lv_obj_t *PTTypeIcon;
static uint16_t PTTypeDscIdx;

static lv_obj_t *PTCModeIcon;
static uint16_t PTCModeDscIdx;

static lv_obj_t *PTTimeNumObj[5];
static uint16_t PTTimeNumDscIdx[5];

static lv_obj_t *PTCTimeNumObj[8];
static uint16_t PTCTimeNumDscIdx[8];

static const int16_t PTCTimePerMax = 10000;

//验证
static int16_t GetPTCTimePer(u8 mode, u8 type, u32 ctime)
{
    int16_t per = 0;

    if(mode == 1)//正计时，0%
        return per;

    if(type >= PT_Num)
        return per;

    u32 total_ctime;
    if(type == Fajr)
    {
        total_ctime = \
            PT_Info.result_timestamp[type];
    }else
    {
        u8 lidx;
        if(type == Maghrib)
            lidx = type - 2;
        else
            lidx = type - 1;
        u32 lct = \
           PT_Info.result_timestamp[lidx] + PTimeElapsed[lidx];
        total_ctime = \
            PT_Info.result_timestamp[type] - lct;
    }

    if(ctime >= total_ctime)
    {
        per = \
            PTCTimePerMax;
        return per;
    }

    per = ctime*PTCTimePerMax/total_ctime;
    per = per < 0?0:per;
    per = per > PTCTimePerMax?PTCTimePerMax:per;

    return per;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    wf_tileview_register_all_menu(obj);

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    uint8_t lang_id = \
        get_comm_sys_language();
    u8 PT_type = GetPTimeType();
    u32 PT_CTime = \
        GetPTimeCountdownTime(PT_type);
    u8 PTCMode = \
        GetPTimeCountdownMode(PT_type);

    /*祈祷时间弧百分比刷新*/
    int16_t arc_cur_value = \
        GetPTCTimePer(PTCMode, PT_type, PT_CTime);
    lv_arc_set_value(PTCTimeArc, arc_cur_value);

    /*祈祷时间倒计时模式刷新*/
    uint32_t mode_dat;
    if(PTCMode == 0)//倒计时
    {
        if(lang_id == lang_id_french)
            mode_dat = \
                watchface_06_rtime_fr_00_index; 
        else if(lang_id == lang_id_arabic)
            mode_dat = \
                watchface_06_rtime_ar_00_index;
        else
            mode_dat = \
                watchface_06_rtime_en_00_index;
    }else if(PTCMode == 1)//正计时
    {
        if(lang_id == lang_id_french)
            mode_dat = \
                watchface_06_etime_fr_00_index; 
        else if(lang_id == lang_id_arabic)
            mode_dat = \
                watchface_06_etime_ar_00_index;
        else
            mode_dat = \
                watchface_06_etime_en_00_index;
    }
    common_widget_img_replace_src(PTCModeIcon, mode_dat, \
        PTCModeDscIdx);

    /*祈祷时间倒计时刷新*/
    char PTCTimeStr[10];
    memset(PTCTimeStr, 0, sizeof(PTCTimeStr));
    sprintf(PTCTimeStr, "%02d:%02d:%02d", \
        PT_CTime/3600, (PT_CTime%3600)/60, (PT_CTime%3600)%60);
    common_widget_num_str_refresh(PTCTimeNumObj, 8, PTCTimeNumDscIdx, \
        PTCTimeStr, 8, watchface_06_10x16_00_index);

    /*类型Icon刷新*/
    uint32_t type_dat;
    if(lang_id == lang_id_french)
        type_dat = \
            watchface_06_prayer_fr_00_index + PT_type; 
    else if(lang_id == lang_id_arabic)
        type_dat = \
            watchface_06_prayer_ar_00_index + PT_type;
    else
        type_dat = \
            watchface_06_prayer_en_00_index + PT_type;
    common_widget_img_replace_src(PTTypeIcon, type_dat, \
        PTTypeDscIdx);

    /*类型时间刷新*/
    char PTNumStr[6];
    uint8_t PTResultHour = \
        PT_Info.result_timestamp[PT_type]/3600;
    uint8_t PTResultMinute = \
        (PT_Info.result_timestamp[PT_type]%3600)/60;    
    int time_format = \
        GetVmParaCacheByLabel(vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(PTResultHour > 12)
            PTResultHour -= 12;
        else if(PTResultHour == 0)
           PTResultHour = 12; 
    }
    memset(PTNumStr, 0, sizeof(PTNumStr));
    sprintf(PTNumStr, "%02d:%02d", \
        PTResultHour, PTResultMinute);
    common_widget_num_str_refresh(PTTimeNumObj, 5, PTTimeNumDscIdx, \
        PTNumStr, 5, watchface_06_12x20_00_index);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    uint8_t lang_id = \
        get_comm_sys_language();
    u8 PT_type = GetPTimeType();
    u32 PT_CTime = \
        GetPTimeCountdownTime(PT_type);
    u8 PTCMode = \
        GetPTimeCountdownMode(PT_type);

    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = \
        watchface_06_bg_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    common_widget_img_create(&widget_img_para, NULL);

    int16_t arc_cur_value = \
        GetPTCTimePer(PTCMode, PT_type, PT_CTime);
    widget_arc_para.arc_parent = obj;
    widget_arc_para.arc_bg_width = 112;
    widget_arc_para.arc_bg_height = 112;
    widget_arc_para.arc_bg_opax = LV_OPA_0;
    widget_arc_para.arc_main_start_angle = 0;
    widget_arc_para.arc_main_end_angle = 270;
    widget_arc_para.arc_indic_start_angle = 0;
    widget_arc_para.arc_indic_end_angle = 270;
    widget_arc_para.arc_rotation_angle = 135;
    widget_arc_para.arc_min_value = 0;
    widget_arc_para.arc_max_value = PTCTimePerMax;
    widget_arc_para.arc_cur_value = arc_cur_value; 
    widget_arc_para.arc_main_line_width = 8;
    widget_arc_para.arc_indic_line_width = 8;
    widget_arc_para.arc_main_line_color = \
        lv_color_hex(0x574C38);
    widget_arc_para.arc_indic_line_color = \
        lv_color_hex(0xF0D990);
    widget_arc_para.arc_main_is_rounded = true;
    widget_arc_para.arc_indic_is_rounded = true;
    widget_arc_para.arc_click_is_clear = true;
    PTCTimeArc = \
        common_widget_arc_create(&widget_arc_para);
    lv_obj_align(PTCTimeArc, LV_ALIGN_TOP_MID, 0, 168);

    widget_img_para.img_x = 146;
    widget_img_para.img_y = 194;
    widget_img_para.img_parent = obj;
    if(PTCMode == 0)//倒计时
    {
        if(lang_id == lang_id_french)
            widget_img_para.file_img_dat = \
                watchface_06_rtime_fr_00_index; 
        else if(lang_id == lang_id_arabic)
            widget_img_para.file_img_dat = \
                watchface_06_rtime_ar_00_index;
        else
            widget_img_para.file_img_dat = \
                watchface_06_rtime_en_00_index;
    }else if(PTCMode == 1)//正计时
    {
        if(lang_id == lang_id_french)
            widget_img_para.file_img_dat = \
                watchface_06_etime_fr_00_index; 
        else if(lang_id == lang_id_arabic)
            widget_img_para.file_img_dat = \
                watchface_06_etime_ar_00_index;
        else
            widget_img_para.file_img_dat = \
                watchface_06_etime_en_00_index;
    }
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    PTCModeIcon = \
        common_widget_img_create(&widget_img_para, &PTCModeDscIdx);

    char PTCTimeStr[10];
    memset(PTCTimeStr, 0, sizeof(PTCTimeStr));
    sprintf(PTCTimeStr, "%02d:%02d:%02d", \
        PT_CTime/3600, (PT_CTime%3600)/60, (PT_CTime%3600)%60);
    num_str_para.parent = \
       obj;
    num_str_para.num_obj_x = \
        148;
    num_str_para.num_obj_y = \
        232;
    num_str_para.p_num_str = \
        PTCTimeStr;
    num_str_para.num_str_len = \
        8;
    num_str_para.num_obj = \
        PTCTimeNumObj;
    num_str_para.num_obj_max = \
        8;
    num_str_para.num_dsc_idx = \
        PTCTimeNumDscIdx;
    num_str_para.file_00_index = \
        watchface_06_10x16_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_img_para.img_x = 66;
    widget_img_para.img_y = 264;
    widget_img_para.img_parent = obj;
    if(lang_id == lang_id_french)
        widget_img_para.file_img_dat = \
            watchface_06_prayer_fr_00_index + PT_type; 
    else if(lang_id == lang_id_arabic)
        widget_img_para.file_img_dat = \
            watchface_06_prayer_ar_00_index + PT_type;
    else
        widget_img_para.file_img_dat = \
            watchface_06_prayer_en_00_index + PT_type;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    PTTypeIcon = \
        common_widget_img_create(&widget_img_para, &PTTypeDscIdx);

    char PTNumStr[6];
    uint8_t PTResultHour = \
        PT_Info.result_timestamp[PT_type]/3600;
    uint8_t PTResultMinute = \
        (PT_Info.result_timestamp[PT_type]%3600)/60;    
    int time_format = \
        GetVmParaCacheByLabel(vm_label_time_format);
    if(time_format == time_format_12)
    {
        if(PTResultHour > 12)
            PTResultHour -= 12;
        else if(PTResultHour == 0)
           PTResultHour = 12; 
    }

    memset(PTNumStr, 0, sizeof(PTNumStr));
    sprintf(PTNumStr, "%02d:%02d", \
        PTResultHour, PTResultMinute);
    num_str_para.parent = \
       obj;
    num_str_para.num_obj_x = \
        220;
    num_str_para.num_obj_y = \
        278;
    num_str_para.p_num_str = \
        PTNumStr;
    num_str_para.num_str_len = \
        5;
    num_str_para.num_obj = \
        PTTimeNumObj;
    num_str_para.num_obj_max = \
        5;
    num_str_para.num_dsc_idx = \
        PTTimeNumDscIdx;
    num_str_para.file_00_index = \
        watchface_06_12x20_00_index;
    common_widget_num_str_create(&num_str_para);

    widget_time_para.time_x = 67;
    widget_time_para.time_y = 314;
    widget_time_para.num_inv = 0;
    widget_time_para.time_parent = \
        obj;
    widget_time_para.num_addr_index = \
        watchface_06_52x78_00_index;
    common_time_widget_create(\
        &widget_time_para, widget_time_mode_hh_mm);

    widget_date_para.date_x = 96;
    widget_date_para.date_y = 404;
    widget_date_para.num_inv = 0;
    widget_date_para.date_parent = obj;
    widget_date_para.num_addr_index = \
        watchface_06_14x22_00_index;
    common_date_widget_create(\
        &widget_date_para, widget_date_mode_mm_dd);

    comm_week_para.week_x = 174;
    comm_week_para.week_y = 395;
    comm_week_para.week_parent = obj;
    if(lang_id == lang_id_french)
        comm_week_para.week_addr_index = \
            watchface_06_week_fr_00_index; 
    else if(lang_id == lang_id_arabic)
        comm_week_para.week_addr_index = \
            watchface_06_week_ar_00_index; 
    else
        comm_week_para.week_addr_index = \
            watchface_06_week_en_00_index;
    common_week_widget_create(&comm_week_para);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_watchface_06) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_watchface,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 100,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
