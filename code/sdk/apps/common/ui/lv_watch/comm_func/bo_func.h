#ifndef __BO_FUNC_H__
#define __BO_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

typedef struct
{
    u16 vm_mask;

    u8 bo_real;
    u8 bo_min[24];
    u8 bo_max[24];
}BoPara_t;
extern BoPara_t BoPara;

u8 GetBoRealVal(void);
void SetBoRealVal(u8 val);
void ClearBoRealVal(void);

void SetBoVmCtxCache(u8 val);

/* 开机、时间同步时调用 */
void WBoParaInit(void);

void PowerOnSetBoVmCache(void);
void PowerOffSetBoVmFlashSave(void);

void VmBoCtxFlashWrite(void);

void SetAutoBoRestart(void);
void BoTimerSecProcess(void);
void BoUtcMinProcess(struct sys_time *ptime);

void BoDataVmRead(void);
void BoDataVmWrite(void);
void BoDataVmReset(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
