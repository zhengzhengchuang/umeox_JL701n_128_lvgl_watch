#include "ui/ui_draw_arc.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "ui/text_matrix.h"

void draw_rectangle(const area_t *coords, const area_t *clip, const draw_rect_dsc_t *dsc);


#if 0
color_t *disp_get_buffer_addr();
u16 disp_get_buffer_size();
u16 disp_get_buffer_line(u16 width);
void disp_set_buffer_rect(struct rect *rect);
void disp_get_buffer_rect(struct rect *rect);
void disp_buffer_flush(struct rect *rect, color_t *buffer);
void disp_buffer_clear(struct rect *rect, color_t color);
void *_mem_buf_get(int size);
void _mem_buf_release(void *buf);
#else

struct disp_param {
	struct lcd_interface *lcd;
	color_t *disp_buffer;
	color_t *disp_buffer_0;
	color_t *disp_buffer_1;
	u16 pixel_len;
	u8 *buf;
	u32 len;
	struct rect disp_rect;
	color_t *line_buffer;
	struct lcd_info info;
};

struct text_matrix_info {
	int font_w;
	int font_h;
	u8 *pixel;
};

static struct disp_param param_t = {0};
#define __this (&param_t)

AT_UI_SIM_RAM
void disp_lcd_init(void *p)
{

	__this->lcd = lcd_get_hdl();

	if (__this->lcd->init) {
		__this->lcd->init(p);
	}

	if (__this->lcd->backlight_ctrl) {
		__this->lcd->backlight_ctrl(true);
	}

	if (__this->lcd->get_screen_info) {
		__this->lcd->get_screen_info(&__this->info);
	}

	if (__this->lcd->buffer_malloc) {
		__this->lcd->buffer_malloc(&__this->buf, &__this->len);
		__this->disp_buffer = (color_t *)__this->buf;
		__this->pixel_len = (__this->len / 4 * 4) / sizeof(color_t) / 2;
		__this->disp_buffer_0 = __this->disp_buffer;
		__this->disp_buffer_1 = &__this->disp_buffer[__this->len];
	}
}

void ui_buffer_init(u8 *buf, u32 len)
{

	__this->lcd = lcd_get_hdl();

	__this->buf = buf;
	__this->len = len;
	__this->disp_buffer = (color_t *)__this->buf;
	__this->pixel_len = (__this->len / 4 * 4) / sizeof(color_t) / 2;
	__this->disp_buffer_0 = __this->disp_buffer;
	__this->disp_buffer_1 = &__this->disp_buffer[__this->pixel_len];

	/* printf("buffer : 0x%x, 0x%x, 0x%x\n", __this->disp_buffer, __this->disp_buffer_0, __this->disp_buffer_1); */
}

AT_UI_SIM_RAM
color_t *disp_get_buffer_addr()
{
	return __this->disp_buffer;
}

AT_UI_SIM_RAM
u16 disp_get_buffer_size()
{
	return __this->pixel_len;
}

AT_UI_SIM_RAM
u16 disp_get_buffer_line(u16 width)
{
	return __this->pixel_len / width;
}

AT_UI_SIM_RAM
void disp_set_buffer_rect(struct rect *rect)
{
	if (__this->disp_buffer == __this->disp_buffer_0) {
		__this->disp_buffer = __this->disp_buffer_1;
	} else {
		__this->disp_buffer = __this->disp_buffer_0;
	}

	/* printf("buffer : 0x%x, 0x%x, 0x%x\n", __this->disp_buffer, __this->disp_buffer_0, __this->disp_buffer_1); */
	memcpy(&__this->disp_rect, rect, sizeof(struct rect));
}

AT_UI_SIM_RAM
void disp_get_buffer_rect(struct rect *rect)
{
	memcpy(rect, &__this->disp_rect, sizeof(struct rect));
}

void lcd_data_copy(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait);
int lcd_qspi_st77903_mode();
AT_UI_SIM_RAM
void disp_buffer_flush(struct rect *rect, color_t *buffer)
{
	if (lcd_qspi_st77903_mode()) {
		lcd_data_copy(0, rect, buffer, rect->width * 2, rect->left, rect->top, rect->width, rect->height, 0);
	} else {
		__this->lcd->set_draw_area(rect->left, rect->left + rect->width - 1, rect->top, rect->top + rect->height - 1);
		__this->lcd->draw((u8 *)buffer, rect->width * rect->height * 2, 0);
	}
}

AT_UI_SIM_RAM
void disp_buffer_clear(struct rect *rect, u16 color)
{
	int w;
	int h;

	if (rect->left < __this->disp_rect.left) {
		rect->left = __this->disp_rect.left;
	}
	if (rect->top < __this->disp_rect.top) {
		rect->top = __this->disp_rect.top;
	}
	if ((rect->left + rect->width) > (__this->disp_rect.left + __this->disp_rect.width)) {
		rect->width = __this->disp_rect.left + __this->disp_rect.width - rect->left;
	}
	if ((rect->top + rect->height) > (__this->disp_rect.top + __this->disp_rect.height)) {
		rect->height = __this->disp_rect.top + __this->disp_rect.height - rect->top;
	}

	/* printf("__this->disp_buffer : 0x%x\n", __this->disp_buffer); */

	for (h = 0; h < rect->height; h++) {
		for (w = 0; w < rect->width; w++) {
			__this->disp_buffer[(rect->top + h - __this->disp_rect.top) * __this->disp_rect.width + (rect->left + w - __this->disp_rect.left)].full = color;
		}
	}
}

AT_UI_SIM_RAM
void *_mem_buf_get(int size)
{
	static color_t line_buffer[480];

	if (!__this->line_buffer) {
		__this->line_buffer = line_buffer;
	}

	/* printf("__this->line_buffer : 0x%x\n", __this->line_buffer); */

	return __this->line_buffer;
}
AT_UI_SIM_RAM
void _mem_buf_release(void *buf)
{

}
AT_UI_SIM_RAM
u16 disp_get_pixel(int x, int y)
{
	u16 color;
	if ((x < __this->disp_rect.left) ||
	    (x > (__this->disp_rect.left + __this->disp_rect.width - 1)) ||
	    (y < __this->disp_rect.top) ||
	    (y > (__this->disp_rect.top + __this->disp_rect.height - 1))) {
		return 0;
	}

	color = __this->disp_buffer[(y - __this->disp_rect.top) * __this->disp_rect.width + (x - __this->disp_rect.left)].full;
	return color;
}
AT_UI_SIM_RAM
void disp_set_pixel(int x, int y, u16 color)
{
	if ((x < __this->disp_rect.left) ||
	    (x > (__this->disp_rect.left + __this->disp_rect.width - 1)) ||
	    (y < __this->disp_rect.top) ||
	    (y > (__this->disp_rect.top + __this->disp_rect.height - 1))) {
		return;
	}
	__this->disp_buffer[(y - __this->disp_rect.top) * __this->disp_rect.width + (x - __this->disp_rect.left)].full = (color >> 8) | (color & 0xff) << 8 ;
}

