#include "app_config.h"
#include "system/includes.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "asm/gpio.h"
#include "asm/mcpwm.h"
#include "ft6336g.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_FT6336G_ENABLE

/* static void delay_10ms(int cnt) */
/* { */
/*     int delay = (cnt + 9) / 10; */
/*     os_time_dly(delay); */
/* } */
/* #define tp_dly_2ms		delay_10ms */

/* #define LOG_TAG_CONST       FT6336G */
#define LOG_TAG             "[ft6336g]"
#include "debug.h"

#define log_info(fmt, ...)


#include "ui/ui.h"
#include "ui/ui_api.h"

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

static ft6336_param module_param = {
    .init = 0,
    .iic_hdl = 1
};
#define __this   (&module_param)

#define FT6336GU_IIC_DELAY   0
#define FT6336GU_IIC_ADDR  0x38

#define TP_RESET_IO			TCFG_TP_RESET_IO
#define TP_INT_IO			TCFG_TP_INT_IO

#define FT6336GU_RESET_H() gpio_write(TP_RESET_IO, 1)
#define FT6336GU_RESET_L() gpio_write(TP_RESET_IO, 0)
#define FT6336GU_INT_IO    (TP_INT_IO)
#define FT6336GU_INT_IN()  \
	do { \
		gpio_direction_input(FT6336GU_INT_IO);\
		gpio_set_pull_down(FT6336GU_INT_IO, 0);\
		gpio_set_pull_up(FT6336GU_INT_IO, 1);\
	} while(0);
#define FT6336GU_INT_R()   gpio_read(FT6336GU_INT_IO)




static int tpd_i2c_write(u8 *buf, int len)
{
    int ret = 0;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, FT6336GU_IIC_ADDR << 1)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    delay(FT6336GU_IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, buf, len);
    iic_stop(__this->iic_hdl);
    return ret;
}


