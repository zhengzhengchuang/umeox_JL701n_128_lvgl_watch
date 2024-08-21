#include "../lv_watch.h"

#define VM_MASK (0x55aa)

static u32 auto_hr_cnt;
static bool auto_hr_enable;
static const u8 auto_hr_dur = 45;//自动心率时长 s

static vm_hr_ctx_t w_hr_cache;

HrPara_t HrPara;
static const HrPara_t init = {
    .hr_real = 0, .hr_min = {0}, .hr_max = {0},
    .hr_low_sw = 1, .hr_high_sw = 1,
    .hr_low_val = 40, .hr_high_val = 150,
};

#define __this_module (&HrPara)
#define __this_module_size (sizeof(HrPara_t))

u8 GetHrRealVal(void)
{
    return __this_module->hr_real;
}

void SetHrRealVal(u8 val)
{
    if(val == 0) return;

    __this_module->hr_real = val;

    u8 VmIdx = w_hr.CurIdx;
    if(VmIdx >= Hr_Day_Num)
        return;

    u8 HIdx = VmIdx/(60/Hr_Inv_Dur);
    if(HIdx >= 24) return;

    if(__this_module->hr_min[HIdx] == 0)
        __this_module->hr_min[HIdx] = val;

    if(__this_module->hr_max[HIdx] == 0)
        __this_module->hr_max[HIdx] = val;

    if(val < __this_module->hr_min[HIdx])
        __this_module->hr_min[HIdx] = val;

    if(val > __this_module->hr_max[HIdx])
        __this_module->hr_max[HIdx] = val;

    HrDataVmWrite();

    return;
}

void ClearHrRealVal(void)
{
    __this_module->hr_real = 0;
    HrDataVmWrite();
}

void SetHrVmCtxCache(u8 val)
{
    if(val == 0) return;

    u8 VmIdx = w_hr.CurIdx;
    if(VmIdx >= Hr_Day_Num)
        return;

    w_hr.data[VmIdx] = val;

    return;
}

//false 没过期  true 过期
static bool HrVmDataIsPast(u32 timestamp)
{
    if(w_hr.timestamp < timestamp) 
        goto __end;

    if(w_hr.timestamp - timestamp < SEC_PER_DAY) 
        return false;

__end:
    return true;
}

static bool GetHrData(void)
{
    bool ret = false;

    u8 num = VmHrItemNum();
    if(num == 0) goto __end;

    u8 idx = num - 1;
    bool r_hr_ret = VmHrCtxByIdx(idx);
    if(r_hr_ret == false)
        goto __end;

    ret = true;

__end:
    return ret;
}

void WHrParaInit(void)
{
    memset(&w_hr, 0, sizeof(vm_hr_ctx_t));
    w_hr.check_code = Nor_Vm_Check_Code;
    struct sys_time time;
    GetUtcTime(&time);
    w_hr.CurIdx = (time.hour*60 + time.min)/Hr_Inv_Dur;
    time.hour = 0; time.min = 0; time.sec = 0;
    w_hr.timestamp = UtcTimeToSec(&time);
    printf("%s:CurIdx = %d\n", __func__, w_hr.CurIdx);

    return;
}

void PowerOnSetHrVmCache(void)
{
    printf("%s:%d\n", __func__, sizeof(vm_hr_ctx_t));

    WHrParaInit();

    u8 num = VmHrItemNum();
    printf("%s:hr_num = %d\n", __func__, num);

    /*读取vm的最新一条数据*/
    bool data_ret = GetHrData();
    if(data_ret == false) return;

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = HrVmDataIsPast(r_hr.timestamp);
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
    {
        //过期数据，清除用户数据
        __this_module->hr_real = 0;
        memset(__this_module->hr_min, 0, 24);
        memset(__this_module->hr_max, 0, 24);
        HrDataVmWrite();

        //如果说已经过期数据，且存储数超过上限，删除最旧一条
        if(num > Hr_Max_Days)
        {
            u8 del_idx = 0;
            VmHrCtxDelByIdx(del_idx);
        }

        return;
    }
        
    //删除副本数据，继续新的vm缓存
    u8 del_idx = num - 1;
    VmHrCtxDelByIdx(del_idx);

    //拷贝保存的数据
    memcpy(w_hr.data, r_hr.data, Hr_Day_Num);

    return;
}

