#include "app_config.h"
#include "system/includes.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "asm/gpio.h"
#include "asm/mcpwm.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "debug.h"
#include "spt511x_firmware_customer.h"
#include "spt511x_firmware.h"
#include "debug.h"
#include "string.h"
#include "typedef.h"

#define VK_X 240
#define VK_Y 284
#if 0
#define log_info   printf
#else
#define log_info
#endif

#if TCFG_TOUCH_PANEL_ENABLE
#if TCFG_TP_SPT511X_ENABLE

#ifndef abs
#define abs(x)  ((x)>0?(x):-(x) )
#endif

#define TP_RESET_IO			TCFG_TP_RESET_IO
#define SPT511X_RESET_H() gpio_direction_output(TCFG_TP_RESET_IO,1)
#define SPT511X_RESET_L() gpio_direction_output(TCFG_TP_RESET_IO,0)
#define SPT511X_INT_IO    (TCFG_TP_INT_IO)
#define SPT511X_INT_IN()  gpio_direction_input(SPT511X_INT_IO);gpio_set_pull_down(SPT511X_INT_IO,0);gpio_set_pull_up(SPT511X_INT_IO,1);
#define SPT511X_INT_R()   gpio_read(SPT511X_INT_IO)
#define IIC_DELAY   900
#define CONFIG_EN   0 //Custom.h
#define DEV_ADDR	(0x60)
#define HYN_I2C_ADDR 0x15
#define CHIP_CODE   0x52
//#define USER_PASSWORD  99999

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

typedef struct {
    u8 init;
    hw_iic_dev iic_hdl;
} ft_param;

static ft_param module_param = {

    .iic_hdl = 0,  //IIC 0//susuTP

};
#define __this   (&module_param)

typedef enum {
    EEP_CUSTOM_ADDR = 0x1080,
    EEP_USR_ADDR    = 0x1100,
} EEP_ADDR_ENUM;


#if (CONFIG_EN == 1)
#include "spt511x_custom_config.h"
#endif

#define i2c_t uint8_t
#define mdelay(ms) delay_ms(ms)
#define msleep(ms) delay_ms(ms)

static void delay_ms(int cnt)
{
    int delay = (cnt + 9) / 10;
    os_time_dly(delay);
}
void spt511x_reset()
{
    SPT511X_RESET_H();
    delay_2ms(10);
    SPT511X_RESET_L();
    delay_2ms(1);
    SPT511X_RESET_H();
    delay_2ms(3);
    SPT511X_RESET_L();
    delay_2ms(5);
    SPT511X_RESET_H();
    delay_2ms(10);
}

int i2c_write_regs(i2c_t dev, uint16_t address, uint16_t reg,
                   void *data, size_t length, uint8_t flags)
{
    int ret;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        r_printf("错误........");
        iic_stop(__this->iic_hdl);
        return -1;
    }
    udelay(IIC_DELAY);
    /* ret = iic_write_buf(__this->iic_hdl, reg & 0xff, 1); */
    iic_tx_byte(__this->iic_hdl, reg);
    udelay(IIC_DELAY);

    udelay(IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, data, length);
    udelay(IIC_DELAY);
    iic_stop(__this->iic_hdl);
    udelay(IIC_DELAY);

    return 0;
}

int i2c_read_regs(i2c_t dev, uint16_t address, uint16_t reg,
                  void *data, size_t length, uint8_t flags)
{
    int ret;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        r_printf("错误........");
        iic_stop(__this->iic_hdl);
        return -1;
    }
    udelay(IIC_DELAY);
    /* ret = iic_write_buf(__this->iic_hdl, reg & 0xff, 1); */
    iic_tx_byte(__this->iic_hdl, reg);
    udelay(IIC_DELAY);

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (address << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        return -1;
    }
    udelay(IIC_DELAY);
    ret = iic_read_buf(__this->iic_hdl, data, length);
    udelay(IIC_DELAY);
    iic_stop(__this->iic_hdl);
    udelay(IIC_DELAY);
    return 0;
}

