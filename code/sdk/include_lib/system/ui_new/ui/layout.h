/**
 * @file layout.h
 * @brief 布局控件API头文件
 * @author
 * @version
 * @date 2022-12-14
 */

#ifndef LAYOUT_H
#define LAYOUT_H


#include "ui/ui_core.h"
#include "ui/control.h"





/* ------------------------------------------------------------------------------------*/
/**
 * @brief 布局控件句柄结构
 */
/* ------------------------------------------------------------------------------------*/
struct layout {
    struct element elm; 	//must be first
    u8 hide: 1;
    u8 inited: 1;
    u8 release: 6;
    // u8 css_num:5;
    // u32 css[2];
    u8 page;

    u8 movable;		// 移动标志，lua使用

    /** 满天星菜单相关 **/
    u8 star_menu;	// 是否为满天星菜单标志
    struct list_head *icon_root;	// 满天星图标列表
    int lcd_w;	// 屏幕宽度
    int lcd_h;	// 屏幕高度

    struct element_luascript_t *lua;
    struct layout *layout;
    const struct layout_info *info;
    const struct element_event_handler *handler;
};



/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_for_id 通过布局ID获取布局控件句柄
 *
 * @param id 布局控件ID
 *
 * @return layout布局控件句柄
 */
/* ------------------------------------------------------------------------------------*/
#define layout_for_id(id) \
		(struct layout *)ui_core_get_element_by_id(id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_new 创建layout控件（内部调用）
 *
 * @param info 布局控件信息
 * @param num 布局控件数量
 * @param parent 父控件句柄
 *
 * @return layout 布局控件句柄
 */
/* ------------------------------------------------------------------------------------*/
struct layout *layout_new(struct layout_info *, int, struct element *);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_delete_probe 布局控件准备删除（内部调用）
 *
 * @param layout 带删除布局控件
 * @param num 布局控件数量
 */
/* ------------------------------------------------------------------------------------*/
void layout_delete_probe(struct layout *layout, int num);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_delete 删除布局控件（内部调用）
 *
 * @param layout 布局控件句柄
 * @param num 布局控件数量
 */
/* ------------------------------------------------------------------------------------*/
void layout_delete(struct layout *layout, int num);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_show 显示布局控件
 *
 * @param id 布局控件ID
 *
 * @return 0 正常，-22控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int layout_show(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_hide 隐藏布局控件
 *
 * @param id 布局控件ID
 *
 * @return 0 正常，-22控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int layout_hide(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_toggle 布局控件显示状态切换，显示时切换为隐藏，隐藏时切换为显示
 *
 * @param id 布局控件ID
 *
 * @return 0 隐藏，1 显示，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int layout_toggle(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_on_focus 布局控件聚焦，将指定布局控件设置为焦点
 *
 * @param layout 布局控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void layout_on_focus(struct layout *layout);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layout_lose_focus 布局控件失焦，将布局控件焦点释放
 *
 * @param layout 布局控件句柄
 */
/* ------------------------------------------------------------------------------------*/
void layout_lose_focus(struct layout *layout);

int custom_draw(void *_layout);

/*int layout_current_highlight(int id);*/

/*int layout_onkey(struct layout *layout, struct element_key_event *e);*/



#endif


