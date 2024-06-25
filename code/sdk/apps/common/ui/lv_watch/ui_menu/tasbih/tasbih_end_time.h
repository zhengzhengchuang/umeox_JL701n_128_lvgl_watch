#ifndef __TASBIH_END_TIME_H__
#define __TASBIH_END_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

uint8_t GetTasbih_etime_hour(void);
void SetTasbih_etime_hour(uint8_t hour);

uint8_t GetTasbih_etime_minute(void);
void SetTasbih_etime_minute(uint8_t minute);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
