#ifndef UI_GRID_H
#define UI_GRID_H


#include "ui/ui_core.h"
#include "ui/control.h"

/* ------------------------------------------------------------------------------------*/
/**
 * @brief
 */
/* ------------------------------------------------------------------------------------*/
enum {
    GRID_SCROLL_MODE,
    GRID_PAGE_MODE,
};
/* ------------------------------------------------------------------------------------*/
/**
 * @brief 列表滚动方向
 */
/* ------------------------------------------------------------------------------------*/
enum {
    SCROLL_DIRECTION_NONE,
    SCROLL_DIRECTION_LR,
    SCROLL_DIRECTION_UD,
};
/* ------------------------------------------------------------------------------------*/
/**
 * @brief 自动居中类型选项
 */
/* ------------------------------------------------------------------------------------*/
typedef enum {
    NO_AUTO_CENTER,
    AUTO_CENTER_MODE1,//滑动惯性距离小于1/2屏幕时回弹
    AUTO_CENTER_MODE2,//滑动惯性较小时依然能滑向下一项
    AUTO_CENTER_MODE3,//高亮项居中，适用于列表
    AUTO_CENTER_MODE4,//基于触摸项的滑动
    AUTO_CENTER_MODE5,//匀减速
    AUTO_CENTER_CUSTOM,//自定义滑动处理
} AUTO_CENTER_MODE;
/* ------------------------------------------------------------------------------------*/
/**
 * @brief
 */
/* ------------------------------------------------------------------------------------*/
struct ui_grid_item_info {
    u8 row;
    u8 col;
    u8 page_mode;
    u8 highlight_index;
    u16 interval;
    struct layout_info *info;
};
/* ------------------------------------------------------------------------------------*/
/**
 * @brief 列表显示/滚动区域
 */
/* ------------------------------------------------------------------------------------*/
struct scroll_area {
    int left;
    int top;
    int right;
    int bottom;
};
/* ------------------------------------------------------------------------------------*/
/**
 * @brief 动态列表info
 */
/* ------------------------------------------------------------------------------------*/
struct ui_grid_dynamic {
    int  dhi_index;
    int  dcol_num;
    int  drow_num;

    int  min_row_index;
    int  max_row_index;
    int  min_col_index;
    int  max_col_index;
    int  min_show_row_index;
    int  max_show_row_index;
    int  min_show_col_index;
    int  max_show_col_index;

    int  grid_xval;
    int  grid_yval;
    u8   grid_col_num;
    u8   grid_row_num;
    u8   grid_show_row;
    u8   grid_show_col;
    int  base_index_once;
    int init_step_once;
};
/* ------------------------------------------------------------------------------------*/
/**
 * @brief 列表元素
 */
/* ------------------------------------------------------------------------------------*/
struct ui_grid {
    struct element elm;
    // char hi_num;
    char hi_index;
    char touch_index;
    char cur_dindex;
    char onfocus;
    u8   page_mode;
    u8   slide_direction;
    u8   col_num;
    u8   row_num;
    u8   show_row;
    u8   show_col;
    u8   avail_item_num;
    u8   pix_scroll;
    u8   ctrl_num;
    u8   page;
    u8   child_init;
    u8   rotate;	// 垂直列表转为旋转列表标志
    int  x_interval;
    int  y_interval;
    int  max_show_left;
    int  max_show_top;
    int  min_show_left;
    int  min_show_top;
    int  max_left;
    int  max_top;
    int  min_left;
    int  min_top;
    // int  scroll_step;
    // u8   ctrl_num;
    int energy_timer;
    float energy_vx0;
    float energy_vy0;
    float energy_a;
    float energy_val;
    u8 energy_xdir;
    u8 energy_ydir;
    u8 energy_status;
    u8 energy_tslide;


    u8 flick_endflag;
    u8 flick_status;
    u16 flick_cmpsize;
    int flick_timer;
    int flick_distance;
    int flick_overdis;
    int flick_resdis;
    float flick_v0;

    u16 center_target_line;		//居中目标中线位置：0-10000
    u16 center_next_threshold;	//滑入下一项阈值：0-10000
    u8 center_item_offset;	//居中项偏移阈值:0-(avail_item_num-1)
    u8 center_index_mode;		//高亮项or居中项
    u8 auto_center_enable;
    u8 flick_close;

