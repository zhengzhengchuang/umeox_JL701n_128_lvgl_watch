#ifndef __CHARGE_FUNC_H__
#define __CHARGE_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

u8 GetChargeState(void);
u8 GetChargeFullFlag(void);

void Ldo5vInHandle(void);
void Ldo5vOutHandle(void);

u8 GetChargePowerFlag(void);
void SetChargePowerFlag(u8 f);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
