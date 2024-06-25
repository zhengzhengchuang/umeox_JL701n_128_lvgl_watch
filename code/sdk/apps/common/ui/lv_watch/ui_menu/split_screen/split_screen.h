#ifndef __SPLIT_SCREEN_H__
#define __SPLIT_SCREEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#if UI_USE_SPLIT_SCREEN
void menu_split_screen_create(lv_obj_t *obj);
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
