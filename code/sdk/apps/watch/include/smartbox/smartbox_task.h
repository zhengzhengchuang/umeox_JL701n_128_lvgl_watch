#ifndef __SMARTBOX_TASK_H__
#define __SMARTBOX_TASK_H__

#include "typedef.h"
#include "app_config.h"

enum {
    SMARTBOX_TASK_ACTION_FILE_TRANSFER = 0x0,
    SMARTBOX_TASK_ACTION_FILE_DELETE,
    SMARTBOX_TASK_ACTION_DEV_FORMAT,
    SMARTBOX_TASK_ACTION_WATCH_TRANSFER,
    SMARTBOX_TASK_ACTION_RESET_EX_FLASH,
    SMARTBOX_TASK_ACTION_BLUK_TRANSFER,
};

void app_smartbox_task_prepare(u8 type, u8 action, u8 OpCode_SN);
void app_smartbox_task(void);

void app_smartbox_task_switch_stop(void);

#endif//__SMARTBOX_TASK_H__


