/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_page_switch.h
 * @brief 卡片页面滑动管理模块API头文件
 * @author
 * @version V201
 * @date 2022-12-15
 */

#ifndef __UI_PAGE_SWITCH_H__
#define __UI_PAGE_SWITCH_H__


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 滑动方向定义
 */
/* ------------------------------------------------------------------------------------*/
enum {
    DIRECTION_NONE,		// 滑动方向未知
    DIRECTION_UP,		// 向上滑动
    DIRECTION_DOWN,		// 向下滑动
    DIRECTION_LEFT,		// 向左滑动
    DIRECTION_RIGHT,	// 向右滑动
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_set_param 配置卡片页面滑动参数
 *
 * @param threshold 滑动阈值（沿某方向滑动距离超过阈值时，move_auto会划向下一页，否则回弹）
 * @param step 滑动步距（滑动时每次移动的步距）
 * @param delay 自动滑动定时器时间（delay * 10 ms）
 */
/* ------------------------------------------------------------------------------------*/
void ui_page_set_param(int threshold, int step, int delay);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_move 界面滑动处理
 *
 * @param curr_win 当前页面ID
 * @param xoffset X方向的移动量
 * @param yoffset Y方向的移动量
 * @param mode 移动模式（暂未开发相关功能）
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int ui_page_move(int curr_win, int xoffset, int yoffset, int mode);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_switch 界面手动切换
 *
 * @param curr_win 当前界面
 * @param next_win 下一个界面
 * @param xoffset X方向移动量
 * @param mode 模式（暂未开发相关功能）
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int ui_page_switch(int curr_win, int next_win, int xoffset, int mode);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_move_auto 卡片界面自动滑动（根据阈值判断自动滑动方向）
 *
 * @param curr_win 当前界面ID
 * @param mode 模式
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int ui_page_move_auto(int curr_win, int mode);
int ui_page_move_stop(int curr_win, int mode);

/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_page_move_callback_run 运行卡片是否使能回调（在卡片滑动时，先获取该回调结果判断是否响应卡片滑动）
 *
 * @return true 响应卡片滑动，false 不响应卡片滑动
 */
/* ------------------------------------------------------------------------------------*/
u8 ui_page_move_callback_run(void);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief get_direction 通过tp两次报点计算滑动方向
 *
 * @param startx 起始X坐标
 * @param starty 起始Y坐标
 * @param endx 结束X坐标
 * @param endy 结束Y坐标
 *
 * @return 滑动方向
 */
/* ------------------------------------------------------------------------------------*/
int get_direction(int startx, int starty, int endx, int endy);


#endif
