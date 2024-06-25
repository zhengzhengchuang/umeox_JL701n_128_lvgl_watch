#ifndef __NOR_VM_CALL_LOG_H__
#define __NOR_VM_CALL_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_time.h"
#include "../include/ui_menu.h"
#include "../include/comm_call.h"

/*********************************************************************************
                                  通话记录最大个数                                  
*********************************************************************************/
#define Call_log_Max_Num (10)

/*********************************************************************************
                                  通话记录状态枚举                                 
*********************************************************************************/
enum
{
    call_log_state_in,          //拨入
    call_log_state_out,         //拨出
    call_log_state_hangup,      //挂断

    call_log_unknown,
};
typedef uint8_t call_log_state_t;

/*********************************************************************************
                                  通话记录内容                                  
*********************************************************************************/
typedef struct
{
    uint16_t check_code;

    struct sys_time time;

    call_log_state_t state;
    char name_str[Call_Name_Max_Len];
    char number_str[Call_Number_Max_Len];
}vm_call_log_ctx_t;
extern vm_call_log_ctx_t w_call_log;
extern vm_call_log_ctx_t r_call_log;

/*********************************************************************************
                                  通话记录接口                                       
*********************************************************************************/
void VmCallLogCtxClear(void);
uint8_t VmCallLogItemNum(void);
bool VmCallLogCtxByIdx(uint8_t idx);
void VmCallLogCtxFlashSave(void *p);
#ifdef __cplusplus
}
#endif

#endif
