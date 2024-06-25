#include "../lv_watch.h"

static uint8_t SecIdx;
uint8_t GetAzkarSecIdx(void)
{
    return SecIdx;
}
void SetAzkarSecIdx(uint8_t idx)
{
    SecIdx = idx;
    return;
}

static uint8_t TxtLang;
uint8_t GetAzkarTxtLang(void)
{
    return TxtLang;
}
void SetAzkarTxtLang(uint8_t lang)
{
    TxtLang = lang;
    return;
}

static uint8_t AzkarType;
uint8_t GetAzkarType(void)
{
    return AzkarType;
}
void SetAzkarType(uint8_t type)
{
    AzkarType = type;
    return;
}

static bool TitleIsValid(const char *str)
{
    if(str == NULL)
        return false;

    int ret = strncmp(str, AzkarNullLogo, 4);
    if(ret == 0)
        return false;
  
    return true;
}

static bool ContentIsImg(const char *str)
{
    if(str == NULL)
        return false;

    int ret = strncmp(str, AzkarImgLogo, 3);
    if(ret != 0)
        return false;

    return true;
}

uint8_t GetAzkarSecNum(void)
{
    uint8_t num = 0;

    uint8_t type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return num;

    if(type == AzkarTypeMR)
        num = Azkar_MR_Sec;
    else if(type == AzkarTypeER)
        num = Azkar_ER_Sec;
    else if(type == AzkarTypeAPR)
        num = Azkar_APR_Sec;
    else if(type == AzkarTypeHAU)
        num = Azkar_HAU_Sec;
    else if(type == AzkarTypeFas)
        num = Azkar_Fas_Sec;
    else if(type == AzkarTypeWWU)
        num = Azkar_WWU_Sec;
    else if(type == AzkarTypeGar)
        num = Azkar_Gar_Sec;
    else if(type == AzkarTypeAbu)
        num = Azkar_Abu_Sec;
    else if(type == AzkarTypeHome)
        num = Azkar_Home_Sec;
    else if(type == AzkarTypeMos)
        num = Azkar_Mos_Sec;
    else if(type == AzkarTypeAth)
        num = Azkar_Ath_Sec;
    else if(type == AzkarTypeFood)
        num = Azkar_Food_Sec;
    else if(type == AzkarTypeTra)
        num = Azkar_Tra_Sec;
    else if(type == AzkarTypeOR)
        num = Azkar_OR_Sec;
    else if(type == AzkarTypePra)
        num = Azkar_Pra_Sec;
    
    return num;
}

//<0：参数错误; 0：不显示朗读次数; >0：显示朗读次数
int GetAzkarFreq(uint8_t sec_idx)
{
    uint8_t type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return -1;

    uint8_t freq = 1;

    if(type == AzkarTypeMR)
    {
        if(sec_idx >= Azkar_MR_Sec)
            return -1;

        freq = azkar_MR_freq[sec_idx];
    }else if(type == AzkarTypeER)
    {
        if(sec_idx >= Azkar_ER_Sec)
            return -1;

        freq = azkar_ER_freq[sec_idx];
    }else if(type == AzkarTypeAPR)
    {
        if(sec_idx >= Azkar_APR_Sec)
            return -1;

        freq = azkar_Apr_freq[sec_idx];
    }else if(type == AzkarTypeHAU)
    {
        if(sec_idx >= Azkar_HAU_Sec)
            return -1;

        freq = azkar_Hau_freq[sec_idx];
    }else if(type == AzkarTypeFas)
    {
        if(sec_idx >= Azkar_Fas_Sec)
            return -1;

        freq = azkar_Fas_freq[sec_idx];
    }else if(type == AzkarTypeWWU)
    {
        if(sec_idx >= Azkar_WWU_Sec)
            return -1;

        freq = azkar_Wwu_freq[sec_idx];
    }else if(type == AzkarTypeGar)
    {
        if(sec_idx >= Azkar_Gar_Sec)
            return -1;

        freq = azkar_Gar_freq[sec_idx];
    }else if(type == AzkarTypeAbu)
    {
        if(sec_idx >= Azkar_Abu_Sec)
            return -1;

        freq = azkar_Abu_freq[sec_idx];
    }else if(type == AzkarTypeHome)
    {
        if(sec_idx >= Azkar_Home_Sec)
            return -1;

        freq = azkar_Home_freq[sec_idx];
    }else if(type == AzkarTypeMos)
    {
        if(sec_idx >= Azkar_Mos_Sec)
            return -1;

        freq = azkar_Mos_freq[sec_idx];
    }else if(type == AzkarTypeAth)
    {
        if(sec_idx >= Azkar_Ath_Sec)
            return -1;

        freq = azkar_Ath_freq[sec_idx];
    }else if(type == AzkarTypeFood)
    {
        if(sec_idx >= Azkar_Food_Sec)
            return -1;

        freq = azkar_Food_freq[sec_idx];
    }else if(type == AzkarTypeTra)
    {
        if(sec_idx >= Azkar_Tra_Sec)
            return -1;

        freq = azkar_Tra_freq[sec_idx];
    }else if(type == AzkarTypeOR)
    {
        if(sec_idx >= Azkar_OR_Sec)
            return -1;

        freq = azkar_OR_freq[sec_idx];
    }else if(type == AzkarTypePra)
    {
        if(sec_idx >= Azkar_Pra_Sec)
            return -1;

        freq = azkar_Pra_freq[sec_idx];
    }

    if(freq == 1)
        return 0;

    return freq;
}

