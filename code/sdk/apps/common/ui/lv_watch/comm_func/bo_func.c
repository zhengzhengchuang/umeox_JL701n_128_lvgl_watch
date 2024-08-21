#include "../lv_watch.h"

#define VM_MASK (0x55aa)

static u8 auto_bo_cnt;
static u8 auto_bo_sync;
static bool auto_bo_enable;
static const u8 auto_bo_dur = 45;//自动血氧时长 s

static vm_bo_ctx_t w_bo_cache;

BoPara_t BoPara;
static const BoPara_t init = {
    .bo_real = 0,
    .bo_min = {0}, .bo_max = {0},
};

#define __this_module (&BoPara)
#define __this_module_size (sizeof(BoPara_t))

u8 GetBoRealVal(void)
{
    return __this_module->bo_real;
}

void SetBoRealVal(u8 val)
{
    if(val == 0) return;

    __this_module->bo_real = val;

    u8 VmIdx = w_bo.CurIdx;
    if(VmIdx >= Bo_Day_Num)
        return;

    u8 HIdx = VmIdx/(60/Bo_Inv_Dur);
    if(HIdx >= 24) return;

    if(__this_module->bo_min[HIdx] == 0)
        __this_module->bo_min[HIdx] = val;

    if(__this_module->bo_max[HIdx] == 0)
        __this_module->bo_max[HIdx] = val;

    if(val < __this_module->bo_min[HIdx])
        __this_module->bo_min[HIdx] = val;

    if(val > __this_module->bo_max[HIdx])
        __this_module->bo_max[HIdx] = val;

    BoDataVmWrite();

    return;
}

void ClearBoRealVal(void)
{
    __this_module->bo_real = 0;
    BoDataVmWrite();
    return;
}

void SetBoVmCtxCache(u8 val)
{
    if(val == 0) return;

    u8 VmIdx = w_bo.CurIdx;
    if(VmIdx >= Bo_Day_Num)
        return;

    w_bo.data[VmIdx] = val;

    return;
}

//false 没过期  true 过期
static bool BoVmDataIsPast(u32 timestamp)
{
    if(w_bo.timestamp < timestamp) 
        goto __end;

    if(w_bo.timestamp - timestamp < SEC_PER_DAY) 
        return false;

__end:
    return true;
}

static bool GetBoData(void)
{
    bool ret = false;

    u8 bo_num = VmBoItemNum();
    if(bo_num == 0)
        goto __end;

    u8 idx = bo_num - 1;
    bool r_bo_ret = VmBoCtxByIdx(idx);
    if(r_bo_ret == false)
        goto __end;

    ret = true;

__end:
    return ret;
}

void WBoParaInit(void)
{
    memset(&w_bo, 0, sizeof(vm_bo_ctx_t));
    w_bo.check_code = Nor_Vm_Check_Code;
    struct sys_time time;
    GetUtcTime(&time);
    w_bo.CurIdx = (time.hour*60 + time.min)/Bo_Inv_Dur;
    time.hour = 0; time.min = 0; time.sec = 0;
    w_bo.timestamp = UtcTimeToSec(&time);

    return;
}

void PowerOnSetBoVmCache(void)
{
    printf("%s:%d\n", __func__, sizeof(vm_bo_ctx_t));

    WBoParaInit();

    u8 num = VmBoItemNum();
    printf("%s:bo_num = %d\n", __func__, num);

    /*读取vm的最新一条数据*/
    bool data_ret = GetBoData();
    if(data_ret == false) return;

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = BoVmDataIsPast(r_bo.timestamp);
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
    {
        //过期数据，清除用户数据
        __this_module->bo_real = 0;
        memset(__this_module->bo_min, 0, 24);
        memset(__this_module->bo_max, 0, 24);
        BoDataVmWrite();

        //如果说已经过期数据，且存储数超过上限，删除最旧一条
        if(num > Bo_Max_Days)
        {
            u8 del_idx = 0;
            VmBoCtxDelByIdx(del_idx);
        }

        return;
    }
        
    //删除副本数据，继续新的vm缓存
    u8 del_idx = num - 1;
    VmBoCtxDelByIdx(del_idx);

    //拷贝保存的数据
    memcpy(w_bo.data, r_bo.data, Bo_Day_Num);

    return;
}

