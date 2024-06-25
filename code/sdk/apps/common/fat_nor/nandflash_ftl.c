#include "include/nandflash_ftl.h"
#include "system/includes.h"
#include "virfat_flash.h"

FTL_HDL ftl_hdl = {0};
static int ftl_init_flag = 1;
#define AT_VOLATILE_RAM_CODE        AT(.volatile_ram_code)

#define NO_DEBUG_EN

#ifdef NO_DEBUG_EN
#undef r_printf
#undef y_printf
#define r_printf(...)
#define y_printf(...)
#endif


static const char ftl_bpb_data[512] = {
    0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x10, 0x08, 0x00,
    0x01, 0x00, 0x02, 0x00, 0x00, 0xF8, 0x80, 0x00, 0x3F, 0x00, 0xFF, 0x00, 0x00, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x80, 0x00, 0x29, 0xB0, 0xB2, 0xD8, 0x98, 0x4E, 0x4F, 0x20, 0x4E, 0x41,
    0x4D, 0x45, 0x20, 0x20, 0x20, 0x20, 0x46, 0x41, 0x54, 0x31, 0x36, 0x20, 0x20, 0x20, 0x33, 0xC9,
    0x8E, 0xD1, 0xBC, 0xF0, 0x7B, 0x8E, 0xD9, 0xB8, 0x00, 0x20, 0x8E, 0xC0, 0xFC, 0xBD, 0x00, 0x7C,
    0x38, 0x4E, 0x24, 0x7D, 0x24, 0x8B, 0xC1, 0x99, 0xE8, 0x3C, 0x01, 0x72, 0x1C, 0x83, 0xEB, 0x3A,
    0x66, 0xA1, 0x1C, 0x7C, 0x26, 0x66, 0x3B, 0x07, 0x26, 0x8A, 0x57, 0xFC, 0x75, 0x06, 0x80, 0xCA,
    0x02, 0x88, 0x56, 0x02, 0x80, 0xC3, 0x10, 0x73, 0xEB, 0x33, 0xC9, 0x8A, 0x46, 0x10, 0x98, 0xF7,
    0x66, 0x16, 0x03, 0x46, 0x1C, 0x13, 0x56, 0x1E, 0x03, 0x46, 0x0E, 0x13, 0xD1, 0x8B, 0x76, 0x11,
    0x60, 0x89, 0x46, 0xFC, 0x89, 0x56, 0xFE, 0xB8, 0x20, 0x00, 0xF7, 0xE6, 0x8B, 0x5E, 0x0B, 0x03,
    0xC3, 0x48, 0xF7, 0xF3, 0x01, 0x46, 0xFC, 0x11, 0x4E, 0xFE, 0x61, 0xBF, 0x00, 0x00, 0xE8, 0xE6,
    0x00, 0x72, 0x39, 0x26, 0x38, 0x2D, 0x74, 0x17, 0x60, 0xB1, 0x0B, 0xBE, 0xA1, 0x7D, 0xF3, 0xA6,
    0x61, 0x74, 0x32, 0x4E, 0x74, 0x09, 0x83, 0xC7, 0x20, 0x3B, 0xFB, 0x72, 0xE6, 0xEB, 0xDC, 0xA0,
    0xFB, 0x7D, 0xB4, 0x7D, 0x8B, 0xF0, 0xAC, 0x98, 0x40, 0x74, 0x0C, 0x48, 0x74, 0x13, 0xB4, 0x0E,
    0xBB, 0x07, 0x00, 0xCD, 0x10, 0xEB, 0xEF, 0xA0, 0xFD, 0x7D, 0xEB, 0xE6, 0xA0, 0xFC, 0x7D, 0xEB,
    0xE1, 0xCD, 0x16, 0xCD, 0x19, 0x26, 0x8B, 0x55, 0x1A, 0x52, 0xB0, 0x01, 0xBB, 0x00, 0x00, 0xE8,
    0x3B, 0x00, 0x72, 0xE8, 0x5B, 0x8A, 0x56, 0x24, 0xBE, 0x0B, 0x7C, 0x8B, 0xFC, 0xC7, 0x46, 0xF0,
    0x3D, 0x7D, 0xC7, 0x46, 0xF4, 0x29, 0x7D, 0x8C, 0xD9, 0x89, 0x4E, 0xF2, 0x89, 0x4E, 0xF6, 0xC6,
    0x06, 0x96, 0x7D, 0xCB, 0xEA, 0x03, 0x00, 0x00, 0x20, 0x0F, 0xB6, 0xC8, 0x66, 0x8B, 0x46, 0xF8,
    0x66, 0x03, 0x46, 0x1C, 0x66, 0x8B, 0xD0, 0x66, 0xC1, 0xEA, 0x10, 0xEB, 0x5E, 0x0F, 0xB6, 0xC8,
    0x4A, 0x4A, 0x8A, 0x46, 0x0D, 0x32, 0xE4, 0xF7, 0xE2, 0x03, 0x46, 0xFC, 0x13, 0x56, 0xFE, 0xEB,
    0x4A, 0x52, 0x50, 0x06, 0x53, 0x6A, 0x01, 0x6A, 0x10, 0x91, 0x8B, 0x46, 0x18, 0x96, 0x92, 0x33,
    0xD2, 0xF7, 0xF6, 0x91, 0xF7, 0xF6, 0x42, 0x87, 0xCA, 0xF7, 0x76, 0x1A, 0x8A, 0xF2, 0x8A, 0xE8,
    0xC0, 0xCC, 0x02, 0x0A, 0xCC, 0xB8, 0x01, 0x02, 0x80, 0x7E, 0x02, 0x0E, 0x75, 0x04, 0xB4, 0x42,
    0x8B, 0xF4, 0x8A, 0x56, 0x24, 0xCD, 0x13, 0x61, 0x61, 0x72, 0x0B, 0x40, 0x75, 0x01, 0x42, 0x03,
    0x5E, 0x0B, 0x49, 0x75, 0x06, 0xF8, 0xC3, 0x41, 0xBB, 0x00, 0x00, 0x60, 0x66, 0x6A, 0x00, 0xEB,
    0xB0, 0x42, 0x4F, 0x4F, 0x54, 0x4D, 0x47, 0x52, 0x20, 0x20, 0x20, 0x20, 0x0D, 0x0A, 0x52, 0x65,
    0x6D, 0x6F, 0x76, 0x65, 0x20, 0x64, 0x69, 0x73, 0x6B, 0x73, 0x20, 0x6F, 0x72, 0x20, 0x6F, 0x74,
    0x68, 0x65, 0x72, 0x20, 0x6D, 0x65, 0x64, 0x69, 0x61, 0x2E, 0xFF, 0x0D, 0x0A, 0x44, 0x69, 0x73,
    0x6B, 0x20, 0x65, 0x72, 0x72, 0x6F, 0x72, 0xFF, 0x0D, 0x0A, 0x50, 0x72, 0x65, 0x73, 0x73, 0x20,
    0x61, 0x6E, 0x79, 0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x72, 0x65, 0x73, 0x74, 0x61,
    0x72, 0x74, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAC, 0xCB, 0xD8, 0x55, 0xAA
};

static int ftl_dev_read(struct device *device, void *buf, u32 len, u32 addr);
static int ftl_dev_write(struct device *device, void *buf, u32 len, u32 addr);
static int ftl_errblock_num_check();
extern int nand_block_internal_data_move(u32 block_src_addr, u32 block_dest_addr, u8 page_num, u16 offset, u16 len, u8 *buf);
extern int nand_page_internal_data_move(u32 page_src_addr, u32 page_dest_addr, u16 offset, u16 len, u8 *buf);
static int ftl_err_block_deal(int err_block);
static int ftl_update_block_info(int block, BLOCK_INFO *dir);
static int ftl_update_spbk_info();
extern void clr_wdt(void);

