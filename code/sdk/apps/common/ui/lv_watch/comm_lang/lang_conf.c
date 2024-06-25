#include "lang_table.h"
#include "../lv_watch.h"
#include "../poc_modem/poc_modem_vm.h"

/*********************************************************************************
                                  获取语言                                       
*********************************************************************************/
lang_id_t get_comm_sys_language(void)
{
    int vm_lang_id = \
        GetVmParaCacheByLabel(vm_label_sys_language);

    lang_id_t lang_id = \
        (lang_id_t)vm_lang_id;

    return lang_id;
}

/*********************************************************************************
                                  设置语言                                       
*********************************************************************************/
void set_comm_sys_language(lang_id_t lang_id)
{
    lang_id_t vm_lang_id = \
        GetVmParaCacheByLabel(vm_label_sys_language);

    if(lang_id == vm_lang_id)
        return;

    SetVmParaCacheByLabel(vm_label_sys_language, \
        lang_id);

    return;
}

/*********************************************************************************
                                  常用词条文本获取                                    
*********************************************************************************/
const void *get_lang_txt_with_id(lang_txtid_t txtid)
{
    int vm_lang_id = \
        GetVmParaCacheByLabel(vm_label_sys_language);

    lang_id_t lang_id = \
        (lang_id_t)vm_lang_id;
        

    return (lang_str_table[lang_id][txtid]);
}

/*********************************************************************************
                                  语言是阿拉伯文?(阿拉伯文只有右对齐、居中对齐q)                                 
*********************************************************************************/
bool lang_txt_is_arabic(void)
{
    lang_id_t lang_id = \
        get_comm_sys_language();

    if(lang_id == lang_id_arabic || \
        lang_id == lang_id_urdu || \
            lang_id == lang_id_persian)
        return true;

    return false;
}

/*********************************************************************************
                                  判断字符串中是否存在阿拉伯文                                 
*********************************************************************************/
bool utf8_str_is_arabic(const char *utf8_str, uint32_t str_len)
{
    if(!utf8_str || !str_len) 
        return false;

    uint8_t utf8_size;
    uint32_t unicode_letter;

    for(uint32_t idx = 0; idx < str_len; idx += utf8_size)
    {
        utf8_size = \
            _lv_txt_encoded_size(&utf8_str[idx]);
        if(!utf8_size)
            break;

        unicode_letter = \
            _lv_txt_encoded_next(utf8_str, &idx);

        if((unicode_letter >= 0x0600 && unicode_letter <= 0x06FF) || \
            (unicode_letter >= 0x0750 && unicode_letter <= 0x07FF) || \
                (unicode_letter >= 0xFB50 && unicode_letter <= 0xFDFF) || \
                    (unicode_letter >= 0xFE70 && unicode_letter <= 0xFEFF))
            return true;

    }
    
    
    return false;
}
