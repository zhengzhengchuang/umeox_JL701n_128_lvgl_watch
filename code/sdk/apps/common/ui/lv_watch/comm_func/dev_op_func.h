#ifndef __DEV_OP_FUNC_H__
#define __DEV_OP_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void DevOpMenuPopUp(void);

void DevOpResetHandle(void);
void DevOpRestartHandle(void);
void DevOpShutdownHandle(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