static u32 __ftl_read(void *p, void *buf, u32 addr, u32 len)
{
    int rlen;
    if (!p) {
        return 1;
    }
    /*log_i("dev_read: %x\n", addr);*/

    /* os_time_dly(10); */
    dev_ioctl(p, IOCTL_CMD_RESUME, 0);
    rlen = dev_bulk_read(p, buf, addr, len);
    dev_ioctl(p, IOCTL_CMD_SUSPEND, 0);
    if (rlen == len) {
        return 0;
    }
    return 1;
}

static u32 __ftl_write(void *p, void *buf, u32 addr, u32 len)
{
    int wlen;
    if (!p) {
        return 1;
    }
    /*log_i("dev_write: %x, %d\n", addr, count);*/

    dev_ioctl(p, IOCTL_CMD_RESUME, 0);
    wlen = dev_bulk_write(p, buf, addr, len);
    dev_ioctl(p, IOCTL_CMD_SUSPEND, 0);
    if (wlen == len) {
        return 0;
    }
    /*log_i("-------------sd:dev_write_err\n");*/
    return 1;
}

static int __ftl_erase(u32 block)
{
    return dev_ioctl(ftl_hdl.hdev, IOCTL_ERASE_BLOCK, block * ftl_hdl.spbk.spbk_block_size);
}

int nandflash_tflag = 0;

static void ftl_fat_copy(void *pDest, void *pSrc, u32 copyLen)
{
    while (copyLen--) {
        ((u8 *)pDest)[copyLen] = ((u8 *)pSrc)[copyLen];
    }
}

static u16 ftl_ld_word(u8 *p)
{
    return (u16)p[1] << 8 | p[0];
}

static u32 ftl_ld_dword(u8 *p)
{
    return (u32)p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
}

static int ftl_check_ff(char *buf, int len)
{
    while (len--) {
        if ((u8)buf[len] != 0xff) {
            return 1;
        }
    }
    return 0;
}

static void ftl_block_info_exchange(void)
{
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    /* return; */
    int res;
    int page_sec = FTL_NAND_PAGE_SIZE / 512;
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    int a = ftl_hdl.spbk.spbk_block_info_addr * block_sec;
    int b = ftl_hdl.spbk.spbk_block_info_logaddr * block_sec;
    int c = ftl_hdl.spbk.spbk_block_info_len * block_sec;
    char *buf = zalloc(FTL_NAND_PAGE_SIZE);
    res = __ftl_read(ftl_hdl.hdev, buf, a, page_sec);
    if (res) {
        goto __exit;
    }
    if (0 == ftl_check_ff(buf, FTL_NAND_PAGE_SIZE)) {
        g_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        for (int i = 0; i < c; i += page_sec) {
            if ((i % block_sec) == 0) {
                //TODO: 擦除不止一个块的处理。
                res = __ftl_erase((a + i) / block_sec);
                if (res) {
                    goto __exit;
                }
            }
            res = __ftl_read(ftl_hdl.hdev, buf, b + i, page_sec);
            if (res) {
                goto __exit;
            }
            res = __ftl_write(ftl_hdl.hdev, buf, a + i, page_sec);
            if (res) {
                goto __exit;
            }
        }
    } else {
        g_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        memset(buf, 0xff, FTL_NAND_PAGE_SIZE);
        //每次 都更新 loginfo 的保持一致
        /* res = __ftl_read(ftl_hdl.hdev, buf, b, page_sec); */
        /* if (res) { */
        /*     goto __exit; */
        /* } */
        /* if (ftl_check_ff(buf, FTL_NAND_PAGE_SIZE)) { */
        /*     g_printf("\n >>>[test]:func = %s,line= %d\n",__FUNCTION__, __LINE__); */
        /*     goto __exit; */
        /* } */
        for (int i = 0; i < c; i += page_sec) {
            if ((i % block_sec) == 0) {
                //TODO: 擦除不止一个块的处理。
                res = __ftl_erase((b + i) / block_sec);
                if (res) {
                    goto __exit;
                }
            }
            res = __ftl_read(ftl_hdl.hdev, buf, a + i, page_sec);
            if (res) {
                goto __exit;
            }
            res = __ftl_write(ftl_hdl.hdev, buf, b + i, page_sec);
            if (res) {
                goto __exit;
            }
        }
    }
__exit:
    if (buf) {
        free(buf);
    }
    return;
}

AT_VOLATILE_RAM_CODE
int ftl_get_block_info(u32 location, BLOCK_INFO *dir, int flag)
{
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    /* int location  = logic_sec_addr / block_sec; */
    int a = 512 / sizeof(BLOCK_INFO); // 每个扇区可表示多少个block
    int i = location / a;
    int b = location % a; // 在第几个 sizeof(BLOCK_INFO) .
    /* printf(">>>[test]:in %s, i = %d, b = %d, location = %d\n", __FUNCTION__, i, b, location); */
    char *rbuf = zalloc(512);
    int res;
    //TODO : 测试实际读256M的nandflash i不会超过31，可以做优化，不用一直找到尾部。
    int length = ftl_hdl.spbk.spbk_flash_size * sizeof(BLOCK_INFO) / 512 + 1;
    /* for (; i < ftl_hdl.spbk.spbk_block_info_len * block_sec; i++) { */
    for (; i < length; i++) {
        /* g_printf(">>>[test]:ftl_hdl.spbk.spbk_block_info_addr = %d\n", ftl_hdl.spbk.spbk_block_info_addr); */
        res =  __ftl_read(ftl_hdl.hdev, rbuf, ftl_hdl.spbk.spbk_block_info_addr * block_sec + i, 1);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        /* put_buf(rbuf, 512); */
        /* printf("\n >>>[test]:func = %s,line= %d\n",__FUNCTION__, __LINE__); */
        if (0 == ftl_check_ff(rbuf, 512)) {
            g_printf(">>>[test]:i = %d,b  = %d, addr_sec = %d\n", i, b, ftl_hdl.spbk.spbk_block_info_addr * block_sec + i);
            //TODO :考虑优化，从整个block大小出发，减少两次读
            res =  __ftl_read(ftl_hdl.hdev, rbuf, ftl_hdl.spbk.spbk_block_info_logaddr * block_sec + i, 1);
            if (res) {
                res = FTL_DISK_ERR;
                goto __exit;
            }
        }

        while (1) {
            memcpy(dir, &rbuf[b * sizeof(BLOCK_INFO)], sizeof(BLOCK_INFO));
            /* r_printf(">>>[test]:logic_map_addr = 0x%x\n", dir->logic_map_addr); */
            /* put_buf(rbuf, 128); */
            if (flag) {
                res = FTL_OK;
                goto __exit;
            }
            if (dir->logic_map_addr >= 0) {
                if ((location - ftl_hdl.spbk.spbk_hot_part_addr) == dir->logic_map_addr / ftl_hdl.spbk.spbk_block_size) {
                    if ((u8)dir->page_status != FTL_FAT) {
                        res = FTL_SEARCH_ERR;
                        goto __exit;
                    }
                    ftl_hdl.cur_block = i * a + b;
                    res = FTL_OK;
                    goto __exit;
                }
            }
            b++;
            if (b > a) {
                b = 0;
                break;
            }
        }
    }
    res = FTL_FLASH_ERR;
__exit:
    if (rbuf) {
        free(rbuf);
    }
    return res;
}

