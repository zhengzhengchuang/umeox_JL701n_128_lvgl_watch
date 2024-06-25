#ifndef __WIDGET_DATE_H__
#define __WIDGET_DATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  一组日期占用的最大obj个数                                       
*********************************************************************************/
#define Date_Obj_Max (10)

/*********************************************************************************
                                  日期组数最大                                       
*********************************************************************************/
#define Date_Group_Max (3)

/*********************************************************************************
                                  日期显示模式枚举                                      
*********************************************************************************/
enum
{
    widget_date_mode___ = 0x00,           /*显示模式   无*/
    widget_date_mode_yyyy,                /*显示模式   年*/
    widget_date_mode_mm,                  /*显示模式   月*/
    widget_date_mode_dd,                  /*显示模式   日*/
    widget_date_mode_yyyy_mm,             /*显示模式   年-月*/
    widget_date_mode_mm_dd,               /*显示模式   月-日*/
    widget_date_mode_yyyy_mm_dd,          /*显示模式   年-月-日 示例:2014-12-22*/ 
};
typedef uint8_t widget_date_mode_t;

/*********************************************************************************
                                  日期控件参数集合                                       
*********************************************************************************/
typedef struct
{  
    int16_t date_x;
    int16_t date_y;
    int16_t num_inv; 
    lv_obj_t *date_parent;
    uint32_t num_addr_index;
}widget_date_para_t;
extern widget_date_para_t widget_date_para;

void common_date_widget_init(void);
void common_date_widget_refresh(void);
void common_date_widget_create(widget_date_para_t *date_para, \
    widget_date_mode_t date_mode);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
