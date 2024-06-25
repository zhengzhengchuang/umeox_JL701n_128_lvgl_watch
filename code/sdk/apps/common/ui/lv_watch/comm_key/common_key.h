#ifndef __COMMON_KEY_H__
#define __COMMON_KEY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    Common_Key_Val_0,
    Common_Key_Val_1,
    
    Common_Key_Val_Max,
};

bool get_menu_timer_lock_flag(void);
void set_menu_timer_lock_flag(bool flag);
void common_key_msg_handle(int key_value, int key_event);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
