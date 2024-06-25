#ifndef __CHSC6X_PLATFORM_H__
#define __CHSC6X_PLATFORM_H__

#include "printf.h"
#include "app_config.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_CHSC6X_ENABLE

#if 0
#define chsc6x_info(x...) pr_notice("[chsc6x] " x)
#define chsc6x_err(x...) pr_err("[chsc6x][error] " x)
#else
#define chsc6x_info       printf
#define chsc6x_err        printf 
#endif

#define CHSC6X_I2C_ID      (0x5c) //8bit 

#define CHSC6X_MAX_POINTS_NUM     (1)
#define CHSC6X_RES_MAX_X	(368)
#define CHSC6X_RES_MAX_Y	(448)

/*MACRO SWITCH for driver update TP FW */
#define CHSC6X_AUTO_UPGRADE           (1)

/*MACRO SWITCH for multi TP_VENDOR Compatible update TP FW */
#define CHSC6X_MUL_VENDOR_UPGRADE     (0)

#define MAX_IIC_WR_LEN          (8)
#define MAX_IIC_RD_LEN          (16)


/* fail : <0 */
int chsc6x_i2c_read(unsigned char id, unsigned char *p_data, unsigned short lenth);

/* RETURN:0->pass else->fail */
int chsc6x_read_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth);

/* RETURN:0->pass else->fail */
int chsc6x_write_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *txbuf, unsigned short lenth);

void chsc6x_msleep(int ms);

void chsc6x_tp_reset(void);

void chsc6x_tp_reset_active(void);

u8 tp_iic_hdl(void);
void chsc6x_iic_init(u8 iic);

#endif

#endif

#endif
