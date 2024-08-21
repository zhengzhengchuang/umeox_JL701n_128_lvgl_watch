#include "../lv_watch.h"

void ResetAllVmData(void)
{
    HrDataVmReset();
    BoDataVmReset();
    SlpDataVmReset();
    LLInfoParaReset();
    GmCaliInfoReset();
    PedoDataVmReset();
    SedInfoParaReset();
    DndInfoParaReset();
    TwsInfoParaReset();
    UserInfoParaReset();
    PTimeCfgParaReset();
    AlarmInfoParaReset();
    vm_para_cache_reset();
    QpUserInfoParaReset();
    TasbihRInfoParaReset();
    BondCodeInfoParaReset(); 
    HcalendarInfoParaReset(); 
    MsgNotifyInfoParaReset();
    
    
    return;
}

void ResetAllNorVmData(void)
{
    nor_flash_vm_clear();
    
    return;
}

void PowerOnVmDataRead(void)
{
#if NOR_DEBUG_DATA
#if 1
    extern void sleep_data_test(void);
    sleep_data_test();

    extern void bo_test_func(void);
    bo_test_func();

    extern void hr_test_func(void);
    hr_test_func();

    extern void pedo_test_func(void);
    pedo_test_func();
#else
    VmSleepCtxClear();
    VmBoCtxClear();
    VmHrCtxClear();
    VmPedoCtxClear();  
#endif
#endif

    /*开机:vm--->数据*/
    PowerOnSetSleepData();
    PowerOnSetHrVmCache();
    PowerOnSetBoVmCache();
    SetWeatherInfoPara();
    PowerOnSetPedoVmCache();
    
    return;
}

void PowerOffVmDataWrite(void)
{
    /*关机:数据--->vm*/
    PowerOffSetHrVmFlashSave();
    PowerOffSetBoVmFlashSave();
    PowerOffSetPedoVmFlashSave();

    return;
}

void TimeUpdateDataHandle(void)
{
    WHrParaInit();
    WBoParaInit();
    WPedoParaInit();
    
    return;
}
