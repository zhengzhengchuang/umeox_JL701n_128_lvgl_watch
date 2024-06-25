#include "includes.h"
#include "app_config.h"
#include "norflash_sfc.h"

#if (defined(TCFG_NORFLASH_SFC_DEV_ENABLE) && TCFG_NORFLASH_SFC_DEV_ENABLE)
#include "asm/sfc_norflash_api.h"

#undef LOG_TAG_CONST
#define LOG_TAG     "[FLASH_SFC]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#include "debug.h"

#define AT_RAM      AT(.norflash_sfc_code)

#define USER_MASK_TYPE       (BIT(16)|BIT(17))//删消息池使用
static u16 last_id = 1;//删消息池使用

#define FLASH_POWERON_TIME       (1000)//1ms  flash 上电使用延时
#define FLASH_EXIT_LOWPOWER_TIME  (200)//200us  退出低功耗使用延时
#define FLASH_POWEROFF_TIME      (200)//200us  掉电延时


 void norflash_flash_power_check();

#define MAX_NORFLASH_PART_NUM       4

struct norflash_partition {
    const char *name;
    u32 start_addr;
    u32 size;
    struct device device;
};

static struct norflash_partition nor_part[MAX_NORFLASH_PART_NUM];

struct norflash_info {
    u32 flash_id;
    u32 flash_capacity;
    int spi_num;
    int spi_err;
    u8 spi_cs_io;
    u8 spi_r_width;
    u8 part_num;
    u8 open_cnt;
    struct norflash_partition *const part_list;
    OS_MUTEX mutex;
    u32 max_end_addr;
};

static struct norflash_info _norflash = {
    .spi_num = (int) - 1,
    .part_list = nor_part,
};

int fat_sfc_norflash_read(u32 addr, u8 *buf, u32 len, u8 cache);
static int _norflash_sfc_ioctl_eraser(u8 eraser, u32 addr);
static void _norflash_cache_sync_timer(void *priv);
static int _norflash_write_pages(u32 addr, u8 *buf, u32 len);

static struct norflash_partition *norflash_find_part(const char *name)
{
    struct norflash_partition *part = NULL;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            continue;
        }
        if (!strcmp(part->name, name)) {
            return part;
        }
    }
    return NULL;
}

static struct norflash_partition *norflash_new_part(const char *name, u32 addr, u32 size)
{
    struct norflash_partition *part;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            break;
        }
    }
    if (part->name != NULL) {
        log_error("create norflash part fail\n");
        return NULL;
    }
    memset(part, 0, sizeof(*part));
    part->name = name;
    part->start_addr = addr;
    part->size = size;
    if (part->start_addr + part->size > _norflash.max_end_addr) {
        _norflash.max_end_addr = part->start_addr + part->size;
    }
    _norflash.part_num++;
    return part;
}

static void norflash_delete_part(const char *name)
{
    struct norflash_partition *part;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL) {
            continue;
        }
        if (!strcmp(part->name, name)) {
            part->name = NULL;
            _norflash.part_num--;
        }
    }
}

static int norflash_verify_part(struct norflash_partition *p)
{
    struct norflash_partition *part = NULL;
    u32 idx;
    for (idx = 0; idx < MAX_NORFLASH_PART_NUM; idx++) {
        part = &_norflash.part_list[idx];
        if (part->name == NULL ||
            0 == strcmp(p->name, "update_noenc") ||
            0 == strcmp(part->name, "update_noenc")) {
            continue;
        }
        if ((p->start_addr >= part->start_addr) && (p->start_addr < part->start_addr + part->size)) {
            if (strcmp(p->name, part->name) != 0) {
                return -1;
            }
        }
    }
    return 0;
}



#define FLASH_CACHE_ENABLE  		1

#if FLASH_CACHE_ENABLE
static u32 flash_cache_addr;
static u8 *flash_cache_buf = NULL; //缓存4K的数据，与flash里的数据一样。
static u8 flash_cache_is_dirty;
static u16 flash_cache_timer;

#define FLASH_CACHE_SYNC_T_INTERVAL     60

static int _check_0xff(u8 *buf, u32 len)
{
    for (u32 i = 0; i < len; i ++) {
        if ((*(buf + i)) != 0xff) {
            return 1;
        }
    }
    return 0;
}
#endif


static u32 _pow(u32 num, int n)
{
    u32 powint = 1;
    int i;
    for (i = 1; i <= n; i++) {
        powint *= num;
    }
    return powint;
}

static u32 _norflash_read_id()
{
    return sfc_spi_read_id();
}

