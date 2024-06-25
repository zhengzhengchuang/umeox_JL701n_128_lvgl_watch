#include "ui/ui_mask.h"
#include "ui/ui_basic.h"

_mask_saved_arr_t _mask_list;

static u8 mask_radius(u8 *mask_buf, int abs_x, int abs_y, int len, mask_radius_param_t *p);
static u8 mask_angle(u8 *mask_buf, int abs_x, int abs_y, int len, mask_angle_param_t *p);
static u8 mask_line(u8 *mask_buf, int abs_x, int abs_y, int len, mask_line_param_t *p);
static u8 line_mask_steep(u8 *mask_buf, int abs_x, int abs_y, int len, mask_line_param_t *p);
static u8 line_mask_flat(u8 *mask_buf, int abs_x, int abs_y, int len, mask_line_param_t *p);

AT_UI_SIM_RAM
void mask_angle_init(mask_angle_param_t *param, int vertex_x, int vertex_y, int start_angle, int end_angle)
{
	u8 start_side;
	u8 end_side;

	if (start_angle < 0) {
		start_angle = 0;
	} else if (start_angle > 359) {
		start_angle = 359;
	}

	if (end_angle < 0) {
		end_angle = 0;
	} else if (end_angle > 359) {
		end_angle = 359;
	}

	if (end_angle < start_angle) {
		param->delta_deg = 360 - start_angle + end_angle;
	} else {
		param->delta_deg = MATH_ABS(end_angle - start_angle);
	}

	param->cfg.start_angle = start_angle;
	param->cfg.end_angle = end_angle;
	param->cfg.vertex_p.x = vertex_x;
	param->cfg.vertex_p.y = vertex_y;
	param->dsc.cb = (mask_cb)mask_angle;
	param->dsc.type = MASK_TYPE_ANGLE;

	if (start_angle >= 0 && start_angle < 180) {
		start_side = MASK_LINE_SIDE_LEFT;
	} else if (start_angle >= 180 && start_angle < 360) {
		start_side = MASK_LINE_SIDE_RIGHT;
	} else {
		printf("Unexpected start_angle = %d\n", start_angle);
		return;
	}

	if (end_angle >= 0 && end_angle < 180) {
		end_side = MASK_LINE_SIDE_RIGHT;
	} else if (end_angle >= 180 && end_angle < 360) {
		end_side = MASK_LINE_SIDE_LEFT;
	} else {
		printf("Unexpected end_angle = %d\n", end_angle);
		return;
	}

	mask_line_angle_init(&param->start_line, vertex_x, vertex_y, start_angle, start_side);
	mask_line_angle_init(&param->end_line, vertex_x, vertex_y, end_angle, end_side);
}
AT_UI_SIM_RAM
void mask_radius_init(mask_radius_param_t *param, const area_t *rect, int radius, bool inv)
{
	int w = area_get_width(rect);
	int h = area_get_height(rect);
	int short_side = MATH_MIN(w, h);
	if (radius > short_side >> 1) {
		radius = short_side >> 1;
	}

	area_copy(&param->cfg.rect, rect);
	param->cfg.radius = radius;
	param->cfg.outer = inv ? 1 : 0;
	param->dsc.cb = (mask_cb)mask_radius;
	param->dsc.type = MASK_TYPE_RADIUS;
	param->y_prev = INT32_MIN;
	param->y_prev_x.f = 0;
	param->y_prev_x.i = 0;
}

