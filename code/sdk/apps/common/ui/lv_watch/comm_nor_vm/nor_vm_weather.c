#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_weather_ctx_t w_weather;
vm_weather_ctx_t r_weather;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_weather;

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmWeatherCtxClear(void)
{
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    uint8_t weather_num = \
        VmWeatherItemNum();

    while(weather_num)
    {
        flash_common_delete_by_index(\
            nor_vm_file, 0);

        weather_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmWeatherItemNum(void)
{
    uint8_t weather_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return weather_num;

    weather_num = \
        flash_common_get_total(nor_vm_file);

    return weather_num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmWeatherCtxFlashRead(void)
{
    uint8_t weather_num = \
        VmWeatherItemNum();
    if(weather_num == 0)
        return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_weather_ctx_t);
    
    if(!nor_vm_file)
        return false;

    memset(&r_weather, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, 0, 0, \
        ctx_len, (uint8_t *)&r_weather);

    if(r_weather.check_code != \
        Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              天气数据存储                                        
*********************************************************************************/
void VmWeatherCtxFlashSave(void *p)
{ 
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_weather_ctx_t);

    if(!nor_vm_file)
        return;

    VmWeatherCtxClear();

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
