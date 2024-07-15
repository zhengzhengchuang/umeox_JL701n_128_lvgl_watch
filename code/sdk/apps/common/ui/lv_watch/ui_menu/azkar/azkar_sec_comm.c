#include "azkar_sec_comm.h"

static lv_obj_t *tileview;
static lv_obj_t *elem_container[Elem_Num];

static int8_t load_sidx;
static int8_t load_eidx;
static const uint8_t load_range = 3;

static void tileview_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *act = lv_tileview_get_tile_act(obj);
    
    uint8_t i;
    uint8_t sec_num = GetAzkarSecNum();
    for(i = 0; i < sec_num; i++)
    {
        if(act == elem_container[i])
            break;
    }
    SetAzkarSecIdx(i);
    printf("%s:%d\n", __func__, i);



    return;
}

static void tileview_create(lv_obj_t *obj)
{
    uint8_t sec_num = GetAzkarSecNum();

    /*tileview 创建*/
    tileview = lv_tileview_create(obj);
    lv_obj_set_size(tileview, LCD_WIDTH, LCD_HEIGHT);
    lv_obj_center(tileview);
    lv_obj_set_style_bg_color(tileview, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);
    lv_obj_remove_style(tileview, NULL, LV_PART_SCROLLBAR|LV_STATE_PRESSED);
    for(uint8_t i = 0; i < sec_num; i++)
    {
        elem_container[i] = lv_tileview_add_tile(tileview, i, 0, LV_DIR_HOR);
        lv_obj_set_scrollbar_mode(elem_container[i], LV_SCROLLBAR_MODE_OFF);
        lv_obj_set_style_pad_bottom(elem_container[i], 30, LV_PART_MAIN);
        lv_obj_update_layout(tileview);
    }      
    u8 idx = GetAzkarSecIdx();
    lv_obj_set_tile_id(tileview, idx, 0, LV_ANIM_OFF);
    lv_obj_add_event_cb(tileview, tileview_cb, LV_EVENT_VALUE_CHANGED, NULL);

    return;
}

static void elem_ctx_create(void)
{ 
    /* 元素创建 */
    int16_t sec_pos0 = 60;
    int16_t sec_pos1 = 100;

    char freq_str[8];

    lv_img_dsc_t *img_dsc;

    uint32_t file = GetAzkarStartFile();

    uint8_t sec_num = GetAzkarSecNum();
    for(uint8_t i = 0; i < sec_num; i++)
    {
        int freq_num = GetAzkarFreq(i);
        if(freq_num > 0) 
        {
            memset(freq_str, 0, sizeof(freq_str));
            sprintf(freq_str, "[%d]", freq_num);
            widget_label_para.label_w = 60;
            widget_label_para.label_h = 28;
            widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
            widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
            widget_label_para.label_text_color = lv_color_hex(0xf0d790);
            widget_label_para.label_ver_center = false;
            widget_label_para.user_text_font = &font_common_num_24;
            widget_label_para.label_parent = elem_container[i];
            widget_label_para.label_text = freq_str;
            lv_obj_t *freq_label = common_widget_label_create(&widget_label_para);
            lv_obj_align(freq_label, LV_ALIGN_TOP_MID, 0, sec_pos0);
        }

        int16_t y;
        if(freq_num > 0)
            y = sec_pos1;
        else
            y = sec_pos0;

        widget_img_para.img_parent = elem_container[i];
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;

        uint8_t img_num = GetAzkarEnImgNum(i);
        for(uint8_t j = 0; j < img_num; j++)
        {
            widget_img_para.file_img_dat = file;
            lv_obj_t *img = common_widget_ex_img_create(&widget_img_para, NULL);
            lv_obj_align(img, LV_ALIGN_TOP_MID, 0, y);

            img_dsc = common_widget_img_open_ex_res(file);
            y += img_dsc->header.h;

            file++;
        }
    }

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    ui_act_id_t prev_act_id = ui_act_id_azkar_list;
    tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
        prev_act_id, ui_act_id_null, ui_act_id_azkar_sec_comm);

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

    uint8_t sec_num = GetAzkarSecNum();
    for(uint8_t i = 0; i < sec_num; i++)
        elem_container[i] = NULL;

    // uint8_t sec_idx = GetAzkarSecIdx();
    // load_sidx = sec_idx - load_range;
    // load_eidx = sec_idx + load_range;
    // load_sidx = load_sidx<0?0:load_sidx;
    // load_eidx = load_eidx>sec_num?sec_num:load_eidx;

    tileview_create(obj);

    elem_ctx_create();

    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    if(!obj) return;

    return;
}

register_ui_menu_load_info(\
    menu_load_azkar_sec_comm) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_azkar_sec_comm,
    .disable_te = true,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
