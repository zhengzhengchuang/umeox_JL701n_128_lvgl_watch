#ifndef __AUDIO_H__
#define __AUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#define Elem_Num (QpAudioNum)

#define Container_W (LCD_WIDTH)
#define Container_H \
    (LCD_HEIGHT - LCD_UI_Y_OFFSET)

extern const uint8_t Qpec_idx[Elem_Num];
extern lv_obj_t *Qpelem_container[Elem_Num];

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
