#include "math.h"
#include "../lv_watch.h"
#include "prayer_time_data.h"

#define VM_MASK (0x55bc)

PTCfgPara_t PT_Cfg;
PTInfoPara_t PT_Info;

static const PTCfgPara_t InitCfg = {
    .conv = Conv_Common,
    .juri = Juri_Shafii,
    .hlat = HLat_AngleBased,

    .fajr_angle = 18.0f,
    .isha_value = 17.0f,
    .maghrib_value = 0.0f,
    .isha_is_minutes = false,
    .maghrib_is_minutes = true,

    .remind_en = {true},
    .voice_en = {true},

    .remind_offset = {0},
    .result_offset = {0},
};

const u16 PTimeElapsed[PT_Num] = {
    PTimeElapsedDur, 0, PTimeElapsedDur, \
    PTimeElapsedDur, 0, PTimeElapsedDur, \
    0xffff,
};

#define PI (3.14159f)

static float __latitude;
static float __longitude;
static float __timezone_ex;
static float __julian_date;

static float fix_angle(float a)
{
    a = a - 360.0f * floorf(a / 360.0f);
    a = a < 0.0f ? a + 360.0f : a;

    return a;
}

static float fix_hour(float a)
{
    a = a - 24.0f * floorf(a / 24.0f);
    a = a < 0.0f ? a + 24.0f : a;

    return a;
}

static float deg2rad(float d)
{
    return d * PI / 180.0f;
}

static float rad2deg(float r)
{
    return r * 180.0f / PI;
}

static float dsin(float d)
{
    return sinf(deg2rad(d));
}

static float dcos(float d)
{
    return cosf(deg2rad(d));
}

static float dtan(float d)
{
    return tanf(deg2rad(d));
}

static float darcsin(float x)
{
    return rad2deg(asinf(x));
}

static float darccos(float x)
{
    return rad2deg(acosf(x));
}

static float darctan2(float y, float x)
{
    return rad2deg(atan2f(y, x));
}

static float darccot(float x)
{
    return rad2deg(atanf(1.0f / x));
}

static float time_diff(float time1, \
    float time2)
{
    return fix_hour(time2 - time1);
}

static float NightPortion(float angle)
{
    HLatMethod_t hlat = PT_Cfg.hlat;

    switch(hlat)
    {
        case HLat_AngleBased:
            return (angle / 60.0f);
        case HLat_Midnight:
            return (1.0f / 2.0f);
        case HLat_SevenNight:
            return (1.0f / 7.0f);
        default:
            return 0.0f;
    }
}

static void DayPortion(float info[])
{
    for(u8 i = 0; i < PT_Num; i++)
        info[i] /= 24.0f;

    return;
}

static FloatPair_t SunPosition(float jd)
{
    float eq_t;
    FloatPair_t pair;
    
    float d = jd - 2451545.0f;
    float g = fix_angle(357.529f + 0.98560028f * d);
    float q = fix_angle(280.459f + 0.98564736f * d);
    float l = fix_angle(q + 1.915f * dsin(g) + 0.020f * dsin(2.0f * g));

    float e = 23.439f - 0.00000036f * d;

    float dd = darcsin(dsin(e) * dsin(l));
    float ra = darctan2(dcos(e) * dsin(l), dcos(l)) / 15.0f;
    
    ra = fix_hour(ra);
    eq_t = q / 15.0f - ra;
    
    pair.dd = dd;
    pair.eq_t = eq_t;

    return pair;
}

static float SunDeclination(float jd)
{
    FloatPair_t pair = SunPosition(jd);
    
    return pair.dd;
}

static float EquationOfTime(float jd)
{
    FloatPair_t pair = SunPosition(jd);
    
    return pair.eq_t;
}

static float CalcMidDay(float _t)
{
    float t = EquationOfTime(__julian_date + _t);
    float z = fix_hour(12.0f - t);

    return z;
}

