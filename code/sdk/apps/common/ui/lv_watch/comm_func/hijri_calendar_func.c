#include "math.h"
#include "../lv_watch.h"

#define VM_MASK (0x55dc)

#define MONTH_START 1
#define MMICOM_SEC_PER_DAY 86400
#define MMICOM_SEC_PER_HOUR 3600
#define MMICOM_SEC_PER_MIN 60
#define MMICOM_DAY_PER_YEAR 365
#define MMICOM_DATE_MIN_YEAR 1980

#define Hours_Per_Day 24
#define Minutes_Per_Hour 60
#define Seconds_Per_Minute 60

#define Seconds_Per_Hour \
    (Seconds_Per_Minute*Minutes_Per_Hour)
#define Seconds_Per_Day \
    (Seconds_Per_Hour*Hours_Per_Day)
#define Minutes_Per_Day \
    (Minutes_Per_Hour*Hours_Per_Day)

#define Months_Num 13
#define Calendar_Epoch 1948086L

static const u16 dayfromjan1[13] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365,
};

static const u16 dayfromjan1run[13] = {
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366,
};

//回历闰年
static const uint8_t HcalendarLDays[12]= {
    30, 29, 30, 29, 30, 29, 30, 29, 30, 29, 30, 30,
};
//回历非闰年
static const uint8_t HcalendarNLDays[12]= {
    30, 29, 30, 29, 30, 29, 30, 29, 30, 29, 30, 29,
};

//公历闰年
static const uint8_t GcalendarLDays[12]= {
    31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};
//公历非闰年
static const uint8_t GcalendarNLDays[12]= {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
};

static const HcalendarFesInfo_t HFesInfoTmp[\
    HcalendarFesNum] = {
    [Hijri_New_Year] = \
        {.HFes_year = 1445, .HFes_month = 1,  .HFes_day = 1},
    [Ashuraa] = \
        {.HFes_year = 1445, .HFes_month = 1,  .HFes_day = 10},
    [Prophets_birthday] = \
        {.HFes_year = 1445, .HFes_month = 3,  .HFes_day = 12},
    [The_Night_of_IM] = \
        {.HFes_year = 1445, .HFes_month = 7,  .HFes_day = 27},
    [Mid_of_Shaban] = \
        {.HFes_year = 1445, .HFes_month = 8,  .HFes_day = 15},
    [Ramadan] = \
        {.HFes_year = 1445, .HFes_month = 9,  .HFes_day = 1},
    [Laylat_al_Qadr] = \
        {.HFes_year = 1445, .HFes_month = 9,  .HFes_day = 27},
    [Eid_ul_Fitr] = \
        {.HFes_year = 1445, .HFes_month = 10, .HFes_day = 1},
    [Tarwiyah_day] = \
        {.HFes_year = 1445, .HFes_month = 12, .HFes_day = 8},
    [Arafa_day] = \
        {.HFes_year = 1445, .HFes_month = 12, .HFes_day = 9},
    [Eid_ul_Adha] = \
        {.HFes_year = 1445, .HFes_month = 12, .HFes_day = 10},
    [Days_of_Tashreeq] = \
        {.HFes_year = 1445, .HFes_month = 12, .HFes_day = 11}, //11 12 13
};

HcalendarInfoPara_t HcalendarInfo;

static u8 GIsLeapYear(u32 year)
{
	u8 ret_val = 0;

	ret_val = (u8)((!(year % 4) && (year % 100)) || !(year % 400));

	return ret_val;
}

static u8 HIsLeapYear(u32 year)
{
	u8 ret_val = 0;

	u8 a = year%30;
    if(a == 2 || a == 5 || a == 7 || \
        a == 10 || a == 13 || a == 16 ||\
            a == 18 || a == 21 || a == 24 ||\
                a == 26 || a == 29)
        ret_val = 1;

	return ret_val;
}

static u8 GetMonthLen(u8 lpyr, u8 mon)
{
	u8 days = 31;

    mon--;

	if(mon == 1)
		days = (28 + lpyr);
	else
	{
		if(mon > 6)
            mon--;

		if(mon & 1)
            days = 30;
	}
	
	return days;
}

