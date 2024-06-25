#include "asm/gpio.h"
#include "asm/iic_soft.h"
#include "chsc6x_platform.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_CHSC6X_ENABLE

#if TCFG_TOUCH_USER_IIC_TYPE
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

#define CHSC6X_RESET_H()     gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_set_hd(TCFG_TP_RESET_IO, 0);gpio_set_hd0(TCFG_TP_RESET_IO, 1);gpio_direction_output(TCFG_TP_RESET_IO,1)
#define CHSC6X_RESET_L()     gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_set_hd(TCFG_TP_RESET_IO, 0);gpio_set_hd0(TCFG_TP_RESET_IO, 1);gpio_direction_output(TCFG_TP_RESET_IO,0)


static const u8 chsc6x_iic_delay = 1;

void chsc6x_iic_init(u8 iic)
{
    iic_init(iic);

    return;
}

static void chsc6x_udelay(u32 us)
{
    delay(us);

    return;
}

/* return: =read lenth succeed; <0 failed 
   read reg addr not need 
   just used for reading xy cord info*/
int chsc6x_i2c_read(unsigned char id, unsigned char *p_data, unsigned short lenth)
{    
    int ret = -1;
    unsigned char addr_8_bit_r = id + 0x01;
    
    iic_start(tp_iic_hdl());

    if(!iic_tx_byte(tp_iic_hdl(), id)) 
    {
        iic_stop(tp_iic_hdl());
        return -1;
    }
    chsc6x_udelay(chsc6x_iic_delay);

    if(!iic_tx_byte(tp_iic_hdl(), 0x00)) 
    {
        iic_stop(tp_iic_hdl());
        return -1;
    }
    chsc6x_udelay(chsc6x_iic_delay);

    iic_start(tp_iic_hdl());

    if(!iic_tx_byte(tp_iic_hdl(), addr_8_bit_r)) 
    {
        iic_stop(tp_iic_hdl());
        return -1;
    }
    chsc6x_udelay(chsc6x_iic_delay);

    ret = iic_read_buf(tp_iic_hdl(), p_data, lenth);
    chsc6x_udelay(chsc6x_iic_delay);
    iic_stop(tp_iic_hdl());
    chsc6x_udelay(chsc6x_iic_delay);
    
    if(ret == lenth)
        return ret;

    return -1;
}

/* RETURN:0->pass else->fail */
int chsc6x_read_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
    //printf("******%s\n", __func__);

    int ret = -1;
    unsigned char reg[2] = {0};
    unsigned char addr_7_bit = id >> 1;

    reg[0] = (adr>>8) & (0xff); 
    reg[1] = adr & (0xff);
    //printf("%s:0x%x, 0x%x\n", __func__, reg[0], reg[1]);

    iic_start(tp_iic_hdl());

    if(!iic_tx_byte(tp_iic_hdl(), addr_7_bit << 1)) 
    {
        iic_stop(tp_iic_hdl());
        return -1;
    }
    chsc6x_udelay(chsc6x_iic_delay);

    int w_len = 0;
    w_len = iic_write_buf(tp_iic_hdl(), reg, 2);
    if(w_len != 2)
    {
        iic_stop(tp_iic_hdl());
        return -1; 
    }
    chsc6x_udelay(chsc6x_iic_delay);

    iic_start(tp_iic_hdl());
    if(!iic_tx_byte(tp_iic_hdl(), (addr_7_bit << 1) | 0x01)) 
    {
        iic_stop(tp_iic_hdl());
        return -1;
    }
    chsc6x_udelay(chsc6x_iic_delay);

    ret = iic_read_buf(tp_iic_hdl(), rxbuf, lenth);
    chsc6x_udelay(chsc6x_iic_delay);
    iic_stop(tp_iic_hdl());
    chsc6x_udelay(chsc6x_iic_delay);
    
    if(ret == lenth)
        return 0;

    return -1;
}

/* RETURN:0->pass else->fail */
int chsc6x_write_bytes_u16addr_sub(unsigned char id, unsigned short adr, unsigned char *txbuf, unsigned short lenth)
{

    int ret = -1;
    unsigned char reg[2] = {0};
    unsigned char addr_7_bit = id >> 1;//.0x2e

    reg[0] = (adr>>8) & (0xff);  
    reg[1] = adr & (0xff);

    iic_start(tp_iic_hdl());

    if(!iic_tx_byte(tp_iic_hdl(), addr_7_bit << 1)) 
    {
        iic_stop(tp_iic_hdl());
        return -1;
    }
    chsc6x_udelay(chsc6x_iic_delay);

    int write_len = 0;
    write_len = iic_write_buf(tp_iic_hdl(), reg, 2);
    if(write_len != 2)
    {
        iic_stop(tp_iic_hdl());
        return -1; 
    }
    chsc6x_udelay(chsc6x_iic_delay);

    ret = iic_write_buf(tp_iic_hdl(), txbuf, lenth);
    chsc6x_udelay(chsc6x_iic_delay);
    iic_stop(tp_iic_hdl());
    chsc6x_udelay(chsc6x_iic_delay);

    if(ret == lenth)
        return 0;

    return -1;
}

void chsc6x_msleep(int ms)
{
    mdelay(ms);

    return;
}

void chsc6x_tp_reset(void)
{
    CHSC6X_RESET_L();
    mdelay(30);//30ms
    CHSC6X_RESET_H();
    mdelay(30);//30ms

    return;
}

void chsc6x_tp_reset_active(void)
{
    CHSC6X_RESET_L();
    mdelay(30);//30ms
    CHSC6X_RESET_H();

    return;
}
#endif

#endif