AT_RAM
static u32 norflash_wait_ok()
{
    while (1) {

        sfc1_spi_cs(0);
        sfc1_spi_write_byte(WINBOND_READ_SR1);

        u8 reg_1 = sfc1_spi_read_byte();
        sfc1_spi_cs(1);

        if (!(reg_1 & BIT(0))) {
            break;
        }
        udelay(100);//100us
    }


    return 0;
}
AT_RAM
void norflash_send_write_enable()
{
    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_WRITE_ENABLE);
    sfc1_spi_cs(1);
}
AT_RAM
static void norflash_set_quad(u8 enable)
{
    u8 status_reg1, status_reg2;
    u8 retry_cnt = 3;

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_READ_SR1);
    status_reg1 = sfc1_spi_read_byte();
    sfc1_spi_cs(1);
    printf("read status_reg1: 0x%x\n", status_reg1);

    const u8 w_reg1 = status_reg1 & 0xfc; // clear WEL WIP

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_READ_SR2);
    status_reg2 = sfc1_spi_read_byte();
    sfc1_spi_cs(1);
    printf("read status_reg2: 0x%x\n", status_reg2);

    if ((!!(status_reg2 & BIT(1))) == enable) {
        printf("norflash_set_quad succ\n");
        return ;
    }

    if (enable) {
        status_reg2 |= BIT(1);
    } else {
        status_reg2 &= ~BIT(1);
    }

    const u8 w_reg2 = status_reg2;

    //write reg 连续写两个寄存器
    norflash_send_write_enable();

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_WRITE_SR1);
    sfc1_spi_write_byte(w_reg1);
    sfc1_spi_write_byte(w_reg2);
    sfc1_spi_cs(1);
    norflash_wait_ok();

__CHECK_FLASH:
    //check reg
    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_READ_SR1);
    status_reg1 = sfc1_spi_read_byte();
    sfc1_spi_cs(1);
    printf("check status_reg1: 0x%x\n", status_reg1);

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_READ_SR2);
    status_reg2 = sfc1_spi_read_byte();
    sfc1_spi_cs(1);
    printf("check status_reg2: 0x%x\n", status_reg2);
    status_reg2 = !!(status_reg2 & BIT(1));
    if (status_reg2  == enable) {
        printf("norflash_set_quad succ\n");
        return ;
    } else {
        if (retry_cnt--) {

        } else {
            printf("norflash_set_quad failed \n");
            return ;
        }
    }

    //write reg  分开两次写寄存器，博雅flash不支持连续写
    norflash_send_write_enable();

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_WRITE_SR1);
    sfc1_spi_write_byte(w_reg1);
    sfc1_spi_cs(1);
    norflash_wait_ok();

    norflash_send_write_enable();
    sfc1_spi_cs(0);
    sfc1_spi_write_byte(WINBOND_WRITE_SR2);
    sfc1_spi_write_byte(w_reg2);
    sfc1_spi_cs(1);
    norflash_wait_ok();
    goto __CHECK_FLASH;
}



#include "xt25f32fsoigt.c"
#include "xt25f64bssigu-s.c"
#include "xt25f128fssigu.c"
#define     NORFLASH_DRV_ENABLE     0
AT_RAM
static u32 sfc1_flash_ext_init(u32 data_width)
{

    u32 flash_id = 0;
    //TODO:add ex flash status register

    //power release
    sfc1_spi_cs(0);
    sfc1_spi_write_byte(0xab);
    //dummy
    sfc1_spi_read_byte();
    sfc1_spi_read_byte();
    sfc1_spi_read_byte();
    //ID
    sfc1_spi_read_byte();
    sfc1_spi_cs(1);
    //根据flash规格书修改tRES1
    //一般是10us左右，
    udelay(100);

    //soft  reset
    sfc1_spi_cs(0);
    sfc1_spi_write_byte(0x66);
    sfc1_spi_cs(1);

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(0x99);
    sfc1_spi_cs(1);

    //soft tReady 部分芯片有50ms
    os_time_dly(2);

    norflash_wait_ok();

    sfc1_spi_cs(0);
    sfc1_spi_write_byte(0x9f);
    for (int i = 0; i < 3; i++) {
        flash_id |= sfc1_spi_read_byte();
        flash_id <<= 8;
    }
    sfc1_spi_cs(1);
    flash_id >>= 8;
#if NORFLASH_DRV_ENABLE
    if (flash_id == 0xb4012) {
        xt25f32fsoigt_norflash_config_drv();
    } else if (flash_id == 0xb4017) {
        xt25f64bssigu_s_norflash_config_drv();
    } else if (flash_id == 0xb4018) {
        xt25f128fssigu_norflash_config_drv();
    }
#endif
    if (data_width == 4) {
        norflash_set_quad(1);
    }
    return flash_id;
}
static int _norflash_init(const char *name, struct norflash_sfc_dev_platform_data *pdata)
{
    printf("norflash_sfc_init >>>>");

    struct norflash_partition *part;
    if (_norflash.spi_num == (int) - 1) {
        _norflash.spi_num = 1;//
        _norflash.flash_id = 0;
        _norflash.flash_capacity = 0;
        os_mutex_create(&_norflash.mutex);
        _norflash.max_end_addr = 0;
        _norflash.part_num = 0;
        // gpio_set_hd0(IO_PORTC_01, 1);
        // gpio_set_hd(IO_PORTC_01, 1);
        sfc_spi_init((struct sfc_spi_platform_data *)pdata->sfc_spi_pdata);
        u32 id = sfc1_flash_ext_init(((struct sfc_spi_platform_data *)pdata->sfc_spi_pdata)->sfc_data_width);
        printf("ext_flash id %x", id);
    }

    part = norflash_find_part(name);

    if (!part) {
        part = norflash_new_part(name, pdata->start_addr, pdata->size);
        ASSERT(part, "not enough norflash partition memory in array\n");
        ASSERT(norflash_verify_part(part) == 0, "norflash partition %s overlaps\n", name);
        log_info("norflash new partition %s\n", part->name);
    } else {
        ASSERT(0, "norflash partition name already exists\n");
    }
    return 0;
}

