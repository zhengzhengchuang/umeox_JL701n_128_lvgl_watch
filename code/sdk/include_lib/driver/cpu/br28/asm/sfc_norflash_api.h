#ifndef __SFC_NORFLASH_API_H__
#define __SFC_NORFLASH_API_H__

#include "typedef.h"

struct flash_wp_arg {
    u8 split_mode: 1;		//0 连续写，1，分开写
    u8 write_en_use_50h: 1;	//0:06H		1:50H
    u8 res: 2;
    u8 numOfwp_array: 4;			//写保护参数的个数
    struct {
        u8 sr1;
        u8 sr2;
        u16 wp_addr;	//写保护结束地址,单位K
    } wp_array[9];
} __attribute__((packed));

int norflash_init(const struct dev_node *node, void *arg);
int norflash_open(const char *name, struct device **device, void *arg);
int norflash_read(struct device *device, void *buf, u32 len, u32 offset);
int norflash_write(struct device *device, void *buf, u32 len, u32 offset);
int norflash_ioctl(struct device *device, u32 cmd, u32 arg);

int norflash_write_protect_config(struct device *device, u32 addr, struct flash_wp_arg *p);//addr传0时，解除写保护
/* ---------------------------------------------------------------------------- */
/**
 * @brief sfc加锁
 *
 * @param index: 0: sfc0, 1: sfc1
 * @note: 该接口不能在中断中调用,且调用前不能关中断
 */
/* ---------------------------------------------------------------------------- */
void sfc_protect_lock(u8 index);

/* ---------------------------------------------------------------------------- */
/**
 * @brief sfc解锁
 *
 * @param index: 0: sfc0, 1: sfc1
 */
/* ---------------------------------------------------------------------------- */
void sfc_protect_release(u8 index);

//SFC Norflash自定义IOCTL命令:
#define IOCTL_SFCN_CMD_DEFINE(cmd) 					(('S' << 24) | ('F' << 16) | ('N' << 8) | (cmd << 0))

#define IOCTL_SFC_NORFLASH_READ_NO_ENC             	IOCTL_SFCN_CMD_DEFINE(1)
#define IOCTL_SFC_NORFLASH_WRITE_NO_ENC           	IOCTL_SFCN_CMD_DEFINE(2)

struct sfc_no_enc_wr {
    u8 *buf;
    u32 addr;
    u32 len;
};

int sfc_spi_read_no_enc(u32 addr, void *buf, u32 len);

void sfc_spi_write_enc_set(u8 enc);

#endif
