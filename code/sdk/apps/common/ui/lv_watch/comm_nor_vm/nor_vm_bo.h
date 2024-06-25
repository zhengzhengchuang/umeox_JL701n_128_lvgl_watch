#ifndef __NOR_VM_BO_H__
#define __NOR_VM_BO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  血氧保存最大天数                                  
*********************************************************************************/
#define Bo_Max_Days (7)

/*********************************************************************************
                                  血氧启动间隔时间(分钟)                                
*********************************************************************************/
#define Bo_Inv_Dur (15)

/*********************************************************************************
                                  血氧一天打点个数                               
*********************************************************************************/
#define Bo_Day_Num (1440/Bo_Inv_Dur)

typedef struct
{
    uint16_t check_code;

    uint8_t CurIdx;
    struct sys_time time;

    uint8_t min_data[24];
    uint8_t max_data[24];
    uint8_t data[Bo_Day_Num];
}vm_bo_ctx_t;
extern vm_bo_ctx_t w_bo;
extern vm_bo_ctx_t r_bo;

bool GetBoDayVmData(void);
void SetBoDayVmData(bool d);

void VmBoCtxClear(void);
uint8_t VmBoItemNum(void);
bool VmBoCtxByIdx(uint8_t idx);
void VmBoCtxFlashSave(void *p);
void VmBoCtxDelByIdx(uint8_t idx);
#ifdef __cplusplus
}
#endif

#endif