static void clock_critical_enter()
{

}
static void clock_critical_exit()
{
    /* if (!(_norflash.flash_id == 0 || _norflash.flash_id == 0xffff)) { */
    /* spi_set_baud(_norflash.spi_num, spi_get_baud(_norflash.spi_num)); */
    /* } */
}
CLOCK_CRITICAL_HANDLE_REG(spi_norflash, clock_critical_enter, clock_critical_exit);

static int _norflash_open(void *arg)
{
    int reg = 0;
    os_mutex_pend(&_norflash.mutex, 0);
    log_info("norflash open\n");
    sfc_spi_open();
    if (!_norflash.open_cnt) {
        _norflash.flash_id = _norflash_read_id();

        log_info("norflash_read_id: 0x%x\n", _norflash.flash_id);
        if ((_norflash.flash_id == 0) || (_norflash.flash_id == 0xffffff)) {
            log_error("read norflash id error !\n");
            reg = -ENODEV;
            goto __exit;
        }
        _norflash.flash_capacity = 64 * _pow(2, (_norflash.flash_id & 0xff) - 0x10) * 1024;
        log_info("norflash_capacity: 0x%x\n", _norflash.flash_capacity);

#if FLASH_CACHE_ENABLE
        flash_cache_buf = (u8 *)malloc(4096);
        ASSERT(flash_cache_buf, "flash_cache_buf is not ok\n");
        flash_cache_addr = 4096;//先给一个大于4096的数
        fat_sfc_norflash_read(0, flash_cache_buf, 4096, 1);
        flash_cache_addr = 0;
#endif
        log_info("norflash open success !\n");
    }
    if (_norflash.flash_id == 0 || _norflash.flash_id == 0xffffff)  {
        log_error("re-open norflash id error !\n");
        reg = -EFAULT;
        goto __exit;
    }
    ASSERT(_norflash.max_end_addr <= _norflash.flash_capacity, "max partition end address is greater than flash capacity\n");
    _norflash.open_cnt++;

__exit:
    os_mutex_post(&_norflash.mutex);
    return reg;
}

static int _norflash_close(void)
{
    os_mutex_pend(&_norflash.mutex, 0);

    log_info("norflash close\n");
    if (_norflash.open_cnt) {
        _norflash.open_cnt--;
    }
    if (!_norflash.open_cnt) {
#if FLASH_CACHE_ENABLE
        if (flash_cache_is_dirty) {
            flash_cache_is_dirty = 0;
            _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, flash_cache_addr);
            _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
        }
        free(flash_cache_buf);
        flash_cache_buf = NULL;
#endif
        log_info("norflash close done\n");
    }
    os_mutex_post(&_norflash.mutex);
    return 0;
}



