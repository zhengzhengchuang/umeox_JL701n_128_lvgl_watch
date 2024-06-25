#ifndef __CAMERA_FUNC_H__
#define __CAMERA_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    DevReqEnterCamera = 0x00,
    DevReqTakePhotos,
    DevReqDlyTakePhotos,
    DevReqExitCamera,

    RemoteReqEnterCamera,
    RemoteReqExitCamera,
};
typedef u8 CameraOpCmd_t;

bool GetCameraIsRemoteExit(void);
void SetCameraIsRemoteExit(bool f);

bool GetCameraUnlockExit(void);
void SetCameraUnlockExit(bool f);

void RemoteReqEnterCameraHandle(void);
void RemoteReqExitCameraHandle(void);
void DevReqOpCameraHandle(CameraOpCmd_t OpCmd);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
