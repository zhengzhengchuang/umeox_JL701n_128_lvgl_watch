#include "math.h"
#include "apple_network.h"

/****************图标源****************/
static const uint32_t apple_network_icon_src[Apple_Network_Icon_Num] = 
{
    menu_96_96_icon_00_index, menu_96_96_icon_01_index, menu_96_96_icon_02_index, menu_96_96_icon_03_index, \
    menu_96_96_icon_04_index, menu_96_96_icon_05_index, menu_96_96_icon_06_index, menu_96_96_icon_07_index, \
    menu_96_96_icon_08_index, menu_96_96_icon_09_index, menu_96_96_icon_10_index, menu_96_96_icon_11_index, \
    menu_96_96_icon_12_index, menu_96_96_icon_13_index, menu_96_96_icon_14_index, menu_96_96_icon_15_index, \
    menu_96_96_icon_16_index, menu_96_96_icon_17_index, menu_96_96_icon_18_index, menu_96_96_icon_19_index, \
    menu_96_96_icon_20_index, menu_96_96_icon_21_index, menu_96_96_icon_22_index, menu_96_96_icon_23_index, \
    menu_96_96_icon_24_index, menu_96_96_icon_25_index, menu_96_96_icon_26_index, menu_96_96_icon_27_index, \
    menu_96_96_icon_28_index, menu_96_96_icon_29_index, menu_96_96_icon_30_index, menu_96_96_icon_31_index, \
    menu_96_96_icon_32_index, menu_96_96_icon_33_index, menu_96_96_icon_34_index, menu_96_96_icon_35_index, \
    menu_96_96_icon_36_index, 
};

/****************图标点击索引****************/
static const uint16_t apple_network_icon_idx[Apple_Network_Icon_Num] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, \
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, \
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, \
    30, 31, 32, 33, 34, 35, 36,
};

/****************网格图标半径****************/
static const uint16_t apple_network_icon_r = 48;

/****************网格图标间隔****************/
static const uint16_t apple_network_icon_inv = 52;

/****************网格曲面半径****************/
static const uint16_t apple_network_curved_r = Apple_Network_Container_H/2;

/****************网格平面中心x****************/
static const int16_t apple_network_plane_cx = Apple_Network_Container_W/2;

/****************网格平面中心y****************/
static const int16_t apple_network_plane_cy = Apple_Network_Container_H/2;

/****************网格图标总数****************/
static const uint16_t apple_network_icon_num = Apple_Network_Icon_Num;

/****************网格蜂窝参数内容****************/
static apple_network_ctx_t apple_network_ctx = {0};


static void apple_network_container_event_cb(lv_event_t *e);

/*********************************************************************************
                                  创建网格容器                                     
*********************************************************************************/
static void apple_network_container_create(lv_obj_t *obj)
{
    lv_obj_t **apple_network_container = \
        &(apple_network_ctx.apple_network_container);

    widget_obj_para.obj_parent = obj;
    widget_obj_para.obj_x = 0;
    widget_obj_para.obj_y = 0;
    widget_obj_para.obj_width = Apple_Network_Container_W;
    widget_obj_para.obj_height = Apple_Network_Container_H;
    widget_obj_para.obj_bg_opax = LV_OPA_100;
    widget_obj_para.obj_bg_color = lv_color_hex(0x000000);
    widget_obj_para.obj_border_opax = LV_OPA_0;
    widget_obj_para.obj_border_width = 0;
    widget_obj_para.obj_border_color = lv_color_hex(0x000000);
    widget_obj_para.obj_radius = 0;
    widget_obj_para.obj_is_scrollable = false;
    *apple_network_container = common_widget_obj_create(&widget_obj_para);
    lv_obj_add_event_cb(*apple_network_container, apple_network_container_event_cb, \
        LV_EVENT_ALL, NULL);

    return;
}

