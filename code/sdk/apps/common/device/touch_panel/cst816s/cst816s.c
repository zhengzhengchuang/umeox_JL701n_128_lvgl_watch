#include "app_config.h"
#include "system/includes.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "asm/gpio.h"
#include "asm/mcpwm.h"
#include "ui/ui.h"
#include "ui/ui_api.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_CST816S_ENABLE//CST816S_UPGRADE==1

#include "capacitive_hynitron_cst816s_update.h"

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

static ft_param module_param = {0};
#define __this   (&module_param)

#define CST816S_IIC_DELAY   1
#define CST816S_IIC_ADDR  0x46

//#define CST816S_RESET_H()   gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_set_pull_up(TCFG_TP_RESET_IO, 1);gpio_set_pull_down(TCFG_TP_RESET_IO, 0);gpio_set_hd(TCFG_TP_RESET_IO, 1);gpio_set_hd0(TCFG_TP_RESET_IO, 1);gpio_direction_input(TCFG_TP_RESET_IO);//gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_direction_output(TCFG_TP_RESET_IO,1)
//#define CST816S_RESET_L()   gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_set_pull_up(TCFG_TP_RESET_IO, 0);gpio_set_pull_down(TCFG_TP_RESET_IO, 1);gpio_set_hd(TCFG_TP_RESET_IO, 1);gpio_set_hd0(TCFG_TP_RESET_IO, 1);gpio_direction_input(TCFG_TP_RESET_IO);//gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_direction_output(TCFG_TP_RESET_IO,0)
#define CST816S_RESET_H()     gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_set_hd(TCFG_TP_RESET_IO, 0);gpio_set_hd0(TCFG_TP_RESET_IO, 1);gpio_direction_output(TCFG_TP_RESET_IO,1)
#define CST816S_RESET_L()     gpio_set_die(TCFG_TP_RESET_IO, 1);gpio_set_hd(TCFG_TP_RESET_IO, 0);gpio_set_hd0(TCFG_TP_RESET_IO, 1);gpio_direction_output(TCFG_TP_RESET_IO,0)

#define CST816S_INT_IO    (TCFG_TP_INT_IO)
#define CST816S_INT_IN()  gpio_set_die(CST816S_INT_IO, 1);gpio_direction_input(CST816S_INT_IO);gpio_set_pull_down(CST816S_INT_IO,0);gpio_set_pull_up(CST816S_INT_IO,1);
#define CST816S_INT_R()   gpio_read(CST816S_INT_IO)


#define PER_LEN	512
#define CST816S_UPGRADE_ADD (0x6A) /* 7-bit i2c addr */
#define HYN_I2C_ADDR 0x15 /* 7-bit i2c addr */ //0x38

#define I2C_MASTER_CI2C0 0
#define i2c_t uint8_t
//#define mdelay(ms) delay_ms(ms)6
//#define msleep(ms) delay_ms(ms)

static void delay_ms(int cnt)
{
    int delay = (cnt + 9) / 10;
    os_time_dly(delay);
}

int i2c_write_regs(i2c_t dev, uint16_t address, uint16_t reg,
                   void *data, size_t length, uint8_t flags)
{
    int ret;
    /* printf("i2c_read_regs_ex.\n"); */

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        iic_stop(__this->iic_hdl);
        return -1;
    }
    delay(CST816S_IIC_DELAY);
    /* ret = iic_write_buf(__this->iic_hdl, reg & 0xff, 1); */
    iic_tx_byte(__this->iic_hdl, reg);
    delay(CST816S_IIC_DELAY);

    delay(CST816S_IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, data, length);
    delay(CST816S_IIC_DELAY);
    iic_stop(__this->iic_hdl);
    delay(CST816S_IIC_DELAY);

    return 0;
}

int i2c_read_regs(i2c_t dev, uint16_t address, uint16_t reg,
                  void *data, size_t length, uint8_t flags)
{
    int ret;
    /* printf("i2c_read_regs_ex.\n"); */

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        iic_stop(__this->iic_hdl);
        return -1;
    }
    delay(CST816S_IIC_DELAY);
    /* ret = iic_write_buf(__this->iic_hdl, reg & 0xff, 1); */
    iic_tx_byte(__this->iic_hdl, reg);
    delay(CST816S_IIC_DELAY);

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (address << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        return -1;
    }

    delay(CST816S_IIC_DELAY);
    ret = iic_read_buf(__this->iic_hdl, data, length);
    delay(CST816S_IIC_DELAY);
    iic_stop(__this->iic_hdl);
    delay(CST816S_IIC_DELAY);

    return 0;
}

