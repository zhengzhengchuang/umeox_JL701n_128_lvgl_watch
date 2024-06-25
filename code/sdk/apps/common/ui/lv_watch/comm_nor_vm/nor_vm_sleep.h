#ifndef __NOR_VM_SLEEP_H__
#define __NOR_VM_SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  睡眠保存最大天数                                  
*********************************************************************************/
#define Sleep_Max_Days (7)

/*********************************************************************************
                                  睡眠一天最大时间段数                                  
*********************************************************************************/
#define SDay_Max_SecNum (30)

/*********************************************************************************
                                  睡眠状态枚举                                  
*********************************************************************************/
enum
{
    sl_wake_up,
    sl_rem_sleep,
    sl_light_sleep,
    sl_deep_sleep,

    sl_state_num,
};
typedef uint8_t sleep_state_t;

typedef struct
{
    sleep_state_t state;

    struct sys_time ss_time; 
    struct sys_time se_time;   
}vm_sleep_sec_ctx_t;

typedef struct
{
    uint16_t check_code;

    uint8_t CurSecNum;
    struct sys_time time;
    vm_sleep_sec_ctx_t vm_ctx[\
        SDay_Max_SecNum];
}vm_sleep_ctx_t;
extern vm_sleep_ctx_t w_sleep;
extern vm_sleep_ctx_t r_sleep;

void VmSleepCtxClear(void);
uint8_t VmSleepItemNum(void);
bool VmSleepCtxByIdx(uint8_t idx);
void VmSleepCtxFlashSave(void *p);
#ifdef __cplusplus
}
#endif

#endif
