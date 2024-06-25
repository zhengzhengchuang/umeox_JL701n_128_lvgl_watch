#include "../lv_watch.h"

SleepInfoPara_t Slp_Info;

//公历闰年
static const uint8_t calendarLDays[12]= {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};
//公历非闰年
static const uint8_t calendarNLDays[12]= {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};

static u8 IsLeapYear(u32 year)
{
	u8 ret_val = 0;

	ret_val = (u8)((!(year % 4) && (year % 100)) || !(year % 400));

	return ret_val;
}

//false 没过期  true 过期
static bool SleepVmDataIsPast(struct sys_time *slp_time)
{
    if(!slp_time) 
        return true;

    //今天
    struct sys_time utc_time1;
    GetUtcTime(&utc_time1);

    struct sys_time *utc_time2 = \
        slp_time;
    
    u16 DysNum1 = 0;
    u16 DysNum2 = 0;
    if(utc_time1.year == utc_time2->year)
    {
        u8 LY = \
            IsLeapYear(utc_time1.year);

        for(u8 i = 1; i < utc_time1.month; i++)
        {
            if(LY)
                DysNum1 += calendarLDays[i - 1];
            else
                DysNum1 += calendarNLDays[i - 1];
        } 
        DysNum1 += utc_time1.day;

        for(u8 i = 1; i < utc_time2->month; i++)
        {
            if(LY)
                DysNum2 += calendarLDays[i - 1];
            else
                DysNum2 += calendarNLDays[i - 1];
        } 
        DysNum2 += utc_time2->day;

        s16 Dyf = DysNum1 - DysNum2;

        printf("Dyf = %d\n", Dyf);

        if(LV_ABS(Dyf) > 1)
            return true;

        if(utc_time1.hour >= SlpStartH)
            return true;
    }else
    {
        if(utc_time2->month == 12 && \
            utc_time2->day == 31)
        {
            if(utc_time1.month == 1 && \
                utc_time1.day == 1)
            {
                if(utc_time1.hour >= SlpStartH)
                    return true;
            }else
            {
                return true;
            }
                 
        }else
        {
            return true;
        }
            
    }

    return false;
}

static void ClearSleepInfoPara(void)
{
    memset(&Slp_Info, 0, \
        sizeof(SleepInfoPara_t));

    return;
}

static bool GetSleepData(void)
{
    bool ret = false;

    u8 sleep_num = \
        VmSleepItemNum();
    if(sleep_num == 0)
        goto __end;

    u8 idx = \
       sleep_num - 1; 
    bool r_slp_ret = \
        VmSleepCtxByIdx(idx);
    if(r_slp_ret == false)
        goto __end;

    if(r_sleep.CurSecNum == 0)
        goto __end;

    ret = true;

__end:
    return ret;
}

void SetSleepInfoPara(void)
{
    ClearSleepInfoPara();

    bool data_ret = \
        GetSleepData();
    if(data_ret == false)
        return;

    bool IsPast = \
        SleepVmDataIsPast(&(r_sleep.time));
    if(IsPast == true)
        return;

    memcpy(&(Slp_Info.time), &(r_sleep.time), \
        sizeof(struct sys_time));

    u8 sec_s = 0;
    Slp_Info.slp_timestamp = \
        (r_sleep.vm_ctx[sec_s].ss_time.hour)*60 + \
            (r_sleep.vm_ctx[sec_s].ss_time.min);

    u8 sec_e = \
        r_sleep.CurSecNum - 1;
    Slp_Info.wkp_timestamp = \
        (r_sleep.vm_ctx[sec_e].se_time.hour)*60 + \
            (r_sleep.vm_ctx[sec_e].se_time.min);

    u16 *d_slp_total = \
        &(Slp_Info.d_slp_total);
    u16 *l_slp_total = \
        &(Slp_Info.l_slp_total);
    u16 *r_slp_total = \
        &(Slp_Info.r_slp_total);
    u16 *wakeup_total = \
        &(Slp_Info.wakeup_total);

    *d_slp_total = 0;
    *l_slp_total = 0;
    *r_slp_total = 0;
    *wakeup_total = 0;
    
    u16 slp_dur;
    sleep_state_t s_state;
    for(u8 i = 0; i < r_sleep.CurSecNum; i++)
    {
        u8 ss_hour = \
            r_sleep.vm_ctx[i].ss_time.hour;
        u8 ss_minute = \
            r_sleep.vm_ctx[i].ss_time.min;
        u8 se_hour = \
            r_sleep.vm_ctx[i].se_time.hour;
        u8 se_minute = \
            r_sleep.vm_ctx[i].se_time.min;

        if(se_hour < ss_hour)
            slp_dur = (se_hour*60 + 1440 + se_minute) - \
                (ss_hour*60 + ss_minute);
        else
            slp_dur = (se_hour*60 + se_minute) - \
                (ss_hour*60 + ss_minute);

        s_state = \
            r_sleep.vm_ctx[i].state;
        if(s_state == sl_wake_up)
            *wakeup_total += slp_dur;
        else if(s_state == sl_rem_sleep)
            *r_slp_total += slp_dur;
        else if(s_state == sl_light_sleep)
            *l_slp_total += slp_dur;
        else if(s_state == sl_deep_sleep)
            *d_slp_total += slp_dur;
    }

    //总的睡眠时间 = 浅睡 + 深睡 + 快速眼动
    u16 *slp_total = \
        &(Slp_Info.slp_total);
    *slp_total = *l_slp_total + *d_slp_total + *r_slp_total;

    bool *valid = \
        &(Slp_Info.valid);
    *valid = true;

    return;
}

void SleepProcess(struct sys_time *utc_time)
{
    if(!utc_time) return;

#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    if(utc_time->hour == SlpStartH && \
        utc_time->min == 0)
    {
        /*每天固定18:00，启动睡眠算法，清除数据*/

        ClearSleepInfoPara();

        memset(&w_sleep, 0, \
            sizeof(vm_sleep_ctx_t));
        w_sleep.check_code = \
            Nor_Vm_Check_Code;
        w_sleep.CurSecNum = \
            0;
        GetUtcTime(&(w_sleep.time));
        

    }

    return;
}
