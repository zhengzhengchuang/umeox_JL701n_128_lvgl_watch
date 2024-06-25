#include "../lv_watch.h"

bool AllahNameIsArabic(void)
{
    bool IsArabic = false;

    int vm_lang_id = \
        GetVmParaCacheByLabel(\
            vm_label_sys_language);

    if(vm_lang_id == \
        lang_id_arabic)
        IsArabic = true;

    return IsArabic;
}

const char *GetAllahNameTableAr(\
    uint8_t id)
{
    static const char *TableTxt;

    TableTxt = NULL;

    TableTxt = al_name_table_ar[id];

    return TableTxt;
}

const char *GetAllahNameTableEnFr(\
    uint8_t id)
{
    static const char *TableTxt;

    int vm_lang_id = \
        GetVmParaCacheByLabel(\
            vm_label_sys_language);

    TableTxt = NULL;

    if(vm_lang_id == \
        lang_id_french)
        TableTxt =  al_name_table_fr[id];
    else
        TableTxt =  al_name_table_en[id];

    return TableTxt;
};

const char *GetAllahNameTableFollowLang(\
    uint8_t id)
{
    static const char *TableTxt;

    int vm_lang_id = \
        GetVmParaCacheByLabel(\
            vm_label_sys_language);

    TableTxt = NULL;

    if(vm_lang_id == \
        lang_id_arabic)
        TableTxt = al_name_table_ar[id];
    else if(vm_lang_id == \
        lang_id_french)
        TableTxt = al_name_table_fr[id];
    else
        TableTxt = al_name_table_en[id];

    return TableTxt;
}

const char *GetAllahNameExTableFollowLang(\
    uint8_t id)
{
    static const char *TableTxt;

    int vm_lang_id = \
        GetVmParaCacheByLabel(\
            vm_label_sys_language);

    TableTxt = NULL;

    if(vm_lang_id == \
        lang_id_arabic)
        TableTxt = al_name_ex_table_ar[id];
    else if(vm_lang_id == \
        lang_id_french)
        TableTxt = al_name_ex_table_fr[id];
    else
        TableTxt = al_name_ex_table_en[id];

    return TableTxt;
}