int i2c_read_regs_ex(i2c_t dev, uint16_t address, void *reg,
                     void *data, size_t length, uint8_t flags)
{
    int ret;
    /* printf("i2c_read_regs_ex.\n"); */

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        iic_stop(__this->iic_hdl);
        return -1;
    }
    delay(CST816S_IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, reg, 2);
    delay(CST816S_IIC_DELAY);

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (address << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        return -1;
    }

    delay(CST816S_IIC_DELAY);
    ret = iic_read_buf(__this->iic_hdl, data, length);
    delay(CST816S_IIC_DELAY);
    iic_stop(__this->iic_hdl);
    delay(CST816S_IIC_DELAY);

    return 0;
}

int i2c_write_regs_ex(i2c_t dev, uint16_t address, void *reg,
                      const void *data, size_t length, uint8_t flags)
{
    int ret;
    int i;
    u8 *_reg = (u8 *)reg;
    u8 *_data = (u8 *)data;

    /* printf("i2c_write_regs_ex. reg:0x%02x%02x\n",_reg[0],_reg[1]); */
    /* printf("data:\n"); */
    /* for(i=0;i<length;i++) { */
    /* printf("%02x ",_data[i]); */
    /* } */
    /* printf("\n"); */
    /* printf("address = 0x%x\n",address<<1); */

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, address << 1)) {
        iic_stop(__this->iic_hdl);
        printf("iic_tx_byte fail!\n");
        return -1;
    }
    /* ret = iic_write_buf(__this->iic_hdl, reg, 2); */
    iic_tx_byte(__this->iic_hdl, _reg[0]);
    iic_tx_byte(__this->iic_hdl, _reg[1]);
    /* printf("ret = %d\n",ret); */
    delay(CST816S_IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, data, length);
    /* printf("ret = %d\n",ret); */
    delay(CST816S_IIC_DELAY);
    iic_stop(__this->iic_hdl);
    delay(CST816S_IIC_DELAY);

    return 0;
}

/*
static uint8_t CST816S_I2C_ReadOneByte(uint8_t reg)
{
    uint8_t retVal = 0;

    i2c_read_reg(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, reg, &retVal, 0);

    return retVal;
}

static void CST816S_I2C_Write_buf(uint8_t *buf, uint8_t len)
{
    i2c_write_regs(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, buf[0], &buf[1], len - 1, 0);
}

static void CST816S_I2C_Direct_Write_buf(uint8_t *buf, uint8_t len)
{
    i2c_write_bytes(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, buf, len, 0);
}

static void CST816S_I2C_Read_buf(uint8_t reg, uint8_t len, uint8_t *buf)
{
    i2c_read_regs(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, reg, buf, len, 0);
}

static uint8_t CST816S_I2C_Read_32(uint32_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t reg_addr[4] = {0};

	reg_addr[0] = ( reg >> 24 )& 0xFF;
	reg_addr[1] = ( reg >> 16 )& 0xFF;
	reg_addr[2] = ( reg >> 8 )& 0xFF;
	reg_addr[3] = reg&0xFF;

	i2c_write_bytes(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, reg_addr, 4, 0);
	i2c_read_bytes(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, buf, len, 0);

    return 0;
}*/

void cst816s_reset();
static int cst816s_enter_bootmode(void)
{
    uint8_t retryCnt = 10;

#if 0
    gpio_output_set(GPIO15, 0);
    uos_sleep(4);/* 20 ms */
    gpio_output_set(GPIO15, 1);
    uos_sleep(1);/* 5ms */
#else
    cst816s_reset();
#endif

    printf("%s %d \r\n", __FUNCTION__, __LINE__);

    while (retryCnt--) {
        uint8_t cmd[3];
        cmd[0] = 0xAB;

        char reg[2] = {0xA0, 0x01};
        if (-1 == i2c_write_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg, cmd, 1, 0)) { // enter program mode
            mdelay(2); // 4ms
            continue;
        }

        char reg1[2] = {0xA0, 0x03};
        if (-1 == i2c_read_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg1, cmd, 1, 0)) { // read flag
            mdelay(2); // 4ms
            continue;
        } else {
            printf("cmd[0] = 0x%02x\n", cmd[0]);
            if (cmd[0] != 0x55) {
                mdelay(2); // 4ms
                continue;
            } else {
                /* printf("%s %d \r\n", __FUNCTION__, __LINE__); */
                printf("cst816s_enter_bootmode succ!\n");
                return 0;
            }
        }
    }
    printf("%s %d retryCnt %d\r\n", __FUNCTION__, __LINE__, retryCnt);
    return -1;
}