int i2c_read_regs_ex(i2c_t dev, uint16_t address, void *reg,
                     void *data, size_t length, uint8_t flags)
{
    int ret;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        iic_stop(__this->iic_hdl);
        printf("%s,%d iic error", __func__, __LINE__);
        return -1;
    }
    udelay(IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, reg, 2);
    udelay(IIC_DELAY);
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (address << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        printf("%s,%d iic error", __func__, __LINE__);
        return -1;
    }
    udelay(IIC_DELAY);
    ret = iic_read_buf(__this->iic_hdl, data, length);
    udelay(IIC_DELAY);
    iic_stop(__this->iic_hdl);
    udelay(IIC_DELAY);

    return 0;
}

int spt511x_read_register(uint16_t address, void *data, uint16_t reg_addr, size_t length)
{
    int ret;
    iic_start(__this->iic_hdl);
    log_info("__this->iic_hdl = %d address << 1 = %x,(u8)(reg_addr) = %x ,(u8)(reg_addr >> 8)= %x ", __this->iic_hdl, address << 1, (u8)(reg_addr), (u8)(reg_addr >> 8));
    if (0 == iic_tx_byte(__this->iic_hdl, (address << 1))) {
        iic_stop(__this->iic_hdl);
        log_info("%s,%d iic error", __func__, __LINE__);
        return -1;
    }
    iic_tx_byte(__this->iic_hdl, (u8)(reg_addr));
    iic_tx_byte(__this->iic_hdl, (u8)(reg_addr >> 8));
    iic_stop(__this->iic_hdl);
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (address << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        log_info("%s,%d iic error", __func__, __LINE__);
        return -1;
    }
    udelay(IIC_DELAY);
    ret = iic_read_buf(__this->iic_hdl, data, length);
    udelay(IIC_DELAY);
    iic_stop(__this->iic_hdl);
    udelay(IIC_DELAY);
    return 0;
}
int spt511x_write_register(uint16_t address, void *data, uint16_t reg_addr, size_t length)
{
    int ret;
    int i;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        iic_stop(__this->iic_hdl);
        log_info("iic_tx_byte fail!\n");
        return -1;
    }
    iic_tx_byte(__this->iic_hdl, (u8)(reg_addr));
    iic_tx_byte(__this->iic_hdl, (u8)(reg_addr >> 8));
    udelay(IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, data, length);
    udelay(IIC_DELAY);
    iic_stop(__this->iic_hdl);
    udelay(IIC_DELAY);
    return 0;
}

#define ENERGY_XDISTANCE	15
#define ENERGY_YDISTANCE	15
#define ENERGY_TIME			20 //ms
struct touch_kinetic_energy {
    int t1;
    int x1;
    int y1;

    int t2;
    int x2;
    int y2;
};
static struct touch_kinetic_energy tke = {0};
static void tke_start(int x, int y)
{
    memcpy(&tke.t2, &tke.t1, sizeof(tke) / 2);

    if ((tke.x1 == x) && (tke.y1 == y)) {
        tke.t1 = tke.t1;
    } else {
        tke.t1 = jiffies_msec();
    }
    tke.x1 = x;
    tke.y1 = y;
    /* printf("tke start %d, %d, %d\n", tke.t1, tke.x1, tke.y1); */
}

