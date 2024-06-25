#ifndef  __SETUP_H__
#define  __SETUP_H__


#include "common.h"


#ifdef __ST_DEBUG
#define ST_DEBUG    1
#else
#define ST_DEBUG    0
#endif

#if ST_DEBUG
#define st_puts			puts
#define st_printf		printf
#define st_put_buf		put_buf
#define st_printf_buf	printf_buf
#else
#define st_puts(a)
#define st_printf(...)
#define st_put_buf(a,b)
#define st_printf_buf(a,b)
#endif


void *get_boot_info_t(void);
u8 arch_init(void);
u8 sfc_mode_boot(void);


#endif

