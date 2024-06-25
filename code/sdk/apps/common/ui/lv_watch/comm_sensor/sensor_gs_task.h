#ifndef __SENSOR_GS_TASK_H__
#define __SENSOR_GS_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../../../device/gSensor/qmi8658/qmi8658.h"

#define Sensor_Gs_Task_Name \
    "sensor_gs_task"

#define Gs_Fifo_Num  (1)
#define Gs_Fifo_Size \
    (Qmi8658_Fifo_WM*Gs_Fifo_Num*6)

enum
{
    SensorGsMsgProcess,
    SensorGsMsgEnableModule,
    SensorGsMsgDisableModule,
};

void SensorGsTaskCreate(void);
int PostSensorGsTaskMsg(int *post_msg, u8 len);
void SensorGsTaskMsgHandle(int *rev_msg, u8 len);

void EnableSensorGsModule(void);
void DisableSensorGsModule(void);

void SensorGsIntProcess(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
