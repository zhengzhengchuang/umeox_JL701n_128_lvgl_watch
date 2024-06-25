#ifndef UI_OBJ_H
#define UI_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
*      INCLUDES
*********************/
// #include <stddef.h>
// #include "cpu.h"
// #include "generic/typedef.h"
#include "ui_draw/ui_type.h"
#include "ui_draw/ui_basic.h"
#include "ui_draw/ui_mask.h"




/*********************
*      DEFINES
*********************/
#define UI_USE_LINEMETER			1
#define UI_USE_ARBIRECT				1
#define UI_USE_LINE					1
#define UI_USE_POLY					1


#define UI_RADIUS_CIRCLE (0x7FFF)

#define ATTRIBUTE_FAST_MEM		AT_UI_RAM





/**********************
*      TYPEDEFS
**********************/
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef int16_t ui_coord_t;
typedef int16_t ui_style_int_t;
typedef color_t ui_color_t;
typedef uint8_t ui_opa_t;
typedef uint8_t ui_grad_dir_t;
typedef uint8_t ui_blend_mode_t;
typedef struct draw_area ui_area_t;
typedef struct draw_point ui_point_t;
typedef uint8_t ui_draw_mask_res_t;

typedef struct _ui_style_list_t {
    ui_style_int_t scale_width;//刻度线的长度,大于等于10
    ui_color_t line_color;//活跃刻度线起始颜色,画线时表示线的颜色
    ui_color_t scale_grad_color;//活跃刻度线终止颜色
    ui_color_t scale_end_color;//非活跃刻度线颜色
    ui_style_int_t line_width;//刻度线的宽度,大于等于6,画线时表示线宽，最小可以为1
} ui_style_list_t;


struct _ui_obj_t;


#define _COLOR_MAKE_TYPE_HELPER (ui_color_t)

# define COLOR_SET_R16(c, v) (c).ch.red = (uint8_t)((v) & 0x1FU)
#if COLOR_16_SWAP == 0
# define COLOR_SET_G16(c, v) (c).ch.green = (uint8_t)((v) & 0x3FU)
#else
# define COLOR_SET_G16(c, v) {(c).ch.green_h = (uint8_t)(((v) >> 3) & 0x7); (c).ch.green_l = (uint8_t)((v) & 0x7);}
#endif
# define COLOR_SET_B16(c, v) (c).ch.blue = (uint8_t)((v) & 0x1FU)
# define COLOR_SET_A16(c, v) do {} while(0)

# define COLOR_GET_R16(c) (c).ch.red
#if COLOR_16_SWAP == 0
# define COLOR_GET_G16(c) (c).ch.green
#else
# define COLOR_GET_G16(c) (((c).ch.green_h << 3) + (c).ch.green_l)
#endif
# define COLOR_GET_B16(c) (c).ch.blue
# define COLOR_GET_A16(c) 0xFF

#if COLOR_16_SWAP == 0
# define _COLOR_ZERO_INITIALIZER16 {{0x00, 0x00, 0x00}}
# define COLOR_MAKE16(r8, g8, b8) (_COLOR_MAKE_TYPE_HELPER{{(uint8_t)((b8 >> 3) & 0x1FU), (uint8_t)((g8 >> 2) & 0x3FU), (uint8_t)((r8 >> 3) & 0x1FU)}})
#else
# define _COLOR_ZERO_INITIALIZER16 {{0x00, 0x00, 0x00, 0x00}}
# define COLOR_MAKE16(r8, g8, b8) (_COLOR_MAKE_TYPE_HELPER{{(uint8_t)((g8 >> 5) & 0x7U), (uint8_t)((r8 >> 3) & 0x1FU), (uint8_t)((b8 >> 3) & 0x1FU), (uint8_t)((g8 >> 2) &     0x7U)}})
#endif


# define COLOR_SET_R32(c, v) (c).ch.red = (uint8_t)((v) & 0xFF)
# define COLOR_SET_G32(c, v) (c).ch.green = (uint8_t)((v) & 0xFF)
# define COLOR_SET_B32(c, v) (c).ch.blue = (uint8_t)((v) & 0xFF)
# define COLOR_SET_A32(c, v) (c).ch.alpha = (uint8_t)((v) & 0xFF)

# define COLOR_GET_R32(c) (c).ch.red
# define COLOR_GET_G32(c) (c).ch.green
# define COLOR_GET_B32(c) (c).ch.blue
# define COLOR_GET_A32(c) (c).ch.alpha

# define _COLOR_ZERO_INITIALIZER32 {{0x00, 0x00, 0x00, 0x00}}
# define COLOR_MAKE32(r8, g8, b8) (_COLOR_MAKE_TYPE_HELPER{{b8, g8, r8, 0xff}}) /*Fix 0xff alpha*/


#define COLOR_SET_R(c, v) COLOR_SET_R16(c, v)
#define COLOR_SET_G(c, v) COLOR_SET_G16(c, v)
#define COLOR_SET_B(c, v) COLOR_SET_B16(c, v)
#define COLOR_SET_A(c, v) COLOR_SET_A16(c, v)

#define COLOR_GET_R(c) COLOR_GET_R16(c)
#define COLOR_GET_G(c) COLOR_GET_G16(c)
#define COLOR_GET_B(c) COLOR_GET_B16(c)
#define COLOR_GET_A(c) COLOR_GET_A16(c)



#define _COLOR_ZERO_INITIALIZER _COLOR_ZERO_INITIALIZER16
#define COLOR_MAKE(r8, g8, b8) COLOR_MAKE16(r8, g8, b8)


