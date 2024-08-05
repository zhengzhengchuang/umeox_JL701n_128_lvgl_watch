#ifndef __NOR_VM_HR_H__
#define __NOR_VM_HR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Hr_Max_Days (7)
#define Hr_Inv_Dur (15)
#define Hr_Day_Num (1440/Hr_Inv_Dur)

typedef struct
{
    u16 check_code;

    u8 CurIdx;
    u32 timestamp;
    u8 data[Hr_Day_Num];
}vm_hr_ctx_t;
extern vm_hr_ctx_t w_hr;
extern vm_hr_ctx_t r_hr;

bool GetHrDayVmData(void);
void SetHrDayVmData(bool d);

void VmHrCtxClear(void);
u8 VmHrItemNum(void);
bool VmHrCtxByIdx(u8 idx);
void VmHrCtxFlashSave(void *p);
void VmHrCtxDelByIdx(u8 idx);
#ifdef __cplusplus
}
#endif

#endif
