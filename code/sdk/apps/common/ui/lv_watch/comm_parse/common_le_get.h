#ifndef __COMMON_LE_GET_H__
#define __COMMON_LE_GET_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "../include/ui_menu.h"

//设备主动上报类型集合
enum
{
    Le_Event_Low_Power = 0x00,    //低电提醒
    Le_Event_Not_Wear,            //未佩戴
    Le_Event_Fall_Off,            //脱落
    Le_Event_Hr_High,             //心率过高
    Le_Event_Hr_Low,              //心率过低
    Le_Event_Req_Weather,         //请求天气
    Le_Event_Find_Phone,          //查找手机

    Le_Event_Num,
};
typedef u8 le_event_t;

u8 calc_crc(u8 *buf, u8 crc_idx);

void RemoteGetDeviceInfo(u8 *buf, u8 len);
void RemoteGetUnitFormat(u8 *buf, u8 len);
void RemoteGetBatteryPower(u8 *buf, u8 len);
void RemoteGetDndMode(u8 *buf, u8 len);
void RemoteGetAlarmInfo(u8 *buf, u8 len);
void RemoteGetHistoryHrData(u8 *buf, u8 len);
void RemoteGetHistoryBoData(u8 *buf, u8 len);
void RemoteGetHistoryActivityData(u8 *buf, u8 len);
void RemoteGetHistorySleepData(u8 *buf, u8 len);
void RemoteGetActivityData(u8 *buf, u8 len);
void RemoteGetSpecFuncSw(u8 *buf, u8 len);
void RemoteGetDevEvents(le_event_t event);
void RemoteGetHealthMonitorPara(u8 *buf, u8 len);
void RemoteGetANCSSw(u8 *buf, u8 len);
void RemoteGetSedentaryInfo(u8 *buf, u8 len);
void DevSendCameraOpCmd(u8 OpCmd);
void DevSendRmusicOpCmd(u8 OpCmd, u8 *para);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
