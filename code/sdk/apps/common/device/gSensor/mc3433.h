#ifndef __MC3433_H_
#define __MC3433_H_

#include "app_config.h"

#if TCFG_MC3433_EN

#define MC3433_XOUT_ACCEL_REG       0x00
#define MC3433_YOUT_ACCEL_REG       0x01
#define MC3433_ZOUT_ACCEL_REG       0x02
#define MC3433_STATUS_REG           0x03
#define MC3433_OP_DEV_ST_REG        0x04
#define MC3433_INTEN_REG            0x06
#define MC3433_MODE_REG             0x07
#define MC3433_SR_REG               0x08
#define MC3433_OUTPUT_CFG_REG       0x20
#define MC3433_XOFFSET_LSB_REG      0x21
#define MC3433_XOFFSET_MSB_REG      0x22
#define MC3433_YOFFSET_LSB_REG      0x23
#define MC3433_YOFFSET_MSB_REG      0x24
#define MC3433_ZOFFSET_LSB_REG      0x25
#define MC3433_ZOFFSET_MSB_REG      0x26
#define MC3433_XGAIN_REG            0x27
#define MC3433_YGAIN_REG            0x28
#define MC3433_ZGAIN_REG            0x29
#define MC3433_PRODUCT_CODE_REG     0x3B

#define MC3433_ADDR_MIN             0x60
#define MC3433_ADDR_MAX             0x6E

#define MC3433_VPP_LOW

#ifdef MC3433_VPP_LOW
#define I2C_ADDR_MC3433_W           0x98
#define I2C_ADDR_MC3433_R           0x99
#else
#define I2C_ADDR_MC3433_W           0xD8
#define I2C_ADDR_MC3433_R           0xD9
#endif

enum {
    SR_16HZ = 0x00,
    SR_8HZ,
    SR_4HZ,
    SR_2HZ,
    SR_1HZ,
    SR_05HZ,
    SR_025HZ,
    SR_0125HZ,
    SR_32HZ,
    SR_64HZ,
    SR_128HZ,
};

enum {
    RESOLUTION_6BIT,
    RESOLUTION_7BIT,
    RESOLUTION_8BIT,
};
#endif

#endif
