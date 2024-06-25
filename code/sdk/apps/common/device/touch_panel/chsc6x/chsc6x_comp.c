#include "chsc6x_comp.h"
#include "chsc6x_ramcode.h"
#include "chsc6x_flash_boot.h"
#include "chsc6x_platform.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_CHSC6X_ENABLE

#define TXRX_ADDR       (0x9000)
#define CMD_ADDR        (0x9f00)
#define RSP_ADDR        (0x9f40)
//#define MAX_CHIP_ID     (10)
//unsigned char chsc6x_chip_name[MAX_CHIP_ID][20];
static unsigned int g_chsc6x_cfg_ver = 0; 
static unsigned int g_chsc6x_boot_ver = 0;

static unsigned int g_mccode;    /* 1:3536 */
static unsigned int g_upgrade_flag = 0; /* 0:driver init upgrade, 1:OTA upgrade */
static unsigned char g_i2c_addr;
struct ts_fw_infos *g_pfw_infos;

struct chsc6x_updfile_header {
    unsigned int sig;
    unsigned int resv;
    unsigned int n_cfg;
    unsigned int n_match;
    unsigned int len_cfg;
    unsigned int len_boot;
};

typedef struct _test_cmd_wr {
    /* offset 0; */
    unsigned char id;    /* cmd_id; */
    unsigned char idv;    /* inverse of cmd_id */
    unsigned short d0;    /* data 0 */
    unsigned short d1;    /* data 1 */
    unsigned short d2;    /* data 2 */
    /* offset 8; */
    unsigned char resv;    /* offset 8 */
    unsigned char tag;    /* offset 9 */
    unsigned short chk;    /* 16 bit checksum */
    unsigned short s2Pad0;    /*  */
    unsigned short s2Pad1;    /*  */
} ctp_tst_wr_t;

typedef struct _test_cmd_rd {
    /* offset 0; */
    unsigned char id;    /* cmd_id; */
    unsigned char cc;    /* complete code */
    unsigned short d0;    /* data 0 */
    unsigned short sn;    /* session number */
    unsigned short chk;    /* 16 bit checksum */
} ctp_tst_rd_t;

#define EPERM           (1)
#define DIRECTLY_MODE   (0x0)
#define DEDICATE_MODE   (0x1)
#define LEN_CMD_CHK_TX  (10)
#define LEN_CMD_PKG_TX  (16)
#define LEN_RSP_CHK_RX  (8)
#define MAX_BULK_SIZE   (1024)

/* to direct memory access mode */
static unsigned char cmd_2dma_42bd[6] = { /*0x42, 0xbd, */ 0x28, 0x35, 0xc1, 0x00, 0x35, 0xae };


/* RETURN:0->pass else->fail */
static int chsc6x_read_bytes_u16addr(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
	int ret = 0;
    int retry;
    unsigned short ofs_adr;
    int len = lenth;
    int rd_len = 0;
	int offset = 0;

	while (len > 0) {
        ofs_adr = adr + offset;
		if (len > MAX_IIC_RD_LEN) {
			rd_len = MAX_IIC_RD_LEN;
			len -= MAX_IIC_RD_LEN;
		} else {
			rd_len = len;
			len = 0;
		}

		retry = 0;
		while (chsc6x_read_bytes_u16addr_sub(id, ofs_adr, &rxbuf[offset], rd_len) != 0) {
			if (retry++ == 3) {
				ret = -1;
				break;
			}
		}
		offset += MAX_IIC_RD_LEN;
		if (ret < 0) {
			break;
		}
	}

	return ret;
}

/* RETURN:0->pass else->fail */
static int chsc6x_write_bytes_u16addr(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
	int ret = 0;
    int retry;
    unsigned short ofs_adr;
    int len = lenth;
    int wr_len = 0;
	int offset = 0;

	while (len > 0) {
        ofs_adr = adr + offset;
		if (len > MAX_IIC_WR_LEN) {
			wr_len = MAX_IIC_WR_LEN;
			len -= MAX_IIC_WR_LEN;
		} else {
			wr_len = len;
			len = 0;
		}

		retry = 0;
		while (chsc6x_write_bytes_u16addr_sub(id, ofs_adr, &rxbuf[offset], wr_len) != 0) {
			if (retry++ == 3) {
				ret = -1;
				break;
			}
		}
		offset += MAX_IIC_WR_LEN;
		if (ret < 0) {
			break;
		}
	}

	return ret;
}

/* <0 : i2c error */
/* 0: direct address mode */
/* 1: protect mode */
static int chsc6x_get_i2cmode(void)
{
    unsigned char regData[4];

    if (chsc6x_read_bytes_u16addr(g_i2c_addr, 0x01, regData, 3)) {
        return -EPERM;
    }

    if ((0x5c == regData[0]) && (regData[2] == 0X01)) {
        return DIRECTLY_MODE;
    }

    return DEDICATE_MODE;
}

