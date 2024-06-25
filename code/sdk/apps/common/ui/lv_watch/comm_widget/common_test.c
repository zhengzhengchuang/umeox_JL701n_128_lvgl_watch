#if 0
/*Apple瀑布流对象集合*/
static apple_fall_ctx_t apple_fall_ctx = {0};

/*Apple瀑布流参数集合*/
static apple_fall_para_t apple_fall_para = {0};

/*Apple瀑布流滑动周期 <滑动 >=拖动*/
static const uint16_t apple_fall_slide_period = 6;

/*Apple瀑布流容器宽*/
static const uint16_t apple_fall_container_w = 320;

/*Apple瀑布流容器高*/
static const uint16_t apple_fall_container_h = 386;

/*Apple瀑布流图标间隔*/
static const uint16_t apple_fall_icon_inv = 85;

/*Apple瀑布流图标大小*/
static const uint16_t apple_fall_icon_size = 80;

/*Apple瀑布流曲面半径*/
static const uint16_t apple_fall_curved_r = 193;

/*Apple瀑布流图标数量*/
static const uint16_t apple_fall_icon_num = APPLE_FALL_ICON_NUM;
#endif

#if 0
    chart_color[0] = lv_color_hex(0xff0000);
    //chart_color[1] = lv_color_hex(0x00ff00);

    widget_chart_para.chart_parent = obj;
    widget_chart_para.chart_width = 250;
    widget_chart_para.chart_height = 150;
    widget_chart_para.chart_x = 35;
    widget_chart_para.chart_y = 133;
    widget_chart_para.chart_type = LV_CHART_TYPE_LINE;
    widget_chart_para.chart_item_cnt = CHART_ITEM_CNT;
    widget_chart_para.chart_update_mode = LV_CHART_UPDATE_MODE_CIRCULAR;
    widget_chart_para.chart_hor_div = 5;
    widget_chart_para.chart_ver_div = 0;
    widget_chart_para.chart_priy_range_min = 0;
    widget_chart_para.chart_priy_range_max = 200;
    widget_chart_para.chart_prix_major_cnt = CHART_PRIX_MAJOR_CNT;
    widget_chart_para.chart_priy_major_cnt = CHART_PRIY_MAJOR_CNT;
    widget_chart_para.chart_prix_label_str = chart_prix_label_str;
    widget_chart_para.chart_priy_label_str = chart_priy_label_str;
    widget_chart_para.chart_series_num = CHART_SERIES_NUM;
    widget_chart_para.chart_ext_y_array = chart_test_points[0];
    widget_chart_para.chart_series_color = chart_color;
    widget_chart_para.chart_series = chart_series;
    widget_chart_para.chart_main_opax = LV_OPA_0;
    widget_chart_para.chart_main_color = lv_color_black();
    common_widget_chart_create(&widget_chart_para);
#endif

#if 0
    widget_button_para.button_parent = obj;
    widget_button_para.button_x = 100;
    widget_button_para.button_y = 100;
    widget_button_para.button_width = 150;
    widget_button_para.button_height = 50;
    widget_button_para.button_main_opax = LV_OPA_100;
    widget_button_para.button_main_color = lv_color_hex(0xff0000);
    widget_button_para.button_radius = 50/2;
    common_widget_button_create(&widget_button_para);
#endif