AT_UI_SIM_RAM
void mask_line_points_init(mask_line_param_t *param, int p1x, int p1y, int p2x, int p2y, u8 side)
{
	_memset_00(param, sizeof(mask_line_param_t));

	if (p1y > p2y) {
		int t;
		t = p2x;
		p2x = p1x;
		p1x = t;

		t = p2y;
		p2y = p1y;
		p1y = t;
	}

	param->cfg.p1.x = p1x;
	param->cfg.p1.y = p1y;
	param->cfg.p2.x = p2x;
	param->cfg.p2.y = p2y;
	param->cfg.side = side;

	param->origo.x = p1x;
	param->origo.y = p1y;
	param->flat = (MATH_ABS(p2x - p1x) > MATH_ABS(p2y - p1y)) ? 1 : 0;
	param->yx_steep = 0;
	param->xy_steep = 0;
	param->dsc.cb = (mask_cb)mask_line;
	param->dsc.type = MASK_TYPE_LINE;

	int dx = p2x - p1x;
	int dy = p2y - p1y;

	if (param->flat) {
		int m;

		if (dx) {
			m = (1 << 20) / dx;
			param->yx_steep = (m * dy) >> 10;
		}

		if (dy) {
			m = (1 << 20) / dy;
			param->xy_steep = (m * dx) >> 10;
		}
		param->steep = param->yx_steep;
	} else {
		int m;

		if (dy) {
			m = (1 << 20) / dy;
			param->xy_steep = (m * dx) >> 10;
		}

		if (dx) {
			m = (1 << 20) / dx;
			param->yx_steep = (m * dy) >> 10;
		}
		param->steep = param->xy_steep;
	}

	if (param->cfg.side == MASK_LINE_SIDE_LEFT) {
		param->inv = 0;
	} else if (param->cfg.side == MASK_LINE_SIDE_RIGHT) {
		param->inv = 1;
	} else if (param->cfg.side == MASK_LINE_SIDE_TOP) {
		if (param->steep > 0) {
			param->inv = 1;
		} else {
			param->inv = 0;
		}
	} else if (param->cfg.side == MASK_LINE_SIDE_BOTTOM) {
		if (param->steep > 0) {
			param->inv = 0;
		} else {
			param->inv = 1;
		}
	}

	param->spx = param->steep >> 2;
	if (param->steep < 0) {
		param->spx = -param->spx;
	}
}
AT_UI_SIM_RAM
void mask_line_angle_init(mask_line_param_t *param, int p1x, int py, int angle, u8 side)
{
	int p2x;
	int p2y;

	if (angle > 180) {
		angle -= 180;
	}

	p2x = (_trigo_sin(angle + 90) >> 5) + p1x;
	p2y = (_trigo_sin(angle) >> 5) + py;

	mask_line_points_init(param, p1x, py, p2x, p2y, side);
}
AT_UI_SIM_RAM
static u8 mask_radius(u8 *mask_buf, int abs_x, int abs_y, int len, mask_radius_param_t *p)
{
	bool outer = p->cfg.outer;
	int radius = p->cfg.radius;
	area_t rect;
	area_copy(&rect, &p->cfg.rect);


	if (outer == false) {
		if (abs_y < rect.y1 || abs_y > rect.y2) {
			return MASK_RES_TRANSP;
		}
	} else {
		if (abs_y < rect.y1 || abs_y > rect.y2) {
			return MASK_RES_FULL_COVER;
		}
	}

	if ((abs_x >= rect.x1 + radius && abs_x + len <= rect.x2 - radius) ||
	    (abs_y >= rect.y1 + radius && abs_y <= rect.y2 - radius)) {
		if (outer == false) {
			int last =  rect.x1 - abs_x;
			if (last > len) {
				return MASK_RES_TRANSP;
			}
			if (last >= 0) {
				_memset_00(&mask_buf[0], last);
			}

			int first = rect.x2 - abs_x + 1;
			if (first <= 0) {
				return MASK_RES_TRANSP;
			} else if (first < len) {
				_memset_00(&mask_buf[first], len - first);
			}
			if (last == 0 && first == len) {
				return MASK_RES_FULL_COVER;
			} else {
				return MASK_RES_CHANGED;
			}
		} else {
			int first = rect.x1 - abs_x;
			if (first < 0) {
				first = 0;
			}
			if (first <= len) {
				int last =  rect.x2 - abs_x - first + 1;
				if (first + last > len) {
					last = len - first;
				}
				if (last >= 0) {
					_memset_00(&mask_buf[first], last);
				}
			}
		}
		return MASK_RES_CHANGED;
	}

	int k = rect.x1 - abs_x;
	int w = area_get_width(&rect);
	int h = area_get_height(&rect);
	abs_x -= rect.x1;
	abs_y -= rect.y1;

	u32 r2 = p->cfg.radius * p->cfg.radius;

	if (abs_y < radius || abs_y > h - radius - 1) {

		u32 sqrt_mask;
		if (radius <= 32) {
			sqrt_mask = 0x200;
		}
		if (radius <= 256) {
			sqrt_mask = 0x800;
		} else {
			sqrt_mask = 0x8000;
		}

		sqrt_res_t x0;
		sqrt_res_t x1;
		int y;
		if (abs_y < radius) {
			y = radius - abs_y;

			if (y == p->y_prev) {
				x0.f = p->y_prev_x.f;
				x0.i = p->y_prev_x.i;
			} else {
				_sqrt(r2 - (y * y), &x0, sqrt_mask);
			}
			_sqrt(r2 - ((y - 1) * (y - 1)), &x1, sqrt_mask);
			p->y_prev = y - 1;
			p->y_prev_x.f = x1.f;
			p->y_prev_x.i = x1.i;
		} else {
			y = radius - (h - abs_y) + 1;

			if ((y - 1) == p->y_prev) {
				x1.f = p->y_prev_x.f;
				x1.i = p->y_prev_x.i;
			} else {
				_sqrt(r2 - ((y - 1) * (y - 1)), &x1, sqrt_mask);
			}

			_sqrt(r2 - (y * y), &x0, sqrt_mask);
			p->y_prev = y;
			p->y_prev_x.f = x0.f;
			p->y_prev_x.i = x0.i;
		}

		if (x0.i == x1.i - 1 && x1.f == 0) {
			x1.i--;
			x1.f = 0xFF;
		}

		if (x0.i == x1.i) {
			u8 m = (x0.f + x1.f) >> 1;
			if (outer) {
				m = 255 - m;
			}
			int ofs = radius - x0.i - 1;

			int kl = k + ofs;

			if (kl >= 0 && kl < len) {
				mask_buf[kl] = mask_mix(mask_buf[kl], m);
			}

			int kr = k + (w - ofs - 1);
			if (kr >= 0 && kr < len) {
				mask_buf[kr] = mask_mix(mask_buf[kr], m);
			}

			if (outer == false) {
				kr++;
				if (kl > len)  {
					return MASK_RES_TRANSP;
				}
				if (kl >= 0) {
					_memset_00(&mask_buf[0], kl);
				}
				if (kr < 0) {
					return MASK_RES_TRANSP;
				}
				if (kr <= len) {
					_memset_00(&mask_buf[kr], len - kr);
				}
			} else {
				kl++;
				int first = kl;
				if (first < 0) {
					first = 0;
				}

				int len_tmp = kr - first;
				if (len_tmp + first > len) {
					len_tmp = len - first;
				}
				if (first < len && len_tmp >= 0) {
					_memset_00(&mask_buf[first], len_tmp);
				}
			}
		} else {
			int ofs = radius - (x0.i + 1);
			int kl = k + ofs;
			int kr = k + (w - ofs - 1);

			if (outer) {
				int first = kl + 1;
				if (first < 0) {
					first = 0;
				}

				int len_tmp = kr - first;
				if (len_tmp + first > len) {
					len_tmp = len - first;
				}
				if (first < len && len_tmp >= 0) {
					_memset_00(&mask_buf[first], len_tmp);
				}
			}

			u32 i = x0.i + 1;
			u8 m;
			sqrt_res_t y_prev;
			sqrt_res_t y_next;

			_sqrt(r2 - (x0.i * x0.i), &y_prev, sqrt_mask);

			if (y_prev.f == 0) {
				y_prev.i--;
				y_prev.f = 0xFF;
			}

			if (y_prev.i >= y) {
				_sqrt(r2 - (i * i), &y_next, sqrt_mask);
				m = 255 - (((255 - x0.f) * (255 - y_next.f)) >> 9);

				if (outer) {
					m = 255 - m;
				}
				if (kl >= 0 && kl < len) {
					mask_buf[kl] = mask_mix(mask_buf[kl], m);
				}
				if (kr >= 0 && kr < len) {
					mask_buf[kr] = mask_mix(mask_buf[kr], m);
				}
				kl--;
				kr++;
				y_prev.f = y_next.f;
				i++;
			}

			for (; i <= x1.i; i++) {
				sqrt_approx(&y_next, &y_prev, r2 - (i * i));

				m = (y_prev.f + y_next.f) >> 1;
				if (outer) {
					m = 255 - m;
				}
				if (kl >= 0 && kl < len) {
					mask_buf[kl] = mask_mix(mask_buf[kl], m);
				}
				if (kr >= 0 && kr < len) {
					mask_buf[kr] = mask_mix(mask_buf[kr], m);
				}
				kl--;
				kr++;
				y_prev.f = y_next.f;
			}

			if (y_prev.f) {
				m = (y_prev.f * x1.f) >> 9;
				if (outer) {
					m = 255 - m;
				}
				if (kl >= 0 && kl < len) {
					mask_buf[kl] = mask_mix(mask_buf[kl], m);
				}
				if (kr >= 0 && kr < len) {
					mask_buf[kr] = mask_mix(mask_buf[kr], m);
				}
				kl--;
				kr++;
			}

			if (outer == 0) {
				kl++;
				if (kl > len) {
					return MASK_RES_TRANSP;
				}
				if (kl >= 0) {
					_memset_00(&mask_buf[0], kl);
				}

				if (kr < 0) {
					return MASK_RES_TRANSP;
				}
				if (kr < len) {
					_memset_00(&mask_buf[kr], len - kr);
				}
			}
		}
	}

	return MASK_RES_CHANGED;
}