/* 0:successful */
static int chsc6x_set_dd_mode(void)
{
    int mod = -1;
    int retry = 0;
    int ret = 0;
    ret = chsc6x_get_i2cmode();
    if (ret < 0) {
        return ret;
    }
    if (ret == DIRECTLY_MODE) {
        return 0;
    }

    while (retry++ < 5) {
        chsc6x_msleep(20);
        chsc6x_write_bytes_u16addr(g_i2c_addr, 0x42bd, cmd_2dma_42bd, 6);
        chsc6x_msleep(30);
        mod = chsc6x_get_i2cmode();
        if (mod == DIRECTLY_MODE) {
            break;
        }
    }

    if (mod == DIRECTLY_MODE) {
        return 0;
    } else {
        return -EPERM;
    }
}

/* ret=0 : successful */
/* write with read-back check, in dd mode */
static int chsc6x_bulk_down_check(unsigned char *pbuf, unsigned short addr, unsigned short len)
{
    unsigned int j, k, retry;
    unsigned char rback[128];

    while (len) {
        k = (len < 128) ? len : 128;
        retry = 0;
        do {
            rback[k - 1] = pbuf[k - 1] + 1;
            chsc6x_write_bytes_u16addr(g_i2c_addr, addr, pbuf, k);
            chsc6x_read_bytes_u16addr(g_i2c_addr, addr, rback, k);
            for (j = 0; j < k; j++) {
                if (pbuf[j] != rback[j]) {
                    break;
                }
            }
            if (j >= k) {
                break;    /* match */
            }
        } while (++retry < 3);

        if (j < k) {
            break;
        }

        addr += k;
        pbuf += k;
        len -= k;
    }

    return (int)len;
}

static unsigned short chsc6x_checksum_u16(unsigned short *buf, unsigned short length)
{
    unsigned short sum, len, i;

    sum = 0;
    len = length >> 1;
    for (i = 0; i < len; i++) {
        sum += buf[i];
    }

    return sum;
}

static unsigned int chsc6x_checksumEx(unsigned char *buf, unsigned short length)
{
    unsigned int combChk;
    unsigned short k, check, checkEx;

    check = 0;
    checkEx = 0;
    for (k = 0; k < length; k++) {
        check += buf[k];
        checkEx += (unsigned short) (k * buf[k]);
    }
    combChk = (checkEx << 16) | check;

    return combChk;
}

/* 0:successful */
static int chsc6x_download_ramcode(unsigned char *pcode, unsigned short len)
{
    unsigned char dwr, retry;
    int ret = -2;
    int sig;

    if (chsc6x_set_dd_mode()) {
        return -EPERM;
    }

    sig = (int) pcode[3];
    sig = (sig<<8) + (int) pcode[2];
    sig = (sig<<8) + (int) pcode[1];
    sig = (sig<<8) + (int) pcode[0];

    if (sig == 0x6d6f8008) {
        sig = 0;
        chsc6x_read_bytes_u16addr(g_i2c_addr, 0x8000, (unsigned char *) &sig, 4);
        if (sig == 0x6d6f8008) {
            return 0;
        }
    }

    dwr = 0x05;
    if (chsc6x_bulk_down_check(&dwr, 0x0602, 1) == 0) {    /* stop mcu */
        dwr = 0x00;
        chsc6x_bulk_down_check(&dwr, 0x0643, 1);    /* disable irq */
    } else {
        return -EPERM;
    }
    if (chsc6x_bulk_down_check(pcode, 0x8000, len) == 0) {
        dwr = 0x88;
        retry = 0;
        do {
            ret = chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0602, &dwr, 1);
        } while ((++retry < 3) && (ret != 0));
    }
    
    chsc6x_msleep(30);   // let caller decide the delay time ?

    return ret;
}

/* return 0=successful: send cmd and get rsp. */
static int chsc6x_cmd_send(ctp_tst_wr_t *ptchcw, ctp_tst_rd_t *pcr)
{
    int ret;
    unsigned int retry;

    retry = 0;
    chsc6x_write_bytes_u16addr(g_i2c_addr, RSP_ADDR, (unsigned char *) &retry, 1);

    /* send command */
    ptchcw->idv = ~(ptchcw->id);
    ptchcw->tag = 0x35;
    ptchcw->chk = 1 + ~(chsc6x_checksum_u16((unsigned short *) ptchcw, LEN_CMD_CHK_TX));
    ptchcw->tag = 0x30;
    ret = chsc6x_write_bytes_u16addr(g_i2c_addr, CMD_ADDR, (unsigned char *) ptchcw, LEN_CMD_PKG_TX);
    if (ret) {
        goto exit;
    }
    ptchcw->tag = 0x35;
    ret = chsc6x_write_bytes_u16addr(g_i2c_addr, CMD_ADDR + 9, (unsigned char *) &(ptchcw->tag), 1);
    if (ret) {
        goto exit;
    }
    /* polling rsp, the caller must init rsp buffer. */
    ret = -1;
    retry = 0;
    while (retry++ < 100) {    /* 2s */
        chsc6x_msleep(20);
        if (chsc6x_read_bytes_u16addr(g_i2c_addr, RSP_ADDR, (unsigned char *) pcr, 1)) {
            break;
        }

        if (ptchcw->id != pcr->id) {
            continue;
        }
        /* chsc6x_msleep(50); */
        chsc6x_read_bytes_u16addr(g_i2c_addr, RSP_ADDR, (unsigned char *) pcr, LEN_RSP_CHK_RX);
        if (!chsc6x_checksum_u16((unsigned short *) pcr, LEN_RSP_CHK_RX)) {
            if ((ptchcw->id == pcr->id) && (pcr->cc == 0)) {
                ret = 0;
            }
        }
        break;
    }
exit:
    return ret;

}

