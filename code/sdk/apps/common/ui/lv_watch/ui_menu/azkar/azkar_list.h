﻿#ifndef __AZKAR_LIST_H__
#define __AZKAR_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#define Elem_Num (AzkarTypeNum)

#define Title_Container_W \
    (LCD_WIDTH)
#define Title_Container_H (94)

#define List_Ctx_Container_W   \
    (LCD_WIDTH)
#define List_Ctx_Container_H   \
    (LCD_HEIGHT - Title_Container_H - \
        LCD_UI_Y_OFFSET)

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