AT_UI_SIM_RAM
static u8 mask_angle(u8 *mask_buf, int abs_x, int abs_y, int len, mask_angle_param_t *p)
{
	int rel_y = abs_y - p->cfg.vertex_p.y;
	int rel_x = abs_x - p->cfg.vertex_p.x;

	if (p->cfg.start_angle < 180 && p->cfg.end_angle < 180 &&
	    p->cfg.start_angle != 0  && p->cfg.end_angle != 0 &&
	    p->cfg.start_angle > p->cfg.end_angle) {

		if (abs_y < p->cfg.vertex_p.y) {
			return MASK_RES_FULL_COVER;
		}

		int end_angle_first = (rel_y * p->end_line.xy_steep) >> 10;
		int start_angle_last = ((rel_y + 1) * p->start_line.xy_steep) >> 10;

		if (p->cfg.start_angle > 270 && p->cfg.start_angle <= 359 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.start_angle > 0 && p->cfg.start_angle <= 90 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.start_angle > 90 && p->cfg.start_angle < 270 && start_angle_last > 0) {
			start_angle_last = 0;
		}

		if (p->cfg.end_angle > 270 && p->cfg.end_angle <= 359 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.end_angle > 0 &&   p->cfg.end_angle <= 90 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.end_angle > 90 &&  p->cfg.end_angle < 270 && start_angle_last > 0) {
			start_angle_last = 0;
		}


		int dist = (end_angle_first - start_angle_last) >> 1;

		u8 res1 = MASK_RES_FULL_COVER;
		u8 res2 = MASK_RES_FULL_COVER;

		int tmp = start_angle_last + dist - rel_x;
		if (tmp > len) {
			tmp = len;
		}
		if (tmp > 0) {
			res1 = mask_line(&mask_buf[0], abs_x, abs_y, tmp, &p->start_line);
			if (res1 == MASK_RES_TRANSP) {
				_memset_00(&mask_buf[0], tmp);
			}
		}

		if (tmp > len) {
			tmp = len;
		}
		if (tmp < 0) {
			tmp = 0;
		}
		res2 = mask_line(&mask_buf[tmp], abs_x + tmp, abs_y, len - tmp, &p->end_line);
		if (res2 == MASK_RES_TRANSP) {
			_memset_00(&mask_buf[tmp], len - tmp);
		}
		if (res1 == res2) {
			return res1;
		} else {
			return MASK_RES_CHANGED;
		}
	} else if (p->cfg.start_angle > 180 && p->cfg.end_angle > 180 && p->cfg.start_angle > p->cfg.end_angle) {

		if (abs_y > p->cfg.vertex_p.y) {
			return MASK_RES_FULL_COVER;
		}

		int end_angle_first = (rel_y * p->end_line.xy_steep) >> 10;
		int start_angle_last = ((rel_y + 1) * p->start_line.xy_steep) >> 10;

		if (p->cfg.start_angle > 270 && p->cfg.start_angle <= 359 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.start_angle > 0 && p->cfg.start_angle <= 90 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.start_angle > 90 && p->cfg.start_angle < 270 && start_angle_last > 0) {
			start_angle_last = 0;
		}

		if (p->cfg.end_angle > 270 && p->cfg.end_angle <= 359 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.end_angle > 0 &&   p->cfg.end_angle <= 90 && start_angle_last < 0) {
			start_angle_last = 0;
		} else if (p->cfg.end_angle > 90 &&  p->cfg.end_angle < 270 && start_angle_last > 0) {
			start_angle_last = 0;
		}

		int dist = (end_angle_first - start_angle_last) >> 1;

		u8 res1 = MASK_RES_FULL_COVER;
		u8 res2 = MASK_RES_FULL_COVER;

		int tmp = start_angle_last + dist - rel_x;
		if (tmp > len) {
			tmp = len;
		}
		if (tmp > 0) {
			res1 = mask_line(&mask_buf[0], abs_x, abs_y, tmp, (mask_line_param_t *)&p->end_line);
			if (res1 == MASK_RES_TRANSP) {
				_memset_00(&mask_buf[0], tmp);
			}
		}

		if (tmp > len) {
			tmp = len;
		}
		if (tmp < 0) {
			tmp = 0;
		}
		res2 = mask_line(&mask_buf[tmp], abs_x + tmp, abs_y, len - tmp, (mask_line_param_t *)&p->start_line);
		if (res2 == MASK_RES_TRANSP) {
			_memset_00(&mask_buf[tmp], len - tmp);
		}
		if (res1 == res2) {
			return res1;
		} else {
			return MASK_RES_CHANGED;
		}
	} else  {

		u8 res1 = MASK_RES_FULL_COVER;
		u8 res2 = MASK_RES_FULL_COVER;

		if (p->cfg.start_angle == 180) {
			if (abs_y < p->cfg.vertex_p.y) {
				res1 = MASK_RES_FULL_COVER;
			} else {
				res1 = MASK_RES_UNKNOWN;
			}
		} else if (p->cfg.start_angle == 0) {
			if (abs_y < p->cfg.vertex_p.y) {
				res1 = MASK_RES_UNKNOWN;
			} else {
				res1 = MASK_RES_FULL_COVER;
			}
		} else if ((p->cfg.start_angle < 180 && abs_y < p->cfg.vertex_p.y) ||
		           (p->cfg.start_angle > 180 && abs_y >= p->cfg.vertex_p.y)) {
			res1 = MASK_RES_UNKNOWN;
		} else  {
			res1 = mask_line(mask_buf, abs_x, abs_y, len, &p->start_line);
		}

		if (p->cfg.end_angle == 180) {
			if (abs_y < p->cfg.vertex_p.y) {
				res2 = MASK_RES_UNKNOWN;
			} else {
				res2 = MASK_RES_FULL_COVER;
			}
		} else if (p->cfg.end_angle == 0) {
			if (abs_y < p->cfg.vertex_p.y) {
				res2 = MASK_RES_FULL_COVER;
			} else {
				res2 = MASK_RES_UNKNOWN;
			}
		} else if ((p->cfg.end_angle < 180 && abs_y < p->cfg.vertex_p.y) ||
		           (p->cfg.end_angle > 180 && abs_y >= p->cfg.vertex_p.y)) {
			res2 = MASK_RES_UNKNOWN;
		} else {
			res2 = mask_line(mask_buf, abs_x, abs_y, len, &p->end_line);
		}

		if (res1 == MASK_RES_TRANSP || res2 == MASK_RES_TRANSP) {
			return MASK_RES_TRANSP;
		} else if (res1 == MASK_RES_UNKNOWN && res2 == MASK_RES_UNKNOWN) {
			return MASK_RES_TRANSP;
		} else if (res1 == MASK_RES_FULL_COVER &&  res2 == MASK_RES_FULL_COVER) {
			return MASK_RES_FULL_COVER;
		} else {
			return MASK_RES_CHANGED;
		}
	}
}
AT_UI_SIM_RAM
static u8 mask_line(u8 *mask_buf, int abs_x, int abs_y, int len, mask_line_param_t *p)
{
	abs_y -= p->origo.y;
	abs_x -= p->origo.x;

	if (p->steep == 0) {
		if (p->flat) {
			if (p->cfg.side == MASK_LINE_SIDE_LEFT ||
			    p->cfg.side == MASK_LINE_SIDE_RIGHT) {
				return MASK_RES_FULL_COVER;
			} else if (p->cfg.side == MASK_LINE_SIDE_TOP && abs_y + 1 < 0) {
				return MASK_RES_FULL_COVER;
			} else if (p->cfg.side == MASK_LINE_SIDE_BOTTOM && abs_y > 0) {
				return MASK_RES_FULL_COVER;
			} else {
				return MASK_RES_TRANSP;
			}
		} else {
			if (p->cfg.side == MASK_LINE_SIDE_TOP ||
			    p->cfg.side == MASK_LINE_SIDE_BOTTOM) {
				return MASK_RES_FULL_COVER;
			} else if (p->cfg.side == MASK_LINE_SIDE_RIGHT && abs_x > 0) {
				return MASK_RES_FULL_COVER;
			} else if (p->cfg.side == MASK_LINE_SIDE_LEFT) {
				if (abs_x + len < 0) {
					return MASK_RES_FULL_COVER;
				} else {
					int k = - abs_x;
					if (k < 0) {
						return MASK_RES_TRANSP;
					}
					if (k >= 0 && k < len) {
						_memset_00(&mask_buf[k], len - k);
					}
					return  MASK_RES_CHANGED;
				}
			} else {
				if (abs_x + len < 0) {
					return MASK_RES_TRANSP;
				} else {
					int k = - abs_x;
					if (k < 0) {
						k = 0;
					}
					if (k >= len) {
						return MASK_RES_TRANSP;
					} else if (k >= 0 && k < len) {
						_memset_00(&mask_buf[0], k);
					}
					return  MASK_RES_CHANGED;
				}
			}
		}
	}

	u8 res;
	if (p->flat) {
		res = line_mask_flat(mask_buf, abs_x, abs_y, len, p);
	} else {
		res = line_mask_steep(mask_buf, abs_x, abs_y, len, p);
	}

	return res;
}
AT_UI_SIM_RAM
static u8 line_mask_flat(u8 *mask_buf, int abs_x, int abs_y, int len, mask_line_param_t *p)
{
	int y_at_x;
	y_at_x = (int)((int)p->yx_steep * abs_x) >> 10;

	if (p->yx_steep > 0) {
		if (y_at_x > abs_y) {
			if (p->inv) {
				return MASK_RES_FULL_COVER;
			} else {
				return MASK_RES_TRANSP;
			}
		}
	} else {
		if (y_at_x < abs_y) {
			if (p->inv) {
				return MASK_RES_FULL_COVER;
			} else {
				return MASK_RES_TRANSP;
			}
		}
	}

	y_at_x = (int)((int)p->yx_steep * (abs_x + len)) >> 10;
	if (p->yx_steep > 0) {
		if (y_at_x < abs_y) {
			if (p->inv) {
				return MASK_RES_TRANSP;
			} else {
				return MASK_RES_FULL_COVER;
			}
		}
	} else {
		if (y_at_x > abs_y) {
			if (p->inv) {
				return MASK_RES_TRANSP;
			} else {
				return MASK_RES_FULL_COVER;
			}
		}
	}

	int xe;
	if (p->yx_steep > 0) {
		xe = ((abs_y << 8) * p->xy_steep) >> 10;
	} else {
		xe = (((abs_y + 1) << 8) * p->xy_steep) >> 10;
	}

	int xei = xe >> 8;
	int xef = xe & 0xFF;

	int px_h;
	if (xef == 0) {
		px_h = 255;
	} else {
		px_h = 255 - (((255 - xef) * p->spx) >> 8);
	}
	int k = xei - abs_x;
	u8 m;

	if (xef) {
		if (k >= 0 && k < len) {
			m = 255 - (((255 - xef) * (255 - px_h)) >> 9);
			if (p->inv) {
				m = 255 - m;
			}
			mask_buf[k] = mask_mix(mask_buf[k], m);
		}
		k++;
	}

	while (px_h > p->spx) {
		if (k >= 0 && k < len) {
			m = px_h - (p->spx >> 1);
			if (p->inv) {
				m = 255 - m;
			}
			mask_buf[k] = mask_mix(mask_buf[k], m);
		}
		px_h -= p->spx;
		k++;
		if (k >= len) {
			break;
		}
	}


	if (k < len && k >= 0) {
		int x_inters = (px_h * p->xy_steep) >> 10;
		m = (x_inters * px_h) >> 9;
		if (p->yx_steep < 0) {
			m = 255 - m;
		}
		if (p->inv) {
			m = 255 - m;
		}
		mask_buf[k] = mask_mix(mask_buf[k], m);
	}


	if (p->inv) {
		k = xei - abs_x;
		if (k > len) {
			return MASK_RES_TRANSP;
		}
		if (k >= 0) {
			_memset_00(&mask_buf[0], k);
		}
	} else {
		k++;
		if (k < 0) {
			return MASK_RES_TRANSP;
		}
		if (k <= len) {
			_memset_00(&mask_buf[k], len - k);
		}
	}

	return MASK_RES_CHANGED;
}

