#ifndef __RAISE_FUNC_H__
#define __RAISE_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Raise_Gs_WM (Qmi8658_Fifo_WM)

void RaiseGsDataHandle(u8 *w_buf, u32 w_len);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
