#include "Hcalendar_festivals.h"

static int16_t scroll_y;
static int16_t scroll_top_y;
static int16_t scroll_bottom_y;
static lv_obj_t *list_ctx_container;

static const u16 ec_h_0 = 285;
static const u16 ec_h_1 = 318;
static const uint8_t Hcale_fes_dysnum[\
    HcalendarFesNum] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
};
static const bool Hcale_fes_edyf[\
    HcalendarFesNum] = {
    0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,  
};

static void enable_cb(lv_event_t *e)
{
    if(!e) return;

    bool *FesRemindersFlag = \
        &HcalendarInfo.FesRemindersFlag;
    if(*FesRemindersFlag == false)
        *FesRemindersFlag = true;
    else
        *FesRemindersFlag = false;
    
    HcalendarInfoParaWrite();

    ui_menu_jump(\
        ui_act_id_Hcalendar_festivals);

    return;
}

static void enable_create(lv_obj_t *obj, uint8_t menu_align)
{
    bool FesRemindersFlag = \
        HcalendarInfo.FesRemindersFlag;

    widget_img_para.img_parent = \
        obj;
    widget_img_para.file_img_dat = \
        comm_icon_05_index;
    widget_img_para.img_click_attr = \
        false;
    widget_img_para.event_cb = \
        NULL;
    lv_obj_t *enable_container = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(enable_container, LV_ALIGN_TOP_MID, 0, 52);

    widget_label_para.label_parent = \
        enable_container;
    widget_label_para.label_w = \
        180;
    widget_label_para.label_h = \
        Label_Line_Height;
    widget_label_para.long_mode = \
        LV_LABEL_LONG_SCROLL;
    if(menu_align == menu_align_right)
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_RIGHT;
    else
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_LEFT;
    widget_label_para.label_text_color = \
        lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = \
        false;
    widget_label_para.user_text_font = \
        NULL;
    widget_label_para.label_text = \
        get_lang_txt_with_id(lang_txtid_festival_reminder);
    lv_obj_t *enable_label = \
        common_widget_label_create(&widget_label_para);
    if(menu_align == menu_align_right)
        lv_obj_align(enable_label, LV_ALIGN_RIGHT_MID, -20, 0);
    else
        lv_obj_align(enable_label, LV_ALIGN_LEFT_MID, 20, 0);

    widget_img_para.img_parent = \
        enable_container;
    widget_img_para.file_img_dat = \
        comm_icon_00_index + FesRemindersFlag;
    widget_img_para.img_click_attr = \
        true;
    widget_img_para.event_cb = \
        enable_cb;
    widget_img_para.user_data = \
        NULL;  
    lv_obj_t *enable_switch = \
        common_widget_img_create(&widget_img_para, NULL);
    if(menu_align == menu_align_right)
        lv_obj_align(enable_switch, LV_ALIGN_LEFT_MID, 20, 0);
    else
        lv_obj_align(enable_switch, LV_ALIGN_RIGHT_MID, -20, 0);
    lv_obj_set_ext_click_area(enable_switch, 20);

    return;
}

static void list_ctx_container_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    scroll_y = lv_obj_get_scroll_y(obj);

    return;
}

static void list_ctx_container_create(lv_obj_t *obj)
{
    widget_obj_para.obj_parent = \
        obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = \
        140;
    widget_obj_para.obj_width = \
        LCD_WIDTH;
    widget_obj_para.obj_height = \
        LCD_HEIGHT - 140;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        true;
    list_ctx_container = \
        common_widget_obj_create(&widget_obj_para);
    lv_obj_set_style_pad_bottom(list_ctx_container, \
        25, LV_PART_MAIN);
    lv_obj_add_event_cb(list_ctx_container, \
        list_ctx_container_cb, LV_EVENT_SCROLL, NULL);

    return;
}

