#include "app_config.h"
#include "typedef.h"
#include "fs.h"
#include "norflash.h"
#include "spi/nor_fs.h"
#include "rec_nor/nor_interface.h"
#include "update_loader_download.h"

#if (USER_FILE_UPDATE_V2_EN)

#define LOG_TAG "[UP_RESER_DNLD]"
#define LOG_INFO_ENABLE
#define LOG_ERROR_ENABLE
#include "system/debug.h"

typedef struct _user_file_base_info_t {
    struct list_head 	entry;
    user_chip_update_info_t *(*get_res_base_info)(void *priv,
            u32(**local_read_hdl)(void *buf, u32 addr, u32 len),
            u32(**local_write_hdl)(void *buf, u32 addr, u32 len),
            u32(**local_erase_hdl)(u32 cmd, u32 addr));
    int (*user_chip_release_opt)(user_chip_update_info_t *update_info);
    void *priv;
} user_file_base_info_t;

typedef struct _res_bin_file_update_info_t {
    user_chip_update_info_t info;
    int (*decode_func)(const u8 *buf, u32 addr, u32 len);
} reserve_file_update_info_t;

static reserve_file_update_info_t *g_reserve_info = NULL;
#define __this (g_reserve_info)

#define FILE_TYPE_FW_RESERVED_ZONE_FILE		0x32
#define SDFILE_EXT_RESERVED_ROOT_PATH		"mnt/sdfile/EXT_RESERVED/"

// 1是不升级预留区域
static u8 reserved_update_in_ota_flag = 0;

typedef struct _record_reserved_file_info_t {
    struct list_head 	entry;
    u32 start_addr;
    u32 len;
    u8 reserved_name[16];
} record_reserved_file_info_t;

static record_reserved_file_info_t *g_record_reserved_file_info = NULL;

static int record_reserved_file_info(u8 *file_name, u32 start_addr, u32 len)
{
    record_reserved_file_info_t *record_reserved_file_info = NULL;
    record_reserved_file_info = (record_reserved_file_info_t *) zalloc(sizeof(record_reserved_file_info_t));
    if (NULL == record_reserved_file_info) {
        goto _end;
    }
    INIT_LIST_HEAD(&record_reserved_file_info->entry);
    record_reserved_file_info->start_addr = start_addr;
    record_reserved_file_info->len = len;
    memcpy(record_reserved_file_info->reserved_name, file_name, sizeof(record_reserved_file_info->reserved_name));

    if (NULL == g_record_reserved_file_info) {
        g_record_reserved_file_info = record_reserved_file_info;
    } else {
        // 入链表
        list_add_tail(&record_reserved_file_info->entry, &g_record_reserved_file_info->entry);
    }
_end:
    return 0;
}

static int record_reserved_file_info_verify(record_reserved_file_info_t *local_resv_info, u32 remote_start_addr, u32 remote_len, u8 remote_resv_opt)
{
    // 检查当前需要升级的预留区域是否不存在，如果不存在(即远端存在需要升级的预留区域本地不存在的情况)，则需要在ota中升级
    // 如果远端配置概升级区域是需要执行擦除操作，则需要在ota中升级
    // 如果远端对应的预留区域大小比本地对应区域要大，则需要在ota中升级
    // 如果远端对应的预留区域起始地址和本地对不上，则需要在ota中升级
    if (0 == remote_resv_opt || remote_start_addr != local_resv_info->start_addr || remote_len > local_resv_info->len) {
        reserved_update_in_ota_flag = 0;
    }
    return 0;
}

static int record_reserved_release_opt(void)
{
    record_reserved_file_info_t *curr = NULL;
    record_reserved_file_info_t *next = NULL;
    if (g_record_reserved_file_info) {
        list_for_each_entry_safe(curr, next, &g_record_reserved_file_info->entry, entry) {
            list_del(&curr->entry);
            free(curr);
        }
        free(g_record_reserved_file_info);
        g_record_reserved_file_info = NULL;
    }
    return 0;
}