static float CalcPTimeFunc(float g, float t)
{
    float d = SunDeclination(__julian_date + t);
    float z = CalcMidDay(t);

    float v = 0.0f;
    float b = dcos(d) * dcos(__latitude);
    float a = -dsin(g) - dsin(d) * dsin(__latitude);
    float c = a / b;
    c = c > (1.0f)?(1.0f):c;
    c = c < (-1.0f)?(-1.0f):c;
    v = 1.0f / 15.0f * darccos(c);  
    
    return z + (g > 90.0f ? - v : v);
}

static float CalcAsrFunc(int step, float t)  // Shafii: step=1, Hanafi: step=2
{
    float d = SunDeclination(__julian_date + t);
    float g = -darccot(step + dtan(fabsf(__latitude - d)));

    return CalcPTimeFunc(g, t);
}

static void CalcPTimeEx(float info[])
{
    DayPortion(info);

    JuriMethod_t juri = PT_Cfg.juri;

    float isha_value = PT_Cfg.isha_value;
    float fajr_angle = PT_Cfg.fajr_angle;
    float maghrib_value = PT_Cfg.maghrib_value;
    
    info[Fajr] = CalcPTimeFunc(180.0f - fajr_angle, info[Fajr]);
    info[Sunrise] = CalcPTimeFunc(180.0f - 0.833f, info[Sunrise]);
    info[Dhuhr] = CalcMidDay(info[Dhuhr]);
    info[Asr] = CalcAsrFunc(1 + juri, info[Asr]);
    info[Sunset] = CalcPTimeFunc(0.833f, info[Sunset]);
    info[Maghrib] = CalcPTimeFunc(maghrib_value, info[Maghrib]);
    info[Isha] = CalcPTimeFunc(isha_value, info[Isha]);

    return;
}

static void AdjustHlatPTime(float info[])
{   
    float fajr_angle = PT_Cfg.fajr_angle;
    float isha_value = PT_Cfg.isha_value;
    float maghrib_value = PT_Cfg.maghrib_value;
    bool isha_is_minutes = PT_Cfg.isha_is_minutes;
    bool maghrib_is_minutes = PT_Cfg.maghrib_is_minutes;

    float n_time = time_diff(info[Sunset], info[Sunrise]);

    float fajr_diff = NightPortion(fajr_angle) * n_time;

    if(isnan(info[Fajr]) || \
        time_diff(info[Fajr], info[Sunrise]) > fajr_diff)
        info[Fajr] = info[Sunrise] - fajr_diff;

    float isha_angle = isha_is_minutes?18.0f:isha_value;
	float isha_diff = NightPortion(isha_angle) * n_time;

    if(isnan(info[Isha]) || \
        time_diff(info[Sunset], info[Isha]) > isha_diff)
        info[Isha] = info[Sunset] + isha_diff;

    float maghrib_angle = maghrib_is_minutes?4.0f:maghrib_value;
	float maghrib_diff = NightPortion(maghrib_angle) * n_time;

    if(isnan(info[Maghrib]) || \
        time_diff(info[Sunset], info[Maghrib]) > maghrib_diff)
        info[Maghrib] = info[Sunset] + maghrib_diff;

    return;
}

static void AdjustPTime(float info[])
{
    for(u8 i = 0; i < PT_Num; ++i)
		info[i] += __timezone_ex - __longitude / 15.0f;

    info[Dhuhr] += 0.0f / 60.0f;

    float maghrib_value = PT_Cfg.maghrib_value;
    bool maghrib_is_minutes = PT_Cfg.maghrib_is_minutes;
    if(maghrib_is_minutes)
		info[Maghrib] = info[Sunset] + maghrib_value/60.0f;

    float isha_value = PT_Cfg.isha_value;
    bool isha_is_minutes = PT_Cfg.isha_is_minutes;
    if(isha_is_minutes)
        info[Isha] = info[Maghrib] + isha_value/60.0f;

    HLatMethod_t hlat = PT_Cfg.hlat;
    if(hlat != HLat_None)
        AdjustHlatPTime(info);
		
    return;
}

static void CalcPTime(float info[])
{
    float def_info[PT_Num] = \
        {5.0f, 6.0f, 12.0f, 13.0f, 18.0f, 18.0f, 18.0f};

    for(u8 i = 0; i < PT_Num; i++)
		info[i] = def_info[i];

    for(u8 i = 0; i < 1; i++)
		CalcPTimeEx(info);

    AdjustPTime(info);

    return;
}

