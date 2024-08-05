#ifndef __ALARM_FUNC_H__
#define __ALARM_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define No_Alarm_Info (0xffffffff)

/*********************************************************************************
                                  闹钟最大个数                                     
*********************************************************************************/
#define Alarm_Max_Num (5)

/*********************************************************************************
                                  闹钟位域                                     
*********************************************************************************/
typedef struct 
{
    uint32_t alarm_id       :4;
	
	uint32_t alarm_hour     :5;
	uint32_t alarm_minute   :6;
	
	uint32_t alarm_enable   :1;
	
	uint32_t alarm_repeat   :7;

	uint32_t alarm_reserved :9;
}alarm_bit_field_t;

/*********************************************************************************
                                  闹钟共同体                                   
*********************************************************************************/
typedef union
{
    uint32_t info;
    alarm_bit_field_t bit_field;
}alarm_union_t;


/*********************************************************************************
                                  闹钟参数                                     
*********************************************************************************/
typedef struct
{
    uint16_t vm_mask;

    uint8_t alarm_num;
    alarm_union_t alarm_union[\
        Alarm_Max_Num];
}AlarmInfoPara_t;
extern AlarmInfoPara_t Alarm_Info;

bool GetAlarmRemoteUpdate(void);
void SetAlarmRemoteUpdate(bool f);

void AlarmInfoParaRead(void);
void AlarmInfoParaWrite(void);
void AlarmInfoParaReset(void);
void AlarmInfoParaUpdate(void);

/*增删改查*/
void UserAlarmAdd(uint32_t info);
void UserAlarmTimeModify(uint32_t info);
void UserAlarmRepeatModify(uint32_t info);
void UserAlarmEnableModify(uint32_t info);
void UserAlarmAllModify(uint32_t info);
void UserAlarmDeleteAll(void);
void UserAlarmDeleteOne(uint8_t alarm_id);
uint32_t GetUserAlarmInfo(uint8_t alarm_id);

uint8_t GetAlarmIsOnId(void);
void SetAlarmIsOnId(uint8_t id);
void UserUtcMinAlarmProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
