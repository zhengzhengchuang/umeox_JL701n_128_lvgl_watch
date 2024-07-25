#include "../lv_watch.h"

static u8 BoAutoCnt = 0;
static vm_bo_ctx_t w_bo_cache;
static const u8 BoAutoDur = 45;//自动心率时长 s

u8 GetBoRealVal(void)
{
    int real_val = \
        GetVmParaCacheByLabel(\
            vm_label_bo_real_val);

    return ((u8)real_val);
}
void SetBoRealVal(u8 val)
{
    SetVmParaCacheByLabel(\
        vm_label_bo_real_val, (int)val);

    return;
}

void SetBoVmCtxCache(u8 val)
{
    if(val == 0)
        return;

    u8 VmIdx = \
        w_bo.CurIdx;
    if(VmIdx >= Bo_Day_Num)
        return;

    w_bo.data[VmIdx] = val;

    u8 HIdx = \
       VmIdx/(60/Bo_Inv_Dur);
    if(HIdx >= 24)
        return;

    if(w_bo.min_data[HIdx] == 0)
        w_bo.min_data[HIdx] = val;

    if(w_bo.max_data[HIdx] == 0)
        w_bo.max_data[HIdx] = val;

    if(val < w_bo.min_data[HIdx])
        w_bo.min_data[HIdx] = val;

    if(val > w_bo.max_data[HIdx])
        w_bo.max_data[HIdx] = val;

    // printf("%d:min = %d, max = %d\n", HIdx, \
    //     w_bo.min_data[HIdx], w_bo.max_data[HIdx]);

    return;
}

//false 没过期  true 过期
static bool BoVmDataIsPast(struct sys_time *bo_time)
{
    if(!bo_time) return true;

    //今天
    struct sys_time *utc_time1 = \
        &(w_bo.time);
    struct sys_time *utc_time2 = \
        bo_time;

    if(utc_time1->year == utc_time2->year && \
        utc_time1->month == utc_time2->month && \
            utc_time1->day == utc_time2->day)
    {
        return false;
    }

    return true;
}

static bool GetBoData(void)
{
    bool ret = false;

    uint8_t bo_num = \
        VmBoItemNum();
    if(bo_num == 0)
        goto __end;

    u8 idx = bo_num - 1;
    bool r_bo_ret = \
        VmBoCtxByIdx(idx);
    if(r_bo_ret == false)
        goto __end;

    ret = true;

__end:
    return ret;
}

void PowerOnSetBoVmCache(void)
{
    /*初始化今天的vm缓存参数*/
    memset(&w_bo, 0, sizeof(vm_bo_ctx_t));
    w_bo.check_code = Nor_Vm_Check_Code;
    GetUtcTime(&(w_bo.time));
    w_bo.CurIdx = \
        (w_bo.time.hour*60 + w_bo.time.min)/Bo_Inv_Dur;

    /*读取vm的最新一条数据*/
    bool data_ret = GetBoData();
    if(!data_ret) return;

    u8 bo_num = VmBoItemNum();
    printf("%s:bo_num = %d\n", __func__, bo_num);

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = BoVmDataIsPast(&(r_bo.time));
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
    {
        //如果说已经过期数据，且存储数超过上限，删除最旧一条
        if(bo_num > Bo_Max_Days)
        {
            u8 del_idx = 0;
            VmBoCtxDelByIdx(del_idx);
        }

        return;
    }
        
    //删除副本数据，继续新的vm缓存
    u8 del_idx = bo_num - 1;
    VmBoCtxDelByIdx(del_idx);

    //拷贝保存的数据
    memcpy(w_bo.data, r_bo.data, Bo_Day_Num);
    memcpy(w_bo.min_data, r_bo.min_data, 24);
    memcpy(w_bo.max_data, r_bo.max_data, 24);
  
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

void BoTimerSecProcess(void)
{
    bool EnableFlag = \
        GetPpgEnableFlag();
    if(EnableFlag)
    {
        u8 work = GetPpgWorkType();
        if(work == PpgWorkHr || work == PpgWorkNone)
            return;

        u8 mode = GetPpgMode();
        if(mode == PpgModeAuto)
        {
            BoAutoCnt++;
            if(BoAutoCnt >= BoAutoDur)
            {
                BoAutoCnt = 0;
                DisablePpgModule();
            }
        }else
        {
            BoAutoCnt = 0;
        }
    }

    return;
}

void BoProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    if(ptime->hour == 0 && ptime->min == 0)
    {
        //保存上一天的心率历史数据
        SetBoDayVmData(true);
        memcpy(&w_bo_cache, &w_bo, sizeof(vm_bo_ctx_t));
        int ui_msg_post[1];
        ui_msg_post[0] = ui_msg_nor_bo_write;
        post_ui_msg(ui_msg_post, 1);
  
        //清除缓存数据，开始新一天的数据记录
        memset(&w_bo, 0, sizeof(vm_bo_ctx_t));
        w_bo.check_code = \
            Nor_Vm_Check_Code;
        memcpy(&(w_bo.time), ptime, \
            sizeof(struct sys_time));

        return;
    }

    u16 utc_timestamp = \
        (ptime->hour*60 + ptime->min);
    if(utc_timestamp > 0) 
        utc_timestamp -= 1;
    else
        return;

    u8 BoCurIdx = \
        utc_timestamp / Bo_Inv_Dur;
    w_bo.CurIdx = BoCurIdx;

    u8 BoInvOn = \
        utc_timestamp % Bo_Inv_Dur;
    int auto_bo_sw = \
        GetVmParaCacheByLabel(vm_label_auto_bo_sw);
    if(BoInvOn == 0 && auto_bo_sw)
    {
        //整点15分钟到 启动自动心率
        bool EnableFlag = \
            GetPpgEnableFlag();
        if(!EnableFlag)
        {
            BoAutoCnt = 0;
            SetBoRealVal(0);

            //如果PPG手动启动，禁止自动启动，只是辅助
            EnablePpgModule(PpgWorkBo, PpgModeAuto);
        }
    }

    return;
}
