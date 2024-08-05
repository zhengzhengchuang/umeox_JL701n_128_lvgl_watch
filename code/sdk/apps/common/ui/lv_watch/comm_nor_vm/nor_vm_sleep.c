#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_sleep_ctx_t w_sleep;
vm_sleep_ctx_t r_sleep;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_sleep;

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmSleepCtxClear(void)
{ 
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    u8 num = VmSleepItemNum();

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
u8 VmSleepItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Slp_Max_Days)
        num = Slp_Max_Days;

    return num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmSleepCtxByIdx(u8 idx)
{
    u8 num = VmSleepItemNum();
    if(num == 0) return false;

    if(idx >= num) return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_sleep_ctx_t);
    
    if(!nor_vm_file) return false;

    memset(&r_sleep, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_sleep);

    if(r_sleep.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmSleepCtxFlashSave(void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_sleep_ctx_t);

    if(!nor_vm_file) return;

    u8 num = VmSleepItemNum();

    printf("num = %d\n", num);
    
    if(num >= Slp_Max_Sec)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
