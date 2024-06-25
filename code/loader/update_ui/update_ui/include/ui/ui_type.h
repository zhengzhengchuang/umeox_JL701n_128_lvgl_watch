#ifndef __TYPE_H__
#define __TYPE_H__

#include "typedef.h"

#define INT32_MAX 2147483647
#define INT32_MIN (-INT32_MAX - 1)

struct rect {
	int left;
	int top;
	int width;
	int height;
};

typedef struct {
	int x1;
	int y1;
	int x2;
	int y2;
} area_t;

typedef struct {
	int x;
	int y;
} point_t;


#define AT_UI_SIM_RAM  AT(.volatile_ram_code)

#endif