/* return 0=successful */
static int chsc6x_read_burn_space(unsigned char *pdes, unsigned short adr, unsigned short len)
{
    int rsp = -1;
    unsigned int left = len;
    unsigned int combChk, retry;
    ctp_tst_wr_t m_cmd;
    ctp_tst_rd_t m_rsp;

    m_cmd.id = 0x31;
    m_cmd.resv = 0x03;
    while (left) {
        len = (left > MAX_BULK_SIZE) ? MAX_BULK_SIZE : left;

        m_cmd.d0 = adr;
        m_cmd.d1 = len;

        rsp = -1;
        retry = 0;
        while (retry++ < 3) {
            m_rsp.id = 0;
            if (chsc6x_cmd_send(&m_cmd, &m_rsp) == 0X0) {
                chsc6x_read_bytes_u16addr(g_i2c_addr, TXRX_ADDR, pdes, len);
                combChk = chsc6x_checksumEx(pdes, len);
                if (m_rsp.d0 == (unsigned short)combChk) {
                    if (m_rsp.sn == (unsigned short)(combChk >> 16)) {
                        rsp = 0;
                        break;
                    }
                }
            }
        }

        if (rsp < 0) {
            break;
        }
        left -= len;
        adr += len;
        pdes += len;
    }

    return rsp;
}

static int chsc6x_write_burn_space(unsigned char *psrc, unsigned short adr, unsigned short len)
{
    int rsp = 0;
    unsigned short left = len;
    unsigned int retry, combChk;
    ctp_tst_wr_t m_cmd;
    ctp_tst_rd_t m_rsp;

    m_cmd.id = 0x30;
    m_cmd.resv = 0x11;

    while (left) {
        len = (left > MAX_BULK_SIZE) ? MAX_BULK_SIZE : left;
        combChk = chsc6x_checksumEx(psrc, len);

        m_cmd.d0 = adr;
        m_cmd.d1 = len;
        m_cmd.d2 = (unsigned short) combChk;
        m_cmd.s2Pad0 = (unsigned short) (combChk >> 16);

        rsp = -1;    /* avoid dead loop */
        retry = 0;
        while (retry < 3) {
            chsc6x_write_bytes_u16addr(g_i2c_addr, TXRX_ADDR, psrc, len);
            m_rsp.id = 0;
            rsp = chsc6x_cmd_send(&m_cmd, &m_rsp);
            if (rsp < 0) {
                if ((m_rsp.d0 == 0X05) && (m_rsp.cc == 0X09)) {    /* fotal error */
                    break;
                }
                retry++;
            } else {
                left -= len;
                adr += len;
                psrc += len;
                break;
            }
        }

        if (rsp < 0) {
            break;
        }
    }

    return (!left) ? 0 : -1;
}


static int is_valid_cfg_data(unsigned short *ptcfg)
{
    if (ptcfg == 0) {
        return 0;
    }

    if ((unsigned char) (ptcfg[53] & 0xff) != 0x5c) {
        chsc6x_err("chsc6x: cfg iic adr error !\r\n");
        return 0;
    }

    if (chsc6x_checksum_u16(ptcfg, 204)) {
        chsc6x_err("chsc6x: cfg chk_sum error !\r\n");
        return 0;
    }

    return 1;
}

/*return: 1 allow; 0 no-allow*/
static int is_tpcfg_update_allow(unsigned short *ptcfg)
{
    unsigned int u32tmp;
    unsigned short vnow, vbuild;

    if (g_chsc6x_cfg_ver == 0) {    /* no available version information */
        return 0;
    }

    if (is_valid_cfg_data(ptcfg) == 0) {
        return 0;
    }

    u32tmp = ptcfg[1];
    u32tmp = (u32tmp << 16) | ptcfg[0];
    if ((g_chsc6x_cfg_ver & 0x3ffffff) != (u32tmp & 0x3ffffff)) {
        chsc6x_err("chsc6x: prj info not match,now_cfg=0x%x:build_cfg=0x%x!\r\n",(g_chsc6x_cfg_ver&0x3ffffff), (u32tmp&0x3ffffff));
        return 0;
    }

    vnow = (g_chsc6x_cfg_ver >> 26) & 0x3f;
    vbuild = (u32tmp >> 26) & 0x3f;
    chsc6x_info("chsc6x: cfg_vnow: 0x%x,cfg_vbuild: 0x%x \r\n", vnow, vbuild);
	if (0 == g_upgrade_flag && vbuild <= vnow) {
		return 0; //driver init upgrade, must vbuild > vnow
	}

    if(1 == g_upgrade_flag && vbuild == vnow) {
		return 0; //OTA upgrade just vbuild != vnow */
    }

    return 1;
}

