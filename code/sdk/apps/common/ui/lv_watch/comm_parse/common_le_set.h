#ifndef __COMMON_LE_SET_H__
#define __COMMON_LE_SET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void RemoteSetUtcTime(u8 *buf, u8 len);
void RemoteSetUnitFormat(u8 *buf, u8 len);
void RemoteDeviceOpHandle(u8 *buf, u8 len);
void RemoteSetDndMode(u8 *buf, u8 len);
void RemoteSetAlarmInfo(u8 *buf, u8 len);
void RemoteFindDev(u8 *buf, u8 len);
void RemoteSetUserInfo(u8 *buf, u8 len);
void RemoteSetSpecFuncSw(u8 *buf, u8 len);
void RemoteModifyGalgoKey(u8 *buf, u8 len);
void RemoteSetHealthMonitorPara(u8 *buf, u8 len);
void RemoteDevBond(u8 *buf, u8 len);
void RemoteDevUnBond(u8 *buf, u8 len);
void RemoteMsgNotifyPush(u8 *buf, u8 len);
void RemoteSetANCSSw(u8 *buf, u8 len);
void RemoteSetLLInfo(u8 *buf, u8 len);
void RemoteSetWeatherData(u8 *buf, u8 len);
void RemoteSetSedentaryInfo(u8 *buf, u8 len);
void RemoteSetCameraOpCmd(u8 *buf, u8 len);
void RemoteSetRmusicPara(u8 *buf, u8 len);
void RemoteSetPTimeCfgPara(u8 *buf, u8 len);
void RemoteUpdateCallContacts(u8 *buf, u8 len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