int fat_sfc_norflash_read(u32 addr, u8 *buf, u32 len, u8 cache)
{
    int ret = 0;
    os_mutex_pend(&_norflash.mutex, 0);

#if FLASH_CACHE_ENABLE
    if (!cache) {
        goto __no_cache1;
    }
    u32 r_len = 4096 - (addr % 4096);
    if ((addr >= flash_cache_addr) && (addr < (flash_cache_addr + 4096))) {
        if (len <= r_len) {
            memcpy(buf, flash_cache_buf + (addr - flash_cache_addr), len);
            ret = len;
            goto __exit;
        } else {
            memcpy(buf, flash_cache_buf + (addr - flash_cache_addr), r_len);
            addr += r_len;
            buf += r_len;
            len -= r_len;
        }
    }
__no_cache1:

#endif

    norflash_flash_power_check();
    ret = sfc_spi_read(addr, buf, len);

__exit:
    os_mutex_post(&_norflash.mutex);
    if (ret != len) {
        ret = -1;
    } else {
        ret = 0;
    }

    return ret;
}

static int _norflash_write_pages(u32 addr, u8 *buf, u32 len)
{
    /* y_printf("flash write addr = %d, num = %d\n", addr, len); */
    int ret = 0;
    ret = sfc_spi_write_pages(addr, buf, len);
    if (ret != len) {
        ret = -1;
    } else {
        ret = 0;
    }

    return ret;
}

#if FLASH_CACHE_ENABLE
static void _norflash_cache_sync_timer(void *priv)
{
    int reg = 0;
    os_mutex_pend(&_norflash.mutex, 0);
    if (flash_cache_is_dirty) {
        flash_cache_is_dirty = 0;
        reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, flash_cache_addr);
        if (reg) {
            goto __exit;
        }
        reg = _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
    }
    if (flash_cache_timer) {
        sys_timeout_del(flash_cache_timer);
        flash_cache_timer = 0;
    }
__exit:
    os_mutex_post(&_norflash.mutex);
}
#endif

static int _norflash_write(u32 addr, void *buf, u32 len, u8 cache)
{
    //printf("%s start\n", __func__);

    int reg = 0;
    os_mutex_pend(&_norflash.mutex, 0);
    norflash_flash_power_check();

    u8 *w_buf = (u8 *)buf;
    u32 w_len = len;

    //printf("flash write addr = %x, num = %d\n", addr, len);
#if FLASH_CACHE_ENABLE
    if (!cache) {
        reg = _norflash_write_pages(addr, w_buf, w_len);
        //printf("0000000000000000000\n");
        goto __exit;
    }
    u32 align_addr = addr / 4096 * 4096;
    u32 align_len = 4096 - (addr - align_addr);
    align_len = w_len > align_len ? align_len : w_len;
    //printf("align_addr = %d, align_len = %d\n", align_addr, align_len);
    if (align_addr != flash_cache_addr) {
        if (flash_cache_is_dirty) {
            flash_cache_is_dirty = 0;
            reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, flash_cache_addr);
            if (reg) {
                line_inf;
                goto __exit;
            }
            reg = _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
            if (reg) {
                line_inf;
                goto __exit;
            }
        }
        fat_sfc_norflash_read(align_addr, flash_cache_buf, 4096, 0);
        flash_cache_addr = align_addr;
    }

    //printf("11111111111111111111111111111\n");
    memcpy(flash_cache_buf + (addr - align_addr), w_buf, align_len);
    if ((addr + align_len) % 4096) {
        flash_cache_is_dirty = 1;
        if (flash_cache_timer) {
            sys_timer_re_run(flash_cache_timer);
        } else {
            flash_cache_timer = sys_timeout_add(0, _norflash_cache_sync_timer, FLASH_CACHE_SYNC_T_INTERVAL);
        }
    } else {
        flash_cache_is_dirty = 0;
        reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, align_addr);
        if (reg) {
            line_inf;
            goto __exit;
        }
        reg = _norflash_write_pages(align_addr, flash_cache_buf, 4096);
        if (reg) {
            line_inf;
            goto __exit;
        }
    }

    //printf("22222222222222222222222222222\n");
    addr += align_len;
    w_buf += align_len;
    w_len -= align_len;
    while (w_len) {
        u32 cnt = w_len > 4096 ? 4096 : w_len;
        fat_sfc_norflash_read(addr, flash_cache_buf, 4096, 0);
        flash_cache_addr = addr;
        memcpy(flash_cache_buf, w_buf, cnt);
        if ((addr + cnt) % 4096) {
            flash_cache_is_dirty = 1;
            if (flash_cache_timer) {
                sys_timer_re_run(flash_cache_timer);
            } else {
                flash_cache_timer = sys_timeout_add(0, _norflash_cache_sync_timer, FLASH_CACHE_SYNC_T_INTERVAL);
            }
        } else {
            flash_cache_is_dirty = 0;
            reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, addr);
            if (reg) {
                line_inf;
                goto __exit;
            }
            reg = _norflash_write_pages(addr, flash_cache_buf, 4096);
            if (reg) {
                line_inf;
                goto __exit;
            }
        }
        addr += cnt;
        w_buf += cnt;
        w_len -= cnt;
    }

    //printf("33333333333333333333333333333\n");
