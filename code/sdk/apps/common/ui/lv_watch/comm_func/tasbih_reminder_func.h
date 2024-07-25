#ifndef __TASBIH_REMINDER_FUNC_H__
#define __TASBIH_REMINDER_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define TR_InvTimeMax (3)//unit:h

typedef struct
{
    u16 vm_mask;

    u8 TasbihR_Days;//赞念提醒重复（无效）
    bool TasbihR_Enable;//赞念提醒开关
    
    u32 TasbihR_STimestamp;//赞念提醒开始时间
    u32 TasbihR_ETimestamp;//赞念提醒结束时间
    u32 TasbihR_TotalInvTime;//赞念提醒总时间间隔

    u32 TasbihR_SetInvTime;//赞念提醒设定时间间隔
}TasbihRInfoPara_t;
extern TasbihRInfoPara_t TasbihR_info;

void TasbihRInfoParaRead(void);
void TasbihRInfoParaWrite(void);
void TasbihRInfoParaReset(void);
void TasbihRInfoParaUpdate(void);

extern u8 rtc_calculate_week_val(\
    struct sys_time *data_time);

void TasbihReminderProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