static int norflash_f_open(u32 file_len, u8 *file_name)
{
    int ret = 0;
    FILE *fp = NULL;
    struct vfs_attr file_attr;
    u8 file_path_len = sizeof(SDFILE_EXT_RESERVED_ROOT_PATH) + strlen(file_name);
    u8 *file_path = zalloc(file_path_len);
    if (NULL == file_path) {
        printf("%s, alloc fail\n", __func__);
        ret = -1;
        goto _ERR_RET;
    }

    // 如果SDFILE_APP_ROOT_PATH + file_name不行，则还需要尝试打开扩展预留区域(mnt/sdfile/EXT_RESERVED)
    memcpy(file_path, SDFILE_APP_ROOT_PATH, strlen(SDFILE_APP_ROOT_PATH));
    memcpy(file_path + strlen(SDFILE_APP_ROOT_PATH), file_name, strlen(file_name));
    fp = fopen(file_path, "r");

    if (!fp) {
        memset(file_path, 0, file_path_len);
        memcpy(file_path, SDFILE_EXT_RESERVED_ROOT_PATH, strlen(SDFILE_EXT_RESERVED_ROOT_PATH));
        memcpy(file_path + strlen(SDFILE_EXT_RESERVED_ROOT_PATH), file_name, strlen(file_name));
        fp = fopen(file_path, "r");
    }

    if (!fp) {
        ret = -1;
        goto _ERR_RET;
    }

    printf("open %s success\n", file_path);

    // 获取起始地址，填充__this->info.dev_addr
    fget_attrs(fp, &file_attr);
    __this->info.dev_addr = sdfile_cpu_addr2flash_addr(file_attr.sclust);

    if (file_len > file_attr.fsize) {
        printf("%s no enough to update\n", file_path);
        __this->info.dev_addr = 0;
        ret = -1;
        goto _ERR_RET;
    }

_ERR_RET:
    // 先记录所有需要升级的预留区域
    record_reserved_file_info(file_name, __this->info.dev_addr, __this->info.dev_addr ? file_attr.fsize : 0);

    if (file_path) {
        free(file_path);
    }
    if (fp) {
        fclose(fp);
    }
    return ret;
}

typedef enum _FLASH_ERASER {
    CHIP_ERASER,
    BLOCK_ERASER,
    SECTOR_ERASER,
} FLASH_ERASER;

extern u32 sfc_read(u8 *buf, u32 addr, u32 len);
extern u32 sfc_write(const u8 *buf, u32 addr, u32 len);
extern bool sfc_erase(FLASH_ERASER cmd, u32 addr);

static u32 reserve_file_sfc_write(const u8 *buf, u32 addr, u32 len)
{
    u32 offset = addr - __this->info.dev_addr;
    if (__this && __this->decode_func) {
        __this->decode_func(buf, __this->info.addr + offset, len);
    }
    return sfc_write(buf, addr, len);
}

static u32 reserve_file_sfc_read(u8 *buf, u32 addr, u32 len)
{
    return sfc_read(buf, addr, len);
}

static bool reserve_file_sfc_erase(FLASH_ERASER cmd, u32 addr)
{
    return sfc_erase(cmd, addr);
}

// 填充当前操作的方式的特殊的读、写和擦函数，并返回对应的info信息
static user_chip_update_info_t *reserve_get_res_base_info(void *priv,

        u32(**local_read_hdl)(void *buf, u32 addr, u32 len),
        u32(**local_write_hdl)(void *buf, u32 addr, u32 len),
        u32(**local_erase_hdl)(u32 cmd, u32 addr))
{
    reserve_file_update_info_t *reserve_info = (reserve_file_update_info_t *)priv;
    if (local_read_hdl && local_write_hdl && local_erase_hdl) {
        printf("reserved area start_addr %x and len %x update ...\n", reserve_info->info.dev_addr, reserve_info->info.len);
        __this = (reserve_file_update_info_t *)priv;
        *local_read_hdl = reserve_file_sfc_read;
        *local_write_hdl = reserve_file_sfc_write;
        *local_erase_hdl = reserve_file_sfc_erase;
    }
    if (0 == reserved_update_in_ota_flag) {
        reserve_info->info.len = 0;
    }
    return &reserve_info->info;
}

