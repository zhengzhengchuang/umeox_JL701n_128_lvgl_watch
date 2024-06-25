#ifndef __DEMO_H__
#define __DEMO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

enum
{
    Op_Alarm_Add,
    Op_Alarm_Modify,
};

uint8_t GetAlarmOpMode(void);
void SetAlarmOpMode(uint8_t mode);

uint8_t GetAlarmEditId(void);
void SetAlarmEditId(uint8_t id);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
