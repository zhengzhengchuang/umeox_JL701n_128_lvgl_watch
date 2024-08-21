#ifndef __POWER_MANAGE_H__
#define __POWER_MANAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

typedef struct
{
    u16 mask;
    float soc;
    int power;
}BatPower_t;
extern BatPower_t BatPower;

int GetBatLevel(void);
int GetBatPower(void);
int BatEnableCtrl(u8 en);
void BatPowerSetZero(void);
u8 BatPowerIsAllowOta(void);
void BatPowerUpdateHandle(void);
void LPRemindProcess(struct sys_time *ptime);
void LPShutdownProcess(struct sys_time *ptime);

void VmPowerRead(void);
void VmPowerWrite(void);
void VmPowerReset(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