AT_VOLATILE_RAM_CODE
static int _ftl_get_plane_block(int org, u8 ex_flag)
{
    //TODO: rand_block 和 ftl_hdl.cur_block 保持奇偶一致
    int rand_block;
    int a;
    if (ex_flag) {
        a = ftl_hdl.spbk.spbk_cold_part_exaddr + 1;
        rand_block = rand32() % (ftl_hdl.spbk.spbk_hot_part_exaddr - a) + a; // 取冷交换区+1 -- 热交换区地址内的block。 +1是为了方便处理奇偶
    } else {
        a = ftl_hdl.spbk.spbk_hot_part_exaddr + 1;
        rand_block = rand32() % (ftl_hdl.spbk.spbk_block_info_logaddr - a) + a; // 取热交换区+1 -- block_info地址内的block。 +1是为了方便处理奇偶
    }
    if (org % 2) {
        if (!(rand_block % 2)) {
            rand_block = rand_block - 1;
        }
    } else {
        if (rand_block % 2) {
            rand_block = rand_block / 2 * 2;
        }
    }
#if 1
    BLOCK_INFO tmp_dir;
    int res;
    /* g_printf(">>>[test]:goto here1 !!!!!!!!!\n"); */
    for (int i = rand_block; i < ftl_hdl.spbk.spbk_block_info_logaddr; i += 2) {
        res = ftl_get_block_info(i, &tmp_dir, 1);
        if (res) {
            /* ASSERT(); //读block_info失败 */
            return -1;
        }
        /* put_buf(&tmp_dir, sizeof(BLOCK_INFO)); */
        if ((tmp_dir.logic_map_addr == FTL_EX_FREE) && ((u8)tmp_dir.page_status == FTL_FREE)) {
            rand_block = i;
            return rand_block;
        }
    }
    /* g_printf(">>>[test]:goto here2 !!!!!!!!!\n"); */
    for (int i = rand_block - 2; i > ftl_hdl.spbk.spbk_cold_part_exaddr; i -= 2) {
        res = ftl_get_block_info(i, &tmp_dir, 1);
        if (res) {
            /* ASSERT(); //读block_info失败 */
            g_printf("\n >>>[test]: errr cold_part = %d, func = %s,line= %d\n", ftl_hdl.spbk.spbk_cold_part_exaddr, __FUNCTION__, __LINE__);
            return -1;
        }
        /* put_buf(&tmp_dir, sizeof(BLOCK_INFO)); */
        if ((tmp_dir.logic_map_addr == FTL_EX_FREE) && ((u8)tmp_dir.page_status == FTL_FREE)) {
            rand_block = i;
            return rand_block;
        }
    }
    g_printf("\n >>>[test]: errr cold_part = %d, func = %s,line= %d\n", ftl_hdl.spbk.spbk_cold_part_exaddr, __FUNCTION__, __LINE__);
    return -1;
#endif
    return rand_block;
}

int _ftl_fcopy_block(int org, int dst, int addr, char *wbuf, int buflen)
{
    int res;
    int page_sec = FTL_NAND_PAGE_SIZE / 512;
    y_printf(">>>[test]:org = %d, dst = %d\n", org, dst);
    int fwrite_time = sys_timer_get_ms();
    if (addr > -1) {
        int page_num = addr / FTL_NAND_PAGE_SIZE;
        int offset = addr % FTL_NAND_PAGE_SIZE;
        res = nand_block_internal_data_move(org * ftl_hdl.spbk.spbk_block_size, dst * ftl_hdl.spbk.spbk_block_size, page_num, offset, buflen, wbuf);
    } else {
        res = nand_block_internal_data_move(org * ftl_hdl.spbk.spbk_block_size, dst * ftl_hdl.spbk.spbk_block_size, 0, 0, 0, NULL);
    }
    y_printf(">>>[test]:in ftl write time = %d\n", sys_timer_get_ms() - fwrite_time);
__exit:
    return res;
}

static int ftl_find_block(int block)
{
    int res;
    int new_block;
    BLOCK_INFO dir;
    BLOCK_INFO tmp_dir;
    res = ftl_get_block_info(block, &dir, 0);
    if (res) {
        return -1;
    }

    if (block % 2) {
        if (ftl_hdl.old_cold_exaddr % 2) {
            new_block = ftl_hdl.old_cold_exaddr;
        } else {
            new_block = ftl_hdl.old_cold_exaddr + 1;
        }
    } else {
        if (ftl_hdl.old_cold_exaddr % 2) {
            new_block = ftl_hdl.old_cold_exaddr + 1;
        } else {
            new_block = ftl_hdl.old_cold_exaddr;
        }
    }
    int i = new_block;
    for (; i < ftl_hdl.spbk.spbk_hot_part_exaddr; i += 2) {
        clr_wdt();
        res = ftl_get_block_info(i, &tmp_dir, 1);
        if (res) {
            /* ASSERT(); //读block_info失败 */
            return -1;
        }
        printf(">>>[test]:map_addr = 0x%x, status = 0x%x\n", tmp_dir.logic_map_addr, (u8)tmp_dir.page_status);
        put_buf(&tmp_dir, 8);
        if ((tmp_dir.logic_map_addr == FTL_EX_FREE) && ((u8)tmp_dir.page_status == FTL_FREE)) {
            new_block = i;
            break;
        }
        new_block = 0;
    }
    if (new_block == 0) {
        return -1;
    }
    memcpy(&dir.block_erase_num[0], &tmp_dir.block_erase_num[0], sizeof(dir.block_erase_num));
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    res = ftl_update_block_info(new_block, &dir);
    if (res) {
        return -1;
    }
    ftl_hdl.cur_block = i;
    g_printf(">>>[test]:更新cur_block = %d\n", ftl_hdl.cur_block);
    if (ftl_hdl.spbk.spbk_cold_part_exaddr <= i) {
        ftl_hdl.spbk.spbk_cold_part_exaddr = i + 1;
        res = ftl_update_spbk_info();
        if (res) {
            return -1;
        }
    }
    return new_block;
}

static int ftl_move_data(int dst_block, int addr_in_block, char *wbuf, int len, int flag)
{
    int res;
    int a = dst_block;
__again:
    int b = _ftl_get_plane_block(a, flag);
    if (b < 0) {
        res = -1;
        goto __exit;
    }
    res = __ftl_erase(b);
    if (res) {
        printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        res = ftl_err_block_deal(b);
        if (res) {
            goto __exit;
        }
        goto __again;
    } else {
        // TODO : 记录擦写次数++
    }
    res = _ftl_fcopy_block(a, b, -1, NULL, 0);
    if (res) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
__eraser_dst:
    res = __ftl_erase(a);
    if (res) {
        if (a == ftl_hdl.spbk.spbk_block_info_addr || a == ftl_hdl.spbk.spbk_block_info_logaddr) {
            /* ASSERT(); */
            goto __exit;
        }
        printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        res = ftl_err_block_deal(a);
        if (res) {
            goto __exit;
        }
        a = ftl_hdl.cur_block;
        g_printf(">>>[test]:now move data a = %d\n", a);
        goto __eraser_dst;
    } else {
        // TODO : 记录擦写次数++
    }
    res = _ftl_fcopy_block(b, a, addr_in_block, wbuf, len);
    if (res) {
        res = FTL_DISK_ERR;
        g_printf(">>>[test]:  copy err = %d!!!!!!!!!\n", res);
        goto __exit;
    }
__exit:
    return res;
}

