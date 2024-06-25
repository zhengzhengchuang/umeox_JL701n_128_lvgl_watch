#ifndef __SCROLLBAL_H__
#define __SCROLLBAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

typedef struct
{
    bool scrollbar_add;
    int16_t scrollbar_total;
    lv_obj_t *scrollbar_0_obj;
    lv_obj_t *scrollbar_1_obj;
}common_scrollbar_ctx_t;

void common_scrollbar_destroy(void);
void common_scrollbar_released_handle(void);
void common_scrollbar_create(lv_obj_t *obj, \
    int16_t scroll_offset, int16_t scroll_total, \
        menu_align_t menu_align);
void common_scrollbar_press_handle(int16_t scroll_offset);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
