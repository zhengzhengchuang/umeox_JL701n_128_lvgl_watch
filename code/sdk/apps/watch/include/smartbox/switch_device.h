#ifndef __SMARTBOX_SWITCH_DEVICE_H__
#define __SMARTBOX_SWITCH_DEVICE_H__

#include "typedef.h"
#include "app_config.h"

void switch_device(void *priv, u8 OpCode, u8 OpCode_SN, u8 *data, u16 len);
u8 get_rcsp_support_new_reconn_flag(void);

#endif//__SMARTBOX_FEATURE_H__