static int chsc6x_update_fcomp_cfg(unsigned short *ptcfg)
{
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn))) {
        chsc6x_err("chsc6x: update fcomp_cfg error:ram-code error!\r\n");
        return -EPERM;
    }

    if (chsc6x_write_burn_space((unsigned char *)ptcfg, 0x8000, 204)) {
        chsc6x_err("chsc6x: update fcomp_cfg fail!\r\n");
        return -EPERM;
    }

    g_chsc6x_cfg_ver = (ptcfg[1] << 16) | ptcfg[0];
    g_pfw_infos->chsc6x_cfg_version = g_chsc6x_cfg_ver>>26;

    return 0;
}

static int chsc6x_update_fcomp_boot(unsigned char *pdata, unsigned short len)
{
    unsigned char buf[1] = {0x4b};
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn))) {
        chsc6x_err("chsc6x: update fcomp_boot error:ram-code error!\r\n");
        return -EPERM;
    }

    //pdata[8] = 0xff;
    if (chsc6x_write_burn_space((unsigned char *)pdata, 0x00, len)) {
        chsc6x_err("chsc6x: update fcomp_boot fail!\r\n");
        return -EPERM;
    }

    if (chsc6x_write_burn_space((unsigned char *)buf, 0x08, 1)) {
        chsc6x_err("chsc6x:update fcomp_boot last sig-byte fail!\r\n");
        return -EPERM;
    }

    g_chsc6x_boot_ver = pdata[5];
    g_chsc6x_boot_ver = (g_chsc6x_boot_ver<<8) + pdata[4];
    g_pfw_infos->chsc6x_boot_version = g_chsc6x_boot_ver;
    
    chsc6x_info("chsc6x: fcomp_boot update pass!\r\n");

    return 0;
}


/*return: 0 SUCESS else FAIL*/
static int chsc6x_get_running_cfg(unsigned short *ptcfg, unsigned short addr)
{
    int retry, err_type;
    retry = 0;
    err_type = 0;
    
    chsc6x_set_dd_mode();
    while (++retry < 5) {
        err_type = 0;
        if (chsc6x_read_bytes_u16addr(g_i2c_addr, addr, (unsigned char *) ptcfg, 204)) {
            chsc6x_msleep(20);
            err_type = 2;    /* i2c error */
            continue;
        }

        if (is_valid_cfg_data(ptcfg) == 0) {
            chsc6x_set_dd_mode();
            err_type = 1;    /* data error or no data */
            chsc6x_msleep(20);
            continue;
        }
        break;
    }
    
    return err_type;

}

/*find factory first burnning cfg info*/
/*return: 0 SUCESS else FAIL*/
static int chsc6x_find_ver(void)
{
    unsigned short buf_tmpcfg[102];
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn))) {
        chsc6x_err("chsc6x: find factory cfg error:ram-code error!\r\n");
        return -EPERM;
    }

    if (chsc6x_read_burn_space((unsigned char *) buf_tmpcfg, 0xf000, 204)) {
        chsc6x_err("chsc6x: read factory cfg fail !\r\n");
        return -EPERM;
    }

    if (is_valid_cfg_data(buf_tmpcfg)) {
        chsc6x_info("chsc6x: read factory cfg OK !\r\n");
        g_chsc6x_cfg_ver = (unsigned int)buf_tmpcfg[1];
        g_chsc6x_cfg_ver = (g_chsc6x_cfg_ver<<16) + (unsigned int)buf_tmpcfg[0];
        g_chsc6x_cfg_ver = g_chsc6x_cfg_ver&0x3ffffff;
        g_pfw_infos->chsc6x_cfg_version = 0;
        //g_pfw_infos->chsc6x_vendor_id = (g_chsc6x_cfg_ver>>9)&0x7F;
        //g_pfw_infos->chsc6x_project_id = g_chsc6x_cfg_ver&0x01FF;
        g_pfw_infos->chsc6x_vendor_id = ((g_chsc6x_cfg_ver>>9)&0x7F | ((g_chsc6x_cfg_ver>>22&0x03)<<7));
        g_pfw_infos->chsc6x_project_id = ((g_chsc6x_cfg_ver&0x01FF) | ((g_chsc6x_cfg_ver>>20&0x03)<<9));
    }
    
    return 0;
}

