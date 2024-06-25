#ifndef __COMMON_WIDGET_H__
#define __COMMON_WIDGET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lvgl/lvgl.h"
#include "../../../../../cpu/br28/ui_driver/imb_demo.h"
#include "../../../../../cpu/br28/ui_driver/lvgl/lvgl_main.h"



/*********************************************************************************
                                  弧控件                                       
*********************************************************************************/
typedef struct 
{
    /*弧的父对象*/
    lv_obj_t *arc_parent;

    /*弧的坐标*/
    int16_t arc_x;
    int16_t arc_y;

    /*弧的背景宽、高*/
    uint16_t arc_bg_width;
    uint16_t arc_bg_height;

    /*弧的背景透明度*/
    lv_opa_t arc_bg_opax;

    /*弧的主体起始角度、终止角度*/
    uint16_t arc_main_start_angle;
    uint16_t arc_main_end_angle;

    /*弧的指示器起始角度、终止角度*/
    uint16_t arc_indic_start_angle;
    uint16_t arc_indic_end_angle;

    /*弧的逆时针旋转角度*/
    uint16_t arc_rotation_angle;

    /*弧的值表达范围*/
    int16_t arc_min_value;
    int16_t arc_max_value;

    /*弧的当前值*/
    int16_t arc_cur_value;

    /*弧的主体线宽度*/
    uint16_t arc_main_line_width;

    /*弧的指示器线宽度*/
    uint16_t arc_indic_line_width;

    /*弧的主体线颜色*/
    lv_color_t arc_main_line_color;

    /*弧的指示器线颜色*/
    lv_color_t arc_indic_line_color;

    /*弧的主体线是否圆角*/
    bool arc_main_is_rounded;

    /*弧的指示器是否圆角*/
    bool arc_indic_is_rounded;

    /*弧的点击状态是否清除*/
    bool arc_click_is_clear;
}common_widget_arc_para_t;
extern common_widget_arc_para_t widget_arc_para;
lv_obj_t *common_widget_arc_create(common_widget_arc_para_t *arc_para);



/*********************************************************************************
                                  图片控件                                       
*********************************************************************************/
#define Img_Dsc_Max (240)
#define File_Img_Dat_None (0xffffffff)
typedef struct
{
    uint8_t img_dst_cnt;
    lv_img_dsc_t img_dst_gather[\
        Img_Dsc_Max];
}user_img_dsc_t;

typedef struct 
{
    /*图片的父对象*/
    lv_obj_t *img_parent;

    /*图片的坐标*/
    int16_t img_x;
    int16_t img_y;

    /*用户图片资源地址的索引*/
    uint32_t file_img_dat;

    /*点击属性*/
    bool img_click_attr;

    /*点击回调函数*/
    lv_event_cb_t event_cb;
    void *user_data;
}common_widget_img_para_t;
extern common_widget_img_para_t widget_img_para;
lv_obj_t *common_widget_img_create(common_widget_img_para_t *img_para, \
    uint16_t *img_dsc_idx);
lv_img_dsc_t *common_widget_img_open_res(uint32_t file_img_dat);
void common_widget_img_replace_src(lv_obj_t *obj, uint32_t file_img_dat, \
    uint16_t img_dsc_idx);



/*********************************************************************************
                                  标签控件                                       
*********************************************************************************/
#define Label_Line_Height (44)

typedef struct
{
    /*标签的父对象*/
    lv_obj_t *label_parent;

    /*标签的坐标*/
    int16_t label_x;
    int16_t label_y;

    /*标签的宽高*/
    uint16_t label_w;
    uint16_t label_h;

    /*标签长文本模式*/
    lv_label_long_mode_t long_mode;

    /*标签文本对齐*/
    lv_text_align_t text_align;

    /*标签显示文本*/
    char *label_text;

    /*标签文本颜色*/
    lv_color_t label_text_color;

    /*标签是否纵向居中*/
    bool label_ver_center;

    /*标签用户文本字体*/
    const lv_font_t *user_text_font; //NULL:默认字体，非NULL:用户自定义字体
}common_widget_label_para_t;
extern common_widget_label_para_t widget_label_para;
lv_obj_t *common_widget_label_create(common_widget_label_para_t *label_para);



