#ifndef __AZKAR_TABLE_H__
#define __AZKAR_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define AzkarImgLogo   "Img"
#define AzkarNullLogo  "NULL"

#define Azkar_MR_Sec   (25)
#define Azkar_ER_Sec   (14)
#define Azkar_APR_Sec  (14)
#define Azkar_HAU_Sec  (5)
#define Azkar_Fas_Sec  (4)
#define Azkar_WWU_Sec  (3)
#define Azkar_Gar_Sec  (3)
#define Azkar_Abu_Sec  (3)
#define Azkar_Home_Sec (7)
#define Azkar_Mos_Sec  (3)
#define Azkar_Ath_Sec  (2)
#define Azkar_Food_Sec (3)
#define Azkar_Tra_Sec  (3)
#define Azkar_OR_Sec   (15)
#define Azkar_Pra_Sec  (9)

#define Azkar_Sec_Max (Azkar_MR_Sec)

enum
{
    AzkarTxtLangEn = 0x00,
    AzkarTxtLangTr,
    AzkarTxtLangAr,

    AzkarTxtLangNum,
};
typedef uint8_t AzkarTxtLang_t;

enum
{
    AzkarTypeMR = 0x00,
    AzkarTypeER,
    AzkarTypeAPR,
    AzkarTypeHAU,
    AzkarTypeFas,
    AzkarTypeWWU,
    AzkarTypeGar,
    AzkarTypeAbu,
    AzkarTypeHome,
    AzkarTypeMos,
    AzkarTypeAth,
    AzkarTypeFood,
    AzkarTypeTra,
    AzkarTypeOR,
    AzkarTypePra,

    AzkarTypeNum,
};
typedef u8 AzkarType_t;

extern const char azkar_MR_en[Azkar_MR_Sec][356];
extern const char azkar_ER_en[Azkar_ER_Sec][356];
extern const char azkar_Apr_en[Azkar_APR_Sec][300];
extern const char azkar_Hau_en[Azkar_HAU_Sec][296];
extern const char azkar_Fas_en[Azkar_Fas_Sec][200];
extern const char azkar_Wwu_en[Azkar_WWU_Sec][110];
extern const char azkar_Gar_en[Azkar_Gar_Sec][228];
extern const char azkar_Abu_en[Azkar_Abu_Sec][160];
extern const char azkar_Home_en[Azkar_Home_Sec][160];
extern const char azkar_Mos_en[Azkar_Mos_Sec][240];
extern const char azkar_Ath_en[Azkar_Ath_Sec][252];
extern const char azkar_Food_en[Azkar_Food_Sec][110];
extern const char azkar_Tra_en[Azkar_Tra_Sec][92];
extern const char azkar_OR_en[Azkar_OR_Sec][248];
extern const char azkar_Pra_en[Azkar_Pra_Sec][200];

extern const char azkar_MR_tr[Azkar_MR_Sec][264];
extern const char azkar_ER_tr[Azkar_ER_Sec][252];
extern const char azkar_Apr_tr[Azkar_APR_Sec][184];
extern const char azkar_Hau_tr[Azkar_HAU_Sec][236];
extern const char azkar_Fas_tr[Azkar_Fas_Sec][144];
extern const char azkar_Wwu_tr[Azkar_WWU_Sec][232];
extern const char azkar_Gar_tr[Azkar_Gar_Sec][144];
extern const char azkar_Abu_tr[Azkar_Abu_Sec][104];
extern const char azkar_Home_tr[Azkar_Home_Sec][104];
extern const char azkar_Mos_tr[Azkar_Mos_Sec][228];
extern const char azkar_Ath_tr[Azkar_Ath_Sec][240];
extern const char azkar_Food_tr[Azkar_Food_Sec][96];
extern const char azkar_Tra_tr[Azkar_Tra_Sec][64];
extern const char azkar_OR_tr[Azkar_OR_Sec][164];
extern const char azkar_Pra_tr[Azkar_Pra_Sec][240];

