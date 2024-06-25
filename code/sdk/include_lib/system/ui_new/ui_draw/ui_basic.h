#ifndef __BASIC_H__
#define __BASIC_H__

#include "generic/typedef.h"
#include "generic/rect.h"

void memset_fast(void *dst, u8 v, u32 len);
void memset_ff(void *dst, u32 len);
void memset_00(void *dst, u32 len);

#define ALIGN_MASK  0x3


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

typedef union {
    struct {
        u16 blue : 5;
        u16 green : 6;
        u16 red : 5;
    } ch;
    u16 full;
} color16_l;

typedef union {
    struct {
        u16 green_h : 3;
        u16 red : 5;
        u16 blue : 5;
        u16 green_l : 3;
    } ch;
    u16 full;
} color16_b;

typedef union {
    struct {
        u8 blue;
        u8 green;
        u8 red;
        u8 alpha;
    } ch;
    u32 full;
} color32_t;


typedef color16_t color_t;
typedef color16_l color_l;
typedef color16_b color_b;

void disp_set_buffer_rect(struct rect *rect);
void disp_get_buffer_rect(struct rect *rect);
void *disp_line_buf_malloc(int size);
void disp_line_buf_release(void *buf);
color_t *disp_get_pixel(int x, int y);
void disp_set_pixel(int x, int y, color_t *color_p);
color_t *disp_get_buffer_addr();
u16 disp_get_buffer_size();
u16 disp_get_buffer_line(u16 width);
void disp_buffer_clear(struct rect *rect, color_t color);
void disp_buffer_flush(struct rect *rect, color_t *buffer);
void disp_set_buffer_addr(u16 *buf, int buf_size);

int ui_disp_line_buf_init();
int ui_disp_line_buf_uninit();

#endif

