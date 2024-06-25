#include "../lv_watch.h"

#define VM_MASK (0x55cc)

TasbihRInfoPara_t TasbihR_info;

static const TasbihRInfoPara_t InitInfo = { 
    .TasbihR_Days = 0x00, \
    .TasbihR_Enable = false, \
    .TasbihR_STimestamp = 8*3600, \
    .TasbihR_ETimestamp = 18*3600, \
    .TasbihR_TotalInvTime = 10*3600, \

    .TasbihR_SetInvTime = 2*3600,
};

void TasbihRInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(TasbihRInfoPara_t);

    int ret = syscfg_read(CFG_TASBIH_R_PARA_INFO, \
        &TasbihR_info, vm_op_len);
    if(ret != vm_op_len || TasbihR_info.vm_mask != VM_MASK)
        TasbihRInfoParaReset();
  
    return;
}

void TasbihRInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(TasbihRInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_TASBIH_R_PARA_INFO, \
            &TasbihR_info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void TasbihRInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(TasbihRInfoPara_t);

    memcpy(&TasbihR_info, &InitInfo, vm_op_len);

    TasbihR_info.vm_mask = VM_MASK;

    TasbihRInfoParaUpdate();

    return;
}

void TasbihRInfoParaUpdate(void)
{
    TasbihRInfoParaWrite();

    return;
}

static void TasbihReminderIsOnHandle(void)
{
    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    //震动
    motor_run(1, def_motor_duty);

    //播放
    //...

    ui_menu_jump(\
        ui_act_id_tasbih_remind);

    return;
}

void TasbihReminderProcess(struct sys_time *utc_time)
{
    if(!utc_time) return;

#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    bool TasbihR_Enable = \
        TasbihR_info.TasbihR_Enable;
    u32 TasbihR_STimestamp = \
        TasbihR_info.TasbihR_STimestamp;
    u32 TasbihR_ETimestamp = \
        TasbihR_info.TasbihR_ETimestamp;
    u32 TasbihR_SetInvTime = \
        TasbihR_info.TasbihR_SetInvTime;

    if(!TasbihR_Enable)
        return;

    if(TasbihR_STimestamp == \
        TasbihR_ETimestamp)
        return;

    if(TasbihR_SetInvTime == 0)
        return;

    u32 CurTimestamp = \
        utc_time->hour*3600 + utc_time->min*60;

    if(TasbihR_ETimestamp > TasbihR_STimestamp)
    { 
        if(CurTimestamp < TasbihR_STimestamp || \
            CurTimestamp > TasbihR_ETimestamp)
            return;

        u32 TimestampDiff = \
            CurTimestamp - TasbihR_STimestamp;
        if(TimestampDiff > 0 && \
            !(TimestampDiff % TasbihR_SetInvTime))
        {
            //条件成立
            TasbihReminderIsOnHandle();
        }
    }else if(TasbihR_ETimestamp < TasbihR_STimestamp)
    {
        if(CurTimestamp < TasbihR_STimestamp && \
            CurTimestamp > TasbihR_ETimestamp)
            return;

        u32 TimestampDiff;
        if(CurTimestamp >= TasbihR_STimestamp)
            TimestampDiff = \
                CurTimestamp - TasbihR_STimestamp;
        else
            TimestampDiff = \
                CurTimestamp + 86400 - TasbihR_STimestamp;

        if(TimestampDiff > 0 && \
            !(TimestampDiff % TasbihR_SetInvTime))
        {
            //条件成立
            TasbihReminderIsOnHandle();
        }
    }

    return;
}
