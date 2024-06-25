/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui.h
 * @brief 杰理UI框架头文件(注意：杰理UI框架在控件刷新时会触发on_change事件回调，因此在on_change事件的回调中禁止调用含有redraw刷新的接口，否则会进入递归引发不可预测的问题)
 * @author
 * @version
 * @date 2022-12-14
 */
#ifndef UI_CORE_H
#define UI_CORE_H

#include "window.h"
#include "ui_button.h"
#include "ui_grid.h"
#include "ui_time.h"
#include "ui_camera.h"
#include "ui_pic.h"
#include "ui_text.h"
#include "ui_battery.h"
#include "ui_browser.h"
#include "ui_slider.h"
#include "ui_slider_vert.h"
#include "ui_number.h"
#include "ui_watch.h"
#include "ui_progress.h"
#include "ui_progress_multi.h"
#include "ui_rotate.h"
#include "ui_page_manager.h"
#include "ui_compass.h"
#include <stdarg.h>

struct uimsg_handl {
    const char *msg;
    int (*handler)(const char *type, u32 args);
};

enum {
    HIDE_WITH_REDRAW,
    HIDE_WITHOUT_REDRAW,
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_framework_init ui框架初始化
 *
 * @param lcd配置句柄，在board.c文件配置
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int ui_framework_init(void *);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_set_style_file 设置ui风格
 *
 * @param style ui风格资源句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_set_style_file(struct ui_style *style);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_style_file_version_compare UI风格版本检查
 *
 * @param version 目标版本号
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_style_file_version_compare(int version);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_redraw UI控件刷新
 *
 * @param id 待刷新的UI控件ID(注意：凡刷新的动作，不允许在ONCHANGE事件中调用)
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_redraw(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_show 设置UI控件为显示
 *
 * @param id UI控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_show(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_hide 设置UI控件为隐藏
 *
 * @param id UI控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_hide(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_set_call 设置页面回调
 * (一般在ONCHANGE_INIT事件中使用，页面第一次显示完后会触发回调，回调中可调用redraw，可用
 * 于图标等内容第一次显示时更新图标。回调被调用后，指针会被清空；即回调注册后只会被调用一
 * 次)
 *
 * @param func 回调函数指针
 * @param param 传给回调函数的参数
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_set_call(int (*func)(int), int param);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_event_onkey UI控件按键事件回调
 *
 * @param e 按键事件句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_event_onkey(struct element_key_event *e);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_event_ontouch UI控件触摸事件回调
 *
 * @param e 触摸事件句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_event_ontouch(struct element_touch_event *e);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_card_ontouch UI卡片触摸事件回调（已放到lcd_ui_api.c文件）
 *
 * @param e 触摸事件句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_card_ontouch(struct element_touch_event *e);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_core_get_root
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_core_get_root();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_get_highlight_child_by_id 获取高亮的子控件
 *
 * @param id 控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
struct element *ui_get_highlight_child_by_id(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_invert_element_by_id
 *
 * @param id
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_invert_element_by_id(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_no_highlight_element 取消控件高亮（不带redraw）
 *
 * @param elm 控件句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_no_highlight_element(struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_no_highlight_element_by_id 取消控件高亮（带redraw）
 *
 * @param id 控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_no_highlight_element_by_id(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_highlight_element 设置控件高亮（不带redraw）
 *
 * @param elm 控件句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_highlight_element(struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_highlight_element_by_id 设置控件高亮（带redraw）
 *
 * @param id 控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_highlight_element_by_id(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_get_current_window_id 获取当前显示的window ID
 *
 * @return 当前显示的window ID
 */
/* ------------------------------------------------------------------------------------*/
int ui_get_current_window_id();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_switch 手动切换页面
 *
 * @param curr_win 当前页面ID
 * @param next_win 下一个页面ID
 * @param xoffset 移动步距
 * @param mode 特效模式（暂时只有模式0）
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_page_switch(int curr_win, int next_win, int xoffset, int mode);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_register_msg_handler 注册指定控件的消息回调句柄
 *
 * @param id 控件ID
 * @param handl 回调句柄
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_register_msg_handler(int id, const struct uimsg_handl *handl);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_message_handler
 *
 * @param id
 * @param msg
 * @param va_list
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_message_handler(int id, const char *msg, va_list);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief str_substr_iter
 *
 * @param str
 * @param delim
 * @param iter
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
const char *str_substr_iter(const char *str, char delim, int *iter);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_get_child_by_id
 *
 * @param id
 * @param event_handler_cb
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_get_child_by_id(int id, int (*event_handler_cb)(void *, int, int));


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_set_default_handler
 *
 * @param ontouch
 * @param onkey
 * @param onchange
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_set_default_handler(struct element *elm, int (*ontouch)(void *, struct element_touch_event *),
                           int (*onkey)(void *, struct element_key_event *),
                           int (*onchange)(void *, enum element_change_event, void *));


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_ontouch_lock 触摸消息锁定发送给控件之外的区域
 *
 * @param elm 目标控件的元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_ontouch_lock(void *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_ontouch_unlock 释放控件之外的区域锁定
 *
 * @param elm 目标控件的元素
 */
/* ------------------------------------------------------------------------------------*/
void ui_ontouch_unlock(void *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_lock_layer 锁定控件的父图层，先不推向imb显示
 *
 * @param id 控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_lock_layer(int id);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_unlock_layer 释放控件的父图层，可以推向imb显示
 *
 * @param id 控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_unlock_layer(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_get_disp_status_by_id 获取指定控件的显示状态(默认隐藏标志)
 *
 * @param id 待获取状态的控件ID
 *
 * @return false 显示，true 隐藏
 */
/* ------------------------------------------------------------------------------------*/
int ui_get_disp_status_by_id(int id);



/* ------------------------------------------------------------------------------------*/
/**
 * @brief create_control_by_id 动态创建控件(将B工程的指定控件显示到A工程的指定容器中)
 *
 * @param tabfile 待创建控件的tab文件路径
 * @param page_id 带创建控件所在PAGE ID
 * @param id 待创建控件的ID
 * @param parent_id 容器控件的ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int create_control_by_id(char *tabfile, int page_id, int id, int parent_id);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief delete_control_by_id 删除动态创建的控件(一般与create_control_by_id成对使用)
 *
 * @param id 要删除的动态创建的控件ID
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int delete_control_by_id(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_remove_backcolor 删除控件的背景颜色
 *
 * @param elm 待删除颜色的控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_remove_backcolor(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_remove_backimage 删除控件的背景图
 *
 * @param elm 待删除背景图的控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_remove_backimage(struct element *elm);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_remove_border 删除控件的边框
 *
 * @param elm 待删除边框的控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_remove_border(struct element *elm);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_fill_rect ui填充矩形
 *
 * @param dc UI框架句柄
 * @param left 矩形left坐标
 * @param top 矩形top坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 * @param acolor 填充颜色（带透明度）
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_fill_rect(struct draw_context *dc, int left, int top, int width, int height, u32 acolor);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_image 自定义绘图方式显示图片
 *
 * @param dc 待显示的上下文句柄
 * @param page 待显示图片所在页面
 * @param id 待显示图片所在控件ID
 * @param x 待显示图片将显示的X坐标
 * @param y 待显示图片将显示的Y坐标
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_draw_image(struct draw_context *dc, int page, int id, int x, int y);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_image_large 大图片截取显示
 *
 * @param dc 显示上下文
 * @param page 页面号
 * @param id 图片索引
 * @param x 显示的X坐标(相对屏幕)
 * @param y 显示的Y坐标(相对屏幕)
 * @param width 显示宽度
 * @param height 显示高度
 * @param image_x 图片的X坐标(相对图片)
 * @param image_y 图片的Y坐标(相对图片)
 *
 * @return -1 失败，0 正常
 * @note 大图片显示具有尺寸限制，请留意工具或者代码提示信息
 */
/* ------------------------------------------------------------------------------------*/
int ui_draw_image_large(struct draw_context *dc, int page, int id, int x, int y, int width, int height, int image_x, int image_y);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_ascii 自定义合成ascii字符串显示
 *
 * @param dc 显示上下文
 * @param str 字符串指针
 * @param strlen 字符串长度
 * @param x 字符串X坐标
 * @param y 字符串Y坐标
 * @param color 字符串显示颜色
 *
 * @return 0 正常，-1 失败
 */
/* ------------------------------------------------------------------------------------*/
int ui_draw_ascii(struct draw_context *dc, char *str, int strlen, int x, int y, int color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_text 自定义显示字符串
 *
 * @param dc 显示上下文
 * @param encode 编码格式
 * @param endian 大小端
 * @param str 字符串指针
 * @param strlen 字符串长度
 * @param x 显示X坐标
 * @param y 显示Y坐标
 * @param color 显示颜色
 *
 * @return 0 正常，-1 失败
 */
/* ------------------------------------------------------------------------------------*/
int ui_draw_text(struct draw_context *dc, int encode, int endian, char *str, int strlen, int x, int y, int color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_strpic 显示多国语言excel表文本
 *
 * @param dc 显示上下文
 * @param id 文本控件ID
 * @param x 显示X坐标
 * @param y 显示Y坐标
 * @param color 文本颜色
 *
 * @return 0 正常，-1 失败
 */
/* ------------------------------------------------------------------------------------*/
int ui_draw_strpic(struct draw_context *dc, int id, int x, int y, int color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_line 绘制直线
 *
 * @param _dc 显示上下文
 * @param x0 直线起点X坐标
 * @param y0 直线起点Y坐标
 * @param x1 直线终点X坐标
 * @param y1 直线终点Y坐标
 * @param color 直线颜色
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_line(void *_dc, int x0, int y0, int x1, int y1, int color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_line_by_angle 绘制指定角度的直线
 *
 * @param _dc 显示上下文
 * @param x 直线起点X坐标
 * @param y 直线起点Y坐标
 * @param length 直线长度
 * @param angle 直线角度
 * @param color 直线颜色
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_line_by_angle(void *_dc, int x, int y, int length, int angle, int color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_rect 绘制矩形填充
 *
 * @param _dc 显示上下文
 * @param x 矩形左上角X坐标
 * @param y 矩形左上角Y坐标
 * @param width 矩形宽度
 * @param height 矩形高度
 * @param color 填充颜色
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_rect(void *_dc, int x, int y, int width, int height, int color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_circle 绘制圆环
 *
 * @param dc 显示上下文
 * @param center_x 圆环中心X坐标
 * @param center_y 圆环中心Y坐标
 * @param radius_big 圆环外径
 * @param radius_small 圆环内径
 * @param angle_begin 起始角度
 * @param angle_end 结束角度
 * @param color 圆环颜色
 * @param percent 绘制百分比
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_circle(struct draw_context *dc, int center_x, int center_y,
                    int radius_big, int radius_small, int angle_begin,
                    int angle_end, int color, int percent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_ring 绘制圆
 *
 * @param _dc 显示上下文
 * @param x 圆心X坐标
 * @param y 圆心Y坐标
 * @param radius_big 圆形外径
 * @param radius_small 圆形内径
 * @param angle_begin 起始角度
 * @param angle_end 结束角度
 * @param color 颜色
 * @param percent 绘制百分比
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_ring(void *_dc, int x, int y, int radius_big, int radius_small, int angle_begin, int angle_end, int color, int percent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_bar 绘制水平进度条
 *
 * @param _dc 显示上下文句柄
 * @param x 绘制的X坐标(左上角)
 * @param y 绘制的Y坐标(右上角)
 * @param width 绘制宽度
 * @param height 绘制高度
 * @param color 高亮颜色
 * @param percent 高亮百分比
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_bar(void *_dc, int x, int y, int width, int height, int color, int percent);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_alpha_bar	透明度圆环
 *
 * @param _dc
 * @param x
 * @param y
 * @param width
 * @param height
 * @param color
 * @param percent (0-100)
 * @param alpha  (0-255)
 */
/* ------------------------------------------------------------------------------------*/
void ui_draw_alpha_bar(void *_dc, int x, int y, int width, int height, int color, int percent, int alpha);

/* 弃用 */
int ui_draw_set_pixel(struct draw_context *dc, int x, int y, int pixel);
u32 ui_draw_get_pixel(struct draw_context *dc, int x, int y);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw_get_mixed_pixel 颜色混合(ARGB565)
 *
 * @param backcolor 背景色
 * @param forecolor 前景色
 * @param alpha 透明度
 *
 * @return u16 混合后的颜色值
 */
/* ------------------------------------------------------------------------------------*/
u16 ui_draw_get_mixed_pixel(u16 backcolor, u16 forecolor, u8 alpha);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_custom_draw_clear 清空自定义绘图
 *
 * @param dc 显示上下文
 */
/* ------------------------------------------------------------------------------------*/
void ui_custom_draw_clear(struct draw_context *dc);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief load_control_info_by_id 通过控件ID加载控件资源信息
 *
 * @param tabfile 控件所在tab文件路径
 * @param page_id 控件所在页面ID
 * @param id 控件ID
 *
 * @return NULL 失败，control_pos * 控件在UI资源中的信息
 */
/* ------------------------------------------------------------------------------------*/
void *load_control_info_by_id(char *tabfile, u32 page_id, u32 id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_control_new 创建新控件
 *
 * @param _pos 控件资源信息
 * @param parent 父控件句柄
 *
 * @return 新创建的控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void *ui_control_new(void *_pos, void *parent);


/* 弃用 */
int ui_draw_begin(struct draw_context *dc);
int ui_draw_end(struct draw_context *dc);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_draw
 *
 * @param dc
 * @param buf
 * @param x
 * @param y
 * @param width
 * @param height
 * @param cb
 * @param priv
 * @param priv_len
 * @param id
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_draw(struct draw_context *dc, u8 *buf, int x, int y, int width, int height, void *cb, void *priv, int priv_len, int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_set_param 卡片界面左右滑动参数设置
 *
 * @param threshold 阈值(继续滑动或回弹)
 * @param step 步距(自动滑动或回弹时每次步进量)
 * @param delay 自动滑动时间间隔(delay * 10 ms)
 */
/* ------------------------------------------------------------------------------------*/
void ui_page_set_param(int threshold, int step, int delay);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_hide_set 设置控件隐藏动作是否刷新
 *
 * @param id 控件ID
 * @param redraw 刷新标志
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_hide_set(int id, int redraw);

#define UI_SLIDE_EN 1

#define RGB565(r,g,b) (((((u8)r)>>3)<<11)|((((u8)g)>>2)<<5)|(((u8)b)>>3))


#endif

