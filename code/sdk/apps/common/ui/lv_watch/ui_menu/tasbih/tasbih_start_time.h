#ifndef __TASBIH_START_TIME_H__
#define __TASBIH_START_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

uint8_t GetTasbih_stime_hour(void);
void SetTasbih_stime_hour(uint8_t hour);

uint8_t GetTasbih_stime_minute(void);
void SetTasbih_stime_minute(uint8_t minute);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
