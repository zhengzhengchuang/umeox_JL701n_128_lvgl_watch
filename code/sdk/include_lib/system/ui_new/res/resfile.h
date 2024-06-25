#ifndef RESFILE_H
#define RESFILE_H

#include "typedef.h"
#include "fs/fs.h"

// resfile共用文件句柄
#if (defined(CONFIG_CPU_BR23) && defined(CONFIG_APP_WATCH))
#define RESFILE_COMMON_HDL_EN		0
#else
#define RESFILE_COMMON_HDL_EN		0
#endif

#define FILE_TYPE_JPEG 	5
#define AT_UI_RAM             AT(.ui_ram)

//文本数据格式
enum {
    TEXT_1BPP,
    TEXT_2BPP,
    TEXT_4BPP,
    TEXT_8BPP,
};

//图像数据格式
enum {
    PIXEL_FMT_ARGB8888,
    PIXEL_FMT_RGB888,
    PIXEL_FMT_RGB565,
    PIXEL_FMT_L8,
    PIXEL_FMT_AL88,
    PIXEL_FMT_AL44,
    PIXEL_FMT_A8,
    PIXEL_FMT_L1,
    PIXEL_FMT_ARGB8565,
    PIXEL_FMT_OSD16,
    PIXEL_FMT_SOLID,
    PIXEL_FMT_JPEG,
    PIXEL_FMT_UNKNOW,
};

// #define EXTERN_PATH "storage/nor_ui/C/res/"
// #define EXTERN_PATH "storage/virfat_flash/C/uires/"
struct image_file {
    u8 format;
    u8 compress;
    u16 data_crc;
    u16 width;
    u16 height;
    u32 offset;
    u32 len;
    u32 unzipOffset;
    u32 unzipLen;
};

typedef struct resfile {
    FILE *file;
#if RESFILE_COMMON_HDL_EN
    struct list_head entry;
    u32 offset;
    u32 size;
#endif
} RESFILE;

int open_resfile(const char *name);
void close_resfile();

int res_file_version_compare(int res_ver);

int open_str_file(const char *name);
void close_str_file();
int str_file_version_compare(int str_ver);

int open_style_file(const char *name);

int font_ascii_init(const char *name);
int open_image_by_id(int prj, RESFILE *specfile, struct image_file *f, int id, int page);
int read_image_data(struct image_file *f, u8 *data, int len);
int br23_read_image_data(int prj, RESFILE *specfile, struct image_file *f, u8 *data, int len, int offset);
int br25_read_image_data(RESFILE *specfile, struct image_file *f, u8 *data, int len, int offset);
u32 image_decode(const void *pSour, void *pDest, u32 SourLen, u32 DestLen, u8 compress);
int open_string_pic(int prj, struct image_file *file, int id);
int open_string_by_id(int prj, RESFILE *specfile, struct image_file *f, int id);
int read_str_data(struct image_file *f, u8 *data, int len);
int br23_read_str_data(struct image_file *f, u8 *data, int len, int offset);
int br25_read_str_data(struct image_file *f, u8 *data, int len, int offset);
int load_pallet_table(int id, u32 *data);
int ui_language_set(int language);
int ui_language_get();
int read_string_type(int prj);
int open_string_pic2(int prj, struct image_file *file, int id);

RESFILE *res_fopen(const char *path, const char *mode);
int res_fread(RESFILE *_file, void *buf, u32 len);
int res_fseek(RESFILE *_file, int offset, int fromwhere);
int res_flen(RESFILE *file);
int res_fclose(RESFILE *file);
int _norflash_read_watch(u8 *buf, u32 addr, u32 len, u8 wait);//加速读

struct flashheader {
    u16 crc16;
    u16 crcfileheaddata;
    u8  info[8];
    u32 filenum;
    u32 version;
    u32 version1;
    u8  chiptype[8];
};

struct flashfilehead {
    u8  filetype;
    u8  reserv;
    u16 crc16;
    u32 addr;
    u32 len;
    u32 version;
    u8  name[64];
};


struct myfile {
    SDFILE *file;
    FILE *_file;
    u8 name[64];
    u32 addr;
    u32 len;
};

struct flash_file_info {
    u32 *tab;
    u16 tab_size;
    u32 offset;
    u32 last_tab_data_len;
};

#define  PHY_JL_EXTERN_FLASH     0  //杰理外挂fat 文件系统方案
#define  PHY_JL_INSERT_FLASH     1  //杰理内置flash 文件系统方案
//针对内封flash 场景 ui 硬件模块需要获取知道当前路径是挂靠在内置flash  还是 外挂flash

struct ui_load_info {
    u8    pj_id;
    u8    phy_dev;
//针对内封flash 场景 ui 硬件模块需要获取知道当前路径是挂靠在内置flash  还是 外挂flash
    u32   phy_addr;
//针对内封flash 场景 设备的物理地址不是从0开始的，需要告诉硬件模块偏移
    const char *path;
    RESFILE *file;
    RESFILE *res;
    RESFILE *str;
    struct flash_file_info image_file_info;
    struct flash_file_info str_file_info;
};

void *ui_load_res_by_pj_id(int pj_id);
void *ui_load_str_by_pj_id(int pj_id);
int ui_set_sty_path_by_pj_id(int pj_id, const u8 *path);
void *ui_load_sty_by_pj_id(int pj_id);
void *ui_get_image_file_info_by_pj_id(int pj_id);
void *ui_get_str_file_info_by_pj_id(int pj_id);
//
// extern int __ui_res_flash_info_get(struct flash_file_info *file_info, char *path, char *str,u8 phy_dev,u8 phy_addr);
//
extern int ui_res_flash_info_get(struct flash_file_info *file_info, char *path, char *str);
extern void ui_res_flash_info_free(struct flash_file_info *file_info, char *str);
extern int ui_res_get_flash_tab_info(struct flash_file_info *file_info, u32 file_base_addr, u32 filelen);

int ui_unload_file_info();

#endif
