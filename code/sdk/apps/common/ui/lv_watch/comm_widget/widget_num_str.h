#ifndef __WIDGET_NUM_STR_H__
#define __WIDGET_NUM_STR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                            数字字符串参数集合(适用于固定格式)                                     
*********************************************************************************/
typedef struct
{
    lv_obj_t *parent;

    int16_t num_obj_x;
    int16_t num_obj_y;

    char *p_num_str;
    uint8_t num_str_len; 

    lv_obj_t **num_obj;
    uint8_t num_obj_max;
    uint16_t *num_dsc_idx;

    uint32_t file_00_index;
}widget_num_str_para_t;
extern widget_num_str_para_t num_str_para;

int16_t common_widget_num_str_create(\
    widget_num_str_para_t *num_str_para);
void common_widget_num_str_refresh(lv_obj_t **num_obj, uint8_t obj_max, \
    uint16_t *num_dsc_idx, char *p_num_str, uint8_t str_len, uint32_t index);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
