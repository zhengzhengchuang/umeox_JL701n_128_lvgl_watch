#include "system/includes.h"
#include "fs.h"
#include "sdfile.h"
#include "asm/crc16.h"
#include "upay_common.h"
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)


#if 0
#pragma bss_seg(  ".upay_bss")
#pragma data_seg(  ".upay_data")
#pragma const_seg(  ".upay_const")
#pragma code_seg(  ".upay_code")
#endif

static OS_MUTEX aliyun_mutex;
#define aliyun_mutex_init() 	os_mutex_create(&aliyun_mutex)
#define aliyun_mutex_pend() 	os_mutex_pend(&aliyun_mutex, portMAX_DELAY)
#define aliyun_mutex_post() 	os_mutex_post(&aliyun_mutex)

#define FLASH_ERASE_SIZE		(4*1024) // (256)

static int aliyun_part_addr = 0;
static int aliyun_part_len = 0;

extern int norflash_write(struct device *device, void *buf, u32 len, u32 offset);
extern int norflash_erase(u32 cmd, u32 addr);
extern int norflash_origin_read(u8 *buf, u32 offset, u32 len);
extern int norflash_read(struct device *device, void *buf, u32 len, u32 offset);

static int _storage_e(u32 cmd, u32 addr)
{
    return norflash_erase(cmd, addr);
}
static int _storage_w(void *buf, u32 len, u32 offset)
{
    return norflash_write(NULL, buf, len, offset);
}
static int _storage_r(void *buf, u32 len, u32 offset)
{
    /* return norflash_read(NULL, buf, len, offset); */
    return norflash_origin_read(buf, offset, len);
}

int aliyun_storage_read(int offset, u8 *data, int len)
{
    r_printf("%s, offset:0x%x len:%d \n", __func__, offset, len);


    if ((offset + len) > aliyun_part_len) {
        log_e("aliyun read over,len:%d,%d,%d \n", offset, len, aliyun_part_len);
        return 0;
    }
    aliyun_mutex_pend();
    int rlen = _storage_r((void *)data, len, aliyun_part_addr + offset);
    aliyun_mutex_post();
    // put_buf(data, len);
    return rlen;
}

int aliyun_storage_write(int offset, u8 *data, int len)
{
    r_printf("%s, offset:0x%x len:%d \n", __func__, offset, len);
    // put_buf(data, len);

    if ((offset + len) > aliyun_part_len) {
        log_e("aliyun write over,len:%d,%d,%d \n", offset, len, aliyun_part_len);
        return 0;
    }
    char *tmpbuf = malloc(FLASH_ERASE_SIZE);
    ASSERT(tmpbuf);
    int tmplen;
    int wlen = len;
    int saddr = aliyun_part_addr + offset;
    u32 align_addr = saddr % FLASH_ERASE_SIZE;
    u16 rlen = FLASH_ERASE_SIZE - align_addr;
    saddr -= align_addr;

    aliyun_mutex_pend();
    if (align_addr) {
        tmplen = _storage_r(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __w_end;
        }
        if (rlen > len) {
            rlen = len;
        }
        memcpy(tmpbuf + align_addr, data, rlen);
#if (FLASH_ERASE_SIZE == 256)
        _storage_e(IOCTL_ERASE_PAGE, saddr);
#else
        _storage_e(IOCTL_ERASE_SECTOR, saddr);
#endif
        tmplen = _storage_w(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __w_end;
        }
        len -= rlen;
        data += rlen;
        saddr += FLASH_ERASE_SIZE;
    }

    while (len >= FLASH_ERASE_SIZE) {
#if (FLASH_ERASE_SIZE == 256)
        _storage_e(IOCTL_ERASE_PAGE, saddr);
#else
        _storage_e(IOCTL_ERASE_SECTOR, saddr);
#endif
        tmplen = _storage_w(data, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __w_end;
        }
        len -= FLASH_ERASE_SIZE;
        data += FLASH_ERASE_SIZE;
        saddr += FLASH_ERASE_SIZE;
    }

    if (len) {
        tmplen = _storage_r(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __w_end;
        }
        memcpy(tmpbuf, data, len);
#if (FLASH_ERASE_SIZE == 256)
        _storage_e(IOCTL_ERASE_PAGE, saddr);
#else
        _storage_e(IOCTL_ERASE_SECTOR, saddr);
#endif
        tmplen = _storage_w(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __w_end;
        }
    }


__w_end:
    aliyun_mutex_post();

    free(tmpbuf);
    return wlen;
}