/*********************************************************************************
                                 计算网格图标的轴向坐标                             
*********************************************************************************/
static void apple_network_calc_axial_coord(void)
{ 
    uint16_t i, total, hexagon_r;
    int16_t icon_axial_x, icon_axial_y;
    uint16_t one_edge_icons, hexagon_icons;
    lv_point_t *apple_network_axial = \
        apple_network_ctx.apple_network_axial;

    for(uint16_t idx = 0; idx < apple_network_icon_num; idx++)
    {
        if(idx != 0)
        {
            total = 0;
            hexagon_r = 0;
            hexagon_icons = 1;

            while(total + hexagon_icons - 1 < idx)
            {
                total += hexagon_icons;
                hexagon_r++;
                hexagon_icons = hexagon_r * 6;
            }

            one_edge_icons = hexagon_r + 1;

            icon_axial_x = 0;
            icon_axial_y = 0 - hexagon_r;

            for(i = 0; i < hexagon_icons; i++)
            {
                if(total + i == idx)
                    break;

                switch (i / (one_edge_icons - 1))
                {
                    case 0:
                        icon_axial_x--;
                        icon_axial_y++;
                        
                        break;
                    case 1:
                        icon_axial_y++;
                        break;
                    case 2:
                        icon_axial_x++;
                        break;
                    case 3:
                        icon_axial_y--;
                        icon_axial_x++;
                        break;
                    case 4:
                        icon_axial_y--;
                        break;
                    case 5:
                        icon_axial_x--;
                        break;
                    default:
                        break;
                }
            }
        }else
        {
            icon_axial_y = 0;
            icon_axial_x = 0;
        }

        apple_network_axial[idx].x = icon_axial_x;
        apple_network_axial[idx].y = icon_axial_y;
    }

    return;
}

/*********************************************************************************
                                  轴向坐标转换为屏幕坐标                             
*********************************************************************************/
static void apple_network_axial_convert_scr_coord(void)
{
    lv_point_t pivot = {0};
    int16_t sin_60 = lv_trigo_sin(60);
    int16_t icon_axial_x, icon_axial_y;
    uint8_t shift = LV_TRIGO_SHIFT - 1;
    lv_point_t *apple_network_axial = \
        apple_network_ctx.apple_network_axial;
    lv_point_t *apple_network_pivot = \
        apple_network_ctx.apple_network_pivot;
    
    for(uint16_t idx = 0; idx < apple_network_icon_num; idx++)
    {
        icon_axial_x = apple_network_axial[idx].x;
        icon_axial_y = apple_network_axial[idx].y;

        pivot.x = (icon_axial_y - icon_axial_x) * apple_network_icon_inv;
        pivot.y = ((icon_axial_y + icon_axial_x) * sin_60 * apple_network_icon_inv) >> shift;

        apple_network_pivot[idx].x = pivot.x;
        apple_network_pivot[idx].y = pivot.y;
    }

    return;
}

/*********************************************************************************
                                  所有图标事件回调函数                             
*********************************************************************************/
static void apple_network_icon_event_cb(lv_event_t *e)
{
    return;
}

