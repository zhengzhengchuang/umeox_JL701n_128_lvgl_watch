#ifndef __ALARM_TIME_H__
#define __ALARM_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

void SetAlarmTimeTmp(uint8_t hour, \
    uint8_t minute);
void GetAlarmTimeTmp(uint8_t *hour, \
    uint8_t *minute);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
