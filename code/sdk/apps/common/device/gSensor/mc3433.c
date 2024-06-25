#include "gSensor/mc3433.h"
#include "gSensor/gSensor_manage.h"

#if TCFG_MC3433_EN

u8 mc3433_register_read(u8 addr, u8 *data)
{
    _gravity_sensor_get_ndata(I2C_ADDR_MC3433_R, addr, data, 1);
    return 0;
}

u8 mc3433_register_write(u8 addr, u8 data)
{
    gravity_sensor_command(I2C_ADDR_MC3433_W, addr, data);
    return 0;
}

u8 mc3433_read_nbyte_data(u8 addr, u8 *data, u8 len)
{
    return _gravity_sensor_get_ndata(I2C_ADDR_MC3433_R, addr, data, len);
}

u8 mc3433_get_xyz_data(u8 addr, void *xyz_data)
{
    u8 buf[3], read_len;
    axis_info_t *data = (axis_info_t *)xyz_data;
    read_len = mc3433_read_nbyte_data(addr, buf, 3);
    if (read_len == 3) {
        data->x = (s8)buf[0];
        data->y = (s8)buf[1];
        data->z = (s8)buf[2];

        data->x = 2 * ACCEL_OF_GRAVITY * ACCEL_DATA_GAIN * data->x / 128;
        data->y = 2 * ACCEL_OF_GRAVITY * ACCEL_DATA_GAIN * data->y / 128;
        data->z = 2 * ACCEL_OF_GRAVITY * ACCEL_DATA_GAIN * data->z / 128;
    }
    return read_len;
}

extern void step_cal_init();
extern int step_cal();
u8 mc3433_init()
{
    u8 res = 0;
    u8 data = 0;
    res = mc3433_register_read(MC3433_PRODUCT_CODE_REG, &data);       //读ID
    if (data >= MC3433_ADDR_MIN && data <= MC3433_ADDR_MAX) {
        g_printf("read MC3433 ID suss");
    } else {
        g_printf("read MC3433 ID err");
        return -1;
    }

    data = 0x00;
    data = SR_32HZ;
    mc3433_register_write(MC3433_SR_REG, data);                       //设置采样率

    data = 0x00;
    data = RESOLUTION_8BIT;
    mc3433_register_write(MC3433_OUTPUT_CFG_REG, data);               //设置分辨率

    data = 0x00;
    data |= BIT(0);
    mc3433_register_write(MC3433_MODE_REG, data);                     //设置唤醒模式

    step_cal_init();
    sys_timer_add(NULL, step_cal, 50);
    return 0;
}

int mc3433_ctl(u8 cmd, void *arg)
{
    int res = -1;
    switch (cmd) {
    case GET_ACCEL_DATA:
        res = mc3433_get_xyz_data(MC3433_XOUT_ACCEL_REG, arg);
        break;
    }
    return res;//>=0:ok,,<0:err
}

REGISTER_GRAVITY_SENSOR(gSensor) = {
    .logo = "mc3433",
    .gravity_sensor_init  = mc3433_init,
    .gravity_sensor_check = NULL,
    .gravity_sensor_ctl   = mc3433_ctl,
};

#endif