AT_UI_SIM_RAM
static u8 line_mask_steep(u8 *mask_buf, int abs_x, int abs_y, int len, mask_line_param_t *p)
{
	int k;
	int x_at_y;
	x_at_y = (int)((int)p->xy_steep * abs_y) >> 10;
	if (p->xy_steep > 0) {
		x_at_y++;
	}
	if (x_at_y < abs_x) {
		if (p->inv) {
			return MASK_RES_FULL_COVER;
		} else {
			return MASK_RES_TRANSP;
		}
	}

	x_at_y = (int)((int)p->xy_steep * (abs_y)) >> 10;
	if (x_at_y > abs_x + len) {
		if (p->inv) {
			return MASK_RES_TRANSP;
		} else {
			return MASK_RES_FULL_COVER;
		}
	}

	int xs = ((abs_y << 8) * p->xy_steep) >> 10;
	int xsi = xs >> 8;
	int xsf = xs & 0xFF;

	int xe = (((abs_y + 1) << 8) * p->xy_steep) >> 10;
	int xei = xe >> 8;
	int xef = xe & 0xFF;

	u8 m;

	k = xsi - abs_x;
	if (xsi != xei && (p->xy_steep < 0 && xsf == 0)) {
		xsf = 0xFF;
		xsi = xei;
		k--;
	}

	if (xsi == xei) {
		if (k >= 0 && k < len) {
			m = (xsf + xef) >> 1;
			if (p->inv) {
				m = 255 - m;
			}
			mask_buf[k] = mask_mix(mask_buf[k], m);
		}
		k++;

		if (p->inv) {
			k = xsi - abs_x;
			if (k >= len) {
				return MASK_RES_TRANSP;
			}
			if (k >= 0) {
				_memset_00(&mask_buf[0], k);
			}

		} else {
			if (k > len) {
				k = len;
			}
			if (k == 0) {
				return MASK_RES_TRANSP;
			} else if (k > 0) {
				_memset_00(&mask_buf[k],  len - k);
			}
		}

	} else {
		int y_inters;
		if (p->xy_steep < 0) {
			y_inters = (xsf * (-p->yx_steep)) >> 10;
			if (k >= 0 && k < len) {
				m = (y_inters * xsf) >> 9;
				if (p->inv) {
					m = 255 - m;
				}
				mask_buf[k] = mask_mix(mask_buf[k], m);
			}
			k--;

			int x_inters = ((255 - y_inters) * (-p->xy_steep)) >> 10;

			if (k >= 0 && k < len) {
				m = 255 - (((255 - y_inters) * x_inters) >> 9);
				if (p->inv) {
					m = 255 - m;
				}
				mask_buf[k] = mask_mix(mask_buf[k], m);
			}

			k += 2;

			if (p->inv) {
				k = xsi - abs_x - 1;

				if (k > len) {
					k = len;
				} else if (k > 0) {
					_memset_00(&mask_buf[0],  k);
				}

			} else {
				if (k > len) {
					return MASK_RES_FULL_COVER;
				}
				if (k >= 0) {
					_memset_00(&mask_buf[k],  len - k);
				}
			}

		} else {
			y_inters = ((255 - xsf) * p->yx_steep) >> 10;
			if (k >= 0 && k < len) {
				m = 255 - ((y_inters * (255 - xsf)) >> 9);
				if (p->inv) {
					m = 255 - m;
				}
				mask_buf[k] = mask_mix(mask_buf[k], m);
			}

			k++;

			int x_inters = ((255 - y_inters) * p->xy_steep) >> 10;
			if (k >= 0 && k < len) {
				m = ((255 - y_inters) * x_inters) >> 9;
				if (p->inv) {
					m = 255 - m;
				}
				mask_buf[k] = mask_mix(mask_buf[k], m);
			}
			k++;

			if (p->inv) {
				k = xsi - abs_x;
				if (k > len) {
					return MASK_RES_TRANSP;
				}
				if (k >= 0) {
					_memset_00(&mask_buf[0],  k);
				}

			} else {
				if (k > len) {
					k = len;
				}
				if (k == 0) {
					return MASK_RES_TRANSP;
				} else if (k > 0) {
					_memset_00(&mask_buf[k],  len - k);
				}
			}
		}
	}

	return MASK_RES_CHANGED;
}