static float GetJulianDate(u16 year, u8 month, u8 day)
{
    float a = 0;
    float b = 0;
    
    if(month <= 2)
    {
        year -= 1;
        month += 12;
    }

    a = floorf(year/100.0f);
    b = 2.0f - a + floorf(a/4.0f);
  
    return floorf(365.25f * (year + 4716)) + floorf(30.6001f * (month + 1)) + (day*1.0f) + b - 1524.5f;
}

static void GetPTimeEx(u16 utc_year, u8 utc_month, u8 utc_day, \
    float latitude, float longitude, float timezone, float info[])
{
    __latitude = latitude;
    __longitude = longitude;
    __timezone_ex = timezone;

    __julian_date = GetJulianDate(utc_year, utc_month, utc_day) - \
        __longitude / (15.0f * 24.0f);

    CalcPTime(info);

    return;
}

static void GetPTime(float latitude, float longitude, \
	float timezone, float info[])
{
    struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);

    u16 year = utc_time.year;
    u8 month = utc_time.month;
    u8 day = utc_time.day;

    GetPTimeEx(year, month, day, \
        latitude, longitude, timezone, info);

    return;
}

static void PTimeInfoParaCommon(void)
{
    float info[PT_Num] = {0};
    float latitude = ll_info.vm_latitude;
    float longitude = ll_info.vm_longitude;
    int tmp = \
        GetVmParaCacheByLabel(vm_label_time_zone);
    float timezone = (tmp*(1.0f))/(10.0f);

    GetPTime(latitude, longitude, timezone, info);

    u8 PT_hour;
    u8 PT_minute;
    float PT_temp; 
    for(u8 i = 0; i < PT_Num; i++)
    {
        PT_temp = \
            fix_hour(info[i] + (0.5f/60.0f));		// add 0.5 minutes to round
        PT_hour = \
            (u8)floorf(PT_temp);
        PT_minute = \
            (u8)floorf((PT_temp - PT_hour) * 60.0f);

        PT_Info.origin_timestamp[i] = PT_hour*3600 + \
            PT_minute*60;

        s32 retult_sub = \
            PT_Info.origin_timestamp[i] + PT_Cfg.result_offset[i];
        if(retult_sub >= 0)
        {
            PT_Info.result_timestamp[i] = retult_sub;
            PT_Info.result_timestamp[i] %= 86400;
        }else
        {
            PT_Info.result_timestamp[i] = 86400 + retult_sub;       
        }

        s32 remind_sub = \
            PT_Info.result_timestamp[i] + PT_Cfg.remind_offset[i];
        if(remind_sub >= 0)
        {
            PT_Info.remind_timestamp[i] = remind_sub;
            PT_Info.remind_timestamp[i] %= 86400;
        }else
        {
            PT_Info.remind_timestamp[i] = 86400 + remind_sub;       
        }
    }

    return;
}


static const u16 dayfromjan1[13] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365,
};

static const u16 dayfromjan1run[13] = {
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366,
};

static bool IsLeapYear(u16 year)
{
	bool ret = false;

	ret = (bool)((!(year % 4) && (year % 100)) || !(year % 400));

	return ret;
}

static u16 Tm2DateNum(u16 year, u8 month, u8 day)
{
    u16 i = 0;
    u32 no_of_days = 0;

    if ((day > 31) || (month > 12) || (year < 1980) || (month < 1))
        return 0;
    
    if(IsLeapYear(year))
    {
        no_of_days += dayfromjan1run[month - 1] + day - 1;
    }
    else
    {
        no_of_days += dayfromjan1[month - 1] + day - 1;
    }

    return no_of_days;
}

static u16 PTimeDateNum(u16 year, u8 month, u8 day)
{
   u16 num = 0;
	 
    if(month < 3)
    {
        num = Tm2DateNum(year, month, day);
    }else
    {
        if(IsLeapYear(year))
        {
            num = Tm2DateNum(year, month, day);
        }else
        {
            num = Tm2DateNum(year, month, day) + 1;
        }		 	
    }

	 return num;
}