void PowerOffSetBoVmFlashSave(void)
{
    SetBoDayVmData(false);
    memcpy(&w_bo_cache, &w_bo, sizeof(vm_bo_ctx_t));
    int ui_msg_post[1];
    ui_msg_post[0] = ui_msg_nor_bo_write;
    post_ui_msg(ui_msg_post, 1);
 
    return;
}

void VmBoCtxFlashWrite(void)
{
    if(w_bo_cache.check_code != 0)
        VmBoCtxFlashSave(&w_bo_cache);

    memset(&w_bo_cache, 0, sizeof(vm_bo_ctx_t));

    return;
}

/* 可能存在自动血氧过程中，睡眠需要开ppg，这个时候需要重新恢复血氧 */
void SetAutoBoRestart(void)
{
    auto_bo_cnt = 0;
    auto_bo_sync = 0;
    auto_bo_enable = true;
    
    return;
}

void BoTimerSecProcess(void)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    if(auto_bo_enable == true)
    {
        bool en = GetPpgEnableFlag();
        if(en == true) goto __end;

        auto_bo_sync++;
        auto_bo_sync %= 2;
        if(auto_bo_sync == 1)
            goto __end;

        auto_bo_enable = false;
        auto_bo_cnt = 0;
        EnablePpgModule(PpgWorkBo, PpgModeAuto);
    }

    u8 work = GetPpgWorkType();
    if(work == PpgWorkBo)
    {
        u8 mode = GetPpgMode();
        if(mode == PpgModeManual)
        {
            /* 手动血氧，不做处理 */
            auto_bo_cnt = 0;
            goto __end;
        }else
        {
            /* 正常自动血氧流程 */
            auto_bo_cnt++;
            if(auto_bo_cnt >= auto_bo_dur)
            {
                auto_bo_cnt = 0;
                DisablePpgModule();
            }
        }
    }else
    {
        /* 传感器不工作在血氧，不做处理 */
        auto_bo_cnt = 0;
        goto __end;
    }

__end:
    return;
}

void BoUtcMinProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    if(ptime->hour == 0 && ptime->min == 0)
    {
        //保存上一天的血氧历史数据
        SetBoDayVmData(true);
        memcpy(&w_bo_cache, &w_bo, sizeof(vm_bo_ctx_t));
        int ui_msg_post[1];
        ui_msg_post[0] = ui_msg_nor_bo_write;
        post_ui_msg(ui_msg_post, 1);
  
        //清除缓存数据，开始新一天的数据记录
        memset(&w_bo, 0, sizeof(vm_bo_ctx_t));
        w_bo.check_code = Nor_Vm_Check_Code;
        w_bo.timestamp = UtcTimeToSec(ptime);
   
        __this_module->bo_real = 0;
        memset(__this_module->bo_min, 0, 24);
        memset(__this_module->bo_max, 0, 24);
        BoDataVmWrite();

        return;
    }

    u32 timestamp = (ptime->hour*60 + ptime->min);
    if(timestamp > 0) timestamp -= 1;
    else return;

    u8 idx = timestamp/Bo_Inv_Dur;
    w_bo.CurIdx = idx;
    u8 on = timestamp%Bo_Inv_Dur;
    int auto_sw = GetVmParaCacheByLabel(vm_label_auto_bo_sw);
    u8 charge_state = GetChargeState();
    if(on == 0 && auto_sw == 1 && charge_state == 0)
    {
        /* 自动血氧 */
        u8 work = GetPpgWorkType();
        if(work == PpgWorkBo)
        {
            /* 如果当前已经工作在血氧，这本次自动血氧不会开启 */
            auto_bo_enable = false;
        }else
        {
            auto_bo_sync = 0;
            auto_bo_enable = true;
        }
    }else
    {
        auto_bo_enable = false;
    }

    return;
}

void BoDataVmRead(void)
{
    int ret = syscfg_read(CFG_BO_PARA_INFO, \
        __this_module, __this_module_size);
    if(ret != __this_module_size || __this_module->vm_mask != VM_MASK)
        BoDataVmReset();

    return;
}

void BoDataVmWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_BO_PARA_INFO, \
            __this_module, __this_module_size);
        if(ret == __this_module_size)
            break;
    }

    return;
}

void BoDataVmReset(void)
{
    memcpy(__this_module, &init, __this_module_size);

    __this_module->vm_mask = VM_MASK;

    BoDataVmWrite();

    return;
}
