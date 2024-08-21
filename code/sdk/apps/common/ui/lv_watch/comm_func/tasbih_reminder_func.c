#include "../lv_watch.h"
#include "../../../../watch/include/tone_player.h"

#define VM_MASK (0x55cc)

TasbihRInfoPara_t TasbihR_info;

static const TasbihRInfoPara_t Init = { 
    .TasbihR_Days = 0x00, \
    .TasbihR_Enable = false, \
    .TasbihR_STimestamp = 8*3600, \
    .TasbihR_ETimestamp = 18*3600, \
    .TasbihR_TotalInvTime = 10*3600, \

    .TasbihR_SetInvTime = 2*3600,
};

void TasbihRInfoParaRead(void)
{
    int vm_op_len = sizeof(TasbihRInfoPara_t);

    int ret = syscfg_read(CFG_TASBIH_R_PARA_INFO, \
        &TasbihR_info, vm_op_len);
    if(ret != vm_op_len || TasbihR_info.vm_mask != VM_MASK)
        TasbihRInfoParaReset();
  
    return;
}

void TasbihRInfoParaWrite(void)
{
    int vm_op_len = sizeof(TasbihRInfoPara_t);
    
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
    int vm_op_len = sizeof(TasbihRInfoPara_t);

    memcpy(&TasbihR_info, &Init, vm_op_len);

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
    if(!MenuSupportPopup()) return;

    motor_run(1, def_motor_duty);
    tone_play_with_callback_by_name(tone_table[IDEX_TONE_MSG_NOTIFY], 1, NULL, NULL);

    ui_menu_jump(ui_act_id_tasbih_remind);

    return;
}

void TasbihReminderProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    bool TasbihR_Enable = TasbihR_info.TasbihR_Enable;
    u32 TasbihR_STimestamp = TasbihR_info.TasbihR_STimestamp;
    u32 TasbihR_ETimestamp = TasbihR_info.TasbihR_ETimestamp;
    u32 TasbihR_SetInvTime = TasbihR_info.TasbihR_SetInvTime;

    if(TasbihR_Enable == false)
        return;

    if(TasbihR_STimestamp == TasbihR_ETimestamp)
        return;

    if(TasbihR_SetInvTime == 0)
        return;

    u32 timestamp = ptime->hour*3600 + ptime->min*60;

    if(TasbihR_ETimestamp > TasbihR_STimestamp)
    { 
        if(timestamp < TasbihR_STimestamp || timestamp > TasbihR_ETimestamp)
            return;

        u32 timestamp_diff = timestamp - TasbihR_STimestamp;
        if(timestamp_diff > 0 && !(timestamp_diff % TasbihR_SetInvTime))
            TasbihReminderIsOnHandle();
    }else if(TasbihR_ETimestamp < TasbihR_STimestamp)
    {
        if(timestamp < TasbihR_STimestamp && timestamp > TasbihR_ETimestamp)
            return;

        u32 timestamp_diff;
        if(timestamp >= TasbihR_STimestamp)
            timestamp_diff = timestamp - TasbihR_STimestamp;
        else
            timestamp_diff = timestamp + SEC_PER_DAY - TasbihR_STimestamp;

        if(timestamp_diff > 0 && !(timestamp_diff % TasbihR_SetInvTime))
            TasbihReminderIsOnHandle();
    }

    return;
}