static int ftl_update_spbk_info()
{
    // TODO ：更新超级块信息
    int res;
    char *spbk = zalloc(512);
    res =  __ftl_read(ftl_hdl.hdev, spbk, 0, 1);
    if (res) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
    if (0 == ftl_hdl.erase_all || ftl_check_ff(spbk, sizeof(SPBK_INFO))) {
        ftl_hdl.erase_all = 0;
        res = __ftl_erase(0);
        if (res) {
            res = FTL_UPDATE_SPBK_ERR;
            goto __exit;
        }
    }
    memset(spbk, 0, sizeof(SPBK_INFO));
    memcpy(spbk, &ftl_hdl.spbk, sizeof(SPBK_INFO));
    res = __ftl_write(ftl_hdl.hdev, spbk, 0, 1);
    if (res) {
        res = FTL_DISK_ERR;
    }
    y_printf(">>>[test]:实际值 size：%d,%d,%d,%d\n", ftl_hdl.spbk.spbk_page_size, ftl_hdl.spbk.spbk_block_size, ftl_hdl.spbk.spbk_flash_size, ftl_hdl.spbk.spbk_mount_size);
    y_printf(">>>[test]:实际值 addr：%d,%d,%d,%d,%d,%d,%d\n", ftl_hdl.spbk.spbk_hot_part_addr, ftl_hdl.spbk.spbk_cold_part_addr, ftl_hdl.spbk.spbk_cold_part_exaddr, ftl_hdl.spbk.spbk_hot_part_exaddr, ftl_hdl.spbk.spbk_block_info_logaddr, ftl_hdl.spbk.spbk_block_info_addr, ftl_hdl.spbk.spbk_block_info_len);

__exit:
    if (spbk) {
        free(spbk);
    }
    return res;
}

/* char fix_buf[2048] = {0}; */
static int ftl_update_block_info(int block, BLOCK_INFO *dir)
{

    int res;
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    int x = 512 / sizeof(BLOCK_INFO);
    int a = ftl_hdl.spbk.spbk_block_info_addr;
    int b = block / x;
    int c = block % x;
    char *tmp_buf = NULL;
    char *info = zalloc(512);
    g_printf(">>>[test]:a = %d\n", a);
    res =  __ftl_read(ftl_hdl.hdev, info, a * block_sec + b, 1);
    if (res) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
    g_printf(">>>[test]:go here\n");
    if (0 == ftl_check_ff(info, 512)) {
        a = ftl_hdl.spbk.spbk_block_info_logaddr;
        res =  __ftl_read(ftl_hdl.hdev, info, a * block_sec + b, 1);
        g_printf(">>>[test]:go here a = %d, res = %d\n", a, res);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
    }
    memcpy(info + c * sizeof(BLOCK_INFO), dir, sizeof(BLOCK_INFO));
    g_printf(">>>[test]:offset = %d\n", c * sizeof(BLOCK_INFO));
    put_buf(info, 512);
    res = ftl_move_data(a, block * sizeof(BLOCK_INFO) / 512 * 512, info, 512, 1); //读出的 info 是512地址对齐的buf，写入的地址也要对齐，不然偏移 出错。
    /* printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__); */
    /* dev_bulk_read(ftl_hdl.hdev, fix_buf, a * block_sec + b, 4); */
    /* put_buf(fix_buf, 512); */
    ftl_block_info_exchange();
__exit:
    if (info) {
        free(info);
    }
    return res;
}

static int ftl_err_block_deal(int err_block)
{
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    int res;
    int x = 512 / sizeof(BLOCK_INFO);
    ftl_hdl.err_block_num++;
    res = ftl_errblock_num_check();
    if (res) {
        goto __exit;
    }
    int flag = 0;
    if ((err_block >= ftl_hdl.spbk.spbk_cold_part_exaddr) && (err_block < ftl_hdl.spbk.spbk_block_info_logaddr)) {
        flag = 1;
    }
    res = ftl_get_block_info(err_block, &ftl_hdl.block_info, flag);
    if (res) {
        goto __exit;
    }
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);

    if ((u8)ftl_hdl.block_info.page_status == FTL_FAT) {
        int i = ftl_hdl.spbk.spbk_cold_part_exaddr;
        int b = ftl_hdl.spbk.spbk_hot_part_exaddr;
        if (b == i) {
            //TODO : 需要重新初始化格式化一下设备。
            g_printf(">>>[test]:i == b = %d\n", b);
            return FTL_FLASH_ERR;
        }

        printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        int new_block = ftl_find_block(err_block);
        if (new_block == -1) {
            res = FTL_FLASH_ERR;
            goto __exit;
        }

    }
    // TODO ：更新坏块信息
    ftl_hdl.block_info.logic_map_addr = FTL_BLOCK_BREAK;
    ftl_hdl.block_info.page_status = FTL_DAMAGED;
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    res = ftl_update_block_info(err_block, &ftl_hdl.block_info);
__exit:
    return res;

}

void ftl_test_write_demo(u32 dst)
{
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    int res;
    int cnt = 2;
    u8 *test_buf = zalloc(512);

    while (cnt--) {
        dst += 2;
        r_printf(">>>[test]:dst = %d\n", dst);
        res = __ftl_erase(dst);
        if (res) {
            res = FTL_DISK_ERR;
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            goto __exit;
        }

        int j = 0;
        for (int i = 0; i < 512; i++) {
            if (cnt == 1) {
                j--;
                if (i == 0) {
                    j = 512;
                }
            } else {
                j++;
            }
            test_buf[i] = (512 - j) & 0xff;
        }
        y_printf(">>>[test]:write\n");
        res = __ftl_write(ftl_hdl.hdev, test_buf, dst * block_sec, 1);
        if (res) {
            res = FTL_DISK_ERR;
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            goto __exit;
        }
        memset(test_buf, 0, 512);
        r_printf(">>>[test]:read\n");
        res =  __ftl_read(ftl_hdl.hdev, test_buf, dst * block_sec, 1);
        if (res) {
            res = FTL_DISK_ERR;
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            goto __exit;
        }
        put_buf(test_buf, 512);
    }

__exit:
    free(test_buf);
    /* while (1); */
}

static int ftl_errblock_num_check()
{
    int res = FTL_OK;
    if (ftl_hdl.err_block_num > ftl_hdl.spbk.spbk_flash_size * 70 / 100) {
        printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        res = FTL_FLASH_ERR;
    }
    return res;
}

