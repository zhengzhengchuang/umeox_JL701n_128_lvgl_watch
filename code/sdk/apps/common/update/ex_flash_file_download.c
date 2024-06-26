#include "app_config.h"
#include "typedef.h"
#include "fs.h"
#include "norflash.h"
#include "spi/nor_fs.h"
#include "rec_nor/nor_interface.h"
#include "update_loader_download.h"

#if TCFG_SFC_ENCRY_ENABLE
#include "asm/sfc_norflash_api.h"
#endif

#define LOG_TAG "[UP_FILE_DNLD]"
#define LOG_INFO_ENABLE
#define LOG_ERROR_ENABLE
#include "system/debug.h"

#define EX_FLASH_FILE_NAME	"res.bin"
#define EX_FLASH_UPDATE_WRITE_TASK_NAME		"ex_f_update"

#define ONCE_REQ_SIZE		(0x200)
#define FLASH_SECTOR_SIZE	(4096L)
#define __SECTOR_4K_ALIGN(len) (((len) + FLASH_SECTOR_SIZE -1 )/FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE)

#define FLASH_BLOCK_SIZE    (64*1024L)
#define __BLOCK_64K_ALIGN(len) (((len) + FLASH_BLOCK_SIZE -1 )/FLASH_BLOCK_SIZE * FLASH_BLOCK_SIZE)

#define EX_FLASH_ERASE_IN_4K           1
#define EX_FLASH_UPDATE_DEBUG_ENABLE   0
#define EX_FLASH_MUTIL_FILE_UPDATE     0	// 外挂flash升级多个文件，要和库中对应的宏对应起来，默认关闭

#define EX_FLASH_CHECK_EXIT_WIRTE_TASK_TIME		(100) // ms

typedef struct _resource_update_info_t {
    user_chip_update_info_t info;
    void *dev_hdl;
#if EX_FLASH_MUTIL_FILE_UPDATE
    void *dev_hdl_noenc;
#endif
    update_op_api_t *file_ops;
    /* u32 buf[ONCE_REQ_SIZE / 4]; */
    u8 *buf;
    u8 exit_write_task_flag;
} res_update_info_t;

extern const u32 g_res_nor_unencry_start_addr;

static u32 g_once_req_size = ONCE_REQ_SIZE;

#define EX_UPDATE_SEMAPHORE_WAIT_TIME  (500)
static volatile int g_ex_update_timeout = EX_UPDATE_SEMAPHORE_WAIT_TIME;

