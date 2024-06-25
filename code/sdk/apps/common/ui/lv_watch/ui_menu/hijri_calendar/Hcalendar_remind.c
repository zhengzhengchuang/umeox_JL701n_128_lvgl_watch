#include "Hcalendar_remind.h"

static const uint8_t Hcale_fes_dysnum[\
    HcalendarFesNum] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
};
static const bool Hcale_fes_edyf[\
    HcalendarFesNum] = {
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,  
};

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_Hcalendar_remind);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_Hcalendar_remind);

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

    uint8_t Type = \
        GetHcalendarFesType();
    uint8_t dysnum = \
        Hcale_fes_dysnum[Type];
    bool edyf = \
        Hcale_fes_edyf[Type];

    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height*2;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_WRAP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(\
            lang_txtid_Hijri_New_Year + Type);
    lv_obj_t *Hcale_fes_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(Hcale_fes_label, LV_ALIGN_TOP_MID, \
        0, 40);

    char HcaleFesDayStr[3];
    for(uint8_t i = 0; i < dysnum; i++)
    {
        widget_img_para.img_parent = \
            obj;
        widget_img_para.file_img_dat = \
            hijri_calendar_icon_04_index;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = \
            NULL;
        widget_img_para.img_y = \
            130;
        widget_img_para.img_x = \
            (LCD_WIDTH - dysnum*80)/2 + i*80; 
        lv_obj_t *Hcale_day_container = \
            common_widget_img_create(&widget_img_para, NULL);

        u8 HFes_day = \
            HcalendarInfo.HFesInfo[Type].HFes_day;
        memset(HcaleFesDayStr, 0, \
            sizeof(HcaleFesDayStr));
        sprintf(HcaleFesDayStr, "%02d", HFes_day + i);
        num_str_para.parent = \
            Hcale_day_container;
        num_str_para.num_obj_x = \
            24;
        num_str_para.num_obj_y = \
            28;
        num_str_para.file_00_index = \
            comm_num_16x28_ye_00_index;
        num_str_para.p_num_str = \
            HcaleFesDayStr;
        num_str_para.num_str_len = \
            strlen(HcaleFesDayStr);
        num_str_para.num_obj = \
            NULL;
        num_str_para.num_obj_max = \
            0;
        num_str_para.num_dsc_idx = \
            NULL;
        common_widget_num_str_create(&num_str_para);
    }

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
    lv_obj_align(comm_11_icon, LV_ALIGN_TOP_MID, \
        0, 198);

    u8 HFes_month = \
        HcalendarInfo.HFesInfo[Type].HFes_month;
    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER; 
    widget_label_para.label_text_color = \
        lv_color_hex(0x666666);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        get_lang_txt_with_id(\
            lang_txtid_Muharram + HFes_month - 1);
    lv_obj_t *Hcalendar_month_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(Hcalendar_month_label, LV_ALIGN_TOP_MID, \
        0, 274);

    u16 HFes_year = \
        HcalendarInfo.HFesInfo[Type].HFes_year;
    char HcaleFesYearStr[5];
    memset(HcaleFesYearStr, 0, sizeof(HcaleFesYearStr));
    sprintf(HcaleFesYearStr, "%d", HFes_year);
    num_str_para.parent = \
        obj;
    num_str_para.num_obj_x = \
        156;
    num_str_para.num_obj_y = \
        328;
    num_str_para.p_num_str = \
        HcaleFesYearStr;
    num_str_para.num_str_len = \
        strlen(HcaleFesYearStr);
    num_str_para.num_obj = \
        NULL;
    num_str_para.num_obj_max = \
        0;
    num_str_para.num_dsc_idx = \
        NULL;
    num_str_para.file_00_index = \
        comm_num_14x22_wh_00_index;
    common_widget_num_str_create(&num_str_para);

    u16 S_GFes_year = \
        HcalendarInfo.GFesInfoS[Type].GFes_year;
    u8  S_GFes_month = \
        HcalendarInfo.GFesInfoS[Type].GFes_month;
    u8  S_GFes_day = \
        HcalendarInfo.GFesInfoS[Type].GFes_day;
    u16 E_GFes_year = \
        HcalendarInfo.GFesInfoE[Type].GFes_year;
    u8  E_GFes_month = \
        HcalendarInfo.GFesInfoE[Type].GFes_month;
    u8  E_GFes_day = \
        HcalendarInfo.GFesInfoE[Type].GFes_day;
    
    char GcaleFesDateStr[14];
    memset(GcaleFesDateStr, 0, sizeof(GcaleFesDateStr));
    sprintf(GcaleFesDateStr, "%02dD  %02dM  %d", \
        S_GFes_day, S_GFes_month, S_GFes_year);
    widget_label_para.label_w = \
        300;
    widget_label_para.label_h = \
        28;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = \
        LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        &font_common_num_24;
    widget_label_para.label_parent = \
        obj;
    widget_label_para.label_text = \
        GcaleFesDateStr;
    lv_obj_t *Gcale_fes_sd_label = \
        common_widget_label_create(&widget_label_para);
    lv_obj_align(Gcale_fes_sd_label, LV_ALIGN_TOP_MID, \
        0, 370);

    if(edyf)
    { 
        memset(GcaleFesDateStr, 0, sizeof(GcaleFesDateStr));
        sprintf(GcaleFesDateStr, "%02dD  %02dM  %d", \
            E_GFes_day, E_GFes_month, E_GFes_year);
        widget_label_para.label_text = \
            GcaleFesDateStr;
        lv_obj_t *Gcale_fes_ed_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(Gcale_fes_ed_label, LV_ALIGN_TOP_MID, \
            0, 403);

        widget_img_para.img_x = 70;
        widget_img_para.img_y = 382;
        widget_img_para.img_parent = \
            obj;
        widget_img_para.file_img_dat = \
            hijri_calendar_icon_05_index;
        widget_img_para.img_click_attr = \
            false;
        widget_img_para.event_cb = NULL;
        common_widget_img_create(&widget_img_para, NULL);
    }

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_Hcalendar_remind) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = false,
    .menu_id = \
        ui_act_id_Hcalendar_remind,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