#else
    reg = _norflash_write_pages(addr, w_buf, w_len);
#endif
__exit:
    os_mutex_post(&_norflash.mutex);

    //printf("%s end\n", __func__);
    return reg;
}

static int _norflash_sfc_ioctl_eraser(u8 eraser, u32 addr)
{
    int ret = 0;

    //printf("%s:%x\n", __func__, addr);

    ret = sfc_spi_eraser(eraser, addr);

    //printf("______%s\n", __func__);

    return ret;
}

static int _norflash_ioctl(u32 cmd, u32 arg, u32 unit, void *_part)
{
    u32 rets;//, reti;
    __asm__ volatile("%0 = rets":"=r"(rets));

    int reg = 0;
    struct norflash_partition *part = _part;
    struct sfc_no_enc_wr *no_enc_wr;
    os_mutex_pend(&_norflash.mutex, 0);
    norflash_flash_power_check();
    switch (cmd) {
    case IOCTL_GET_STATUS:
        *(u32 *)arg = 1;
        break;
    case IOCTL_GET_ID:
        *((u32 *)arg) = _norflash.flash_id;
        break;
    case IOCTL_GET_CAPACITY:
        if (_norflash.flash_capacity == 0)  {
            *(u32 *)arg = 0;
        } else if (_norflash.part_num == 1 && part->start_addr == 0) {
            *(u32 *)arg = _norflash.flash_capacity / unit;
        } else {
            *(u32 *)arg = part->size / unit;
        }
        break;
    case IOCTL_GET_BLOCK_SIZE:
        *(u32 *)arg = 512;
        break;
    case IOCTL_ERASE_PAGE:
        reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_PAGE, arg * unit + part->start_addr);
        break;
    case IOCTL_ERASE_SECTOR:
        printf("arg = %x, unit = %d, size = %x\n", arg, unit, part->size);
        printf("start_addr = %x\n", part->start_addr);
        if ((arg * unit) + 4096 > part->size) {
            log_error("__func__ %s %x %x %x\n", __func__, rets, arg, unit);
            ASSERT(0);
        }
        reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, arg * unit + part->start_addr);
        printf("IOCTL_ERASE_SECTOR end\n");
        break;
    case IOCTL_ERASE_BLOCK:
        if ((arg * unit) + (64 * 1024) > part->size) {
            log_error("__func__ %s %x %x %x\n", __func__, rets, arg, unit);
            ASSERT(0);
        }
        reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_BLOCK, arg * unit + part->start_addr);
        break;
    case IOCTL_ERASE_CHIP:
        reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_CHIP, 0);
        break;
    case IOCTL_FLUSH:
#if FLASH_CACHE_ENABLE
        if (flash_cache_is_dirty) {
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            flash_cache_is_dirty = 0;
            reg = _norflash_sfc_ioctl_eraser(IOCTL_ERASE_SECTOR, flash_cache_addr);
            if (reg) {
                goto __exit;
            }
            reg = _norflash_write_pages(flash_cache_addr, flash_cache_buf, 4096);
        }
#endif
        break;
    case IOCTL_CMD_RESUME:
        break;
    case IOCTL_CMD_SUSPEND:
        break;
    case IOCTL_GET_PART_INFO:
        u32 *info = (u32 *)arg;
        u32 *start_addr = &info[0];
        u32 *part_size = &info[1];
        *start_addr = part->start_addr;
        *part_size = part->size;
        break;
    case IOCTL_SFC_NORFLASH_READ_NO_ENC:
        no_enc_wr = (struct sfc_no_enc_wr *)arg;
        reg = sfc_spi_read_no_enc(no_enc_wr->addr, no_enc_wr->buf, no_enc_wr->len);
        break;
    case IOCTL_SFC_NORFLASH_WRITE_NO_ENC:
        no_enc_wr = (struct sfc_no_enc_wr *)arg;
        sfc_spi_write_enc_set(0);
        reg = _norflash_write_pages(no_enc_wr->addr, no_enc_wr->buf, no_enc_wr->len);
        sfc_spi_write_enc_set(1);
        break;
    case IOCTL_GET_CURR_ERASE_STATE:
        reg = _norflash_sfc_ioctl_eraser(IOCTL_GET_CURR_ERASE_STATE, 0);
        break;
    default:
        reg = -EINVAL;
        break;
    }
