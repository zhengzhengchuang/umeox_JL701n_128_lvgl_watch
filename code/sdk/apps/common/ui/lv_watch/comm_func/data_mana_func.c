#include "../lv_watch.h"

void ResetAllVmData(void)
{
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
    /*开机:vm--->数据*/
    PowerOnSetHrVmCache();
    PowerOnSetBoVmCache();
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
