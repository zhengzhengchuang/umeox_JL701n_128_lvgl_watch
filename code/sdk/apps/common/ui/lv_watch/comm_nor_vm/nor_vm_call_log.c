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
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    uint8_t call_log_num = \
        VmCallLogItemNum();

    while(call_log_num)
    {
        flash_common_delete_by_index(\
            nor_vm_file, 0);

        call_log_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmCallLogItemNum(void)
{
    uint8_t call_log_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return call_log_num;

    call_log_num = \
        flash_common_get_total(nor_vm_file);

    if(call_log_num > Call_log_Max_Num)
        call_log_num = Call_log_Max_Num;

    return call_log_num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmCallLogCtxByIdx(uint8_t idx)
{
    uint8_t num = \
        VmCallLogItemNum();

    if(idx >= num)
        return false;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_call_log_ctx_t);
    
    if(!nor_vm_file)
        return false;

    idx = (num - 1) - idx;
    
    memset(&r_call_log, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_call_log);

    if(r_call_log.check_code != \
        Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmCallLogCtxFlashSave(void *p)
{
#if !Vm_Debug_En
    /*如果设备不绑定、不允许存储任何数据*/
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_call_log_ctx_t);

    if(!nor_vm_file)
        return;

    uint8_t call_log_num = \
        VmCallLogItemNum();

    printf("call_log_num = %d\n", \
        call_log_num);
    
    if(call_log_num >= Call_log_Max_Num)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