static int chsc6x_cfg_update(unsigned short *parray, unsigned int cfg_num)
{
    unsigned int  k;
    int new_idx_active = -1;

    chsc6x_info("chsc6x: g_chsc6x_cfg_ver is 0x%x \r\n",g_chsc6x_cfg_ver);
    
    if (g_chsc6x_cfg_ver == 0) {    /* no available version information */
        chsc6x_err("chsc6x: no current version information!\r\n");
        return -EPERM;
    }

    for (k = 0; k < cfg_num; k++) {
        if (is_tpcfg_update_allow(parray) == 1) {
            new_idx_active = k;
            chsc6x_info("chsc6x: new_idx_active is %d.\r\n", new_idx_active);
            break;
        }
        parray = parray + 102;
    }

    if (new_idx_active < 0) {
        chsc6x_info("chsc6x: 3536 cfg not need update!\r\n");
        return -EPERM;
    }

    if (chsc6x_set_dd_mode()) {
        chsc6x_err("chsc6x: cfg update error:can't control hw mode!\r\n");
        return -EPERM;
    }

    if (chsc6x_update_fcomp_cfg(parray) == 0) {
        chsc6x_info("chsc6x: fcomp_cfg update pass!\r\n");
    } else {
        chsc6x_err("chsc6x: fcomp_cfg update fail!\r\n");
    }

    return 0;        /* need hw reset */
}

static int chsc6x_boot_ver_comp(unsigned int ver)
{
    if (g_chsc6x_boot_ver == 0) {
        chsc6x_info("chsc6x: try to force update boot ! \r\n", g_chsc6x_boot_ver, ver);
        return 1; //try to force update
    }

    if (0 == g_upgrade_flag && ver > g_chsc6x_boot_ver ) {
        chsc6x_info("chsc6x: boot_vnow: 0x%x,boot_vbuild: 0x%x \r\n", g_chsc6x_boot_ver, ver);
        return 1; //driver init upgrade, must vbuild > vnow
    }        

	if (1 == g_upgrade_flag && ver != g_chsc6x_boot_ver ) {
        chsc6x_info("chsc6x: boot_vnow: 0x%x,boot_vbuild: 0x%x \r\n", g_chsc6x_boot_ver, ver);
        return 1; //OTA upgrade just vbuild != vnow */
	}	

    chsc6x_info("chsc6x: boot_vnow: 0x%x,boot_vbuild: 0x%x \r\n", g_chsc6x_boot_ver, ver);
    return 0; //no-need update boot
}

/*return: 0,no-need update | update succeed; 1,update failed*/
static int chsc6x_boot_update(unsigned char *pdata, unsigned short boot_len)
{
    unsigned int ver = 0;

    ver = pdata[5];
    ver = (ver<<8) + pdata[4];

    if (chsc6x_boot_ver_comp(ver) == 0) {
        chsc6x_info("chsc6x: 3536 boot not need update!\r\n");
        return 0;
    }
    
    return  chsc6x_update_fcomp_boot(pdata, boot_len);
}

/*analysis the ".h fw" and check burn*/
/*return: 0 SUCESS else FAIL*/
static int chsc6x_update_compat_ctl(unsigned char *pupd, int len)
{
    unsigned int k;
    unsigned int n;
    unsigned int offset;
    unsigned int *vlist;

    int ret = -1;

    struct chsc6x_updfile_header *upd_header;

    if (len < sizeof(struct chsc6x_updfile_header)){
        chsc6x_err("chsc6x: update file len error_1 ! \r\n");
        return -EPERM;
    }

    upd_header = (struct chsc6x_updfile_header *) pupd;

    if (upd_header->sig != 0x43534843) {
        chsc6x_err("chsc6x: update file sign error ! \r\n");
        return -EPERM;
    }

    n = upd_header->n_cfg;
    offset = (upd_header->n_match * 4) + sizeof(struct chsc6x_updfile_header);

    if ((offset + upd_header->len_cfg + upd_header->len_boot) != len) {
        chsc6x_err("chsc6x: update file len error_2 ! \r\n");
        return -EPERM;
    }
    
    if ((n * 204) != upd_header->len_cfg) {
        chsc6x_err("chsc6x: update file len error_3 ! \r\n");
        return -EPERM;
    }

    if (n != 0) {
        chsc6x_cfg_update((unsigned short *) (pupd + offset), n);
    }

    n = upd_header->n_match;
    if (n != 0) {
        vlist = (unsigned int *) (pupd + sizeof(struct chsc6x_updfile_header));
        offset = offset + upd_header->len_cfg;
        for (k=0; k < n; k++) {
            if (vlist[k] == (g_chsc6x_cfg_ver & 0xffffff)) {
                ret = chsc6x_boot_update((pupd + offset), upd_header->len_boot);
                if(0 == ret) {
                    break;
                }
                else {
                    return -EPERM;
                }
            }
        }
    }
    return 0;
}

