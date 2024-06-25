#include "app_config.h"
#include "system/includes.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "asm/gpio.h"
#include "asm/mcpwm.h"

#if TCFG_TOUCH_PANEL_ENABLE

#if TCFG_TP_IT7259E_ENABLE

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

typedef struct {
    u8 init;
    hw_iic_dev iic_hdl;
} ft_param;

static ft_param module_param = {0};
#define __this   (&module_param)

#define FT6336G_IIC_DELAY   1
#define FT6336G_IIC_ADDR  0x46


#define TP_RESET_IO			TCFG_TP_RESET_IO
#define TP_INT_IO			TCFG_TP_INT_IO

#define FT6336G_RESET_H() gpio_direction_output(TP_RESET_IO, 1)
#define FT6336G_RESET_L() gpio_direction_output(TP_RESET_IO, 0)
#define FT6336G_INT_IO    (TP_INT_IO)
#define FT6336G_INT_IN()  \
	do { \
		gpio_direction_input(FT6336G_INT_IO);\
		gpio_set_pull_down(FT6336G_INT_IO, 0);\
		gpio_set_pull_up(FT6336G_INT_IO, 1);\
	} while(0);
#define FT6336G_INT_R()   gpio_read(FT6336G_INT_IO)




static int tpd_i2c_write(u8 *buf, int len)
{
    int ret = 0;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, FT6336G_IIC_ADDR << 1)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    delay(FT6336G_IIC_DELAY);
    ret = iic_write_buf(__this->iic_hdl, buf, len);
    return ret;
}


static u8 tpd_i2c_read(u8 *buf, u8 len, u8 address)
{
    int i;
    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, FT6336G_IIC_ADDR << 1)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    delay(FT6336G_IIC_DELAY);
    if (0 == iic_tx_byte(__this->iic_hdl, address)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }
    delay(FT6336G_IIC_DELAY);

    iic_start(__this->iic_hdl);
    if (0 == iic_tx_byte(__this->iic_hdl, (FT6336G_IIC_ADDR << 1) | 0x01)) {
        iic_stop(__this->iic_hdl);
        return 0;
    }

    for (i = 0; i < len; i++) {
        delay(FT6336G_IIC_DELAY);
        if (i == (len - 1)) {
            *buf++ = iic_rx_byte(__this->iic_hdl, 0);
        } else {
            *buf++ = iic_rx_byte(__this->iic_hdl, 1);
        }

    }
    delay(FT6336G_IIC_DELAY);
    iic_stop(__this->iic_hdl);
    delay(FT6336G_IIC_DELAY);

    return 1;
}

extern void delay_2ms(int cnt);

static int tpd_print_version(void)
{
    char buffer[9];
    int ret = -1;
    int retry = 0;

    extern void wdt_clear();
    wdt_clear();
    do {
        buffer[0] = 0xFF;
        ret = tpd_i2c_read(&buffer[0], 1, 0x80);
        printf(">>>>>>>>>>>>>> %s %d \n", __FUNCTION__, __LINE__);
        os_time_dly(1);
    } while ((buffer[0] & 0x01) && (retry++ < 10));
    if (buffer[0] & 0x01) {
        return -1;
    }

    buffer[0] = 0x20;
    buffer[1] = 0x1;
    buffer[2] = 0x0;
    ret = tpd_i2c_write(buffer, 3);
    if (ret != 3) {
        printf("[mtk-tpd] i2c write communcate error in getting FW version : 0x%x\n", ret);
    }

    os_time_dly(10);

    retry = 0;
    do {
        buffer[0] = 0xFF;
        ret = tpd_i2c_read(&buffer[0], 1, 0x80);
    } while ((buffer[0] & 0x01) && (retry++ < 10));
    if (buffer[0] & 0x01) {
        return -1;
    }

    ret = tpd_i2c_read(&buffer[0], 9, 0xA0);

    if (ret == 0) {
        printf("[mtk-tpd] i2c read communcate error in getting FW version : 0x%x\n", ret);
    } else {
        printf("[mtk-tpd] ITE7260 Touch Panel Firmware Version %x %x %x %x %x %x %x %x %x\n",
               buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);
    }
    return 0;
}



