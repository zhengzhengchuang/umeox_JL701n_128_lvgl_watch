#include "azkar_sec_ar.h"

static lv_obj_t *tileview;
static lv_obj_t *tv_tile[Elem_Num];
static lv_obj_t *elem_container[Elem_Num];

static void tileview_cb(lv_event_t *e)
{
    if(!e) return;

    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *act = lv_tileview_get_tile_act(obj);
    
    uint8_t i;
    uint8_t sec_num = GetAzkarSecNum();
    for(i = 0; i < sec_num; i++)
    {
        if(act == tv_tile[i])
            break;
    }
    SetAzkarSecIdx(i);
    
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
        tv_tile[sec_num - 1 - i] = lv_tileview_add_tile(tileview, i, 0, LV_DIR_HOR);

        widget_obj_para.obj_parent = tv_tile[sec_num - 1 - i];
        widget_obj_para.obj_width = LCD_WIDTH;
        widget_obj_para.obj_height = 388;
        widget_obj_para.obj_bg_opax = LV_OPA_0;
        widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
        widget_obj_para.obj_border_opax = LV_OPA_0;
        widget_obj_para.obj_border_width = 0;
        widget_obj_para.obj_border_color = lv_color_hex(0x000000);
        widget_obj_para.obj_radius = 0;
        widget_obj_para.obj_is_scrollable = true;
        elem_container[sec_num - 1 - i] = common_widget_obj_create(&widget_obj_para);
        lv_obj_align(elem_container[sec_num - 1 - i], LV_ALIGN_TOP_MID, 0, 0);
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
 
    char freq_str[8];

    lv_img_dsc_t *img_dsc;
    uint8_t sec_num = GetAzkarSecNum();
    uint32_t file = GetAzkarStartFile();

    for(uint8_t i = 0; i < sec_num; i++)
    {
        int freq_num = GetAzkarFreq(i);
        if(freq_num > 0) 
        {
            memset(freq_str, 0, sizeof(freq_str));
            sprintf(freq_str, "[%d]", freq_num);
            widget_label_para.label_w = 60;
            widget_label_para.label_h = 37;
            widget_label_para.long_mode = LV_LABEL_LONG_CLIP;
            widget_label_para.text_align = LV_TEXT_ALIGN_CENTER;
            widget_label_para.label_text_color = lv_color_hex(0xf0d790);
            widget_label_para.label_ver_center = false;
            widget_label_para.user_text_font = &font_common_num_32;
            widget_label_para.label_parent = tv_tile[i];
            widget_label_para.label_text = freq_str;
            lv_obj_t *freq_label = common_widget_label_create(&widget_label_para);
            lv_obj_align(freq_label, LV_ALIGN_BOTTOM_MID, 0, -20);
        }

        int16_t y = sec_pos0;

        widget_img_para.img_parent = elem_container[i];
        widget_img_para.img_click_attr = false;
        widget_img_para.event_cb = NULL;

        uint8_t img_num = GetAzkarImgNum(i);
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
        ui_act_id_null, prev_act_id, ui_act_id_azkar_sec_ar);

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
    {
        tv_tile[i] = NULL;
        elem_container[i] = NULL;
    }
        
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
    menu_load_azkar_sec_ar) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_azkar_sec_ar,
    .disable_te = true,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
