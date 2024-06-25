#ifndef __LANG_TABLE_H__
#define __LANG_TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lang_conf.h"
#include "lang_txtid.h"
#include "../include/ui_menu.h"

/*********************************************************************************
                                  语言字符串长度                                       
*********************************************************************************/
#define Lang_Str_Len (110)

/*********************************************************************************
                                  语言字符串数量                                       
*********************************************************************************/
#define Lang_Str_Num (lang_txtid_max)

extern const char lang_str_table[Lang_Num]\
    [Lang_Str_Num][Lang_Str_Len];
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
