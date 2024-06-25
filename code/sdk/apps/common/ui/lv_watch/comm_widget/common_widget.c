#include "common_widget.h"
#include "../comm_lang/lang_conf.h"


/*********************************************************************************
                                  弧控件                                       
*********************************************************************************/
common_widget_arc_para_t widget_arc_para = {0};
lv_obj_t *common_widget_arc_create(common_widget_arc_para_t *arc_para)
{
    if(!arc_para) 
        return NULL;

    lv_obj_t *common_widget_arc = \
        lv_arc_create(arc_para->arc_parent);
    lv_obj_set_size(common_widget_arc, \
        arc_para->arc_bg_width, arc_para->arc_bg_height);
    lv_obj_set_pos(common_widget_arc, \
        arc_para->arc_x, arc_para->arc_y);
    lv_obj_set_style_bg_opa(common_widget_arc, \
        arc_para->arc_bg_opax, LV_PART_MAIN);
    lv_arc_set_bg_angles(common_widget_arc, \
        arc_para->arc_main_start_angle, arc_para->arc_main_end_angle);
    lv_arc_set_angles(common_widget_arc, \
        arc_para->arc_indic_start_angle, arc_para->arc_indic_end_angle);
    lv_arc_set_rotation(common_widget_arc, \
        arc_para->arc_rotation_angle);
    lv_arc_set_range(common_widget_arc, \
        arc_para->arc_min_value, arc_para->arc_max_value);
    lv_arc_set_value(common_widget_arc, \
        arc_para->arc_cur_value);
    lv_obj_set_style_arc_width(common_widget_arc, \
        arc_para->arc_main_line_width, LV_PART_MAIN);
    lv_obj_set_style_arc_width(common_widget_arc, \
        arc_para->arc_indic_line_width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(common_widget_arc, \
        arc_para->arc_main_line_color, LV_PART_MAIN);
    lv_obj_set_style_arc_color(common_widget_arc, \
        arc_para->arc_indic_line_color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(common_widget_arc, \
        arc_para->arc_main_is_rounded, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(common_widget_arc, \
        arc_para->arc_indic_is_rounded, LV_PART_INDICATOR);

    if(arc_para->arc_click_is_clear)
        lv_obj_clear_flag(common_widget_arc, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_remove_style(common_widget_arc, NULL, LV_PART_KNOB);

    return common_widget_arc;
}



/*********************************************************************************
                                  图片控件                                       
*********************************************************************************/
static user_img_dsc_t user_img_dsc = {0};
common_widget_img_para_t widget_img_para = {0};
lv_obj_t *common_widget_img_create(common_widget_img_para_t *img_para, \
    uint16_t *img_dsc_idx)
{
    if(!img_para)
        return NULL;

    uint8_t img_dst_cnt = \
        user_img_dsc.img_dst_cnt;
    lv_img_dsc_t *img_dst_gather = \
        user_img_dsc.img_dst_gather;

    if(img_dst_cnt >= Img_Dsc_Max)
    {
        printf("error!!!!!!!!!!!! Img_Dsc_Max\n");
        return NULL;
    }

    if(!(img_dst_gather[img_dst_cnt].data) && \
        img_para->file_img_dat != File_Img_Dat_None)
    {
        open_fd("usr_nor");
        lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, file_index[img_para->file_img_dat], \
            &img_dst_gather[img_dst_cnt]);
        close_fd();
    }
    
    lv_obj_t *common_widget_img = lv_img_create(img_para->img_parent);
    lv_img_set_src(common_widget_img, &img_dst_gather[img_dst_cnt]);
    lv_obj_set_pos(common_widget_img, img_para->img_x, img_para->img_y);

    if(img_para->img_click_attr)
        lv_obj_add_flag(common_widget_img, LV_OBJ_FLAG_CLICKABLE);
    else
        lv_obj_clear_flag(common_widget_img, LV_OBJ_FLAG_CLICKABLE);
    
    if(img_para->event_cb)
        lv_obj_add_event_cb(common_widget_img, img_para->event_cb, \
            LV_EVENT_SHORT_CLICKED, img_para->user_data);

    if(img_dsc_idx)
        *img_dsc_idx = img_dst_cnt;

    user_img_dsc.img_dst_cnt++;

    //printf("%d\n", user_img_dsc.img_dst_cnt);

    return common_widget_img;
}

lv_img_dsc_t *common_widget_img_open_res(uint32_t file_img_dat)
{
    static lv_img_dsc_t widget_img_dst;

    open_fd("usr_nor");
    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, file_index[file_img_dat], \
        &widget_img_dst);
    close_fd();

    return (&widget_img_dst);
}

void common_widget_img_replace_src(lv_obj_t *obj, uint32_t file_img_dat, \
    uint16_t img_dsc_idx)
{
    if(!obj) return;

    uint8_t img_dst_cnt = \
        user_img_dsc.img_dst_cnt;
    lv_img_dsc_t *img_dst_gather = \
        user_img_dsc.img_dst_gather;

    if(img_dsc_idx >= img_dst_cnt)
        return;

    open_fd("usr_nor");
    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, file_index[file_img_dat], \
        &img_dst_gather[img_dsc_idx]);
    close_fd();

    lv_img_set_src(obj, &img_dst_gather[img_dsc_idx]);

    return;
}

/*********************************************************************************
                                  标签控件                                       
*********************************************************************************/
common_widget_label_para_t widget_label_para = {0};

static const lv_font_t *sys_default_label_font(void)
{
    const lv_font_t *sel_font = \
        &font_common_32;

    return sel_font;
}

lv_obj_t *common_widget_label_create(common_widget_label_para_t *label_para)
{
    if(!label_para) 
        return NULL;

    lv_obj_t *common_widget_label = \
        lv_label_create(label_para->label_parent);
    lv_obj_set_pos(common_widget_label, \
        label_para->label_x, label_para->label_y);
    lv_label_set_text(common_widget_label, \
        label_para->label_text);
    lv_label_set_ver_center(common_widget_label, \
        label_para->label_ver_center);
    lv_label_set_long_mode(common_widget_label, \
        label_para->long_mode);
    lv_obj_set_size(common_widget_label, \
        label_para->label_w, label_para->label_h);
    lv_obj_set_style_text_align(common_widget_label, \
        label_para->text_align, LV_PART_MAIN);
    lv_obj_set_style_text_color(common_widget_label, \
        label_para->label_text_color, LV_PART_MAIN);
    if(label_para->user_text_font)
        lv_obj_set_style_text_font(common_widget_label, \
            label_para->user_text_font, LV_PART_MAIN);
    else
        lv_obj_set_style_text_font(common_widget_label, \
            sys_default_label_font(), LV_PART_MAIN);
  

    return common_widget_label;
}




/*********************************************************************************
                                  基容器控件                                       
*********************************************************************************/
common_widget_obj_para_t widget_obj_para = {0};
lv_obj_t *common_widget_obj_create(common_widget_obj_para_t *obj_para)
{
    if(!obj_para)
        return NULL;

    lv_obj_t *common_widget_obj = \
        lv_obj_create(obj_para->obj_parent);
    lv_obj_set_size(common_widget_obj, \
        obj_para->obj_width, obj_para->obj_height);
    lv_obj_set_pos(common_widget_obj, \
        obj_para->obj_x, obj_para->obj_y);
    lv_obj_set_style_bg_opa(common_widget_obj, \
        obj_para->obj_bg_opax, LV_PART_MAIN);
    lv_obj_set_style_bg_color(common_widget_obj, \
        obj_para->obj_bg_color, LV_PART_MAIN);

#if 0
    // if(obj_para->obj_bg_opax == LV_OPA_100)
    //     lv_obj_set_style_bg_color(common_widget_obj, obj_para->obj_bg_color, LV_PART_MAIN);
    // else if(obj_para->obj_bg_opax == LV_OPA_0)
    //     lv_obj_set_style_bg_opa(common_widget_obj, obj_para->obj_bg_opax, LV_PART_MAIN);
    // else
    // {
    //     lv_obj_set_style_bg_opa(common_widget_obj, obj_para->obj_bg_opax, LV_PART_MAIN);
    //     lv_obj_set_style_bg_color(common_widget_obj, obj_para->obj_bg_color, LV_PART_MAIN);
    // }
#endif

    if(obj_para->obj_border_width)
    {
        lv_obj_set_style_border_opa(common_widget_obj, \
            obj_para->obj_border_opax, LV_PART_MAIN);
        lv_obj_set_style_border_width(common_widget_obj, \
            obj_para->obj_border_width, LV_PART_MAIN);
        lv_obj_set_style_border_color(common_widget_obj, \
            obj_para->obj_border_color, LV_PART_MAIN);
    }else
        lv_obj_set_style_border_width(common_widget_obj, \
            obj_para->obj_border_width, LV_PART_MAIN);

    lv_obj_set_style_radius(common_widget_obj, \
        obj_para->obj_radius, LV_PART_MAIN);
    lv_obj_set_style_pad_all(common_widget_obj, \
        0, LV_PART_MAIN);

    if(!(obj_para->obj_is_scrollable))
        lv_obj_clear_flag(common_widget_obj, \
            LV_OBJ_FLAG_SCROLLABLE);
    else
        lv_obj_remove_style(common_widget_obj, \
            NULL, LV_PART_SCROLLBAR);

    return common_widget_obj;
}



/*********************************************************************************
                                  滑块控件                                       
*********************************************************************************/
common_widget_slider_para_t widget_slider_para = {0};
lv_obj_t *common_widget_slider_create(common_widget_slider_para_t *slider_para)
{
    if(!slider_para)
        return NULL;

    lv_obj_t *common_widget_slider = \
        lv_slider_create(slider_para->slider_parent);
    lv_obj_set_size(common_widget_slider, \
        slider_para->slider_width, slider_para->slider_height);
    lv_obj_set_pos(common_widget_slider, \
        slider_para->slider_x, slider_para->slider_y);
    lv_slider_set_range(common_widget_slider, \
        slider_para->slider_min_value, slider_para->slider_max_value);
    lv_slider_set_value(common_widget_slider, \
        slider_para->slider_cur_value, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(common_widget_slider, \
        slider_para->slider_main_color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(common_widget_slider, \
        slider_para->slider_indic_color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(common_widget_slider, \
        slider_para->slider_knob_opax, LV_PART_KNOB);
    if(slider_para->slider_knob_opax)
        lv_obj_set_style_bg_color(common_widget_slider, \
            slider_para->slider_knob_color, LV_PART_KNOB);

    if(slider_para->event_cb)
        lv_obj_add_event_cb(common_widget_slider, \
            slider_para->event_cb, LV_EVENT_VALUE_CHANGED, slider_para->user_data);

    return common_widget_slider;
}



/*********************************************************************************
                                  开关控件                                       
*********************************************************************************/
common_widget_switch_para_t widget_switch_para = {0};
lv_obj_t *common_widget_switch_create(common_widget_switch_para_t *switch_para)
{
    if(!switch_para)
        return NULL;

    lv_obj_t *common_widget_switch = \
        lv_switch_create(switch_para->switch_parent);
    lv_obj_set_size(common_widget_switch, \
        switch_para->switch_width, switch_para->switch_height);
    lv_obj_set_pos(common_widget_switch, \
        switch_para->switch_x, switch_para->switch_y);
    lv_obj_set_style_bg_color(common_widget_switch, \
        switch_para->switch_main_color, LV_PART_MAIN);
    lv_obj_set_style_bg_color(common_widget_switch, \
        switch_para->switch_indic_color, LV_PART_INDICATOR|LV_STATE_CHECKED);  

    if(switch_para->switch_cur_state)
        lv_obj_add_state(common_widget_switch, \
            LV_STATE_CHECKED);
    else
        lv_obj_clear_state(common_widget_switch, \
            LV_STATE_CHECKED);

    if(switch_para->event_cb)
        lv_obj_add_event_cb(common_widget_switch, \
            switch_para->event_cb, LV_EVENT_VALUE_CHANGED, switch_para->user_data);

    return common_widget_switch;
}



/*********************************************************************************
                                  滚轮控件                                       
*********************************************************************************/
common_widget_roller_para_t widget_roller_para = {0};
lv_obj_t *common_widget_roller_create(common_widget_roller_para_t *roller_para)
{
    if(!roller_para)
        return NULL;

    lv_obj_t *common_widget_roller = \
        lv_roller_create(roller_para->roller_parent);
    lv_obj_set_size(common_widget_roller, \
        roller_para->roller_width, roller_para->roller_height);
    lv_obj_set_pos(common_widget_roller, \
        roller_para->roller_x, roller_para->roller_y);
    lv_roller_set_options(common_widget_roller, \
        roller_para->roller_options, roller_para->roller_mode);
    lv_roller_set_selected(common_widget_roller, \
        roller_para->roller_sel, LV_ANIM_OFF);
    lv_obj_set_style_text_line_space(common_widget_roller, \
        roller_para->roller_line_inv, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(common_widget_roller, \
        roller_para->roller_main_bg_opax, LV_PART_MAIN);
    if(roller_para->roller_main_bg_opax)
        lv_obj_set_style_bg_color(common_widget_roller, \
            roller_para->roller_main_bg_color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(common_widget_roller, \
        roller_para->roller_selected_bg_opax, LV_PART_SELECTED);
    if(roller_para->roller_selected_bg_opax)
        lv_obj_set_style_bg_color(common_widget_roller, \
            roller_para->roller_selected_bg_color, LV_PART_SELECTED);

    if(roller_para->roller_border_width)
    {
        lv_obj_set_style_border_opa(common_widget_roller, \
            roller_para->roller_border_opax, LV_PART_MAIN);
        lv_obj_set_style_border_width(common_widget_roller, \
            roller_para->roller_border_width, LV_PART_MAIN);
        lv_obj_set_style_border_color(common_widget_roller, \
            roller_para->roller_border_color, LV_PART_MAIN);
    }else
        lv_obj_set_style_border_width(common_widget_roller, \
            roller_para->roller_border_width, LV_PART_MAIN);

    lv_obj_set_style_text_font(common_widget_roller, \
        roller_para->roller_main_text_font, LV_PART_MAIN);
    lv_obj_set_style_text_color(common_widget_roller, \
        roller_para->roller_main_text_color, LV_PART_MAIN);
    lv_obj_set_style_text_font(common_widget_roller, \
        roller_para->roller_selected_text_font, LV_PART_SELECTED);
    lv_obj_set_style_text_color(common_widget_roller, \
        roller_para->roller_selected_text_color, LV_PART_SELECTED);
    
    lv_roller_set_visible_row_count(common_widget_roller, \
        roller_para->roller_row_count);
    
    if(roller_para->event_cb)
        lv_obj_add_event_cb(common_widget_roller, \
            roller_para->event_cb, LV_EVENT_VALUE_CHANGED, roller_para->user_data);

    return common_widget_roller;
}



/*********************************************************************************
                                  按钮控件                                       
*********************************************************************************/
common_widget_button_para_t widget_button_para = {0};
lv_obj_t *common_widget_button_create(common_widget_button_para_t *button_para)
{
    if(!button_para)
        return NULL;

    lv_obj_t *common_widget_button = \
        lv_btn_create(button_para->button_parent);
    lv_obj_remove_style(common_widget_button, \
        NULL, LV_PART_MAIN|LV_STATE_PRESSED);
    lv_obj_set_size(common_widget_button, \
        button_para->button_width, button_para->button_height);
    lv_obj_set_pos(common_widget_button, \
        button_para->button_x, button_para->button_y);
    lv_obj_set_style_bg_opa(common_widget_button, \
        button_para->button_main_opax, LV_PART_MAIN);
    if(button_para->button_main_opax)
        lv_obj_set_style_bg_color(common_widget_button, \
            button_para->button_main_color, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(common_widget_button, \
        0, LV_PART_MAIN);
    lv_obj_set_style_radius(common_widget_button, \
        button_para->button_radius, LV_PART_MAIN);

    if(button_para->event_cb)
        lv_obj_add_event_cb(common_widget_button, \
            button_para->event_cb, LV_EVENT_SHORT_CLICKED, button_para->user_data);

    return common_widget_button;
}



/*********************************************************************************
                                  图表控件                                       
*********************************************************************************/
common_widget_chart_para_t widget_chart_para = {0};
static void chart_draw_part_begin(lv_event_t *e)
{
    if(!e) return;

    lv_obj_draw_part_dsc_t *dsc = \
        lv_event_get_param(e);
    common_widget_chart_para_t *user_data = \
        (common_widget_chart_para_t *)lv_event_get_user_data(e);

    if(dsc->part == LV_PART_TICKS && \
        dsc->id == LV_CHART_AXIS_PRIMARY_X) 
    {
        lv_snprintf(dsc->text, dsc->text_length, "%s", \
            user_data->chart_prix_label_str[dsc->value]);
    }

    if(dsc->part == LV_PART_TICKS && \
        dsc->id == LV_CHART_AXIS_PRIMARY_Y)
    {
#if 0
        int16_t range_radio = 1;

        if(user_data->chart_priy_major_cnt >= 2)
            range_radio = user_data->chart_priy_range_max/\
                (user_data->chart_priy_major_cnt-1);
        else
            return;

        printf("range_radio = %d\n", range_radio);
        printf("value = %d\n", dsc->value);

        lv_snprintf(dsc->text, dsc->text_length, "%s", \
            user_data->chart_priy_label_str[dsc->value/range_radio]);
#endif
    }

#if 0
    //这块透明度混合运算比较复杂，吃帧率
    lv_obj_t *obj = lv_event_get_target(e);
    if(dsc->part == LV_PART_ITEMS && lv_chart_get_type(obj) == LV_CHART_TYPE_LINE)
    {
        if(!dsc->p1 || !dsc->p2) return;

        /*Add a line mask that keeps the area below the line*/
        lv_draw_mask_line_param_t line_mask_param;
        lv_draw_mask_line_points_init(&line_mask_param, dsc->p1->x, dsc->p1->y, dsc->p2->x, dsc->p2->y,
                                      LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);

        /*Add a fade effect: transparent bottom covering top*/
        lv_coord_t h = lv_obj_get_height(obj);
        lv_draw_mask_fade_param_t fade_mask_param;
        lv_draw_mask_fade_init(&fade_mask_param, &obj->coords, LV_OPA_COVER, obj->coords.y1 + h / 8, LV_OPA_TRANSP,
                               obj->coords.y2);
        int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);

        /*Draw a rectangle that will be affected by the mask*/
        lv_draw_rect_dsc_t draw_rect_dsc;
        lv_draw_rect_dsc_init(&draw_rect_dsc);
        draw_rect_dsc.bg_opa = LV_OPA_100;
        draw_rect_dsc.bg_color = dsc->line_dsc->color;

        lv_area_t a;
        a.x1 = dsc->p1->x;
        a.x2 = dsc->p2->x - 1;
        a.y1 = LV_MIN(dsc->p1->y, dsc->p2->y);
        a.y2 = obj->coords.y2;
        lv_draw_rect(dsc->draw_ctx, &draw_rect_dsc, &a);

        /*Remove the masks*/
        lv_draw_mask_free_param(&line_mask_param);
        lv_draw_mask_free_param(&fade_mask_param);
        lv_draw_mask_remove_id(line_mask_id);
        lv_draw_mask_remove_id(fade_mask_id);
    }
#endif

    return;
}

lv_obj_t *common_widget_chart_create(common_widget_chart_para_t *chart_para)
{
    if(!chart_para)
        return NULL;

    lv_obj_t *common_widget_chart = \
        lv_chart_create(chart_para->chart_parent);
    lv_obj_set_size(common_widget_chart, \
        chart_para->chart_width, chart_para->chart_height);
    lv_obj_set_pos(common_widget_chart, \
        chart_para->chart_x, chart_para->chart_y);
    lv_chart_set_type(common_widget_chart, \
        chart_para->chart_type);
    lv_chart_set_point_count(common_widget_chart, \
        chart_para->chart_item_cnt);
    lv_chart_set_update_mode(common_widget_chart, \
        chart_para->chart_update_mode);
    lv_chart_set_div_line_count(common_widget_chart, \
        chart_para->chart_hor_div, chart_para->chart_ver_div);
    lv_chart_set_range(common_widget_chart, \
        LV_CHART_AXIS_PRIMARY_Y, chart_para->chart_priy_range_min, \
            chart_para->chart_priy_range_max);

    lv_obj_set_style_line_dash_gap(common_widget_chart, \
        3, LV_PART_MAIN);    
    lv_obj_set_style_line_dash_width(common_widget_chart, \
        5, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(common_widget_chart, \
        chart_para->chart_main_opax, LV_PART_MAIN);
    if(chart_para->chart_main_opax)
        lv_obj_set_style_bg_color(common_widget_chart, \
            chart_para->chart_main_color, LV_PART_MAIN);

    lv_obj_set_style_pad_ver(common_widget_chart, \
        0, LV_PART_MAIN);
    lv_obj_set_style_border_width(common_widget_chart, \
        2, LV_PART_MAIN);
    lv_obj_set_style_radius(common_widget_chart, \
        0, LV_PART_MAIN);
    lv_obj_set_style_border_side(common_widget_chart, \
        LV_BORDER_SIDE_LEFT|LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN);

    if(chart_para->chart_type == LV_CHART_TYPE_BAR)
        lv_obj_set_style_radius(common_widget_chart, \
            5, LV_PART_ITEMS);
    else if(chart_para->chart_type == LV_CHART_TYPE_LINE)
    {
        lv_obj_set_style_size(common_widget_chart, \
            6, LV_PART_INDICATOR);
        lv_obj_set_style_line_width(common_widget_chart, \
            4, LV_PART_ITEMS);
    }

    lv_chart_set_axis_tick(common_widget_chart, \
        LV_CHART_AXIS_PRIMARY_X, 0, 0, chart_para->chart_prix_major_cnt, 5, true, 35);
    lv_chart_set_axis_tick(common_widget_chart, \
        LV_CHART_AXIS_PRIMARY_Y, 0, 0, chart_para->chart_priy_major_cnt, 5, true, 35);

    for(uint8_t i = 0; i < chart_para->chart_series_num; i++)
    {
        chart_para->chart_series[i] = \
            lv_chart_add_series(common_widget_chart, \
                chart_para->chart_series_color[i], LV_CHART_AXIS_PRIMARY_Y);
        lv_chart_set_ext_y_array(common_widget_chart, \
            chart_para->chart_series[i], chart_para->chart_ext_y_array + \
                i*chart_para->chart_item_cnt);
    }

    lv_obj_set_style_text_font(common_widget_chart, \
        &font_common_num_18, LV_PART_TICKS);

    lv_obj_add_event_cb(common_widget_chart, \
        chart_draw_part_begin, LV_EVENT_DRAW_PART_BEGIN, (void *)chart_para);

    return common_widget_chart;
}


/*********************************************************************************
                                  文本区域控件                                       
*********************************************************************************/
common_widget_textarea_para_t widget_textarea_para = {0};
lv_obj_t *common_widget_textarea_create(common_widget_textarea_para_t *textarea_para)
{
    if(!textarea_para)
        return NULL;

    lv_obj_t *common_widget_textarea = \
        lv_textarea_create(textarea_para->parent);
    lv_obj_set_size(common_widget_textarea, \
        textarea_para->width, textarea_para->height);
    lv_obj_set_pos(common_widget_textarea, \
        textarea_para->x, textarea_para->y);
    lv_textarea_add_text(common_widget_textarea, \
        textarea_para->txt);
    lv_obj_set_scrollbar_mode(common_widget_textarea, \
        LV_SCROLLBAR_MODE_OFF);
    if(textarea_para->font)
        lv_obj_set_style_text_font(common_widget_textarea, \
            textarea_para->font, LV_PART_MAIN);
    else
        lv_obj_set_style_text_font(common_widget_textarea, \
            sys_default_label_font(), LV_PART_MAIN);
    lv_obj_set_style_text_color(common_widget_textarea, \
        textarea_para->color, LV_PART_MAIN);
    lv_obj_set_style_text_align(common_widget_textarea, \
        textarea_para->align, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(common_widget_textarea, \
        textarea_para->bg_opa_x, LV_PART_MAIN);
    lv_obj_set_style_bg_color(common_widget_textarea, \
        textarea_para->bg_color, LV_PART_MAIN);
    lv_obj_set_style_border_width(common_widget_textarea, \
        textarea_para->border_width, LV_PART_MAIN);
    if(textarea_para->border_width)
    {
        lv_obj_set_style_border_opa(common_widget_textarea, \
            textarea_para->border_opa_x, LV_PART_MAIN);
        lv_obj_set_style_border_color(common_widget_textarea, \
            textarea_para->border_color, LV_PART_MAIN);
    }
    // lv_obj_set_style_pad_ver(common_widget_textarea, 5, \
    //     LV_PART_MAIN);

    return common_widget_textarea;
}


/*********************************************************************************
                                  动画相关                                       
*********************************************************************************/
common_widget_anim_para_t widget_anim_para = {0};
void common_widget_anim_create(common_widget_anim_para_t *anim_para)
{
    if(!anim_para) return;

    lv_anim_init(anim_para->anim);

    if(anim_para->anim_obj)
        lv_anim_set_var(anim_para->anim, anim_para->anim_obj);

    if(anim_para->anim_exec_xcb)  
        lv_anim_set_exec_cb(anim_para->anim, anim_para->anim_exec_xcb);

    lv_anim_set_time(anim_para->anim, anim_para->anim_duration);
    lv_anim_set_values(anim_para->anim, anim_para->anim_start_val, \
        anim_para->anim_end_val);

    if(anim_para->is_start_anim)
        lv_anim_start(anim_para->anim);

    return;
}



/*********************************************************************************
                                  所有控件参数初始化                                      
*********************************************************************************/
void common_widget_para_init(void)
{
    /**************元素参数初始化**************/
    widget_refresh_init();
    common_time_widget_init();
    common_date_widget_init();
    common_week_widget_init();
    common_data_widget_init();

    /**************时钟指针参数初始化**************/
    common_clock_pointer_para_init();

    /**************弧形控件参数复位**************/
    memset(&widget_arc_para, 0, \
        sizeof(common_widget_arc_para_t));

    /**************图片控件参数复位**************/
    memset(&user_img_dsc, 0, \
        sizeof(user_img_dsc_t));
    memset(&widget_img_para, 0, \
        sizeof(common_widget_img_para_t));

    /**************标签控件参数复位**************/
    memset(&widget_label_para, 0, \
        sizeof(common_widget_label_para_t));

    /**************基控件参数复位**************/
    memset(&widget_obj_para, 0, \
        sizeof(common_widget_obj_para_t));

    /**************滑块控件参数复位**************/
    memset(&widget_slider_para, 0, \
        sizeof(common_widget_slider_para_t));

    /**************开关控件参数复位**************/
    memset(&widget_switch_para, 0, \
        sizeof(common_widget_switch_para_t));

    /**************滚轮控件参数复位**************/
    memset(&widget_roller_para, 0, \
        sizeof(common_widget_roller_para_t));

    /**************按钮控件参数复位**************/
    memset(&widget_button_para, 0, \
        sizeof(common_widget_button_para_t));

    /**************图表控件参数复位**************/
    memset(&widget_chart_para, 0, \
        sizeof(common_widget_chart_para_t));

    /**************文本区域控件参数复位**************/
    memset(&widget_textarea_para, 0, \
        sizeof(common_widget_textarea_para_t));

    /**************动画相关参数复位**************/
    memset(&widget_anim_para, 0, \
        sizeof(common_widget_anim_para_t));

    return;
}