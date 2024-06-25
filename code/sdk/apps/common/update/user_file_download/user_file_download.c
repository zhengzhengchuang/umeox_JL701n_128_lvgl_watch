#include "app_config.h"
#include "typedef.h"
#include "fs.h"
#include "norflash.h"
#include "spi/nor_fs.h"
#include "rec_nor/nor_interface.h"
#include "update_loader_download.h"

#if (USER_FILE_UPDATE_V2_EN)
#define LOG_TAG "[UP_FILE_DNLD]"
#define LOG_INFO_ENABLE
#define LOG_ERROR_ENABLE
#include "system/debug.h"

#define ONCE_REQ_SIZE		(0x200)
#define FLASH_SECTOR_SIZE	(4096L)
#define __SECTOR_4K_ALIGN(len) (((len) + FLASH_SECTOR_SIZE -1 )/FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE)

#define FLASH_BLOCK_SIZE    (64*1024L)
#define __BLOCK_64K_ALIGN(len) (((len) + FLASH_BLOCK_SIZE -1 )/FLASH_BLOCK_SIZE * FLASH_BLOCK_SIZE)

#define USER_FILE_FLASH_ERASE_IN_4K           1
#define USER_FILE_FLASH_UPDATE_DEBUG_ENABLE   0

#define USER_FILE_FLASH_UPDATE_WRITE_TASK_NAME		"ex_f_update"

#define EX_FLASH_CHECK_EXIT_WIRTE_TASK_TIME		(100) // ms

typedef struct _user_file_base_info_t {
    struct list_head 	entry;
    user_chip_update_info_t *(*get_res_base_info)(void *priv,
            u32(**local_read_hdl)(void *buf, u32 addr, u32 len),
            u32(**local_write_hdl)(void *buf, u32 addr, u32 len),
            u32(**local_erase_hdl)(u32 cmd, u32 addr));
    int (*user_chip_release_opt)(user_chip_update_info_t *update_info);
    void *priv;
} user_file_base_info_t;

typedef struct _user_file_update_info_t {
    struct list_head 	list;
    update_op_api_t *file_ops;
    u8 *buf;
    u8 exit_write_task_flag;
    u8 en;
} user_file_update_info_t;

typedef struct _program_info_t {
    u32 remote_file_begin;
    u32 remote_file_length;
    u32 local_program_addr;
    u32(*remote_file_read)(void *buf, u32 addr, u32 len);
    u32(*local_write_hdl)(void *buf, u32 addr, u32 len);
    u32(*local_erase_hdl)(u32 cmd, u32 addr);
    u8 *buf;
} program_info_t;

static u32 g_once_req_size = ONCE_REQ_SIZE;

#define USER_FILE_UPDATE_SEMAPHORE_WAIT_TIME  (500)
static volatile int g_user_file_update_timeout = USER_FILE_UPDATE_SEMAPHORE_WAIT_TIME;

static user_file_update_info_t *user_info = NULL;
#define __this (user_info)

extern struct list_head *reserve_file_update_init(void *priv, u8 type, u8 *file_name, int (*update_info_get)(u8 type, user_chip_update_info_t *info, void *priv));
extern struct list_head *ext_flash_file_update_init(void *priv, u8 type, u8 *file_name, int (*update_info_get)(u8 type, user_chip_update_info_t *info, void *priv));

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

static int user_file_update_init(void *priv, update_op_api_t *file_ops, u8 type, u8 *file_name, int (*update_info_get)(u8 type, user_chip_update_info_t *info, void *priv))
{
    int ret = -1;
    struct list_head *update_entry = NULL;

    update_entry = ext_flash_file_update_init(priv, type, file_name, update_info_get);

    if (NULL == update_entry) {
        update_entry = reserve_file_update_init(priv, type, file_name, update_info_get);
    }

    if (update_entry && file_ops) {
        if (NULL == __this) {
            // 分配、初始化
            __this = zalloc(sizeof(user_file_update_info_t));
            if (NULL == __this) {
                printf("%s, alloc fail\n", __func__);
                goto _ERR_RET;
            }
            INIT_LIST_HEAD(&__this->list);
            __this->file_ops = file_ops;
            __this->en = 1;
            g_user_file_update_timeout = USER_FILE_UPDATE_SEMAPHORE_WAIT_TIME;
#if SMART_BOX_EN
            extern void app_smartbox_prepare_update_ex_flash(void);
            app_smartbox_prepare_update_ex_flash();
#endif
        }
        // 入链表
        list_add_tail(update_entry, &__this->list);
        ret = 0;
    }

_ERR_RET:
    return ret;
}

static int user_file_update_get_len(void)
{
    // 遍历链表，获取获取并累加所有长度
    int user_file_total_len = 0;
    user_file_base_info_t *base_info = NULL;
    user_chip_update_info_t *update_info_entry = NULL;
    if (__this && !list_empty(&__this->list)) {
        list_for_each_entry(base_info, &__this->list, entry) {
            if (base_info->get_res_base_info && base_info->priv) {
                update_info_entry = base_info->get_res_base_info(base_info->priv, NULL, NULL, NULL);
                if (update_info_entry) {
                    user_file_total_len += update_info_entry->len;
                }
            }
        }
    }
    return user_file_total_len;
}

