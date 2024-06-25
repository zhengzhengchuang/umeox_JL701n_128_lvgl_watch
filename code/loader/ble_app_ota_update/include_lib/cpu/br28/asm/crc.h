#ifndef  __CRC_H__
#define  __CRC_H__

#include "common.h"
#include "crc16.h"

u16 chip_crc16(void *ptr, u32 len);
u16 chip_crc16_with_init(void *ptr, u32 len, u16 init);

#endif

