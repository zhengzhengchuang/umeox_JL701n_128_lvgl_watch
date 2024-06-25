#ifndef __MASK_H__
#define __MASK_H__

#include "generic/typedef.h"
#include "ui_draw/ui_type.h"
#include "ui_draw/ui_math.h"

typedef u8(*mask_cb)(u8 *mask_buf, int abs_x, int abs_y, int len, void *p);

typedef struct {
    void *param;
    void *custom_id;
} mask_info;

#define MASK_ID_INV       (-1)
#define MASK_MAX_NUM     16
typedef mask_info mask_info_list[MASK_MAX_NUM];

enum {
    MASK_TYPE_LINE,
    MASK_TYPE_ANGLE,
    MASK_TYPE_RADIUS,
    MASK_TYPE_FADE,
    MASK_TYPE_MAP,
};

enum {
    MASK_LINE_SIDE_LEFT = 0,
    MASK_LINE_SIDE_RIGHT,
    MASK_LINE_SIDE_TOP,
    MASK_LINE_SIDE_BOTTOM,
};

enum {
    MASK_RES_TRANSP,
    MASK_RES_FULL_COVER,
    MASK_RES_CHANGED,
    MASK_RES_UNKNOWN
};

typedef struct {
    mask_cb cb;
    u8 type;
} mask_common_dsc_t;

typedef struct {
    mask_common_dsc_t dsc;
    struct {
        struct draw_area rect;
        int radius;
        u8 outer: 1;
    } cfg;
    int y_prev;
    sqrt_res_t y_prev_x;
} mask_radius_param_t;

typedef struct {
    mask_common_dsc_t dsc;
    struct {
        struct draw_point p1;
        struct draw_point p2;
        u8 side : 2;
    } cfg;
    struct draw_point origo;
    int xy_steep;
    int yx_steep;
    int steep;
    int spx;
    u8 flat : 1;
    u8 inv: 1;
} mask_line_param_t;

typedef struct {
    mask_common_dsc_t dsc;
    struct {
        struct draw_point vertex_p;
        int start_angle;
        int end_angle;
    } cfg;
    mask_line_param_t start_line;
    mask_line_param_t end_line;
    u16 delta_deg;
} mask_angle_param_t;


void mask_angle_init(mask_angle_param_t *param, int vertex_x, int vertex_y, int start_angle, int end_angle);
void mask_radius_init(mask_radius_param_t *param, const struct draw_area *rect, int radius, bool inv);
void mask_line_points_init(mask_line_param_t *param, int p1x, int p1y, int p2x, int p2y, u8 side);
void mask_line_angle_init(mask_line_param_t *param, int p1x, int py, int angle, u8 side);

#define OPA_MIN 2
#define OPA_MAX 253
#define MATH_UDIV255(x) ((u32)((u32) (x) * 0x8081) >> 0x17)
AT_UI_RAM
static inline u8 mask_mix(u8 mask_act, u8 mask_new)
{
    if (mask_new >= OPA_MAX) {
        return mask_act;
    }
    if (mask_new <= OPA_MIN) {
        return 0;
    }

#if 1
    return (((u32)mask_act * (u32)mask_new) >> 8);
#else
    return MATH_UDIV255(mask_act * mask_new);// >> 8);
#endif
}

AT_UI_RAM
static inline void *memcpy_small(void *dst, const void *src, u32 len)
{

#if 1
    memcpy(dst, src, len);
    return dst;
#else
    u8 *d8 = (u8 *)dst;
    const u8 *s8 = (const u8 *)src;

    while (len) {
        *d8 = *s8;
        d8++;
        s8++;
        len--;
    }

    return dst;
#endif

}

AT_UI_RAM
inline static void draw_area_copy(struct draw_area *dest, const struct draw_area *src)
{
    memcpy_small(dest, src, sizeof(struct draw_area));
}

AT_UI_RAM
static inline int draw_area_get_width(const struct draw_area *area_p)
{
    return (int)(area_p->x2 - area_p->x1 + 1);
}

AT_UI_RAM
static inline int draw_area_get_height(const struct draw_area *area_p)
{
    return (int)(area_p->y2 - area_p->y1 + 1);
}

AT_UI_RAM
static inline int draw_area_get_size(const struct draw_area *area_p)
{
    return (int)(area_p->x2 - area_p->x1 + 1) * (area_p->y2 - area_p->y1 + 1);
}

#endif

