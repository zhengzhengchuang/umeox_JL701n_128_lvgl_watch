#ifndef __DRAW_ARC_H__
#define __DRAW_ARC_H__

#include "typedef.h"
#include "ui/ui_mask.h"
#include "ui/ui_basic.h"


#define COLOR_16_SWAP   0
typedef union {
	struct {
#if COLOR_16_SWAP == 0
		u16 blue : 5;
		u16 green : 6;
		u16 red : 5;
#else
		u16 green_h : 3;
		u16 red : 5;
		u16 blue : 5;
		u16 green_l : 3;
#endif
	} ch;
	u16 full;
} color16_t;

typedef color16_t color_t;

enum {
	BORDER_SIDE_NONE     = 0x00,
	BORDER_SIDE_BOTTOM   = 0x01,
	BORDER_SIDE_TOP      = 0x02,
	BORDER_SIDE_LEFT     = 0x04,
	BORDER_SIDE_RIGHT    = 0x08,
	BORDER_SIDE_FULL     = 0x0F,
	BORDER_SIDE_INTERNAL = 0x10,
	_BORDER_SIDE_LAST
};

enum {
	OPA_TRANSP = 0,
	OPA_COVER  = 255,
};
#define RADIUS_CIRCLE (0x7FFF)

#define COLOR_BLACK 0x0000

#define SPLIT_RADIUS_LIMIT 10
#define SPLIT_ANGLE_GAP_LIMIT 60

typedef struct {
	int radius;
	color_t bg_color;
	u8 bg_opa;
	color_t border_color;
	int border_width;
	int border_side;
	u8 border_opa;
} draw_rect_dsc_t;

typedef struct {
	color_t color;
	int width;
	u8 opa;
	u8 round_start : 1;
	u8 round_end   : 1;
} draw_line_dsc_t;


typedef struct {
	int center_x;
	int center_y;
	int radius;
	u16 start_angle;
	u16 end_angle;
	u16 start_quarter;
	u16 end_quarter;
	int width;
	draw_rect_dsc_t *draw_dsc;
	const area_t *draw_area;
	const area_t *clip_area;
} quarter_draw_dsc_t;


#endif