    struct element_luascript_t *lua;
    struct scroll_area *area;
    struct layout *item;
    struct layout_info *item_info;
    // struct element elm2;
    struct ui_grid_dynamic *dynamic;
    struct position pos;
    struct draw_context dc;
    struct element_touch_event *e;
    const struct ui_grid_info *info;
    const struct element_event_handler *handler;
    // int key_jump;
    u8 key_jump;
    u8 hi_move;
    void (*highlight_ajust)(struct ui_grid *grid, int direction);
    void (*end_once)(struct ui_grid *grid);
};

extern const struct element_event_handler grid_elm_handler;

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_enable
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_enable();
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_on_focus 列表作为焦点控件
 *
 * @param grid 列表控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_on_focus(struct ui_grid *grid);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_lose_focus 列表不作为焦点控件
 *
 * @param grid 列表控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_lose_focus(struct ui_grid *grid);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_state_reset 重置列表状态
 *
 * @param grid 列表控件句柄
 * @param highlight_item 默认高亮项
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_state_reset(struct ui_grid *grid, int highlight_item);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_highlight_item 设置列表高亮项
 *
 * @param grid 列表控件句柄
 * @param item 高亮项索引
 * @param yes 高亮/非高亮
 *
 * @return 0,正常；其他，异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_highlight_item(struct ui_grid *grid, int item, bool yes);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_highlight_item_by_id 设置列表高亮项
 *
 * @param id 列表控件id
 * @param item 高亮项索引
 * @param yes 高亮/非高亮
 *
 * @return 0,正常；其他，异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_highlight_item_by_id(int id, int item, bool yes);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief __ui_grid_new 创建列表控件
 *
 * @param css 控件css属性
 * @param id  控件id
 * @param info 列表项信息
 * @param parent 父控件句柄
 *
 * @return 列表句柄
 */
/* ------------------------------------------------------------------------------------*/
struct ui_grid *__ui_grid_new(struct element_css1 *css, int id, struct ui_grid_item_info *info, struct element *parent);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_child_init 自动化初始化
 *
 * @param grid 列表控件句柄
 * @param info 列表子控件info
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_child_init(struct ui_grid *grid, struct ui_grid_info *info);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_slide_direction 设置列表滑动方向
 *
 * @param grid 列表控件句柄
 * @param dir 列表滑动方向
 *
 * @return 0,正常；其他，异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_slide_direction(struct ui_grid *grid, int dir);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief grid_key_jump 静态垂直或者水平列表才能使用，用于使用按键时，
 * 没有高亮项，但按下按键能看到列表跳动
 * 注意：必须要初始化的回调里面，并且设置了方向之后才能调用
 *
 * @param grid 列表句柄
 * @param en 是否使能该模式
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int grid_key_jump(struct ui_grid *grid, int en);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_pix_scroll 设置列表跟手滑动使能
 *
 * @param grid 列表控件句柄
 * @param enable true 跟手滑动：false 按项滚动
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_pix_scroll(struct ui_grid *grid, int enable);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_slide 列表滑动
 *
 * @param grid 列表控件句柄
 * @param direction 列表滑动方向
 * @param steps 滑动距离
 *
 * @return true 成功 ；false 失败
 *
 * @note 自动刷新
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_slide(struct ui_grid *grid, int direction, int steps);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_slide_with_callback 静态列表滑动接口_with_callback
 *
 * @param grid 列表控件句柄
 * @param direction 列表滑动方向
 * @param steps 列表滑动距离
 * @param callback 滑动后回调，一般需要在回调调用redraw
 *
 * @return true 成功 ；false 失败
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_slide_with_callback(struct ui_grid *grid, int direction, int steps, void(*callback)(void *ctrl));
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_item_num 设置列表项的数量
 *
 * @param grid 列表控件句柄
 * @param item_num 项的数量
 *
 * @return 0,正常；其他，异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_item_num(struct ui_grid *grid, int item_num);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_scroll_area 设置列表滑动区域
 *
 * @param grid 列表控件句柄
 * @param area 滑动区域 *传入全局变量或局部静态变量
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_set_scroll_area(struct ui_grid *grid, struct scroll_area *area);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_touch_item 列表触摸项
 *
 * @param grid 列表控件句柄
 *
 * @return 触摸项索引；空白区域=-1
 */
