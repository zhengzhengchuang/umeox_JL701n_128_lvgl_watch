#include "../lv_watch.h"

#define VM_MASK (0x55aa)

SedInfoPara_t Sed_Info;

static const u16 SedLimitSteps = 120;

static const SedInfoPara_t InitInfo = {
    .enable = false,
    .start_hour = 10,
    .start_minute = 0,
    .end_hour = 18,
    .end_minute = 0,
    .repeat = 0x00,
};

void SedInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(SedInfoPara_t);

    int ret = syscfg_read(CFG_SEDENTARY_PARA_INFO, \
        &Sed_Info, vm_op_len);
    if(ret != vm_op_len || Sed_Info.vm_mask != VM_MASK)
        SedInfoParaReset();
    
    return;
}

void SedInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(SedInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_SEDENTARY_PARA_INFO, \
            &Sed_Info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void SedInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(SedInfoPara_t);

    memcpy(&Sed_Info, &InitInfo, vm_op_len);

    Sed_Info.vm_mask = VM_MASK;

    SedInfoParaUpdate();

    return;
}

void SedInfoParaUpdate(void)
{
    // printf("%d, 0x%x\n", Sed_Info.enable, Sed_Info.repeat);
    // printf("%d, %d\n", Sed_Info.start_hour, Sed_Info.end_hour);
    // printf("%d, %d\n", Sed_Info.start_minute, Sed_Info.end_minute);

    SedInfoParaWrite();

    return;
}

static void SedIsOnHandle(void)
{
    int dnd_state = \
        GetVmParaCacheByLabel(vm_label_dnd_state);
    if(dnd_state == dnd_state_enable)
        return;

    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    //震动
    motor_run(1, def_motor_duty);

    ui_menu_jump(ui_act_id_sedentary);

    return;
}

void SedProcess(struct sys_time *utc_time)
{
    if(!utc_time) return;

#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    bool enable = Sed_Info.enable;
    if(enable == false)
    {
        if(utc_time->min == 0)
            SetVmParaCacheByLabel(vm_label_sedentary_steps, 0);

        return;
    }
        
    u8 start_hour = Sed_Info.start_hour;
    u8 start_minute = Sed_Info.start_minute;
    u8 end_hour = Sed_Info.end_hour;
    u8 end_minute = Sed_Info.end_minute;
    u8 repeat = Sed_Info.repeat;

    u8 weekday = GetUtcWeek(utc_time);
    u32 start_ts = start_hour*60 + start_minute;
    u32 end_ts = end_hour*60 + end_minute;
    u32 utc_ts = utc_time->hour*60 + utc_time->min;
    
    if(end_ts < start_ts) return;

    /*久坐提醒不存在跨天的情况*/
    if(end_ts >= start_ts)
    {
        //未在时间段内
        if(utc_ts < start_ts || utc_ts > end_ts)
            return;

        if(utc_ts == start_ts)
            SetVmParaCacheByLabel(\
                vm_label_sedentary_steps, 0);

        if(utc_time->min == 0 && utc_ts != start_ts)
        {
            //整小时判断
            int SedSteps = \
                GetVmParaCacheByLabel(vm_label_sedentary_steps);

            if(repeat & (0x1<<weekday))
            {
                if(SedSteps >= SedLimitSteps)
                    SedIsOnHandle();
            }
            
            SetVmParaCacheByLabel(vm_label_sedentary_steps, 0);
        }
    }
    
    return;
}