#if 0
    widget_roller_para.roller_parent = obj;
    widget_roller_para.roller_x = 80;
    widget_roller_para.roller_y = 80;
    widget_roller_para.roller_width = 80;
    widget_roller_para.roller_height = 200;
    widget_roller_para.roller_row_count = 3;
    widget_roller_para.roller_options = test_str;
    widget_roller_para.roller_mode = LV_ROLLER_MODE_INFINITE;
    widget_roller_para.roller_sel = 5;
    widget_roller_para.roller_line_inv = 30;
    widget_roller_para.roller_main_bg_opax = LV_OPA_0;
    widget_roller_para.roller_main_bg_color = lv_color_hex(0x000000);
    widget_roller_para.roller_selected_bg_opax = LV_OPA_100;
    widget_roller_para.roller_selected_bg_color = lv_color_hex(0xff0000);
    widget_roller_para.roller_border_width = 2;
    widget_roller_para.roller_border_opax = LV_OPA_100;
    widget_roller_para.roller_border_color = lv_color_hex(0x00ff00);
    widget_roller_para.roller_main_text_font = &lv_font_montserrat_32;
    widget_roller_para.roller_main_text_color = lv_color_hex(0xffffff);
    widget_roller_para.roller_selected_text_font = &lv_font_montserrat_32;
    widget_roller_para.roller_selected_text_color = lv_color_hex(0x00ff00);
    widget_roller_para.event_cb = NULL;
    widget_roller_para.user_data = NULL;
    common_widget_roller_create(&widget_roller_para);
#endif

#if 0
    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 100;
    widget_img_para.img_y = 100;
    widget_img_para.file_img_dat = FILE_menu_icon_80x80_00_dat;
    common_widget_img_create(&widget_img_para, NULL);
    
#endif

#if 0
    apple_fall_container_create();

    menu_apple_fall_layout();
#endif

#if 0
    widget_switch_para.switch_parent = obj;
    widget_switch_para.switch_width = 100;
    widget_switch_para.switch_height = 50;
    widget_switch_para.switch_x = 100;
    widget_switch_para.switch_y = 100;
    widget_switch_para.switch_main_color = lv_color_hex(0xe9e9e9);
    widget_switch_para.switch_indic_color = lv_color_hex(0x00ff00);
    widget_switch_para.switch_cur_state = true;
    widget_switch_para.event_cb = NULL;
    common_widget_switch_create(&widget_switch_para);

    //lv_timer_create(timer_test, 5000, NULL);
#endif

#if 0
    widget_slider_para.slider_parent = obj;
    widget_slider_para.slider_x = 100;
    widget_slider_para.slider_y = 100;
    widget_slider_para.slider_width = 80;
    widget_slider_para.slider_height = 300;
    widget_slider_para.slider_min_value = 0;
    widget_slider_para.slider_max_value = 100;
    widget_slider_para.slider_cur_value = 50;
    widget_slider_para.slider_main_color = lv_color_hex(0x363636);
    widget_slider_para.slider_indic_color = lv_color_hex(0xffffff);
    widget_slider_para.slider_knob_opax = LV_OPA_0;
    widget_slider_para.slider_knob_color = lv_color_hex(0x000000);
    widget_slider_para.event_cb = NULL;
    widget_slider_para.user_data = NULL;
    lv_obj_t *slider_0 = common_widget_slider_create(&widget_slider_para);
    lv_obj_center(slider_0);
#endif

#if 0
    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 100;
    widget_obj_para.obj_y = 100;
    widget_obj_para.obj_width = 200;
    widget_obj_para.obj_height = 200;
    widget_obj_para.obj_bg_opax = LV_OPA_100;
    widget_obj_para.obj_bg_color = lv_color_hex(0xff0000);//透明度显示有问题
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x00ff00);
    widget_obj_para.obj_radius = 20;
    common_widget_obj_create(&widget_obj_para);
#endif

#if 0
    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
    widget_img_para.file_img_dat = FILE_watchface_00_bg_dat;
    lv_obj_t *watchface_bg_img = common_widget_img_create(&widget_img_para);

    widget_img_para.img_parent = watchface_bg_img;
    widget_img_para.img_x = 100;
    widget_img_para.img_y = 100;
    widget_img_para.file_img_dat = FILE_menu_icon_80x80_00_dat;
    common_widget_img_create(&widget_img_para);
#endif