#define UI_COLOR_WHITE   COLOR_MAKE(0xFF, 0xFF, 0xFF)
#define UI_COLOR_SILVER  COLOR_MAKE(0xC0, 0xC0, 0xC0)
#define UI_COLOR_GRAY    COLOR_MAKE(0x80, 0x80, 0x80)
#define UI_COLOR_BLACK   COLOR_MAKE(0x00, 0x00, 0x00)
#define UI_COLOR_RED     COLOR_MAKE(0xFF, 0x00, 0x00)
#define UI_COLOR_MAROON  COLOR_MAKE(0x80, 0x00, 0x00)
#define UI_COLOR_YELLOW  COLOR_MAKE(0xFF, 0xFF, 0x00)
#define UI_COLOR_OLIVE   COLOR_MAKE(0x80, 0x80, 0x00)
#define UI_COLOR_LIME    COLOR_MAKE(0x00, 0xFF, 0x00)
#define UI_COLOR_GREEN   COLOR_MAKE(0x00, 0x80, 0x00)
#define UI_COLOR_CYAN    COLOR_MAKE(0x00, 0xFF, 0xFF)
#define UI_COLOR_AQUA    UI_COLOR_CYAN
#define UI_COLOR_TEAL    COLOR_MAKE(0x00, 0x80, 0x80)
#define UI_COLOR_BLUE    COLOR_MAKE(0x00, 0x00, 0xFF)
#define UI_COLOR_NAVY    COLOR_MAKE(0x00, 0x00, 0x80)
#define UI_COLOR_MAGENTA COLOR_MAKE(0xFF, 0x00, 0xFF)
#define UI_COLOR_PURPLE  COLOR_MAKE(0x80, 0x00, 0x80)
#define UI_COLOR_ORANGE  COLOR_MAKE(0xFF, 0xA5, 0x00)


#define UI_COLOR_LZ  COLOR_MAKE(0x80, 0x80, 0xFF)
#define UI_COLOR_LB  COLOR_MAKE(0x00, 0xFF, 0xFF)
#define UI_COLOR_ZI  COLOR_MAKE(0xFF, 0x00, 0xFF)



#ifndef COLOR_MIX_ROUND_OFS

#define COLOR_MIX_ROUND_OFS 128

// #if COLOR_DEPTH == 32
// #define COLOR_MIX_ROUND_OFS 0
// #else
// #define COLOR_MIX_ROUND_OFS 128
// #endif
#endif

ATTRIBUTE_FAST_MEM static inline ui_color_t ui_color_mix(ui_color_t c1, ui_color_t c2, uint8_t mix)
{
    ui_color_t ret;
    COLOR_SET_R(ret, MATH_UDIV255((uint16_t) COLOR_GET_R(c1) * mix + COLOR_GET_R(c2) *
                                  (255 - mix) + COLOR_MIX_ROUND_OFS));
    COLOR_SET_G(ret, MATH_UDIV255((uint16_t) COLOR_GET_G(c1) * mix + COLOR_GET_G(c2) *
                                  (255 - mix) + COLOR_MIX_ROUND_OFS));
    COLOR_SET_B(ret, MATH_UDIV255((uint16_t) COLOR_GET_B(c1) * mix + COLOR_GET_B(c2) *
                                  (255 - mix) + COLOR_MIX_ROUND_OFS));
    COLOR_SET_A(ret, 0xFF);

    return ret;
}


/*To avoid overflow don't let the max ranges (reduce with 1000) */
#define UI_COORD_MAX ((ui_coord_t)((uint32_t)((uint32_t)1 << (8 * sizeof(ui_coord_t) - 1)) - 1000))
#define UI_COORD_MIN (-UI_COORD_MAX)



enum {
    UI_BORDER_SIDE_NONE     = 0x00,
    UI_BORDER_SIDE_BOTTOM   = 0x01,
    UI_BORDER_SIDE_TOP      = 0x02,
    UI_BORDER_SIDE_LEFT     = 0x04,
    UI_BORDER_SIDE_RIGHT    = 0x08,
    UI_BORDER_SIDE_FULL     = 0x0F,
    UI_BORDER_SIDE_INTERNAL = 0x10,
    _UI_BORDER_SIDE_LAST
};


enum {
    UI_GRAD_DIR_NONE,
    UI_GRAD_DIR_VER,
    UI_GRAD_DIR_HOR,
    _UI_GRAD_DIR_LAST
};



enum {
    UI_OPA_TRANSP = 0,
    UI_OPA_0      = 0,
    UI_OPA_10     = 25,
    UI_OPA_20     = 51,
    UI_OPA_30     = 76,
    UI_OPA_40     = 102,
    UI_OPA_50     = 127,
    UI_OPA_60     = 153,
    UI_OPA_70     = 178,
    UI_OPA_80     = 204,
    UI_OPA_90     = 229,
    UI_OPA_100    = 255,
    UI_OPA_COVER  = 255,
};


enum {
    UI_STATE_DEFAULT   =  0x00,
    UI_STATE_CHECKED  =  0x01,
    UI_STATE_FOCUSED  =  0x02,
    UI_STATE_EDITED   =  0x04,
    UI_STATE_HOVERED  =  0x08,
    UI_STATE_PRESSED  =  0x10,
    UI_STATE_DISABLED =  0x20,
};
typedef uint8_t ui_state_t;

typedef struct _ui_obj_t {
    ui_area_t coords; /**< Coordinates of the object (x1, y1, x2, y2)*/
    void *ext_attr;             /**< Object type specific extended data*/

    int ext_draw_pad; /**< EXTend the size in every direction for drawing. */

    ui_state_t state;

    ui_style_list_t style;
} ui_obj_t;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UI_OBJ_H*/



