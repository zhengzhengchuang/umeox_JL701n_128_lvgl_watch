#ifndef __NOR_VM_SLEEP_H__
#define __NOR_VM_SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Slp_Max_Days (7)
#define Slp_Max_Sec (50)

enum
{
    slp_awake,
    slp_rem,
    slp_light,
    slp_deep,

    slp_state_num,
};

typedef struct
{
    int8_t stage;
    u32 start_ts; 
    u32 end_ts;   
}SlpSecCtx_t;

typedef struct
{
    u16 check_code;

    u8 SecNum;
    u32 timestamp;
    u32 total_start_ts;
    u32 total_end_ts;
    SlpSecCtx_t ctx[Slp_Max_Sec];
}vm_sleep_ctx_t;
extern vm_sleep_ctx_t w_sleep;
extern vm_sleep_ctx_t r_sleep;

void VmSleepCtxClear(void);
u8 VmSleepItemNum(void);
bool VmSleepCtxByIdx(u8 idx);
void VmSleepCtxFlashSave(void *p);
#ifdef __cplusplus
}
#endif

#endif
