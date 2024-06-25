#ifndef __WIDGET_STEP_H__
#define __WIDGET_STEP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common_data.h"
#include "../include/ui_menu.h"

/*********************************************************************************
                                  步数显示最大值                                    
*********************************************************************************/
#define Step_Disp_Max (99999)

void common_step_widget_init(void);
void common_step_widget_refresh(void);
int16_t common_step_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
