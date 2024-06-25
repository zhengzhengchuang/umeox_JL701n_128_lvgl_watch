#ifndef __SET_MAIN_H__
#define __SET_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#define Elem_Num (5)

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
