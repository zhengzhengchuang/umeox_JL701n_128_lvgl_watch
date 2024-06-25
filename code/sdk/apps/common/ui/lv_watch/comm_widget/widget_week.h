#ifndef __WIDGET_WEEK_H__
#define __WIDGET_WEEK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  一组星期占用的最大obj个数                                       
*********************************************************************************/
#define Week_Obj_Max (1)

/*********************************************************************************
                                  星期组数最大                                       
*********************************************************************************/
#define Week_Group_Max (3)

/*********************************************************************************
                                  星期枚举                                       
*********************************************************************************/
enum
{
    Comm_Enum_Week_Sun = 0x00,
    Comm_Enum_Week_Mon,
    Comm_Enum_Week_Tue,
    Comm_Enum_Week_Wed,
    Comm_Enum_Week_Thu,
    Comm_Enum_Week_Fri,
    Comm_Enum_Week_Sat, 

    Comm_Enum_Week_Max, 
};
typedef uint8_t comm_enum_week_t;

/*********************************************************************************
                                  星期控件参数集合                                       
*********************************************************************************/
typedef struct
{
    int16_t week_x;
    int16_t week_y;
    lv_obj_t *week_parent;
    uint32_t week_addr_index;
}comm_week_para_t;
extern comm_week_para_t comm_week_para;

void common_week_widget_init(void);
void common_week_widget_refresh(void);
void common_week_widget_create(comm_week_para_t \
    *week_para);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
