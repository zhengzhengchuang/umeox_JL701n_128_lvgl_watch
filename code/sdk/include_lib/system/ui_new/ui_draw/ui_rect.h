#ifndef __DRAW_RECT_H__
#define __DRAW_RECT_H__

#include "generic/typedef.h"
#include "ui_draw/ui_basic.h"
#include "ui_draw/ui_mask.h"

#define COLOR_BLACK 0x0000

enum {
    OPA_TRANSP = 0,
    OPA_COVER  = 255,
};

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

typedef struct {
    int draw_id;
    int radius;
    color_t bg_color;
    u8 bg_opa;
    u8 alpha;
    color_t border_color;
    int border_width;
    int border_side;
    u8 border_opa;
} draw_rect_dsc_t;

typedef struct {
    int draw_id;
    color_t color;
    int width;
    u8 alpha;
    u8 opa;
    u8 round_start : 1;
    u8 round_end   : 1;
} draw_line_dsc_t;

#endif