static u64 HcalendarTm2Second(struct sys_time *utc_time)
{
    u16 i = 0;
    u64 time_sec = 0;
    u32 no_of_days = 0;
    
    if(!utc_time) return 0;

    u16 tm_year = \
        utc_time->year + 10;
    u8 tm_mon = \
        utc_time->month;
    u8 tm_mday = \
        utc_time->day;
    u8 tm_hour = \
        utc_time->hour;
    u8 tm_min = \
        utc_time->min;
    u8 tm_sec = \
        utc_time->sec;

    if((tm_mday > 31) || (tm_mon > 12) || (tm_year < MMICOM_DATE_MIN_YEAR) || \
        (tm_mon < MONTH_START))
        return 0;
    
    for(i = MMICOM_DATE_MIN_YEAR; i < tm_year; i++)
        no_of_days += \
            (MMICOM_DAY_PER_YEAR + GIsLeapYear(i));

    if(GIsLeapYear(tm_year))
        no_of_days += \
            dayfromjan1run[tm_mon-MONTH_START] + tm_mday - 1;
    else
        no_of_days += \
            dayfromjan1[tm_mon-MONTH_START] + tm_mday - 1;

    time_sec = \
         no_of_days*MMICOM_SEC_PER_DAY + tm_hour*MMICOM_SEC_PER_HOUR + \
            tm_min*MMICOM_SEC_PER_MIN + tm_sec;

    return time_sec;
}

static int GetCurJdEx2(void)
{
    int ljd = 0;
    u64 second = 0;
    float t2k = 0.0f;
    float custom_s = 1631176282.0f;
    const float j2000 = 2451545.0f;
    const float jan_1970 = 2440587.5f - j2000;
	
    struct sys_time \
        utc_time;
    GetUtcTime(&utc_time);
    memcpy(&HcalendarInfo.Hutc_time, \
        &utc_time, sizeof(struct sys_time));

    printf("utc time:%04d-%d-%d\n", \
        utc_time.year, utc_time.month, utc_time.day);

    u8 LeapYear = \
        GIsLeapYear(utc_time.year);
    u8 month_len = \
        GetMonthLen(LeapYear, utc_time.month);
    s8 HdayTmp = \
        utc_time.day + HcalendarInfo.Hcalendar_modify;

    if(HdayTmp >= 1 && HdayTmp <= month_len)
        utc_time.day = HdayTmp;
    else
    {
        if(HdayTmp < 1)
        {
            if(utc_time.month > 1)
            {
                utc_time.month -= 1;
                month_len = \
                    GetMonthLen(LeapYear, utc_time.month);
                utc_time.day = month_len + HdayTmp;
            }else
            {
                month_len = 31;

                utc_time.year -= 1;
                utc_time.month = 12;   
                utc_time.day = \
                    month_len + HdayTmp;
            }  
        }else
        {
            if(utc_time.month < 12)
            {
                utc_time.day = \
                    HdayTmp % month_len;
                utc_time.month += 1;
            }else
            {
                month_len = 31;

                utc_time.year += 1;
                utc_time.month = 1;
                utc_time.day = \
                    HdayTmp % month_len;
            }
        }
    }
    
    second = \
        HcalendarTm2Second(&utc_time);
			
    t2k = \
        jan_1970 + second/(float)Seconds_Per_Day;

    ljd = (int)floorf(t2k + 0.5f + j2000);	

    return ljd;	
}

static int Hmod(int x, int y)
{
	int r;

	r = (int)(x / y);

	if(r < 0)
        r--;

	return (int)(x - y*r);
}

static int ApproxYear(int jd)
{
    int year;
    int n1 = 0, n2 = 0;
    int calendar_epoch, day_in_cycle;

    calendar_epoch = Calendar_Epoch - 1;
    n1 = 30;             /* 30 Islamic years = 10631 days,  exactly */
    n2 = 10631;

    jd -= calendar_epoch;
    day_in_cycle = Hmod(jd, n2);
    year = n1 * ((jd - day_in_cycle) / n2);
    year += day_in_cycle * n1 / n2;
        
    return year;	 
}

static void GetIslamicYearData(int Hyear, int *Hdays, char *Hmonth_data)
{
    int rval;

    int thirty_islamic_years = 10631;
    int year_within_cycle = Hmod(Hyear, 30);
    int thirty_year_cycles = (Hyear - year_within_cycle)/30;

    int tval = year_within_cycle*11 + 3;
	
    rval = Calendar_Epoch + thirty_year_cycles*thirty_islamic_years + \
            year_within_cycle*354;

    /* The Islamic calendar alternates between 30-day and 29-day */
    /* months for the first eleven months;  the twelfth is 30    */
    /* Hdays in a leap year,  29 otherwise (see above).           */

    Hmonth_data[12] = 0;
    Hmonth_data[11] = (char)(29 + ((tval % 30) > 18));
    rval += tval / 30;
    *Hdays = rval;
            
    for(u8 i = 0; i < 11; i++)
      Hmonth_data[i] = (char)(30 - (i % 2));

    return;
}