static int ftl_erase_all(int cap, int block_num)
{
    //TODO : 记录坏块, 坏块 > 70% return FTL_FLASH_ERR.

    int res;
    int a = 0;
    int b = 512 / sizeof(BLOCK_INFO);
    int block_erase_num = 1;
    int block_num_cnt = 0;
    int block_i = block_num - 1;

    /* ftl_test_write_demo(300); */

    for (; block_i >= 0; block_i--) {
        clr_wdt();
        res = __ftl_erase(block_i);
        if (res) {
            block_num_cnt = 0;
            ftl_hdl.err_block_num++;
            putchar('E');
            continue;
            /* return FTL_DISK_ERR; */
        }
        res = ftl_errblock_num_check();
        if (res) {
            goto __exit;
        }
        block_num_cnt++;
        if (block_num_cnt == 2 * ftl_hdl.spbk.spbk_block_info_len) {
            //更新 block_info 以及 其log区 地址
            ftl_hdl.spbk.spbk_block_info_logaddr = block_i;
            ftl_hdl.spbk.spbk_block_info_addr = block_i + ftl_hdl.spbk.spbk_block_info_len;
            break;
        }
    }
    char *info = zalloc(512);
    char err_block_flag = 0;
    char hot_part_first = 1;
    int logic_cnt = 0;
    y_printf("\n >>>[test]:func = %s,line= %d, block_num_cnt = %d, block_i = %d\n", __FUNCTION__, __LINE__, block_num_cnt, block_i);
    for (int i = 0; i < block_num; i++) {
        clr_wdt();
        if (i < block_i) {
            res = __ftl_erase(i);
            if (res) {
                putchar('$');
                ftl_hdl.err_block_num++;
                err_block_flag = 1;
            }
        }
        if (i < 1) {
            ftl_hdl.block_info.logic_map_addr = FTL_SPBK;
            ftl_hdl.block_info.page_status = ~FTL_CONSUMED & FTL_USED;
            ftl_fat_copy(&ftl_hdl.block_info.block_erase_num[0], &block_erase_num, sizeof(ftl_hdl.block_info.block_erase_num));
        } else if (i < ftl_hdl.spbk.spbk_cold_part_exaddr) {
            if (err_block_flag) {
                memset(&ftl_hdl.block_info, 0, sizeof(BLOCK_INFO));
                ftl_hdl.spbk.spbk_mount_size--;
            } else {
                if (hot_part_first) {
                    ftl_hdl.spbk.spbk_hot_part_addr = i;
                    hot_part_first = 0;
                }
                ftl_hdl.block_info.logic_map_addr = logic_cnt * ftl_hdl.spbk.spbk_block_size;
                ftl_hdl.block_info.page_status = FTL_FAT;
                ftl_fat_copy(&ftl_hdl.block_info.block_erase_num[0], &block_erase_num, sizeof(ftl_hdl.block_info.block_erase_num));
                logic_cnt++;
            }
        } else if (i < ftl_hdl.spbk.spbk_block_info_logaddr) {
            ftl_hdl.block_info.logic_map_addr = FTL_EX_FREE;
            ftl_hdl.block_info.page_status = FTL_FREE;
            ftl_fat_copy(&ftl_hdl.block_info.block_erase_num[0], &block_erase_num, sizeof(ftl_hdl.block_info.block_erase_num));
            if (err_block_flag) {
                memset(&ftl_hdl.block_info, 0, sizeof(BLOCK_INFO));
            }
        } else {
            ftl_hdl.block_info.logic_map_addr = FTL_BLOCK_INFO;
            ftl_hdl.block_info.page_status = FTL_USED;
            ftl_fat_copy(&ftl_hdl.block_info.block_erase_num[0], &block_erase_num, sizeof(ftl_hdl.block_info.block_erase_num));
        }
        a = (i & (b - 1)) * sizeof(BLOCK_INFO); //计算存放在info 的位置。
        if (((i % b == 0) && i) || (i == (block_num - 1))) {
            if (i == (block_num - 1)) {
                memcpy(info + a, &ftl_hdl.block_info, sizeof(BLOCK_INFO));
                i = (i + b - 1) / b * b; //最后的位置需要处理 对齐。
            }
            /* g_printf(">>>[test]:i = %d, b = %d\n", i, b); */
            /* put_buf(info , 512); */
            res = __ftl_write(ftl_hdl.hdev, info, ftl_hdl.spbk.spbk_block_info_addr * (ftl_hdl.spbk.spbk_block_size / 512) + (i / b - 1), 1); //凑齐512个字节一起写进去,最后结束把剩下的写进去。
            if (res) {
                res = FTL_DISK_ERR;
            }
            memset(info, 0, 512);
        }
        memcpy(info + a, &ftl_hdl.block_info, sizeof(BLOCK_INFO));
        err_block_flag = 0;
        res = FTL_OK;
    }
    /* res = __ftl_read(ftl_hdl.hdev, info, ftl_hdl.spbk.spbk_block_info_addr * (ftl_hdl.spbk.spbk_block_size / 512) + 31, 1); //凑齐512个字节一起写进去,最后结束把剩下的写进去。 */
    /* g_printf(">>>[test]: sec = %d\n", ftl_hdl.spbk.spbk_block_info_addr * (ftl_hdl.spbk.spbk_block_size / 512)); */
    /* put_buf(info, 512); */
    res = ftl_errblock_num_check();
    if (res) {
        goto __exit;
    }

    ftl_hdl.erase_all = 1;

__exit:
    if (info) {
        free(info);
    }
    return res;
}

static int ftl_create_part_info(int cap, int block_num)
{
    // 备份区占flash 1/16 大小
    // 冷区交换区 占备份区 1/4 大小
    int res;
    memcpy(ftl_hdl.spbk.spbk_logo, "spbk", 4);
    ftl_hdl.spbk.spbk_flash_size = block_num;
    int backup_num = block_num / 16; //备份区占 block数

    ftl_hdl.spbk.spbk_cold_part_exaddr = block_num / 16 * 15; //备份区占 1/16 后面的位置.
    ftl_hdl.spbk.spbk_hot_part_exaddr = ftl_hdl.spbk.spbk_cold_part_exaddr + backup_num / 4; //热交换区起始位置，以block大小对齐,单位block。
    ftl_hdl.spbk.spbk_block_info_len = (block_num * 8  + ftl_hdl.spbk.spbk_block_size - 1) / ftl_hdl.spbk.spbk_block_size; // block_info的长度，以block大小对齐， 单位block。
    ftl_hdl.spbk.spbk_block_info_logaddr = block_num - 2 * ftl_hdl.spbk.spbk_block_info_len; // block_info log地址 在最后，总block_num - block_info len x 2.
    ftl_hdl.spbk.spbk_block_info_addr = block_num - ftl_hdl.spbk.spbk_block_info_len; // block_info地址 在最后，总block_num - block_info len.
    ftl_hdl.spbk.spbk_hot_part_addr = 1; // super block 占一个block.
    ftl_hdl.spbk.spbk_mount_size = ftl_hdl.spbk.spbk_cold_part_exaddr - ftl_hdl.spbk.spbk_hot_part_addr; // 挂载的fat大小，单位block .
    ftl_hdl.spbk.spbk_cold_part_addr = ftl_hdl.spbk.spbk_cold_part_exaddr / 2; //冷区地址预设 1/2挂载大小地方.
    y_printf(">>>[test]:预设值 size：%d,%d,%d,%d\n", ftl_hdl.spbk.spbk_page_size, ftl_hdl.spbk.spbk_block_size, ftl_hdl.spbk.spbk_flash_size, ftl_hdl.spbk.spbk_mount_size);
    y_printf(">>>[test]:预设值 addr：%d,%d,%d,%d,%d,%d,%d\n", ftl_hdl.spbk.spbk_hot_part_addr, ftl_hdl.spbk.spbk_cold_part_addr, ftl_hdl.spbk.spbk_cold_part_exaddr, ftl_hdl.spbk.spbk_hot_part_exaddr, ftl_hdl.spbk.spbk_block_info_logaddr, ftl_hdl.spbk.spbk_block_info_addr, ftl_hdl.spbk.spbk_block_info_len);

    return 0;
}

#if 0
static int ftl_align_block(int align_num, int num)
{
    return (num + align_num - 1) / align_num * align_num;
}

static int ftl_create_part_info(int cap, int align_num)
{
    // 备份区占flash 1/32 大小
    // 冷区交换区 占备份区 1/4 大小
    int res;
    memcpy(ftl_hdl.spbk.spbk_logo, "spbk", 4);
    ftl_hdl.spbk.spbk_flash_size = cap;
    int page_num = cap / (ftl_hdl.spbk.spbk_page_size / 512); //总page数
    int backup_num = page_num / 32; //备份区占 page数

    ftl_hdl.spbk.spbk_cold_part_exaddr = ftl_align_block(align_num, page_num / 32 * 31); //备份区占 1/32 后面的位置.
    ftl_hdl.spbk.spbk_hot_part_exaddr = ftl_align_block(align_num, ftl_hdl.spbk.spbk_cold_part_exaddr + backup_num / 4); //热交换区起始位置，以block大小对齐,单位page。
    ftl_hdl.spbk.spbk_block_info_len = ftl_align_block(align_num, page_num * 8 / ftl_hdl.spbk.spbk_page_size); // block_info的长度，以block大小对齐， 单位page。
    ftl_hdl.spbk.spbk_block_info_logaddr = page_num - 2 * ftl_hdl.spbk.spbk_block_info_len; // block_info log地址 在最后，总page_num - block_info len x 2.
    ftl_hdl.spbk.spbk_block_info_addr = page_num - ftl_hdl.spbk.spbk_block_info_len; // block_info地址 在最后，总page_num - block_info len.
    ftl_hdl.spbk.spbk_hot_part_addr = align_num; // super block 占一个block 即align_num 的page大小.
    ftl_hdl.spbk.spbk_mount_size = ftl_hdl.spbk.spbk_cold_part_exaddr - ftl_hdl.spbk.spbk_hot_part_addr; // 挂载的fat大小，单位page .
#if 0
    res = f_format("nandflash", "fat", ftl_hdl.spbk.spbk_page_size); //  挂载一个fat到flash上。
    if (res) {
        return FTL_MOUNT_FAT_ERR;
    }

    char *bpb = zalloc(512);
    res =  __ftl_read(ftl_hdl.hdev, fbuf, ftl_hdl.spbk.spbk_hot_part_addr * ftl_hdl.spbk.spbk_page_size / 512, 1);
    if (res) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
    int database = ftl_ld_word(&bpb[14]) + ftl_ld_dword(&bpb[36]) * bpb[16] + ftl_ld_word(&bpb[17]) / 16 + ftl_hdl.spbk.spbk_page_size / 512; // fatbase + fatszie * fatnum + n_root /16 + 目录项占一个簇大小，转化为512扇区。
    ftl_hdl.spbk.spbk_cold_part_addr = ftl_align_block(align_num, ftl_hdl.spbk.spbk_hot_part_addr + database * 512 / ftl_hdl.spbk.spbk_page_size); //从格式化好的BPB数据获取到数据区地址，转化为page单位的冷区地址。
    res = ftl_update_spbk_info();
__exit:
    if (bpb) {
        free(bpb);
    }
#endif
    return res;
}
#endif

