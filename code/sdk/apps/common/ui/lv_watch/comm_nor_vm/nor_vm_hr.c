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
    if(!nor_vm_file) return;

    u8 num = VmHrItemNum();
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
u8 VmHrItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Hr_Max_Days)
        num = Hr_Max_Days;

    return num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmHrCtxByIdx(u8 idx)
{
    u8 num = VmHrItemNum();
    if(num == 0) return false;

    if(idx >= num) return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_hr_ctx_t);
    
    if(!nor_vm_file) return false;

    memset(&r_hr, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_hr);

    if(r_hr.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmHrCtxDelByIdx(u8 idx)
{
    u8 num = VmHrItemNum();
    if(num == 0) return;

    if(idx >= num) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return;

    flash_common_delete_by_index(nor_vm_file, idx);

    return;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmHrCtxFlashSave(void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_hr_ctx_t);
    if(!nor_vm_file) return;

    bool DayVD = GetHrDayVmData();
    u8 num = VmHrItemNum();
    
    printf("hr num = %d\n", num);
    
    if(num >= Hr_Max_Days && DayVD == true)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}

#if 0
static const vm_hr_ctx_t hr_test[7] = {
    [0] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 17, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [1] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 18, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [2] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 19, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [3] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 20, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [4] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 21, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [5] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 22, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [6] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 23, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },
};

void hr_test_func(void)
{
    for(u8 i = 0; i < 7; i++)
        VmHrCtxFlashSave(&hr_test[i]);
    
    return;
}
#endif
