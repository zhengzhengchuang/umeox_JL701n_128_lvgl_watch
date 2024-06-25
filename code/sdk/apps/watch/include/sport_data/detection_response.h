#ifndef __DETECTION_RESPONSE_H__
#define __DETECTION_RESPONSE_H__
#include "system/includes.h"
void sleep_detecion_response(void);
void sport_hr_detection_response(void);
void wrist_detection_response(void);
void fall_detection_response(void);
void sedentary_detecion_response(void);

int set_emergency_contact_number(u8 *number, u8 len);
int call_emergency_contact_number(void);
int refresh_detection_time(void);
int save_detection_set(u8 type, u8 enable, u8 response_mode, u8 *time);
int get_detection_time(u8 type, u8 *allday_enable, u8 *time);
#endif

