#ifndef __PRAYER_TIME_FUNC_H__
#define __PRAYER_TIME_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define PTimeElapsedDur (15*60)

enum
{
    Fajr = 0x00,
    Sunrise,
    Dhuhr,
    Asr,
    Sunset,
    Maghrib,
    Isha,

    PT_Num,
};
typedef u8 PTType_t;

enum
{
    Conv_Common = 0x01,
    Conv_Ingushetia = 0x64,
    Conv_KheuBrunei = 0x65,
};
typedef u8 PTConv_t;

enum
{
    Juri_Shafii,
	Juri_Hanafi,
};
typedef u8 JuriMethod_t;

enum
{
    HLat_None,
    HLat_Midnight,
    HLat_SevenNight,
    HLat_AngleBased,
};
typedef u8 HLatMethod_t;

typedef struct
{
    float dd;
    float eq_t;
}FloatPair_t;

typedef struct
{
    u16 vm_mask;

    PTConv_t conv;
    JuriMethod_t juri;
    HLatMethod_t hlat;

    float fajr_angle;
    float isha_value;
    float maghrib_value;

    bool isha_is_minutes;
    bool maghrib_is_minutes;

    bool remind_en[PT_Num];
    bool voice_en[PT_Num];
    s8 remind_offset[PT_Num];
    s8 result_offset[PT_Num];
}PTCfgPara_t;

typedef struct
{
    /*原始时间戳*/
    u32 origin_timestamp[PT_Num];

    /*结果时间戳*/
    u32 result_timestamp[PT_Num];

    /*提醒时间戳*/
    u32 remind_timestamp[PT_Num];
}PTInfoPara_t;

extern PTCfgPara_t PT_Cfg;
extern PTInfoPara_t PT_Info;
extern const u16 PTimeElapsed[PT_Num];

u8 GetPTimeAdvanceFlag(void);
void SetPTimeAdvanceFlag(u8 f);

u8 GetPTimeType(void);
u8 GetPTimeProcessType(void);
void SetPTimeProcessType(u8 Type);
u8 GetPTimeCountdownMode(u8 type);
u32 GetPTimeCountdownTime(u8 type);
void PTimeSecProcess(struct sys_time *ptime);
void PTimeDayProcess(struct sys_time *ptime);
void PTimeResultOffsetUpdate(u8 type, s8 val);
void PTimeRemindOffsetUpdate(u8 type, s8 val);

void PTimeCfgParaRead(void);
void PTimeCfgParaWrite(void);
void PTimeCfgParaReset(void);
void PTimeCfgParaUpdate(void);
void PTimeInfoParaUpdate(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
