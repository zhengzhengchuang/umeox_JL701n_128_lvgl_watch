#ifndef __SLEEP_FUNC_H__
#define __SLEEP_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define SlpStartH (20)
#define SlpEndH (8)

typedef struct
{
    u16 vm_mask;

    bool valid;        //数据是否有效 
    u32 slp_dur;     //睡眠总时间:min
    u32 d_slp_dur;   //深睡时长:min
    u32 l_slp_dur;   //浅睡时长:min
    u32 r_slp_dur;   //快速眼动时长:min
    u32 wake_dur;    //清醒时长:min

    u32 slp_start_ts;  //睡眠开始时间戳
    u32 slp_end_ts;    //睡眠结束时间戳
}SlpPara_t;
extern SlpPara_t SlpPara;

extern struct SleepSummaryInput slp_in;
extern struct SleepSummaryOutput slp_out;

bool GetSleepEnState(void);
void SetSleepEnState(bool en);

bool GetFallAsleepFlag(void);
void SetFallAsleepFlag(bool f);

u8 GetSlpPpgEnState(void);
void SetSlpPpgEnState(u8 en);

u8 GetSlpHrRawDataLen(void);
float *GetSlpHrRawData(void);
void SetSlpHrRawData(u32 *buf, u16 len);

void PowerOnSetSleepData(void);
void VmFlashSleepCtxWrite(void);

void SleepPpgSensorOnOff(struct sys_time *ptime, s8 onoff);
void SleepStatusOutHandle(struct sys_time *ptime, u8 status);

void ManualEndSleepHandle(void);

void SleepUtcMinProcess(struct sys_time *ptime);

void SlpDataVmRead(void);
void SlpDataVmWrite(void);
void SlpDataVmReset(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
