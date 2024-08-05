#ifndef __NOR_VM_PEDO_H__
#define __NOR_VM_PEDO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Pedo_Max_Days (7)
#define Pedo_Inv_Dur (60)
#define Pedo_Day_Num (1440/Pedo_Inv_Dur)

typedef struct
{
    u16 check_code;

    u8 CurIdx;
    u32 timestamp;
    float steps[Pedo_Day_Num];
    float calorie[Pedo_Day_Num];
    float distance[Pedo_Day_Num];
}vm_pedo_ctx_t;
extern vm_pedo_ctx_t w_pedo;
extern vm_pedo_ctx_t r_pedo;

bool GetPedoDayVmData(void);
void SetPedoDayVmData(bool d);

void VmPedoCtxClear(void);
u8 VmPedoItemNum(void);
bool VmPedoCtxByIdx(u8 idx);
void VmPedoCtxFlashSave(void *p);
void VmPedoCtxDelByIdx(u8 idx);
#ifdef __cplusplus
}
#endif

#endif
