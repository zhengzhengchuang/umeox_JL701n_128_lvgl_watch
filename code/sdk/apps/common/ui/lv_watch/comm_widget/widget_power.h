#ifndef __WIDGET_POWER_H__
#define __WIDGET_POWER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Power_Obj_Max (4)
#define Power_Group_Max (5)

typedef struct
{  
    int16_t x, y;
    int16_t inv;
    bool symb_disp;//是否显示百分比符号 
    lv_obj_t *parent;
    uint32_t file_idx;
    widget_data_align_t align;
}PowerPara_t;
extern PowerPara_t PowerPara;

void WidgetPowerInit(void);
void WidgetPowerRefresh(void);
int16_t WidgetPowerCreate(PowerPara_t *ppower);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
