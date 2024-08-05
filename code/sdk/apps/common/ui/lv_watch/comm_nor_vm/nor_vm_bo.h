#ifndef __NOR_VM_BO_H__
#define __NOR_VM_BO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Bo_Max_Days (7)
#define Bo_Inv_Dur  (15)
#define Bo_Day_Num  (1440/Bo_Inv_Dur)

typedef struct
{
    u16 check_code;

    u8 CurIdx;
    u32 timestamp;
    u8 data[Bo_Day_Num];
}vm_bo_ctx_t;
extern vm_bo_ctx_t w_bo;
extern vm_bo_ctx_t r_bo;

bool GetBoDayVmData(void);
void SetBoDayVmData(bool d);

void VmBoCtxClear(void);
u8 VmBoItemNum(void);
bool VmBoCtxByIdx(u8 idx);
void VmBoCtxFlashSave(void *p);
void VmBoCtxDelByIdx(u8 idx);
#ifdef __cplusplus
}
#endif

#endif