#if 0
    widget_arc_para.arc_parent = obj;
    widget_arc_para.arc_x = 0;
    widget_arc_para.arc_y = 0;
    widget_arc_para.arc_bg_width = 300;
    widget_arc_para.arc_bg_height = 300;
    widget_arc_para.arc_bg_opax = LV_OPA_0;
    widget_arc_para.arc_main_start_angle = 0;
    widget_arc_para.arc_main_end_angle = 360;
    widget_arc_para.arc_indic_start_angle = 0;
    widget_arc_para.arc_indic_end_angle = 360;
    widget_arc_para.arc_rotation_angle = 270;
    widget_arc_para.arc_min_value = 0;
    widget_arc_para.arc_max_value = 100;
    widget_arc_para.arc_cur_value = 0;
    widget_arc_para.arc_main_line_width = 35;
    widget_arc_para.arc_indic_line_width = 35;
    widget_arc_para.arc_main_line_color = lv_color_hex(0x320411);
    widget_arc_para.arc_indic_line_color = lv_color_hex(0xff0000);//lv_color_hex(0xf61e5f);
    widget_arc_para.arc_main_is_rounded = true;
    widget_arc_para.arc_indic_is_rounded = true;
    widget_arc_para.arc_click_is_clear = true;
    lv_obj_t *steps_target_arc = common_widget_arc_create(&widget_arc_para);
    lv_obj_center(steps_target_arc);

    widget_arc_para.arc_bg_width = 220;
    widget_arc_para.arc_bg_height = 220;
    widget_arc_para.arc_main_line_color = lv_color_hex(0x162000);
    widget_arc_para.arc_indic_line_color = lv_color_hex(0xb9ff00);
    lv_obj_t *kcal_target_arc = common_widget_arc_create(&widget_arc_para);
    lv_obj_center(kcal_target_arc);

    widget_arc_para.arc_bg_width = 140;
    widget_arc_para.arc_bg_height = 140;
    widget_arc_para.arc_main_line_color = lv_color_hex(0x023232);
    widget_arc_para.arc_indic_line_color = lv_color_hex(0x00f3fa);
    lv_obj_t *distance_target_arc = common_widget_arc_create(&widget_arc_para);
    lv_obj_center(distance_target_arc);

    lv_anim_t steps_target_arc_anim;
    widget_anim_para.anim = &steps_target_arc_anim;
    widget_anim_para.anim_obj = steps_target_arc;
    widget_anim_para.anim_duration = 3000;
    widget_anim_para.anim_start_val = 0;
    widget_anim_para.anim_end_val = 100;
    widget_anim_para.anim_exec_xcb = steps_target_arc_anim_cb;
    widget_anim_para.is_start_anim = true;
    common_widget_anim_create(&widget_anim_para);

    lv_anim_t kcal_target_arc_anim;
    widget_anim_para.anim = &kcal_target_arc_anim;
    widget_anim_para.anim_obj = kcal_target_arc;
    widget_anim_para.anim_exec_xcb = kcal_target_arc_anim_cb;
    common_widget_anim_create(&widget_anim_para);

    lv_anim_t distance_target_arc_anim;
    widget_anim_para.anim = &distance_target_arc_anim;
    widget_anim_para.anim_obj = distance_target_arc;
    widget_anim_para.anim_exec_xcb = distance_target_arc_anim_cb;
    common_widget_anim_create(&widget_anim_para);
#endif




#if 0
static void my_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_target(e);

    int32_t value = lv_slider_get_value(obj);
    printf("============value = %d\n", value);

    return;
}
#endif