typedef struct _ex_update_exit_write_opt_t {
    void *priv;
    bool (*condition)(void *priv);
    u32 check_period; // ms
    u16 timer_hdl;
} user_update_exit_write_opt;

typedef struct _user_file_update_write_info_t {
    OS_SEM write_sem;
    OS_SEM write_finish_sem;
    u16 write_len;
    u32 write_offset;
    u32 write_program_addr;
    int ret;
    u32(*write_hdl)(void *buf, u32 addr, u32 len);
    u32(*erase_hdl)(u32 cmd, u32 addr);
    u8 *write_buf;
    user_update_exit_write_opt exit_opt;
    u8 en;
} user_file_update_write_info_t;

static bool exit_write_condition(user_file_update_write_info_t *write_info)
{
    if (write_info) {
        if (write_info->exit_opt.condition) {
            return write_info->exit_opt.condition(write_info->exit_opt.priv);
        }
    }
    return false;
}

static void exit_write_opt_prepare(user_file_update_write_info_t *write_info);
static void exit_write_opt_func(void *priv)
{
    user_file_update_write_info_t *write_info = (user_file_update_write_info_t *)priv;
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

static void exit_write_opt_prepare(user_file_update_write_info_t *write_info)
{
    if (write_info && (0 == write_info->exit_opt.timer_hdl)) {
        if (0 == write_info->exit_opt.check_period) {
            write_info->exit_opt.check_period = EX_FLASH_CHECK_EXIT_WIRTE_TASK_TIME;
        }
        write_info->exit_opt.timer_hdl = sys_timeout_add((void *)write_info, exit_write_opt_func, write_info->exit_opt.check_period);
    }
}

static void exit_write_finsh_opt(user_file_update_write_info_t *write_info)
{
    if (write_info) {
        if (write_info->exit_opt.timer_hdl) {
            sys_timeout_del(write_info->exit_opt.timer_hdl);
        }
    }
}

static void user_file_update_write_task_exit(user_file_update_write_info_t *write_info)
{
    exit_write_finsh_opt(write_info);
    if (0 == write_info->ret) {
        os_sem_pend(&write_info->write_finish_sem, 0);
    }
    if (write_info->write_buf) {
        free(write_info->write_buf);
    }
    task_kill(USER_FILE_FLASH_UPDATE_WRITE_TASK_NAME);
}

#if USER_FILE_FLASH_UPDATE_DEBUG_ENABLE
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
static void user_file_update_write_task(void *p)
{
    user_file_update_write_info_t *write_info = (user_file_update_write_info_t *)p;
    u8 begin_flag = 1;
    u32 tmp_addr = 0;
    u32 cur_erase_len = 0;
    u32 cur_erase_unit = 0;
#if USER_FILE_FLASH_ERASE_IN_4K
    u32 erase_addr = __SECTOR_4K_ALIGN(write_info->write_program_addr);
#else
    u32 erase_addr = __BLOCK_64K_ALIGN(write_info->write_program_addr);
#endif
    write_info->en = 1;

    // 检测是否退出写线程timeout定时器
    exit_write_opt_prepare(write_info);

    while (0 == write_info->ret) {

        if (os_sem_pend(&write_info->write_sem, g_user_file_update_timeout)) {
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
#if USER_FILE_FLASH_ERASE_IN_4K
            cur_erase_len = __SECTOR_4K_ALIGN(write_info->write_len);
            tmp_addr = (write_info->write_offset) / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
            cur_erase_unit = FLASH_SECTOR_SIZE;
#else
            cur_erase_len = __BLOCK_64K_ALIGN(write_info->write_len);
            tmp_addr = (write_info->write_offset) / FLASH_BLOCK_SIZE * FLASH_BLOCK_SIZE;
            cur_erase_unit = FLASH_BLOCK_SIZE;
#endif
            // 防止当前升级数据只有一包，而且大小小于擦除单位的情况下，不擦除导致升级失败
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

#if USER_FILE_FLASH_UPDATE_DEBUG_ENABLE
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

static int user_file_update_write_data(user_file_update_write_info_t *write_info, u32 offset, u16 data_len)
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

static bool user_file_write_task_exit_condition(void *priv)
{
    return __this->exit_write_task_flag;
}

static int user_file_update_write_task_active(user_file_update_write_info_t *write_info, program_info_t *info)
{
    memset(write_info, 0, sizeof(user_file_update_write_info_t));

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
    write_info->exit_opt.condition = user_file_write_task_exit_condition;

    task_create(user_file_update_write_task, (void *)write_info, USER_FILE_FLASH_UPDATE_WRITE_TASK_NAME);

    // 保证先创建写线程再进行外挂flash升级
    u32 create_timeout_cnt = USER_FILE_UPDATE_SEMAPHORE_WAIT_TIME / 10;
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

static int user_file_flash_program_loop(program_info_t *info)
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

    user_file_update_write_info_t write_info;
    ret = user_file_update_write_task_active(&write_info, info);
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
            ret = user_file_update_write_data(&write_info, local_addr + remain_len, r_len);
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
    user_file_update_write_task_exit(&write_info);
    return ret;
}

extern u16 calc_crc16_with_init_val(u16 init_crc, u8 *ptr, u16 len);
static u16 user_file_flash_local_file_verify(u8 *buf, u32 addr, u32 len, u32(*read_func)(u8 *buf, u32 addr, u32 len))
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

static int user_file_download_all_res_loop(program_info_t *info, user_chip_update_info_t *update_info_entry, u32(*verify_data_read_func)(void *buf, u32 addr, u32 len))
{
    int ret = 0;
    u16 offset = 0;
    u16 files_info_len = 0;
    user_chip_update_info_t *update_info = NULL;

    if (0 == update_info_entry->len) {
        return ret;
    }

    if (update_info_entry->priv) {
        update_info = (user_chip_update_info_t *)update_info_entry->priv;
        files_info_len = update_info_entry->files_info_len;
    } else {
        update_info = update_info_entry;
    }

    do {
        info->remote_file_begin = update_info->addr;
        info->remote_file_length = update_info->len;
        info->local_program_addr = update_info->dev_addr;
        ret = user_file_flash_program_loop(info);
        if (ret) {
            ret = -1;
            break;
        }

        if (update_info->crc != \
            user_file_flash_local_file_verify(__this->buf, update_info->dev_addr, update_info->len, verify_data_read_func)) {
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

static int user_file_download_loop(void *priv)
{
    int ret = 0;
    user_file_base_info_t *n = NULL;
    user_file_base_info_t *base_info = NULL;
    user_chip_update_info_t *update_info_entry = NULL;
    u32(*verify_data_read_func)(void *buf, u32 addr, u32 len) = NULL;
    program_info_t info;
    if (0 == __this->en) {
        ret = -1;
        goto _ERR_RET;
    }
    __this->buf = zalloc(g_once_req_size);
    if (NULL == __this->buf) {
        ret = -1;
        goto _ERR_RET;
    }

    info.remote_file_read = remote_file_read;
    info.buf = __this->buf;
    // 遍历链表，升级成功就把对应节点删掉，可以做到不重复升级的效果，但对应的内存要是否掉
    list_for_each_entry_safe(base_info, n, &__this->list, entry) {
        if (base_info->get_res_base_info && base_info->priv) {
            update_info_entry = base_info->get_res_base_info(base_info->priv, &verify_data_read_func, &info.local_write_hdl, &info.local_erase_hdl);
            // 遍历、升级所有的res资源
            ret = user_file_download_all_res_loop(&info, update_info_entry, verify_data_read_func);
            if (0 == ret) {
                if (base_info->user_chip_release_opt) {
                    base_info->user_chip_release_opt(update_info_entry);
                }
                /* user_file_update_release_each_entry(update_info_entry); */
                list_del(&base_info->entry);
                free(base_info);
                base_info = NULL;
            }
        }
    }

_ERR_RET:
    __this->exit_write_task_flag = 0;
    if (__this->buf) {
        free(__this->buf);
        __this->buf = NULL;
        g_once_req_size = ONCE_REQ_SIZE;
    }
    return ret;
}

static int user_file_update_release(void *priv)
{
    user_file_base_info_t *base_info = NULL;
    user_file_base_info_t *n = NULL;
    user_chip_update_info_t *update_info_entry = NULL;
    // 遍历链表，获取所有元素，并逐个free
    if (__this && !list_empty(&__this->list)) {
        list_for_each_entry_safe(base_info, n, &__this->list, entry) {
            if (base_info->get_res_base_info && base_info->priv) {
                update_info_entry = base_info->get_res_base_info(base_info->priv, NULL, NULL, NULL);
                if (base_info->user_chip_release_opt) {
                    base_info->user_chip_release_opt(update_info_entry);
                }
                /* user_file_update_release_each_entry(update_info_entry); */
                list_del(&base_info->entry);
                free(base_info);
                base_info = NULL;
            }
        }
        free(__this);
        __this = NULL;
    }
    return 0;
}

const static user_chip_update_v2_t user_file_update_ins = {
    .retry_cnt = 2,
    .update_init = user_file_update_init,
    .update_get_len = user_file_update_get_len,
    .update_loop = user_file_download_loop,
    .update_release = user_file_update_release,
};

void user_file_flash_update_once_req_size_set(u32 pack_size)
{
    g_once_req_size	= pack_size;
}

void user_file_flash_read_timeout_set(int read_timeout)
{
    g_user_file_update_timeout = read_timeout;
}

void user_file_flash_update_len_clear(void)
{
    if (__this) {
        __this->en = 0;
    }
}

void user_file_exit_write_task(void)
{
    if (__this) {
        __this->exit_write_task_flag = 1;
    }
}

void user_file_flash_file_download_init(void)
{
    register_user_chip_update_v2_handle(&user_file_update_ins);
}

#else

void user_file_flash_file_download_init(void)
{

}

#endif
