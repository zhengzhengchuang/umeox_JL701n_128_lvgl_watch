#ifndef __SLEEP_FUNC_H__
#define __SLEEP_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define SlpStartH (18)

typedef struct
{
    bool valid;        //数据是否有效 

    u16 slp_total;     //睡眠总时间

    u16 d_slp_total;   //深睡总时间
    u16 l_slp_total;   //浅睡总时间
    u16 r_slp_total;   //快速眼动总时间
    u16 wakeup_total;  //醒来总时间

    u16 slp_timestamp; //睡眠时间戳
    u16 wkp_timestamp; //醒来时间戳

    struct sys_time time; //存储时间
}SleepInfoPara_t;
extern SleepInfoPara_t Slp_Info;

void SetSleepInfoPara(void);
void SleepProcess(struct sys_time *utc_time);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
