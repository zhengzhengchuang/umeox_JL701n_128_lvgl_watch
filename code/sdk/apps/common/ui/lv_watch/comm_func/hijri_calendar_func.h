#ifndef __HIJRI_CALENDAR_FUNC_H__
#define __HIJRI_CALENDAR_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    Hijri_New_Year = 0x00,
    Ashuraa,
    Prophets_birthday,
    The_Night_of_IM,
    Mid_of_Shaban,
    Ramadan,
    Laylat_al_Qadr,
    Eid_ul_Fitr,
    Tarwiyah_day,
    Arafa_day,
    Eid_ul_Adha,
    Days_of_Tashreeq,

    HcalendarFesNum,
};
typedef u8 HcalendarFes_t;

typedef struct
{
    u16 HFes_year;
    u8  HFes_month;
    u8  HFes_day;   //节日的回历时间
}HcalendarFesInfo_t;

typedef struct
{
    u16 GFes_year;
    u8  GFes_month;
    u8  GFes_day; //节日的公历时间
}GcalendarFesInfo_t;

typedef struct
{
    u16 vm_mask;

    u16 Hcalendar_year;
    u8  Hcalendar_month;
    u8  Hcalendar_day;
    s8  Hcalendar_modify;

    struct sys_time Hutc_time;

    bool FesRemindersFlag;  
    HcalendarFesInfo_t HFesInfo[\
        HcalendarFesNum];
    GcalendarFesInfo_t GFesInfoS[\
        HcalendarFesNum];
    GcalendarFesInfo_t GFesInfoE[\
        HcalendarFesNum];
}HcalendarInfoPara_t;
extern HcalendarInfoPara_t HcalendarInfo;

u8 GetHcalendarFesType(void);
void SetHcalendarFesType(u8 type);

void HcalendarInfoParaRead(void);
void HcalendarInfoParaWrite(void);
void HcalendarInfoParaReset(void);
void HcalendarInfoParaUpdate(void);
void HcalendarProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
