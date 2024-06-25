#ifndef __TASBIH_INV_TIME_H__
#define __TASBIH_INV_TIME_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

uint8_t GetTasbih_inv_time_hour(void);
void SetTasbih_inv_time_hour(uint8_t hour);

uint8_t GetTasbih_inv_time_minute(void);
void SetTasbih_inv_time_minute(uint8_t minute);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