__exit:
    os_mutex_post(&_norflash.mutex);
    return reg;
}


/*************************************************************************************
 *                                  挂钩 device_api
 ************************************************************************************/

static int norflash_sfc_dev_init(const struct dev_node *node, void *arg)
{
    struct norflash_sfc_dev_platform_data *pdata = arg;

    return _norflash_init(node->name, pdata);
}

static int norflash_sfc_dev_open(const char *name, struct device **device, void *arg)
{
    struct norflash_partition *part;
    part = norflash_find_part(name);
    if (!part) {
        log_error("no norflash partition is found\n");
        return -ENODEV;
    }
    *device = &part->device;
    (*device)->private_data = part;
    if (atomic_read(&part->device.ref)) {
        return 0;
    }
    return _norflash_open(arg);
}
static int norflash_sfc_dev_close(struct device *device)
{
    return _norflash_close();
}
static int norflash_sfc_dev_read(struct device *device, void *buf, u32 len, u32 offset)
{
    //printf("%s start\n", __func__);
    int reg;
    /* printf("flash read sector = %d, num = %d\n", offset, len); */
    offset = offset * 512;
    len = len * 512;
    struct norflash_partition *part;
    part = (struct norflash_partition *)device->private_data;
    if (!part) {
        log_error("norflash partition invalid\n");
        return -EFAULT;
    }
    offset += part->start_addr;
    reg = fat_sfc_norflash_read(offset, buf, len, 1);
    if (reg) {
        r_printf(">>>[r error]:\n");
        len = 0;
    }

    len = len / 512;
    //printf("%s end\n", __func__);
    return len;
}
static int norflash_sfc_dev_write(struct device *device, void *buf, u32 len, u32 offset)
{
    //printf("%s start\n", __func__);
    /* printf("flash write sector = %d, num = %d\n", offset, len); */
    int reg = 0;
    offset = offset * 512;
    len = len * 512;
    struct norflash_partition *part = device->private_data;
    if (!part) {
        log_error("norflash partition invalid\n");
        return -EFAULT;
    }
    offset += part->start_addr;
    reg = _norflash_write(offset, buf, len, 1);
    if (reg) {
        r_printf(">>>[w error]:\n");
        len = 0;
    }
    len = len / 512;
    //printf("%s end\n", __func__);
    return len;
}
static bool norflash_sfc_dev_online(const struct dev_node *node)
{
    return 1;
}
static int norflash_sfc_dev_ioctl(struct device *device, u32 cmd, u32 arg)
{
    struct norflash_partition *part = device->private_data;
    if (!part) {
        log_error("norflash partition invalid\n");
        return -EFAULT;
    }
    return _norflash_ioctl(cmd, arg, 512, part);
}

/*
 * 1. 外部调用时以512字节为单位的地址和长度，且需要驱动write自己处理擦除，
 * 请使用norflash_sfc_dev_ops接口，否则使用本文件内的其他ops
 *
 * 2. 如果不需要驱动自己处理擦除，可以把宏FLASH_CACHE_ENABLE清零，或者把
 * norflash_sfc_dev_read()里面调用的fat_sfc_norflash_read()的实参cache填0，
 * norflash_sfc_dev_write()同理
 *
 * 3. norflash_sfc_dev_ops可以被多个设备名注册，每个设备名被认为是不同分区，所以
 * 需要填不同的分区起始地址和大小，若分区地址有重叠或者最大分区结束地址大于
 * flash容量，会触发ASSERT()
 *
 * 4. 关于IOCTL_GET_CAPACITY，有多个分区注册时返回分区的大小，如果只注册了1
 * 个分区，分区起始地址 == 0时返回flash容量，起始地址 != 0时返回分区大小，
 * norflash_sfc_dev_ops返回的长度以512字节为单位
 *
 * 5. 本文件内的各个ops可以同时使用
 */
//按512字节单位读写
const struct device_operations norflash_sfc_dev_ops = {
    .init   = norflash_sfc_dev_init,
    .online = norflash_sfc_dev_online,
    .open   = norflash_sfc_dev_open,
    .read   = norflash_sfc_dev_read,
    .write  = norflash_sfc_dev_write,
    .ioctl  = norflash_sfc_dev_ioctl,
    .close  = norflash_sfc_dev_close,
};


static int norflash_sfc_fs_dev_init(const struct dev_node *node, void *arg)
{
    struct norflash_sfc_dev_platform_data *pdata = arg;

    return _norflash_init(node->name, pdata);
}