#define Y_MIRROR  1
#define X_MIRROR  1
#if (defined(TCFG_LCD_128X128_TEST) && TCFG_LCD_128X128_TEST)
#define VK_Y      		128
#define VK_X      		128
#define VK_Y_MOVE   	64
#define VK_X_MOVE      	64
#define VK_X_Y_DIFF    	15
#else // #if TCFG_LCD_128X128_TEST
#define VK_Y      		240
#define VK_X      		240
#define VK_Y_MOVE   	120
#define VK_X_MOVE      	120
#define VK_X_Y_DIFF    	25
#endif // #if TCFG_LCD_128X128_TEST

static u8 tp_last_staus = ELM_EVENT_TOUCH_UP;
static int tp_down_cnt = 0;

#define abs(x)  ((x)>0?(x):-(x) )
u8 touch_press_status = 0;

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
            if ((x == first_x) || (y == first_y)) {
                return;
            }
            tp_last_staus = ELM_EVENT_TOUCH_MOVE;
        }
        /* tp_last_staus = ELM_EVENT_TOUCH_HOLD; */
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

    /* printf("U[%4d %4d %4d]\n", x, y, 0); */
    tp_last_staus = ELM_EVENT_TOUCH_UP;
    tp_down_cnt = 0;
    t.event = tp_last_staus;
    t.x = x;
    t.y = y;
    ui_touch_msg_post(&t);
}

static int x[2] = {(int) - 1, (int) - 1 };
static int y[2] = {(int) - 1, (int) - 1 };
static bool finger[2] = { 0, 0 };
static bool flag = 0;
static bool touch_flag = 0;

static int touch_event_handler();