static int ftl_format(int clust_size)
{
    int res;
    char *ftl_buf = NULL;
    char *page_buf = NULL;
    int mount_cap = ftl_hdl.spbk.spbk_mount_size * ftl_hdl.spbk.spbk_block_size / 512;
    if (mount_cap < 32 * 1024 * 1024 / 512) {
        res = FTL_DISK_ERR;
        goto __exit;
    }

    int secPerClus = clust_size / 512;
    int vir_cap = ftl_ld_dword(&ftl_bpb_data[BPB_TotSec32]);

    ftl_buf = zalloc(512);
    memcpy(ftl_buf, ftl_bpb_data, 512);
    if (mount_cap != vir_cap) {
        ftl_fat_copy(&ftl_buf[BPB_TotSec32], &mount_cap, 4);
    }
    int fatsize = (mount_cap * 512 / clust_size * 2 + clust_size + 1) / clust_size * clust_size; //计算fat表大小,按fat16来,注意fat表前面占有F8FFFFFF.
    int fat_sector = fatsize / 512;
    int rsvdsec = clust_size / 512;
    int fatbase = rsvdsec;
    r_printf(">>>[test]:fat_sector = 0x%x, rsvdsec = 0x%x\n", fat_sector, rsvdsec);
    ftl_fat_copy(&ftl_buf[BPB_FATSz16], &fat_sector, 2);
    ftl_fat_copy(&ftl_buf[BPB_RsvdSecCnt], &rsvdsec, 2);

    put_buf(ftl_buf, 512);
    ftl_buf[BPB_SecPerClus] = secPerClus;
    //更新写入DBR信息
    res = ftl_dev_write(NULL, ftl_buf, 1, 0);
    if (res != 1) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
    ///////test write//////
    /* u8 test_buf[512]= {0}; */
    /* res = ftl_dev_read(NULL, test_buf, 1, 0); */
    /* put_buf(test_buf, 512); */
    /* memset(test_buf, 0, 512); */
    /* res = ftl_dev_read(NULL, test_buf, 1, 2); */
    /* put_buf(test_buf, 512); */
    /////////////

    //更新冷区地址
    int database = ftl_ld_word(&ftl_buf[BPB_RsvdSecCnt]) + ftl_ld_word(&ftl_buf[BPB_FATSz16]) * ftl_buf[BPB_NumFATs] + ftl_ld_word(&ftl_buf[BPB_RootEntCnt]) * 32 / 512; // fatbase + fatszie * fatnum + n_root /16 。 fat16默认
    r_printf(">>>[test]:database = %d\n", database);
    int hot_part_len = (database * 512 + ftl_hdl.spbk.spbk_block_size - 1) / ftl_hdl.spbk.spbk_block_size;
    ftl_hdl.spbk.spbk_cold_part_addr = ftl_hdl.spbk.spbk_hot_part_addr + hot_part_len ; //从格式化好的BPB数据获取到数据区地址，转化为page单位的冷区地址。

#if 0
    //更新写入FAT表信息.默认fat16
    memset(ftl_buf, 0, 512);
    u8 *p = ftl_buf;
    p[0] = 0xF8;
    p[1] = p[2] = p[3] = 0xFF;
    res = ftl_dev_write(NULL, ftl_buf, 1, fatbase);
    if (res != 1) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
    //清0 fat表和目录项 避免文件系统检查到0xff然后判断出错
    int offset = ftl_hdl.spbk.spbk_hot_part_addr * ftl_hdl.spbk.spbk_block_size / 512;
    printf(">>>[test]:FAT table & DIR entry start = %d, end = %d\n", fatbase + 1 + offset, database + offset);
    memset(ftl_buf, 0, 512);
    for (int sec = (fatbase + 1); sec < database; sec++) {
        res = ftl_dev_write(NULL, ftl_buf, 1, sec);
        if (res != 1) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        res = ftl_dev_read(NULL, ftl_buf, 1, sec);
        if (res != 1) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        for (int i = 0; i < 512; i++) {
            if (ftl_buf[i] != 0) {
                res = FTL_DISK_ERR;
                goto __exit;
            }
        }
    }
    res = FTL_OK;
#else
    //: 上面基本操作读写时间过长，导致初始化时间达到 13s左右，加速处理此过程。
    // 保证有一次 ftl_dev_write可以获取到坏簇信息，拿到正确地址的block： ftl_hdl.cur_block.
    // 加速处理初始化可快至2.4s.
    //
    //更新写入FAT表信息.默认fat16
    char DBR[512];
    memcpy(DBR, ftl_buf, 512); // 提前保存DBR数据。
    memset(ftl_buf, 0, 512);
    u8 *p = ftl_buf;
    p[0] = 0xF8;
    p[1] = p[2] = p[3] = 0xFF;
    /* res = ftl_dev_write(NULL, ftl_buf, 1, fatbase); */
    /* if (res != 1) { */
    /*     res = FTL_DISK_ERR; */
    /*     goto __exit; */
    /* } */
    page_buf = zalloc(FTL_NAND_PAGE_SIZE);//使用最大page的长度读写（2048）
    if (fatbase * 512 % FTL_NAND_PAGE_SIZE) {
        memcpy(page_buf + fatbase * 512 % FTL_NAND_PAGE_SIZE, ftl_buf, 512);
    } else {
        memcpy(page_buf, ftl_buf, 512);
    }
    //清0 fat表和目录项 避免文件系统检查到0xff然后判断出错
    int offset = ftl_hdl.cur_block * ftl_hdl.spbk.spbk_block_size / 512;
    printf(">>>[test]:FAT table & DIR entry start = %d, end = %d\n", fatbase + offset, database + offset);
    int first_page = (fatbase + offset) * 512 / FTL_NAND_PAGE_SIZE;
    int end_page = ((database + offset) * 512 + FTL_NAND_PAGE_SIZE - 1) / FTL_NAND_PAGE_SIZE;
    int first_block = ftl_hdl.cur_block;
    int all_block = ((end_page - first_page) * FTL_NAND_PAGE_SIZE + FTL_NAND_BLOCK_SIZE - 1) / FTL_NAND_BLOCK_SIZE;
    for (int i = ftl_hdl.cur_block; i < ftl_hdl.cur_block + all_block; i++) {
        printf(">>>[test]:i = %d\n", i);
        res  = ftl_get_block_info(i, &ftl_hdl.block_info, 0);
        if (res) {
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            goto __exit;
        }
        res = __ftl_erase(ftl_hdl.cur_block);
        // TODO : 记录擦写次数++
        if (res) {
            printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            res = ftl_err_block_deal(ftl_hdl.cur_block);
            if (res) {
                goto __exit;
            }
        }
    }
    //写入DBR信息和fat表初始信息。
    if (fatbase * 512 < FTL_NAND_PAGE_SIZE) {
        memcpy(page_buf, DBR, 512);
        res = __ftl_write(ftl_hdl.hdev, page_buf, first_page * FTL_NAND_PAGE_SIZE / 512, FTL_NAND_PAGE_SIZE / 512);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
    } else {
        printf(">>>[test]:offset = %d, first_page = %d\n", offset, first_page);
        res = __ftl_write(ftl_hdl.hdev, DBR, offset, 1);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        res = __ftl_write(ftl_hdl.hdev, page_buf, first_page * FTL_NAND_PAGE_SIZE / 512, FTL_NAND_PAGE_SIZE / 512);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
    }
    memset(page_buf, 0, FTL_NAND_PAGE_SIZE);
    int block_page = FTL_NAND_BLOCK_SIZE / FTL_NAND_PAGE_SIZE;
    int all_page = end_page - first_page - 1;
    int sec = 0;
    int page_num = 0;
    int start = first_page + 1;
    while (all_page > 0) {
        //: 连续读写超过1个block，注意是否有坏块，需要处理
        page_num = start + sec;
        /* printf(">>>[test]:sec = %d, page_num = %d\n", sec, page_num); */
        if (0 == page_num % block_page) {
            res  = ftl_get_block_info(page_num / block_page, &ftl_hdl.block_info, 0);
            if (res) {
                y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
                goto __exit;
            }
            sec = 0;
            all_page -= block_page;
            start = ftl_hdl.cur_block * block_page;
            page_num = start + sec;
            /* printf(">>>[test]:sec = %d, page_num = %d\n", sec, page_num); */
        }
        res = __ftl_write(ftl_hdl.hdev, page_buf, page_num * FTL_NAND_PAGE_SIZE / 512, FTL_NAND_PAGE_SIZE / 512);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        res = __ftl_read(ftl_hdl.hdev, page_buf, page_num * FTL_NAND_PAGE_SIZE / 512, FTL_NAND_PAGE_SIZE / 512);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        for (int i = 0; i < FTL_NAND_PAGE_SIZE; i++) {
            if (page_buf[i] != 0) {
                printf(">>>[test]:i = %d, page_buf[i] = %d\n", i, (u8)page_buf[i]);
                res = FTL_DISK_ERR;
                goto __exit;
            }
        }
        sec++;
        if (sec > all_page) {
            break;
        }
    }

    res = FTL_OK;
#endif
    printf(">>>[test]:format okokokok\n");

__exit:
    if (ftl_buf) {
        free(ftl_buf);
    }
    if (page_buf) {
        free(page_buf);
    }
    return res;
}

