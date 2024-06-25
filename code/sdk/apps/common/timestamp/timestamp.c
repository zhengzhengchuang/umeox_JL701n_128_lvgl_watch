#include "timestamp.h"


#define UTC_BASE_YEAR 		1970
#define MONTH_PER_YEAR 		12
#define DAY_PER_YEAR 		365
#define SEC_PER_DAY 		86400
#define SEC_PER_HOUR 		3600
#define SEC_PER_MIN 		60

/* Number of days per month */
static const u8  days_per_month[MONTH_PER_YEAR] = {
    /*
    1   2   3   4   5   6   7   8   9   10  11  12
    */
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static bool get_is_leap_year(u16 year)
{
    if (((year % 4) == 0) && ((year % 100) != 0)) {
        return 1;
    } else if ((year % 400) == 0) {
        return 1;
    }

    return 0;
}

static u8 get_day_of_mon(u8 month, u16 year)
{
    if ((month == 0) || (month > 12)) {
        return days_per_month[1] + get_is_leap_year(year);
    }

    if (month != 2) {
        return days_per_month[month - 1];
    } else {
        return days_per_month[1] + get_is_leap_year(year);
    }
}

void timestamp_utc_sec_2_mytime(u32 utc_sec, struct sys_time *mytime)
{
    int32_t sec, day;
    u16	y;
    u8 m;
    u16	d;

    sec = utc_sec % SEC_PER_DAY;
    mytime->hour = sec / SEC_PER_HOUR;

    sec %= SEC_PER_HOUR;
    mytime->min = sec / SEC_PER_MIN;

    mytime->sec = sec % SEC_PER_MIN;

    day = utc_sec / SEC_PER_DAY;
    for (y = UTC_BASE_YEAR; day > 0; y++) {
        d = (DAY_PER_YEAR + get_is_leap_year(y));
        if (day >= d) {
            day -= d;
        } else {
            break;
        }
    }

    mytime->year = y;

    for (m = 1; m < MONTH_PER_YEAR; m++) {
        d = get_day_of_mon(m, y);
        if (day >= d) {
            day -= d;
        } else {
            break;
        }
    }

    mytime->month = m;
    mytime->day = (u8)(day + 1);
}

u32 timestamp_mytime_2_utc_sec(struct sys_time *mytime)
{
    u16	i;
    u32 no_of_days = 0;
    u32 utc_time;
    u8  dst = 1;

    if (mytime->year < UTC_BASE_YEAR) {
        return 0;
    }

    for (i = UTC_BASE_YEAR; i < mytime->year; i++) {
        no_of_days += (DAY_PER_YEAR + get_is_leap_year(i));
    }

    for (i = 1; i < mytime->month; i++) {
        no_of_days += get_day_of_mon(i, mytime->year);
    }

    no_of_days += (mytime->day - 1);

    utc_time = (unsigned int) no_of_days * SEC_PER_DAY + (unsigned int)(mytime->hour * SEC_PER_HOUR +
               mytime->min * SEC_PER_MIN + mytime->sec);

    return utc_time;
}



