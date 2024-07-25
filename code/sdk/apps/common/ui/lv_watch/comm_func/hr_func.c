#include "../lv_watch.h"

static u32 HrAutoCnt = 0;
static vm_hr_ctx_t w_hr_cache;
static const u8 HrAutoDur = 45;//自动心率时长 s

u8 GetHrRealVal(void)
{
    int real_val = \
        GetVmParaCacheByLabel(vm_label_hr_real_val);

    return ((u8)real_val);
}
void SetHrRealVal(u8 val)
{
    SetVmParaCacheByLabel(\
        vm_label_hr_real_val, (int)val);

    return;
}

void SetHrVmCtxCache(u8 val)
{
    if(val == 0)
        return;

    u8 VmIdx = w_hr.CurIdx;
    if(VmIdx >= Hr_Day_Num)
        return;

    w_hr.data[VmIdx] = val;

    u8 HIdx = VmIdx/(60/Hr_Inv_Dur);
    if(HIdx >= 24)
        return;

    if(w_hr.min_data[HIdx] == 0)
        w_hr.min_data[HIdx] = val;

    if(w_hr.max_data[HIdx] == 0)
        w_hr.max_data[HIdx] = val;

    if(val < w_hr.min_data[HIdx])
        w_hr.min_data[HIdx] = val;

    if(val > w_hr.max_data[HIdx])
        w_hr.max_data[HIdx] = val;

    // printf("%d:min = %d, max = %d\n", HIdx, \
    //     w_hr.min_data[HIdx], w_hr.max_data[HIdx]);

    return;
}

//false 没过期  true 过期
static bool HrVmDataIsPast(struct sys_time *hr_time)
{
    if(!hr_time) return true;

    //今天
    struct sys_time *utc_time1 = &(w_hr.time);
    struct sys_time *utc_time2 = hr_time;

    if(utc_time1->year == utc_time2->year && \
        utc_time1->month == utc_time2->month && \
            utc_time1->day == utc_time2->day)
    {
        return false;
    }
        

    return true;
}

static bool GetHrData(void)
{
    bool ret = false;

    uint8_t hr_num = \
        VmHrItemNum();
    if(hr_num == 0)
        goto __end;

    u8 idx = hr_num - 1;
    bool r_hr_ret = \
        VmHrCtxByIdx(idx);
    if(r_hr_ret == false)
        goto __end;

    ret = true;

__end:
    return ret;
}

void PowerOnSetHrVmCache(void)
{
    printf("%s:%d\n", __func__, sizeof(vm_hr_ctx_t));

    /*初始化今天的vm缓存参数*/
    memset(&w_hr, 0, sizeof(vm_hr_ctx_t));
    w_hr.check_code = Nor_Vm_Check_Code;
    GetUtcTime(&(w_hr.time));
    w_hr.CurIdx = (w_hr.time.hour*60 + w_hr.time.min)/Hr_Inv_Dur;

    u8 hr_num = VmHrItemNum();
    printf("%s:hr_num = %d\n", __func__, hr_num);

    /*读取vm的最新一条数据*/
    bool data_ret = GetHrData();
    if(!data_ret) return;

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = \
        HrVmDataIsPast(&(r_hr.time));
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
    {
        //如果说已经过期数据，且存储数超过上限，删除最旧一条
        if(hr_num > Hr_Max_Days)
        {
            u8 del_idx = 0;
            VmHrCtxDelByIdx(del_idx);
        }

        return;
    }
        

    //删除副本数据，继续新的vm缓存
    u8 del_idx = hr_num - 1;
    VmHrCtxDelByIdx(del_idx);

    //拷贝保存的数据
    memcpy(w_hr.data, r_hr.data, Hr_Day_Num);
    memcpy(w_hr.min_data, r_hr.min_data, 24);
    memcpy(w_hr.max_data, r_hr.max_data, 24);

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
    bool PpgEn = GetPpgEnableFlag();
    if(PpgEn == true)
    {
        u8 work = GetPpgWorkType();
        if(work == PpgWorkBo || work == PpgWorkNone)
            return;

        u8 mode = GetPpgMode();
        if(mode == PpgModeAuto)
        {
            HrAutoCnt++;
            if(HrAutoCnt >= HrAutoDur)
            {
                HrAutoCnt = 0;
                DisablePpgModule();
            }
        }else
        {
            HrAutoCnt = 0;
        }
    }

    return;
}

void HrProcess(struct sys_time *ptime)
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
        memcpy(&(w_hr.time), ptime, sizeof(struct sys_time));
    }

    u16 utc_timestamp = \
        ptime->hour*60 + ptime->min;
    u8 HrCurIdx = \
        utc_timestamp / Hr_Inv_Dur;
    w_hr.CurIdx = HrCurIdx;

    u8 HrInvOn = \
        utc_timestamp % Hr_Inv_Dur;
    int auto_hr_sw = \
        GetVmParaCacheByLabel(vm_label_auto_hr_sw);
    if(HrInvOn == 0 && auto_hr_sw)
    {
        //整点15分钟到 启动自动心率
        bool EnableFlag = \
            GetPpgEnableFlag();
        if(!EnableFlag)
        {
            HrAutoCnt = 0;
            SetHrRealVal(0);

            //如果PPG手动启动，禁止自动启动，只是辅助
            EnablePpgModule(PpgWorkHr, PpgModeAuto);
        }
    }
    
    return;
}