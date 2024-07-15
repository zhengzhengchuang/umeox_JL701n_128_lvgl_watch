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

static const u8 *azkar_en_img[AzkarTypeNum] = 
{
    MR_en_img, ER_en_img, 
};

u8 GetAzkarEnImgNum(u8 sec_idx)
{
    u8 err = 0;

    u8 type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return err;

    u8 img_num = azkar_en_img[type][sec_idx];

    return img_num;
}

/* 获取赞念词起始img */
static const u32 azkar_en_file[AzkarTypeNum] = {
    azkar_en_MR_00_index, azkar_en_ER_00_index, 
};
u32 GetAzkarStartFile(void)
{
    u8 lang = GetAzkarLang();
    u8 type = GetAzkarType();

    u32 file = azkar_en_file[type];

    return file;
}