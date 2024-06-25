#ifndef __NANDFLASH_FTL_H__
#define __NANDFLASH_FTL_H__

#define FTL_NAND_PAGE_SIZE 				2048
#define FTL_NAND_BLOCK_SIZE    			0x20000
enum {
    FTL_OK = 0,
    FTL_DISK_ERR,
    FTL_FILE_ERR,
    FTL_BLOCK_ERR,
    FTL_FLASH_ERR,
    FTL_MOUNT_FAT_ERR,
    FTL_UPDATE_SPBK_ERR,
    FTL_SEARCH_ERR,
};

extern const struct device_operations nandflash_ftl_ops;

#define FTL_FREE         0xFF
#define FTL_DAMAGED      0x00
#define FTL_USED         0x01
#define FTL_CONSUMED     0x10
#define FTL_FAT          0x20
#define FTL_USED_LOG     0x40
#define FTL_SPBK         0xF8FFFFFF
#define FTL_BLOCK_INFO   0xF4FFFFFF
#define FTL_EX_FREE      0xFEFFFFFF
#define FTL_BLOCK_BREAK  0xFFFFFFFF

typedef struct _spbk_info {
    char spbk_logo[4];
    int spbk_page_size;
    int spbk_block_size;
    int spbk_flash_size;
    int spbk_mount_size;
    int spbk_hot_part_addr;
    int spbk_cold_part_addr;
    int spbk_cold_part_exaddr;
    int spbk_hot_part_exaddr;
    int spbk_block_info_logaddr;
    int spbk_block_info_addr;
    int spbk_block_info_len;
} SPBK_INFO;

typedef struct _block_info {
    int logic_map_addr;
    char page_status;
    char block_erase_num[3];
} BLOCK_INFO;

typedef struct _ftl_hdl {
    SPBK_INFO spbk;
    BLOCK_INFO block_info;
    void *hdev;
    char re_devname[16];
    char erase_all;
    char block_info_exchange;
    char err_block_num;
    int cur_block;
    int new_cold_sblock;
    int old_cold_exaddr;
} FTL_HDL;

#endif
