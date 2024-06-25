#ifndef _LTV_FORMAT_H_
#define _LTV_FORMAT_H_
#include "typedef.h"

typedef struct __JL_ATTR_ITEM {
    u8 size;
    u8 type;
    u8 data[0];
} LTV_ATTR_ITEM;

typedef struct __JL_ATTR_GET {
    u8 type;
    u8(*get_size)(void);
    u8 *(*get_data)(void);
} LTV_ATTR_GET;

u32 ltv_attr_total_len_get(u32 mask, LTV_ATTR_GET *attr_ptr, u8 attr_max);
bool ltv_attr_fill(u32 mask, LTV_ATTR_GET *attr_ptr, u8 attr_max, u8 *fill_buf);

#endif