static int cst816s_update(uint16_t startAddr, uint16_t len, uint8_t *src)
{
    uint16_t sum_len;
    uint8_t cmd[10];

    if (cst816s_enter_bootmode() == -1) {
        //printf("%s %d \r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    sum_len = 0;
    do {
        if (sum_len >= len) {
            return -1;
        }

        // send address
        cmd[0] = startAddr & 0xFF;
        cmd[1] = startAddr >> 8;

        char reg0[2] = {0xA0, 0x14};
        i2c_write_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg0, cmd, 2, 0);
        char reg1[2] = {0xA0, 0x18};
        i2c_write_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg1, src, PER_LEN, 0);

        cmd[0] = 0xEE;
        char reg2[2] = {0xA0, 0x04};
        i2c_write_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg2, cmd, 1, 0);
        mdelay(100);
        //printf("%s %d \r\n", __FUNCTION__, __LINE__);
        uint8_t retrycnt = 50;
        while (retrycnt--) {
            cmd[0] = 0;
            char reg_read[2] = {0xA0, 0x05};
            i2c_read_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg_read, cmd, 1, 0);
            //printf("%s %d \r\n", __FUNCTION__, __LINE__);
            if (cmd[0] == 0x55) {
                // success
                break;
            }
            mdelay(10);
        }

        startAddr += PER_LEN;
        src += PER_LEN;
        sum_len += PER_LEN;
        //printf("%s %d \r\n", __FUNCTION__, __LINE__);
    } while (len);

    // exit program mode
    cmd[0] = 0x00;
    char reg3[2] = {0xA0, 0x03};
    i2c_write_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg3, cmd, 1, 0);

    return 0;
}

static uint32_t cst816s_read_checksum(uint16_t startAddr, uint16_t len)
{
    union {
        uint32_t sum;
        uint8_t buf[4];
    } checksum;

    char cmd[3];
    char readback[4] = {0};

    if (cst816s_enter_bootmode() == -1) {
        return -1;
    }

    cmd[0] = 0;
    char reg[2] = {0xA0, 0x03};
    if (-1 == i2c_write_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg, cmd, 1, 0)) {
        return -1;
    }
    mdelay(500);

    checksum.sum = 0;
    char reg_read[2] = {0xA0, 0x08};
    if (-1 == i2c_read_regs_ex(I2C_MASTER_CI2C0, CST816S_UPGRADE_ADD, &reg_read, checksum.buf, 2, 0)) {
        return -1;
    }
    printf("%s checksum.sum 0x%x \r\n", __FUNCTION__, checksum.sum);
    return checksum.sum;
}

bool cst816s_upgrade(void)
{
    uint8_t lvalue;

    if (cst816s_enter_bootmode() == 0) {
        printf("%s %d \r\n", __FUNCTION__, __LINE__);
        if (sizeof(app_bin) > 10) {
            printf("%s %d \r\n", __FUNCTION__, __LINE__);
            uint16_t startAddr = app_bin[1];
            uint16_t length = app_bin[3];
            uint16_t checksum = app_bin[5];
            startAddr <<= 8;
            startAddr |= app_bin[0];
            length <<= 8;
            length |= app_bin[2];
            checksum <<= 8;
            checksum |= app_bin[4];

            printf("checksum = 0x%x\n", checksum);
            if (cst816s_read_checksum(startAddr, length) != checksum) {
                cst816s_update(startAddr, length, app_bin + 6);
                cst816s_read_checksum(startAddr, length);
                printf("%s %d \r\n", __FUNCTION__, __LINE__);
            }
        }
        return true;
    }

    return false;
}

extern void timer1_run(u32 precesion);

void ctp_delay_us(unsigned int time)
{
    udelay(time);
}
void ctp_delay_ms(unsigned short time)
{
    mdelay(time);
}

void cst816s_reset()
{
    CST816S_RESET_H();
    printf("cst816s_reset_h\n");
    ctp_delay_ms(10);
    CST816S_RESET_L();
    printf("cst816s_reset_l\n");
    ctp_delay_ms(10);
    CST816S_RESET_H();
    printf("cst816s_reset_h\n");
    //delay(5000);//5ms
    ctp_delay_ms(10);
}

static bool touch_flag = 0;
static void touch_event_handler(void *priv);

static int touch_int_handler()
{
    touch_flag = 1;

#if defined(CONFIG_CPU_BR28)
    touch_event_handler(NULL);
#endif

    return 0;
}