static void PTimeInfoParaSpecial(u8 conv)
{
    struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);

    u16 year = utc_time.year;
    u8 month = utc_time.month;
    u8 day = utc_time.day;
    u16 num = PTimeDateNum(year, month, day);

    if(num >= 366) return;

    PTDataPara_t *PTData = NULL;

    if(conv == Conv_Ingushetia)
        PTData = PTIngushetia;
    else
        PTData = PTKheuBrunei;

    if(PTData == NULL) return;

    PT_Info.origin_timestamp[Fajr] = \
        (PTData[num].PTFajr[0])*3600 + (PTData[num].PTFajr[1])*60;
    PT_Info.origin_timestamp[Sunrise] = \
        (PTData[num].PTSunrise[0])*3600 + (PTData[num].PTSunrise[1])*60;
    PT_Info.origin_timestamp[Dhuhr] = \
        (PTData[num].PTDhuhr[0])*3600 + (PTData[num].PTDhuhr[1])*60;
    PT_Info.origin_timestamp[Asr] = \
        (PTData[num].PTAsr[0])*3600 + (PTData[num].PTAsr[1])*60;
    PT_Info.origin_timestamp[Sunset] = \
        (PTData[num].PTMaghrib[0])*3600 + (PTData[num].PTMaghrib[1])*60;
    PT_Info.origin_timestamp[Maghrib] = \
        (PTData[num].PTMaghrib[0])*3600 + (PTData[num].PTMaghrib[1])*60;
    PT_Info.origin_timestamp[Isha] = \
        (PTData[num].PTIsha[0])*3600 + (PTData[num].PTIsha[1])*60;

    for(u8 i = 0; i < PT_Num; i++)
    {
        s32 retult_sub = \
            PT_Info.origin_timestamp[i] + PT_Cfg.result_offset[i];
        if(retult_sub >= 0)
        {
            PT_Info.result_timestamp[i] = retult_sub;
            PT_Info.result_timestamp[i] %= 86400;
        }else
        {
            PT_Info.result_timestamp[i] = 86400 + retult_sub;       
        }

        s32 remind_sub = \
            PT_Info.result_timestamp[i] + PT_Cfg.remind_offset[i];
        if(remind_sub >= 0)
        {
            PT_Info.remind_timestamp[i] = remind_sub;
            PT_Info.remind_timestamp[i] %= 86400;
        }else
        {
            PT_Info.remind_timestamp[i] = 86400 + remind_sub;       
        }
    }

    return;
}

u8 GetPTimeType(void)
{
    struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);

    u8 hour = utc_time.hour;
    u8 minute = utc_time.min;
    u8 second = utc_time.sec;

    u32 UtcTimestamp = \
        hour*3600 + minute*60 + second;

    u8 i;
    u32 PTTimestamp;

    if(UtcTimestamp >= \
        PT_Info.result_timestamp[Isha])
    {
        i = Isha;

        return i;
    }

    for(i = Fajr; i < PT_Num; i++)
    {
        if(i == Sunset)
            continue;

        PTTimestamp = \
            PT_Info.result_timestamp[i];

        if(UtcTimestamp < \
            (PTTimestamp + PTimeElapsed[i]))
            break;
    }

    return i;
}

u8 GetPTimeCountdownMode(u8 type)
{
    /****
        *0:倒计时 
        *1：正计时 
     ****/
    
    u8 mode = 0;

    if(type >= PT_Num)
        return mode;

    struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);

    u8 hour = utc_time.hour;
    u8 minute = utc_time.min;
    u8 second = utc_time.sec;

    u32 UtcTimestamp = \
        hour*3600 + minute*60 + second;

    if(UtcTimestamp > \
        PT_Info.result_timestamp[type])
        mode = 1;

    return mode;
}