int aliyun_storage_earse(int offset, int len)
{
    log_i("%s, offset:0x%x, len:%d \n", __func__, offset, len);

    if ((offset + len) > aliyun_part_len) {
        log_e("aliyun write over,len:%d,%d,%d \n", offset, len, aliyun_part_len);
        return 0;
    }
    char *tmpbuf = malloc(FLASH_ERASE_SIZE);
    ASSERT(tmpbuf);
    int tmplen;
    int wlen = len;
    int saddr = aliyun_part_addr + offset;
    u32 align_addr = saddr % FLASH_ERASE_SIZE;
    u16 rlen = FLASH_ERASE_SIZE - align_addr;
    saddr -= align_addr;

    aliyun_mutex_pend();
    if (align_addr) {
        tmplen = _storage_r(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __e_end;
        }
        if (rlen > len) {
            rlen = len;
        }
        memset(tmpbuf + align_addr, 0xFF, rlen);
#if (FLASH_ERASE_SIZE == 256)
        _storage_e(IOCTL_ERASE_PAGE, saddr);
#else
        _storage_e(IOCTL_ERASE_SECTOR, saddr);
#endif
        tmplen = _storage_w(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __e_end;
        }
        len -= rlen;
        saddr += FLASH_ERASE_SIZE;
    }

    while (len >= FLASH_ERASE_SIZE) {
#if (FLASH_ERASE_SIZE == 256)
        _storage_e(IOCTL_ERASE_PAGE, saddr);
#else
        _storage_e(IOCTL_ERASE_SECTOR, saddr);
#endif
        len -= FLASH_ERASE_SIZE;
        saddr += FLASH_ERASE_SIZE;
    }

    if (len) {
        tmplen = _storage_r(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __e_end;
        }
        memset(tmpbuf, 0xff, len);
#if (FLASH_ERASE_SIZE == 256)
        _storage_e(IOCTL_ERASE_PAGE, saddr);
#else
        _storage_e(IOCTL_ERASE_SECTOR, saddr);
#endif
        tmplen = _storage_w(tmpbuf, FLASH_ERASE_SIZE, saddr);
        if (tmplen != FLASH_ERASE_SIZE) {
            log_e("%s,%d \n", __func__, __LINE__);
            wlen = 0;
            goto __e_end;
        }
    }

__e_end:
    aliyun_mutex_post();

    free(tmpbuf);
    return wlen;
}

int aliyun_storage_init(void)
{
    char *path = "mnt/sdfile/EXT_RESERVED/UPAY";
    FILE *fp = fopen(path, "wr");
    if (!fp) {
        log_e("aliyun fopen err :%s \n", path);
        return false;
    }
    aliyun_mutex_init();

    struct vfs_attr attr = {0};
    fget_attrs(fp, &attr);
    aliyun_part_addr = attr.sclust;
    aliyun_part_len = attr.fsize;
    u32 sdfile_cpu_addr2flash_addr(u32 offset);
    aliyun_part_addr = sdfile_cpu_addr2flash_addr(aliyun_part_addr);

    log_i("aliyun storage, addr:0x%x, len:%d \n", attr.sclust, attr.fsize);


    /* extern void jjj_t(); */
    /* jjj_t(); */
    /*  */
    /* extern int hal_secure_partition_read(int fd, uint32_t offset, void *buffer, uint32_t read_len); */
    /*  */
    /* u8 *test = zalloc(512); */
    /* hal_secure_partition_read(0,0,test,512); */
    /* put_buf(test,512); */

    fclose(fp);

    /* void aliyun_storage_test(void); */
    /* aliyun_storage_test(); */

    return true;
}

#if 0
void aliyun_storage_test(void)
{
    log_i("aliyun_storage_test start \n");
    int saddr, tlen;
    u8 *dat = malloc(10 * 1024);
    u16 len;
    u8 cnt = 100;
    while (cnt--) {
        void wdt_clear(void);
        wdt_clear();

        len = rand32() % (10 * 1024 - 100) + 100;
        for (int i = 0; i < len; i++) {
            dat[i] = len + i;
        }
        if (len > aliyun_part_len) {
            log_e("len err:%d,%d \n", len, aliyun_part_len);
            while (1);
        }
        saddr = rand32() % (aliyun_part_len - len);
        tlen = aliyun_storage_write(saddr, dat, len);
        if (tlen != len) {
            log_e("write len err:%d,%d \n", tlen, len);
            while (1);
        }
        memset(dat, 0, len);
        tlen = aliyun_storage_read(saddr, dat, len);
        if (tlen != len) {
            log_e("read len err:%d,%d \n", tlen, len);
            while (1);
        }
        for (int i = 0; i < len; i++) {
            if ((u8)dat[i] != (u8)(len + i)) {
                log_e("test err, len:%d \n", len);
                put_buf(dat, len);
                while (1);
            }
        }
    }
    free(dat);

    log_i("aliyun_storage_test end\n");
}

#endif

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
