#ifndef __COMMON_SEND_H__
#define __COMMON_SEND_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void umeox_common_le_reply_fail(u8 *buf, u8 len);
bool umeox_common_le_notify_data(u8 *buf, u8 len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
