#ifndef __NOR_VM_PEDO_H__
#define __NOR_VM_PEDO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  保存最大天数                                  
*********************************************************************************/
#define Pedo_Max_Days (7)

/*********************************************************************************
                                  存储间隔时间(分钟)                               
*********************************************************************************/
#define Pedo_Inv_Dur (60)

/*********************************************************************************
                                  一天打点个数                               
*********************************************************************************/
#define Pedo_Day_Num (1440/Pedo_Inv_Dur)

typedef struct
{
    uint16_t check_code;

    uint8_t CurIdx;
    struct sys_time time;

    uint32_t steps[Pedo_Day_Num];
    uint32_t calorie[Pedo_Day_Num];
    uint32_t distance[Pedo_Day_Num];
}vm_pedo_ctx_t;
extern vm_pedo_ctx_t w_pedo;
extern vm_pedo_ctx_t r_pedo;

bool GetPedoDayVmData(void);
void SetPedoDayVmData(bool d);

void VmPedoCtxClear(void);
uint8_t VmPedoItemNum(void);
bool VmPedoCtxByIdx(uint8_t idx);
void VmPedoCtxFlashSave(void *p);
void VmPedoCtxDelByIdx(uint8_t idx);

void pedo_test_func(void);
#ifdef __cplusplus
}
#endif

#endif
