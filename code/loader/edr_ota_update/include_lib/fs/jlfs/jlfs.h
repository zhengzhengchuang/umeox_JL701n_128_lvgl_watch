#ifndef  __JLFS_H__
#define  __JLFS_H__

#include "common.h"

#ifdef __JLFS_DEBUG
#define JLFS_DEBUG    1
#else
#define JLFS_DEBUG    0
#endif

#if JLFS_DEBUG
#define jlfs_puts			puts
#define jlfs_printf			printf
#define jlfs_put_buf		put_buf
#define jlfs_printf_buf		printf_buf
#else
#define jlfs_puts(a)
#define jlfs_printf(...)
#define jlfs_put_buf(a,b)
#define jlfs_printf_buf(a,b)
#endif

typedef struct {
    u32 addr;   //flash里的绝对地址
    u32 size;   //文件大小
    u32 pos;    //操作的偏移量
    u32 res;    //预留参数
} JLFS_FILE;


u32 jlfs_get_flash_align_size(void);
u32 jlfs_get_flash_app_base_addr(void);
u32 jlfs_get_app_entry_addr(void);
u8 *jlfs_get_isd_cfg_ptr(void);
u8 jlfs_read_isd_cfg_ini(const char *cfg, void *value);
u8 jlfs_mount(void);
u8 jlfs_fopen_by_addr(JLFS_FILE *file, u32 addr);
u8 jlfs_fopen_by_name(JLFS_FILE *file, const char *name, u8 mode);
u32 jlfs_fread(JLFS_FILE *file, u8 *buf, u32 len);
u32 jlfs_erase(JLFS_FILE *file, u32 len);
u32 jlfs_write(JLFS_FILE *file, u8 *buf, u32 len);
void jlfs_fseek(JLFS_FILE *file, u32 offset);
void jlfs_get_pid_vid(u8 *pid, u8 *vid);


#endif

