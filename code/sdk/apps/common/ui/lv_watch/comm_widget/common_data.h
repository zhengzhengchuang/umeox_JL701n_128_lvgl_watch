#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../comm_nor_vm/nor_vm_weather.h"

/*********************************************************************************
                                  心率  示例:102bmp                                
*********************************************************************************/
#define Hr_Obj_Max (3)
#define Hr_Group_Max (6)


/*********************************************************************************
                                  血氧  示例:100%                                      
*********************************************************************************/
#define Bo_Obj_Max (4)
#define Bo_Group_Max (6)


/*********************************************************************************
                                  步数  示例:88888steps                                    
*********************************************************************************/
#define Step_Obj_Max (5)
#define Step_Group_Max (3)


/*********************************************************************************
                                  卡路里(千卡)  示例:8888kcal                                 
*********************************************************************************/
#define Calorie_Obj_Max (4)
#define Calorie_Group_Max (3)


/*********************************************************************************
                                  配速  示例:99'99''                                    
*********************************************************************************/
#define Pace_Obj_Max (6)
#define Pace_Group_Max (3)


/*********************************************************************************
                                  距离  示例:99.99km                               
*********************************************************************************/
#define Distance_Obj_Max (5)
#define Distance_Group_Max (3)

/*********************************************************************************
                                  天气  示例:-88°C  -888°F                           
*********************************************************************************/
#define Weather_Obj_Max (5)
#define Weather_Group_Max (2*(Weather_Sync_Days+1) + 2)//8组最大最小温度+2组实时温度


/*********************************************************************************
                                  通用数据类型枚举                                       
*********************************************************************************/
enum
{
    widget_data_type___ = 0x00,

    widget_data_type_hr,             /*数据类型：心率值*/

    widget_data_type_bo,             /*数据类型：血氧值*/

    widget_data_type_step,           /*数据类型：步数*/
    widget_data_type_calorie,        /*数据类型：卡路里(千卡)*/
    widget_data_type_pace,           /*数据类型：配速*/
    Widget_Data_type_distance,       /*数据类型：距离*/

    widget_data_type_weather,        /*数据类型：实时天气*/
    widget_data_type_min0_weather,  /*数据类型：第一天最小温度*/
    widget_data_type_max0_weather,  /*数据类型：第一天最大温度*/
    widget_data_type_min1_weather,  /*数据类型：第二天最小温度*/
    widget_data_type_max1_weather,  /*数据类型：第二天最大温度*/
    widget_data_type_min2_weather,  /*数据类型：第三天最小温度*/
    widget_data_type_max2_weather,  /*数据类型：第三天最大温度*/
    widget_data_type_min3_weather,  /*数据类型：第四天最小温度*/
    widget_data_type_max3_weather,  /*数据类型：第四天最大温度*/
    widget_data_type_min4_weather,  /*数据类型：第五天最小温度*/
    widget_data_type_max4_weather,  /*数据类型：第五天最大温度*/
    widget_data_type_min5_weather,  /*数据类型：第六天最小温度*/
    widget_data_type_max5_weather,  /*数据类型：第六天最大温度*/
    widget_data_type_min6_weather,  /*数据类型：第七天最小温度*/
    widget_data_type_max6_weather,  /*数据类型：第七天最大温度*/
};
typedef uint8_t widget_data_type_t;

/*********************************************************************************
                                  通用数据对齐枚举                                       
*********************************************************************************/
enum
{
    widget_data_align____ = 0x00,
    widget_data_align_left,
    widget_data_align_right,
    widget_data_align_center,
};
typedef uint8_t widget_data_align_t;

/*********************************************************************************
                                  通用数据参数集合                                      
*********************************************************************************/
typedef struct
{
    int16_t data_x;
    int16_t data_y;
    int16_t num_inv;
    lv_obj_t *data_parent;
    uint32_t num_addr_index;
    widget_data_align_t data_align;

    /*********后续扩展参数*************/
    bool user0_para_valid;
    uint32_t user0_num_addr_index;//有些控件需要在特定条件更换数字组，如果无，忽略此参数
}widget_data_para_t;
extern widget_data_para_t widget_data_para;

void common_data_refresh(void);
void common_data_widget_init(void);
int16_t common_data_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