/*********************************************************************************
                                  基容器控件                                       
*********************************************************************************/
typedef struct
{
    /*基容器的父对象*/
    lv_obj_t *obj_parent;

    /*基容器的坐标*/
    int16_t obj_x;
    int16_t obj_y;

    /*基容器的宽、高*/
    int16_t obj_width;
    int16_t obj_height;

    /*基容器的背景透明度、颜色*/
    lv_opa_t obj_bg_opax;
    lv_color_t obj_bg_color;

    /*基容器ji的边框透明度、颜色、宽度*/
    lv_opa_t obj_border_opax;
    lv_color_t obj_border_color;
    uint16_t obj_border_width;

    /*基容器的滚动属性是否保留 false:不滚动 true:滚动*/
    bool obj_is_scrollable;

    /*基容器的圆角*/
    uint16_t obj_radius;
}common_widget_obj_para_t;
extern common_widget_obj_para_t widget_obj_para;
lv_obj_t *common_widget_obj_create(common_widget_obj_para_t *obj_para);



/*********************************************************************************
                                  滑块控件                                       
*********************************************************************************/
typedef struct
{
    /*滑块的父对象*/
    lv_obj_t *slider_parent;

    /*滑块的坐标*/
    int16_t slider_x;
    int16_t slider_y;

    /*滑块的宽、高*/
    uint16_t slider_width;
    uint16_t slider_height;

    /*滑块的范围*/
    int32_t slider_min_value;
    int32_t slider_max_value;

    /*滑块的当前值*/
    int32_t slider_cur_value;

    /*滑块主体的颜色*/
    lv_color_t slider_main_color;

    /*滑块指示器的颜色*/
    lv_color_t slider_indic_color;

    /*滑块knob的透明度、颜色*/
    lv_opa_t slider_knob_opax;
    lv_color_t slider_knob_color;

    /*滑块的事件回调*/
    lv_event_cb_t event_cb;
    void *user_data;
}common_widget_slider_para_t;
extern common_widget_slider_para_t widget_slider_para;
lv_obj_t *common_widget_slider_create(common_widget_slider_para_t *slider_para);



/*********************************************************************************
                                  开关控件                                       
*********************************************************************************/
typedef struct
{
    /*开关的父对象*/
    lv_obj_t *switch_parent;

    /*开关的坐标*/
    int16_t switch_x;
    int16_t switch_y;

    /*开关的宽、高*/
    uint16_t switch_width;
    uint16_t switch_height;

    /*开关的主体颜色*/
    lv_color_t switch_main_color;

    /*开关的指示器颜色*/
    lv_color_t switch_indic_color;

    /*开关的当前状态*/
    bool switch_cur_state;

    /*开关的事件回调*/
    lv_event_cb_t event_cb;
    void *user_data;
}common_widget_switch_para_t;
extern common_widget_switch_para_t widget_switch_para;
lv_obj_t *common_widget_switch_create(common_widget_switch_para_t *switch_para);



/*********************************************************************************
                                  滚轮控件                                       
*********************************************************************************/
typedef struct
{
    /*滚轮的父对象*/
    lv_obj_t *roller_parent;

    /*滚轮的坐标*/
    int16_t roller_x;
    int16_t roller_y;

    /*滚轮的宽、高*/
    uint16_t roller_width;
    uint16_t roller_height;

    /*滚轮的选项卡、模式*/
    const char *roller_options;
    lv_roller_mode_t roller_mode;

    /*滚轮的当前选项*/
    uint16_t roller_sel;

    /*滚轮的行间隔*/
    int16_t roller_line_inv;

    /*滚轮的可视化行数*/
    uint8_t roller_row_count;

    /*滚轮的主体透明度、颜色*/
    lv_opa_t roller_main_bg_opax;
    lv_color_t roller_main_bg_color;

    /*滚轮的选择部分透明度、颜色*/
    lv_opa_t roller_selected_bg_opax;
    lv_color_t roller_selected_bg_color;

    /*滚轮的边框透明度、宽度、颜色*/
    lv_opa_t roller_border_opax;
    uint16_t roller_border_width;
    lv_color_t roller_border_color;

    /*滚轮的主体文本字体、颜色*/
    const lv_font_t *roller_main_text_font;
    lv_color_t roller_main_text_color;

    /*滚轮的选择文本字体、颜色*/
    const lv_font_t *roller_selected_text_font;
    lv_color_t roller_selected_text_color;

    /*滚轮的事件回调*/
    lv_event_cb_t event_cb;
    void *user_data;
}common_widget_roller_para_t;
extern common_widget_roller_para_t widget_roller_para;
lv_obj_t *common_widget_roller_create(common_widget_roller_para_t *roller_para);



