#ifndef __GS_TASK_H__
#define __GS_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../../../device/gSensor/qmi8658/qmi8658.h"

#define Gs_Task_Name "gs_task"

#define Gs_Fifo_Num  (1)
#define Gs_Fifo_Size (Qmi8658_Fifo_WM*Gs_Fifo_Num*6)

enum
{
    GsMsgProcess,
    GsMsgEnable,
    GsMsgDisable,
};

void GsTaskCreate(void);
int PostGsTaskMsg(int *msg, u8 len);
void GsTaskMsgHandle(int *msg, u8 len);

void EnableGsModule(void);
void DisableGsModule(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
