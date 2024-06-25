#ifndef __AZKAR_PRAYER_FUNC_H__
#define __AZKAR_PRAYER_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

uint8_t GetAzkarSecIdx(void);
void SetAzkarSecIdx(uint8_t idx);

uint8_t GetAzkarTxtLang(void);
void SetAzkarTxtLang(uint8_t lang);

uint8_t GetAzkarType(void);
void SetAzkarType(uint8_t type);

uint8_t GetAzkarSecNum(void);

int GetAzkarFreq(uint8_t sec_idx);

char *GetpTitle(void);
int GetAzkarTitle(uint8_t sec_idx);

char *GetpContent(void);
int GetAzkarContent(uint8_t sec_idx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
