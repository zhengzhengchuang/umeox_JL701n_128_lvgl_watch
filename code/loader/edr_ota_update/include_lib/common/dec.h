#ifndef  __DEC_H__
#define  __DEC_H__

#include "common.h"


void doe(u16 k, void *pBuf, u32 lenIn, u32 addr);

void decode(u16 key, u8 *buf, u32 len, u32 offset);

u8 dec_isd_cfg_ini(const char *cfg, void *value, u8 *ptr);


#endif
