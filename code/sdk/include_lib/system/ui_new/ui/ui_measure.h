#ifndef __UI_MEASURE_H__
#define __UI_MEASURE_H__

#include "generic/typedef.h"

#define PORT(i,a,b) (u16)((i<<8)|((a-'A')<<4)|(b))

#if 0
#define IO_FRAME    PORT(0,'A',10)
#define IO_FLASH    PORT(1,'A',11)
#define IO_IMB      PORT(2,'A',12)
#define IO_IMD      PORT(3,'A',13)
#define IO_IMG      PORT(4,'A',14)
#define IO_JPEG     PORT(5,'A',15)
#define IO_CPU      PORT(6,'B',0)
#else
#define IO_FRAME    PORT(0,'A',12)
#define IO_FLASH    PORT(1,'A',13)
#define IO_IMB      PORT(2,'A',14)
#define IO_IMD      PORT(3,'A',15)
#define IO_IMG      PORT(4,'B',0)
#define IO_JPEG     PORT(5,'B',1)
#define IO_CPU      PORT(6,'B',3)
#define IO_MELD     PORT(6,'B',4)
#endif

#define LOW         0
#define HIGH        1

void ui_io_set(u16 io, u8 level);

#endif
