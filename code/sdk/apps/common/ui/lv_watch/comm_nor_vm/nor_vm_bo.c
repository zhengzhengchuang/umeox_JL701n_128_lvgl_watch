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
    if(!nor_vm_file) return;

    u8 num = VmBoItemNum();

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
u8 VmBoItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Bo_Max_Days)
        num = Bo_Max_Days;

    return num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmBoCtxByIdx(u8 idx)
{
    u8 num = VmBoItemNum();
    if(num == 0) return false;

    if(idx >= num) return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_bo_ctx_t);
    if(!nor_vm_file) return false;

    memset(&r_bo, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_bo);

    if(r_bo.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmBoCtxDelByIdx(u8 idx)
{
    u8 num = VmBoItemNum();
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
void VmBoCtxFlashSave(void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_bo_ctx_t);

    if(!nor_vm_file) return;

    bool DayVD = GetBoDayVmData();
    u8 num = VmBoItemNum();

    printf("bo_num = %d\n", num);
    
    if(num >= Bo_Max_Days && DayVD == true)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}

#if NOR_DEBUG_DATA
static const vm_bo_ctx_t bo_test[7] = {
    [0] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723420800,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 12, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [1] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723507200,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 13, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [2] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723593600,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 14, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [3] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723680000,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 15, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [4] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723766400,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 16, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [5] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723852800,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 17, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },

    [6] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723939200,
        .CurIdx = Bo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 18, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .data = {45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, 45, 65, 78, 59, 78, \
                 45, 65, 78, 59, 78, 45,},
    },
};

void bo_test_func(void)
{
    for(u8 i = 0; i < 7; i++)
        VmBoCtxFlashSave(&bo_test[i]);
    
    return;
}
#endif
