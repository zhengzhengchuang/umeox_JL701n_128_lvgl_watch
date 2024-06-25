#ifndef __COMMON_RDEC_H__
#define __COMMON_RDEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    Rdec_Forward,
    Rdec_Backward,
};

void common_rdec_msg_handle(int state);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
