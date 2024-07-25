#include "../lv_watch.h"

static u8 sec_idx;
u8 GetAzkarSecIdx(void)
{
    return sec_idx;
}
void SetAzkarSecIdx(u8 idx)
{
    sec_idx = idx;
    return;
}

static u8 azkar_type;
u8 GetAzkarType(void)
{
    return azkar_type;
}
void SetAzkarType(u8 type)
{
    azkar_type = type;
    return;
}

static u8 zakar_lang;
u8 GetAzkarLang(void)
{
    return zakar_lang;
}
void SetAzkarLang(u8 lang)
{
    zakar_lang = lang;
    return;
}

static const u8 sec_table[AzkarTypeNum] = {
    Azkar_MR_Sec, Azkar_ER_Sec, Azkar_BS_Sec, Azkar_APR_Sec, \
    Azkar_HAU_Sec, Azkar_FAS_Sec, Azkar_WWU_Sec, Azkar_GAR_Sec, \
    Azkar_ABU_Sec, Azkar_HOME_Sec, Azkar_MOS_Sec, Azkar_ATH_Sec, \
    Azkar_FOOD_Sec, Azkar_TRA_Sec, Azkar_OR_Sec, Azkar_PRA_Sec,
};
u8 GetAzkarSecNum(void)
{
    u8 num = 0;

    u8 type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return num;

    num = sec_table[type];

    return num;
}


/* 朗读次数 */
static const u8 MR_freq[Azkar_MR_Sec] = 
{
    1, 3, 3, 3, 1, 1, 1, 4, 1, 3, 7, 1, 1, 3, \
    3, 1, 1, 1, 100, 10, 100, 3, 1, 100, 10,  
};

static const u8 ER_freq[Azkar_ER_Sec] = 
{
    1, 3, 3, 3, 1, 3, 1, 3, 1, 3, 3, 1, 100, 10, 
};

