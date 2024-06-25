#include "../lv_watch.h"

#define VM_MASK (0x55fd)

DndParaInfo_t Dnd_Info;

static const DndParaInfo_t InitInfo = {
    .main_sw = false,

    .enable = false,
    .dnd_start_hour = 22,
    .dnd_start_minute = 0,
    .dnd_end_hour = 8,
    .dnd_end_minute = 0,
    .dnd_repeat = 0x00,
};

void DndManualUpdate(void)
{
    struct sys_time utc_time;
    GetUtcTime(&utc_time);

    DndProcess(&utc_time);

    return;
}

void DndProcess(struct sys_time *utc_time)
{
    if(!utc_time) return;

#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    int cur_dnd_state;
    int next_dnd_state;
    u16 label = vm_label_dnd_state;

    cur_dnd_state = \
        GetVmParaCacheByLabel(label);

    bool main_sw = \
        Dnd_Info.main_sw;
    if(main_sw == true)
    {
        if(cur_dnd_state == \
            dnd_state_enable)
            return;

        next_dnd_state = \
            dnd_state_enable;
        SetVmParaCacheByLabel(label, \
            next_dnd_state);

        return;
    }

    bool enable = \
        Dnd_Info.enable;
    if(enable == false)
    {
        if(cur_dnd_state == \
            dnd_state_disable)
            return;

        next_dnd_state = \
            dnd_state_disable;
        SetVmParaCacheByLabel(label, \
            next_dnd_state);

        return;
    }

    u8 start_hour = \
        Dnd_Info.dnd_start_hour;
    u8 start_minute = \
        Dnd_Info.dnd_start_minute;
    u8 end_hour = \
        Dnd_Info.dnd_end_hour;
    u8 end_minute = \
        Dnd_Info.dnd_end_minute;
    u8 repeat = \
        Dnd_Info.dnd_repeat;

    u32 utc_ts = \
        utc_time->hour*60 + utc_time->min;
    u32 start_ts = \
        start_hour*60 + start_minute;
    u32 end_ts = \
        end_hour*60 + end_minute;
    
    u8 weekday = \
        GetUtcWeek(utc_time);
    u8 weeklday = \
        (weekday + Comm_Enum_Week_Max) - 1;
    weeklday %= Comm_Enum_Week_Max;

    if(end_ts >= start_ts)
    {
        //未在勿扰时间段内
        if(utc_ts < start_ts || utc_ts >= end_ts)
        {
            if(cur_dnd_state == \
                dnd_state_disable)
                return;

            next_dnd_state = \
                dnd_state_disable;
            SetVmParaCacheByLabel(label, \
                next_dnd_state);

            return;
        }

        //星期使能
        if(repeat & (0x1<<weekday))
        {
            if(cur_dnd_state == \
                dnd_state_enable)
                return;

            next_dnd_state = \
                dnd_state_enable;
            SetVmParaCacheByLabel(label, \
                next_dnd_state);
        }else
        {
            if(cur_dnd_state == \
                dnd_state_disable)
                return;

            next_dnd_state = \
                dnd_state_disable;
            SetVmParaCacheByLabel(label, \
                next_dnd_state);
        }
    }else
    {
        //未在勿扰时间段内
        if(utc_ts >= end_ts && utc_ts < start_ts)
        {
            if(cur_dnd_state == \
                dnd_state_disable)
                return;

            next_dnd_state = \
                dnd_state_disable;
            SetVmParaCacheByLabel(label, \
                next_dnd_state);
                
            return;
        }

        //星期使能
        if(utc_ts >= start_ts)
        {
            //还没到跨天
            if(repeat & (0x1<<weekday))
            {
                if(cur_dnd_state == \
                    dnd_state_enable)
                    return;

                next_dnd_state = \
                    dnd_state_enable;
                SetVmParaCacheByLabel(label, \
                    next_dnd_state);
            }else
            {
                if(cur_dnd_state == \
                    dnd_state_disable)
                    return;

                next_dnd_state = \
                    dnd_state_disable;
                SetVmParaCacheByLabel(label, \
                    next_dnd_state);
            }
        }else if(utc_ts < end_ts)
        {
            if(repeat & (0x1<<weeklday))
            {
                if(cur_dnd_state == \
                    dnd_state_enable)
                    return;

                next_dnd_state = \
                    dnd_state_enable;
                SetVmParaCacheByLabel(label, \
                    next_dnd_state);
            }else
            {
                if(cur_dnd_state == \
                    dnd_state_disable)
                    return;

                next_dnd_state = \
                    dnd_state_disable;
                SetVmParaCacheByLabel(label, \
                    next_dnd_state);
            }
        }
    }
}

void DndInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(DndParaInfo_t);

    int ret = syscfg_read(CFG_DND_PARA_INFO, \
        &Dnd_Info, vm_op_len);
    if(ret != vm_op_len || Dnd_Info.vm_mask != VM_MASK)
        DndInfoParaReset();

    return;
}

void DndInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(DndParaInfo_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_DND_PARA_INFO, \
            &Dnd_Info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void DndInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(DndParaInfo_t);

    memcpy(&Dnd_Info, &InitInfo, vm_op_len);

    Dnd_Info.vm_mask = VM_MASK;

    DndInfoParaUpdate();
    
    return;
}

void DndInfoParaUpdate(void)
{
    DndInfoParaWrite();

    return;
}