int _ftl_format()
{
    int res;
    int clust_size = ftl_hdl.spbk.spbk_block_size / 16; //簇大小为8K，簇数量65536 / 2(256M byte的nandflash)，每个簇16个扇区(簇数量符合fat16)
    /* res = f_format("nandflash_ftl", "fat", clust_size); //  挂载一个fat到flash上。 */
    res = ftl_format(clust_size); //  挂载一个fat到flash上。
    if (res) {
        return FTL_MOUNT_FAT_ERR;
    }
    return res;
}

int _ftl_init(void)
{
    int res;
    int cap;
    ftl_hdl.spbk.spbk_page_size = FTL_NAND_PAGE_SIZE;
    ftl_hdl.spbk.spbk_block_size = FTL_NAND_BLOCK_SIZE;
    res = dev_ioctl(ftl_hdl.hdev, IOCTL_GET_CAPACITY, (u32)&cap);
    if (res) {
        return FTL_DISK_ERR;
    }
    int block_num = cap / ftl_hdl.spbk.spbk_block_size; //总block数
    r_printf(">>>[test]:cap = 0x%x, block_num = %d\n", cap, block_num);
    ftl_create_part_info(cap, block_num); //全擦前预设置好地址。
    r_printf(">>>[test]:goto erase all\n");
    res = ftl_erase_all(cap, block_num);
    if (res) {
        return FTL_DISK_ERR;
    }
    //TODO : 格式化
    y_printf(">>>[test]:goto format\n");
    res = _ftl_format();
    if (res == FTL_OK) {
        //TODO : 更新spbk_info
        res = ftl_update_spbk_info();
    }
    return res;
}

int _ftl_fcopy_block_bak(int org, int dst, char *tmp_buf, int sec_addr, char *wbuf, int buflen)
{
    int res;
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    int block_page = ftl_hdl.spbk.spbk_block_size / FTL_NAND_PAGE_SIZE; // 一个block占的page数。
    int page_sec = FTL_NAND_PAGE_SIZE / 512;
    res = __ftl_erase(dst);
    // TODO : 记录擦写次数++
    if (res) {
        ftl_err_block_deal(dst);
        goto __exit;
    }
    r_printf(">>>[test]:org = %d, dst = %d, sec_addr = %d\n", org, dst, sec_addr);
    for (int i = 0; i < block_page; i++) {
        res =  __ftl_read(ftl_hdl.hdev, tmp_buf, org * block_sec + i * page_sec, page_sec);
        if (res) {
            res = FTL_DISK_ERR;
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            goto __exit;
        }
        if (sec_addr / page_sec == i) {
            memcpy(tmp_buf + sec_addr % page_sec * 512, wbuf, buflen);
        }
        int fwrite_time = sys_timer_get_ms();
        res = __ftl_write(ftl_hdl.hdev, tmp_buf, dst * block_sec + i * page_sec, page_sec);
        y_printf(">>>[test]:in ftl write time = %d\n", sys_timer_get_ms() - fwrite_time);
        if (res) {
            res = FTL_DISK_ERR;
            y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
            goto __exit;
        }
    }
__exit:
    return res;
}


int _ftl_write(void *buf, u32 logic_sec_addr, u32 len)
{
    int res;
    /* int rand_block; */
    /* char *tmp_buf = zalloc(FTL_NAND_PAGE_SIZE); //使用page大小代替512减少读写次数加快速度。 */
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    res  = ftl_get_block_info(logic_sec_addr / block_sec, &ftl_hdl.block_info, 0);
    if (res) {
        g_printf("\n >>>[test]:func = %s,line= %d, location = %d\n", __FUNCTION__, __LINE__, logic_sec_addr / block_sec);
        goto __exit;
    }
    /* int real_sec_addr = ftl_hdl.cur_block * block_sec + logic_sec_addr % block_sec; */
    int sec_addr = logic_sec_addr % block_sec;
    r_printf(">>>[test]:wwww sec_addr = %d\n", sec_addr);
    if (ftl_hdl.new_cold_sblock != ftl_hdl.spbk.spbk_cold_part_exaddr) { //new_cold_sblock:表示可能扩展占用冷交换区地址。
        /* y_printf("\n >>>[test]:func = %s,line= %d\n",__FUNCTION__, __LINE__); */
        ftl_hdl.new_cold_sblock = ftl_hdl.spbk.spbk_cold_part_exaddr;
    }
    y_printf(">>>[test]:cur_block = %d, new_cold_sblock = %d\n", ftl_hdl.cur_block, ftl_hdl.new_cold_sblock);
    if (ftl_hdl.cur_block < ftl_hdl.spbk.spbk_cold_part_addr) { // 热区数据
        res = ftl_move_data(ftl_hdl.cur_block, sec_addr * 512, buf, len, 0);
    } else if (ftl_hdl.cur_block < ftl_hdl.new_cold_sblock) { //冷区数据
        res = ftl_move_data(ftl_hdl.cur_block, sec_addr * 512, buf, len, 1);
    } else {
        res = FTL_FLASH_ERR;
    }
__exit:
    return res;
}

