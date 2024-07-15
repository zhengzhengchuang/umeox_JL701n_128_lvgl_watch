#ifndef __AZKAR_FUNC_H__
#define __AZKAR_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Azkar_MR_Sec   (25)//Morning Remembrance
#define Azkar_ER_Sec   (14)//Evening Remembrance
#define Azkar_BS_Sec   (17)//Before sleeping
#define Azkar_APR_Sec  (14)//After Prayer Remembrance
#define Azkar_HAU_Sec  (5) //Hajj and Umra
#define Azkar_FAS_Sec  (3) //Fasting
#define Azkar_WWU_Sec  (3) //When waking up
#define Azkar_GAR_Sec  (3) //Garment
#define Azkar_ABU_Sec  (3) //Abulation
#define Azkar_HOME_Sec (4) //Home
#define Azkar_MOS_Sec  (3) //Mosque
#define Azkar_ATH_Sec  (2) //Athan (the call to prayer)
#define Azkar_FOOD_Sec (3) //Food
#define Azkar_TRA_Sec  (3) //Traveling
#define Azkar_OR_Sec   (16)//Other Remembrance
#define Azkar_PRA_Sec  (10)//Prayer

#define Azkar_Max_Sec (Azkar_MR_Sec)

enum
{
    AzkarLangEn,   //英语
    AzkarLangEnTr, //英语音译
    AzkarLangFr,   //法语
    AzkarLangFrTr, //法语音译
    AzkarLangAr,   //阿拉伯语

    AzkarLangNum,
};

enum
{
    AzkarTypeMR,
    AzkarTypeER,
    AzkarTypeBS,
    AzkarTypeAPR,
    AzkarTypeHAU,
    AzkarTypeFAS,
    AzkarTypeWWU,
    AzkarTypeGAR,
    AzkarTypeABU,
    AzkarTypeHOME,
    AzkarTypeMOS,
    AzkarTypeATH,
    AzkarTypeFOOD,
    AzkarTypeTRA,
    AzkarTypeOR,
    AzkarTypePRA,

    AzkarTypeNum,
};

u8 GetAzkarSecIdx(void);
void SetAzkarSecIdx(u8 idx);

u8 GetAzkarType(void);
void SetAzkarType(u8 type);

u8 GetAzkarLang(void);
void SetAzkarLang(u8 lang);

u8 GetAzkarSecNum(void);

int GetAzkarFreq(u8 sec_idx);

u8 GetAzkarEnImgNum(u8 sec_idx);

u32 GetAzkarStartFile(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