/* ------------------------------------------------------------------------------------*/
static inline int ui_grid_touch_item(struct ui_grid *grid)
{
    return grid->touch_index;
}
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_get_hindex 获取列表高亮项
 *
 * @param grid 列表控件句柄
 *
 * @return 静态高亮项
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_get_hindex(struct ui_grid *grid);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_cur_item 列表选中项
 *
 * @param grid 列表控件句柄
 *
 * @return 存在触摸时反回触摸项；否则返回高亮项
 */
/* ------------------------------------------------------------------------------------*/
static inline int ui_grid_cur_item(struct ui_grid *grid)
{
    if (grid->touch_index >= 0) {
        return grid->touch_index;
    }
    return grid->hi_index;
}

#define ui_grid_set_item(grid, index)  	(grid)->hi_index = index
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_hi_index 设置列表高亮项
 *
 * @param grid 列表控件句柄
 * @param hi_index 高亮项索引值
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_hi_index(struct ui_grid *grid, int hi_index);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_flick_ctrl_close 列表自动回弹功能开关
 *
 * @param grid 列表控件句柄
 * @param flag 是否关闭标志（1关闭，0不关闭）
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_flick_ctrl_close(struct ui_grid *grid, u8 flag);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_tslide 列表惯性/回弹滑动时间间隔
 *
 * @param grid 列表控件句柄
 * @param tslide 时间间隔ms,默认：回弹50ms 惯性40ms
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_tslide(struct ui_grid *grid, u8 tslide);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_energy 惯性滑动参数设置
 *
 * @param grid 列表控件句柄
 * @param a 惯性负加速度 默认-0.01
 * @param val 惯性系数，用于压缩滑动距离，默认0.3
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_energy(struct ui_grid *grid, float a, float val);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_energy_auto_center 列表滑动居中使能
 *
 * @param grid 列表控件句柄
 * @param auto_center_enable MODE/DISABLE(0)
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_energy_auto_center(struct ui_grid *grid, AUTO_CENTER_MODE auto_center_mode);//是否启动滑动居中
#define grid_energy_auto_center(a, b)	ui_grid_energy_auto_center(a, b)

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_energy_item_offset 设置最大偏移项的数量
 *
 * @param grid 列表句柄
 * @param item_offset 默认值为0，设置为1时，每次滑动只能滑动1项
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_energy_item_offset(struct ui_grid *grid, int item_offset);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_energy_target_line 设置自动居中的目标线
 *
 * @param grid 列表控件句柄
 * @param target_line 目标线(默认值为父控件的一半；例如屏高454，项高112，间隔2，第二项高亮居中，则target_line=(112+2+(112/2))。)
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_energy_target_line(struct ui_grid *grid, int target_line);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_energy_next_threshold 滑动居中偏移到下一项阈值
 *
 * @param grid 列表控件句柄
 * @param next_threshold 跟随模式配置(默认值0,1000 移动距离超过父控件宽/高的10%时，居中下一项, 否则回弹)
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_energy_next_threshold(struct ui_grid *grid, int next_threshold);

int ui_grid_auto_center_set_custom_param(struct ui_grid *grid, float a, float val, u8 tslide, int target_line, int item_offset, int next_threshold, int index_mode);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_hi_move 设置静态列表滑动之后,刷新前,调整高亮项
 *
 * @param grid 列表控件句柄
 * @param hi_move true 使能；false 关闭
 * @param hi_func 回调，可自行处理逻辑；NULL 默认逻辑
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_hi_move(struct ui_grid *grid, u8 hi_move, void (*hi_func)(struct ui_grid *grid, int direction));

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_end_once 设置列表滑动(含惯性、回弹)结束的回调函数
 *
 * @param grid 列表控件句柄
 * @param end_once 回调
 */
