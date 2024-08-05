#ifndef __COMM_TASK_H__
#define __COMM_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Comm_Task_Name "comm_task"

enum
{
    comm_msg_gomore_init,

    comm_msg_utcday_handle,
    comm_msg_utcmin_handle,
    comm_msg_utcsec_handle,

    comm_msg_timersec_handle,

    comm_msg_stopwatch_over,
    comm_msg_countdown_timeout,

    comm_msg_dev_shutdown,
    comm_msg_dev_restart,
    comm_msg_dev_reset,
};

void CommTaskCreate(void);
int PostCommTaskMsg(int *msg, u8 len);
void CommTaskMsgHandle(int *msg, u8 len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
