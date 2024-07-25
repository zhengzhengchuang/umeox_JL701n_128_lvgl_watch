#ifndef __HR_FUNC_H__
#define __HR_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

u8 GetHrRealVal(void);
void SetHrRealVal(u8 val);

void SetHrVmCtxCache(u8 val);

void PowerOnSetHrVmCache(void);
void PowerOffSetHrVmFlashSave(void);

void VmHrCtxFlashWrite(void);

void HrTimerSecProcess(void);
void HrProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