/* ------------------------------------------------------------------------------------*/
void ui_grid_set_end_once(struct ui_grid *grid, void (*end_once)(struct ui_grid *grid));
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_dynamic_create 动态列表创建*旧接口
 *
 * @param grid 列表控件句柄
 * @param direction 滑动方向
 * @param list_total 项数
 * @param event_handler_cb 列表项的子控件onchange事件回调
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_dynamic_create(struct ui_grid *grid, int direction, int list_total, int (*event_handler_cb)(void *, int, int, int));
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_dynamic_release 动态列表释放*旧接口
 *
 * @param grid 列表控件句柄
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_dynamic_release(struct ui_grid *grid);//动态列表释放

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_dynamic_cur_item 动态列表获取选项
 *
 * @param grid 列表句柄
 *
 * @return -1 异常；有触摸，触摸项；无触摸，高亮项
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_dynamic_cur_item(struct ui_grid *grid);//动态列表获取选项
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_dynamic_set_item_by_id 修改动态列表项数*旧接口
 *
 * @param id 列表id
 * @param count 项数
 *
 * @return  0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_dynamic_set_item_by_id(int id, int count);//修改动态列表数
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_dynamic_reset 重置动态列表*旧接口
 *
 * @param grid
 * @param index
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_dynamic_reset(struct ui_grid *grid, int index); //重置动态列表
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_init_dynamic 初始化动态列表 *新接口
 *
 * @param grid 列表控件句柄
 * @param row 行数
 * @param col 列数
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_init_dynamic(struct ui_grid *grid, int *row, int *col);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_add_dynamic 增加动态项
 *
 * @param grid 列表控件句柄
 * @param row 增加行数
 * @param col 增加列数
 * @param redraw 1 刷新 ；0 不刷新
 *
 * @return  0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_add_dynamic(struct ui_grid *grid, int *row, int *col, int redraw);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_add_dynamic_by_id 增加动态列表项数 *新接口
 *
 * @param id 列表id
 * @param row 增加行数
 * @param col 增加列数
 * @param redraw 是否刷新
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_add_dynamic_by_id(int id, int *row, int *col, int redraw);/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_del_dynamic 删除动态项
 *
 * @param grid 列表控件句柄
 * @param row 减少行数
 * @param col 减少列数
 * @param redraw 1 刷新 ；0 不刷新
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_del_dynamic(struct ui_grid *grid, int *row, int *col, int redraw);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_del_dynamic_by_id 减少动态列表项数 *新接口
 *
 * @param id 列表id
 * @param row 减少的行数
 * @param col 减少的列数
 * @param redraw 是否刷新
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_del_dynamic_by_id(int id, int *row, int *col, int redraw);/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_dynamic_slide 动态列表滑动
 *
 * @param grid 列表控件句柄
 * @param direction 列表滑动方向
 * @param steps 列表滑动距离
 *
 * @return true 成功 ；false 失败
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_dynamic_slide(struct ui_grid *grid, int direction, int steps);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_slide_with_callback_dynamic 动态列表滑动
 *
 * @param grid 列表控件句柄
 * @param direction 滑动方向
 * @param steps 滑动距离
 * @param callback 回调函数，一般需要在回调中调用ui_core_redraw刷新
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_slide_with_callback_dynamic(struct ui_grid *grid, int direction, int steps, void(*callback)(void *ctrl));

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_get_hindex_dynamic 获取动态高亮项
 *
 * @param grid 列表控件句柄
 *
 * @return 动态高亮项
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_get_hindex_dynamic(struct ui_grid *grid);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_cur_item_dynamic 获取动态选择项
 *
 * @param grid 列表控件句柄
 *
 * @return 返回触摸项或高亮项
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_cur_item_dynamic(struct ui_grid *grid);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_hindex_dynamic 设置动态高亮项
 *
 * @param grid 列表控件句柄
 * @param dhindex 初始化时需要跳转到的动态高亮项
 * @param init 释放初始化
 * @param hi_index init为1时有效,表示要将dhindex设置到真实列表项的第几项
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_hindex_dynamic(struct ui_grid *grid, int dhindex, int init, int hi_index);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_set_base_dynamic 设置动态列表初始步进
 *
 * @param grid 列表控件句柄
 * @param base_index_once 0~max_dynamic_index,动态列表初始化时一次性索引基础值，一般配置为ui_grid_set_hindex_dynamic函数参数的(dhindex - hi_index),如果dhindex < hi_index,则需要根据列表项本身来调整

 * @param init_step_once 初始化记忆之后，在第一次刷新之前，需要移动的步进
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_set_base_dynamic(struct ui_grid *grid, u32 base_index_once, int init_step_once);
/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_grid_update_by_id_dynamic 更新动态列表高亮项*新接口
 *
 * @param id 列表id
 * @param item_sel 选中项
 * @param redraw 是否刷新
 *
 * @return 0 正常；其他 异常
 */
/* ------------------------------------------------------------------------------------*/
int ui_grid_update_by_id_dynamic(int id, int item_sel, int redraw);



#endif



