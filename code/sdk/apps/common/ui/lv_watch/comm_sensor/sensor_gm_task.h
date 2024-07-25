#ifndef __SENSOR_GM_TASK_H__
#define __SENSOR_GM_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../../../device/gSensor/qmi8658/qmi8658.h"
#include "../../../device/m_sensor/qmc6309/qmc6309.h"

#define Gm_Task_Name "gm_task"

#define GmGs_Fifo_WM (Qmi8658_Fifo_WM)

enum
{
    GmMsgProcess,
    GmMsgEnable,
    GmMsgDisable,
};

typedef struct
{
    u16 vm_mask;

    bool cali_succ;
    float calipara[4];
}GmCaliInfoPara_t;
extern GmCaliInfoPara_t Cali_Info;

void GmTaskCreate(void);
int PostGmTaskMsg(int *msg, u8 len);
void GmTaskMsgHandle(int *msg, u8 len);

void EnableGmModule(void);
void DisableGmModule(void);

s16 GetGmYawAngle(void);
void SetGmYawAngle(s16 yaw);

bool GetGmEnableFlag(void);
void SetGmEnableFlag(bool f);

bool GetGmCaliSucc(void);
void SetGmCaliSucc(bool f);

void GmGsDataFifoWrite(u8 *w_buf, u32 w_len);
u16 GmGsDataFifoRead(s16 *xdata, s16 *ydata, s16 *zdata, u16 max_len);

void GmCaliInfoRead(void);
void GmCaliInfoWrite(void);
void GmCaliInfoReset(void);
void GmCaliInfoUpdate(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