u32 GetPTimeCountdownTime(u8 type)
{
    u32 timestamp = 0;

    if(type >= PT_Num)
        return timestamp;

    struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);

    u8 hour = utc_time.hour;
    u8 minute = utc_time.min;
    u8 second = utc_time.sec; 

    u32 UtcTimestamp = \
        hour*3600 + minute*60 + second;
    s32 timestamp_diff = \
        UtcTimestamp - PT_Info.result_timestamp[type];
    timestamp = LV_ABS(timestamp_diff);

    return timestamp;
}

void PTimeResultOffsetUpdate(u8 type, s8 val)
{
    if(type >= PT_Num)
        return;

    PT_Cfg.result_offset[type] = val;
    PTimeCfgParaUpdate();

    s32 retult_sub = \
        PT_Info.origin_timestamp[type] + val;
    if(retult_sub >= 0)
    {
        PT_Info.result_timestamp[type] = retult_sub;
        PT_Info.result_timestamp[type] %= 86400;
    }else
    {
        PT_Info.result_timestamp[type] = 86400 + retult_sub;       
    }

    return;
}

void PTimeRemindOffsetUpdate(u8 type, s8 val)
{
    if(type >= PT_Num)
        return;

    PT_Cfg.remind_offset[type] = val;
    PTimeCfgParaUpdate();

    s32 remind_sub = \
        PT_Info.result_timestamp[type] + val;
    if(remind_sub >= 0)
    {
        PT_Info.remind_timestamp[type] = remind_sub;
        PT_Info.remind_timestamp[type] %= 86400;
    }else
    {
        PT_Info.remind_timestamp[type] = 86400 + remind_sub;       
    }

    return;
}

static u8 ProcessType = Fajr;
u8 GetPTimeProcessType(void)
{
    return ProcessType;
}

void SetPTimeProcessType(u8 Type)
{
    ProcessType = Type;

    return;
}

static void PTimeIsOnHandle(uint8_t type)
{
    SetPTimeProcessType(type);

    //总提醒开关   语音提醒开关TODO USER 
    bool remind_en = \
        PT_Cfg.remind_en[type];
    if(remind_en == false)
        return;

    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    //震动
    motor_run(1, def_motor_duty);

    //播放
    bool voice_en = \
        PT_Cfg.voice_en[type];
    if(voice_en == true)
    {
        //...
    }

    ui_menu_jump(\
        ui_act_id_prayer_time_popup);

    return;
}

void PTimeProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    u8 hour = ptime->hour;
    u8 minute = ptime->min;
    u8 second = ptime->sec;

    u32 UtcTimestamp = \
        hour*3600 + minute*60 + second;

    u8 i;
    for(i = Fajr; i < PT_Num; i++)
    {
        if(i == Sunrise || i == Sunset)
            continue;

        if(UtcTimestamp == \
            PT_Info.remind_timestamp[i])
            break;
    }

    if(i < PT_Num)
        PTimeIsOnHandle(i);

    return;
}

void PTimeCfgParaRead(void)
{
    int vm_op_len = \
        sizeof(PTCfgPara_t);

    int ret = syscfg_read(CFG_PT_CFG_PARA_INFO, \
        &PT_Cfg, vm_op_len);
    if(ret != vm_op_len || PT_Cfg.vm_mask != VM_MASK)
        PTimeCfgParaReset();
   
    u8 conv = PT_Cfg.conv;

    if(conv == Conv_Common)
        PTimeInfoParaCommon();
    else
        PTimeInfoParaSpecial(conv);
    
    return;
}

void PTimeCfgParaWrite(void)
{
    int vm_op_len = \
        sizeof(PTCfgPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_PT_CFG_PARA_INFO, \
            &PT_Cfg, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void PTimeCfgParaReset(void)
{
    int vm_op_len = sizeof(PTCfgPara_t);

    memcpy(&PT_Cfg, &InitCfg, vm_op_len);

    PT_Cfg.vm_mask = VM_MASK;

    PTimeCfgParaUpdate();

    return;
}

void PTimeCfgParaUpdate(void)
{
    PTimeCfgParaWrite();

    return;
}

void PTimeInfoParaUpdate(void)
{
    u8 conv = PT_Cfg.conv;

    if(conv == Conv_Common)
        PTimeInfoParaCommon();
    else
        PTimeInfoParaSpecial(conv);

    return;
}