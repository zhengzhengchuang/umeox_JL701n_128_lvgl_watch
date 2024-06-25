#ifndef  __NORFLASH_H__
#define  __NORFLASH_H__

#include "common.h"

#ifdef __NOR_DEBUG
#define NOR_DEBUG    1
#else
#define NOR_DEBUG    0
#endif

#if NOR_DEBUG
#define nor_puts			puts
#define nor_printf			printf
#define nor_put_buf		    put_buf
#define nor_printf_buf	    printf_buf
#else
#define nor_puts(a)
#define nor_printf(...)
#define nor_put_buf(a,b)
#define nor_printf_buf(a,b)
#endif

#define WINBOND_WRITE_ENABLE                                0x06
#define WINBOND_JEDEC_ID                                    0x9F
#define WINBOND_UUID                                        0x4B
#define WINBOND_READ_SR1                                    0x05
#define WINBOND_READ_SR2                                    0x35
#define WINBOND_WRITE_SR1                                   0x01
#define WINBOND_WRITE_SR2                                   0x31
#define WINBOND_FAST_READ                                   0x0B
#define WINBOND_FAST_READ_DUAL_OUTPUT                       0x3B
#define WINBOND_FAST_READ_QUAD_OUTPUT                       0x6B
#define WINBOND_FAST_READ_DUAL_IO                           0xBB
#define WINBOND_FAST_READ_QUAD_IO                           0xEB
#define WINBOND_CONTINUOUS_READ_NORMAL_MODE                 0x00
#define WINBOND_CONTINUOUS_READ_ENHANCE_MODE                0xa0
#define WINBOND_PAGE_PROGRAM                                0x02
#define WINBOND_PAGE_ERASE                                  0x81
#define WINBOND_SECTOR_ERASE                                0x20
#define WINBOND_BLOCK_ERASE                                 0xD8
#define WINBOND_CHIP_ERASE                                  0xC7

struct flash_info_t {
    u8 uuid[16];
    u32 id;
    u32 capacity;
};

struct flash_info_t *get_flash_info(void);
u32 get_flash_capacity(void);

//void flash_poweron(void);
void norflash_init(u8 width, u8 clk_div, u8 read_mode);
int norflash_read(u8 *buf, u32 addr, u32 len);
u8 norflash_write(u8 *buf, u32 addr, u32 len);
u8 norflash_eraser(u32 eraser_cmd, u32 addr);


#endif

