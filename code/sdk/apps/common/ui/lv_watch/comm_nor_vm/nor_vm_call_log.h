#ifndef __NOR_VM_CALL_LOG_H__
#define __NOR_VM_CALL_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_time.h"
#include "../include/ui_menu.h"
#include "../include/comm_call.h"

#define Call_log_Max_Num (10)

enum
{
    call_log_in,          //拨入
    call_log_out,         //拨出
    call_log_hangup,      //挂断

    call_log_unknown,
};
typedef u8 call_log_state_t;

typedef struct
{
    u16 check_code;

    u32 timestamp;
    call_log_state_t state;
    char name_str[Call_Name_Max_Len];
    char number_str[Call_Number_Max_Len];
}vm_call_log_ctx_t;
extern vm_call_log_ctx_t w_call_log;
extern vm_call_log_ctx_t r_call_log;

void VmCallLogCtxClear(void);
u8 VmCallLogItemNum(void);
bool VmCallLogCtxByIdx(u8 idx);
void VmCallLogCtxFlashSave(void *p);
#ifdef __cplusplus
}
#endif

#endif
