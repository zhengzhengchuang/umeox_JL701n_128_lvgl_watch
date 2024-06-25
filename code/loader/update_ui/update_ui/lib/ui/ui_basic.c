#include "ui/ui_basic.h"
#include "ui/ui_type.h"

#define SET32(x) *d32 = x; d32++;

AT_UI_SIM_RAM
void _memset(void *dst, u8 v, u32 len)
{
#if 1
	memset(dst, v, len);
#else
	u8 *d8 = (u8 *) dst;
	u32 d_align = (u32) d8 & ALIGN_MASK;

	memset(dst, v, len);
	return;

	if (d_align) {
		d_align = ALIGN_MASK + 1 - d_align;
		while (d_align && len) {
			*d8 = v;
			d8++;
			len--;
			d_align--;
		}
	}

	u32 v32 = v + (v << 8) + (v << 16) + (v << 24);
	u32 *d32 = (u32 *)d8;

	while (len > 32) {
		SET32(v32);
		SET32(v32);
		SET32(v32);
		SET32(v32);
		SET32(v32);
		SET32(v32);
		SET32(v32);
		SET32(v32);
		len -= 32;
	}

	while (len > 4) {
		SET32(v32);
		len -= 4;
	}


	d8 = (u8 *)d32;
	while (len) {
		*d8 = v;
		d8++;
		len--;
	}
#endif // 1
}
AT_UI_SIM_RAM
void _memset_ff(void *dst, u32 len)
{
#if 1
	memset(dst, 0xff, len);
#else
	u8 *d8 = (u8 *) dst;
	u32 d_align = (u32) d8 & ALIGN_MASK;

	memset(dst, 0xff, len);

	if (d_align) {
		d_align = ALIGN_MASK + 1 - d_align;
		while (d_align && len) {
			*d8 = 0xFF;
			d8++;
			len--;
			d_align--;
		}
	}

	u32 *d32 = (u32 *)d8;
	while (len > 32) {
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		SET32(0xFFFFFFFF);
		len -= 32;
	}

	while (len > 4) {
		SET32(0xFFFFFFFF);
		len -= 4;
	}


	d8 = (u8 *)d32;
	while (len) {
		*d8 = 0xFF;
		d8++;
		len--;
	}
#endif // 1
}

AT_UI_SIM_RAM
void _memset_00(void *dst, u32 len)
{
#if 1
	memset(dst, 0, len);
#else
	u8 *d8 = (u8 *) dst;
	u32 d_align = (u32) d8 & ALIGN_MASK;

	memset(dst, 0x00, len);
	return;

	if (d_align) {
		d_align = ALIGN_MASK + 1 - d_align;
		while (d_align && len) {
			*d8 = 0x00;
			d8++;
			len--;
			d_align--;
		}
	}

	u32 *d32 = (u32 *)d8;
	while (len > 32) {
		SET32(0);
		SET32(0);
		SET32(0);
		SET32(0);
		SET32(0);
		SET32(0);
		SET32(0);
		SET32(0);
		len -= 32;
	}

	while (len > 4) {
		SET32(0);
		len -= 4;
	}

	d8 = (u8 *)d32;
	while (len) {
		*d8 = 0;
		d8++;
		len--;
	}
#endif // 1
}

