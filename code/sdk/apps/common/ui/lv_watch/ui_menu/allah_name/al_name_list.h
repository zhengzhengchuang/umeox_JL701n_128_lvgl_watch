#ifndef __AL_NAME_LIST_H__
#define __AL_NAME_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

#define Elem_Num (Al_Name_Num)

#define Container_W (LCD_WIDTH)
#define Container_H \
    (LCD_HEIGHT - LCD_UI_Y_OFFSET)

extern lv_obj_t *al_list_ctx_container;
extern const uint8_t al_ec_idx[Elem_Num];
extern lv_obj_t *al_elem_container[Elem_Num];

uint8_t GetAlNameExId(void);
void SetAlNameExId(uint8_t id);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