AT_UI_SIM_RAM
int mask_add(void *param, void *custom_id)
{
	u8 i;
	for (i = 0; i < _MASK_MAX_NUM; i++) {
		if (_mask_list[i].param == NULL) {
			break;
		}
	}

	if (i >= _MASK_MAX_NUM) {
		return MASK_ID_INV;
	}

	_mask_list[i].param = param;
	_mask_list[i].custom_id = custom_id;

	return i;
}

AT_UI_SIM_RAM
u8 mask_get_cnt(void)
{
	u8 cnt = 0;
	u8 i;
	for (i = 0; i < _MASK_MAX_NUM; i++) {
		if (_mask_list[i].param) {
			cnt++;
		}
	}
	return cnt;
}
AT_UI_SIM_RAM
void *mask_remove_id(int id)
{
	void *p = NULL;

	if (id != MASK_ID_INV) {
		p = _mask_list[id].param;
		_mask_list[id].param = NULL;
		_mask_list[id].custom_id = NULL;
	}

	return p;
}
AT_UI_SIM_RAM
u8 mask_apply(u8 *mask_buf, int abs_x, int abs_y, int len)
{
	bool changed = false;
	mask_common_dsc_t *dsc;

	_mask_saved_t *m = _mask_list;

	while (m->param) {
		dsc = m->param;
		u8 res = MASK_RES_FULL_COVER;
		res = dsc->cb(mask_buf, abs_x, abs_y, len, (void *)m->param);
		if (res == MASK_RES_TRANSP) {
			return MASK_RES_TRANSP;
		} else if (res == MASK_RES_CHANGED) {
			changed = true;
		}

		m++;
	}

	return changed ? MASK_RES_CHANGED : MASK_RES_FULL_COVER;
}