static void tke_stop(int x, int y)
{
    struct touch_event t;
    u8 xdir, ydir;

    if (tke.t1 == 0) {
        return;
    }
    tke.t1 = tke.t1 - tke.t2;
    tke.x1 = tke.x1 - tke.x2;
    tke.y1 = tke.y1 - tke.y2;

    /* printf("tke end %d, %d, %d\n", tke.t1, tke.x1, tke.y1); */

    if ((tke.t1 < ENERGY_TIME) &&
        ((abs(tke.x1) > ENERGY_XDISTANCE) || abs(tke.y1) > ENERGY_YDISTANCE)) {

        if (abs(tke.x1) <= ENERGY_XDISTANCE) {
            tke.x1 = 0;
        }
        if (abs(tke.y1) <= ENERGY_YDISTANCE) {
            tke.y1 = 0;
        }
        xdir = (tke.x1 < 0) ? 1 : 2;
        ydir = (tke.y1 < 0) ? 1 : 2;
        /* printf("xdir %d, ydir %d\n", xdir, ydir); */

        t.event = ELM_EVENT_TOUCH_ENERGY;
        t.x = (tke.x1 << 16) | (tke.t1 & 0xffff);
        t.y = (tke.y1 << 16) | (ydir << 8) | (xdir & 0xff);
        ui_touch_msg_post(&t);
        /* printf("tke out %d, %d, %d, %d, %d\n", (t.x & 0xffff), (t.x >> 16), (t.y >> 16), (t.y & 0xff), (t.y >> 8) & 0xff); */
    }
    memset(&tke, 0, sizeof(tke));

}

struct point {
    int x;
    int y;
};
typedef struct {
    struct point point;
    int state;
} lv_indev_data_t;
bool spt511x_read(lv_indev_data_t *data);
static bool touch_flag = 0;

static void touch_int_handler(u8 index, u8 gpio)
{
    if (gpio != TCFG_TP_INT_IO) {
        return ;
    }
    lv_indev_data_t touch_data;
    touch_flag = 1;
    spt511x_read(&touch_data);

}


#define LV_INDEV_STATE_PR  0
#define LV_INDEV_STATE_REL 1

static int vk_state = LV_INDEV_STATE_REL;


#define Y_MIRROR  0//1
#define X_MIRROR  0
#define VK_Y_MOVE   	100//227
#define VK_X_MOVE      	100//227
#define VK_X_Y_DIFF    	50

static u8 tp_last_staus = ELM_EVENT_TOUCH_UP;
static int tp_down_cnt = 0;

static void tpd_down(int raw_x, int raw_y, int x, int y, int p)
{
    struct touch_event t;
    static int first_x = 0;
    static int first_y = 0;
    static u8 move_flag = 0;

    // static int first_x = 0;
    // static int first_y = 0;
    if (x < 0) {
        x = 0;
    }
    if (x > (VK_X - 1)) {
        x = VK_X - 1;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > (VK_Y - 1)) {
        y = VK_Y - 1;
    }
#if Y_MIRROR
    x = VK_X - x - 1;
#endif

#if X_MIRROR
    y = VK_Y - y - 1;
#endif

    if ((tp_last_staus == ELM_EVENT_TOUCH_DOWN) && (x == first_x) && (y == first_y)) {
        tp_down_cnt++;
        if (tp_down_cnt < 30) {
            return;
        }
        tp_last_staus = ELM_EVENT_TOUCH_HOLD;
        tp_down_cnt = 0;

        t.event = tp_last_staus;
        t.x = x;
        t.y = y;
        log_info("----tpd_hold----");
        ui_touch_msg_post(&t);
        return;
    }
    if (tp_last_staus != ELM_EVENT_TOUCH_UP) {
        int x_move = abs(x - first_x);
        int y_move = abs(y - first_y);
        log_info("D[x_move %4d y_move%4d first_x %4d first_y %4d]\n", x_move, y_move, first_x, first_y);
        if (!move_flag && (x_move >= VK_X_MOVE || y_move >= VK_Y_MOVE) && (abs(x_move - y_move) >= VK_X_Y_DIFF)) {
            if (x_move > y_move) {
                if (x > first_x) {
                    tp_last_staus = ELM_EVENT_TOUCH_R_MOVE;
                    log_info("----tpd_rigth_move----");
                } else {
                    tp_last_staus = ELM_EVENT_TOUCH_L_MOVE;
                    log_info("----tpd_left_move----");
                }

            } else {

                if (y > first_y) {
                    tp_last_staus = ELM_EVENT_TOUCH_D_MOVE;
                    log_info("----tpd_down_move----");
                } else {

                    tp_last_staus = ELM_EVENT_TOUCH_U_MOVE;
                    log_info("----tpd_up_move----");
                }
            }
            move_flag = 1;
        } else {
            if ((x_move <= 5) && (y_move <= 5)) {
                //log_info("x y 移动小于5像素,判断为按下");
                tp_last_staus = ELM_EVENT_TOUCH_DOWN;
                return;
            }
            tp_last_staus = ELM_EVENT_TOUCH_MOVE;
            log_info("----tpd_move----");
        }
        /* tp_last_staus = ELM_EVENT_TOUCH_HOLD; */
        tke_start(x, y);
    } else {
        tp_last_staus = ELM_EVENT_TOUCH_DOWN;
        first_x = x;
        first_y = y;
        move_flag = 0;
        log_info("----tpd_down----");
    }

    t.event = tp_last_staus;
    t.x = x;
    t.y = y;
    ui_touch_msg_post(&t);
}

