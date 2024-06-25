#ifndef __GOODIX_NADT_H
#define __GOODIX_NADT_H

#include <stdint.h>
#ifndef _ARM_MATH_H
#include "goodix_type.h"
#endif
#include "../../Algo_Code/call/inc/goodix_algo.h"

#ifdef __cplusplus
extern "C" {
#endif



extern void NADT_ProcInit(void);
extern void NADT_ProcDeInit(void);
extern GBOOL NADT_Proc(GS32 lPacketInfoArr[], GU8 lResult[]);
extern void NADT_Control(GS32 lOptTye, GS32 lConfigValue[]);
extern GS8* NADT_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif
