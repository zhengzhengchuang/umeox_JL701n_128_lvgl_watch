#ifndef __BO_FUNC_H__
#define __BO_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

u8 GetBoRealVal(void);
void SetBoRealVal(u8 val);

void SetBoVmCtxCache(u8 val);

void PowerOnSetBoVmCache(void);
void PowerOffSetBoVmFlashSave(void);

void VmBoCtxFlashWrite(void);

void BoTimerSecProcess(void);
void BoProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