static u8 tpd_i2c_read(u8 *buf, u8 len, u8 address)
{
    int i;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, FT6336GU_IIC_ADDR << 1)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    delay(FT6336GU_IIC_DELAY);
    if (0 == iic_tx_byte(__this->iic_hdl, address)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    delay(FT6336GU_IIC_DELAY);

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (FT6336GU_IIC_ADDR << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    for (i = 0; i < len; i++) {
        delay(FT6336GU_IIC_DELAY);
        if (i == (len - 1)) {
            *buf++ = iic_rx_byte(__this->iic_hdl, 0);
        } else {
            *buf++ = iic_rx_byte(__this->iic_hdl, 1);
        }
    }
    delay(FT6336GU_IIC_DELAY);
    iic_stop(__this->iic_hdl);
    delay(FT6336GU_IIC_DELAY);
    return 1;
}


/*touch info*/
struct touch_info {
    int y[10];
    int x[10];
    int p[10];
    int id[10];
    int count;
};
static int tpd_halt = 0; //suspend=1, resume=0
static int point_num = 0;
/* static int p_point_num = 0; */


#define Y_MIRROR  1
#define X_MIRROR  1
#define VK_Y      240
#define VK_X      240
/* struct touch_event { */
/* 	int event; */
/* 	int x; */
/* 	int y; */
/* }; */

static u8 tp_last_staus = ELM_EVENT_TOUCH_UP;
static int tp_down_cnt = 0;

#define abs(x)  ((x)>0?(x):-(x) )

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
        log_info("----tpd_hold----");
        ui_touch_msg_post(&t);
        return;
    }

    /* log_info("D[%4d %4d %4d]\n", x, y, p); */
    if (tp_last_staus != ELM_EVENT_TOUCH_UP) {
        int x_move = abs(x - first_x);
        int y_move = abs(y - first_y);

        if (!move_flag && (x_move >= 120 || y_move >= 120) && (abs(x_move - y_move) >= 25)) {
            if (x_move > y_move) {
                if (x > first_x) {
                    tp_last_staus = ELM_EVENT_TOUCH_R_MOVE;
                    log_info("----tpd_right_move----");
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
            if ((x == first_x) || (y == first_y)) {
                return;
            }
            tp_last_staus = ELM_EVENT_TOUCH_MOVE;
            log_info("----tpd_move----");
        }
        /* tp_last_staus = ELM_EVENT_TOUCH_HOLD; */
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
    log_info("----tpd_up----");
    ui_touch_msg_post(&t);
}

static bool gesture_en_mode = 0;
/************************************************************************
 * Name: tpd_touchinfo
 * Brief: touch info
 * Input: touch info point, no use
 * Output: no
 * Return: success nonzero
 ***********************************************************************/
static int tpd_touchinfo(struct touch_info *cinfo, struct touch_info *pinfo)
{
    int i = 0;
    u8 data[64] = {0};
    u16 high_byte, low_byte, reg;
    u8 report_rate = 0;
    /* p_point_num = point_num; */
    if (tpd_halt) {
        log_info("tpd_touchinfo return ..\n");
        return false;
    }

    if (gesture_en_mode == 1) {
        tpd_i2c_read(&data[30], 1, 0xd3); //手势ID
        tpd_i2c_read(&data[37], 1, 0xda); //debug use
        point_num = 0;
        log_info("0xd3:%d,0xda:%d", data[30], data[37]);
    } else {
        reg = 0x00;
        tpd_i2c_read(data, 15, reg); //读2个

        /*get the number of the touch points*/
        point_num = data[2] & 0x0f;
        for (i = 0; i < point_num; i++) {
            cinfo->p[i] = data[3 + 6 * i] >> 6; //event flag
            cinfo->id[i] = data[3 + 6 * i + 2] >> 4; //touch id
            /*get the X coordinate, 2 bytes*/
            high_byte = data[3 + 6 * i];
            high_byte <<= 8;
            high_byte &= 0x0f00;
            low_byte = data[3 + 6 * i + 1];
            cinfo->x[i] = high_byte | low_byte;
            high_byte = data[3 + 6 * i + 2];
            high_byte <<= 8;
            high_byte &= 0x0f00;
            low_byte = data[3 + 6 * i + 3];
            cinfo->y[i] = high_byte | low_byte;
        }
        /* log_info("device mode0:%d,gest_id01=0x%x, weigth7=0x%x, misc8=0x%x", data[0],data[1],data[7],data[8]); */
        log_info("tpd cinfo->x[0] = %d, cinfo->y[0] = %d, cinfo->p[0] = %d, cinfo->id[0] = %d", cinfo->x[0], cinfo->y[0], cinfo->p[0], cinfo->id[0]);
    }
    return true;
};


static bool touch_flag = 0;
static bool touch_event_polling_flag = 0;
static int touch_event_handler();
#ifdef CONFIG_CPU_BR23
static int touch_int_handler()
{
#elif defined(CONFIG_CPU_BR28)||defined(CONFIG_CPU_BR27)
static int touch_int_handler(u8 index, u8 gpio) {
    if (gpio != FT6336GU_INT_IO) {
        return 0;
    }
#endif

    touch_flag = 1;
    if (FT6336G_ID_G_MODE) {
        touch_event_handler();
    } else {
        touch_event_polling_flag = 1;
    }
    return 0;
}

static int touch_event_handler() {
    static struct touch_info cinfo;//, pinfo;
    struct touch_info pinfo;
    int i = 0;
    if (!touch_flag) {
        return 0;
    }
    touch_flag = 0;

    do {
        if (tpd_touchinfo(&cinfo, &pinfo)) {
            log_info("tpd point_num = %d", point_num);
            if (point_num > 0) { //only support 1 point
                tpd_down(cinfo.x[0], cinfo.y[0], cinfo.x[0], cinfo.y[0], 0);
                if (point_num == 2) {
                    tpd_down(cinfo.x[0], cinfo.y[0], cinfo.x[0], cinfo.y[0], 0);
                }
            } else {
                tpd_up(cinfo.x[0], cinfo.y[0], cinfo.x[0], cinfo.y[0], 0);
            }
        }
    } while (0);
    return 0;
}
////////
void touch_event_polling() {
    if (touch_event_polling_flag) {
        touch_flag = 1;
        touch_event_handler();
        if (FT6336GU_INT_R() == 1) {
            touch_event_polling_flag = 0;
        }
        /* if(FT6336GU_INT_R()==0){ */
        /* 	touch_flag = 1; */
        /* 	touch_event_handler(); */
        /* } else { */
        /* 	touch_event_polling_flag=0; */
        /* } */
    }
}

int ft6336g_id_g_mode(u8 mode) { //1:trigger, 0:polling
    u8 data_w[2] = {FTS_REG_ID_G_MODE, mode};
    int res = 0;
    res = tpd_i2c_write(data_w, 2);
    if (res != 2) {
        log_error("set id_g_mode fail!");
    }
    return res;
}

void ft6336g_gesture_switch(u8 gesture_en) { //1:normal to gesture,  0:gesture to normal
    u8 data_w[2] = {ID_G_SPEC_GESTURE_ENABLE, gesture_en};
    int res = 0;
    res = tpd_i2c_write(data_w, 2);
    if (res != 2) {
        log_error("set gesture en/dis fail!");
    }
    data_w[0] = ID_G_SPEC_GESTURE_TYPE_ENABLE; //字符,手势
    data_w[1] = 0xff; //bit7bit6:no,bit5:数字字符识别,bit4:双击,bit3:下滑直线,bit2:上滑直线,bit1:右滑直线,bit0:左滑直线
    res = tpd_i2c_write(data_w, 2);
    if (res != 2) {
        log_error("set gesture en/dis fail!");
    }
    if (gesture_en) {
        gesture_en_mode = 1;
    } else {
        gesture_en_mode = 0;
    }
    //字符
}

int ft6336g_power_consumption_mode(enum power_mode mode) {
    u8 data_w[2] = {FTS_REG_ID_G_PMODE, mode};
    int res = 0;
    res = tpd_i2c_write(data_w, 2);
    if (res != 2) {
        log_error("set power mode fail!");
    }
    return res;
}

void ft6336g_reset() {
    FT6336GU_RESET_L();
    mdelay(5);//>=5ms
    FT6336GU_RESET_H();
    mdelay(297);//>=300-5ms
}
void ft6336g_exit_hibernate_mode() {
    ft6336g_reset();
    ft6336g_power_consumption_mode(POWER_ACTIVE_MODE);//enter active mode
}
//iic scl:10K~400KHz
int ft6336g_init(ft6336_param * param) {
    int ret = 0;
    u8 uc_reg_value;
    FT6336GU_INT_IN();
    gpio_write(TP_RESET_IO, 1);
    gpio_set_direction(TP_RESET_IO, 0);
    gpio_set_die(TP_RESET_IO, 1);
    mdelay(1);//上电后低电平时间>=1ms
    FT6336GU_RESET_L();
    mdelay(5);//>=5ms
    FT6336GU_RESET_H();
    mdelay(297);//>=300-5ms
    iic_init(__this->iic_hdl);
    u8 data_w[2] = {0x86, 1};
    //register0 must be zero
    if (tpd_i2c_read(&uc_reg_value, 1, 0x86)) { //monitor模式开关
        if (uc_reg_value == 0) {
            tpd_i2c_write(data_w, 2);
        }
    } else {
        log_info("FT6336g read fail!");
    }
    ft6336g_id_g_mode(FT6336G_ID_G_MODE);//1:trigger, 0:polling
    if (tpd_i2c_read(&uc_reg_value, 1, 0xa4)) { //host work:trigger
        if (uc_reg_value != FT6336G_ID_G_MODE) {
            log_info("FT6336g read fail!");
            return -1;
        }
    }
    //add set gesture or normal mode
    if (tpd_i2c_read(&uc_reg_value, 1, FTS_REG_POINT_RATE)) {
        log_info("FT6336g report rate is %dHz.", uc_reg_value * 10);
    } else {
        log_info("FT6336g read report rate fail!");
    }
    if (tpd_i2c_read(&uc_reg_value, 1, FTS_REG_FW_VER)) {
        log_info("FT6336g Firmware version = 0x%x", uc_reg_value);
    } else {
        log_info("FT6336g read Firmware version fail!");
    }
    if (tpd_i2c_read(&uc_reg_value, 1, FTS_REG_CHIP_ID_HIGH)) {
        log_info("FT6336g chip id is 0x%x.\n", uc_reg_value);
    } else {
        log_info("FT6336g read chip id fail!\n");
        return -1;
    }

    log_info("fts Touch Panel Device Probe pass\n");
    /* u8 data_w1[2]={0x86,1}; */
    /* u8 data_w1[2]={0xa4,1}; */
    /* u8 data_w1[2]={0xd0,0}; */
    /* tpd_i2c_write(data_w1, 2); */

#if 0
    u8 reg_addr = 0x80;
    for (; reg_addr < 0xb1; reg_addr++) {
        udelay(100);
        tpd_i2c_read(&uc_reg_value, 1, reg_addr);
        log_info("reg_addr:0x%x, value:0x%x", reg_addr, uc_reg_value);
    }
    tpd_i2c_read(&uc_reg_value, 1, 0xbc);
    log_info("\nreg_addr:0xbc, value:0x%x\n", uc_reg_value);
    for (reg_addr = 0xd0; reg_addr < 0xdb; reg_addr++) {
        udelay(100);
        tpd_i2c_read(&uc_reg_value, 1, reg_addr);
        log_info("reg_addr:0x%x, value:0x%x", reg_addr, uc_reg_value);
    }
#endif
    return 0;
}
void ft6336g_test() {
    ft6336g_init(NULL);
}



void ui_set_touch_event(void (*touch_event)(void *priv), int interval);

extern void port_wkup_interrupt_init(u32 port, u8 trigger_edge, void (*cbfun)(void));
void ft6336g_int_en() {
    /* log_info("-------------------port wkup isr---------------------------"); */
#ifdef CONFIG_CPU_BR23
    io_ext_interrupt_init(FT6336GU_INT_IO, 1, touch_int_handler);
#elif defined(CONFIG_CPU_BR28)
    // br28外部中断回调函数，按照现在的外部中断注册方式
    // io配置在板级，定义在板级头文件，这里只是注册回调函数
    port_edge_wkup_set_callback_by_index(1, touch_int_handler); // 序号需要和板级配置中的wk_param对应上
#elif defined(CONFIG_CPU_BR27)
    port_edge_wkup_set_callback(touch_int_handler);
#endif
    if (FT6336G_ID_G_MODE == 0) {
        //定时执行touch_event_polling();
        sys_timer_add((void *)NULL, touch_event_polling, 20); //注册按键扫描定时器
    }

    /* port_wkup_interrupt_init(IO_PORTA_03, 0, ft6336g_scan);//上升沿触发 */
    /* sys_timer_add((void *)NULL, touch_event_handler, 20); //注册按键扫描定时器 */
#if TCFG_UI_ENABLE
    ui_set_touch_event(NULL, 0);
#endif
}


#endif

#endif


