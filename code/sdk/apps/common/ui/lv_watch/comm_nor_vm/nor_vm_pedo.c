#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_pedo_ctx_t w_pedo;
vm_pedo_ctx_t r_pedo;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_pedo;

static bool DayVmData;
bool GetPedoDayVmData(void)
{
    return DayVmData;
}

void SetPedoDayVmData(bool d)
{
    DayVmData = d;

    return;
}

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmPedoCtxClear(void)
{ 
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return;

    u8 num = VmPedoItemNum();

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
u8 VmPedoItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Pedo_Max_Days)
        num = Pedo_Max_Days;

    return num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmPedoCtxByIdx(u8 idx)
{
    u8 num = VmPedoItemNum();
    if(num == 0) return false;
    
    if(idx >= num) return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_pedo_ctx_t);
    
    if(!nor_vm_file) return false;

    memset(&r_pedo, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_pedo);

    if(r_pedo.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmPedoCtxDelByIdx(u8 idx)
{
    u8 num = VmPedoItemNum();
    if(num == 0) return;

    if(idx >= num) return;

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
void VmPedoCtxFlashSave(void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_pedo_ctx_t);
    if(!nor_vm_file) return;

    bool DayVD = GetHrDayVmData();
    u8 num = VmPedoItemNum();

    printf("pedo num = %d\n", num);
    
    if(num >= Pedo_Max_Days && DayVD == true)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}

#if NOR_DEBUG_DATA
static const vm_pedo_ctx_t pedo_test[7] = {
    [0] = {
        .check_code = Nor_Vm_Check_Code,
        .timestamp = 1723420800,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 12, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },

    [1] = {
        .check_code = Nor_Vm_Check_Code,
        .timestamp = 1723507200,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 13, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },

    [2] = {
        .check_code = Nor_Vm_Check_Code,
        .timestamp = 1723593600,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 14, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },

    [3] = {
        .check_code = Nor_Vm_Check_Code,
        .timestamp = 1723680000,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 15, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },

    [4] = {
        .check_code = Nor_Vm_Check_Code,
        .timestamp = 1723766400,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 16, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },

    [5] = {
        .check_code = Nor_Vm_Check_Code, 
        .timestamp = 1723852800,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 17, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },

    [6] = {
        .check_code = Nor_Vm_Check_Code,
        .timestamp = 1723939200,
        .CurIdx = Pedo_Day_Num - 1,
        //.time.year = 2024, .time.month = 8, .time.day = 18, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 342.0f, 3456.0f, 3425.0f, 345.0f, 987.0f, 434.0f, 253.0f, 1245.0f, 4563.0f, 2345.0f, 567.0f, 3425.0f, 43.0f, 0.0f, 0.0f,},
        .calorie = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 124.0f, 654.0f, 245.0f, 875.0f, 0.0f, 0.0f,},
        .distance = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 8547.0f, 5641.0f, 4578.0f, 8475.0f, 0.0f, 0.0f,},
    },
};

void pedo_test_func(void)
{
    for(u8 i = 0; i < 7; i++)
        VmPedoCtxFlashSave(&pedo_test[i]);
    
    return;
}
#endif