/*return: 0 SUCESS else FAIL*/
static int chsc6x_do_update_ifneed(unsigned char* p_fw_upd, unsigned int fw_len)
{
    const unsigned char *fupd; 
    unsigned int fw_size;
    int ret = -1;
    
  #if CHSC6X_MUL_VENDOR_UPGRADE
    if(41 == g_pfw_infos->chsc6x_vendor_id && 27 == g_pfw_infos->chsc6x_project_id) {
        fupd = chsc_boot_41_27;
        fw_size = sizeof(chsc_boot);
        ret = chsc6x_update_compat_ctl((unsigned char *) fupd, fw_size);
    }
    else if(29 == g_pfw_infos->chsc6x_vendor_id && 08 == g_pfw_infos->chsc6x_project_id) {
        fupd = chsc_boot_29_08;
        fw_size = sizeof(chsc_boot_29_08);
        ret = chsc6x_update_compat_ctl((unsigned char *) fupd, fw_size);
    }
  #else
  
    if(1 == g_upgrade_flag) { //ota upgrade
        fupd = p_fw_upd;
        fw_size = fw_len;
    }else{
        fupd = chsc_boot;
        fw_size = sizeof(chsc_boot);
    }
	
    ret = chsc6x_update_compat_ctl((unsigned char *) fupd, fw_size);
  #endif

    return ret;
}

static void chsc6x_tp_mccode(void)
{
    unsigned int tmp[3];

    g_mccode = 0xff;

    if (chsc6x_read_bytes_u16addr(g_i2c_addr, 0x8000, (unsigned char *) tmp, 12)) {
        chsc6x_err("chsc6x: read 0x8000-12 fail! \r\n");
        return;
    }
    if ( 0x544c4e4b == tmp[2] ) {    /*  boot code  */
        if (tmp[0] == 0x35368008) {
            g_mccode = 1;
            g_chsc6x_boot_ver = tmp[1] & 0xffff;
            g_pfw_infos->chsc6x_boot_version = g_chsc6x_boot_ver;
            chsc6x_info("chsc6x: read 0x8000-12 OK! \r\n");
        }
    } else if ( 0x544c4e00 == (tmp[2]&0xffffff00) ) {
        if (tmp[0] == 0x35368008) {
            g_mccode = 1;
            g_chsc6x_boot_ver = 0;
            g_pfw_infos->chsc6x_boot_version = g_chsc6x_boot_ver;
            chsc6x_err("chsc6x: IC run flag error! \r\n");
        }
    }else {    /* none code */
        tmp[0] = 0;
        if (chsc6x_read_bytes_u16addr(g_i2c_addr, 0x09, (unsigned char *) tmp, 3)) {
            chsc6x_err("chsc6x: read 0x09-3 fail! \r\n");
            return;
        }
        if (tmp[0] == 0x5c5c5c){ //empty ic
            g_mccode = 1;
            chsc6x_info("chsc6x: empty ic! \r\n");
        }
    }
}

/* FUNC: get fw info in struct ts_fw_infos you can call this func anytime.
/* PARM iic_addr: your iic addr.
/* PARM pfw_infos: can get all fw infos in struct ts_fw_infos, after call this interface.
*/
void chsc6x_get_chip_info(struct ts_fw_infos *infos)
{
    int ret, cnt = 0;
    unsigned char data_t[4] = {0};
    g_i2c_addr = CHSC6X_I2C_ID;
    
    for(cnt=0; cnt<3; cnt++) {
        chsc6x_tp_reset();
        ret = chsc6x_set_dd_mode();
        if(ret != 0) {
            chsc6x_err("chsc6x: Change to dd mode failed! \r\n");       
        }
        
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e00, data_t, 4);
        if ((ret != 0) || (data_t[0] == 0x0) || (data_t[2] == 0x0)) {
            chsc6x_err("chsc6x: Get chip_info failed::0x%02x%02x 0x%02x%02x \r\n", data_t[0], data_t[1], data_t[2], data_t[3]);
            continue;
        } else {
            infos->chsc6x_cfg_version = data_t[3]>>2;
            //infos->chsc6x_vendor_id = data_t[1]>>1;
            //infos->chsc6x_project_id = ((data_t[1]&1)<<8) | data_t[0];
            infos->chsc6x_vendor_id = (data_t[1]>>1) | ((data_t[2]&0xc0)<<1);
            infos->chsc6x_project_id = (((data_t[1]&1)<<8) | data_t[0]) | ((data_t[2]&0x30)<<5);
            chsc6x_info("chsc6x: TP chip_info OK::fw_ver:%d, vendor_id:%d, prj_id:%d \r\n", infos->chsc6x_cfg_version, infos->chsc6x_vendor_id, infos->chsc6x_project_id);
        }
        
        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x8004, data_t, 2);
        if(0 == ret) {
            infos->chsc6x_boot_version = (data_t[1]<<8)|data_t[0];
            chsc6x_info("chsc6x: boot_ver OK::0x%02x  \r\n", infos->chsc6x_boot_version);
        } else {
            chsc6x_err("chsc6x: Get boot_ver failed! \r\n"); 
            continue;
        }
        
        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e4c, data_t, 4);
        if(0 == ret) {
            infos->chsc6x_rpt_lcd_x = (data_t[1]<<8) + data_t[0];
            infos->chsc6x_rpt_lcd_y = (data_t[3]<<8) + data_t[2];
            chsc6x_info("chsc6x: rpt_lcd_x & rpt_lcd_y OK::%d:%d \r\n", infos->chsc6x_rpt_lcd_x, infos->chsc6x_rpt_lcd_y);
        } else {
            chsc6x_err("chsc6x: Get rpt_lcd_x & rpt_lcd_y failed! \r\n"); 
            continue;
        }
        
        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e6a, data_t, 2);
        if(0 == ret) {
            infos->chsc6x_chip_id = data_t[0];
            infos->chsc6x_chip_type = data_t[1]&0xf;
            chsc6x_info("chsc6x: chsc6x_chip_id & chsc6x_chip_type OK::0x%02x:0x%x \r\n", infos->chsc6x_chip_id, infos->chsc6x_chip_type);
        } else {
            chsc6x_err("chsc6x: Get chsc6x_chip_id & chsc6x_chip_type failed! \r\n"); 
            continue;
        }

        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9ff8, data_t, 1);
        if(0 == ret) {
            infos->chsc6x_max_pt_num = data_t[0];
            chsc6x_info("chsc6x: chsc6x_max_pt_num OK::%d \r\n", infos->chsc6x_max_pt_num);
        } else {
            chsc6x_err("chsc6x: Get chsc6x_max_pt_num failed! \r\n"); 
            continue;
        }
        
        break;        
    }

    chsc6x_tp_reset();

    if(cnt >= 3) {
        chsc6x_err("chsc6x: Get chip_info failed! \r\n");    
    }
}


