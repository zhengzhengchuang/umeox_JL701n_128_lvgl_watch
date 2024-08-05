#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include "system/includes.h"
#include "system/sys_time.h"

#define UTC_BASE_YEAR 		1970
#define MONTH_PER_YEAR 		12
#define DAY_PER_YEAR 		365
#define SEC_PER_DAY 		86400
#define SEC_PER_HOUR 		3600
#define SEC_PER_MIN 		60

void timestamp_utc_sec_2_mytime(u32 utc_sec, struct sys_time *mytime);
u32 timestamp_mytime_2_utc_sec(struct sys_time *mytime);

#endif