static res_update_info_t *res_info = NULL;
#define __this (res_info)
static int norflash_f_open(u32 file_len)
{
    int ret = 0;

    g_ex_update_timeout = EX_UPDATE_SEMAPHORE_WAIT_TIME;

    if (__this) {
        __this->dev_hdl = dev_open("res_nor", NULL);
#if EX_FLASH_MUTIL_FILE_UPDATE
        __this->dev_hdl_noenc = dev_open("update_noenc", NULL);
        if (__this->dev_hdl && __this->dev_hdl_noenc) {
            log_info("open dev succ 0x%x, 0x%x\n", __this->dev_hdl, __this->dev_hdl_noenc);
#else // EX_FLASH_MUTIL_FILE_UPDATE
        if (__this->dev_hdl) {
            log_info("open dev succ 0x%x\n", __this->dev_hdl);
#endif // EX_FLASH_MUTIL_FILE_UPDATE
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

#if EX_FLASH_MUTIL_FILE_UPDATE
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
#else // EX_FLASH_MUTIL_FILE_UPDATE
    if (__this && __this->dev_hdl) {
        wdt_clear();
        //printf("%s: addr = 0x%x, rlen = 0x%x", __func__, addr, len);
#if TCFG_SFC_ENCRY_ENABLE
        struct sfc_no_enc_wr r_info = {(u8 *)buf, addr, len};
        if (0 == dev_ioctl(__this->dev_hdl, IOCTL_SFC_NORFLASH_READ_NO_ENC, &r_info)) {
            rlen = len;
        }
#else
        rlen = dev_bulk_read(__this->dev_hdl, buf, addr, len);
#endif
        //put_buf(buff, 128);
    }
#endif // EX_FLASH_MUTIL_FILE_UPDATE

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

#if EX_FLASH_MUTIL_FILE_UPDATE
    if (__this && type) {
        if (__this->dev_hdl && __this->dev_hdl_noenc) {
            wdt_clear();
            if (g_res_nor_unencry_start_addr > addr) {
                dev_ioctl(__this->dev_hdl, type, addr);
            } else {
                dev_ioctl(__this->dev_hdl_noenc, type, addr - g_res_nor_unencry_start_addr);
            }
        }
#else // EX_FLASH_MUTIL_FILE_UPDATE
    if (__this && __this->dev_hdl && type) {
        wdt_clear();
        dev_ioctl(__this->dev_hdl, type, addr);
#endif // EX_FLASH_MUTIL_FILE_UPDATE
    } else {
        log_error("f_erase parm err\n");
    }

    return 0;
}

static u32 norflash_f_write(void *buf, u32 addr, u32 len)
{
    int rlen = 0;

#if EX_FLASH_MUTIL_FILE_UPDATE
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
#else // EX_FLASH_MUTIL_FILE_UPDATE
    if (__this && __this->dev_hdl) {
        wdt_clear();
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
    }
#endif // EX_FLASH_MUTIL_FILE_UPDATE

    return rlen;
}

static int ex_flash_update_file_init(void *priv, update_op_api_t *file_ops)
{
    int ret = 0;

    user_chip_update_info_t *info = (user_chip_update_info_t *)priv;

    if (priv && file_ops) {
        __this = malloc(sizeof(res_update_info_t));
        if (!__this) {
            ret = -1;
            goto _ERR_RET;
        }
        memset((u8 *)__this, 0x00, sizeof(res_update_info_t));
        memcpy((u8 *)&__this->info, (u8 *)info, sizeof(user_chip_update_info_t));
        __this->file_ops = file_ops;

#if EX_FLASH_MUTIL_FILE_UPDATE
        if (info->priv) {
            __this->info.priv = (void *)zalloc(info->files_info_len);
            if (NULL == __this->info.priv) {
                ret = -1;
                goto _ERR_RET;
            }
            memcpy((u8 *)__this->info.priv, info->priv, info->files_info_len);
            log_info("update multiple files to exflash, total_len:%x files_info_len:%x\n", __this->info.len, __this->info.files_info_len);
        } else {
            log_info("ADDR:%x LEN:%x CRC:%x dev_addr:%x\n", __this->info.addr, __this->info.len, __this->info.crc, __this->info.dev_addr);
        }
#else // EX_FLASH_MUTIL_FILE_UPDATE
        log_info("ADDR:%x LEN:%x CRC:%x dev_addr:%x\n", __this->info.addr, __this->info.len, __this->info.crc, __this->info.dev_addr);
#endif // EX_FLASH_MUTIL_FILE_UPDATE

        if (norflash_f_open(__this->info.len)) {
            ret = -3;
        }
    } else {
        log_error("not find target file :%s\n", EX_FLASH_FILE_NAME);
        ret = -2;
        goto _ERR_RET;
    }
#if SMART_BOX_EN
    extern void app_smartbox_prepare_update_ex_flash(void);
    app_smartbox_prepare_update_ex_flash();
#endif

_ERR_RET:
    return ret;
}

static int ex_flash_update_file_get_len(void)
{
    if (__this) {
        return __this->info.len;
    } else {
        return 0;
    }
}
typedef struct _program_info_t {
    u32 remote_file_begin;
    u32 remote_file_length;
    u32 local_program_addr;
    u32(*remote_file_read)(void *buf, u32 addr, u32 len);
    u32(*local_write_hdl)(void *buf, u32 addr, u32 len);
    u32(*local_erase_hdl)(u32 cmd, u32 addr);
    u8 *buf;
} program_info_t;

static u32 remote_file_read(void *buf, u32 addr, u32 len)
{
    u32 ret = 0;

    if (__this && __this->file_ops) {
        putchar('%');
        __this->file_ops->f_seek(NULL, SEEK_SET, addr);
        if ((u16) - 1 == __this->file_ops->f_read(NULL, buf, len)) {
            ret = (u32) - 1;
        }
    }

    return ret;
}

typedef struct _ex_update_exit_write_opt_t {
    void *priv;
    bool (*condition)(void *priv);
    u32 check_period; // ms
    u16 timer_hdl;
} ex_update_exit_write_opt;

typedef struct _ex_update_write_info_t {
    OS_SEM write_sem;
    OS_SEM write_finish_sem;
    u16 write_len;
    u32 write_offset;
    u32 write_program_addr;
    int ret;
    u32(*write_hdl)(void *buf, u32 addr, u32 len);
    u32(*erase_hdl)(u32 cmd, u32 addr);
    u8 *write_buf;
    ex_update_exit_write_opt exit_opt;
    u8 en;
} ex_update_write_info;

static bool exit_write_condition(ex_update_write_info *write_info)
{
    if (write_info) {
        if (write_info->exit_opt.condition) {
            return 	write_info->exit_opt.condition(write_info->exit_opt.priv);
        }
    }
    return false;
}

static void exit_write_opt_prepare(ex_update_write_info *write_info);
static void exit_write_opt_func(void *priv)
{
    ex_update_write_info *write_info = (ex_update_write_info *)priv;
    if (write_info) {
        if (exit_write_condition(write_info)) {
            // post消息到write结束任务等待
            os_sem_post(&write_info->write_sem);
        } else {
            write_info->exit_opt.timer_hdl = 0;
            exit_write_opt_prepare(write_info);
        }
    }
}

static void exit_write_opt_prepare(ex_update_write_info *write_info)
{
    if (write_info && (0 == write_info->exit_opt.timer_hdl)) {
        if (0 == write_info->exit_opt.check_period) {
            write_info->exit_opt.check_period = EX_FLASH_CHECK_EXIT_WIRTE_TASK_TIME;
        }
        write_info->exit_opt.timer_hdl = sys_timeout_add((void *)write_info, exit_write_opt_func, write_info->exit_opt.check_period);
    }
}

static void exit_write_finsh_opt(ex_update_write_info *write_info)
{
    if (write_info) {
        if (write_info->exit_opt.timer_hdl) {
            sys_timeout_del(write_info->exit_opt.timer_hdl);
        }
    }
}

static void ex_update_write_task_exit(ex_update_write_info *write_info)
{
    exit_write_finsh_opt(write_info);
    if (0 == write_info->ret) {
        os_sem_pend(&write_info->write_finish_sem, 0);
    }
    if (write_info->write_buf) {
        free(write_info->write_buf);
    }
    task_kill(EX_FLASH_UPDATE_WRITE_TASK_NAME);
}

#if EX_FLASH_UPDATE_DEBUG_ENABLE
static void flash_info_debug_func(void *buf, u32 addr, u32 len)
{
    // 记录远端传过来的crc
    u16 crc = CRC16((u8 *)buf, len);
    // 清除buffer
    memset((u8 *)buf, 0, len);
    // 从flash中读出刚写入的数据计算crc
    norflash_f_read(buf, addr, len);
    // 打印地址、长度、crc信息
    printf("%x,%x,%x", addr, len, crc);
    // 校验flash的数据
    if (crc != CRC16((u8 *)buf, len)) {
        printf("%s, %d >>>>>>>>>>>>>>>>>error : %x, %x, %x, %x\n", __func__, __LINE__, addr, len, crc, CRC16((u8 *)buf, len));
    }
}
#endif

extern void flash_erase_by_blcok_n_sector(u32 start_addr, u32 len, int (*erase_hdl)(int cmd, u32 addr));
static void ex_update_write_task(void *p)
{
    ex_update_write_info *write_info = (ex_update_write_info *)p;
    u8 begin_flag = 1;
    u32 tmp_addr = 0;
    u32 cur_erase_len = 0;
#if EX_FLASH_ERASE_IN_4K
    u32 erase_addr = __SECTOR_4K_ALIGN(write_info->write_program_addr);
#else
    u32 erase_addr = __BLOCK_64K_ALIGN(write_info->write_program_addr);
#endif
    write_info->en = 1;

    // 检测是否退出写线程timeout定时器
    exit_write_opt_prepare(write_info);

    while (0 == write_info->ret) {

        if (os_sem_pend(&write_info->write_sem, g_ex_update_timeout)) {
            printf("%s timeout\n", __func__);
            write_info->ret = -1;
            break;
        }

        // 如果退出条件满足，则结束写流程，并返回错误
        if (exit_write_condition(write_info)) {
            printf("%s, exit right now\n", __func__);
            write_info->ret = -1;
            break;
        }

        /* if (write_info->write_buf) { */
        /*     memcpy(write_info->write_buf, __this->buf, write_info->write_len); */
        /* } */

        if (write_info->erase_hdl) {
#if EX_FLASH_ERASE_IN_4K
            cur_erase_len = __SECTOR_4K_ALIGN(write_info->write_len);
            tmp_addr = (write_info->write_offset) / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
#else
            cur_erase_len = __BLOCK_64K_ALIGN(write_info->write_len);
            tmp_addr = (write_info->write_offset) / FLASH_BLOCK_SIZE * FLASH_BLOCK_SIZE;
#endif
            if (erase_addr != tmp_addr || begin_flag) {
                if (erase_addr > tmp_addr) {
                    cur_erase_len = erase_addr - tmp_addr;
                }
                erase_addr = tmp_addr;
                log_info("erase_addr:%x len:%d\n", erase_addr, cur_erase_len);
                flash_erase_by_blcok_n_sector(erase_addr, cur_erase_len, write_info->erase_hdl);
                begin_flag = 0;
            }
        }

        if (write_info->write_hdl && write_info->write_buf) {
            if (write_info->write_len != write_info->write_hdl(write_info->write_buf, write_info->write_offset, write_info->write_len)) {
                log_error("write addr:%x len:%x err\n", write_info->write_offset, write_info->write_len);
                write_info->ret = -2;
                break;
            }
        } else {
            log_error("%s basic interface err\n");
            write_info->ret = -1;
            break;
        }

#if EX_FLASH_UPDATE_DEBUG_ENABLE
        flash_info_debug_func(write_info->write_buf, write_info->write_offset, write_info->write_len);
#endif

        if (0 == write_info->ret) {
            os_sem_post(&write_info->write_finish_sem);
        }

        if (write_info->write_offset == write_info->write_program_addr) {
            break;
        }
    }

    os_sem_post(&write_info->write_finish_sem);
    while (1) {
        os_time_dly(10);
    }
}

static int ex_update_write_data(ex_update_write_info *write_info, u32 offset, u16 data_len)
{
    if (0 == write_info->ret) {
        os_sem_pend(&write_info->write_finish_sem, 0);
    }
    write_info->write_offset = offset;
    write_info->write_len = data_len;
    if (write_info->write_buf) {
        memcpy(write_info->write_buf, __this->buf, write_info->write_len);
    }
    os_sem_post(&write_info->write_sem);
    return write_info->ret;
}

static bool ex_update_write_task_exit_condition(void *priv)
{
    return __this->exit_write_task_flag;
}

static int ex_update_write_task_active(ex_update_write_info *write_info, program_info_t *info)
{
    memset(write_info, 0, sizeof(ex_update_write_info));

    write_info->write_buf = zalloc(g_once_req_size);
    if (NULL == write_info->write_buf) {
        printf("%s alloc fail\n");
        return -1;
    }

    if (info->local_write_hdl) {
        write_info->write_hdl = info->local_write_hdl;
    }
    if (info->local_erase_hdl) {
        write_info->erase_hdl = info->local_erase_hdl;
    }

    write_info->write_program_addr = info->local_program_addr;
    os_sem_create(&write_info->write_sem, 0);
    os_sem_create(&write_info->write_finish_sem, 1);

    // 指定写任务立即退出条件
    write_info->exit_opt.condition = ex_update_write_task_exit_condition;

    task_create(ex_update_write_task, (void *)write_info, EX_FLASH_UPDATE_WRITE_TASK_NAME);

    // 保证先创建写线程再进行外挂flash升级
    u32 create_timeout_cnt = EX_UPDATE_SEMAPHORE_WAIT_TIME / 10;
    while ((0 == write_info->en) && (--create_timeout_cnt)) {
        os_time_dly(10);
    }
    if (0 == create_timeout_cnt) {
        printf("%s, create task timeout\n", __func__);
        write_info->ret = -1;
        return -1;
    }
    return 0;
}

static int ex_flash_program_loop(program_info_t *info)
{
    int ret = 0;
    u32 remain_len;
    u32 remote_addr;
    u32 local_addr;

    u32 erase_addr = __SECTOR_4K_ALIGN(info->local_program_addr);
    u32 cur_erase_len = 0;
    /* if (info->local_erase_hdl) { */
    /*     remain_len = __SECTOR_4K_ALIGN(info->remote_file_length); */
    /*     local_addr = __SECTOR_4K_ALIGN(info->local_program_addr); */
    /*     flash_erase_by_blcok_n_sector(local_addr, remain_len, info->local_erase_hdl); */
    /* } */

    remain_len = info->remote_file_length;
    remote_addr = info->remote_file_begin;
    local_addr = info->local_program_addr;

    u32 tmp_addr = 0;
    u16 r_len = 0;

    ex_update_write_info write_info;
    ret = ex_update_write_task_active(&write_info, info);
    if (ret) {
        goto _ERR_RET;
    }

    do {
        r_len = (remain_len % g_once_req_size) ? (remain_len % g_once_req_size) : g_once_req_size;
        remain_len -= r_len;

        if (info->remote_file_read && info->buf && (0 == write_info.ret)) {
            ret = info->remote_file_read(info->buf, remote_addr + remain_len, r_len);
            if (ret != 0) {
                log_error("read addr:%x len:%x err\n", remote_addr + remain_len, r_len);
                ret = -1;
                break;
            }
        }

        if (0 == write_info.ret) {
            ret = ex_update_write_data(&write_info, local_addr + remain_len, r_len);
        }

        if (ret) {
            log_error("write err\n");
            break;
        }
    } while (remain_len);
_ERR_RET:
    if (write_info.ret) {
        ret = write_info.ret;
    }
    ex_update_write_task_exit(&write_info);
    return ret;
}

extern u16 calc_crc16_with_init_val(u16 init_crc, u8 *ptr, u16 len);
static u16 ex_flash_local_file_verify(u8 *buf, u32 addr, u32 len, u32(*read_func)(u8 *buf, u32 addr, u32 len))
{
    u16 crc_temp = 0;
    u16 r_len;

    log_info("verify-addr:%x len:%x\n", addr, len);
    while (len) {
        r_len = (len > g_once_req_size) ? g_once_req_size : len;

        if (read_func) {
            read_func(buf, addr, r_len);
        }
        crc_temp = calc_crc16_with_init_val(crc_temp, buf, r_len);

        addr += r_len;
        len -= r_len;
    }

    return crc_temp;
}

#if EX_FLASH_MUTIL_FILE_UPDATE
static int ex_flash_file_download_all_res_loop(void)
{
    int ret = 0;
    u16 offset = 0;
    u16 files_info_len = 0;
    user_chip_update_info_t *update_info = NULL;
    program_info_t info = {
        .remote_file_read = remote_file_read,
        .local_write_hdl = norflash_f_write,
        .local_erase_hdl = norflash_f_erase,
        .buf = __this->buf,
    };

    if (__this->info.priv) {
        update_info = (user_chip_update_info_t *)__this->info.priv;
        files_info_len = __this->info.files_info_len;
    } else {
        update_info = &__this->info;
    }

    do {
        info.remote_file_begin = update_info->addr;
        info.remote_file_length = update_info->len;
        info.local_program_addr = update_info->dev_addr;
        ret = ex_flash_program_loop(&info);
        if (ret) {
            ret = -1;
            break;
        }

        if (update_info->crc != \
            ex_flash_local_file_verify(__this->buf, update_info->dev_addr, update_info->len, norflash_f_read)) {
            log_error("update crc verify err\n");
            ret = -2;
            break;
        } else {
            log_info("update crc verify succ\n");
        }

        if (files_info_len) {
            update_info++;
            offset += sizeof(user_chip_update_info_t);
        }
    } while (files_info_len > offset);

    return ret;
}
#endif

static int ex_flash_file_download_loop(void *priv)
{
    int ret = 0;

    if (0 == __this->info.len) {
        ret = -1;
        goto _ERR_RET;
    }

    __this->buf = malloc(g_once_req_size);
    if (NULL == __this->buf) {
        ret = -1;
        goto _ERR_RET;
    }
    // 遍历、升级所有的res资源
#if EX_FLASH_MUTIL_FILE_UPDATE
    ret = ex_flash_file_download_all_res_loop();
#else // EX_FLASH_MUTIL_FILE_UPDATE
    program_info_t info = {
        .remote_file_begin = __this->info.addr,
        .remote_file_length = __this->info.len,
        .local_program_addr = __this->info.dev_addr,
        .remote_file_read = remote_file_read,
        .local_write_hdl = norflash_f_write,
        .local_erase_hdl = norflash_f_erase,
        .buf = __this->buf,
    };

    ret = ex_flash_program_loop(&info);
    if (ret) {
        ret = -1;
        goto _ERR_RET;
    }

    if (__this->info.crc != \
        ex_flash_local_file_verify(__this->buf, __this->info.dev_addr, __this->info.len, norflash_f_read)) {
        log_error("update crc verify err\n");
        ret = -2;
    } else {
        log_info("update crc verify succ\n");
    }
#endif // EX_FLASH_MUTIL_FILE_UPDATE

_ERR_RET:
    __this->exit_write_task_flag = 0;
    if (__this->buf) {
        free(__this->buf);
        __this->buf = NULL;
        g_once_req_size = ONCE_REQ_SIZE;
    }
    return ret;
}

#if EX_FLASH_MUTIL_FILE_UPDATE
static int ex_flash_file_release(void)
{
    if (__this) {
        if (__this->info.priv) {
            free(__this->info.priv);
            __this->info.priv = NULL;
        }
        free(__this);
        __this = NULL;
    }
    return 0;
}
#endif

const static user_chip_update_t ex_flash_update_ins = {
    .retry_cnt = 2,
    .file_name = EX_FLASH_FILE_NAME,
    .update_init = ex_flash_update_file_init,
    .update_get_len = ex_flash_update_file_get_len,
    .update_loop = ex_flash_file_download_loop,
#if EX_FLASH_MUTIL_FILE_UPDATE
    .update_release = ex_flash_file_release,
#endif
};

void ex_flash_update_once_req_size_set(u32 pack_size)
{
    g_once_req_size	= pack_size;
}

void ex_flash_file_download_init(void)
{
    register_user_chip_update_handle(&ex_flash_update_ins);
}

void ex_flash_update_len_clear(void)
{
    if (__this && __this->info.len) {
        __this->info.len = 0;
    }
}

void ex_flash_read_timeout_set(int read_timeout)
{
    g_ex_update_timeout = read_timeout;
}

void ex_flash_exit_write_task(void)
{
    if (__this) {
        __this->exit_write_task_flag = 1;
    }
}