extern const char azkar_MR_ar[Azkar_MR_Sec][548];
extern const char azkar_ER_ar[Azkar_ER_Sec][548];
extern const char azkar_Apr_ar[Azkar_APR_Sec][504];
extern const char azkar_Hau_ar[Azkar_HAU_Sec][484];
extern const char azkar_Fas_ar[Azkar_Fas_Sec][308];
extern const char azkar_Wwu_ar[Azkar_WWU_Sec][388];
extern const char azkar_Gar_ar[Azkar_Gar_Sec][300];
extern const char azkar_Abu_ar[Azkar_Abu_Sec][212];
extern const char azkar_Home_ar[Azkar_Home_Sec][212];
extern const char azkar_Mos_ar[Azkar_Mos_Sec][440];
extern const char azkar_Ath_ar[Azkar_Ath_Sec][364];
extern const char azkar_Food_ar[Azkar_Food_Sec][176];
extern const char azkar_Tra_ar[Azkar_Tra_Sec][132];
extern const char azkar_OR_ar[Azkar_OR_Sec][580];
extern const char azkar_Pra_ar[Azkar_Pra_Sec][504];

extern const u8 azkar_MR_freq[Azkar_MR_Sec];
extern const u8 azkar_ER_freq[Azkar_ER_Sec];
extern const u8 azkar_Apr_freq[Azkar_APR_Sec];
extern const u8 azkar_Hau_freq[Azkar_HAU_Sec];
extern const u8 azkar_Fas_freq[Azkar_Fas_Sec];
extern const u8 azkar_Wwu_freq[Azkar_WWU_Sec];
extern const u8 azkar_Gar_freq[Azkar_Gar_Sec];
extern const u8 azkar_Abu_freq[Azkar_Abu_Sec];
extern const u8 azkar_Home_freq[Azkar_Home_Sec];
extern const u8 azkar_Mos_freq[Azkar_Mos_Sec];
extern const u8 azkar_Ath_freq[Azkar_Ath_Sec];
extern const u8 azkar_Food_freq[Azkar_Food_Sec];
extern const u8 azkar_Tra_freq[Azkar_Tra_Sec];
extern const u8 azkar_OR_freq[Azkar_OR_Sec];
extern const u8 azkar_Pra_freq[Azkar_Pra_Sec];

extern const char azkar_title_MR_en_tr[Azkar_MR_Sec][5];
extern const char azkar_title_ER_en_tr[Azkar_ER_Sec][5];
extern const char azkar_title_Apr_en_tr[Azkar_APR_Sec][5];
extern const char azkar_title_Hau_en_tr[Azkar_HAU_Sec][45];
extern const char azkar_title_Fas_en_tr[Azkar_Fas_Sec][40];
extern const char azkar_title_Wwu_en_tr[Azkar_WWU_Sec][5];
extern const char azkar_title_Gar_en_tr[Azkar_Gar_Sec][30];
extern const char azkar_title_Abu_en_tr[Azkar_Abu_Sec][30];
extern const char azkar_title_Home_en_tr[Azkar_Home_Sec][30];
extern const char azkar_title_Mos_en_tr[Azkar_Mos_Sec][40];
extern const char azkar_title_Ath_en_tr[Azkar_Ath_Sec][25];
extern const char azkar_title_Food_en_tr[Azkar_Food_Sec][40];
extern const char azkar_title_Tra_en_tr[Azkar_Tra_Sec][50];
extern const char azkar_title_OR_en_tr[Azkar_OR_Sec][55];
extern const char azkar_title_Pra_en_tr[Azkar_Pra_Sec][60];

extern const char azkar_title_MR_ar[Azkar_MR_Sec][5];
extern const char azkar_title_ER_ar[Azkar_ER_Sec][5];
extern const char azkar_title_Apr_ar[Azkar_APR_Sec][5];
extern const char azkar_title_Hau_ar[Azkar_HAU_Sec][65];
extern const char azkar_title_Fas_ar[Azkar_Fas_Sec][40];
extern const char azkar_title_Wwu_ar[Azkar_WWU_Sec][5];
extern const char azkar_title_Gar_ar[Azkar_Gar_Sec][30];
extern const char azkar_title_Abu_ar[Azkar_Abu_Sec][45];
extern const char azkar_title_Home_ar[Azkar_Home_Sec][50];
extern const char azkar_title_Mos_ar[Azkar_Mos_Sec][60];
extern const char azkar_title_Ath_ar[Azkar_Ath_Sec][45];
extern const char azkar_title_Food_ar[Azkar_Food_Sec][45];
extern const char azkar_title_Tra_ar[Azkar_Tra_Sec][45];
extern const char azkar_title_OR_ar[Azkar_OR_Sec][65];
extern const char azkar_title_Pra_ar[Azkar_Pra_Sec][80];

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
