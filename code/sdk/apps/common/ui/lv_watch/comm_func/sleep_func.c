#include "../lv_watch.h"

#define VM_MASK (0x55aa)

SlpPara_t SlpPara;

#define __this_module (&SlpPara)
#define __this_module_size (sizeof(SlpPara_t))
static const SlpPara_t init = {
    .valid = false, 
    .slp_dur = 0.0f, 
    .d_slp_dur = 0.0f, .l_slp_dur = 0.0f, .r_slp_dur = 0.0f, .wake_dur = 0.0f,
    .d_slp_radio = 0.0f, .l_slp_radio = 0.0f, .r_slp_radio = 0.0f, .wake_radio = 0.0f,
    .slp_start_ts = 0,.slp_end_ts = 0,
};

struct SleepSummaryInput slp_in;
struct SleepSummaryOutput slp_out;

/* true：睡眠启动 false:睡眠关闭*/
static bool slp_en;
bool GetSleepEnState(void)
{
    return slp_en;
}
void SetSleepEnState(bool en)
{
    slp_en = en;
    return;
}

/* true：入睡成功 false：醒来 */
static bool FallAsleep;
bool GetFallAsleepFlag(void)
{
    return FallAsleep;
}
void SetFallAsleepFlag(bool f)
{
    FallAsleep = f;
    return;
}

//false 没过期  true 过期
static bool SleepVmDataIsPast(u32 timestamp)
{
    struct sys_time time;
    GetUtcTime(&time);
    u32 cur_timestamp = UtcTimeToSec(&time);
    if(cur_timestamp < timestamp) goto __end;

    if(cur_timestamp - timestamp < SEC_PER_DAY) 
        return false;

__end:
    return true;
}

static bool GetSleepData(void)
{
    bool ret = false;

    u8 num = VmSleepItemNum();
    if(num == 0) goto __end;

    u8 idx = num - 1;
    bool r_ret = VmSleepCtxByIdx(idx);
    if(r_ret == false) goto __end;

    ret = true;

__end:
    return ret;
}

void PowerOnSetSleepData(void)
{
    /*读取vm的最新一条数据*/
    bool data_ret = GetSleepData();
    if(data_ret == false) return;

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = SleepVmDataIsPast(r_sleep.timestamp);
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
        SlpDataVmReset();

    return;
}

/* 设置算法自动监测标志位 */
static void SetSleepAutoDetEnFlag(bool en)
{
    SleepConfig slp_cfg;
    getSleepConfig(0, &slp_cfg);
    slp_cfg.disableAutoDetection = en;
    int16_t cfg = setSleepConfig(&slp_cfg);
    if(cfg == 0) printf("setSleepConfig success\n");
    // printf("minNapLength = %d\n", slp_cfg.minNapLength);
    // printf("maxGapLength = %d\n", slp_cfg.maxGapLength);
    // printf("disablePeriodFinetuning = %d\n", slp_cfg.disablePeriodFinetuning);
    // printf("disableAutoDetection = %d\n", slp_cfg.disableAutoDetection);
    // printf("longSleepPeriod[0] = %d\n", slp_cfg.longSleepPeriod[0]);
    // printf("longSleepPeriod[1] = %d\n", slp_cfg.longSleepPeriod[1]);

    return;
}

static void GetSlpSummary(struct SleepSummaryInput *in, struct SleepSummaryOutput *out) 
{
    getEmbeddedSleepSummary(in, out);

    /* 短睡不做处理 */
    if(out->type == 2)
        return;

    /* 长睡 */
    if(out->type == 1)
    {
        __this_module->valid = true;
        __this_module->slp_dur = out->sleepPeriod;
        __this_module->d_slp_dur = out->deepNumMinutes;
        __this_module->l_slp_dur = out->lightNumMinutes;
        __this_module->r_slp_dur = out->remNumMinutes;
        __this_module->wake_dur = out->wakeNumMinutes;
        __this_module->d_slp_radio = out->deepRatio;
        __this_module->l_slp_radio = out->lightRatio;
        __this_module->r_slp_radio = out->remRatio;
        __this_module->wake_radio = out->wakeRatio;
        __this_module->slp_start_ts = out->startTS;
        __this_module->slp_end_ts = out->endTS;
        SlpDataVmWrite();

        w_sleep.total_start_ts = out->startTS;
        w_sleep.total_end_ts = out->endTS;

        int8_t idx = -1;
        int8_t last_stage = -1;//上一个状态
        int32_t numEpochs = out->numEpochs;
        for(int32_t i = 0; i < numEpochs; i++)
        {
            if(last_stage != out->stages[i])
            {
                idx += 1;
                if(idx >= Slp_Max_Sec) 
                    break;

                last_stage = out->stages[i];
                w_sleep.ctx[idx].stage = last_stage;
                w_sleep.ctx[idx].start_ts = out->startTS + i*30;
                w_sleep.ctx[idx].end_ts = out->startTS + (i+1)*30;
            }else
            {
                if(idx == -1) continue;

                w_sleep.ctx[idx].end_ts = out->startTS + (i+1)*30;
            }
        }

        //统计的总段数
        u8 sec_num = idx + 1;
        sec_num = sec_num>Slp_Max_Sec?Slp_Max_Sec:sec_num;
        w_sleep.SecNum = sec_num;
        w_sleep.check_code = Nor_Vm_Check_Code;

        int msg[2];
        msg[0] = ui_msg_nor_sleep_write;
        post_ui_msg(msg, 1);
    }

    return;
}