static int norflash_sfc_fs_dev_open(const char *name, struct device **device, void *arg)
{
    struct norflash_partition *part;
    part = norflash_find_part(name);
    if (!part) {
        log_error("no norflash partition is found\n");
        return -ENODEV;
    }
    *device = &part->device;
    (*device)->private_data = part;
    if (atomic_read(&part->device.ref)) {
        return 0;
    }
    return _norflash_open(arg);
}
static int norflash_sfc_fs_dev_close(struct device *device)
{
    return _norflash_close();
}
static int norflash_sfc_fs_dev_read(struct device *device, void *buf, u32 len, u32 offset)
{
    u32 rets;//, reti;
    __asm__ volatile("%0 = rets":"=r"(rets));


    int reg;
    /* printf("flash read sector = %d, num = %d\n", offset, len); */
    struct norflash_partition *part;
    part = (struct norflash_partition *)device->private_data;
    if (!part) {
        log_error("norflash partition invalid\n");
        return -EFAULT;
    }

    // printf("offset = %x, len = %d, size = %x\n", \
    //     offset, len, part->size);
    if (offset + len > part->size) {
        log_error("__func__ %s %x %x %x\n", __func__, rets, offset, len);
        ASSERT(0);
    }

    offset += part->start_addr;
    reg = fat_sfc_norflash_read(offset, buf, len, 0);
    if (reg) {
        r_printf(">>>[r error]:\n");
        len = 0;
    }

    return len;
}

static int norflash_sfc_fs_dev_write(struct device *device, void *buf, u32 len, u32 offset)
{
    //printf("flash write addr = 0x%x, len = 0x%x\n", offset, len);
    //
    u32 rets;//, reti;
    __asm__ volatile("%0 = rets":"=r"(rets));


    int reg = 0;
    struct norflash_partition *part = device->private_data;
    if (!part) {
        log_error("norflash partition invalid\n");
        return -EFAULT;
    }


    if (offset + len > part->size) {
        log_error("__func__ %s %x %x %x\n", __func__, rets, offset, len);
        ASSERT(0);
    }


    offset += part->start_addr;

    reg = _norflash_write(offset, buf, len, 0);
    if (reg) {
        r_printf(">>>[w error]:\n");
        len = 0;
    }
    return len;
}
static bool norflash_sfc_fs_dev_online(const struct dev_node *node)
{
    return 1;
}

static int norflash_sfc_fs_dev_ioctl(struct device *device, u32 cmd, u32 arg)
{
    struct norflash_partition *part = device->private_data;
    if (!part) {
        log_error("norflash partition invalid\n");
        return -EFAULT;
    }
    return _norflash_ioctl(cmd, arg, 1, part);
}

/*
 * 1. 外部调用时以1字节为单位的地址和长度，且驱动write自己不处理擦除，
 * 请使用norflash_sfc_fs_dev_ops接口，否则使用本文件内的其他ops。注意：有些文件系
 * 统需要满足这个条件的驱动，如果期望修改成驱动内部处理擦除，需要测试所
 * 有关联的文件系统是否支持，或者新建一个符合需求的ops
 *
 * 2. 如果需要驱动自己处理擦除，需要把宏FLASH_CACHE_ENABLE置1，且
 * norflash_sfc_fs_dev_read()里面调用的fat_sfc_norflash_read()的实参cache填1，
 * norflash_sfc_fs_dev_write()同理
 *
 * 3. norflash_sfc_fs_dev_ops可以被多个设备名注册，每个设备名被认为是不同分区，所以
 * 需要填不同的分区起始地址和大小，若分区地址有重叠或者最大分区结束地址大于
 * flash容量，会触发ASSERT()
 *
 * 4. 关于IOCTL_GET_CAPACITY，有多个分区注册时返回分区的大小，如果只注册了1
 * 个分区，分区起始地址 == 0时返回flash容量，起始地址 != 0时返回分区大小
 * norflash_sfc_fs_dev_ops返回的长度以1字节为单位
 *
 * 5. 本文件内的各个ops可以同时使用
 */
//按1字节单位读写
const struct device_operations norflash_sfc_fs_dev_ops = {
    .init   = norflash_sfc_fs_dev_init,
    .online = norflash_sfc_fs_dev_online,
    .open   = norflash_sfc_fs_dev_open,
    .read   = norflash_sfc_fs_dev_read,
    .write  = norflash_sfc_fs_dev_write,
    .ioctl  = norflash_sfc_fs_dev_ioctl,
    .close  = norflash_sfc_fs_dev_close,
};
/*
 * 对ops的读写单位有另外需求，或者驱动内部是否支持擦除，可以参照上面的ops，
 * 不同条件自由组合，建立新的ops
 */