struct point {
    int x;
    int y;
};

typedef struct {
    struct point point;
    int state;
} lv_indev_data_t;

#define LV_INDEV_STATE_PR  0
#define LV_INDEV_STATE_REL 1
#define VK_X 400
#define VK_Y 400

static int vk_state = LV_INDEV_STATE_REL;

#define abs(x)  ((x)>0?(x):-(x) )

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

static void tke_stop(int x, int y, struct touch_event *up_t)
{
    struct touch_event t = {0};
    u8 xdir, ydir;

    if (tke.t1 == 0) {
        ui_touch_msg_post(up_t);
        return;
    }
    tke.t1 = tke.t1 - tke.t2;
    tke.x1 = tke.x1 - tke.x2;
    tke.y1 = tke.y1 - tke.y2;

    /* printf("tke end %d, %d, %d\n", tke.t1, tke.x1, tke.y1); */

    if ((tke.t1 < ENERGY_TIME) &&
        ((abs(tke.x1) > ENERGY_XDISTANCE) || abs(tke.y1) > ENERGY_YDISTANCE)) {
        up_t->has_energy = 1;
        ui_touch_msg_post(up_t);

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

    } else {
        //在这里发送up消息，因为没有惯性，所以直接发送
        ui_touch_msg_post(up_t);
    }

    memset(&tke, 0, sizeof(tke));

}


#define Y_MIRROR  0
#define X_MIRROR  0
#define VK_Y      		454
#define VK_X      		454
#define VK_Y_MOVE   	100//227
#define VK_X_MOVE      	100//227
#define VK_X_Y_DIFF    	47

static u8 tp_last_staus = ELM_EVENT_TOUCH_UP;
static int tp_down_cnt = 0;

static void tpd_down(int raw_x, int raw_y, int x, int y, int p)
{
    struct touch_event t;
    static int first_x = 0;
    static int first_y = 0;
    static u8 move_flag = 0;

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
        //log_info("----tpd_hold----");
        ui_touch_msg_post(&t);
        return;
    }

    /* printf("D[%4d %4d %4d]\n", x, y, p); */
    if (tp_last_staus != ELM_EVENT_TOUCH_UP) {
        int x_move = abs(x - first_x);
        int y_move = abs(y - first_y);

        if (!move_flag && (x_move >= VK_X_MOVE || y_move >= VK_Y_MOVE) && (abs(x_move - y_move) >= VK_X_Y_DIFF)) {
            if (x_move > y_move) {
                if (x > first_x) {
                    tp_last_staus = ELM_EVENT_TOUCH_R_MOVE;
                } else {
                    tp_last_staus = ELM_EVENT_TOUCH_L_MOVE;
                }

            } else {

                if (y > first_y) {
                    tp_last_staus = ELM_EVENT_TOUCH_D_MOVE;
                } else {
                    tp_last_staus = ELM_EVENT_TOUCH_U_MOVE;
                }
            }
            move_flag = 1;
        } else {
            if ((x == first_x) && (y == first_y)) {
                return;
            }
            tp_last_staus = ELM_EVENT_TOUCH_MOVE;
            /* return; */
        }
        /* tp_last_staus = ELM_EVENT_TOUCH_HOLD; */
        tke_start(x, y);
    } else {
        tp_last_staus = ELM_EVENT_TOUCH_DOWN;
        first_x = x;
        first_y = y;
        move_flag = 0;
    }

    t.event = tp_last_staus;
    t.x = x;
    t.y = y;
    ui_touch_msg_post(&t);
}



static void tpd_up(int raw_x, int raw_y, int x, int y, int p)
{
    struct touch_event t = {0};

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

    /* printf("U[%4d %4d %4d]\n", x, y, 0); */
    tp_last_staus = ELM_EVENT_TOUCH_UP;
    tp_down_cnt = 0;
    t.event = tp_last_staus;
    t.x = x;
    t.y = y;

    //不在这里发送up消息，在tke_stop里面!!!
    /* ui_touch_msg_post(&t); */

    tke_stop(x, y, &t);

}