static void tpd_up(int raw_x, int raw_y, int x, int y, int p)
{
    struct touch_event t;

    if (x < 0) {
        x = 0;
    }
    if (x > (VK_X - 1)) {
        x = VK_X - 1;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > (VK_Y - 1)) {
        y = VK_Y - 1;
    }

#if Y_MIRROR
    x = VK_X - x - 1;
#endif

#if X_MIRROR
    y = VK_Y - y - 1;
#endif

    /* log_info("U[%4d %4d %4d]\n", x, y, 0); */
    tp_last_staus = ELM_EVENT_TOUCH_UP;
    tp_down_cnt = 0;
    t.event = tp_last_staus;
    t.x = x;
    t.y = y;
    log_info("----tpd_up---- x = %d y = %d ", x, y);
    ui_touch_msg_post(&t);

    tke_stop(x, y);
}


bool spt511x_read(lv_indev_data_t *data)
{
    unsigned irq_state;
    unsigned char buf[7];
    int update = 0;
    static int x_last, y_last;
    i2c_read_regs(__this->iic_hdl, HYN_I2C_ADDR, 0x00, &buf[0], 7, 0);

    log_info("%d,%d,%d,%d,%d,%d,%d\r\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);

    data->point.x = (buf[4] | ((buf[3] & 0xF) << 8));
    //data->point.x = VK_X - (buf[4] | ((buf[3] & 0xF) << 8));
    data->point.y = buf[6] | ((buf[5] & 0xF) << 8);

    log_info("[ %d,%d ]", data->point.x, data->point.y);

#define LCD_WIDTH   240
#if 0//TOUCH_X_DIR_REVERSE
    data->point.x = LCD_WIDTH - data->point.x - 1;
#endif

    x_last = data->point.x;
    y_last = data->point.y;
    data->state = (buf[2] != 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    log_info("finger:%d, (%d, %d), %s\n", buf[2], data->point.x, data->point.y, (data->state == LV_INDEV_STATE_PR) ? "pressed" : "released");
    if ((data->point.x == VK_X) && (data->point.y == VK_Y)) {
        vk_state = data->state;
        // printf("vk %s\n", (vk_state == LV_INDEV_STATE_PR) ? "press" : "release");
    }

    if (buf[2] != 0) {
        tpd_down(data->point.x, data->point.y, data->point.x, data->point.y, 0);
    } else {
        tpd_up(data->point.x, data->point.y, data->point.x, data->point.y, 0);
    }
    return false; /* no more data to read */
}

static void touch_event_handler(void *priv)
{
    u8 buf[1];
    int i;
    lv_indev_data_t touch_data;
    if (!touch_flag) {
        return ;
    }
    touch_flag = 0;
    spt511x_read(&touch_data);
}

FIRMWARE_UPDATE_STATUS spt511x_asu_connect(void)
{
    uint8_t reg_temp;
    //CHIP CODE verify
    spt511x_read_register(DEV_ADDR, &reg_temp, 0xC000, 1);
    log_info("reg_temp = %d ", reg_temp);
    if (reg_temp != CHIP_CODE) {
        return CHIP_CODE_ERR;
    }
    //Enter upgrade
    spt511x_read_register(DEV_ADDR, &reg_temp, 0xC103, 1);
    reg_temp &= 0xf0;// low 4 bits clear
    reg_temp |= 0x20;
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC103, 1);//re-write
    spt511x_read_register(DEV_ADDR, &reg_temp, 0xC103, 1);
    log_info("reg_temp = %d ", reg_temp);
    if ((reg_temp & 0x20) == 0) {
        return UPGRADE_HOLD_ERR;
    }
    return EXEC_OK;
}

FIRMWARE_UPDATE_STATUS spt511x_unlock(void)
{
    uint8_t reg_temp;
#ifdef USER_PASSWORD
    reg_temp = (uint8_t)(USER_PASSWORD & 0xFF);
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC40C, 1);
    reg_temp = (uint8_t)(USER_PASSWORD >> 8);
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC40D, 1);
#endif //USER_PASSWORD
    spt511x_read_register(DEV_ADDR, &reg_temp, 0xC407, 1);
    if (reg_temp & 0x40) {
        return EXEC_OK;
    } else {
        return UNLOCK_ERR;
    }
}

