#ifndef __AZKAR_SEC_H__
#define __AZKAR_SEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#define Container_W (LCD_WIDTH)
#define Container_H \
    (LCD_HEIGHT - LCD_UI_Y_OFFSET)

void ClearScrollPara(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
