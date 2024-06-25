#include "sensor_iic.h"

#if TCFG_SENSOR_IIC_TYPE
#define iic_init(iic)                       hw_iic_init(iic)
#define iic_uninit(iic)                     hw_iic_uninit(iic)
#define iic_start(iic)                      hw_iic_start(iic)
#define iic_stop(iic)                       hw_iic_stop(iic)
#define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    hw_iic_suspend(iic)
#define iic_resume(iic)                     hw_iic_resume(iic)
#else
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    soft_iic_suspend(iic)
#define iic_resume(iic)                     soft_iic_resume(iic)
#endif

static spinlock_t sensor_iic_lock;
static const u32 sensor_iic_delay = 1;
static bool sensor_iic_init_flag = false;
static const soft_iic_dev sensor_iic_hdl = \
    TCFG_SENOR_IIC_INDEX;

u32 Get_sensor_iic_delay(void)
{
    return sensor_iic_delay;
}

soft_iic_dev Get_sensor_iic_hdl(void)
{
    return sensor_iic_hdl;
}

void sensor_iic_init(void)
{
    if(sensor_iic_init_flag)
        return;

    iic_init(sensor_iic_hdl);

    spin_lock_init(&sensor_iic_lock);

    sensor_iic_init_flag = true;

    printf("******%s\n", __func__);

    return;
}

bool sensor_iic_u8addr_tx_byte(u8 slave_addr, u8 reg_addr, u8 data)
{
    u8 op_ret;
    bool op_succ = false;

    spin_lock(&sensor_iic_lock);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, slave_addr << 1);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, reg_addr);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, data);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    op_succ = true;

__end:
    iic_stop(sensor_iic_hdl);
    spin_unlock(&sensor_iic_lock);
    return op_succ;
}

bool sensor_iic_u8addr_tx_buf(u8 slave_addr, u8 reg_addr, u8 *buf, u8 len)
{
    u8 op_ret;
    bool op_succ = false;

    if(buf == NULL || len == 0)
        return op_succ;

    spin_lock(&sensor_iic_lock);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, slave_addr << 1);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, reg_addr);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    for(u8 i = 0; i < len; i++)
    {
        op_ret = \
            iic_tx_byte(sensor_iic_hdl, buf[i]);
        if(op_ret == 0)
            goto __end;
        delay(sensor_iic_delay);
    }

    op_succ = true;

__end:
    iic_stop(sensor_iic_hdl);
    spin_unlock(&sensor_iic_lock);
    return op_succ;
}

bool sensor_iic_u8addr_rx_buf(u8 slave_addr, u8 reg_addr, u8 *buf, u8 len)
{
    u8 op_ret;
    bool op_succ = false;

    if(buf == NULL || len == 0)
        return op_succ;

    spin_lock(&sensor_iic_lock);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, slave_addr << 1);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, reg_addr);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, (slave_addr << 1) | (0x01));
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    u8 r_len = 0;
    for(; len > 1; len--)
        buf[r_len++] = iic_rx_byte(sensor_iic_hdl, 1);

    buf[r_len++] = iic_rx_byte(sensor_iic_hdl, 0);

    op_succ = true;

__end:
    iic_stop(sensor_iic_hdl);
    spin_unlock(&sensor_iic_lock);
    return op_succ;
}

bool sensor_iic_u16addr_tx_byte(u8 slave_addr, u16 reg_addr, u8 data)
{
    u8 op_ret;
    bool op_succ = false;

    spin_lock(&sensor_iic_lock);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, slave_addr << 1);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    //先发高8，再发低8
    u8 addr_buf[2];
    addr_buf[0] = \
        (u8)((reg_addr >> 8)&(0xff));
    addr_buf[1] = \
        (u8)((reg_addr)&(0xff));
    u8 reg_w_len = \
        iic_write_buf(sensor_iic_hdl, addr_buf, 2);
    if(reg_w_len != 2)
        goto __end;
    delay(sensor_iic_delay);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, data);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    op_succ = true;

__end:
    iic_stop(sensor_iic_hdl);
    spin_unlock(&sensor_iic_lock);
    return op_succ;
}

bool sensor_iic_u16addr_tx_buf(u8 slave_addr, u16 reg_addr, u8 *buf, u8 len)
{
    u8 op_ret;
    bool op_succ = false;

    if(buf == NULL || len == 0)
        return op_succ;
    
    spin_lock(&sensor_iic_lock);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, slave_addr << 1);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    //先发高8，再发低8
    u8 addr_buf[2];
    addr_buf[0] = \
        (u8)((reg_addr >> 8)&(0xff));
    addr_buf[1] = \
        (u8)((reg_addr)&(0xff));
    u8 reg_w_len = \
        iic_write_buf(sensor_iic_hdl, addr_buf, 2);
    if(reg_w_len != 2)
        goto __end;
    delay(sensor_iic_delay);

    for(u8 i = 0; i < len; i++)
    {
        op_ret = \
            iic_tx_byte(sensor_iic_hdl, buf[i]);
        if(op_ret == 0)
            goto __end;
        delay(sensor_iic_delay);
    }

    op_succ = true;

__end:
    iic_stop(sensor_iic_hdl);
    spin_unlock(&sensor_iic_lock);
    return op_succ;
}

bool sensor_iic_u16addr_rx_buf(u8 slave_addr, u16 reg_addr, u8 *buf, u8 len)
{
    u8 op_ret;
    bool op_succ = false;

    if(buf == NULL || len == 0)
        return op_succ;

    spin_lock(&sensor_iic_lock);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, slave_addr << 1);
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    //先发高8，再发低8
    u8 addr_buf[2];
    addr_buf[0] = \
        (u8)((reg_addr >> 8)&(0xff));
    addr_buf[1] = \
        (u8)((reg_addr)&(0xff));
    u8 reg_w_len = \
        iic_write_buf(sensor_iic_hdl, addr_buf, 2);
    if(reg_w_len != 2)
        goto __end;
    delay(sensor_iic_delay);

    iic_start(sensor_iic_hdl);

    op_ret = \
        iic_tx_byte(sensor_iic_hdl, (slave_addr << 1) | (0x01));
    if(op_ret == 0)
        goto __end;
    delay(sensor_iic_delay);

    u8 r_len = 0;
    for(; len > 1; len--)
        buf[r_len++] = iic_rx_byte(sensor_iic_hdl, 1);

    buf[r_len++] = iic_rx_byte(sensor_iic_hdl, 0);

    op_succ = true;

__end:
    iic_stop(sensor_iic_hdl);
    spin_unlock(&sensor_iic_lock);
    return op_succ;
}
