#ifndef __GOMORE_FUNC_H__
#define __GOMORE_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define GalgoKeyLen (64)

typedef struct
{
    u16 vm_mask;

    u8 algo_key[GalgoKeyLen];
}GMalgokeyInfo_t;
extern GMalgokeyInfo_t Galgo_info;

void GalgoInfoParaRead(void);
void GalgoInfoParaWrite(void);
void GalgoInfoParaReset(void);
void GalgoInfoParaUpdate(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
