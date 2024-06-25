/**
* @file layer.h
* @brief 图层控件API头文件
* @author
* @version V2.0.1
* @date 2022-12-14
*/
#ifndef LAYER_H
#define LAYER_H


#include "ui/layout.h"
#include "ui/control.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 图层控件句柄结构形式
 */
/* ------------------------------------------------------------------------------------*/
struct layer {
    struct element elm; 		//must be first
    u8 hide;
    u8 inited;
    u8 highlight;
    u8 ctrl_num;
    u8 css_num;	// 属性数量
    u32 css[2];
    struct draw_context dc;
    struct layout *layout;
    struct element_luascript_t *lua;
    const struct layer_info *info;
    const struct element_event_handler *handler;
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_for_id 通过图层ID获取图层控件句柄
 *
 * @param id 图层控件ID
 *
 * @return layer 图层控件句柄
 */
/* ------------------------------------------------------------------------------------*/
#define layer_for_id(id) \
		(struct layer *)ui_core_get_element_by_id(id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_new 创建图层（内部调用）
 *
 * @param info 图层控件信息
 * @param num 图层数量
 * @param parent 父控件
 *
 * @return layer 图层句柄
 */
/* ------------------------------------------------------------------------------------*/
struct layer *layer_new(struct layer_info *info, int num, struct element *parent);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_delete_probe 准备删除图层（内部调用）
 *
 * @param layer
 * @param num
 */
/* ------------------------------------------------------------------------------------*/
void layer_delete_probe(struct layer *layer, int num);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_delete 删除图层（内部调用）
 *
 * @param layer
 * @param num
 */
/* ------------------------------------------------------------------------------------*/
void layer_delete(struct layer *layer, int num);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_show 显示图层控件
 *
 * @param id 图层控件ID
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int layer_show(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_hide 隐藏图层控件
 *
 * @param id 图层控件ID
 *
 * @return 0 正常，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int layer_hide(int id);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief layer_toggle 图层的显示、隐藏状态切换。原来为显示则设置为隐藏，原来为隐藏则设置为显示。
 *
 * @param id 图层控件的ID
 *
 * @return 0 隐藏，1 显示，-22 控件不存在
 */
/* ------------------------------------------------------------------------------------*/
int layer_toggle(int id);









#endif