FIRMWARE_UPDATE_STATUS spt511x_chip_erase(void)
{
    uint8_t reg_temp;
    uint16_t over_time;
    reg_temp = 0x00;//addr1
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC403, 1);
    reg_temp = 0x03;//clk margin
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC401, 1);
    reg_temp = 0x24;//cfg
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC400, 1);
    reg_temp = 0x04;//IE
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC408, 1);
    reg_temp = 0x82;
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC406, 1);
    over_time = 150;//150ms overtime
    while (over_time--) {
        spt511x_read_register(DEV_ADDR, &reg_temp, 0xC409, 1);
        if (reg_temp & 0x04) {
            reg_temp = 0x04;
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC409, 1);
            break;
        }
        delay_2ms(1);
    }
    if (over_time) {
        return EXEC_OK;
    } else {
        return ERASE_OVERTIME_ERR;
    }
}

FIRMWARE_UPDATE_STATUS spt511x_write_flash(uint16_t write_addr, uint8_t *write_buffer, uint16_t write_length)
{
    uint8_t reg_temp;
    uint8_t last_data_addr = write_addr + write_length;
    if ((write_buffer == 0) || (write_length == 0) || (write_addr > 0x0FFF) || ((write_addr + write_length) > 0x1000)) {
        return PG_PARAMS_ERR;
    }
    reg_temp = 0x03;//clk margin
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC401, 1);
    reg_temp = 0x94;//cfg
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC400, 1);
    reg_temp = 0x01;//ctl
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC406, 1);
    reg_temp = (write_addr / 2) & 0x3F; //addr_lo
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC402, 1);
    reg_temp = ((write_addr / 2) >> 6) & 0x1F; //addr hi
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC403, 1);
    //addr odd deal
    if (write_addr % 2) { //odd
        reg_temp = 0;
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC404, 1);
        reg_temp = *write_buffer++;
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC405, 1);
        write_length--;
        if (write_length == 0) {
            return EXEC_OK;
        }
    } else { //even
        if (write_length == 1) {
            reg_temp = *write_buffer++;
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC404, 1);
            reg_temp = 0;
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC405, 1);
            return EXEC_OK;
        }
    }
    //because of i2c, no udelay needed
    for (uint16_t i = 0; i < write_length; i += 2) {
        wdt_clear();
        reg_temp = *(write_buffer + i);
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC404, 1);
        if (i == (write_length - 1) && (write_length % 2)) { //length odd deal
            spt511x_read_register(0x060, &reg_temp, last_data_addr, 1);//rewrite read data
        } else {
            reg_temp = *(write_buffer + i + 1);
        }
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC405, 1);//Convenient for single byte I2C migration
    }
    reg_temp = 0x14;//reset to read mode
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC400, 1);
    reg_temp = 0x00;//ctl canncel pg
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC406, 1);
    return EXEC_OK;
}

FIRMWARE_UPDATE_STATUS spt511x_read_flash(uint16_t read_addr, uint8_t *read_buffer, uint16_t read_length)
{
    if ((read_buffer == 0) || (read_length == 0) || (read_addr > 0x0FFF) || ((read_addr + read_length) > 0x1000)) {
        return RD_PARAMS_ERR;
    }
    spt511x_read_register(DEV_ADDR, read_buffer, read_addr, read_length);
    return EXEC_OK;
}

