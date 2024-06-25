#ifndef  __UPGRADE_H__
#define  __UPGRADE_H__


#include "common.h"

#ifdef __UP_DEBUG
#define UP_DEBUG    1
#else
#define UP_DEBUG    0
#endif

#if UP_DEBUG
#define up_puts			puts
#define up_printf		printf
#define up_put_buf		put_buf
#define up_printf_buf	printf_buf
#else
#define up_puts(a)
#define up_printf(...)
#define up_put_buf(a,b)
#define up_printf_buf(a,b)
#endif

void *get_sdk_swap_addr();
void uart_upgrade(const char *utrx, u32 baud);
u8 update_loop_run(u32 flash_ota_addr);
u8 jl_check_upgrade(u8 jlfs_err);


#endif

