#ifndef __ALLAH_NAME_FUNC_H__
#define __ALLAH_NAME_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

bool AllahNameIsArabic(void);
const char *GetAllahNameTableAr(uint8_t id);
const char *GetAllahNameTableEnFr(uint8_t id);
const char *GetAllahNameTableFollowLang(uint8_t id);
const char *GetAllahNameExTableFollowLang(uint8_t id);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
