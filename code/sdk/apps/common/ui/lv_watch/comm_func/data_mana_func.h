#ifndef __DATA_MANA_FUNC_H__
#define __DATA_MANA_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void ResetAllNorVmData(void);

void PowerOnVmDataRead(void);
void PowerOffVmDataWrite(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
