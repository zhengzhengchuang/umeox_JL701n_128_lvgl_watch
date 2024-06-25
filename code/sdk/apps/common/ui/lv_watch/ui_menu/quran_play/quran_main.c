#include "quran_main.h"

static lv_obj_t *PPImg;
static uint16_t PPImgDsc;
static lv_obj_t *ModeImg;
static uint16_t ModeImgDsc;

static lv_obj_t *MorImg;
static lv_obj_t *EveImg;
static lv_obj_t *SelArLabel;
static lv_obj_t *SelEnLabel;

static lv_obj_t *PrgSlider;
static lv_obj_t *CurTimeLabel;
static lv_obj_t *TotalTimeLabel;

static void quran_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_quran_list);

    return;
}

static void pp_cb(lv_event_t *e)
{
    if(!e) return;

    common_refresh_timer_re_run();

    QpPPHandle();

    return;
}

static void prev_cb(lv_event_t *e)
{
    if(!e) return;

    common_refresh_timer_re_run();

    QpPrevHandle();

    return;
}

static void next_cb(lv_event_t *e)
{
    if(!e) return;

    common_refresh_timer_re_run();

    QpNextHandle();

    return;
}

static void mode_cb(lv_event_t *e)
{
    if(!e) return;

    uint8_t NewCycleMode;
    uint8_t OldCycleMode = QpGetCycleMode();

    if(OldCycleMode == FCYCLE_ALL)
        NewCycleMode = FCYCLE_ONE;
    else if(OldCycleMode == FCYCLE_ONE)
        NewCycleMode = FCYCLE_RANDOM;
    else if(OldCycleMode == FCYCLE_RANDOM)
        NewCycleMode = FCYCLE_ALL;
    else
        NewCycleMode = FCYCLE_ALL;
    QpSetCycleMode(NewCycleMode);  

    uint32_t ModeImgFile;
    if(NewCycleMode == FCYCLE_ONE)
        ModeImgFile = quran_icon_10_index;
    else if(NewCycleMode == FCYCLE_RANDOM)
        ModeImgFile = quran_icon_11_index;
    else
        ModeImgFile = quran_icon_09_index;
    common_widget_img_replace_src(ModeImg, ModeImgFile, ModeImgDsc);

    return;
}

static void voice_cb(lv_event_t *e)
{
    if(!e) return;

    ui_menu_jump(ui_act_id_quran_volume);

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_mode_t ui_mode = p_ui_info_cache->ui_mode;
    if(ui_mode == ui_mode_watchface)
    {
        ui_act_id_t left_act_id = \
            ui_act_id_watchface;
        tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
            left_act_id, ui_act_id_null, ui_act_id_quran_main);
    }else
    {
        ui_act_id_t prev_act_id = \
            ui_act_id_menu;
        if(!lang_txt_is_arabic())
            tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
                prev_act_id, ui_act_id_null, ui_act_id_quran_main);
        else
            tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
                ui_act_id_null, prev_act_id, ui_act_id_quran_main);
    }

    return;
}

static void menu_destory_cb(lv_obj_t *obj)
{
    return;
}