/*********************************************************************************
                                  按钮控件                                       
*********************************************************************************/
typedef struct
{
    /*按钮的父对象*/
    lv_obj_t *button_parent;

    /*按钮的坐标*/
    int16_t button_x;
    int16_t button_y;

    /*按钮的宽、高*/
    uint16_t button_width;
    uint16_t button_height;

    /*按钮的主体透明度、颜色*/
    lv_opa_t button_main_opax;
    lv_color_t button_main_color;

    /*按钮的圆角*/
    uint16_t button_radius;

    /*按钮的事件回调*/
    lv_event_cb_t event_cb;
    void *user_data;
}common_widget_button_para_t;
extern common_widget_button_para_t widget_button_para;
lv_obj_t *common_widget_button_create(common_widget_button_para_t *button_para);



/*********************************************************************************
                                  图表控件                                       
*********************************************************************************/
typedef struct
{
    /*图表的父对象*/
    lv_obj_t *chart_parent;

    /*图表的坐标*/
    int16_t chart_x;
    int16_t chart_y;

    /*图表的宽、高*/
    uint16_t chart_width;
    uint16_t chart_height;

    /*图表的类型：折线、柱状*/
    lv_chart_type_t chart_type;

    /*图表条目的总数*/
    uint16_t chart_item_cnt;

    /*图表更新的模式*/
    lv_chart_update_mode_t chart_update_mode;

    /*图表的横、纵向分割线数量*/
    uint8_t chart_hor_div;
    uint8_t chart_ver_div;

    /*图表主轴y的数值范围*/
    int16_t chart_priy_range_min;
    int16_t chart_priy_range_max;

    /*图表主轴x、y的主刻度线的数量*/
    int16_t chart_prix_major_cnt;
    int16_t chart_priy_major_cnt;

    /*图表主轴x、y的主刻度线标签字符串*/
    const char **chart_prix_label_str;
    const char **chart_priy_label_str;

    /*图表数据系列相关*/
    uint8_t chart_series_num;
    int16_t *chart_ext_y_array;
    lv_color_t *chart_series_color;
    lv_chart_series_t **chart_series;

    /*图表主体透明度、颜色*/
    lv_opa_t chart_main_opax;
    lv_color_t chart_main_color;
}common_widget_chart_para_t;
extern common_widget_chart_para_t widget_chart_para;
lv_obj_t *common_widget_chart_create(common_widget_chart_para_t *chart_para);


/*********************************************************************************
                                  文本区域控件                                       
*********************************************************************************/
typedef struct
{
    lv_obj_t *parent;

    /*文本区域的坐标*/
    int16_t x;
    int16_t y;

    /*文本区域的宽、高*/
    uint16_t width;
    uint16_t height;

    /*文本区域的文本*/
    const char *txt;

    /*文本区域的字体*/
    const lv_font_t *font;

    /*文本区域的颜色*/
    lv_color_t color;

    /*文本区域的对齐*/
    lv_text_align_t align;

    /*文本区域的背景透明、颜色*/
    lv_opa_t bg_opa_x;
    lv_color_t bg_color;

    /*文本区域的边框透明、颜色、宽度*/
    lv_opa_t border_opa_x;
    uint16_t border_width;
    lv_color_t border_color;
}common_widget_textarea_para_t;
extern common_widget_textarea_para_t widget_textarea_para;
lv_obj_t *common_widget_textarea_create(common_widget_textarea_para_t *textarea_para);

/*********************************************************************************
                                  动画相关                                       
*********************************************************************************/
typedef struct
{
    /*指向描述动画的变量*/
    lv_anim_t *anim;

    /*动画关联的对象*/
    void *anim_obj;

    /*动画回调函数*/
    lv_anim_exec_xcb_t anim_exec_xcb;

    /*动画时长*/
    uint32_t anim_duration;

    /*动画起始值、结束值*/
    int32_t anim_start_val;
    int32_t anim_end_val;

    /*是否启动动画*/
    bool is_start_anim;
}common_widget_anim_para_t;
extern common_widget_anim_para_t widget_anim_para;
void common_widget_anim_create(common_widget_anim_para_t *anim_para);



void common_widget_para_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