static void elem_ctx_create(uint8_t menu_align)
{
    int16_t ctx_w = \
        LCD_WIDTH;
    widget_obj_para.obj_parent = \
        list_ctx_container;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_width = \
        ctx_w;
    widget_obj_para.obj_bg_opax = \
        LV_OPA_0;
    widget_obj_para.obj_bg_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = \
        LV_OPA_0;
    widget_obj_para.obj_border_width = \
        0;
    widget_obj_para.obj_border_color = \
        lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = \
        false;

    int16_t y = 0;
    char HcaleDayStr[3];
    char HcaleYearStr[5];
    char GcaleDateStr[14];
    for(uint8_t i = Hijri_New_Year; i < HcalendarFesNum; i++)
    {
        widget_obj_para.obj_y = y;
        if(Hcale_fes_edyf[i])
            widget_obj_para.obj_height = ec_h_1;
        else
            widget_obj_para.obj_height = ec_h_0;
        lv_obj_t *elem_ctx_container = \
            common_widget_obj_create(&widget_obj_para);
        
        if(Hcale_fes_edyf[i])
            y += ec_h_1;
        else
            y += ec_h_0;

        widget_label_para.label_w = \
            300;
        widget_label_para.label_h = \
            Label_Line_Height*2;
        widget_label_para.long_mode = \
            LV_LABEL_LONG_WRAP;
        widget_label_para.text_align = \
            LV_TEXT_ALIGN_CENTER;
        widget_label_para.label_text_color = \
            lv_color_hex(0xf0d790);
        widget_label_para.label_ver_center = \
            true;
        widget_label_para.user_text_font = \
            NULL;
        widget_label_para.label_parent = \
            elem_ctx_container;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_Hijri_New_Year + i);
        lv_obj_t *Hcale_fes_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(Hcale_fes_label, LV_ALIGN_TOP_MID, \
            0, 0);
   
        u8 dysnum = \
            Hcale_fes_dysnum[i];
        for(uint8_t j = 0; j < dysnum; j++)
        {
            widget_img_para.img_parent = \
                elem_ctx_container;
            widget_img_para.file_img_dat = \
                hijri_calendar_icon_04_index;
            widget_img_para.img_click_attr = \
                false;
            widget_img_para.event_cb = \
                NULL;
            widget_img_para.img_y = \
                90;
            widget_img_para.img_x = \
                (ctx_w - dysnum*80)/2 + j*80;
            lv_obj_t *Hcale_day_container = \
                common_widget_img_create(&widget_img_para, NULL);

            memset(HcaleDayStr, 0, sizeof(HcaleDayStr));
            sprintf(HcaleDayStr, "%02d", \
                HcalendarInfo.HFesInfo[i].HFes_day + j);
            num_str_para.parent = \ 
                Hcale_day_container;
            num_str_para.num_obj_x = \
                24;
            num_str_para.num_obj_y = \
                28;
            num_str_para.p_num_str = \
                HcaleDayStr;
            num_str_para.num_str_len = \
                strlen(HcaleDayStr);
            num_str_para.num_obj = \
                NULL;
            num_str_para.num_obj_max = \
                0;
            num_str_para.num_dsc_idx = \
                NULL;
            num_str_para.file_00_index = \
                comm_num_16x28_ye_00_index;
            common_widget_num_str_create(&num_str_para);
        }
        
        u8 HFes_month = \
            HcalendarInfo.HFesInfo[i].HFes_month;
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
            elem_ctx_container;
        widget_label_para.label_text = \
            get_lang_txt_with_id(lang_txtid_Muharram + HFes_month - 1);
        lv_obj_t *Hcale_fes_month_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(Hcale_fes_month_label, LV_ALIGN_TOP_MID, \
            0, 165);

        u16 HFes_year = \
            HcalendarInfo.HFesInfo[i].HFes_year;
        memset(HcaleYearStr, 0, sizeof(HcaleYearStr));
        sprintf(HcaleYearStr, "%d", HFes_year);
        num_str_para.parent = \
            elem_ctx_container;
        num_str_para.num_obj_x = \
            156;
        num_str_para.num_obj_y = \
            215;
        num_str_para.p_num_str = \
            HcaleYearStr;
        num_str_para.num_str_len = \
            strlen(HcaleYearStr);
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
            HcalendarInfo.GFesInfoS[i].GFes_year;
        u8  S_GFes_month = \
            HcalendarInfo.GFesInfoS[i].GFes_month;
        u8  S_GFes_day = \
            HcalendarInfo.GFesInfoS[i].GFes_day;
        u16 E_GFes_year = \
            HcalendarInfo.GFesInfoE[i].GFes_year;
        u8  E_GFes_month = \
            HcalendarInfo.GFesInfoE[i].GFes_month;
        u8  E_GFes_day = \
            HcalendarInfo.GFesInfoE[i].GFes_day;
        memset(GcaleDateStr, 0, sizeof(GcaleDateStr));
        sprintf(GcaleDateStr, "%02dD  %02dM  %d", \
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
            elem_ctx_container;
        widget_label_para.label_text = \
            GcaleDateStr;
        lv_obj_t *Gcale_fes_sd_label = \
            common_widget_label_create(&widget_label_para);
        lv_obj_align(Gcale_fes_sd_label, LV_ALIGN_TOP_MID, \
            0, 245);

        if(Hcale_fes_edyf[i])
        {
            memset(GcaleDateStr, 0, sizeof(GcaleDateStr));
            sprintf(GcaleDateStr, "%02dD  %02dM  %d", \
                E_GFes_day, E_GFes_month, E_GFes_year);
            widget_label_para.label_text = \
                GcaleDateStr;
            lv_obj_t *Gcale_fes_ed_label = \
                common_widget_label_create(&widget_label_para);
            lv_obj_align(Gcale_fes_ed_label, LV_ALIGN_TOP_MID, \
                0, 278);

            widget_img_para.img_x = 70;
            widget_img_para.img_y = 257;
            widget_img_para.img_parent = \
                elem_ctx_container;
            widget_img_para.file_img_dat = \
                hijri_calendar_icon_05_index;
            widget_img_para.img_click_attr = \
                false;
            widget_img_para.event_cb = NULL;
            common_widget_img_create(&widget_img_para, NULL);
        }
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = \
        ui_act_id_Hcalendar_main;
    if(!lang_txt_is_arabic())
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            prev_act_id, ui_act_id_null, ui_act_id_Hcalendar_festivals);
    else
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            ui_act_id_null, prev_act_id, ui_act_id_Hcalendar_festivals);
            
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

    enable_create(obj, menu_align);

    list_ctx_container_create(obj);

    elem_ctx_create(menu_align);

    lv_obj_scroll_to_y(list_ctx_container, 0, LV_ANIM_OFF);
    lv_obj_update_layout(list_ctx_container);
    scroll_top_y = lv_obj_get_scroll_top(list_ctx_container);
    scroll_bottom_y = lv_obj_get_scroll_bottom(list_ctx_container);
    scroll_y = scroll_y<scroll_top_y?scroll_top_y:scroll_y;
    scroll_y = scroll_y>scroll_bottom_y?scroll_bottom_y:scroll_y;
    lv_obj_scroll_to_y(list_ctx_container, scroll_y, LV_ANIM_OFF);

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
    if(lv_anim_get(list_ctx_container, NULL))
        return;

    int16_t scroll_y_tmp = scroll_y;

    if(state == Rdec_Forward)
        scroll_y_tmp += 154;
    else if(state == Rdec_Backward)
        scroll_y_tmp -= 154;
    else
        return;

    scroll_y_tmp = scroll_y_tmp < scroll_top_y?scroll_top_y:scroll_y_tmp;
    scroll_y_tmp = scroll_y_tmp > scroll_bottom_y?scroll_bottom_y:scroll_y_tmp;
    if(scroll_y_tmp != scroll_y)
        lv_obj_scroll_to_y(list_ctx_container, scroll_y_tmp, LV_ANIM_ON);
    
    return;
}

register_ui_menu_load_info(\
    menu_load_Hcalendar_festivals) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_Hcalendar_festivals,
    .disable_te = true,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .rdec_func_cb = menu_rdec_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
