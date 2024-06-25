#ifndef __NOR_VM_ACTIVITY_H__
#define __NOR_VM_ACTIVITY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  活动保存最大天数                                  
*********************************************************************************/
#define Activity_Max_Days (7)

/*********************************************************************************
                                  活动存储间隔时间(分钟)                               
*********************************************************************************/
#define Activity_Inv_Dur (60)

/*********************************************************************************
                                  活动一天打点个数                               
*********************************************************************************/
#define Activity_Day_Num (1440/Activity_Inv_Dur)

typedef struct
{
    uint16_t check_code;

    uint8_t CurIdx;
    struct sys_time time;

    uint32_t steps[Activity_Day_Num];
    uint32_t calorie[Activity_Day_Num];
    uint32_t distance[Activity_Day_Num];
}vm_activity_ctx_t;
extern vm_activity_ctx_t w_activity;
extern vm_activity_ctx_t r_activity;

void VmActivityCtxClear(void);
uint8_t VmActivityItemNum(void);
bool VmActivityCtxByIdx(uint8_t idx);
void VmActivityCtxDelByIdx(uint8_t idx);
void VmActivityCtxFlashSave(void *p);
#ifdef __cplusplus
}
#endif

#endif