static void menu_refresh_cb(lv_obj_t *obj)
{
    if(!obj) return;

    int8_t SelIdx = QpUser.SelIdx;

    /*播放暂停更新*/
    uint8_t PPState;
    uint32_t PPImgFile;
    if(QpIsRunning() == true)
        PPState = 1; 
    else
        PPState = 0;
    if(PPState == 1) 
        PPImgFile = quran_icon_17_index; 
    else
        PPImgFile = quran_icon_16_index;
    common_widget_img_replace_src(PPImg, PPImgFile, PPImgDsc);

    if(QpIsRunning() == false)
        return;

    if(SelIdx == QpSelIdxInvalid)
    {
        lv_obj_add_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(EveImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);
    }else if(SelIdx == Qp_Mor_R)
    {
        lv_obj_add_flag(EveImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
    }else if(SelIdx == Qp_Eve_R)
    {
        lv_obj_add_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(EveImg, LV_OBJ_FLAG_HIDDEN);
    }else
    {
        lv_obj_add_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(EveImg, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(SelArLabel, quran_table_ar[SelIdx]);

        lv_obj_clear_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(SelEnLabel, quran_table_en[SelIdx]);
    }

    /*当前时间*/
    int CurTime = QpGetDecCurTime();
    u8 CurMinute = CurTime/60;
    u8 CurSecond = CurTime%60;
    char CurTimeStr[10];
    memset(CurTimeStr, 0, sizeof(CurTimeStr));
    sprintf(CurTimeStr, "%d:%02d", CurMinute, CurSecond);
    lv_label_set_text(CurTimeLabel, CurTimeStr);

    /*总时间*/
    int TotalTime = QpGetDecTotalTime();
    u8 TotalMinute = TotalTime/60;
    u8 TotalSecond = TotalTime%60;
    char TotalTimeStr[10];
    memset(TotalTimeStr, 0, sizeof(TotalTimeStr));
    sprintf(TotalTimeStr, "%d:%02d", TotalMinute, TotalSecond);
    lv_label_set_text(TotalTimeLabel, TotalTimeStr);

    /*进度条*/
    int32_t PrgMin = 0;
    int32_t PrgMax = 10000;
    int32_t PrgCur;
    if(TotalTime == 0)
        PrgCur = PrgMin;
    else
        PrgCur = (CurTime*PrgMax)/TotalTime;
    PrgCur = PrgCur<0?0:PrgCur;
    PrgCur = PrgCur>PrgMax?PrgMax:PrgCur;
    lv_slider_set_value(PrgSlider, PrgCur, LV_ANIM_OFF);

    return;
}

static void menu_display_cb(lv_obj_t *obj)
{
    if(!obj) return;

    int8_t SelIdx = QpUser.SelIdx;

    /*标题*/
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = quran_icon_06_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    MorImg = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(MorImg, LV_ALIGN_TOP_MID, 0, 44);

    widget_img_para.file_img_dat = quran_icon_07_index;
    EveImg = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(EveImg, LV_ALIGN_TOP_MID, 0, 44);

    widget_label_para.label_w = 180;
    widget_label_para.label_h = Label_Line_Height;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0xffffff);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = NULL;
    widget_label_para.label_parent = obj;
    if(SelIdx >= 0 && SelIdx < QpMaxNum)
        widget_label_para.label_text = quran_table_ar[SelIdx];
    else
        widget_label_para.label_text = NULL;
    SelArLabel = common_widget_label_create(&widget_label_para);
    lv_obj_align(SelArLabel, LV_ALIGN_TOP_MID, 0, 32);

    widget_label_para.label_w = 180;
    widget_label_para.label_h = 41;
    widget_label_para.long_mode = LV_LABEL_LONG_SCROLL;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0x666666);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = &font_common_30_no_ch;
    widget_label_para.label_parent = obj;
    if(SelIdx >= 0 && SelIdx < QpMaxNum)
        widget_label_para.label_text = quran_table_en[SelIdx];
    else
        widget_label_para.label_text = NULL;
    SelEnLabel = common_widget_label_create(&widget_label_para);
    lv_obj_align(SelEnLabel, LV_ALIGN_TOP_MID, 0, 76);

    if(SelIdx == QpSelIdxInvalid)
    {
        lv_obj_add_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(EveImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);
    }else if(SelIdx == Qp_Mor_R)
    {
        lv_obj_add_flag(EveImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
    }else if(SelIdx == Qp_Eve_R)
    {
        lv_obj_add_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(EveImg, LV_OBJ_FLAG_HIDDEN);
    }else
    {
        lv_obj_add_flag(MorImg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(EveImg, LV_OBJ_FLAG_HIDDEN);

        lv_obj_clear_flag(SelArLabel, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(SelEnLabel, LV_OBJ_FLAG_HIDDEN);
    }

    /*古兰经列表图标*/
    widget_img_para.img_x = 24;
    widget_img_para.img_y = 130;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = quran_icon_13_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = quran_cb;
    lv_obj_t *QpListIcon = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(QpListIcon, 20);

    /*当前时间*/
    int CurTime;
    if(SelIdx == QpSelIdxInvalid)
        CurTime = 0;
    else
        CurTime = QpGetDecCurTime();
    u8 CurMinute = CurTime/60;
    u8 CurSecond = CurTime%60;
    char CurTimeStr[10];
    memset(CurTimeStr, 0, sizeof(CurTimeStr));
    sprintf(CurTimeStr, "%d:%02d", CurMinute, CurSecond);
    widget_label_para.label_x = 68;
    widget_label_para.label_y = 132;
    widget_label_para.label_w = 52;
    widget_label_para.label_h = 28;
    widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
    widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
    widget_label_para.label_text_color = lv_color_hex(0x666666);
    widget_label_para.label_ver_center = false;
    widget_label_para.user_text_font = &font_common_num_24;
    widget_label_para.label_parent = obj;
    widget_label_para.label_text = CurTimeStr;
    CurTimeLabel = common_widget_label_create(&widget_label_para);

    /*总时间*/
    int TotalTime;
    if(SelIdx == QpSelIdxInvalid)
        TotalTime = 0;
    else
        TotalTime = QpGetDecTotalTime();
    u8 TotalMinute = TotalTime/60;
    u8 TotalSecond = TotalTime%60;
    char TotalTimeStr[10];
    memset(TotalTimeStr, 0, sizeof(TotalTimeStr));
    sprintf(TotalTimeStr, "%d:%02d", TotalMinute, TotalSecond);
    widget_label_para.label_x = 300;
    widget_label_para.label_text = TotalTimeStr;
    TotalTimeLabel = common_widget_label_create(&widget_label_para);

    /*进度条*/
    int32_t PrgMin = 0;
    int32_t PrgMax = 10000;
    int32_t PrgCur;
    if(TotalTime == 0)
        PrgCur = PrgMin;
    else
        PrgCur = (CurTime*PrgMax)/TotalTime;
    PrgCur = PrgCur<0?0:PrgCur;
    PrgCur = PrgCur>PrgMax?PrgMax:PrgCur;
    widget_slider_para.slider_parent = obj;
    widget_slider_para.slider_width = 164;
    widget_slider_para.slider_height = 6;
    widget_slider_para.slider_min_value = PrgMin;
    widget_slider_para.slider_max_value = PrgMax;
    widget_slider_para.slider_cur_value = PrgCur;
    widget_slider_para.slider_main_color = lv_color_hex(0xB28146);
    widget_slider_para.slider_indic_color = lv_color_hex(0xF0D990);
    widget_slider_para.slider_knob_opax = LV_OPA_100;
    widget_slider_para.slider_knob_color = lv_color_hex(0xF0D990);
    widget_slider_para.event_cb = NULL;
    widget_slider_para.user_data = NULL;
    PrgSlider = common_widget_slider_create(&widget_slider_para);
    lv_obj_align_to(PrgSlider, CurTimeLabel, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
    lv_obj_clear_flag(PrgSlider, LV_OBJ_FLAG_CLICKABLE);
 
    /*背景*/
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = quran_icon_08_index;
    widget_img_para.img_click_attr = false;
    widget_img_para.event_cb = NULL;
    lv_obj_t *MainBgImg = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_align(MainBgImg, LV_ALIGN_TOP_MID, 0, 172);

    /*播放状态*/
    uint8_t PPState;
    uint32_t PPImgFile;
    if(QpIsRunning() == true)
        PPState = 1; 
    else
        PPState = 0;
    widget_img_para.img_x = 156;
    widget_img_para.img_y = 220;
    widget_img_para.img_parent = obj;
    if(PPState == 1)
        PPImgFile = quran_icon_17_index;
    else
        PPImgFile = quran_icon_16_index;
    widget_img_para.file_img_dat = PPImgFile;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = pp_cb;
    PPImg = common_widget_img_create(&widget_img_para, &PPImgDsc);
    lv_obj_set_ext_click_area(PPImg, 20);

    /*上一曲*/
    widget_img_para.img_x = 24;
    widget_img_para.img_y = 220;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = quran_icon_18_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = prev_cb;
    lv_obj_t *PrevImg = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(PrevImg, 20);

    /*下一曲*/
    widget_img_para.img_x = 288;
    widget_img_para.img_y = 220;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = quran_icon_19_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = next_cb;
    lv_obj_t *NextImg = \
        common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(NextImg, 20);

    /*播放模式*/
    uint8_t CycleMode = QpGetCycleMode();
    uint32_t ModeImgFile;
    if(CycleMode == FCYCLE_ONE)
        ModeImgFile = quran_icon_10_index;
    else if(CycleMode == FCYCLE_RANDOM)
        ModeImgFile = quran_icon_11_index;
    else
        ModeImgFile = quran_icon_09_index;
    widget_img_para.img_x = 44;
    widget_img_para.img_y = 388;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = ModeImgFile;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = mode_cb;
    ModeImg = common_widget_img_create(&widget_img_para, &ModeImgDsc);
    lv_obj_set_ext_click_area(ModeImg, 20);

    /*声音*/
    widget_img_para.img_x = 288;
    widget_img_para.img_y = 388;
    widget_img_para.img_parent = obj;
    widget_img_para.file_img_dat = quran_icon_12_index;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = voice_cb;
    lv_obj_t *VoiceImg = common_widget_img_create(&widget_img_para, NULL);
    lv_obj_set_ext_click_area(VoiceImg, 20);

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_quran_main) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_quran_main,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 200,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