FIRMWARE_UPDATE_STATUS spt511x_eeprom_erase(EEP_ADDR_ENUM eep_addr)
{
    uint8_t reg_temp;
    uint16_t over_time;
    reg_temp = 0;//addr lo
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC402, 1);
    if (eep_addr == EEP_CUSTOM_ADDR) {
        reg_temp = 33 | 0x20;//addr hi
    } else {
        reg_temp = 34 | 0x20;//addr hi
    }
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC403, 1);
    reg_temp = 0x03;//clk margin
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC401, 1);
    reg_temp = 0x24;//cfg
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC400, 1);
    reg_temp = 0x02; //IE
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC408, 1);
    reg_temp = 0x84;//ctl
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC406, 1);
    over_time = 150;
    while (over_time--) {
        spt511x_read_register(DEV_ADDR, &reg_temp, 0xC409, 1);
        if (reg_temp & 0x02) {
            reg_temp = 0x02;
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC409, 1);
            break;
        }
        delay_2ms(1);
    }
    if (over_time) {
        return EXEC_OK;
    } else {
        return ERASE_OVERTIME_ERR;
    }
}

//mapping addr 0~127 to eeprom addr
FIRMWARE_UPDATE_STATUS spt511x_eeprom_read(EEP_ADDR_ENUM eep_addr, uint8_t read_addr, uint8_t *read_buffer, uint8_t read_length)
{
    uint16_t mtp_addr = eep_addr + read_addr;
    if ((read_buffer == 0) || (read_length == 0) || (read_addr > 127) || ((read_addr + read_length - 1) > 127)) {
        return EEP_RD_PARAMS_ERR;
    }
    spt511x_read_register(DEV_ADDR, read_buffer, mtp_addr, read_length);
    return EXEC_OK;
}

FIRMWARE_UPDATE_STATUS spt511x_eeprom_write(EEP_ADDR_ENUM eep_addr, uint8_t write_addr, uint8_t *write_buffer, uint8_t write_length)
{
    uint8_t reg_temp;
    uint16_t over_time;
    uint16_t mtp_addr = eep_addr + write_addr;
    if ((write_buffer == 0) || (write_length == 0) || (write_addr > 127) || ((write_addr + write_length - 1) > 127)) {
        return EEP_WR_PARAMS_ERR;
    }
    reg_temp = 0x03;//clk margin
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC401, 1);
    reg_temp = 0x14;//cfg
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC400, 1);
    while (write_length--) {
        reg_temp = (mtp_addr / 2) & 0x3F; //addr_lo
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC402, 1);
        reg_temp = ((mtp_addr / 2) >> 6) & 0x3F; //addr hi
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC403, 1);
        if (mtp_addr % 2) { //odd deal
            reg_temp = 0; //data lo
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC404, 1);
            reg_temp = *write_buffer; //data hi
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC405, 1);
        } else {
            reg_temp = *write_buffer; //data lo
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC404, 1);
            reg_temp = 0; //data hi
            spt511x_write_register(DEV_ADDR, &reg_temp, 0xC405, 1);
        }
        write_buffer++;
        mtp_addr ++;
        reg_temp = 0x81;//ctl
        spt511x_write_register(DEV_ADDR, &reg_temp, 0xC406, 1);
        over_time = 150;
        while (over_time--) {
            spt511x_read_register(DEV_ADDR, &reg_temp, 0xC407, 1);
            if (reg_temp & 0x02) { //idle
                break;
            }
            delay_2ms(1);
        }
        if (over_time) {
            continue;
        } else {
            return EEP_WR_OVERTIME_ERR;
        }
    }
    reg_temp = 0x14;//reset to read mode
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC400, 1);
    reg_temp = 0x00;//ctl canncel pg
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC406, 1);
    return EXEC_OK;
}

