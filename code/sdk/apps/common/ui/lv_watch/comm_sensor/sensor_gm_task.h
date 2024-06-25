#ifndef __SENSOR_GM_TASK_H__
#define __SENSOR_GM_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../../../device/gSensor/qmi8658/qmi8658.h"
#include "../../../device/m_sensor/qmc6309/qmc6309.h"

#define Sensor_Gm_Task_Name \
    "sensor_gm_task"

#define GmGs_Fifo_WM (Qmi8658_Fifo_WM)

enum
{
    SensorGmMsgProcess,
    SensorGmMsgEnableModule,
    SensorGmMsgDisableModule,
};

typedef struct
{
    u16 vm_mask;

    bool cali_succ;
    float calipara[4];
}GmCaliInfoPara_t;
extern GmCaliInfoPara_t Cali_Info;

void SensorGmTaskCreate(void);
int PostSensorGmTaskMsg(int *post_msg, u8 len);
void SensorGmTaskMsgHandle(int *rev_msg, u8 len);

s16 GetSensorGmYaw(void);
void SetSensorGmYaw(s16 yaw);

bool GetSensorGmEnableFlag(void);
void SetSensorGmEnableFlag(bool f);

bool GetSensorGmCaliSucc(void);
void SetSensorGmCaliSucc(bool f);

void EnableSensorGmModule(void);
void DisableSensorGmModule(void);

void SensorGmProcess(void);

void SensorGmGsDataFifoWrite(u8 *w_buf, u32 w_len);
u16 SensorGmGsDataFifoRead(s16 *xdata, s16 *ydata, \
    s16 *zdata, u16 r_max_num);

void SensorGmCaliInfoRead(void);
void SensorGmCaliInfoWrite(void);
void SensorGmCaliInfoReset(void);
void SensorGmCaliInfoUpdate(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