/* FUNC In your systerm init process,Must call this interface function to detec if the TP IC is Chipsemi corp'. 
/* PARM pfw_infos: to get top 5 fw info in struct ts_fw_infos.
/* PARM update_ret_flag: point value=1 update succeed; point value=0 update failed, If opend CHSC6X_AUTO_UPGRADE macro.
/* RETURN 1:is chsc chip, 0:is not chsc chip
*/
int chsc6x_tp_dect(struct ts_fw_infos *pfw_infos, unsigned char *update_ret_flag)
{
    int ret = -1;
    int try_cnt;
    unsigned char dwr=0x05;
    unsigned short buf_tmpcfg[102];    
    
    g_mccode = 0xff;        /* default */
    g_i2c_addr = CHSC6X_I2C_ID;
    g_pfw_infos = pfw_infos;

    for(try_cnt=0; try_cnt<5; try_cnt++) {
        if (chsc6x_set_dd_mode()) {
            chsc6x_tp_reset_active();
            if (chsc6x_bulk_down_check(&dwr, 0x0602, 1) == 0) {    
                dwr = 0x00;
                chsc6x_bulk_down_check(&dwr, 0x0643, 1);    
            } else {
                chsc6x_info("chsc6x: write 0x0602 failed \r\n");
            }
        } else {
            break;
        }
    }
    if(try_cnt >= 5) {
        chsc6x_info("chsc6x: chsc6x_set_dd_mode failed \r\n");
    }
    //chsc6x_info("chsc6x: chsc_boot size=%d \r\n", sizeof(chsc_boot));

    chsc6x_tp_mccode();    /* MUST: call this function there!!! */
    chsc6x_info("chsc6x: g_mccode is 0x%x \r\n",g_mccode);    

    if(g_mccode == 0xff) {
        chsc6x_err("chsc6x: get mccode fail! \r\n");
        return 0;
    }

    /*try to get running time tp-cfg. if fail : wrong boot? wrong rom-cfg?*/
    if (chsc6x_get_running_cfg(buf_tmpcfg, 0x9e00) == 0) {
        chsc6x_info("chsc6x: get_running_cfg pass !\r\n");
        g_chsc6x_cfg_ver = (unsigned int)buf_tmpcfg[1];
        g_chsc6x_cfg_ver = (g_chsc6x_cfg_ver<<16) + (unsigned int)buf_tmpcfg[0];

        g_pfw_infos->chsc6x_cfg_version = g_chsc6x_cfg_ver>>26;
        //g_pfw_infos->chsc6x_vendor_id = (g_chsc6x_cfg_ver>>9)&0x7F;
        //g_pfw_infos->chsc6x_project_id = g_chsc6x_cfg_ver&0x01FF;
        g_pfw_infos->chsc6x_vendor_id = ((g_chsc6x_cfg_ver>>9)&0x7F | ((g_chsc6x_cfg_ver>>22&0x03)<<7));
        g_pfw_infos->chsc6x_project_id = ((g_chsc6x_cfg_ver&0x01FF) | ((g_chsc6x_cfg_ver>>20&0x03)<<9));
        g_pfw_infos->chsc6x_rpt_lcd_x = buf_tmpcfg[38];
        g_pfw_infos->chsc6x_rpt_lcd_y = buf_tmpcfg[39];
        g_pfw_infos->chsc6x_chip_id = buf_tmpcfg[53]&0xff;
        g_pfw_infos->chsc6x_chip_type = (buf_tmpcfg[53]>>8)&0xf;
        chsc6x_info("%s:chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", __func__, \
            g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id,g_pfw_infos->chsc6x_boot_version, \
            g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
        );
    } else {
        if(0 == buf_tmpcfg[2] && 0 == buf_tmpcfg[3]) {
            g_chsc6x_boot_ver = 0;
         }
         chsc6x_find_ver();
    }

    if (0 == g_chsc6x_cfg_ver) {
        chsc6x_err("chsc6x: get tp-info fail! \r\n");
        return 0;
    }
  #if CHSC6X_AUTO_UPGRADE
    g_upgrade_flag = 0;
    ret = chsc6x_do_update_ifneed(0, 0);
    if(0 != ret) {
        *update_ret_flag = 0;
        chsc6x_err("chsc6x: do fw update failed !\r\n");
    } else {
        *update_ret_flag = 1;
        chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
            g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id, \
            g_pfw_infos->chsc6x_boot_version,g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
        );
    }
  #endif  
    