void spt511x_reboot_soft(void)
{
    uint8_t reg_temp;
    spt511x_read_register(DEV_ADDR, &reg_temp, 0xC100, 1);
    reg_temp |= 0x81;
    spt511x_write_register(DEV_ADDR, &reg_temp, 0xC100, 1);
}

FIRMWARE_UPDATE_STATUS spt511x_flash_verify(uint16_t flash_addr, uint8_t *firmware, uint16_t firmware_length)
{
    uint8_t read_temp;
    for (uint16_t i = 0; i < firmware_length; i++) {
        wdt_clear();
        spt511x_read_flash(flash_addr + i, &read_temp, 1);
        if (read_temp != firmware[i]) { //verify ng
            return PG_VERIFY_ERR;
        }
    }
    return EXEC_OK;
}

uint8_t spt511x_config_verify(uint8_t *eep_read, uint8_t *config, uint16_t config_length)
{
    for (uint16_t i = 0; i < config_length; i++) {
        if (eep_read[i] != config[i]) {
            return 1;
        }
    }

    for (uint16_t i = config_length; i < 128; i++) {
        if (eep_read[i] != 0) {
            return 1;
        }
    }
    return 0;
}


void spt511x_firmware_update(void)
{
    int8_t firmware_version_read[18] = {0}; //version read buffer from eeprom
#if (CONFIG_EN == 1) //config size verify
    uint8_t config_read[128] = {0};
#endif
    FIRMWARE_UPDATE_STATUS status = EXEC_OK;
    int ret;
    log_info("\r\n#######################plxc spt511x firmware update#######################\r\n");
    //firmware size verify
    if (FIRMWARE_SIZE > 4096) {
        log_info("# spt511x firmware size verify fail ! \r\n");
        return;
    }

#if (CONFIG_EN == 1) //config size verify
    if (CONFIG_SIZE > 128) {
        log_info("# spt511x config size verify fail ! \r\n");
        return;
    }
#endif
    //connect
    status = spt511x_asu_connect();
    if (status == EXEC_OK) {
        log_info("# spt511x connect success! \r\n");
    } else {
        log_info("# spt511x connect fail. fail code:%d \r\n", status);
        goto exit;
    }
    //unlock
    status = spt511x_unlock();
    if (status == EXEC_OK) {
        log_info("## spt511x unlock success! \r\n");
    } else {
        log_info("## spt511x unlock fail. fail code:%d \r\n", status);
        goto exit;
    }

#if (CONFIG_EN == 1) //config update
    status = spt511x_eeprom_read(EEP_CUSTOM_ADDR, 0, config_read, 128);
    if (status == EXEC_OK) {
        log_info("###*** spt511x config read ok:\r\n");
    } else {
        log_info("###*** spt511x config read fail. fail code:%d \r\n", status);
        goto exit;
    }

    ret = spt511x_config_verify(config_read, (uint8_t *)spt511x_config, CONFIG_SIZE);
    if (ret == 0) {
        log_info("####*** spt511x config is already latest!\r\n");
    } else {
        log_info("####*** spt511x config version verify different, updating\r\n");
        //erase custom eeprom
        status = spt511x_eeprom_erase(EEP_CUSTOM_ADDR);
        if (status == EXEC_OK) {
            log_info("#####*** spt511x custom eeprom erase success! \r\n");
        } else {
            log_info("#####*** spt511x custom eeprom erase fail. fail code:%d \r\n", status);
            goto exit;
        }

        //write config to custom eeprom
        status = spt511x_eeprom_write(EEP_CUSTOM_ADDR, 0, (uint8_t *)spt511x_config, CONFIG_SIZE);
        if (status == EXEC_OK) {
            log_info("######*** spt511x write config to custom eeprom success! \r\n");
        } else {
            log_info("######*** spt511x write config to custom eeprom fail. fail code:%d \r\n", status);
            goto exit;
        }
    }

#endif

    //read eep to get firmware version
    status = spt511x_eeprom_read(EEP_USR_ADDR, 0, (uint8_t *)firmware_version_read, 17);
    if (status == EXEC_OK) {
        log_info("### spt511x firmware read: %s\r\n", firmware_version_read);
    } else {
        log_info("### spt511x firmware read fail. fail code:%d \r\n", status);
        goto exit;
    }
    //verify firmware version
    ret = memcmp((char *)firmware_version_read, spt511x_version, 17);
    if (ret == 0) {
        log_info("#### spt511x firmware is already latest!\r\n");
        goto exit;
    } else {
        log_info("#### spt511x firmware version verify different, updating\r\n");
    }
    //flash erase
    status = spt511x_chip_erase();
    if (status == EXEC_OK) {
        log_info("##### spt511x chip erase success! \r\n");
    } else {
        log_info("##### spt511x chip erase fail. fail code:%d \r\n", status);
        goto exit;
    }
    //flash write
    status = spt511x_write_flash(0, (uint8_t *)spt511x_firmware, FIRMWARE_SIZE);
    if (status == EXEC_OK) {
        log_info("###### spt511x flash write success! \r\n");
    } else {
        log_info("###### spt511x flash write fail. fail code:%d \r\n", status);
        goto exit;
    }
    //falsh verify
    status = spt511x_flash_verify(0, (uint8_t *)spt511x_firmware, FIRMWARE_SIZE);
    if (status == EXEC_OK) {
        log_info("####### spt511x flash verify success! \r\n");
    } else {
        log_info("####### spt511x flash verify fail. fail code:%d \r\n", status);
        goto exit;
    }
    //erase user eeprom
    status = spt511x_eeprom_erase(EEP_USR_ADDR);
    if (status == EXEC_OK) {
        log_info("######## spt511x user eeprom erase success! \r\n");
    } else {
        log_info("######## spt511x user eeprom erase fail. fail code:%d \r\n", status);
        goto exit;
    }
    //write firmware version to user eeprom
    status = spt511x_eeprom_write(EEP_USR_ADDR, 0, (uint8_t *)spt511x_version, 17);
    if (status == EXEC_OK) {
        log_info("######### spt511x write firmware version to eeprom success! \r\n");
    } else {
        log_info("######### spt511x write firmware version to eeprom fail. fail code:%d \r\n", status);
        goto exit;
    }
    log_info("\r\n#######################plxc spt511x firmware update complete#######################\r\n");
    goto exit;
exit:
    spt511x_reboot_soft(); //exit must reboot
}

