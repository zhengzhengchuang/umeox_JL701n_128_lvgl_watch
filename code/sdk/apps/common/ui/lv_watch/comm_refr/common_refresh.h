#ifndef __COMMON_REFRESH_H__
#define __COMMON_REFRESH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void common_refresh_msg_handle(void);
void common_refresh_timer_create(void);
void common_refresh_timer_re_run(void);
void common_refresh_timer_destroy(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