/*********************************************************************************
                                  创建所有图标对象                                
*********************************************************************************/
static void apple_network_icon_obj_create(void)
{
    lv_obj_t **apple_network_icon = \
        apple_network_ctx.apple_network_icon;
    lv_obj_t *apple_network_container = \
        apple_network_ctx.apple_network_container;
    

    widget_img_para.img_parent = apple_network_container;
    widget_img_para.img_click_attr = true;
    widget_img_para.event_cb = apple_network_icon_event_cb;

    for(uint16_t idx = 0; idx < apple_network_icon_num; idx++)
    {
        widget_img_para.img_x = 0;
        widget_img_para.img_y = 0;
        widget_img_para.file_img_dat = apple_network_icon_src[idx];
        widget_img_para.user_data = (void *)&apple_network_icon_idx[idx];
        apple_network_icon[idx] = common_widget_img_create(&widget_img_para, NULL);
        lv_obj_add_flag(apple_network_icon[idx], LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_clear_flag(apple_network_icon[idx], LV_OBJ_FLAG_HIDDEN);
    }

    return;
}


/*********************************************************************************
                                  网格曲面区域限制                                
*********************************************************************************/
static void apple_network_container_area_limit(uint16_t idx, int16_t *x, int16_t *y, uint16_t *scale)
{
    if(x == NULL || y == NULL || scale == NULL)
        return;

    uint16_t overlap_w;
    uint16_t overlap_h;
    uint16_t icon_zoom_r;
    lv_area_t overlap_area;
    lv_area_t icon_zoom_area;
    lv_area_t container_area;

    container_area.x1 = 0;
    container_area.y1 = 0;
    container_area.x2 = Apple_Network_Container_W - 1;
    container_area.y2 = Apple_Network_Container_H - 1;

    icon_zoom_r = (uint16_t)((*scale)*(1.0f)*apple_network_icon_r/256);

    icon_zoom_area.x1 = *x + apple_network_plane_cx - icon_zoom_r;
    icon_zoom_area.y1 = *y + apple_network_plane_cy - icon_zoom_r;
    icon_zoom_area.x2 = *x + apple_network_plane_cx + icon_zoom_r;
    icon_zoom_area.y2 = *y + apple_network_plane_cy + icon_zoom_r;

    if(!_lv_area_is_in(&icon_zoom_area, &container_area, 0))
    {
        if(_lv_area_intersect(&overlap_area, &icon_zoom_area, &container_area))
        {
            overlap_w = overlap_area.x2 - overlap_area.x1;
            overlap_h = overlap_area.y2 - overlap_area.y1;

            icon_zoom_r = LV_MIN((overlap_w >> 1), (overlap_h >> 1));

            *x = overlap_area.x1 + (overlap_w >> 1) - apple_network_plane_cx;
            *y = overlap_area.y1 + (overlap_h >> 1) - apple_network_plane_cy;
            *scale = icon_zoom_r*(1.0f)*256/apple_network_icon_r;
        }
    }

    return;
}

/*********************************************************************************
                                  网格曲面缩放算法                               
*********************************************************************************/
static void apple_network_curved_zoom_algo(void)
{
    lv_sqrt_res_t sqrt;
    uint16_t zoom_scale = 0;
    float radian_angle_f = 0.0f;
    lv_point_t zoom_pivot = {0};
    int32_t curved_x_offset = 0;
    int32_t curved_y_offset = 0;
    int32_t curved_offset_squ = 0;
    float curved_offset_sqr = 0.0f;
    int32_t curved_projection_l = 0;
    int32_t curved_projection_h = 0;
    float cosine_law_angle_cos = 0.0f;
    float cosine_law_angle_sin = 0.0f;
    
    zoom_ctx_t *zoom_ctx = \
        apple_network_ctx.zoom_ctx;
    lv_point_t *curved_dela = \
        &apple_network_ctx.curved_dela;
    lv_point_t *curved_offset = \
        &apple_network_ctx.curved_offset;
    bool *apple_network_hidden = \
        apple_network_ctx.apple_network_hidden;
    lv_point_t *apple_network_pivot = \
        apple_network_ctx.apple_network_pivot;
    

    for(uint16_t idx = 0; idx < apple_network_icon_num; idx++)
    {
        curved_x_offset = apple_network_pivot[idx].x + curved_offset->x + curved_dela->x;
        curved_y_offset = apple_network_pivot[idx].y + curved_offset->y + curved_dela->y;

        curved_offset_squ = Int_Num_Squ(curved_x_offset) + Int_Num_Squ(curved_y_offset);

        lv_sqrt(curved_offset_squ, &sqrt, 0x8000);
        curved_offset_sqr = sqrt.i + (sqrt.f*(1.0f)/256 + 0.5f);

        cosine_law_angle_cos = (1.0f) - curved_offset_squ*(1.0f)/(2*Int_Num_Squ(apple_network_curved_r));

        radian_angle_f = curved_offset_sqr*(1.0f)/apple_network_curved_r;

        cosine_law_angle_sin = cosf((3.14159f)/2 - radian_angle_f);

        curved_projection_h = (int32_t)(apple_network_curved_r*cosine_law_angle_cos + 0.5f);

        if(curved_projection_h > 0)
        {
            apple_network_hidden[idx] = false;

            curved_projection_l = (int32_t)(apple_network_curved_r*cosine_law_angle_sin + 0.5f);

            zoom_scale = 256*curved_projection_h/apple_network_curved_r;
            zoom_pivot.x = curved_offset_sqr?(curved_x_offset*curved_projection_l/curved_offset_sqr):0;
            zoom_pivot.y = curved_offset_sqr?(curved_y_offset*curved_projection_l/curved_offset_sqr):0;

            apple_network_container_area_limit(idx, &zoom_pivot.x, &zoom_pivot.y, &zoom_scale);
        }else
        {
            apple_network_hidden[idx] = true;
        }

        if(!apple_network_hidden[idx])
        {
            zoom_ctx[idx].zoom_scale = zoom_scale;
            zoom_ctx[idx].zoom_pivot.x = zoom_pivot.x + apple_network_plane_cx;
            zoom_ctx[idx].zoom_pivot.y = zoom_pivot.y + apple_network_plane_cy;
        }
    }

    return;
}

/*********************************************************************************
                                  网格曲面图标缩放显示                             
*********************************************************************************/
static void apple_network_zoom_disp(void)
{
    uint16_t zoom_scale = 0;
    lv_point_t zoom_coord = {0};

    zoom_ctx_t *zoom_ctx = \
        apple_network_ctx.zoom_ctx;
    bool *apple_network_hidden = \
        apple_network_ctx.apple_network_hidden;
    lv_obj_t **apple_network_icon = \
        apple_network_ctx.apple_network_icon;

    for(uint16_t idx = 0; idx < apple_network_icon_num; idx++)
    {
        if(!apple_network_hidden[idx])
        {
            zoom_scale = zoom_ctx[idx].zoom_scale;
            zoom_coord.x = zoom_ctx[idx].zoom_pivot.x - apple_network_icon_r;
            zoom_coord.y = zoom_ctx[idx].zoom_pivot.y - apple_network_icon_r;

            lv_img_set_zoom(apple_network_icon[idx], zoom_scale);
            lv_obj_set_pos(apple_network_icon[idx], zoom_coord.x, zoom_coord.y);
            lv_obj_clear_flag(apple_network_icon[idx], LV_OBJ_FLAG_HIDDEN);

            continue;
        }

        lv_obj_add_flag(apple_network_icon[idx], LV_OBJ_FLAG_HIDDEN);
    }

    return;
}

/*********************************************************************************
                                  网格容器事件回调                                 
*********************************************************************************/
static void apple_network_container_pressed_cb(lv_event_t *e)
{
    lv_point_t *curved_dela = \
        &apple_network_ctx.curved_dela;
    
    memset(curved_dela, 0, sizeof(lv_point_t));

    return;
}

static void apple_network_container_pressing_cb(lv_event_t *e)
{
    lv_point_t vert_point = {0};
    lv_indev_get_vect(lv_indev_get_act(), \
        &vert_point);

    lv_point_t *curved_dela = \
        &apple_network_ctx.curved_dela;
        
    curved_dela->x += vert_point.x;
    curved_dela->y += vert_point.y;

    apple_network_curved_zoom_algo();

    //apple_network_zoom_disp();

    return;
}

static void apple_network_container_release_cb(lv_event_t *e)
{
    lv_point_t *curved_dela = \
        &apple_network_ctx.curved_dela;
    lv_point_t *curved_offset = \
        &apple_network_ctx.curved_offset;

    curved_offset->x += curved_dela->x;
    curved_offset->y += curved_dela->y;

    memset(curved_dela, 0, sizeof(lv_point_t));

    apple_network_curved_zoom_algo();

    //apple_network_zoom_disp();

    return;
}

static void apple_network_container_event_cb(lv_event_t *e)
{
    if(!e) return;

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_PRESSED)
        apple_network_container_pressed_cb(e);
    else if(event_code == LV_EVENT_PRESSING)
        apple_network_container_pressing_cb(e);
    else if(event_code == LV_EVENT_RELEASED)
        apple_network_container_release_cb(e);

    return;
} 