#endif

static void draw_full_border(const area_t *area_inner, const area_t *area_outer, const area_t *clip,
                             int radius, color_t color, u8 opa, u8 blend_mode);

AT_UI_SIM_RAM
void draw_rect_dsc_init(draw_rect_dsc_t *dsc)
{
	_memset_00(dsc, sizeof(draw_rect_dsc_t));
	dsc->border_color.full = COLOR_BLACK;
	dsc->border_opa = OPA_COVER;
	dsc->border_side = BORDER_SIDE_FULL;
}

AT_UI_SIM_RAM
bool _area_intersect(area_t *res_p, const area_t *a1_p, const area_t *a2_p)
{
	res_p->x1 = MATH_MAX(a1_p->x1, a2_p->x1);
	res_p->y1 = MATH_MAX(a1_p->y1, a2_p->y1);
	res_p->x2 = MATH_MIN(a1_p->x2, a2_p->x2);
	res_p->y2 = MATH_MIN(a1_p->y2, a2_p->y2);

	bool union_ok = true;
	if ((res_p->x1 > res_p->x2) || (res_p->y1 > res_p->y2)) {
		union_ok = false;
	}

	return union_ok;
}


#if 0
AT_UI_SIM_RAM
void blend(u8 *mask_buf, int abs_x, int abs_y, int len, color_t color)
{
	int i;
	color_t mix_color;
	color_t back_color;
	u16 red, green, blue;
	struct rect draw;

#if 0
	for (i = 0; i < len; i++) {
		if (mask_buf[i]) {
			putchar('*');
		} else {
			putchar(' ');
		}
	}
	putchar('\n');
#endif

	/* disp_get_buffer_rect(&draw); */


	for (i = 0; i < len; i++) {
		u8 alpha = mask_buf[i];
		if (alpha) {
			u16 color_p = disp_get_pixel(abs_x + i, abs_y);
			back_color.full = (color_p >> 8) | (color_p & 0xff) << 8;

			red = (back_color.ch.red * (255 - alpha) + color.ch.red * alpha) / 255;
			mix_color.ch.red = (red > 0x1f) ? 0x1f : red;

			green = (back_color.ch.green * (255 - alpha) + color.ch.green * alpha) / 255;
			green = (green > 0x3f) ? 0x3f : green;
			mix_color.ch.green = green;

			blue = (back_color.ch.blue * (255 - alpha) + color.ch.blue * alpha) / 255;
			mix_color.ch.blue = (blue > 0x1f) ? 0x1f : blue;

			disp_set_pixel(abs_x + i, abs_y, mix_color.full);
		}
	}

}
#endif

static bool get_rect_cover(const struct rect *a, const struct rect *b, struct rect *c);

AT_UI_SIM_RAM
void blend(u8 *mask_buf, int abs_x, int abs_y, int len, color_t color)
{
	int i;
	u16 red, green, blue;
	u8 alpha;
	u32 alpha2;
	color_t c;
	struct rect rect;
	struct rect draw;

	rect.left = abs_x;
	rect.top = abs_y;
	rect.width = len;
	rect.height = 1;

	// printf("left = %d, top = %d\n", rect.left, rect.top);
	// printf("width = %d, height = %d\n", rect.width, rect.height);

	// printf("___left = %d, top = %d\n", __this->disp_rect.left, __this->disp_rect.top);
	// printf("___width = %d, height = %d\n", __this->disp_rect.width, __this->disp_rect.height);

	if (!get_rect_cover(&__this->disp_rect, &rect, &draw)) {
		//printf("get_rect_cover\n");
		return;
	}

	// printf("+++left = %d, top = %d\n", draw.left, draw.top);
	// printf("+++width = %d, height = %d\n", draw.width, draw.height);

	/* printf("%d, %d, %d, %d, %d\n", draw.top, disp_rect.top, draw.left, disp_rect.left, disp_rect.width); */

	ASSERT(!memcmp(&rect, &draw, sizeof(struct rect)));

	//user add
	c.full = (color.full & 0xff) << 8 | (color.full >> 8);

	color_t *color_p = &__this->disp_buffer[(draw.top/* abs_y */ - __this->disp_rect.top) * ((__this->disp_rect.width + 1) / 2 * 2) + (draw.left/*abs_x*/ - __this->disp_rect.left)];

	u8 r, g, b;
	r = color.ch.red ;
	g = color.ch.green ;
	b = color.ch.blue ;

	//printf("___________blend\n");
	for (i = 0; i < draw.width/* len */; i++) {
		alpha = mask_buf[i];
		alpha2 = (256 - alpha);

		if (alpha == 0xff) {
			color_p->full = c.full;
		} else if (alpha) {
			color_p->full = (color_p->full & 0xff) << 8 | (color_p->full >> 8);

			red = (color_p->ch.red * alpha2 + r * alpha) >> 8;
			green = (color_p->ch.green * alpha2 + g * alpha) >> 8;
			blue = (color_p->ch.blue * alpha2 + b * alpha) >> 8;

			color_p->full = (green >> 3) | (red << 3) | (blue << 8) | ((green & 0x7) << 13);
		}
		color_p ++;
	}
}