void VmFlashSleepCtxWrite(void)
{
    if(w_sleep.check_code != 0)
        VmSleepCtxFlashSave(&w_sleep);

    memset(&w_sleep, 0, sizeof(vm_sleep_ctx_t));

    return;
}

/* 根据睡眠反馈，打开ppg监测 todo */
void SleepPpgSensorOnOff(s8 onoff)
{
    /* 没有监测到入睡，ppg不做处理 */
    bool fall_asleep = GetFallAsleepFlag();
    if(fall_asleep == false) return;

    /* 入睡阶段，心率由睡眠算法去接管 */
    if(onoff == 1)
    {
        u8 work = GetPpgWorkType();
        if(work == PpgWorkBo)
        {
            if(GetPpgMode() == PpgModeAuto)
                SetAutoBoRestart();
        }else if(work == PpgWorkHr)
            return;

        /* 启动睡眠心率 */
        EnablePpgModule(PpgWorkHr, PpgModeAuto);
    }else
    {
        /* 看当前ppg工作类型 */
        if(GetPpgWorkType() == PpgWorkHr) 
            DisablePpgModule();
    }

    return;
}

/* 根据睡眠反馈，设置状态 */
void SleepStatusOutHandle(u8 status)
{
    if(status & 0x02) 
    {
        /* 监测入睡成功 */
        SetFallAsleepFlag(true);

        printf("fall asleep success\n");
    }else if(status & 0x04)
    {
        SetFallAsleepFlag(false);
        SetSleepAutoDetEnFlag(false);

        /* 如果说清醒的时候，时间在有效时间段内 */
        //todo

        if(status & 0x08) 
        {
            /* 睡眠数据有效，对数据进行解析 */
            GetSlpSummary(&slp_in, &slp_out);
        }

        printf("fall asleep --->wake \n");
    }

    return;
}

void SleepUtcMinProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    u8 hour = ptime->hour;
    u8 minute = ptime->min;
    if(hour >= SlpStartH || hour < SlpEndH)
    {
        /* 20:00 ~ 8:00 */
        bool slp_en = GetSleepEnState();
        if(slp_en == true) return;

        SlpDataVmReset();

        /* 时间点刻印在20点 */
        memset(&w_sleep, 0, sizeof(vm_sleep_ctx_t));
        struct sys_time time;
        memcpy(&time, ptime, sizeof(struct sys_time));
        time.hour = SlpStartH; time.min = 0; time.sec = 0;
        if(hour >= SlpStartH)
            w_sleep.timestamp = UtcTimeToSec(&time);
        else
            w_sleep.timestamp = UtcTimeToSec(&time) - SEC_PER_DAY;

        SetSleepEnState(true);
        SetFallAsleepFlag(false);
        SetSleepAutoDetEnFlag(true);

        printf("---------->sleep start\n");
    }else
    {
        SetSleepEnState(false);
    }

    return;
}

void SlpDataVmRead(void)
{
    int ret = syscfg_read(CFG_SLP_PARA_INFO, \
        __this_module, __this_module_size);
    if(ret != __this_module_size || __this_module->vm_mask != VM_MASK)
        SlpDataVmReset();

    return;
}

void SlpDataVmWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_SLP_PARA_INFO, \
            __this_module, __this_module_size);
        if(ret == __this_module_size)
            break;
    }

    return;
}

void SlpDataVmReset(void)
{
    memcpy(__this_module, &init, __this_module_size);

    __this_module->vm_mask = VM_MASK;

    SlpDataVmWrite();

    return;
}
