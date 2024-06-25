#ifndef __TYPE_H__
#define __TYPE_H__

#include "generic/typedef.h"

#define INT32_MAX 2147483647
#define INT32_MIN (-INT32_MAX - 1)

#define RGB565(r,g,b) (((((u8)r)>>3)<<11)|((((u8)g)>>2)<<5)|(((u8)b)>>3))

struct draw_area {
    int x1;
    int y1;
    int x2;
    int y2;
};

struct draw_point {
    int x;
    int y;
};


#endif

