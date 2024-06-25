#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_hr_ctx_t w_hr;
vm_hr_ctx_t r_hr;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_hr;

static bool DayVmData;
bool GetHrDayVmData(void)
{
    return DayVmData;
}

void SetHrDayVmData(bool d)
{
    DayVmData = d;

    return;
}

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmHrCtxClear(void)
{
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    uint8_t hr_num = \
        VmHrItemNum();

    while(hr_num)
    {
        flash_common_delete_by_index(\
            nor_vm_file, 0);

        hr_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmHrItemNum(void)
{
    uint8_t hr_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return hr_num;

    hr_num = \
        flash_common_get_total(nor_vm_file);

    if(hr_num > Hr_Max_Days)
        hr_num = Hr_Max_Days;

    return hr_num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmHrCtxByIdx(uint8_t idx)
{
    uint8_t hr_num = \
        VmHrItemNum();
    if(hr_num == 0)
        return false;

    if(idx >= hr_num)
        return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_hr_ctx_t);
    
    if(!nor_vm_file)
        return false;

    memset(&r_hr, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (uint8_t *)&r_hr);

    if(r_hr.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmHrCtxDelByIdx(uint8_t idx)
{
    uint8_t hr_num = \
        VmHrItemNum();
    if(hr_num == 0)
        return;

    if(idx >= hr_num)
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
void VmHrCtxFlashSave(void *p)
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
        sizeof(vm_hr_ctx_t);

    if(!nor_vm_file)
        return;

    bool DayVD = GetHrDayVmData();
    uint8_t hr_num = VmHrItemNum();
    
    printf("hr_num = %d\n", hr_num);
    
    if(hr_num >= Hr_Max_Days && DayVD == true)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
