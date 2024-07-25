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

    if(!nor_vm_file)
        return;

    uint8_t num = VmPedoItemNum();

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
uint8_t VmPedoItemNum(void)
{
    uint8_t num = 0;

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
bool VmPedoCtxByIdx(uint8_t idx)
{
    uint8_t num = VmPedoItemNum();
    if(num == 0) return false;

    if(idx >= num) return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_pedo_ctx_t);
    
    if(!nor_vm_file) return false;

    memset(&r_pedo, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (uint8_t *)&r_pedo);

    if(r_pedo.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              删除指定项内容                                        
*********************************************************************************/
void VmPedoCtxDelByIdx(uint8_t idx)
{
    uint8_t num = VmPedoItemNum();
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
    uint8_t num = VmPedoItemNum();

    printf("pedo num = %d\n", num);
    
    if(num >= Pedo_Max_Days && DayVD == true)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}

#if 0
static const vm_pedo_ctx_t pedo_test[7] = {
    [0] = {
        .check_code = Nor_Vm_Check_Code,
        .time.year = 2024, .time.month = 7, .time.day = 17, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },

    [1] = {
        .check_code = Nor_Vm_Check_Code,
        .time.year = 2024, .time.month = 7, .time.day = 18, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },

    [2] = {
        .check_code = Nor_Vm_Check_Code,
        .time.year = 2024, .time.month = 7, .time.day = 19, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },

    [3] = {
        .check_code = Nor_Vm_Check_Code,
        .time.year = 2024, .time.month = 7, .time.day = 20, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },

    [4] = {
        .check_code = Nor_Vm_Check_Code,
        .time.year = 2024, .time.month = 7, .time.day = 21, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },

    [5] = {
        .check_code = Nor_Vm_Check_Code, 
        .time.year = 2024, .time.month = 7, .time.day = 22, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },

    [6] = {
        .check_code = Nor_Vm_Check_Code,
        .time.year = 2024, .time.month = 7, .time.day = 23, .time.hour = 0, .time.min = 0, .time.sec = 0,
        .steps = {1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 1245, 3256, 5478, 457, 7845, 3256, 5478, 457, 7845,},
        .calorie = {245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654, 245, 875, 124, 654,},
        .distance = {4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641, 4578, 8475, 8547, 5641,},
    },
};

void pedo_test_func(void)
{
    for(u8 i = 0; i < 7; i++)
        VmPedoCtxFlashSave(&pedo_test[i]);
    
    return;
}
#endif
