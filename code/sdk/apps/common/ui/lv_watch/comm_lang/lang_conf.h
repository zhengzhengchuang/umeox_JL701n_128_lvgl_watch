#ifndef __LANG_CONF_H__
#define __LANG_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lang_txtid.h"
#include "../include/ui_menu.h"

/*********************************************************************************
                                  语言支持配置                                       
*********************************************************************************/
#define Conf_Lang_Ar  (1)  //阿拉伯语
#define Conf_Lang_En  (1)  //英文
#define Conf_Lang_Fr  (1)  //法语
#define Conf_Lang_Ur  (1)  //乌尔都语
#define Conf_Lang_Tu  (1)  //土耳其语
#define Conf_Lang_Ch  (1)  //简体中文
#define Conf_Lang_In  (1)  //印尼语
#define Conf_Lang_Pe  (1)  //波斯语
#define Conf_Lang_Ge  (1)  //德语
#define Conf_Lang_Th  (0)  //泰语 no use
#define Conf_Lang_Ru  (1)  //俄语
#define Conf_Lang_Hi  (0)  //印地语 no use
#define Conf_Lang_Ma  (1)  //马来西亚语
#define Conf_Lang_Uz  (1)  //乌兹别克斯坦语


/*********************************************************************************
                                  语言支持枚举                                       
*********************************************************************************/
enum
{
#if Conf_Lang_Ar
    lang_id_arabic = 0x00,
#endif

#if Conf_Lang_En
    lang_id_english,
#endif

#if Conf_Lang_Fr
    lang_id_french,
#endif

#if Conf_Lang_Ur
    lang_id_urdu,
#endif

#if Conf_Lang_Tu
    lang_id_turkish,
#endif

#if Conf_Lang_Ch
    lang_id_chinese,
#endif

#if Conf_Lang_In
    lang_id_indonesian,
#endif

#if Conf_Lang_Pe
    lang_id_persian,
#endif

#if Conf_Lang_Ge
    lang_id_german,
#endif

#if Conf_Lang_Th
    lang_id_thai,
#endif

#if Conf_Lang_Ru
    lang_id_russian,
#endif

#if Conf_Lang_Hi
    lang_id_hindi,
#endif

#if Conf_Lang_Ma
    lang_id_malaysian,
#endif

#if Conf_Lang_Uz
    lang_id_uzbek,
#endif

    lang_id_num,
};
typedef uint8_t lang_id_t;

/*********************************************************************************
                                  语言类型数量                                       
*********************************************************************************/
#define Lang_Num (lang_id_num)

bool lang_txt_is_arabic(void);
lang_id_t get_comm_sys_language(void);
void set_comm_sys_language(lang_id_t lang_id);
const void *get_lang_txt_with_id(lang_txtid_t txtid);
bool utf8_str_is_arabic(const char *utf8_str, uint32_t str_len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