void ctp_enter_sleep(void)   //
{
    printf("TP进入休眠.......................");
    unsigned char buf[3];
    delay_ms(100);
    char reg[2] = {0x00, 0xE5};
    buf[0] = 0x03;
    i2c_write_regs(__this->iic_hdl, HYN_I2C_ADDR, 0xE5, buf, 1, 1);
}

void ctp_exit_sleep(void)
{
    printf("TP进入工作.......................");
    int ret_val;
    unsigned char buf[5] = {0};
    // control reset pin
    spt511x_reset();
    delay_ms(100);
    //check tp exist
    char reg[2] = {0x00, 0xA9};
    i2c_read_regs(__this->iic_hdl, HYN_I2C_ADDR, 0xA9, buf, 1, 1);
    printf("cst816 firmware version: %d\r\n", buf[0]);
}

void ui_set_touch_event(void (*touch_event)(void *priv), int interval);
// extern void io_ext_interrupt_init(u8 port, trigger_mode_type trigger_mode, IO_ISR_FUNC cbfun);
void spt511x_init()
{
    u8 buf[1];
    int i;
    printf("spt511x_init!\n");
    spt511x_reset();
    iic_init(__this->iic_hdl);
    spt511x_firmware_update();
    delay_2ms(75);
    SPT511X_INT_IN();
    port_edge_wkup_set_callback_by_index(1, touch_int_handler); // 序号需要和板级配置中的wk_param对应上
    // io_ext_interrupt_init(spt511x_INT_IO, falling_edge_trigger, touch_int_handler);
#if TCFG_UI_ENABLE
    ui_set_touch_event(NULL, 0);
#endif
}
#endif

#endif