#define EX_FLASH_POWER_WORK     (0)
#define EX_FLASH_POWER_CLOSE    (1)
#define EX_FLASH_POWER_CLOSING  (2)
#define EX_FLASH_POWER_OPENING  (3)


static volatile u8 extern_flash_status = EX_FLASH_POWER_WORK;


extern void udelay(u32 us);

static void norflash_flash_poweroff(int priv)
{
    /* printf("norflash_flash_poweroff \n\n"); */
#if (TCFG_EX_FLASH_POWER_IO != NO_CONFIG_PORT)
    os_mutex_pend(&_norflash.mutex, 0);
    ASSERT(extern_flash_status == EX_FLASH_POWER_CLOSING);
    extern void hw_cache1_idle();
    extern void hw_close_flash1(void);
    hw_cache1_idle();
    udelay(FLASH_POWEROFF_TIME);
    hw_close_flash1();
    extern_flash_status = EX_FLASH_POWER_CLOSE;
    os_mutex_post(&_norflash.mutex);
#else
    os_mutex_pend(&_norflash.mutex, 0);
    //1是操作外挂
    norflash_ioctl((struct device *)1, IOCTL_POWER_SUSPEND, 0);
    extern_flash_status = EX_FLASH_POWER_CLOSE;
    os_mutex_post(&_norflash.mutex);
#endif
}

void ex_norflash_poweroff(void)
{
    /* printf("ex_norflash_poweroff \n\n"); */
    if (extern_flash_status != EX_FLASH_POWER_CLOSE) {
        extern_flash_status = EX_FLASH_POWER_CLOSING;
        norflash_flash_poweroff(0);
    }
}



 void norflash_flash_power_check()
{
#if (TCFG_EX_FLASH_POWER_IO != NO_CONFIG_PORT)
    if (!extern_flash_status) {
        return;
    }

    local_irq_disable();
    if (extern_flash_status == EX_FLASH_POWER_CLOSING) {

        os_taskq_del_type("app_core", Q_CALLBACK | last_id | USER_MASK_TYPE);
        extern_flash_status = EX_FLASH_POWER_WORK;
        //ASSERT(0);
        //验证临界
    } else if (extern_flash_status == EX_FLASH_POWER_CLOSE) {
        extern void hw_cache1_run();
        extern void hw_flash1_pwr_en();
        hw_flash1_pwr_en();
        udelay(FLASH_POWERON_TIME);
        sfc_spi_reinit_4byte_mode();
        hw_cache1_run();
        extern_flash_status = EX_FLASH_POWER_WORK;
    }
    local_irq_enable();
#else

    if (!extern_flash_status) {
        return;
    }

    local_irq_disable();
    if (extern_flash_status == EX_FLASH_POWER_CLOSING) {
        os_taskq_del_type("app_core", Q_CALLBACK | last_id | USER_MASK_TYPE);
        extern_flash_status = EX_FLASH_POWER_WORK;
        //ASSERT(0);
        //验证临界
    } else if (extern_flash_status == EX_FLASH_POWER_CLOSE) {
        local_irq_enable();
        norflash_ioctl((struct device *)1, IOCTL_POWER_RESUME, 0);
        udelay(FLASH_EXIT_LOWPOWER_TIME);
        local_irq_disable();
        extern_flash_status = EX_FLASH_POWER_WORK;
    }

    local_irq_enable();

#endif
}

static u8 extern_flash_handler(u32 timeout)
{
    int msg[3];
    ASSERT(extern_flash_status != EX_FLASH_POWER_OPENING);

    if (extern_flash_status == EX_FLASH_POWER_CLOSE) {
        return 0;
    }

    if (extern_flash_status == EX_FLASH_POWER_CLOSING) {
        return 1;
    }

    extern_flash_status = EX_FLASH_POWER_CLOSING;
    msg[0] = (int)norflash_flash_poweroff;
    msg[1] = 1;
    msg[2] = (int)NULL;
    last_id++;
    if (!last_id) {
        last_id = 1;
    }
    os_taskq_post_type("app_core", Q_CALLBACK | last_id | USER_MASK_TYPE, 3, msg);

    return 1;
}

static u8 extern_flash_handler1(u32 timeout)
{
    return 0;
}

//低功耗线程请求所有模块关闭，由对应线程处理
REGISTER_LP_REQUEST(power_flash_target) = {
    .name = "extern_flash",
    .request_enter = extern_flash_handler,
    .request_exit = extern_flash_handler1,
};




#endif /* #if (defined(TCFG_NORFLASH_SFC_DEV_ENABLE) && TCFG_NORFLASH_SFC_DEV_ENABLE) */
