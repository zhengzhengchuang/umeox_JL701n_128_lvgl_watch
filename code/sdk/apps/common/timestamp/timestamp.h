#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

#include "system/includes.h"
#include "system/sys_time.h"

void timestamp_utc_sec_2_mytime(u32 utc_sec, struct sys_time *mytime);
u32 timestamp_mytime_2_utc_sec(struct sys_time *mytime);

#endif