/*********************************************************************************
                                  网格曲面布局创建                                
*********************************************************************************/
static void apple_network_layout_create(void)
{
    apple_network_calc_axial_coord();

    apple_network_axial_convert_scr_coord();

    apple_network_icon_obj_create();

    apple_network_curved_zoom_algo();

    apple_network_zoom_disp();

    return;
}

static void menu_create_cb(lv_obj_t *obj)
{
    if(!obj) return;

    tileview_register_all_menu(obj, ui_act_id_null, ui_act_id_null, \
        ui_act_id_null, ui_act_id_null, ui_act_id_menu);

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

    memset(&apple_network_ctx, 0, sizeof(apple_network_ctx_t));

    apple_network_container_create(obj);

    apple_network_layout_create();
    
    return;
}

static void menu_key_cb(lv_obj_t *obj, int key_value, \
    int key_event)
{
    return;
}

register_ui_menu_load_info(\
    menu_load_apple_network) = 
{
    .menu_arg = NULL,
    .lock_flag = false,
    .return_flag = true,
    .menu_id = \
        ui_act_id_menu,
    .user_offscreen_time = 0,
    .user_refresh_time_inv = 0,
    .key_func_cb = menu_key_cb,
    .create_func_cb = menu_create_cb,
    .destory_func_cb = menu_destory_cb,
    .refresh_func_cb = menu_refresh_cb,
    .display_func_cb = menu_display_cb,
};
