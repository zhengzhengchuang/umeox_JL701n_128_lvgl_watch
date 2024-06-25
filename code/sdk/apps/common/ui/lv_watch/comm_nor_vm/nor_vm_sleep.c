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

    uint8_t sleep_num = \
        VmSleepItemNum();

    while(sleep_num)
    {
        flash_common_delete_by_index(\
            nor_vm_file, 0);

        sleep_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmSleepItemNum(void)
{
    uint8_t sleep_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return sleep_num;

    sleep_num = \
        flash_common_get_total(nor_vm_file);

    if(sleep_num > Sleep_Max_Days)
        sleep_num = Sleep_Max_Days;

    return sleep_num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmSleepCtxByIdx(uint8_t idx)
{
    uint8_t sleep_num = \
        VmSleepItemNum();
    if(sleep_num == 0)
        return false;

    if(idx >= sleep_num)
        return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_sleep_ctx_t);
    
    if(!nor_vm_file)
        return false;

    memset(&r_sleep, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (uint8_t *)&r_sleep);

    if(r_sleep.check_code != \
        Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmSleepCtxFlashSave(void *p)
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
        sizeof(vm_sleep_ctx_t);

    if(!nor_vm_file)
        return;

    uint8_t sleep_num = VmSleepItemNum();

    printf("sleep_num = %d\n", sleep_num);
    
    if(sleep_num >= Sleep_Max_Days)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
