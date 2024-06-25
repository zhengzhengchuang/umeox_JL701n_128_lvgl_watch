#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_activity_ctx_t w_activity;
vm_activity_ctx_t r_activity;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_activity;

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmActivityCtxClear(void)
{
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    uint8_t activity_num = \
        VmActivityItemNum();

    while(activity_num)
    {
        flash_common_delete_by_index(\
            nor_vm_file, 0);

        activity_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmActivityItemNum(void)
{
    uint8_t activity_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return activity_num;

    activity_num = \
        flash_common_get_total(nor_vm_file);

    if(activity_num > Activity_Max_Days)
        activity_num = Activity_Max_Days;

    return activity_num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmActivityCtxByIdx(uint8_t idx)
{
    uint8_t activity_num = \
        VmActivityItemNum();
    if(activity_num == 0)
        return false;

    if(idx >= activity_num)
        return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_activity_ctx_t);
    
    if(!nor_vm_file)
        return false;

    memset(&r_activity, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (uint8_t *)&r_activity);

    if(r_activity.check_code != \
        Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmActivityCtxDelByIdx(uint8_t idx)
{
    uint8_t activity_num = \
        VmActivityItemNum();
    if(activity_num == 0)
        return;

    if(idx >= activity_num)
        return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    flash_common_delete_by_index(\
        nor_vm_file, idx);

    return;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmActivityCtxFlashSave(void *p)
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
        sizeof(vm_activity_ctx_t);

    if(!nor_vm_file)
        return;

    uint8_t activity_num = \
        VmActivityItemNum();

    printf("activity_num = %d\n", \
        activity_num);
    
    if(activity_num >= Activity_Max_Days)
        flash_common_delete_by_index(\
            nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, \
        ctx_len, (u8 *)p);

    return;
}
