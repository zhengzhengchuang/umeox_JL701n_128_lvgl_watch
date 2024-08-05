#ifndef __HR_FUNC_H__
#define __HR_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

typedef struct
{
    u16 vm_mask;

    u8 hr_real;
    u8 hr_min[24];
    u8 hr_max[24];

    u8 hr_low_sw;   //过低提醒开关
    u8 hr_high_sw;  //过低提醒开关
    u8 hr_low_val;  //过低值
    u8 hr_high_val; //过高值
}HrPara_t;
extern HrPara_t HrPara;

u8 GetHrRealVal(void);
void SetHrRealVal(u8 val);
void ClearHrRealVal(void);

void SetHrVmCtxCache(u8 val);

/* 开机、同步时调用 */
void WHrParaInit(void);

void PowerOnSetHrVmCache(void);
void PowerOffSetHrVmFlashSave(void);

void VmHrCtxFlashWrite(void);

void HrTimerSecProcess(void);
void HrUtcMinProcess(struct sys_time *ptime);

void HrDataVmRead(void);
void HrDataVmWrite(void);
void HrDataVmReset(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
