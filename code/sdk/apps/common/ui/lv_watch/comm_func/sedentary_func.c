#include "../lv_watch.h"

#define VM_MASK (0x55ab)

SedInfoPara_t Sed_Info;
static const float SedSteps = 120.0f;

#define __this_module (&Sed_Info)
#define __this_module_size (sizeof(SedInfoPara_t))

static const SedInfoPara_t init = 
{
    .enable = false,
    .start_hour = 10,
    .start_minute = 0,
    .end_hour = 18,
    .end_minute = 0,
    .repeat = 0x00,

    .steps = 0.0f,
};

static void SedIsOnHandle(void)
{
    int dnd_state = GetVmParaCacheByLabel(vm_label_dnd_state);
    if(dnd_state == dnd_state_enable) return;

    if(!MenuSupportPopup()) 
        return;
        
    motor_run(1, def_motor_duty);
    ui_menu_jump(ui_act_id_sedentary);

    return;
}

void SedSetSteps(float steps)
{
    if(steps == 0) return;

    __this_module->steps += steps;
    SedInfoParaUpdate();

    return;
}

void SedUtcMinProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    bool enable = __this_module->enable;
    if(enable == false)
    {
        if(ptime->min == 0)
        {
            __this_module->steps = 0.0f;
            SedInfoParaUpdate();
        }

        return;
    }
        
    u8 start_hour = __this_module->start_hour;
    u8 start_minute = __this_module->start_minute;
    u8 end_hour = __this_module->end_hour;
    u8 end_minute = __this_module->end_minute;
    u8 repeat = __this_module->repeat;

    u8 weekday = GetUtcWeek(ptime);
    u32 start_ts = start_hour*60 + start_minute;
    u32 end_ts = end_hour*60 + end_minute;
    u32 utc_ts = ptime->hour*60 + ptime->min;
    
    if(end_ts < start_ts) return;

    /*久坐提醒不存在跨天的情况*/
    if(end_ts >= start_ts)
    {
        //未在时间段内
        if(utc_ts < start_ts || utc_ts > end_ts)
            return;

        if(utc_ts == start_ts)
        {
            __this_module->steps = 0.0f;
            SedInfoParaUpdate();
        }
  
        if(ptime->min == 0 && utc_ts != start_ts)
        {
            //整小时判断
            if(repeat & (0x1<<weekday))
            {
                if(__this_module->steps < SedSteps)
                    SedIsOnHandle();
            }
            
            __this_module->steps = 0.0f;
            SedInfoParaUpdate();
        }
    }
    
    return;
}

void SedInfoParaRead(void)
{
    int ret = syscfg_read(CFG_SEDENTARY_PARA_INFO, \
        __this_module, __this_module_size);
    if(ret != __this_module_size || __this_module->vm_mask != VM_MASK)
        SedInfoParaReset();
    
    return;
}

void SedInfoParaWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_SEDENTARY_PARA_INFO, \
            __this_module, __this_module_size);
        if(ret == __this_module_size)
            break;
    }

    return;
}

void SedInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(SedInfoPara_t);

    memcpy(__this_module, &init, vm_op_len);

    __this_module->vm_mask = VM_MASK;

    SedInfoParaUpdate();

    return;
}

void SedInfoParaUpdate(void)
{
    SedInfoParaWrite();

    return;
}