#if 0
static void apple_fall_container_create(void)
{
    lv_obj_t *obj = p_ui_info_cache->menu_load_info.p_menu_arg->menu_container;;

    apple_fall_ctx.apple_fall_container = lv_obj_create(obj);
    lv_obj_set_size(apple_fall_ctx.apple_fall_container, apple_fall_container_w, apple_fall_container_h);
    lv_obj_center(apple_fall_ctx.apple_fall_container);
    lv_obj_set_style_border_width(apple_fall_ctx.apple_fall_container, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(apple_fall_ctx.apple_fall_container, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_radius(apple_fall_ctx.apple_fall_container, 60, LV_PART_MAIN);
    lv_obj_clear_flag(apple_fall_ctx.apple_fall_container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_all(apple_fall_ctx.apple_fall_container, 0, LV_PART_MAIN);

    return;
}

static void apple_fall_icon_coord(void)
{
    uint16_t icon_id = 0;
    uint16_t line_id = 0;
    uint16_t line_num = 0;
    int16_t icon_start_x = 0;
    int16_t icon_start_y = 0;

    line_num = (apple_fall_icon_num/7)*2 + \
        ((apple_fall_icon_num%7)?(apple_fall_icon_num%7/4 + 1):0);

    icon_start_x = apple_fall_container_w/2 - apple_fall_icon_inv;
    icon_start_y = apple_fall_container_h/2 - 2*apple_fall_icon_inv*sin(3.14159f/3);

    for(line_id = 0; line_id < line_num; line_id++)
    {
        if(line_id%2 == 0)
        {
            for(uint8_t i = 0; i < 3 && icon_id < apple_fall_icon_num; i++, icon_id++)
            {
                int16_t icon_x_pivot = icon_start_x + i*apple_fall_icon_inv;
                int16_t icon_y_pivot = icon_start_y + line_id*apple_fall_icon_inv*sin(3.14159f/3);
                apple_fall_para.icon_pivot[icon_id].x = icon_x_pivot;
                apple_fall_para.icon_pivot[icon_id].y = icon_y_pivot;
            }
        }else
        {
            for(uint8_t j = 0; j < 4 && icon_id < apple_fall_icon_num; j++, icon_id++)
            {
                int16_t icon_x_pivot = icon_start_x + j*apple_fall_icon_inv - apple_fall_icon_inv/2;
                int16_t icon_y_pivot = icon_start_y + line_id*apple_fall_icon_inv*sin(3.14159f/3);
                apple_fall_para.icon_pivot[icon_id].x = icon_x_pivot;
                apple_fall_para.icon_pivot[icon_id].y = icon_y_pivot;
            }
        }
    }

    return;
}

static void apple_fall_icon_create(void)
{
    lv_obj_t *obj = apple_fall_ctx.apple_fall_container;

    widget_img_para.img_parent = obj;
    widget_img_para.img_x = 0;
    widget_img_para.img_y = 0;
  
    for(uint16_t i = 0; i < apple_fall_icon_num; i++)
    {
        widget_img_para.file_img_dat = FILE_menu_icon_80x80_00_dat + i;
        apple_fall_ctx.apple_fall_icon[i] = common_widget_img_create(&widget_img_para);
        lv_obj_add_flag(apple_fall_ctx.apple_fall_icon[i], LV_OBJ_FLAG_EVENT_BUBBLE);
    }

    return;
}

static void apple_fall_curved_zoom_algo(int16_t x_offset_val, int16_t y_offset_val)
{
    uint16_t zoom_scale = 0;
    int32_t curved_offset_squ = 0;
    int32_t curved_offset_sqr = 0;
    int32_t curved_x_offset_val = 0;
    int32_t curved_y_offset_val = 0;
    int32_t curved_projection_h = 0;
    int32_t curved_projection_l = 0;
    lv_point_t zoom_draw_pivot = {0};
    float cosine_law_angle_cos = 0.0f;
    float cosine_law_angle_sin = 0.0f;

    for(uint16_t i = 0; i < apple_fall_icon_num; i++)
    {
        curved_x_offset_val = apple_fall_para.icon_pivot[i].x + x_offset_val - apple_fall_container_w/2;
        curved_y_offset_val = apple_fall_para.icon_pivot[i].y + y_offset_val - apple_fall_container_h/2;

        curved_offset_squ = INT_NUM_SQUARE(curved_x_offset_val) + INT_NUM_SQUARE(curved_y_offset_val);

        curved_offset_sqr = (int32_t)(sqrt(curved_offset_squ*(1.0f)) + 0.5f);

        #if 0
        if(curved_offset_sqr < origin_min && apple_fall_para.curved_press)
        {
            origin_min = curved_offset_sqr;
            apple_fall_para.curved_origin_min.x = curved_x_offset_val;
            apple_fall_para.curved_origin_min.y = curved_y_offset_val;
        }
        #endif

        cosine_law_angle_cos = (1.0f) - curved_offset_squ*(1.0f)/(2*INT_NUM_SQUARE(apple_fall_curved_r));

        cosine_law_angle_sin = (float)(sqrt((1.0f) - cosine_law_angle_cos*cosine_law_angle_cos));

        curved_projection_h = (int32_t)(apple_fall_curved_r*cosine_law_angle_cos + 0.5f);

        if(curved_projection_h > 0)
        {
            apple_fall_para.icon_hidden[i] = false;

            curved_projection_l = (int32_t)(apple_fall_curved_r*cosine_law_angle_sin + 0.5f);

            zoom_scale = 256*curved_projection_h/apple_fall_curved_r;
            zoom_draw_pivot.x = curved_offset_sqr?(curved_x_offset_val*curved_projection_l/curved_offset_sqr):0;
            zoom_draw_pivot.y = curved_offset_sqr?(curved_y_offset_val*curved_projection_l/curved_offset_sqr):0;
        }else
        {
            apple_fall_para.icon_hidden[i] = true;
        }

        if(!apple_fall_para.icon_hidden[i])
        {
            apple_fall_para.apple_fall_zoom[i].zoom_scale = zoom_scale;
            apple_fall_para.apple_fall_zoom[i].zoom_draw_pivot.x = zoom_draw_pivot.x + apple_fall_container_w/2;
            apple_fall_para.apple_fall_zoom[i].zoom_draw_pivot.y = zoom_draw_pivot.y + apple_fall_container_h/2;
        }
    }

    return;
}

static void apple_fall_curved_zoom_draw(void)
{
    uint16_t zoom_scale = 0;
    lv_point_t zoom_draw_xy = {0};

    for(uint16_t i = 0; i < apple_fall_icon_num; i++)
    {
        if(apple_fall_para.icon_hidden[i])
        {
            lv_obj_add_flag(apple_fall_ctx.apple_fall_icon[i], LV_OBJ_FLAG_HIDDEN);
        }else
        {
            zoom_scale = apple_fall_para.apple_fall_zoom[i].zoom_scale;
            zoom_draw_xy.x = apple_fall_para.apple_fall_zoom[i].zoom_draw_pivot.x - apple_fall_icon_size/2;
            zoom_draw_xy.y = apple_fall_para.apple_fall_zoom[i].zoom_draw_pivot.y - apple_fall_icon_size/2;

            lv_img_set_zoom(apple_fall_ctx.apple_fall_icon[i], zoom_scale);
            lv_obj_clear_flag(apple_fall_ctx.apple_fall_icon[i], LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_pos(apple_fall_ctx.apple_fall_icon[i], zoom_draw_xy.x, zoom_draw_xy.y);
        }
    }

    return;
}

static void apple_fall_pressed_event_cb(lv_event_t *e)
{
    apple_fall_para.curved_x_dela = 0;
    apple_fall_para.curved_y_dela = 0;

    apple_fall_para.curved_press = true;
    apple_fall_para.curved_press_cnt = 0;

    return;
}

static void apple_fall_pressing_event_cb(lv_event_t *e)
{
    lv_point_t vert_point = {0};

    lv_indev_get_vect(lv_indev_get_act(), &vert_point);

    apple_fall_para.curved_x_dela += vert_point.x;
    apple_fall_para.curved_y_dela += vert_point.y;

    int16_t x_offset_val = apple_fall_para.curved_x_offset + apple_fall_para.curved_x_dela;
    int16_t y_offset_val = apple_fall_para.curved_y_offset + apple_fall_para.curved_y_dela;
    apple_fall_curved_zoom_algo(x_offset_val, y_offset_val);
    apple_fall_curved_zoom_draw();

    if(apple_fall_para.curved_press_cnt < 0xffffffff)
        apple_fall_para.curved_press_cnt++;

    return;
}

static void apple_fall_released_event_cb(lv_event_t *e)
{
    lv_point_t vert_point = {0};
    static bool curved_slide = false;

    apple_fall_para.curved_press = false;

    lv_indev_get_vect(lv_indev_get_act(), &vert_point);

    apple_fall_para.curved_x_offset += apple_fall_para.curved_x_dela;
    apple_fall_para.curved_y_offset += apple_fall_para.curved_y_dela;

    if(apple_fall_para.curved_press_cnt < apple_fall_slide_period)
    {
        curved_slide = true;
    }else
    {
        if(abs(vert_point.x) > 15 || abs(vert_point.y) > 15)
            curved_slide = true;
        else
            curved_slide = false;
    }

    if(curved_slide)
    {

    }else
    {

    }

    return;
}

static void apple_fall_tp_event_cb(lv_event_t *e)
{
    if(!e)  return;

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        apple_fall_pressed_event_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        apple_fall_pressing_event_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        apple_fall_released_event_cb(e);

    return;
}

static void menu_apple_fall_layout(void)
{
    apple_fall_icon_coord();

    apple_fall_icon_create();

    int16_t x_offset_val = apple_fall_para.curved_x_offset + apple_fall_para.curved_x_dela;
    int16_t y_offset_val = apple_fall_para.curved_y_offset + apple_fall_para.curved_y_dela;
    apple_fall_curved_zoom_algo(x_offset_val, y_offset_val);
    apple_fall_curved_zoom_draw();

    // lv_obj_t *obj = apple_fall_ctx.apple_fall_container;
    // lv_obj_add_event_cb(obj, apple_fall_tp_event_cb, LV_EVENT_ALL, NULL);

    return;
}
#endif

#if 0
static lv_obj_t *obj_00;
static void timer_test(lv_timer_t *timer)
{
    if(widget_switch_para.switch_cur_state)
        widget_switch_para.switch_cur_state = false;
    else
        widget_switch_para.switch_cur_state = true;

    if(widget_switch_para.switch_cur_state)
        lv_obj_add_state(obj_00, LV_STATE_CHECKED);
    else
        lv_obj_clear_state(obj_00, LV_STATE_CHECKED);
}
#endif

#if 0
const char test_str[] =
{
    "00\n01\n02\n03\n04\n05\n06\n07\n08\n09"
};
#endif

#if 0
#define CHART_ITEM_CNT 24
#define CHART_SERIES_NUM 1
static int16_t chart_test_points[CHART_SERIES_NUM][CHART_ITEM_CNT] =
{
    // [0] = 
    // {
    //     89, 95, 102, 92, 85, 87, 98,
    // },

    // [1] = 
    // {
    //     92, 85, 87, 98, 90, 100, 95
    // },
    89, 95, 102, 92, 85, 87, 98, 90, 100, 95, \
    89, 95, 102, 92, 85, 87, 98, 90, 100, 95, \
    89, 95, 102, 92,
};

#define CHART_PRIX_MAJOR_CNT 5
static const char *chart_prix_label_str[CHART_PRIX_MAJOR_CNT] =
{
    "00:00", "06:00", "12:00", "18:00", "00:00",
};

#define CHART_PRIY_MAJOR_CNT 5
static const char *chart_priy_label_str[CHART_PRIY_MAJOR_CNT] =
{
    " ", "50", "100", "150", "200",
};

static lv_color_t chart_color[CHART_SERIES_NUM];
static lv_chart_series_t *chart_series[CHART_SERIES_NUM];
#endif

#if 0
static void steps_target_arc_anim_cb(void *var, int32_t anim_val)
{
    lv_obj_t *obj = (lv_obj_t *)var;

    lv_arc_set_value(obj, anim_val);
}

static void kcal_target_arc_anim_cb(void *var, int32_t anim_val)
{
    lv_obj_t *obj = (lv_obj_t *)var;

    lv_arc_set_value(obj, anim_val);
}

static void distance_target_arc_anim_cb(void *var, int32_t anim_val)
{
    lv_obj_t *obj = (lv_obj_t *)var;

    lv_arc_set_value(obj, anim_val);
}
#endif


