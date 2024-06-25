#ifndef __COMMON_H__
#define __COMMON_H__

#include "asm/cpu.h"
#include "asm/csfr.h"
#include "typedef.h"
#include "device_errno.h"
//#include "printf.h"


#define SFR(sfr, start, len, dat)   (sfr = (sfr & ~((~(0xffffffff << (len))) << (start))) | (((dat) & (~(0xffffffff << (len)))) << (start)))


#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif


#ifdef __PRINTF_DEBUG
#define APP_DEBUG    1
#else
#define APP_DEBUG    0
#endif

// #if APP_DEBUG
// #define log_info			printf
// #define log_error(...)
// #define log_info_hexdump    printf_buf
// #else
// #define log_info(...)
// #define log_info_hexdump(a,b)
// #endif

void wdt_clr();

#endif

