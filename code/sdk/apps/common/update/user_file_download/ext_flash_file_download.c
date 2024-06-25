#include "app_config.h"
#include "typedef.h"
#include "fs.h"
#include "norflash.h"
#include "spi/nor_fs.h"
#include "rec_nor/nor_interface.h"
#include "update_loader_download.h"

#if (USER_FILE_UPDATE_V2_EN)

#if TCFG_SFC_ENCRY_ENABLE
#include "asm/sfc_norflash_api.h"
#endif

#define LOG_TAG "[UP_EX_F_DNLD]"
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
    void *dev_hdl;
    void *dev_hdl_noenc;
} res_bin_file_update_info_t;

#define RES_BIN_FILE_NAME		"res.bin"

static res_bin_file_update_info_t *g_res_bin_info = NULL;
#define __this (g_res_bin_info)

extern const u32 g_res_nor_unencry_start_addr;

// 打开函数
static int norflash_f_open(u32 file_len)
{
    int ret = 0;

    if (__this) {
        //__this->dev_hdl = dev_open("res_nor", NULL);
        __this->dev_hdl = dev_open("usr_nor", NULL);
        //__this->dev_hdl_noenc = dev_open("update_noenc", NULL);
        if (__this->dev_hdl /*&& __this->dev_hdl_noenc*/) {
            log_info("open dev succ 0x%x, 0x%x\n", __this->dev_hdl, __this->dev_hdl_noenc);
        } else {
            log_error("open dev fail");
            ret = -1;
        }
    } else {
        ret = -2;
    }

    return ret;
}

static u32 norflash_f_read(void *buf, u32 addr, u32 len)
{
    int rlen = 0;

    if (__this && __this->dev_hdl && __this->dev_hdl_noenc) {
        wdt_clear();
        //printf("%s: addr = 0x%x, rlen = 0x%x", __func__, addr, len);
        if (g_res_nor_unencry_start_addr > addr) {
#if TCFG_SFC_ENCRY_ENABLE
            struct sfc_no_enc_wr r_info = {(u8 *)buf, addr, len};
            if (0 == dev_ioctl(__this->dev_hdl, IOCTL_SFC_NORFLASH_READ_NO_ENC, &r_info)) {
                rlen = len;
            }
#else
            rlen = dev_bulk_read(__this->dev_hdl, buf, addr, len);
#endif
            //put_buf(buff, 128);
        } else {
            rlen = dev_bulk_read(__this->dev_hdl_noenc, buf, addr - g_res_nor_unencry_start_addr, len);
        }
    }

    return rlen;
}

static u32 norflash_f_write(void *buf, u32 addr, u32 len)
{
    int rlen = 0;

    if (__this && __this->dev_hdl && __this->dev_hdl_noenc) {
        wdt_clear();
        if (g_res_nor_unencry_start_addr > addr) {
#if TCFG_SFC_ENCRY_ENABLE
            struct sfc_no_enc_wr w_info = {(u8 *)buf, addr, len};
            if (0 == dev_ioctl(__this->dev_hdl, IOCTL_SFC_NORFLASH_WRITE_NO_ENC, (void *)&w_info)) {
                rlen = len;
            }
#else
            //printf("%s: addr = 0x%x, rlen = 0x%x", __func__, addr, len);
            rlen = dev_bulk_write(__this->dev_hdl, buf, addr, len);
#endif
            //put_buf(buff, 128);
        } else {
            rlen = dev_bulk_write(__this->dev_hdl_noenc, buf, addr - g_res_nor_unencry_start_addr, len);
        }
    }

    return rlen;
}

typedef enum _FLASH_ERASER {
    CHIP_ERASER,
    BLOCK_ERASER,
    SECTOR_ERASER,
    PAGE_ERASER,
} FLASH_ERASER;

