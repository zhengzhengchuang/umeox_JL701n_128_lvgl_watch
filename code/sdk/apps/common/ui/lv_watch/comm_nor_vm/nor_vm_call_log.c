#include "nor_vm_main.h"

vm_call_log_ctx_t w_call_log;
vm_call_log_ctx_t r_call_log;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_call_log;

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmCallLogCtxClear(void)
{ 
    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return;

    u8 num = VmCallLogItemNum();

    while(num)
    {
        flash_common_delete_by_index(nor_vm_file, 0);
        num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
u8 VmCallLogItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Call_log_Max_Num)
        num = Call_log_Max_Num;

    return num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmCallLogCtxByIdx(u8 idx)
{
    u8 num = VmCallLogItemNum();
    if(idx >= num) return false;

    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_call_log_ctx_t);
    if(!nor_vm_file) return false;

    idx = (num - 1) - idx;
    
    memset(&r_call_log, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_call_log);

    if(r_call_log.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmCallLogCtxFlashSave(void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) return;

    if(!p) return;

    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_call_log_ctx_t);
    if(!nor_vm_file) return;

    u8 num = VmCallLogItemNum();

    printf("call_log_num = %d\n", num);
    
    if(num >= Call_log_Max_Num)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
