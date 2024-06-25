#ifndef __KAABA_QIBLA_FUNC_H__
#define __KAABA_QIBLA_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

s16 GetKaabaQiblaAzimuth(void);
void SetKaabaQiblaAzimuth(s16 a);

void KaabaQiblaParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