static int GetCalendarData(int Hyear, int *Hdays, char *Hmonth_data)
{
    int rval = 0;
   
    memset(Hmonth_data, 0, Months_Num);

    GetIslamicYearData(Hyear, Hdays, Hmonth_data);
        
    /* New Years Day of the following year.  If you have */
    /* Hdays[0] <= JD < Hdays[1],  JD is in the current year.*/
    if(!rval)   
    {     
        Hdays[1] = Hdays[0];
        for(u8 i = 0; i < Months_Num; i++)
            Hdays[1] += Hmonth_data[i];
    }
	 
	return rval; 
}


static void SetGcalendarFestivals(void)
{
    u16 Hcalendar_year = \
        HcalendarInfo.Hcalendar_year;
    u8 Hcalendar_month = \
        HcalendarInfo.Hcalendar_month;
    u8 Hcalendar_day = \
        HcalendarInfo.Hcalendar_day;
    

    struct sys_time *Hutc_time = \
        &HcalendarInfo.Hutc_time;
    HcalendarFesInfo_t *HFesInfo = \
        HcalendarInfo.HFesInfo;
    GcalendarFesInfo_t *GFesInfoS = \
        HcalendarInfo.GFesInfoS;
    GcalendarFesInfo_t *GFesInfoE = \
        HcalendarInfo.GFesInfoE;

    u8 HLY = \
        HIsLeapYear(Hcalendar_year);
    printf("%s %d\n", __func__, HLY);

    u16 ds0 = 0;
    for(u8 j = 1; j < Hcalendar_month; j++)
    {
        if(HLY)
            ds0 += HcalendarLDays[j - 1];
        else
            ds0 += HcalendarNLDays[j - 1];
    } 
    ds0 += Hcalendar_day;

    u8 LY = GIsLeapYear(Hutc_time->year);

    u16 ds2 = 0;
    for(u8 j = 1; j < Hutc_time->month; j++)
    {
        if(LY)
            ds2 += GcalendarLDays[j - 1];
        else
            ds2 += GcalendarNLDays[j - 1];
    } 
    ds2 += (Hutc_time->day);
 
    s16 df;
    u16 ds1;
    u16 ds3;
    u16 Yds;
    u16 Yrds;
    u16 GFesY;
    u8  GFesM;
    u8  GFesD;
    for(u8 i = Hijri_New_Year; i < HcalendarFesNum; i++)
    {
        ds1 = 0;
        for(u8 j = 1; j < HFesInfo[i].HFes_month; j++)
        {
            if(HLY)
                ds1 += HcalendarLDays[j - 1];
            else
                ds1 += HcalendarNLDays[j - 1];
        }
        ds1 += HFesInfo[i].HFes_day;

        df = ds1 - ds0;
        df = ds2 + df;
        if(df <= 0)
        {
            GFesY = \
                Hutc_time->year - 1;
            LY = GIsLeapYear(GFesY);
            Yds = 365 + LY;
            Yrds = Yds + df - 1;
   
            u8 m;
            ds3 = 0;
            for(m = 1; m <= 12; m++)
            {
                if(LY)
                    ds3 += GcalendarLDays[m - 1];
                else
                    ds3 += GcalendarNLDays[m - 1];
                if(ds3 > Yrds)
                    break;
            }
    
            GFesM = m;

            if(LY)
                GFesD = GcalendarLDays[m - 1] - \
                    (ds3 - Yrds - 1);
            else
                GFesD = GcalendarNLDays[m - 1] - \
                    (ds3 - Yrds - 1);
        }else
        {
            GFesY = \
                Hutc_time->year;
            LY = GIsLeapYear(GFesY);
            GFesY += df/(365 + LY);
            Yrds = df%(365 + LY);

            if(GFesY != Hutc_time->year)
                LY = GIsLeapYear(GFesY);

            u8 m;
            ds3 = 0;
            for(m = 1; m <= 12; m++)
            {
                if(LY)
                    ds3 += GcalendarLDays[m - 1];
                else
                    ds3 += GcalendarNLDays[m - 1];
                if(ds3 >= Yrds)
                    break;
            }

            GFesM = m;
        
            if(LY)
                GFesD = GcalendarLDays[m - 1] - \
                    (ds3 - Yrds);
            else
                GFesD = GcalendarNLDays[m - 1] - \
                    (ds3 - Yrds);
        }

        GFesInfoS[i].GFes_year = \
            GFesY;
        GFesInfoS[i].GFes_month = \
            GFesM;
        GFesInfoS[i].GFes_day = \
            GFesD;

        u8  f_num;
        if(i == Ramadan)
            f_num = 30 - 1;
        else if(i == Days_of_Tashreeq)
            f_num = 3 - 1;
        else
            f_num = 1 - 1;
        
        u8 ed = \
            GFesD + f_num;
        u8 em = \
            GFesM;
        u16 ey = \
            GFesY;
        u8 mds;
        if(LY)
            mds = GcalendarLDays[GFesM - 1];
        else
            mds = GcalendarNLDays[GFesM - 1];
        if(ed > mds)
        {
            if(!LY && (f_num == 30 - 1) && \
                GFesM == 1 && GFesD == 31)//平年 1月31 持续30天的Ramadan 强行写到3月1号
            {
                em = 3;
                ed = 1;
            }else
            {
                ed = ed%mds;
                if(em < 12)
                    em += 1;
                else
                {
                    ey += 1;
                    em = 1;
                }
            }
        }

        GFesInfoE[i].GFes_year = \
            ey;
        GFesInfoE[i].GFes_month = \
            em;
        GFesInfoE[i].GFes_day = \
            ed;
    }

    return;
}