exit:
    chsc6x_tp_reset();

    //chsc6x_tp_reset();

    return 1;
}

/* FUNC You can call this interfacce function to realize upgrade TP Firmware by OTA. 
/* PARM pfw_infos: to get top 6 fw infos in struct ts_fw_infos, after ota upgrade.
/* PARM p_fw_upd: array address of the upgrade firmware array 
/* PARM fw_len: total size of the upgrade firmware array 
/* RETURN NULL
*/
void chsc6x_ota_upgrade_tp_fw(struct ts_fw_infos *pfw_infos, unsigned char* p_fw_upd, unsigned int fw_len)
{
    int ret;
    int try_cnt;
    unsigned char dwr=0x05;
    
    g_mccode = 0xff;		/* default */
    g_i2c_addr = CHSC6X_I2C_ID;
    g_pfw_infos = pfw_infos;
    
    unsigned short buf_tmpcfg[102];    

    for(try_cnt=0; try_cnt<5; try_cnt++) {
        if (chsc6x_set_dd_mode()) {
            chsc6x_tp_reset_active();
            if (chsc6x_bulk_down_check(&dwr, 0x0602, 1) == 0) {    
                dwr = 0x00;
                chsc6x_bulk_down_check(&dwr, 0x0643, 1);    
            } else {
                chsc6x_info("chsc6x: write 0x0602 failed \r\n");
            }
        } else {
            break;
        }
    }
    if(try_cnt >= 5) {
        chsc6x_info("chsc6x: chsc6x_set_dd_mode failed \r\n");
    }

    chsc6x_tp_mccode();	/* MUST: call this function there!!! */
    chsc6x_info("chsc6x: g_mccode is 0x%x\n",g_mccode);	

    if (g_mccode == 0xff) {
    	chsc6x_err("chsc6x: get mccode fail\n");
    	goto exit;
    }

    if (chsc6x_get_running_cfg(buf_tmpcfg, 0x9e00) == 0) {
        chsc6x_info("chsc6x: get_running_cfg pass !\r\n");
        g_chsc6x_cfg_ver = (unsigned int)buf_tmpcfg[1];
        g_chsc6x_cfg_ver = (g_chsc6x_cfg_ver<<16) + (unsigned int)buf_tmpcfg[0];

        g_pfw_infos->chsc6x_cfg_version = g_chsc6x_cfg_ver>>26;
        //g_pfw_infos->chsc6x_vendor_id = (g_chsc6x_cfg_ver>>9)&0x7F;
        //g_pfw_infos->chsc6x_project_id = g_chsc6x_cfg_ver&0x01FF;
        g_pfw_infos->chsc6x_vendor_id = ((g_chsc6x_cfg_ver>>9)&0x7F | ((g_chsc6x_cfg_ver>>22&0x03)<<7));
        g_pfw_infos->chsc6x_project_id = ((g_chsc6x_cfg_ver&0x01FF) | ((g_chsc6x_cfg_ver>>20&0x03)<<9));
        g_pfw_infos->chsc6x_chip_id = buf_tmpcfg[53]&0xff;
        g_pfw_infos->chsc6x_chip_type = (buf_tmpcfg[53]>>8)&0xf;
        chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
            g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id, \
            g_pfw_infos->chsc6x_boot_version,g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
        );
    } else {
    	chsc6x_find_ver();
    }

    if (g_chsc6x_cfg_ver == 0) {
    	chsc6x_err("chsc6x: get cfg-ver fail\n");
    	goto exit;
    }

    g_upgrade_flag = 1;
    ret = chsc6x_do_update_ifneed(p_fw_upd, fw_len);
    if(0 != ret) {
        chsc6x_err("chsc6x: do fw update failed !\r\n");
    } else {
       chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
           g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id, \
           g_pfw_infos->chsc6x_boot_version,g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
       );
    }
    exit:
    chsc6x_tp_reset();

}
#endif

#endif

