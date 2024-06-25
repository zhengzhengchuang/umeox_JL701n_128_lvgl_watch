#ifndef __BASIC_H__
#define __BASIC_H__

#include "typedef.h"

void _memset(void *dst, u8 v, u32 len);
void _memset_ff(void *dst, u32 len);
void _memset_00(void *dst, u32 len);

#define ALIGN_MASK  0x3

#endif