AT_UI_SIM_RAM
static void draw_full_border(const area_t *area_inner, const area_t *area_outer, const area_t *clip,
                             int radius, color_t color, u8 opa, u8 blend_mode)
{
	u8 other_mask_cnt = mask_get_cnt();
	bool simple_mode = true;
	if (other_mask_cnt) {
		simple_mode = false;
	}

	int inner_w = area_get_width(area_inner);
	int inner_h = area_get_height(area_inner);
	int border_width = area_outer->x2 - area_inner->x2;
	int rin = radius;

	int short_side = MATH_MIN(inner_w, inner_h);
	if (rin > short_side >> 1) {
		rin = short_side >> 1;
	}

	int rout = rin + border_width;

	int coords_out_w = area_get_width(area_outer);
	int coords_out_h = area_get_height(area_outer);
	short_side = MATH_MIN(coords_out_w, coords_out_h);
	if (rout > short_side >> 1) {
		rout = short_side >> 1;
	}

	area_t draw_area;
	bool is_common;
	is_common = _area_intersect(&draw_area, area_outer, clip);//�ж�ˢ�������Ƿ�����Բ�ཻ
	if (is_common == false) {
		return;
	}

	int draw_area_w = area_get_width(&draw_area);

	mask_radius_param_t mask_rin_param;
	mask_radius_init(&mask_rin_param, area_inner, rin, true);//��Բ
	int mask_rin_id = mask_add(&mask_rin_param, NULL);

	mask_radius_param_t mask_rout_param;
	mask_radius_init(&mask_rout_param, area_outer, rout, false);//��Բ
	int mask_rout_id = mask_add(&mask_rout_param, NULL);

	u8 *mask_buf = _mem_buf_get(draw_area_w);

	int corner_size = MATH_MAX(rout, border_width - 1);

	int h;
	u8 mask_res;

	if (0/* simple_mode */) {
		int upper_corner_end = area_outer->y1 + corner_size;
		for (h = draw_area.y1; h <= upper_corner_end; h++) {
			_memset_ff(mask_buf, draw_area_w);
			mask_res = mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
			blend(mask_buf, draw_area.x1, h, draw_area_w, color);
		}

		int lower_corner_end = area_outer->y2 - corner_size;
		if (lower_corner_end <= upper_corner_end) {
			lower_corner_end = upper_corner_end + 1;
		}

		for (h = lower_corner_end; h <= draw_area.y2; h++) {
			_memset_ff(mask_buf, draw_area_w);
			mask_res = mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
			blend(mask_buf, draw_area.x1, h, draw_area_w, color);
		}
	} else {
		for (h = draw_area.y1; h <= draw_area.y2; h++) {
			_memset_ff(mask_buf, draw_area_w);
			mask_res = mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
			blend(mask_buf, draw_area.x1, h, draw_area_w, color);
		}
	}

	mask_remove_id(mask_rin_id);
	mask_remove_id(mask_rout_id);
	_mem_buf_release(mask_buf);
}

AT_UI_SIM_RAM
static void draw_quarter_0(quarter_draw_dsc_t *q)
{
	area_t quarter_area;

	if (q->start_quarter == 0 && q->end_quarter == 0 && q->start_angle < q->end_angle) {
		quarter_area.y1 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);
		quarter_area.x2 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius)) >> TRIGO_SHIFT);

		quarter_area.y2 = q->center_y + ((_trigo_sin(q->end_angle) * q->radius) >> TRIGO_SHIFT);
		quarter_area.x1 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	} else if (q->start_quarter == 0 || q->end_quarter == 0) {
		if (q->start_quarter == 0) {
			quarter_area.x1 = q->center_x;
			quarter_area.y2 = q->center_y + q->radius;

			quarter_area.y1 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);
			quarter_area.x2 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
		if (q->end_quarter == 0) {
			quarter_area.x2 = q->center_x + q->radius;
			quarter_area.y1 = q->center_y;

			quarter_area.y2 = q->center_y + ((_trigo_sin(q->end_angle) * q->radius) >> TRIGO_SHIFT);
			quarter_area.x1 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
	} else if ((q->start_quarter == q->end_quarter && q->start_quarter != 0 && q->end_angle < q->start_angle) ||
	           (q->start_quarter == 2 && q->end_quarter == 1) ||
	           (q->start_quarter == 3 && q->end_quarter == 2) ||
	           (q->start_quarter == 3 && q->end_quarter == 1)) {
		quarter_area.x1 = q->center_x;
		quarter_area.y1 = q->center_y;
		quarter_area.x2 = q->center_x + q->radius;
		quarter_area.y2 = q->center_y + q->radius;

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	}
}
AT_UI_SIM_RAM
static void draw_quarter_1(quarter_draw_dsc_t *q)
{
	area_t quarter_area;

	if (q->start_quarter == 1 && q->end_quarter == 1 && q->start_angle < q->end_angle) {
		quarter_area.y2 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius)) >> TRIGO_SHIFT);
		quarter_area.x2 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);

		quarter_area.y1 = q->center_y + ((_trigo_sin(q->end_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);
		quarter_area.x1 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius)) >> TRIGO_SHIFT);

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	} else if (q->start_quarter == 1 || q->end_quarter == 1) {
		if (q->start_quarter == 1) {
			quarter_area.x1 = q->center_x - q->radius;
			quarter_area.y1 = q->center_y;

			quarter_area.y2 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius)) >> TRIGO_SHIFT);
			quarter_area.x2 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
		if (q->end_quarter == 1) {
			quarter_area.x2 = q->center_x - 1;
			quarter_area.y2 = q->center_y + q->radius;

			quarter_area.y1 = q->center_y + ((_trigo_sin(q->end_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);
			quarter_area.x1 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
	} else if ((q->start_quarter == q->end_quarter && q->start_quarter != 1 && q->end_angle < q->start_angle) ||
	           (q->start_quarter == 0 && q->end_quarter == 2) ||
	           (q->start_quarter == 0 && q->end_quarter == 3) ||
	           (q->start_quarter == 3 && q->end_quarter == 2)) {
		quarter_area.x1 = q->center_x - q->radius;
		quarter_area.y1 = q->center_y;
		quarter_area.x2 = q->center_x - 1;
		quarter_area.y2 = q->center_y + q->radius;

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	}
}
AT_UI_SIM_RAM
static void draw_quarter_2(quarter_draw_dsc_t *q)
{
	area_t quarter_area;

	if (q->start_quarter == 2 && q->end_quarter == 2 && q->start_angle < q->end_angle) {
		quarter_area.x1 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius)) >> TRIGO_SHIFT);
		quarter_area.y2 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);

		quarter_area.y1 = q->center_y + ((_trigo_sin(q->end_angle) * q->radius) >> TRIGO_SHIFT);
		quarter_area.x2 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	} else if (q->start_quarter == 2 || q->end_quarter == 2) {
		if (q->start_quarter == 2) {
			quarter_area.x2 = q->center_x - 1;
			quarter_area.y1 = q->center_y - q->radius;

			quarter_area.x1 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius)) >> TRIGO_SHIFT);
			quarter_area.y2 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
		if (q->end_quarter == 2) {
			quarter_area.x1 = q->center_x - q->radius;
			quarter_area.y2 = q->center_y - 1;

			quarter_area.x2 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);
			quarter_area.y1 = q->center_y + ((_trigo_sin(q->end_angle) * (q->radius)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
	} else if ((q->start_quarter == q->end_quarter && q->start_quarter != 2 && q->end_angle < q->start_angle) ||
	           (q->start_quarter == 0 && q->end_quarter == 3) ||
	           (q->start_quarter == 1 && q->end_quarter == 3) ||
	           (q->start_quarter == 1 && q->end_quarter == 0)) {
		quarter_area.x1 = q->center_x - q->radius;
		quarter_area.y1 = q->center_y - q->radius;
		quarter_area.x2 = q->center_x - 1;
		quarter_area.y2 = q->center_y - 1;

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	}
}

