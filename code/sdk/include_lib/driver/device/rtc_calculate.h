#ifndef __RTC_CALCULATE_H__
#define __RTC_CALCULATE_H__

#include "typedef.h"

bool leapyear(u16 year);
u16 month_to_day(u16 year, u8 month);
void day_to_ymd(u16 day, struct sys_time *sys_time);
u16 ymd_to_day(struct sys_time *time);
u8 caculate_weekday_by_time(struct sys_time *r_time);
u16 __month_to_day(u16 year, u8 month);

#endif // __RTC_CALCULATE_H__

