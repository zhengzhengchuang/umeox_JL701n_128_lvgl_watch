#ifndef __OTA_FUNC_H__
#define __OTA_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    upgrade_none, //无升级状态
    upgrading,    //升级中
    upgrade_succ, //升级成功
    upgrade_fail, //升级失败 
};

u8 GetOtaUpgradeState(void);
void SetOtaUpgradeState(u8 state);

void OtaUpgradeHandle(void);
void OtaUpgradeFailHandle(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