static u16 norflash_f_erase(u32 cmd, u32 addr)
{
    u32 type = 0;

    switch (cmd) {
    case BLOCK_ERASER:
        type = IOCTL_ERASE_BLOCK;
        break;
    case SECTOR_ERASER:
        type = IOCTL_ERASE_SECTOR;
        break;
    case CHIP_ERASER:
        type = IOCTL_ERASE_CHIP;
        break;
    }

    if (__this && type) {
        if (__this->dev_hdl && __this->dev_hdl_noenc) {
            wdt_clear();
            if (g_res_nor_unencry_start_addr > addr) {
                dev_ioctl(__this->dev_hdl, type, addr);
            } else {
                dev_ioctl(__this->dev_hdl_noenc, type, addr - g_res_nor_unencry_start_addr);
            }
        }
    } else {
        log_error("f_erase parm err\n");
    }

    return 0;
}

// 填充当前操作的方式的特殊的读、写和擦函数，并返回对应的info信息
static user_chip_update_info_t *ex_flash_get_res_base_info(void *priv,
        u32(**local_read_hdl)(void *buf, u32 addr, u32 len),
        u32(**local_write_hdl)(void *buf, u32 addr, u32 len),
        u32(**local_erase_hdl)(u32 cmd, u32 addr))
{
    res_bin_file_update_info_t *res_info = (res_bin_file_update_info_t *)priv;
    if (local_write_hdl && local_write_hdl && local_erase_hdl) {
        printf("ex flash start_addr %x and len %d update...\n", res_info->info.dev_addr, res_info->info.len);
        __this = (res_bin_file_update_info_t *)priv;
        *local_read_hdl = norflash_f_read;
        *local_write_hdl = norflash_f_write;
        *local_erase_hdl = norflash_f_erase;
    }
    return &res_info->info;
}

static int ex_flash_update_release(user_chip_update_info_t *update_info_entry)
{
    if (update_info_entry) {
        if (update_info_entry->priv) {
            free(update_info_entry->priv);
            update_info_entry->priv = NULL;
        }
        free(update_info_entry);
        update_info_entry = NULL;
    }
    return 0;
}

struct list_head *ext_flash_file_update_init(void *priv, u8 type, u8 *file_name, int (*update_info_get)(u8 type, user_chip_update_info_t *info, void *priv))
{
    // 库中传出来的，包含远端地址、长度和crc信息的结构体
    user_chip_update_info_t *info = (user_chip_update_info_t *)priv;
    user_file_base_info_t *base_info = NULL;
    __this = NULL;

    // 先根据当前的名字和类型，匹配当前是否是需要处理
    if (NULL == priv || 0 != memcmp(file_name, RES_BIN_FILE_NAME, strlen(file_name))) {
        goto _ERR_RET;
    }
    // 重新分配多一个info结构体，把地址、长度和crc信息填充进入
    __this = zalloc(sizeof(res_bin_file_update_info_t));
    if (NULL == __this) {
        printf("%s, alloc fail\n", __func__);
        goto _ERR_RET;
    }
    memcpy(&__this->info, info, sizeof(user_chip_update_info_t));

    // 再通过update_info_get传入，获取当前的信息，这样做里面就不需要再对info中的priv进行free操作
    if (update_info_get(type, &__this->info, NULL)) {
        printf("%s, get info err\n", __func__);
        goto _ERR_RET;
    }

    // 尝试open设备，并记录相关信息
    if (norflash_f_open(__this->info.len)) {
        printf("%s, open fail\n", __func__);
        goto _ERR_RET;
    }

    // 分配当前链表信息，记录上面的数据，返回当前链表
    base_info = zalloc(sizeof(user_file_base_info_t));
    if (NULL == base_info) {
        printf("%s, alloc base_info fail\n", __func__);
        goto _ERR_RET;
    }

    base_info->get_res_base_info = ex_flash_get_res_base_info;
    base_info->user_chip_release_opt = ex_flash_update_release;
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

#endif