bool cst816s_read(lv_indev_data_t *data)
{
    unsigned irq_state;
    unsigned char buf[5];
    int update = 0;
    static int x_last, y_last;

    /* if (update) { */
    i2c_read_regs(I2C_MASTER_CI2C0, HYN_I2C_ADDR, 0x2, &buf[0], 5, 0);
    data->point.x = buf[2] | ((buf[1] & 0xF) << 8);
    data->point.y = buf[4] | ((buf[3] & 0xF) << 8);
    x_last = data->point.x;
    y_last = data->point.y;
    data->state = (buf[0] != 0) ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    printf("finger:%d, (%d, %d), %s\n", buf[0], data->point.x, data->point.y, (data->state == LV_INDEV_STATE_PR) ? "pressed" : "released");
    if ((data->point.x == VK_X) && (data->point.y == VK_Y)) {
        vk_state = data->state;
        //printf("vk %s\n", (vk_state == LV_INDEV_STATE_PR) ? "press" : "release");
    }

    if (buf[0] != 0) {
        tpd_down(0, 0, data->point.x, data->point.y, 0);
    } else {
        tpd_up(0, 0, data->point.x, data->point.y, 0);
    }

    /* } else { */
    /* data->point.x = x_last; */
    /* data->point.y = y_last; */
    /* data->state = LV_INDEV_STATE_REL; */
    /* } */

    return false; /* no more data to read */
}

static u16 regs_table[] = {
    0x01,//GestureID
    0x02,//FingerNum
    0x03,//XposH
    0x04,//XposL
    0x05,//YposH
    0x06,//YposL
    0xB0,//BPC0H
    0xB1,//BPC0L
    0xB2,//BPC1H
    0xB3,//BPC1L
    0xA7,//ChipID
    0xA8,//ProjID
    0xA9,//FwVersion
    0xEC,//MotionMask
    0xED,//IrqPluseWidth
    0xEE,//NorScanPer
    0xEF,//MotionS1Angle
    0xF0,//LpScanRaw1H
    0xF1,//LpScanRaw1L
    0xF2,//LpScanRaw2H
    0xF3,//LpScanRaw2L
    0xF4,//LpAutoWakeTime
    0xF5,//LpScanTH
    0xF6,//LpScanWin
    0xF7,//LpScanFreq
    0xF8,//LpScanIdac
    0xF9,//AutoSleepTime
    0xFA,//IrqCtl
    0xFB,//AutoReset
    0xFC,//LongPressTime
    0xFD,//IOCtl
    0xFE//DisAutoSleep
};
static void touch_event_handler(void *priv)
{
    u8 buf[1];
    int i;

    lv_indev_data_t touch_data;
    if (!touch_flag) {
        return ;
    }
    touch_flag = 0;
    /* printf("touch_event_handler\n"); */
    cst816s_read(&touch_data);

#if 0
    buf[0] = 0x3;
    i2c_write_regs(I2C_MASTER_CI2C0, HYN_I2C_ADDR, 0xf8, &buf[0], 1, 0);

    for (i = 0; i < sizeof(regs_table) / sizeof(regs_table[0]); i++) {
        i2c_read_regs(I2C_MASTER_CI2C0, HYN_I2C_ADDR, regs_table[i], &buf[0], 1, 0);
        printf("addr:0x%x , val:0x%02x\n", regs_table[i], buf[0]);
    }
#endif
}


void ui_set_touch_event(void (*touch_event)(void *priv), int interval);
void cst816s_init()
{
    u8 buf[1];
    int i;
    printf("cst816s init...\n");

    iic_init(__this->iic_hdl);

    CST816S_INT_IN();

    cst816s_reset();
    ctp_delay_ms(100);
    i2c_read_regs(I2C_MASTER_CI2C0, HYN_I2C_ADDR, 0xa7, &buf[0], 1, 0);
    printf("tp id : 0x%x\n", buf[0]);

    i2c_read_regs(I2C_MASTER_CI2C0, HYN_I2C_ADDR, 0xa9, &buf[0], 1, 0);
    printf("tp version : 0x%x\n", buf[0]);

    if (!cst816s_upgrade()) {
        printf("cst816s_upgrade fail!\n");
        ASSERT(0);
    }
    cst816s_reset();

#ifdef CONFIG_CPU_BR23
    extern void io_ext_interrupt_init(u8 port, trigger_mode_type trigger_mode, IO_ISR_FUNC cbfun);
    io_ext_interrupt_init(CST816S_INT_IO, falling_edge_trigger, touch_int_handler);

#if TCFG_UI_ENABLE
    ui_set_touch_event(touch_event_handler, 2);
#endif

#elif defined(CONFIG_CPU_BR28)
    // br28外部中断回调函数，按照现在的外部中断注册方式
    // io配置在板级，定义在板级头文件，这里只是注册回调函数
    port_edge_wkup_set_callback_by_index(1, touch_int_handler); // 序号需要和板级配置中的wk_param对应上
#endif

}

#endif  //CST816S_COB==1

#endif