static const u8 BS_freq[Azkar_BS_Sec] = 
{
    1, 3, 3, 3, 1, 1, 33, 33, 34, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 APR_freq[Azkar_APR_Sec] = 
{
    1, 3, 1, 1, 1, 1, 33, 33, 33, 1, 1, 3, 3, 3,
};

static const u8 HAU_freq[Azkar_HAU_Sec] = 
{
    1, 1, 1, 1, 1,   
};

static const u8 FAS_freq[Azkar_FAS_Sec] = 
{
    1, 1, 1,     
};

static const u8 WWU_freq[Azkar_WWU_Sec] = 
{
    1, 1, 1,
};

static const u8 GAR_freq[Azkar_GAR_Sec] = 
{
    1, 1, 1,
};

static const u8 ABU_freq[Azkar_ABU_Sec] = 
{
    1, 1, 1,
};

static const u8 HOME_freq[Azkar_HOME_Sec] = 
{
    1, 1, 1, 1,  
};

static const u8 MOS_freq[Azkar_MOS_Sec] = 
{
    1, 1, 1,
};

static const u8 ATH_freq[Azkar_ATH_Sec] = 
{
    1, 1,
};

static const u8 FOOD_freq[Azkar_FOOD_Sec] = 
{
    1, 1, 1,
};

static const u8 TRA_freq[Azkar_TRA_Sec] = 
{
    1, 1, 1,
};

static const u8 OR_freq[Azkar_OR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 7, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 PRA_freq[Azkar_PRA_Sec] = 
{
    1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 
};

static const u8 *azkar_freq[AzkarTypeNum] = {
    MR_freq, ER_freq, BS_freq, APR_freq, \
    HAU_freq, FAS_freq, WWU_freq, GAR_freq, \
    ABU_freq, HOME_freq, MOS_freq, ATH_freq, \
    FOOD_freq, TRA_freq, OR_freq, PRA_freq,
};

//<0：参数错误; 0：不显示朗读次数; >0：显示朗读次数
int GetAzkarFreq(u8 sec_idx)
{
    int err = -1;

    u8 type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return err;

    u8 freq = azkar_freq[type][sec_idx];
    if(freq == 1) 
        freq = 0;

    return freq;
}

/* 赞念词条英文图片 */
static const u8 MR_en_img[Azkar_MR_Sec] = 
{
    2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static const u8 ER_en_img[Azkar_ER_Sec] = 
{
    2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 BS_en_img[Azkar_BS_Sec] = 
{
    2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
};

static const u8 APR_en_img[Azkar_APR_Sec] = 
{
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1,
};

static const u8 HAU_en_img[Azkar_HAU_Sec] = 
{
    1, 1, 2, 1, 1,
};

static const u8 FAS_en_img[Azkar_FAS_Sec] = 
{
    1, 1, 1, 
};

static const u8 WWU_en_img[Azkar_WWU_Sec] = 
{
    1, 1, 1, 
};

static const u8 GAR_en_img[Azkar_GAR_Sec] = 
{
    1, 1, 1, 
};

static const u8 ABU_en_img[Azkar_ABU_Sec] = 
{
    1, 1, 1, 
};

static const u8 HOME_en_img[Azkar_HOME_Sec] = 
{
    1, 1, 1, 1, 
};

static const u8 MOS_en_img[Azkar_MOS_Sec] = 
{
    1, 1, 1, 
};

static const u8 ATH_en_img[Azkar_ATH_Sec] = 
{
    2, 1, 
};

static const u8 FOOD_en_img[Azkar_FOOD_Sec] = 
{
    1, 1, 1, 
};

static const u8 TRA_en_img[Azkar_TRA_Sec] = 
{
    2, 1, 1, 
};

static const u8 OR_en_img[Azkar_OR_Sec] = 
{
    1, 2, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 
};

static const u8 PRA_en_img[Azkar_PRA_Sec] = 
{
     1, 1, 1, 1, 1, 1, 1, 2, 1, 3, 
};

static const u8 *azkar_en_img[AzkarTypeNum] = 
{
    MR_en_img, ER_en_img, BS_en_img, APR_en_img, HAU_en_img, \
    FAS_en_img, WWU_en_img, GAR_en_img, ABU_en_img, HOME_en_img, \
    MOS_en_img, ATH_en_img, FOOD_en_img, TRA_en_img, OR_en_img, \
    PRA_en_img,
};

static const u32 azkar_en_file[AzkarTypeNum] = {
    azkar_en_MR_00_index, azkar_en_ER_00_index, azkar_en_BS_00_index, azkar_en_APR_00_index, azkar_en_HAU_00_index, \
    azkar_en_FAS_00_index, azkar_en_WWU_00_index, azkar_en_GAR_00_index, azkar_en_ABU_00_index, azkar_en_HOME_00_index, \
    azkar_en_MOS_00_index, azkar_en_ATH_00_index, azkar_en_FOOD_00_index, azkar_en_TRA_00_index, azkar_en_OR_00_index, \
    azkar_en_PRA_00_index,
};

/* 赞念词条英译图片 */
static const u8 MR_en_tr_img[Azkar_MR_Sec] = 
{
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 ER_en_tr_img[Azkar_ER_Sec] = 
{
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static const u8 BS_en_tr_img[Azkar_BS_Sec] = 
{
    2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 
};

static const u8 APR_en_tr_img[Azkar_APR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 
};

static const u8 HAU_en_tr_img[Azkar_HAU_Sec] = 
{
    1, 1, 1, 1, 1, 
};

static const u8 FAS_en_tr_img[Azkar_FAS_Sec] = 
{
    1, 1, 1, 
};

static const u8 WWU_en_tr_img[Azkar_WWU_Sec] = 
{
    1, 1, 1, 
};

static const u8 GAR_en_tr_img[Azkar_GAR_Sec] = 
{
    1, 1, 1, 
};

static const u8 ABU_en_tr_img[Azkar_ABU_Sec] = 
{
    1, 1, 1, 
};

static const u8 HOME_en_tr_img[Azkar_HOME_Sec] = 
{
    1, 1, 1, 1, 
};

static const u8 MOS_en_tr_img[Azkar_MOS_Sec] = 
{
    1, 1, 1, 
};

static const u8 ATH_en_tr_img[Azkar_ATH_Sec] = 
{
    1, 1, 
};

static const u8 FOOD_en_tr_img[Azkar_FOOD_Sec] = 
{
    1, 1, 1, 
};

static const u8 TRA_en_tr_img[Azkar_TRA_Sec] = 
{
    2, 1, 1, 
};

static const u8 OR_en_tr_img[Azkar_OR_Sec] = 
{
    1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 PRA_en_tr_img[Azkar_PRA_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 2, 1, 2,
};

static const u8 *azkar_en_tr_img[AzkarTypeNum] = 
{
    MR_en_tr_img, ER_en_tr_img, BS_en_tr_img, APR_en_tr_img, HAU_en_tr_img, \
    FAS_en_tr_img, WWU_en_tr_img, GAR_en_tr_img, ABU_en_tr_img, HOME_en_tr_img, \
    MOS_en_tr_img, ATH_en_tr_img, FOOD_en_tr_img, TRA_en_tr_img, OR_en_tr_img, \
    PRA_en_tr_img,
};

static const u32 azkar_en_tr_file[AzkarTypeNum] = {
    azkar_en_tr_MR_00_index, azkar_en_tr_ER_00_index, azkar_en_tr_BS_00_index, azkar_en_tr_APR_00_index, azkar_en_tr_HAU_00_index, \
    azkar_en_tr_FAS_00_index, azkar_en_tr_WWU_00_index, azkar_en_tr_GAR_00_index, azkar_en_tr_ABU_00_index, azkar_en_tr_HOME_00_index, \
    azkar_en_tr_MOS_00_index, azkar_en_tr_ATH_00_index, azkar_en_tr_FOOD_00_index, azkar_en_tr_TRA_00_index, azkar_en_tr_OR_00_index, \
    azkar_en_tr_PRA_00_index,
};

/* 赞念词条法文图片 */
static const u8 MR_fr_img[Azkar_MR_Sec] = 
{
    2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 ER_fr_img[Azkar_ER_Sec] = 
{
    2, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static const u8 BS_fr_img[Azkar_BS_Sec] = 
{
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
};

static const u8 APR_fr_img[Azkar_APR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1,
};

static const u8 HAU_fr_img[Azkar_HAU_Sec] = 
{
    1, 1, 2, 1, 1, 
};

static const u8 FAS_fr_img[Azkar_FAS_Sec] = 
{
    1, 1, 1,
};

static const u8 WWU_fr_img[Azkar_WWU_Sec] = 
{
    1, 1, 1,
};

static const u8 GAR_fr_img[Azkar_GAR_Sec] = 
{
    1, 1, 1,
};

static const u8 ABU_fr_img[Azkar_ABU_Sec] = 
{
    1, 1, 1,
};

static const u8 HOME_fr_img[Azkar_HOME_Sec] = 
{
    1, 1, 1, 1, 
};

static const u8 MOS_fr_img[Azkar_MOS_Sec] = 
{
    2, 1, 1,
};

static const u8 ATH_fr_img[Azkar_ATH_Sec] = 
{
    1, 1,
};

static const u8 FOOD_fr_img[Azkar_FOOD_Sec] = 
{
    1, 1, 1, 
};

static const u8 TRA_fr_img[Azkar_TRA_Sec] = 
{
    2, 1, 1, 
};

static const u8 OR_fr_img[Azkar_OR_Sec] = 
{
    1, 1, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 
};

static const u8 PRA_fr_img[Azkar_PRA_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 2, 1, 4
};

static const u8 *azkar_fr_img[AzkarTypeNum] = 
{
    MR_fr_img, ER_fr_img, BS_fr_img, APR_fr_img, HAU_fr_img, \
    FAS_fr_img, WWU_fr_img, GAR_fr_img, ABU_fr_img, HOME_fr_img, \
    MOS_fr_img, ATH_fr_img, FOOD_fr_img, TRA_fr_img, OR_fr_img, \
    PRA_fr_img,
};

static const u32 azkar_fr_file[AzkarTypeNum] = {
    // azkar_fr_MR_00_index, azkar_fr_ER_00_index, azkar_fr_BS_00_index, azkar_fr_APR_00_index, azkar_fr_HAU_00_index, \
    // azkar_fr_FAS_00_index, azkar_fr_WWU_00_index, azkar_fr_GAR_00_index, azkar_fr_ABU_00_index, azkar_fr_HOME_00_index, \
    // azkar_fr_MOS_00_index, azkar_fr_ATH_00_index, azkar_fr_FOOD_00_index, azkar_fr_TRA_00_index, azkar_fr_OR_00_index, \
    // azkar_fr_PRA_00_index,
};

/* 赞念词条法译图片 */
static const u8 MR_fr_tr_img[Azkar_MR_Sec] = 
{
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 ER_fr_tr_img[Azkar_ER_Sec] = 
{
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 BS_fr_tr_img[Azkar_BS_Sec] = 
{
    1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 
};

static const u8 APR_fr_tr_img[Azkar_APR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 HAU_fr_tr_img[Azkar_HAU_Sec] = 
{
    1, 1, 1, 1, 1,
};

static const u8 FAS_fr_tr_img[Azkar_FAS_Sec] = 
{
    1, 1, 1,
};

static const u8 WWU_fr_tr_img[Azkar_WWU_Sec] = 
{
    1, 1, 1,
};

static const u8 GAR_fr_tr_img[Azkar_GAR_Sec] = 
{
    1, 1, 1,
};

static const u8 ABU_fr_tr_img[Azkar_ABU_Sec] = 
{
    1, 1, 1,
};

static const u8 HOME_fr_tr_img[Azkar_HOME_Sec] = 
{
    1, 1, 1, 1, 
};

static const u8 MOS_fr_tr_img[Azkar_MOS_Sec] = 
{
    2, 1, 1,
};

static const u8 ATH_fr_tr_img[Azkar_ATH_Sec] = 
{
    1, 1,
};

static const u8 FOOD_fr_tr_img[Azkar_FOOD_Sec] = 
{
    1, 1, 1, 
};

static const u8 TRA_fr_tr_img[Azkar_TRA_Sec] = 
{
    2, 1, 1, 
};

static const u8 OR_fr_tr_img[Azkar_OR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 PRA_fr_tr_img[Azkar_PRA_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 2, 1, 2
};

static const u8 *azkar_fr_tr_img[AzkarTypeNum] = 
{
    MR_fr_tr_img, ER_fr_tr_img, BS_fr_tr_img, APR_fr_tr_img, HAU_fr_tr_img, \
    FAS_fr_tr_img, WWU_fr_tr_img, GAR_fr_tr_img, ABU_fr_tr_img, HOME_fr_tr_img, \
    MOS_fr_tr_img, ATH_fr_tr_img, FOOD_fr_tr_img, TRA_fr_tr_img, OR_fr_tr_img, \
    PRA_fr_tr_img,
};

static const u32 azkar_fr_tr_file[AzkarTypeNum] = {
    // azkar_fr_tr_MR_00_index, azkar_fr_tr_ER_00_index, azkar_fr_tr_BS_00_index, azkar_fr_tr_APR_00_index, azkar_fr_tr_HAU_00_index, \
    // azkar_fr_tr_FAS_00_index, azkar_fr_tr_WWU_00_index, azkar_fr_tr_GAR_00_index, azkar_fr_tr_ABU_00_index, azkar_fr_tr_HOME_00_index, \
    // azkar_fr_tr_MOS_00_index, azkar_fr_tr_ATH_00_index, azkar_fr_tr_FOOD_00_index, azkar_fr_tr_TRA_00_index, azkar_fr_tr_OR_00_index, \
    // azkar_fr_tr_PRA_00_index,
};

/* 赞念词条阿拉伯图片 */
static const u8 MR_ar_img[Azkar_MR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static const u8 ER_ar_img[Azkar_ER_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 BS_ar_img[Azkar_BS_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 APR_ar_img[Azkar_APR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
};

static const u8 HAU_ar_img[Azkar_HAU_Sec] = 
{
    1, 1, 1, 1, 1, 
};

static const u8 FAS_ar_img[Azkar_FAS_Sec] = 
{
    1, 1, 1, 
};

static const u8 WWU_ar_img[Azkar_WWU_Sec] = 
{
    1, 1, 1, 
};

static const u8 GAR_ar_img[Azkar_GAR_Sec] = 
{
    1, 1, 1, 
};

static const u8 ABU_ar_img[Azkar_ABU_Sec] = 
{
    1, 1, 1, 
};

static const u8 HOME_ar_img[Azkar_HOME_Sec] = 
{
    1, 1, 1, 1, 
};

static const u8 MOS_ar_img[Azkar_MOS_Sec] = 
{
    2, 1, 1, 
};

static const u8 ATH_ar_img[Azkar_ATH_Sec] = 
{
    1, 1, 
};

static const u8 FOOD_ar_img[Azkar_FOOD_Sec] = 
{
    1, 1, 1, 
};

static const u8 TRA_ar_img[Azkar_TRA_Sec] = 
{
    2, 1, 1, 
};

static const u8 OR_ar_img[Azkar_OR_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

static const u8 PRA_ar_img[Azkar_PRA_Sec] = 
{
    1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 
};

static const u8 *azkar_ar_img[AzkarTypeNum] = 
{
    MR_ar_img, ER_ar_img, BS_ar_img, APR_ar_img, HAU_ar_img, \
    FAS_ar_img, WWU_ar_img, GAR_ar_img, ABU_ar_img, HOME_ar_img, \
    MOS_ar_img, ATH_ar_img, FOOD_ar_img, TRA_ar_img, OR_ar_img, \
    PRA_ar_img,
};

static const u32 azkar_ar_file[AzkarTypeNum] = {
    azkar_ar_MR_00_index, azkar_ar_ER_00_index, azkar_ar_BS_00_index, azkar_ar_APR_00_index, azkar_ar_HAU_00_index, \
    azkar_ar_FAS_00_index, azkar_ar_WWU_00_index, azkar_ar_GAR_00_index, azkar_ar_ABU_00_index, azkar_ar_HOME_00_index, \
    azkar_ar_MOS_00_index, azkar_ar_ATH_00_index, azkar_ar_FOOD_00_index, azkar_ar_TRA_00_index, azkar_ar_OR_00_index, \
    azkar_ar_PRA_00_index,
};

u8 GetAzkarImgNum(u8 sec_idx)
{
    u8 lang = GetAzkarLang();
    u8 type = GetAzkarType();

    u8 img_num;
    if(lang == AzkarLangEn)
        img_num = azkar_en_img[type][sec_idx];
    else if(lang == AzkarLangEnTr)
        img_num = azkar_en_tr_img[type][sec_idx];
    else if(lang == AzkarLangFr)
        img_num = azkar_fr_img[type][sec_idx];
    else if(lang == AzkarLangFrTr)
        img_num = azkar_fr_tr_img[type][sec_idx];
    else if(lang == AzkarLangAr)
        img_num = azkar_ar_img[type][sec_idx];

    return img_num;
}

u32 GetAzkarStartFile(void)
{
    u8 lang = GetAzkarLang();
    u8 type = GetAzkarType();

    u32 file;
    if(lang == AzkarLangEn)
        file = azkar_en_file[type];
    else if(lang == AzkarLangEnTr)
        file = azkar_en_tr_file[type];
    else if(lang == AzkarLangFr)
        file = azkar_fr_file[type];
    else if(lang == AzkarLangFrTr)
        file = azkar_fr_tr_file[type];
    else if(lang == AzkarLangAr)
        file = azkar_ar_file[type];

    return file;
}
