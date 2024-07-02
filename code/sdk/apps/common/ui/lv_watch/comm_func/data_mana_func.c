#include "../lv_watch.h"

void ResetAllVmData(void)
{
    LLInfoParaReset();
    SedInfoParaReset();
    DndInfoParaReset();
    TwsInfoParaReset();
    UserInfoParaReset();
    PTimeCfgParaReset();
    GalgoInfoParaReset();
    AlarmInfoParaReset();
    vm_para_cache_reset();
    QpUserInfoParaReset();
    TasbihRInfoParaReset();
    BondCodeInfoParaReset();
    SensorGmCaliInfoReset();
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
    /*开机:vm--->数据*/
    PowerOnSetHrVmCache();
    PowerOnSetBoVmCache();

    return;
}

void PowerOffVmDataWrite(void)
{
    /*关机:数据--->vm*/
    PowerOffSetHrVmFlashSave();
    PowerOffSetBoVmFlashSave();

    return;
}
