#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_bo_ctx_t w_bo;
vm_bo_ctx_t r_bo;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_bo; 

static bool DayVmData;
bool GetBoDayVmData(void)
{
    return DayVmData;
}

void SetBoDayVmData(bool d)
{
    DayVmData = d;

    return;
}

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmBoCtxClear(void)
{
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    uint8_t bo_num = \
        VmBoItemNum();

    while(bo_num)
    {
        flash_common_delete_by_index(\
            nor_vm_file, 0);

        bo_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmBoItemNum(void)
{
    uint8_t bo_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return bo_num;

    bo_num = \
        flash_common_get_total(nor_vm_file);

    if(bo_num > Bo_Max_Days)
        bo_num = Bo_Max_Days;

    return bo_num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmBoCtxByIdx(uint8_t idx)
{
    uint8_t bo_num = \
        VmBoItemNum();
    if(bo_num == 0)
        return false;

    if(idx >= bo_num)
        return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_bo_ctx_t);
    
    if(!nor_vm_file)
        return false;

    memset(&r_bo, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (uint8_t *)&r_bo);

    if(r_bo.check_code != \
        Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmBoCtxDelByIdx(uint8_t idx)
{
    uint8_t bo_num = \
        VmBoItemNum();
    if(bo_num == 0)
        return;

    if(idx >= bo_num)
        return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    flash_common_delete_by_index(nor_vm_file, idx);

    return;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmBoCtxFlashSave(void *p)
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
        sizeof(vm_bo_ctx_t);

    if(!nor_vm_file)
        return;

    bool DayVD = GetBoDayVmData();
    uint8_t bo_num = VmBoItemNum();

    printf("bo_num = %d\n", bo_num);
    
    if(bo_num >= Bo_Max_Days && DayVD == true)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
