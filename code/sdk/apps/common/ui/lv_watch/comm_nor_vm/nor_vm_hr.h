#ifndef __NOR_VM_HR_H__
#define __NOR_VM_HR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  心率保存最大天数                                  
*********************************************************************************/
#define Hr_Max_Days (7)

/*********************************************************************************
                                  心率启动间隔时间(分钟)                                
*********************************************************************************/
#define Hr_Inv_Dur (15)

/*********************************************************************************
                                  心率一天打点个数                               
*********************************************************************************/
#define Hr_Day_Num (1440/Hr_Inv_Dur)

typedef struct
{
    uint16_t check_code;

    uint8_t CurIdx;
    struct sys_time time;

    uint8_t min_data[24];
    uint8_t max_data[24];
    uint8_t data[Hr_Day_Num];
}vm_hr_ctx_t;
extern vm_hr_ctx_t w_hr;
extern vm_hr_ctx_t r_hr;

bool GetHrDayVmData(void);
void SetHrDayVmData(bool d);

void VmHrCtxClear(void);
uint8_t VmHrItemNum(void);
bool VmHrCtxByIdx(uint8_t idx);
void VmHrCtxFlashSave(void *p);
void VmHrCtxDelByIdx(uint8_t idx);
#ifdef __cplusplus
}
#endif

#endif