int _ftl_read(void *buf, u32 logic_sec_addr, u32 len)
{
    int res;
    int block_sec = ftl_hdl.spbk.spbk_block_size / 512; // 一个block占的扇区。
    res  = ftl_get_block_info(logic_sec_addr / block_sec, &ftl_hdl.block_info, 0);
    if (res) {
        y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
        goto __exit;
    }
    int real_sec_addr = ftl_hdl.cur_block * block_sec + logic_sec_addr % block_sec;
    r_printf(">>>[test]:rrrr real_sec_addr = %d\n", real_sec_addr);
    res =  __ftl_read(ftl_hdl.hdev, buf, real_sec_addr, len);
    if (res) {
        res = FTL_DISK_ERR;
        goto __exit;
    }
__exit:
    return res;
}

int ftl_init(void)
{
    int res = FTL_OK;
    y_printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);

    /* ftl_test_erase_demo(); */
    /* ftl_test_write_demo(300); */
    /* __ftl_erase(0); */
    /* __ftl_erase(1); */

    if (ftl_init_flag) {
        char *fbuf = zalloc(512);
        res =  __ftl_read(ftl_hdl.hdev, fbuf, 0, 1);
        if (res) {
            res = FTL_DISK_ERR;
            goto __exit;
        }
        if (0 == memcmp(fbuf, "spbk", 4)) {
            memcpy(&ftl_hdl.spbk, fbuf, sizeof(SPBK_INFO));
        } else {
            y_printf(">>>[test]:goto _ftl_init()\n");
            put_buf(fbuf, 512);
            res = _ftl_init();
        }

__exit:
        if (fbuf) {
            free(fbuf);
        }
        ftl_init_flag = 0;
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////device api//////////////////////////////////////////////////////////

static struct device ftl_nandflash_dev;
static int ftl_dev_init(const struct dev_node *node, void *arg)
{
    r_printf(">>>[test]:init name  = %s\n", arg);
    if (arg) {
        memcpy(ftl_hdl.re_devname, arg, strlen(arg));
    }
    ftl_hdl.hdev = dev_open((const char *)ftl_hdl.re_devname, arg);
    /* printf(">>>[test]:test write\n"); */
    /* ftl_test_write_demo(1); */
    /* __ftl_erase(0); */



    int init_timer = sys_timer_get_ms();
    ftl_init();
    ftl_hdl.old_cold_exaddr = ftl_hdl.spbk.spbk_cold_part_exaddr;
    ftl_block_info_exchange();
    printf(">>>[test]:init_timer = %d\n", sys_timer_get_ms() - init_timer);

    nandflash_tflag = 1;
    return 0;
}

static int ftl_dev_open(const char *name, struct device **device, void *arg)
{
    y_printf(">>>[test]:dev_name = %s\n", ftl_hdl.re_devname);
    r_printf(">>>[test]:ftl flash open!!!!!!\n");
    if (ftl_hdl.hdev == NULL) {
        /* *device = NULL; */
        return FTL_DISK_ERR;
    }
    *device = &ftl_nandflash_dev;
    return 0;
}

static int ftl_dev_read(struct device *device, void *buf, u32 len, u32 addr)
{
    y_printf(">>>[test]:r；addr = %d ,len = %d\n", addr, len);
    clr_wdt();
    int res = 0;
    addr += ftl_hdl.spbk.spbk_hot_part_addr * ftl_hdl.spbk.spbk_block_size / 512;

    /* void *_buf = buf; */
    /* u32 _len = len; */
    /* u32 _lba = addr; */
    /* while (_len) { */
    /*     res = _ftl_read(_buf, _lba, 1); */
    /*     if (res) { */
    /*         return -1; */
    /*     } */
    /*     _len -= 1; */
    /*     _lba += 1; */
    /*     _buf += 512; */
    /* } */
    /* return len; */

    res = _ftl_read(buf, addr, len);
    if (res) {
        return -1;
    }
    return len;
}

static int ftl_dev_write(struct device *device, void *buf, u32 len, u32 addr)
{

    clr_wdt();
    /* printf(">>>[test]:w；addr = %d ,len = %d\n", addr, len); */
    int write_retry = 5;
    int res = 0;
    /* if (addr == 0) { */
    /*     if (!ftl_init_flag) { */
    /*         printf("\n >>>[test]:func = %s,line= %d\n",__FUNCTION__, __LINE__); */
    /*         put_buf(buf, len * 512); */
    /*         return len; */
    /*     } */
    /* } */
    addr += ftl_hdl.spbk.spbk_hot_part_addr * ftl_hdl.spbk.spbk_block_size / 512;
    void *_buf = buf;
    u32 _len = len * 512;
    u32 _lba = addr * 512;

    if (len > FTL_NAND_BLOCK_SIZE / 512) {
        printf(">>>[test]:not surport len = %d\n", len);
        return -1;
    }
    /* put_buf(buf, len * 512); */
    putchar('M');
    res = _ftl_write(_buf, _lba / 512, _len);
    if (res) {
        return -1;
    }

    /* u32 wlen; */
    /* while (_len) { */
    /*     wlen = _len; */
    /*     if (wlen > FTL_NAND_PAGE_SIZE) { */
    /*         wlen = FTL_NAND_PAGE_SIZE; */
    /*     } */
    /*     for (int i = 0; i < write_retry; i++) { */
    /*         res = _ftl_write(_buf, _lba / 512, wlen); */
    /*         if (!res) { */
    /*             break; */
    /*         } */
    /*     } */
    /*     if (res) { */
    /*         return -1; */
    /*     } */
    /*     _len -= wlen; */
    /*     _lba += wlen; */
    /*     _buf += wlen; */
    /* } */
    /* u8 test_buf[512] = {0}; */
    /* __ftl_read(ftl_hdl.hdev, test_buf, addr, 1); */
    /* y_printf(">>>[test]:addr = %d, res = %d\n", addr, res); */
    /* put_buf(test_buf, 512); */
    /* while(1); */
    return len;

}

static bool ftl_dev_online(const struct dev_node *node)
{
    return 1;
}

static int ftl_dev_close(struct device *device)
{
    dev_close(device);
    return 0;
}

static int ftl_dev_ioctrl(struct device *device, u32 cmd, u32 arg)
{
    int res = 0;
    switch (cmd) {
    case IOCTL_GET_STATUS:
        *(u32 *)arg = 1;
        break;
    case IOCTL_GET_ID:
        dev_ioctl(device, IOCTL_GET_ID, (u32)&arg);
        break;
    case IOCTL_GET_CAPACITY:
        *((u32 *)arg) = ftl_hdl.spbk.spbk_mount_size * ftl_hdl.spbk.spbk_block_size / 512;
        break;
    case IOCTL_GET_BLOCK_SIZE:
        *((u32 *)arg) = 512;
        break;
    case IOCTL_ERASE_PAGE:
    case IOCTL_ERASE_SECTOR:
    case IOCTL_ERASE_BLOCK:
        /* y_printf(">>>[test]:in flash_vir eraser addr = %d\n", arg); */
        /* err = virfat_flash_erase(cmd, arg); */
        break;
    case IOCTL_FLUSH:
        break;
    case IOCTL_CMD_RESUME:
        break;
    case IOCTL_CMD_SUSPEND:
        break;
    case IOCTL_SET_ASYNC_MODE:
        break;
    default:
        res = -1;
        break;
    }
    return res;
}


const struct device_operations nandflash_ftl_ops = {
    .init = ftl_dev_init,
    .online = ftl_dev_online,
    .open = ftl_dev_open,
    .read = ftl_dev_read,
    .write = ftl_dev_write,
    .ioctl = ftl_dev_ioctrl,
    .close = ftl_dev_close,
};

void ftl_task()
{
    //TODO : 回收功能
}
