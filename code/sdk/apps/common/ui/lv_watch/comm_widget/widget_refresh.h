#ifndef __WIDGET_REFRESH_H__
#define __WIDGET_REFRESH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void widget_refresh_init(void);
void widget_refresh_handle(void);
void widget_refresh_timer_create(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