static int reserve_update_release(user_chip_update_info_t *update_info_entry)
{
    if (update_info_entry) {
        if (update_info_entry->priv) {
            free(update_info_entry->priv);
            update_info_entry->priv = NULL;
        }
        free(update_info_entry);
        update_info_entry = NULL;
    }
    record_reserved_release_opt();
    return 0;
}

struct list_head *reserve_file_update_init(void *priv, u8 type, u8 *file_name, int (*update_info_get)(u8 type, user_chip_update_info_t *info, void *priv))
{
    user_chip_update_info_t *info = (user_chip_update_info_t *)priv;
    user_file_base_info_t *base_info = NULL;
    reserved_update_in_ota_flag = 1;
    __this = NULL;

    if (NULL == priv || FILE_TYPE_FW_RESERVED_ZONE_FILE != type) {
        goto _ERR_RET;
    }
    __this = zalloc(sizeof(reserve_file_update_info_t));
    if (NULL == __this) {
        printf("%s, alloc fail\n", __func__);
        goto _ERR_RET;
    }
    memcpy(&__this->info, info, sizeof(user_chip_update_info_t));

    // 尝试open设备，并记录相关信息
    if (norflash_f_open(__this->info.len, file_name)) {
        goto _ERR_RET;
    }

    update_info_get(type, info, &__this->decode_func);

    // 分配当前链表信息，记录上面的数据，返回当前链表
    base_info = zalloc(sizeof(user_file_base_info_t));
    if (NULL == base_info) {
        printf("%s, alloc base_info fail\n", __func__);
        goto _ERR_RET;
    }

    base_info->get_res_base_info = reserve_get_res_base_info;
    base_info->user_chip_release_opt = reserve_update_release;
    base_info->priv = (void *)__this;

    return &base_info->entry;

_ERR_RET:
    if (__this) {
        // 注意，如果info中的priv成员有值，最后是需要free掉
        if (__this->info.priv) {
            free(__this->info.priv);
            __this->info.priv = NULL;
        }
        free(__this);
        __this = NULL;
    }
    if (base_info) {
        free(base_info);
    }
    return NULL;
}

u8 reserve_file_flag_get(void)
{
    return reserved_update_in_ota_flag;
}

// 获取远端flash.bin中预留区域信息
void get_remote_reserved_info_during_update_detail(u8 *reserved_name, u32 reserved_addr, u32 reserved_len, u8 opt)
{
    record_reserved_file_info_t *item = NULL;
    // 如果当前在ota中升级预留区域标志位有效，则不需要对比
    if (0 == reserved_update_in_ota_flag) {
        return;
    }
    if (NULL == g_record_reserved_file_info) {
        return;
    }
    // 把已记录了需要升级的预留区域信息，与远端flash.bin中的预留区域对比，筛选出远端真正需要升级的预留区域
    if (list_empty(&g_record_reserved_file_info->entry)) {
        if (0 == memcmp(reserved_name, g_record_reserved_file_info->reserved_name, strlen(reserved_name) + 1)) {
            record_reserved_file_info_verify(g_record_reserved_file_info, reserved_addr, reserved_len, opt);
        }
    } else {
        list_for_each_entry(item, &g_record_reserved_file_info->entry, entry) {
            if (0 == memcmp(item->reserved_name, reserved_name, strlen(reserved_name) + 1)) {
                record_reserved_file_info_verify(item, reserved_addr, reserved_len, opt);
            }
        }
    }
}

#endif