//<0：参数错误; 0：没有标题; >0：显示标题
static char *ptitle;
char *GetpTitle(void)
{
    return ptitle;
}
int GetAzkarTitle(uint8_t sec_idx)
{
    uint8_t type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return -1;

    uint8_t lang = GetAzkarTxtLang();
    if(lang >= AzkarTxtLangNum)
        return -1;

    char *title = NULL;

    if(type == AzkarTypeMR)
    {
        if(sec_idx >= Azkar_MR_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_MR_ar[sec_idx];
        else
            title = azkar_title_MR_en_tr[sec_idx];
    }else if(type == AzkarTypeER)
    {
        if(sec_idx >= Azkar_ER_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_ER_ar[sec_idx];
        else
            title = azkar_title_ER_en_tr[sec_idx];
    }else if(type == AzkarTypeAPR)
    {
        if(sec_idx >= Azkar_APR_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Apr_ar[sec_idx];
        else
            title = azkar_title_Apr_en_tr[sec_idx];
    }else if(type == AzkarTypeHAU)
    {
        if(sec_idx >= Azkar_HAU_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Hau_ar[sec_idx];
        else
            title = azkar_title_Hau_en_tr[sec_idx];
    }else if(type == AzkarTypeFas)
    {
        if(sec_idx >= Azkar_Fas_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Fas_ar[sec_idx];
        else
            title = azkar_title_Fas_en_tr[sec_idx];
    }else if(type == AzkarTypeWWU)
    {
        if(sec_idx >= Azkar_WWU_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Wwu_ar[sec_idx];
        else
            title = azkar_title_Wwu_en_tr[sec_idx];
    }else if(type == AzkarTypeGar)
    {
        if(sec_idx >= Azkar_Gar_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Gar_ar[sec_idx];
        else
            title = azkar_title_Gar_en_tr[sec_idx];
    }else if(type == AzkarTypeAbu)
    {
        if(sec_idx >= Azkar_Abu_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Abu_ar[sec_idx];
        else
            title = azkar_title_Abu_en_tr[sec_idx];
    }else if(type == AzkarTypeHome)
    {
        if(sec_idx >= Azkar_Home_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Home_ar[sec_idx];
        else
            title = azkar_title_Home_en_tr[sec_idx];
    }else if(type == AzkarTypeMos)
    {
        if(sec_idx >= Azkar_Mos_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Mos_ar[sec_idx];
        else
            title = azkar_title_Mos_en_tr[sec_idx];
    }else if(type == AzkarTypeAth)
    {
        if(sec_idx >= Azkar_Ath_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Ath_ar[sec_idx];
        else
            title = azkar_title_Ath_en_tr[sec_idx];
    }else if(type == AzkarTypeFood)
    {
        if(sec_idx >= Azkar_Food_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Food_ar[sec_idx];
        else
            title = azkar_title_Food_en_tr[sec_idx];
    }else if(type == AzkarTypeTra)
    {
        if(sec_idx >= Azkar_Tra_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Tra_ar[sec_idx];
        else
            title = azkar_title_Tra_en_tr[sec_idx];
    }else if(type == AzkarTypeOR)
    {
        if(sec_idx >= Azkar_OR_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_OR_ar[sec_idx];
        else
            title = azkar_title_OR_en_tr[sec_idx];
    }else if(type == AzkarTypePra)
    {
        if(sec_idx >= Azkar_Pra_Sec)
            return -1;

        if(lang == AzkarTxtLangAr)
            title = azkar_title_Pra_ar[sec_idx];
        else
            title = azkar_title_Pra_en_tr[sec_idx];
    }

    if(TitleIsValid(title) == false)
        return 0;

    ptitle = title;

    return 1;
}

//<0：参数错误; 0：显示图标; >0：显示文字
static char *pcontent;
char *GetpContent(void)
{
    return pcontent;
}
int GetAzkarContent(uint8_t sec_idx)
{
    uint8_t type = GetAzkarType();
    if(type >= AzkarTypeNum)
        return -1;

    uint8_t lang = GetAzkarTxtLang();
    if(lang >= AzkarTxtLangNum)
        return -1;

    char *ctx = NULL;

    if(type == AzkarTypeMR)
    {
        if(sec_idx >= Azkar_MR_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_MR_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_MR_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_MR_ar[sec_idx];
    }else if(type == AzkarTypeER)
    {
        if(sec_idx >= Azkar_ER_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_ER_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_ER_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_ER_ar[sec_idx];
    }else if(type == AzkarTypeAPR)
    {
        if(sec_idx >= Azkar_APR_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Apr_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Apr_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Apr_ar[sec_idx];
    }else if(type == AzkarTypeHAU)
    {
        if(sec_idx >= Azkar_HAU_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Hau_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Hau_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Hau_ar[sec_idx];
    }else if(type == AzkarTypeFas)
    {
        if(sec_idx >= Azkar_Fas_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Fas_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Fas_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Fas_ar[sec_idx];
    }else if(type == AzkarTypeWWU)
    {
        if(sec_idx >= Azkar_WWU_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Wwu_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Wwu_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Wwu_ar[sec_idx];
    }else if(type == AzkarTypeGar)
    {
        if(sec_idx >= Azkar_Gar_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Gar_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Gar_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Gar_ar[sec_idx];
    }else if(type == AzkarTypeAbu)
    {
        if(sec_idx >= Azkar_Abu_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Abu_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Abu_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Abu_ar[sec_idx];
    }else if(type == AzkarTypeHome)
    {
        if(sec_idx >= Azkar_Home_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Home_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Home_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Home_ar[sec_idx];
    }else if(type == AzkarTypeMos)
    {
        if(sec_idx >= Azkar_Mos_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Mos_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Mos_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Mos_ar[sec_idx];
    }else if(type == AzkarTypeAth)
    {
        if(sec_idx >= Azkar_Ath_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Ath_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Ath_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Ath_ar[sec_idx];
    }else if(type == AzkarTypeFood)
    {
        if(sec_idx >= Azkar_Food_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Food_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Food_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Food_ar[sec_idx];
    }else if(type == AzkarTypeTra)
    {
        if(sec_idx >= Azkar_Tra_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Tra_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Tra_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Tra_ar[sec_idx];
    }else if(type == AzkarTypeOR)
    {
        if(sec_idx >= Azkar_OR_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_OR_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_OR_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_OR_ar[sec_idx];
    }else if(type == AzkarTypePra)
    {
        if(sec_idx >= Azkar_Pra_Sec)
            return -1;

        if(lang == AzkarTxtLangEn)
            ctx = azkar_Pra_en[sec_idx];
        else if(lang == AzkarTxtLangTr)
            ctx = azkar_Pra_tr[sec_idx];
        else if(lang == AzkarTxtLangAr)
            ctx = azkar_Pra_ar[sec_idx];
    }

    if(ContentIsImg(ctx) == true)
        return 0;

    pcontent = ctx;

    return 1;
}
