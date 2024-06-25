#include "includes.h"
#include "app_config.h"

#if (CONFIG_UART_DEBUG_ENABLE == 0)
void putchar(char a)
{
}

void putbyte(char a)
{
}

int puts(const char *out)
{
    return 0;
}

void put_u32hex(unsigned int dat)
{
}

void put_buf(const u8 *buf, int len)
{
}

int printf(const char *format, ...)
{
    return 0;
}

#endif /* #if (CONFIG_UART_DEBUG_ENABLE == 0) */
