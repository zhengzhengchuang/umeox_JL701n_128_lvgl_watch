/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file ui_watch.h
 * @brief 表盘控件API接口头文件
 * @author zhuhaifang@zh-jieli.com
 * @version V201
 * @date 2022-12-15
 */
#ifndef UI_WATCH_H
#define UI_WATCH_H


#include "ui/control.h"
#include "ui/ui_core.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 表盘控件子控件数量定义
 */
/* ------------------------------------------------------------------------------------*/
#define WATCH_CHILD_NUM 	(CTRL_WATCH_CHILD_END - CTRL_WATCH_CHILD_BEGIN)


struct watch_pic_info {
    struct ui_ctrl_info_head head;
    u16 cent_x;
    u16 cent_y;
    u16 dst_cent_x;
    u16 dst_cent_y;
    struct ui_image_list *img;
};

struct watch_css_info {
    int left: 16;
    int top: 16;
    int width: 16;
    int height: 16;
};

/* ------------------------------------------------------------------------------------*/
/**
 * @brief 表盘控件句柄结构定义
 */
/* ------------------------------------------------------------------------------------*/
struct ui_watch {
    struct element elm;
    struct element child_elm[WATCH_CHILD_NUM];	// 子控件句柄
    struct watch_css_info child_css[WATCH_CHILD_NUM];	// 子控件属性
    char source[8];	// 数据源
    u8 hour;	// 时
    u8 min;		// 分
    u8 sec;		// 秒
    u8 last_hour;
    u8 last_min;
    u8 last_sec;
    u8 updata;
    u8 ctrl_num;	// 控件数量
    u8 sec_cnt;		// 秒计数
    u8 slow_sec;	// 扫秒模式开关（true开启，false关闭）
    void *timer;
    const struct layout_info *info;
    const struct watch_pic_info *pic_info[WATCH_CHILD_NUM];
    const struct element_event_handler *handler;
    const struct element_luascript_t *lua;
};



/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_watch_enable 表盘控件使能（内部调用）
 */
/* ------------------------------------------------------------------------------------*/
void ui_watch_enable();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_watch_set_time_by_id 设置表盘控件实际（待redraw）
 *
 * @param id 表盘控件ID
 * @param hour 小时
 * @param min 分钟
 * @param sec 秒钟
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_watch_set_time_by_id(int id, int hour, int min, int sec);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_watch_set_time 设置表盘控件时间（不带redraw）
 *
 * @param watch表盘控件句柄
 * @param hour 小时
 * @param min 分钟
 * @param sec 秒钟
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int ui_watch_set_time(struct ui_watch *watch, int hour, int min, int sec);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_watch_update 表盘控件主动刷新
 *
 * @param watch 表盘控件句柄
 * @param refresh 是否调用redraw刷新
 */
/* ------------------------------------------------------------------------------------*/
void ui_watch_update(struct ui_watch *watch, u8 refresh);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief ui_watch_slow_sec_by_id 表盘控件启动扫秒模式（设置扫秒标志位）
 *
 * @param id 表盘控件ID
 * @param flag 是否使用扫秒模式（true 是，false 否）
 */
/* ------------------------------------------------------------------------------------*/
void ui_watch_slow_sec_by_id(int id, u8 flag);



#endif