AT_UI_SIM_RAM
static void draw_quarter_3(quarter_draw_dsc_t *q)
{
	area_t quarter_area;

	if (q->start_quarter == 3 && q->end_quarter == 3 && q->start_angle < q->end_angle) {
		quarter_area.x1 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);
		quarter_area.y1 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius)) >> TRIGO_SHIFT);

		quarter_area.x2 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius)) >> TRIGO_SHIFT);
		quarter_area.y2 = q->center_y + ((_trigo_sin(q->end_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	} else if (q->start_quarter == 3 || q->end_quarter == 3) {
		if (q->start_quarter == 3) {
			quarter_area.x2 = q->center_x + q->radius;
			quarter_area.y2 = q->center_y - 1;

			quarter_area.x1 = q->center_x + ((_trigo_sin(q->start_angle + 90) * (q->radius - q->width)) >> TRIGO_SHIFT);
			quarter_area.y1 = q->center_y + ((_trigo_sin(q->start_angle) * (q->radius)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
		if (q->end_quarter == 3) {
			quarter_area.x1 = q->center_x;
			quarter_area.y1 = q->center_y - q->radius;

			quarter_area.x2 = q->center_x + ((_trigo_sin(q->end_angle + 90) * (q->radius)) >> TRIGO_SHIFT);
			quarter_area.y2 = q->center_y + ((_trigo_sin(q->end_angle) * (q->radius - q->width)) >> TRIGO_SHIFT);

			bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
			if (ok) {
				draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
			}
		}
	} else if ((q->start_quarter == q->end_quarter && q->start_quarter != 3 && q->end_angle < q->start_angle) ||
	           (q->start_quarter == 2 && q->end_quarter == 0) ||
	           (q->start_quarter == 1 && q->end_quarter == 0) ||
	           (q->start_quarter == 2 && q->end_quarter == 1)) {
		quarter_area.x1 = q->center_x;
		quarter_area.y1 = q->center_y - q->radius;
		quarter_area.x2 = q->center_x + q->radius;
		quarter_area.y2 = q->center_y - 1;

		bool ok = _area_intersect(&quarter_area, &quarter_area, q->clip_area);
		if (ok) {
			draw_rectangle(q->draw_area, &quarter_area, q->draw_dsc);
		}
	}
}

AT_UI_SIM_RAM
static void get_rounded_area(int angle, int radius, u8 tickness, area_t *res_area)
{
	const u8 ps = 8;
	const u8 pa = 127;

	int thick_half = tickness / 2;
	u8 thick_corr = (tickness & 0x01) ? 0 : 1;

	int rx_corr;
	int ry_corr;

	if (angle > 90 && angle < 270) {
		rx_corr = 0;
	} else {
		rx_corr = 0;
	}

	if (angle > 0 && angle < 180) {
		ry_corr = 0;
	} else {
		ry_corr = 0;
	}

	int cir_x;
	int cir_y;

	cir_x = ((radius - rx_corr - thick_half) * _trigo_sin(90 - angle)) >> (TRIGO_SHIFT - ps);
	cir_y = ((radius - ry_corr - thick_half) * _trigo_sin(angle)) >> (TRIGO_SHIFT - ps);

	if (cir_x > 0) {
		cir_x = (cir_x - pa) >> ps;
		res_area->x1 = cir_x - thick_half + thick_corr;
		res_area->x2 = cir_x + thick_half;
	} else {
		cir_x = (cir_x + pa) >> ps;
		res_area->x1 = cir_x - thick_half;
		res_area->x2 = cir_x + thick_half - thick_corr;
	}

	if (cir_y > 0) {
		cir_y = (cir_y - pa) >> ps;
		res_area->y1 = cir_y - thick_half + thick_corr;
		res_area->y2 = cir_y + thick_half;
	} else {
		cir_y = (cir_y + pa) >> ps;
		res_area->y1 = cir_y - thick_half;
		res_area->y2 = cir_y + thick_half - thick_corr;
	}
}

AT_UI_SIM_RAM
void draw_spec_arc(int center_x, int center_y, u16 radius,  u16 start_angle, u16 end_angle,
                   const area_t *clip_area, const draw_line_dsc_t *dsc)
{
	if (dsc->opa <= OPA_MIN) {
		return;
	}
	if (dsc->width == 0) {
		return;
	}
	if (start_angle == end_angle) {
		return;
	}

	int width = dsc->width;
	if (width > radius) {
		width = radius;
	}

	draw_rect_dsc_t cir_dsc;
	draw_rect_dsc_init(&cir_dsc);
	cir_dsc.radius = RADIUS_CIRCLE;
	cir_dsc.bg_opa = OPA_TRANSP;
	cir_dsc.border_opa = dsc->opa;
	cir_dsc.border_color = dsc->color;
	cir_dsc.border_width = width;

	area_t area;
	area.x1 = center_x - radius;
	area.y1 = center_y - radius;
	area.x2 = center_x + radius - 1;
	area.y2 = center_y + radius - 1;

	if (start_angle + 360 == end_angle || start_angle == end_angle + 360) {
		draw_rectangle(&area, clip_area, &cir_dsc);
		return;
	}

	if (start_angle >= 360) {
		start_angle -= 360;
	}
	if (end_angle >= 360) {
		end_angle -= 360;
	}

	mask_angle_param_t mask_angle_param;
	mask_angle_init(&mask_angle_param, center_x, center_y, start_angle, end_angle);

	int mask_angle_id = mask_add(&mask_angle_param, NULL);

	int angle_gap;
	if (end_angle > start_angle) {
		angle_gap = 360 - (end_angle - start_angle);
	} else {
		angle_gap = start_angle - end_angle;
	}
	if (angle_gap > SPLIT_ANGLE_GAP_LIMIT && radius > SPLIT_RADIUS_LIMIT) {
		quarter_draw_dsc_t q_dsc;
		q_dsc.center_x = center_x;
		q_dsc.center_y = center_y;
		q_dsc.radius = radius;
		q_dsc.start_angle = start_angle;
		q_dsc.end_angle = end_angle;
		q_dsc.start_quarter = (start_angle / 90) & 0x3;
		q_dsc.end_quarter = (end_angle / 90) & 0x3;
		q_dsc.width = width;
		q_dsc.draw_dsc =  &cir_dsc;
		q_dsc.draw_area = &area;
		q_dsc.clip_area = clip_area;

		draw_quarter_0(&q_dsc);
		draw_quarter_1(&q_dsc);
		draw_quarter_2(&q_dsc);
		draw_quarter_3(&q_dsc);
	} else {
		draw_rectangle(&area, clip_area, &cir_dsc);
	}
	mask_remove_id(mask_angle_id);

	if (dsc->round_start || dsc->round_end) {
		draw_rect_dsc_t cir_dsc_backup;
		_memcpy_small(&cir_dsc_backup, &cir_dsc, sizeof(draw_rect_dsc_t));

		cir_dsc_backup.bg_color      = dsc->color;
		cir_dsc_backup.bg_opa        = dsc->opa;
		cir_dsc_backup.border_width  = 0;

		area_t round_area;
		if (dsc->round_start) {
			get_rounded_area(start_angle, radius, width, &round_area);
			round_area.x1 += center_x;
			round_area.x2 += center_x;
			round_area.y1 += center_y;
			round_area.y2 += center_y;

			draw_rectangle(&round_area, clip_area, &cir_dsc_backup);
		}

		if (dsc->round_end) {
			get_rounded_area(end_angle, radius, width, &round_area);
			round_area.x1 += center_x;
			round_area.x2 += center_x;
			round_area.y1 += center_y;
			round_area.y2 += center_y;

			draw_rectangle(&round_area, clip_area, &cir_dsc_backup);
		}
	}
}

AT_UI_SIM_RAM
static void draw_border(const area_t *coords, const area_t *clip,
                        const draw_rect_dsc_t *dsc)
{
	if (dsc->border_opa <= OPA_MIN) {
		return;
	}
	if (dsc->border_width == 0) {
		return;
	}
	if (dsc->border_side == BORDER_SIDE_NONE) {
		return;
	}

	int coords_w = area_get_width(coords);
	int coords_h = area_get_height(coords);

	/* printf("dsc : 0x%x\n", dsc); */
	/* printf("dsc->radius : %d\n", dsc->radius); */
	int rout = dsc->radius;
	int short_side = MATH_MIN(coords_w, coords_h);
	if (rout > short_side >> 1) {
		rout = short_side >> 1;
	}
	area_t area_inner;
	area_copy(&area_inner, coords);
	area_inner.x1 += ((dsc->border_side & BORDER_SIDE_LEFT) ? dsc->border_width : - (dsc->border_width + rout));
	area_inner.x2 -= ((dsc->border_side & BORDER_SIDE_RIGHT) ? dsc->border_width : - (dsc->border_width + rout));
	area_inner.y1 += ((dsc->border_side & BORDER_SIDE_TOP) ? dsc->border_width : - (dsc->border_width + rout));
	area_inner.y2 -= ((dsc->border_side & BORDER_SIDE_BOTTOM) ? dsc->border_width : - (dsc->border_width + rout));
	if (dsc->border_side == BORDER_SIDE_FULL) {
		draw_full_border(&area_inner, coords, clip, dsc->radius, dsc->border_color, dsc->border_opa, 0);
	} else {
		u8 opa = dsc->border_opa;
		if (opa > OPA_MAX) {
			opa = OPA_COVER;
		}

		area_t draw_area;
		bool is_common;
		is_common = _area_intersect(&draw_area, coords, clip);
		if (is_common == false) {
			return;
		}

		int draw_area_w = area_get_width(&draw_area);

		u8 *mask_buf = _mem_buf_get(draw_area_w);

		int mask_rout_id = MASK_ID_INV;
		mask_radius_param_t mask_rout_param;
		if (rout > 0) {
			mask_radius_init(&mask_rout_param, coords, rout, false);
			mask_rout_id = mask_add(&mask_rout_param, NULL);
		}

		int rin = rout - dsc->border_width;
		if (rin < 0) {
			rin = 0;
		}
		mask_radius_param_t mask_rin_param;
		mask_radius_init(&mask_rin_param, &area_inner, rout - dsc->border_width, true);
		int mask_rin_id = mask_add(&mask_rin_param, NULL);

		int corner_size = MATH_MAX(rout, dsc->border_width - 1);

		int h;
		area_t fill_area;

		fill_area.x1 = coords->x1;
		fill_area.x2 = coords->x2;
		fill_area.y1 = draw_area.y1;
		fill_area.y2 = fill_area.y1;

		u32 buf_ofs = 0;
		if (dsc->border_side == BORDER_SIDE_LEFT) {
			fill_area.x2 = coords->x1 + corner_size;
		} else if (dsc->border_side == BORDER_SIDE_RIGHT) {
			fill_area.x1 = coords->x2 - corner_size;
			buf_ofs = fill_area.x1 - coords->x1;
		}

		volatile bool top_only = false;
		volatile bool bottom_only = false;
		if (dsc->border_side == BORDER_SIDE_TOP) {
			top_only = true;
		}
		if (dsc->border_side == BORDER_SIDE_BOTTOM) {
			bottom_only = true;
		}
		if (dsc->border_side == (BORDER_SIDE_TOP | BORDER_SIDE_BOTTOM)) {
			top_only = true;
			bottom_only = true;
		}

		volatile bool normal = !top_only && !bottom_only ? true : false;

		for (h = draw_area.y1; h <= draw_area.y2; h++) {
			if (normal ||
			    (top_only && fill_area.y1 <= coords->y1 + corner_size) ||
			    (bottom_only && fill_area.y1 >= coords->y2 - corner_size)) {
				_memset_ff(mask_buf, draw_area_w);
				u8 mask_res = mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
				blend(mask_buf, draw_area.x1, h, draw_area_w, dsc->bg_color);
			}
			fill_area.y1++;
			fill_area.y2++;

		}
		mask_remove_id(mask_rin_id);
		mask_remove_id(mask_rout_id);
		_mem_buf_release(mask_buf);
	}
}

AT_UI_SIM_RAM
static void draw_bg(const area_t *coords, const area_t *clip,
                    const draw_rect_dsc_t *dsc)
{
	printf("_____draw_bg\n");

	if (dsc->bg_opa <= OPA_MIN) {
		return;
	}

	area_t coords_bg;
	area_copy(&coords_bg, coords);


	u8 opa = dsc->bg_opa;
	if (opa > OPA_MAX) {
		opa = OPA_COVER;
	}

	area_t draw_area;
	bool is_common;
	is_common = _area_intersect(&draw_area, &coords_bg, clip);
	if (is_common == false) {
		return;
	}
	
	// printf("x1 = %d, x2 = %d\n", draw_area.x1, draw_area.x2);
	// printf("y1 = %d, y2 = %d\n", draw_area.y1, draw_area.y2);

	int draw_area_w = area_get_width(&draw_area);
	int draw_area_h = area_get_height(&draw_area);

	u16 other_mask_cnt = mask_get_cnt();
	bool simple_mode = true;
	if (other_mask_cnt) {
		simple_mode = false;
	}

	int mask_rout_id = MASK_ID_INV;
	int coords_w = area_get_width(&coords_bg);
	int coords_h = area_get_height(&coords_bg);

	int rout = dsc->radius;
	int short_side = MATH_MIN(coords_w, coords_h);
	if (rout > short_side >> 1) {
		rout = short_side >> 1;
	}

	mask_radius_param_t mask_rout_param;
	if (rout > 0) {
		mask_radius_init(&mask_rout_param, &coords_bg, rout, false);
		mask_rout_id = mask_add(&mask_rout_param, NULL);
	}

	//printf("++++++++radius end\n");

	static u8 mask_buf[480];

	int h;
	u8 mask_res = MASK_RES_FULL_COVER;
	for (h = draw_area.y1; h <= draw_area.y2; h++) {
		_memset(mask_buf, opa, draw_area_w);
		mask_res = mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
		blend(mask_buf, draw_area.x1, h, draw_area_w, dsc->bg_color);
	}

	mask_remove_id(mask_rout_id);
	_mem_buf_release(mask_buf);
}

AT_UI_SIM_RAM
void draw_rectangle(const area_t *coords, const area_t *clip, const draw_rect_dsc_t *dsc)
{
	if (dsc->border_width == 0) {
		draw_bg(coords, clip, dsc);
	} else {
		draw_border(coords, clip, dsc); \
	}
}


#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define rect_left(r)        ((r)->left)
#define rect_top(r)         ((r)->top)
#define rect_right(r)       ((r)->left + (r)->width)
#define rect_bottom(r)      ((r)->top + (r)->height)

AT_UI_SIM_RAM
static bool get_rect_cover(const struct rect *a, const struct rect *b, struct rect *c)
{
	int right, bottom;

	c->top = MAX(a->top, b->top);
	c->left = MAX(a->left, b->left);
	right = MIN(rect_right(a), rect_right(b));
	bottom = MIN(rect_bottom(a), rect_bottom(b));
	if ((c->top < bottom) && (c->left < right)) {
		c->width = right - c->left;
		c->height = bottom - c->top;
		return true;
	}
	// c->top = 260
	//c->left = 54
	//right = 315
	//bottom = 1

	// left = 54, top = 260
	// width = 261, height = 1
	// ___left = 0, top = 304
	// ___width = 368, height = 144
	//get_rect_cover

	return false;
}


AT_UI_SIM_RAM
int get_text_matrix_info(struct text_matrix_info *info, u16 code)
{
	int i;

	for (i = 0; i < sizeof(text_matrix) / sizeof(text_matrix[0]); i++) {
		if (code == text_matrix[i].code) {
			info->font_w = text_matrix[i].width;
			info->font_h = text_matrix[i].height;
			info->pixel = &osd_str_matrix[text_matrix[i].offset];
			return 0;
		}
	}

	return -1;
}


AT_UI_SIM_RAM
int get_text_matrix_width(u16 code)
{
	int i;

	for (i = 0; i < sizeof(text_matrix) / sizeof(text_matrix[0]); i++) {
		if (code == text_matrix[i].code) {
			return text_matrix[i].width;
		}
	}

	return -1;
}


AT_UI_SIM_RAM
int get_text_matrix_height(u16 code)
{
	int i;

	for (i = 0; i < sizeof(text_matrix) / sizeof(text_matrix[0]); i++) {
		if (code == text_matrix[i].code) {
			return text_matrix[i].height;
		}
	}

	return -1;
}


AT_UI_SIM_RAM
void draw_text(struct rect *draw, int x, int y, u8 *str, int strlen, u16 color)
{
	struct rect cover;
	struct rect font_rect;
	struct text_matrix_info info;
	u8 *pixel;
	int w;
	int h;
	int n;
	u16 text;

	font_rect.left = x;
	font_rect.top = y;

	for (n = 0; n < strlen;) {
		if (str[n] & BIT(7)) {
			text = (u16)(str[n] << 8) | str[n + 1];
			//printf("text:0x%x\n",text);
			n += 2;
		} else {
			text = str[n];
			n++;
		}

		if (get_text_matrix_info(&info, text)) {
//            printf("get_text_matrix_info err");
			continue;
		}
		font_rect.width = info.font_w;
		font_rect.height = info.font_h;
		pixel = info.pixel;
        //printf("font_rect(%d %d %d %d)",font_rect.left,font_rect.top,font_rect.width,font_rect.height);
		if (get_rect_cover(draw, &font_rect, &cover)) {
			for (h = 0; h < cover.height; h++) {
				for (w = 0; w < cover.width; w++) {
					int x = cover.left + w - font_rect.left;
					int y = cover.top + h - font_rect.top;

					if (pixel[y * ((info.font_w + 7) / 8) + x / 8] & BIT(7 - x % 8)) {
						disp_set_pixel(cover.left + w, cover.top + h, color);
					}
				}
			}
		}

		font_rect.left += info.font_w;
	}
}


void put_buf(const u8 *buf, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		if (i && (i % 0x10 == 0)) {
			printf(" \n");
		}
		printf("%02x ", buf[i]);
	}
	printf(" \n");
}

void get_text_rect(u8 *str, int len, int *width, int  *height)
{
	int i;
	int w, h;
	int string_w;
	int string_h;
	u16 code;
	//printf("str : %s\n", str);
	//put_buf(str, len);

	string_w = 0;
	string_h = 0;
	for (i = 0; i < len; i++) {
		if (str[i] & BIT(7)) {
			code = str[i] << 8 | str[i + 1];
			i++;
		} else {
			code = str[i];
		}
		w = get_text_matrix_width(code);
		h = get_text_matrix_height(code);
		string_w += w;
		if (string_h < h) {
			string_h = h;
		}
		//printf("0x%x, %d x %d\n", code, w, h);
	}

	*width = string_w;
	*height = string_h;
}


enum FILE_ENCODE {
    FILE_ENCODE_UNKNOWN,
    FILE_ENCODE_GBK,
    FILE_ENCODE_UTF8,
};

static enum FILE_ENCODE file_encoding_detect()
{
    static const u8 *text_encode_test = "��";

    if(strlen(text_encode_test) == 3) {
        if((text_encode_test[0] == 0xe5) && (text_encode_test[1] == 0x8d) && (text_encode_test[2] == 0x87)) {
            return FILE_ENCODE_UTF8;
        }
    } else if (strlen(text_encode_test) == 2) {
        if((text_encode_test[0] == 0xc9) && (text_encode_test[1] == 0xfd)) {
            return FILE_ENCODE_GBK;
        }
    } else {
        return FILE_ENCODE_UNKNOWN;
    }

    return FILE_ENCODE_UNKNOWN;
}


AT_UI_SIM_RAM
void draw_ring(int center_x, int center_y, int radius, u16 color, int percent)
{
	u16 start_angle;
	u16 end_angle;
	area_t clip_area;
	struct rect draw;
	struct rect rect;
	draw_line_dsc_t dsc;
	int i;

	if (percent < 0) {
		percent += 100;
	}
	if (percent > 100) {
		percent -= 100;
	}

	dsc.width = 15;
	dsc.opa = 255;
	dsc.round_start = 1;
	dsc.round_end = 1;

	rect.left = center_x - radius;
	rect.top = center_y - radius;
	rect.width = radius * 2;
	rect.height = radius * 2;

	memcpy(&draw, &rect, sizeof(struct rect));

	/* printf("rect (%d, %d, %d, %d)\n", rect.left, rect.left + rect.width - 1, rect.top, rect.top + rect.height - 1); */
	int block = disp_get_buffer_line(rect.width);
	/* printf("block : %d\n", block); */
	for (i = 0; i < (rect.height + block - 1) / block; i++) {
		draw.top = rect.top + block * i;
		draw.height = (rect.top + rect.height - draw.top) > block ? block : (rect.top + rect.height - draw.top);

		disp_set_buffer_rect(&draw);
		disp_buffer_clear(&draw, 0x0000);

		clip_area.x1 = draw.left;
		clip_area.x2 = draw.left + draw.width - 1;
		clip_area.y1 = draw.top;
		clip_area.y2 = draw.top + draw.height - 1;

		/* printf("(%d, %d, %d, %d)\n", clip_area.x1, clip_area.x2, clip_area.y1, clip_area.y2); */

		start_angle = 270;
		end_angle = 270 + 360;
		dsc.color.ch.red = 160 >> 3;
		dsc.color.ch.green = 160 >> 2;
		dsc.color.ch.blue = 160 >> 3;
		/* dsc.color.full = 43540; */
		draw_spec_arc(center_x, center_y, radius, start_angle, end_angle, &clip_area, &dsc);

		u16 other_mask_cnt = mask_get_cnt();
		ASSERT(other_mask_cnt == 0);
		start_angle = 270;
		end_angle = 270 + (360) * percent / 100;
		dsc.color.full = color;
		draw_spec_arc(center_x, center_y, radius, start_angle, end_angle, &clip_area, &dsc);

//user add
#if 1
        enum FILE_ENCODE file_encode = file_encoding_detect();
        ASSERT(file_encode == FILE_ENCODE_GBK, ", file_encode : %d\n", file_encode);

		const char *text_zh = "������...";
		const char *text_zh1 = "�벻Ҫ�ϵ�!";
		const char *text_zh2 = "�������";
		const char *text_zh3 = "������������";
		const char *text_zh4 = "��������:";
		u8 num_buf[32] = {0};
		int text_width = 0;
		int text_height = 0;
		int line_height = 0;
		int interval = 8;

		memcpy(num_buf, text_zh4, strlen(text_zh4));
		sprintf(&num_buf[strlen(text_zh4)], "%d%%", percent);

		if (percent < 100) { //������
			get_text_rect(text_zh, strlen(text_zh), &text_width, &text_height);
			//printf("%s , %d x %d\n", text_zh, text_width, text_height);
			line_height = text_height + interval;
			draw_text(&draw, rect.left + (rect.width - text_width) / 2, rect.top + (rect.height - line_height * 3) / 2, text_zh, strlen(text_zh), 0xf800);

			get_text_rect(text_zh1, strlen(text_zh1), &text_width, &text_height);
			//printf("%s , %d x %d\n", text_zh, text_width, text_height);
			line_height = text_height + interval;
			draw_text(&draw, rect.left + (rect.width - text_width) / 2, rect.top + (rect.height - line_height * 3) / 2 + line_height, text_zh1, strlen(text_zh1), 0xf800);

			get_text_rect(num_buf, strlen(num_buf), &text_width, &text_height);
			//printf("%s , %d x %d\n", num_buf, text_width, text_height);
			line_height = text_height + interval;
			draw_text(&draw, rect.left + (rect.width - text_width) / 2, rect.top + (rect.height - line_height * 3) / 2 + line_height * 2, num_buf, strlen(num_buf), 0x07e0);

		} else { // �������
			get_text_rect(text_zh2, strlen(text_zh2), &text_width, &text_height);
			//printf("%s , %d x %d\n", text_zh2, text_width, text_height);
			line_height = text_height + interval;
			draw_text(&draw, rect.left + (rect.width - text_width) / 2, rect.top + (rect.height - line_height * 3) / 2, text_zh2, strlen(text_zh2), 0xf800);

			get_text_rect(text_zh3, strlen(text_zh3), &text_width, &text_height);
			//printf("%s , %d x %d\n", text_zh3, text_width, text_height);
			line_height = text_height + interval;
			draw_text(&draw, rect.left + (rect.width - text_width) / 2, rect.top + (rect.height - line_height * 3) / 2 + line_height, text_zh3, strlen(text_zh3), 0xf800);

			get_text_rect(num_buf, strlen(num_buf), &text_width, &text_height);
			//printf("%s , %d x %d\n", num_buf, text_width, text_height);
			line_height = text_height + interval;
			draw_text(&draw, rect.left + (rect.width - text_width) / 2, rect.top + (rect.height - line_height * 3) / 2 + line_height * 2, num_buf, strlen(num_buf), 0x07e0);
		}
#endif
		disp_buffer_flush(&draw, disp_get_buffer_addr());
	}
}

AT_UI_SIM_RAM
void ui_draw_ring(int center_x, int center_y, int radius, u16 color, u16 bg_color,int percent)
{
	u16 start_angle;
	u16 end_angle;
	area_t clip_area;
	struct rect draw;
	struct rect rect;
	draw_line_dsc_t dsc;
	int i;

	if (percent < 0) {
		percent += 100;
	}
	if (percent > 100) {
		percent -= 100;
	}

	dsc.width = 15;
	dsc.opa = 255;
	dsc.round_start = 1;
	dsc.round_end = 1;

	rect.left = center_x - radius;
	rect.top = center_y - radius;
	rect.width = radius * 2;
	rect.height = radius * 2;

	memcpy(&draw, &rect, sizeof(struct rect));

	/* printf("rect (%d, %d, %d, %d)\n", rect.left, rect.left + rect.width - 1, rect.top, rect.top + rect.height - 1); */
	int block = disp_get_buffer_line(rect.width);
	/* printf("block : %d\n", block); */
	for (i = 0; i < (rect.height + block - 1) / block; i++) {
		draw.top = rect.top + block * i;
		draw.height = (rect.top + rect.height - draw.top) > block ? block : (rect.top + rect.height - draw.top);

		disp_set_buffer_rect(&draw);
		disp_buffer_clear(&draw, 0x0000);

		clip_area.x1 = draw.left;
		clip_area.x2 = draw.left + draw.width - 1;
		clip_area.y1 = draw.top;
		clip_area.y2 = draw.top + draw.height - 1;

		/* printf("(%d, %d, %d, %d)\n", clip_area.x1, clip_area.x2, clip_area.y1, clip_area.y2); */

		start_angle = 270;
		end_angle = 270 + 360;
		dsc.color.ch.red = (bg_color >> 11)&0x1f;
		dsc.color.ch.green = (bg_color >> 5)&0x3f;
		dsc.color.ch.blue = (bg_color & 0x1f);
		/* dsc.color.full = 43540; */
		draw_spec_arc(center_x, center_y, radius, start_angle, end_angle, &clip_area, &dsc);

		u16 other_mask_cnt = mask_get_cnt();
		ASSERT(other_mask_cnt == 0);
		start_angle = 270;
		end_angle = 270 + (360) * percent / 100;
		dsc.color.full = color;
		draw_spec_arc(center_x, center_y, radius, start_angle, end_angle, &clip_area, &dsc);

		disp_buffer_flush(&draw, disp_get_buffer_addr());
	}
}

AT_UI_SIM_RAM
void ui_draw_bar(int percent)
{
	area_t bg_bar = {
		.x1 = 40,
		.y1 = 309,
		.x2 = 327,
		.y2 = 328,
	};
	area_t clip = {
		.x1 = 40,
		.y1 = 309,
		.x2 = 327,
		.y2 = 328,
	};

	draw_rect_dsc_t bg_dsc = {
		.radius = 10,
		.bg_color = {
			.full = 0x3143,
		},
		.bg_opa = 255,

		.border_width = 0,
	};

	draw_rect_dsc_t fg_dsc = {
		.radius = 10,
		.bg_color = {
			.full = 0xF6D2,
		},
		.bg_opa = 255,

		.border_width = 0,
	};

	struct rect rect;
	struct rect draw;

	rect.left = bg_bar.x1;
	rect.top = bg_bar.y1;
	rect.width = area_get_width(&bg_bar);
	rect.height = area_get_height(&bg_bar);

	memcpy(&draw, &rect, sizeof(struct rect));

	disp_set_buffer_rect(&draw);
	disp_buffer_clear(&draw, 0x0000);

	draw_rectangle(&bg_bar, &clip, &bg_dsc);

	if(percent <= 0)
		goto __flush;
		
	percent = percent>100?100:percent;

	area_t fg_bar;
	memcpy(&fg_bar, &bg_bar, sizeof(struct rect));
	fg_bar.x2 = (int)(bg_bar.x1 + (percent*(rect.width)*(1.0f))/100);

	draw_rectangle(&fg_bar, &clip, &fg_dsc);

__flush:
	disp_buffer_flush(&draw, disp_get_buffer_addr());

	return;
}

#if 0
AT_UI_SIM_RAM
void draw_bar(int percent)
{
	area_t coords = {
		.x1 = 143,
		.y1 = 103,
		.x2 = 336,
		.y2 = 115,
	};
	area_t clip = {
		.x1 = 143,
		.y1 = 103,
		.x2 = 336,
		.y2 = 115,
	};
	draw_rect_dsc_t dsc = {
		.radius = 6,//32767,
		.bg_color = {
			/* .ch = { */
			/* .blue = 27, */
			/* .green = 53, */
			/* .red = 26, */
			/* }, */
			.full = 0x001f,//54971,
		},
		.bg_opa = 255,

		.border_width = 0,
		.border_side = BORDER_SIDE_NONE,//15,
		.border_opa = 0,//255,
	};

	printf("draw bar\n");
	draw_rectangle(&coords, &clip, &dsc);

	area_t coords_t;
	memcpy(&coords_t, &coords, sizeof(area_t));
	coords_t.x2 = coords.x1 + (coords.x2 - coords.x1 + 1 - (coords.y2 - coords.y1)) * percent / 100 + (coords.y2 - coords.y1) - 1;
	dsc.bg_color.full = 0xf800;
	if (coords_t.x2 > coords.x1) {
		draw_rectangle(&coords_t, &clip, &dsc);
	}
}
#endif