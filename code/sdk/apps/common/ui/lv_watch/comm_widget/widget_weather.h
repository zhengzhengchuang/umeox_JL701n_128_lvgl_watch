#ifndef __WIDGET_WEATHER_H__
#define __WIDGET_WEATHER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common_data.h"
#include "../include/ui_menu.h"

void common_weather_widget_init(void);
void common_weather_widget_refresh(void);
int16_t common_weather_widget_create(widget_data_para_t *data_para, \
    widget_data_type_t type, void *data_val);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