static void HcalendarInfoParaCalc(void)
{
    int jd;
    int curr_jd;

    int Hday = 0;
    int Hyear = 0;
    int Hmonth = 0;
    int Hyear_ends[2];
    char Hmonth_data[Months_Num];

    jd = GetCurJdEx2();

    Hyear = ApproxYear(jd);
    Hday -= 1;

    do{
        if(GetCalendarData(Hyear, Hyear_ends, Hmonth_data))
            return;

        if(Hyear_ends[0] > jd)
            (Hyear)--;
        if( Hyear_ends[1] <= jd)
            (Hyear)++;
    }while(Hyear_ends[0] > jd || Hyear_ends[1] <= jd);

    Hmonth = -1;
    curr_jd = Hyear_ends[0];
   
    for(u8 i = 0; i < Months_Num; i++)
    {
        Hday = (int)(jd - curr_jd);

        if(Hday < (int)Hmonth_data[i])
        {
            Hmonth = i + 1;
            (Hday)++;
            break;
        }

        curr_jd += (int)Hmonth_data[i];
    }

    HcalendarInfo.Hcalendar_year = \
        Hyear;
    HcalendarInfo.Hcalendar_month = \
        Hmonth;
    HcalendarInfo.Hcalendar_day = \
        Hday;

    memcpy(HcalendarInfo.HFesInfo, HFesInfoTmp, \
        sizeof(HcalendarFesInfo_t)*HcalendarFesNum);
    for(u8 i = Hijri_New_Year; i < HcalendarFesNum; i++)
        HcalendarInfo.HFesInfo[i].HFes_year = Hyear;

    SetGcalendarFestivals();

    printf("%s:%04d-%d-%d\n", \
        __func__, Hyear, Hmonth, Hday);
    
    return;
}

void HcalendarInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(HcalendarInfoPara_t);

    int ret = syscfg_read(CFG_HCALENDAR_PARA_INFO, \
        &HcalendarInfo, vm_op_len);
    if(ret != vm_op_len || HcalendarInfo.vm_mask != VM_MASK)
    {
        HcalendarInfo.vm_mask = VM_MASK;

        HcalendarInfoParaUpdate();
    }else
    {
        struct sys_time utc_time;
        GetUtcTime(&utc_time);

        u16 Hyear = \
            HcalendarInfo.Hutc_time.year;
        u8 Hmonth = \
            HcalendarInfo.Hutc_time.month;
        u8 Hday = \
            HcalendarInfo.Hutc_time.day;

        if(utc_time.year != Hyear || utc_time.month != Hmonth || \
            utc_time.day != Hday)
        {
            HcalendarInfoParaUpdate();
        } 
    }

    return;
}

void HcalendarInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(HcalendarInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_HCALENDAR_PARA_INFO, \
            &HcalendarInfo, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void HcalendarInfoParaReset(void)
{
    HcalendarInfo.vm_mask = 0;

    HcalendarInfoParaWrite();
    
    return;
}

void HcalendarInfoParaUpdate(void)
{
    HcalendarInfoParaCalc();

    HcalendarInfoParaWrite();

    return;
}

static void HcalendarFesIsOnHandle(uint8_t type)
{
    SetHcalendarFesType(type);

    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    //震动
    motor_run(1, def_motor_duty);

    ui_menu_jump(ui_act_id_Hcalendar_remind);

    return;
}

static bool IsPrayerTimeSunset(struct sys_time *utc_time)
{
    bool IsSunset = false;

    if(!utc_time) 
        return IsSunset;

    u32 PTimeTimestamp = \
        PT_Info.result_timestamp[Sunset];

    u32 UtcTimestamp = \
        utc_time->hour*3600 + utc_time->min*60 + \
            utc_time->sec;

    if(PTimeTimestamp == UtcTimestamp)
        IsSunset = true;

    return IsSunset;
}

static bool IsPrayerTimeSunrise(struct sys_time *utc_time)
{
    bool IsSunrise = false;

    if(!utc_time) 
        return IsSunrise;

    u32 PTimeTimestamp = \
        PT_Info.result_timestamp[Sunrise];
    u32 UtcTimestamp = \
        utc_time->hour*3600 + utc_time->min*60 + \
            utc_time->sec;

    if(PTimeTimestamp == UtcTimestamp)
        IsSunrise = true;

    return IsSunrise;
}

static u8 FesType;
u8 GetHcalendarFesType(void)
{
    return FesType;
}

void SetHcalendarFesType(u8 type)
{
    FesType = type;

    return;
}

typedef struct
{
    u8 Hmonth;
    u8 Hday;
    u8 PTtype;
}HcaleFesSInfo_t;

static const HcaleFesSInfo_t HFSInfo[\
    HcalendarFesNum] = {
    {.Hmonth = 12, .Hday = 29, .PTtype = Sunset},
    {.Hmonth = 1,  .Hday = 9,  .PTtype = Sunset},
    {.Hmonth = 3,  .Hday = 11, .PTtype = Sunset},
    {.Hmonth = 7,  .Hday = 26, .PTtype = Sunset},
    {.Hmonth = 8,  .Hday = 14, .PTtype = Sunset},
    {.Hmonth = 8,  .Hday = 29, .PTtype = Sunset},
    {.Hmonth = 9,  .Hday = 26, .PTtype = Sunset},
    {.Hmonth = 10, .Hday = 1,  .PTtype = Sunrise},
    {.Hmonth = 12, .Hday = 8,  .PTtype = Sunrise},
    {.Hmonth = 12, .Hday = 9,  .PTtype = Sunrise},
    {.Hmonth = 12, .Hday = 10, .PTtype = Sunrise},
    {.Hmonth = 12, .Hday = 11, .PTtype = Sunrise},

};

void HcalendarProcess(struct sys_time *utc_time)
{
    if(!utc_time) return;

#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    bool FesRemindersFlag = \
        HcalendarInfo.FesRemindersFlag;
    if(!FesRemindersFlag)
        return;

    bool HcaleFesIsOn = false;

    bool IsSunset = \
        IsPrayerTimeSunset(utc_time);
    bool IsSunrise = \
        IsPrayerTimeSunrise(utc_time);

    u16 Hyear = \
        HcalendarInfo.Hcalendar_year;
    u8  Hmonth = \
        HcalendarInfo.Hcalendar_month;
    u8  Hday = \
        HcalendarInfo.Hcalendar_day;
    
    u8 HLY = \
        HIsLeapYear(Hyear);

    u8 i;
    for(i = Hijri_New_Year; i < HcalendarFesNum; i++)
    {
        if(i == Hijri_New_Year)
        {
            if(Hmonth == HFSInfo[i].Hmonth && \
                Hday == HFSInfo[i].Hday + HLY)
                break;
        }else
        {
            if(Hmonth == HFSInfo[i].Hmonth && \
                Hday == HFSInfo[i].Hday)
                break;
        }
    }

    if(i < HcalendarFesNum)
    {
        if(HFSInfo[i].PTtype == Sunset)
        {
            if(IsSunset)
               HcaleFesIsOn = true; 
        }else if(HFSInfo[i].PTtype == Sunrise)
        {
            if(IsSunrise)
               HcaleFesIsOn = true;
        }
    }
     
    if(HcaleFesIsOn)
        HcalendarFesIsOnHandle(i);

    return;
}
