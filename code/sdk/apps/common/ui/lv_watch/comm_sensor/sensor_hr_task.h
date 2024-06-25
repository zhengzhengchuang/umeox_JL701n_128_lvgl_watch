#ifndef __SENSOR_HR_TASK_H__
#define __SENSOR_HR_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../../../device/hr_sensor/gh3018/Drv_Code/inc/gh30x_example_common.h"

#define Sensor_Hr_Task_Name \
    "sensor_hr_task"

#define Hr_Data_Filter_Cnt 5

enum
{
    SensorWorkHr, //心率
    SensorWorkBo, //血氧

    SensorWorkNone, //无工作
};
typedef u8 SensorWorkType_t;

enum
{
    SensorModeAuto,   //自动
    SensorModeManual, //手动
};
typedef u8 SensorWorkMode_t;

enum
{
    SensorHrProcess,
    SensorHrEnableModule,
    SensorHrDisableModule,
};

void SensorHrTaskCreate(void);
void SensorHrTaskHandle(int *rev_msg, u8 len);
int PostSensorHrTaskMsg(int *post_msg, u8 len);

u8 GetHrSensorMode(void);
void SetHrSensorMode(u8 mode);

u8 GetHrSensorWorkType(void);
void SetHrSensorWorkType(u8 type);

u8 GetHrSensorDataFilterCnt(void);
void SetHrSensorDataFilterCnt(u8 Cnt);

bool GetHrSensorEnableFlag(void);
void SetHrSensorEnableFlag(bool en);

bool GetHrSensorWearStatus(void);
void SetHrSensorWearStatus(bool status);

void HrSensorStopSample(void);
void HrSensorStartSample(SensorWorkType_t type, \
    SensorWorkMode_t mode);

void HrGsDataFifoWrite(u8 *w_buf, u32 w_len);
void HrGsDataFifoRead(ST_GS_DATA_TYPE *Gs_data, u16 *r_idx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
