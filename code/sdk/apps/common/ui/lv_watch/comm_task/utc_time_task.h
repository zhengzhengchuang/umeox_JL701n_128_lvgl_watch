#ifndef __UTC_TIME_TASK_H__
#define __UTC_TIME_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void timer_sec_task_handle(void);

void utc_day_task_handle(int priv);
void utc_minute_task_handle(int priv);
void utc_second_task_handle(int priv);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