void PowerOffSetHrVmFlashSave(void)
{
    SetHrDayVmData(false);
    memcpy(&w_hr_cache, &w_hr, sizeof(vm_hr_ctx_t));
    int ui_msg_post[1];
    ui_msg_post[0] = ui_msg_nor_hr_wirte;
    post_ui_msg(ui_msg_post, 1);

    return;
}

void VmHrCtxFlashWrite(void)
{
    if(w_hr_cache.check_code != 0)
        VmHrCtxFlashSave(&w_hr_cache);

    memset(&w_hr_cache, 0, sizeof(vm_hr_ctx_t));

    return;
}

void HrTimerSecProcess(void)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(auto_hr_enable == true)
    {
        bool en = GetPpgEnableFlag();
        if(en == true) goto __end;

        auto_hr_enable = false;
        auto_hr_cnt = 0;
        EnablePpgModule(PpgWorkHr, PpgModeAuto);
    }

    u8 work = GetPpgWorkType();
    if(work == PpgWorkHr)
    {
        u8 mode = GetPpgMode();
        if(mode == PpgModeManual)
        {
            /* 手动心率，不做处理 */
            auto_hr_cnt = 0;
            goto __end;
        }else
        {
            bool fall_asleep = GetFallAsleepFlag();
            if(fall_asleep == true)
            {
                /* 睡眠自动心率，不做处理 */
                auto_hr_cnt = 0;
                goto __end;
            }else
            {
                /* 正常自动心率流程 */
                auto_hr_cnt++;
                if(auto_hr_cnt >= auto_hr_dur)
                {
                    auto_hr_cnt = 0;
                    DisablePpgModule();
                }
            }
        }
    }else
    {
        /* 传感器不工作在心率，不做处理 */
        auto_hr_cnt = 0;
        goto __end;
    }

__end:
    return;
}

void HrUtcMinProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(ptime->hour == 0 && ptime->min == 0)
    {
        //保存上一天的心率历史数据
        SetHrDayVmData(true);
        memcpy(&w_hr_cache, &w_hr, sizeof(vm_hr_ctx_t));
        int ui_msg_post[1];
        ui_msg_post[0] = ui_msg_nor_hr_wirte;
        post_ui_msg(ui_msg_post, 1);
    
        //清除缓存数据，开始新一天的数据记录
        memset(&w_hr, 0, sizeof(vm_hr_ctx_t));
        w_hr.check_code = Nor_Vm_Check_Code;
        w_hr.timestamp = UtcTimeToSec(ptime);
   
        __this_module->hr_real = 0;
        memset(__this_module->hr_min, 0, 24);
        memset(__this_module->hr_max, 0, 24);
        HrDataVmWrite();
    }

    u32 timestamp = ptime->hour*60 + ptime->min;
    u8 idx = timestamp/Hr_Inv_Dur;
    w_hr.CurIdx = idx;
    u8 on = timestamp%Hr_Inv_Dur;
    int auto_sw = GetVmParaCacheByLabel(vm_label_auto_hr_sw);
    u8 charge_state = GetChargeState();
    u8 upgrade_state = GetOtaUpgradeState();
    if(on == 0 && auto_sw == 1 && charge_state == 0 && upgrade_state == upgrade_none)
    {
        /* 自动心率 */
        u8 work = GetPpgWorkType();
        if(work == PpgWorkHr)
        {
            /* 如果当前已经工作在心率，这本次自动心率不会开启 */
            auto_hr_enable = false;
        }else
        {
            bool fall_asleep = GetFallAsleepFlag();
            if(fall_asleep == true)
            {
                /* 入睡不开启自动心率，由算法睡眠去接管 */
                auto_hr_enable = false;
            }else
            {
                auto_hr_enable = true;
                printf("%s:enable ppg\n", __func__);
            }
        }
    }else
    {
        auto_hr_enable = false;
    }

    return;
}

void HrDataVmRead(void)
{
    int ret = syscfg_read(CFG_HR_PARA_INFO, __this_module, \
        __this_module_size);
    if(ret != __this_module_size || __this_module->vm_mask != VM_MASK)
        HrDataVmReset();

    return;
}

void HrDataVmWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_HR_PARA_INFO, __this_module, \
            __this_module_size);
        if(ret == __this_module_size)
            break;
    }

    return;
}

void HrDataVmReset(void)
{
    memcpy(__this_module, &init, __this_module_size);

    __this_module->vm_mask = VM_MASK;

    HrDataVmWrite();

    return;
}
