#ifndef __LL_INFO_FUNC_H__
#define __LL_INFO_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

typedef struct
{
    u16 vm_mask;

    float vm_latitude;   //纬度
    float vm_longitude;  //经度
    bool position_valid; //位置有效
}ll_info_t;
extern ll_info_t ll_info;

void LLInfoParaRead(void);
void LLInfoParaWrite(void);
void LLInfoParaReset(void);
void LLInfoParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