#ifdef CONFIG_CPU_BR23
static int touch_int_handler()
{
#elif defined(CONFIG_CPU_BR28)||defined(CONFIG_CPU_BR27)
static int touch_int_handler(u8 index, u8 gpio) {
    if (gpio != FT6336G_INT_IO) {
        return 0;
    }
#endif

    touch_flag = 1;
    touch_event_handler();

    return 0;
}

static int touch_event_handler() {
    unsigned char pucPoint[14];
    unsigned char cPoint[8];
    unsigned char ePoint[6];
    unsigned char key_temp = 0;
    int ret = 0;
    int xraw, yraw;
    int i = 0;

    if (!touch_flag) {
        return 0;
    }
    touch_flag = 0;

    do {
        ret = tpd_i2c_read(&pucPoint[0], 1, 0x80);
        if (!(pucPoint[0] & 0x80 || pucPoint[0] & 0x01)) {
            /* printf("No point.\n"); */
            continue;
        }
        ret = tpd_i2c_read(&pucPoint[0], 14, 0xE0);
        /* printf("puc[0] = %d,puc[1] = %d,puc[2] = %d\n",pucPoint[0],pucPoint[1],pucPoint[2]); */
        if (ret != 0) {
            if (pucPoint[0] & 0xF0) {          /* gesture */
                {
                    /* printf("[mtk-tpd] it's a button/gesture\n"); */
                    continue;
                }
            } else if (pucPoint[1] & 0x01) {        /* palm */
                /* printf("[mtk-tpd] it's a palm\n"); */
                continue;
            } else if (!(pucPoint[0] & 0x08)) {        /* no more data */
                if (finger[0]) {
                    finger[0] = 0;
                    tpd_up(x[0], y[0], x[0], y[0], 0);
                    flag = 1;
                }

                if (finger[1]) {
                    finger[1] = 0;
                    tpd_up(x[1], y[1], x[1], y[1], 0);
                    flag = 1;
                }

                if (flag) {
                    flag = 0;
                }

                /* printf("[mtk-tpd] no more data\n"); */

                continue;
            } else if (pucPoint[0] & 0x04) {        /* 3 fingers */
                /* printf("[mtk-tpd] we don't support three fingers\n"); */
                continue;
            } else {
                if (pucPoint[0] & 0x01) {              /* finger 1 */
                    //char pressure_point;

                    xraw = ((pucPoint[3] & 0x0F) << 8) + pucPoint[2];
                    yraw = ((pucPoint[3] & 0xF0) << 4) + pucPoint[4];
                    //pressure_point = pucPoint[5] & 0x0f;
                    //printf("[mtk-tpd] input Read_Point1 x=%d y=%d p=%d\n",xraw,yraw,pressure_point);
                    //tpd_calibrate(&xraw, &yraw);
                    x[0] = xraw;
                    y[0] = yraw;
                    finger[0] = 1;
                    tpd_down(x[0], y[0], x[0], y[0], 0);//1
                    //printf("tpd_down:x0=%d,y0=%d\n",x[0],y[0]);

                } else if (finger[0]) {
                    tpd_up(x[0], y[0], x[0], y[0], 0);
                    finger[0] = 0;
                }

                if (pucPoint[0] & 0x02) {              // finger 2
                    //char pressure_point;
                    xraw = ((pucPoint[7] & 0x0F) << 8) + pucPoint[6];
                    yraw = ((pucPoint[7] & 0xF0) << 4) + pucPoint[8];
                    //pressure_point = pucPoint[9] & 0x0f;
                    //printf("[mtk-tpd] input Read_Point2 x=%d y=%d p=%d\n",xraw,yraw,pressure_point);
                    // tpd_calibrate(&xraw, &yraw);
                    x[1] = xraw;
                    y[1] = yraw;
                    finger[1] = 1;
                    tpd_down(x[1], y[1], x[1], y[1], 1);
                    //printf("tpd_down:x1=%d,y1=%d\n",x[1],y[1]);

                } else if (finger[1]) {
                    tpd_up(x[1], y[1], x[1], y[1], 0);
                    finger[1] = 0;
                }
            }
        } else {
            /* printf("[mtk-tpd] i2c read communcate error in getting pixels : 0x%x\n", ret); */
        }
    } while (0);
    return 0;
}

extern void port_wkup_interrupt_init(u32 port, u8 trigger_edge, void (*cbfun)(void));

static void delay_ms(int cnt) {
    int delay = (cnt + 9) / 10;
    os_time_dly(delay);
}

int it7259e_init(ft_param * param) {
    int ret = 0;
    FT6336G_INT_IN();

    FT6336G_RESET_H();
    delay_ms(10);
    FT6336G_RESET_L();
    delay_ms(10);
    FT6336G_RESET_H();
    iic_init(__this->iic_hdl);
    ret = tpd_print_version();        //read firmware version
    if (ret) {
        return -1;
    }
    unsigned char reset_buf[2] = { 0x20, 0x6F};
    u8 buffer[8];
    if (!tpd_i2c_read(buffer, 1, 0x80)) {
        printf("it7260 I2C transfer error, line: %d\n", __LINE__);
        return -1;
    }

    os_time_dly(1);
    ret = tpd_i2c_write(reset_buf, 2);
    /* os_time_dly(10); */

    return 0;
}



void ui_set_touch_event(void (*touch_event)(void *priv), int interval);
void it7259_test() {
    int i;
    if (it7259e_init(NULL)) {
        return;
    }
}

void it7259_int_en() {
#ifdef CONFIG_CPU_BR23
    io_ext_interrupt_init(FT6336G_INT_IO, 1, touch_int_handler);
#elif defined(CONFIG_CPU_BR28)
    // br28外部中断回调函数，按照现在的外部中断注册方式
    // io配置在板级，定义在板级头文件，这里只是注册回调函数
    port_edge_wkup_set_callback_by_index(1, touch_int_handler); // 序号需要和板级配置中的wk_param对应上
#elif defined(CONFIG_CPU_BR27)
    port_edge_wkup_set_callback(touch_int_handler);
#endif
    /* port_wkup_interrupt_init(IO_PORTA_03, 0, ft6336g_scan);//上升沿触发 */
    /* sys_timer_add((void *)NULL, touch_event_handler, 20); //注册按键扫描定时器 */
#if TCFG_UI_ENABLE
    /* ui_set_touch_event(touch_event_handler, 20); */
    ui_set_touch_event(NULL, 0);
#endif
}

#endif

#endif
