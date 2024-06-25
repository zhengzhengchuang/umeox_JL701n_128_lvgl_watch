#include "app_config.h"
#include "includes.h"
#include "ui/ui_api.h"
#include "system/includes.h"
#include "system/timer.h"
#include "asm/spi.h"
#include "clock_cfg.h"
#include "asm/mcpwm.h"
#include "ui/res_config.h"
#include "asm/imd.h"
#include "asm/imb.h"
#include "ui/ui_sys_param.h"
#include "asm/psram_api.h"
#include "asm/gpio.h"
#include "os/os_api.h"
#include "product_test.h"
#include "ui/lcd_spi/lcd_drive.h"


#define lcd_debug printf

#define AT_ST77903_RAM      AT(.st77903_ram_code)

#ifndef LCD_POWER_DOWN_EN
#define LCD_POWER_DOWN_EN  0
#endif


#ifndef TP_POWER_DOWN_EN
#define TP_POWER_DOWN_EN  0
#endif


static u8  backlight_status = 0;
static u8  lcd_sleep_in     = 0;
static volatile u8 is_lcd_busy = 0;
static struct lcd_platform_data lcd_data_t = {
    .pin_reset = -1,
    .pin_cs = -1,
    .pin_dc = -1,
    .pin_en = -1,
    .pin_bl = -1,
    .pin_te = -1,
    .mcu_pins = {
        .pin_wr = -1,
        .pin_rd = -1,
    },
    .rgb_pins = {
        .pin_hsync = -1,
        .pin_vsync = -1,
        .pin_den = -1,
    },
};
static struct lcd_platform_data *lcd_dat = &lcd_data_t;
struct pwm_platform_data lcd_pwm_p_data;

static struct lcd_drive *__lcd = NULL;
static struct imd_param *__this = NULL;

extern void *malloc_vlt(size_t size);
extern void free_vlt(void *pv);
extern u32 check_ram1_size(void);
static void lcd_drv_cmd_list(u8 *cmd_list, int cmd_cnt);
void *lcd_qspi_st77903_get_info();
int lcd_qspi_st77903_mode();
void lcd_qspi_st77903_start(u8 *pixel, int lcd_w, int lcd_h, int lcd_stride, int back_porch, int front_porch);
void lcd_qspi_st77903_stop();
void lcd_qspi_st77903_refresh_mode_set(int mode);
int lcd_qspi_st77903_refresh_mode_get();
void lcd_qspi_st77903_idle_line_set(int idle);
void lcd_qspi_st77903_frame_mode_set(int mode);

void *br28_malloc(int size);
void br28_free(void *buf);

#define SINGAL_FRAME_MODE       0  //单帧模式
#define CONTINUE_FRAME_MODE     1  //连续帧模式
static int st77903_frame_mode = CONTINUE_FRAME_MODE;
static OS_SEM lcd_sem;


void dma_memcpy(void *dest, void *src, u32 len);
void dma_memcpy_wait_idle(void);

#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
static u8 *psram_frame_buffer = NULL;
static u8 *psram_obuf = NULL;
static u8 *psram_dobuf = NULL;

u8 *get_psram_dobuf()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return NULL;
    }
    ASSERT(__this);
    u8 rgb_factor = (__this->in_format/* LCD_FORMAT */ == OUTPUT_FORMAT_RGB888) ? 3 : 2;
    int stride = (__this->in_width/* SCR_W */ * rgb_factor + 3) / 4 * 4;

    if (psram_dobuf == NULL) {
        if (ENABLE_PSRAM_UI_FRAME) {
            psram_dobuf = malloc_psram(stride * __this->in_height/* SCR_H */);
        } else {
            psram_dobuf = malloc(stride * __this->in_height/* SCR_H */);
        }
        /* printf("\n\n\n\n\npsram1_addr %x\n\n\n\n\n", (u32)psram_dobuf); */
    }
    return psram_dobuf;
}

u8 *get_psram_obuf()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return NULL;
    }
    ASSERT(__this);
    u8 rgb_factor = (__this->in_format/* LCD_FORMAT */ == OUTPUT_FORMAT_RGB888) ? 3 : 2;
    int stride = (__this->in_width/* SCR_W */ * rgb_factor + 3) / 4 * 4;

    if (psram_obuf == NULL) {
        if (ENABLE_PSRAM_UI_FRAME) {
            psram_obuf = malloc_psram(stride * __this->in_height/* SCR_H */);
        } else {
            psram_obuf = malloc(stride * __this->in_height/* SCR_H */);
        }
        /* printf("\n\n\n\n\npsram1_addr %x\n\n\n\n\n", (u32)psram_obuf); */
    }
    return psram_obuf;
}

u8 *get_psram_frame_buffer()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return NULL;
    }
    ASSERT(__this);
    u8 rgb_factor = (__this->in_format/* LCD_FORMAT */ == OUTPUT_FORMAT_RGB888) ? 3 : 2;
    int stride = (__this->in_width/* SCR_W */ * rgb_factor + 3) / 4 * 4;

    if (psram_frame_buffer == NULL) {
        if (ENABLE_PSRAM_UI_FRAME) {
            psram_frame_buffer = malloc_psram(2 * stride * __this->in_height/* SCR_H */);
        } else {
            psram_frame_buffer = malloc(2 * stride * __this->in_height/* SCR_H */);
        }
        /* printf("\n\n\n\n\npsram0_addr %x\n\n\n\n\n", (u32)psram_frame_buffer); */
    }
    return psram_frame_buffer;
}

void free_psram_obuf()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return;
    }
    if (psram_obuf) {
        free_psram(psram_obuf);
        psram_obuf = NULL;
    }
}
void free_psram_dobuf()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return;
    }
    if (psram_dobuf) {
        free_psram(psram_dobuf);
        psram_dobuf = NULL;
    }
}


void free_psram_buffer()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return;
    }
    if (psram_frame_buffer) {
        free_psram(psram_frame_buffer);
        psram_frame_buffer = NULL;
    }
}

void set_psram_free()
{
    if (ENABLE_PSRAM_UI_FRAME == 0) {
        return;
    }
    if (psram_frame_buffer) {
        free_psram(psram_frame_buffer);
        psram_frame_buffer = NULL;
    }
    if (psram_obuf) {
        free_psram(psram_obuf);
        psram_obuf = NULL;
    }
}

#else
/* 如果没使能PSRAM，则不声明PSRAM buffer */
static u8 psram_frame_buffer[0];
u8 *get_psram_frame_buffer()
{
    return NULL;
}

u8 *get_psram_dobuf()
{
    return NULL;
}
u8 *get_psram_obuf()
{
    return NULL;
}
void free_psram_obuf()
{
}
void free_psram_dobuf()
{
}
void free_psram_buffer()
{
}
void set_psram_free()
{

}

#endif


#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
#define PHY_ADDR(addr) psram_cache2nocache_addr(addr)
#else
#define PHY_ADDR(addr) addr
#endif


// EN 控制
void lcd_en_ctrl(u8 val)
{
    if (lcd_dat->pin_en == NO_CONFIG_PORT)
        return;

    printf("=======lcd_en_ctrl:%d\n", val);
    
    gpio_set_die(lcd_dat->pin_en, 1);
    gpio_direction_output(lcd_dat->pin_en, val);

    return;
}

// BL 控制
void lcd_bl_ctrl(u8 val)
{
    if (lcd_dat->pin_bl == NO_CONFIG_PORT) {
        return;
    }
    gpio_set_die(lcd_dat->pin_bl, 1);
    gpio_direction_output(lcd_dat->pin_bl, !!val);
}

AT_ST77903_RAM
static u32 lcd_gpio_regs[] = {
    (u32) JL_PORTA,
    (u32) JL_PORTB,
    (u32) JL_PORTC,
    (u32) JL_PORTD,
    (u32) JL_PORTE,
    (u32) JL_PORTG,
    (u32) JL_PORTP,
};

AT_ST77903_RAM
static struct gpio_reg *gpio2reg(u32 gpio)
{
    if (gpio > IO_MAX_NUM) {
        return NULL;
    }
    return (struct gpio_reg *)lcd_gpio_regs[gpio / IO_GROUP_NUM];
}

#define USE_OLD_GPIO_CTRL 0

#if USE_OLD_GPIO_CTRL

#define __gpio_mask(gpio) \
	BIT((gpio) % IO_GROUP_NUM)

AT_ST77903_RAM
int lcd_gpio_direction_output(u32 gpio, int value)
{
    u32 mask;
    struct gpio_reg *g;

    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }

    mask = __gpio_mask(gpio);
    /* spin_lock(&lock); */
    if (value == 1) {
        g->out |= mask;
    } else if (value == 0) {
        g->out &= ~mask;
    }
    g->dir &= ~mask;
    /* spin_unlock(&lock); */

    return 0;
}

AT_ST77903_RAM
int lcd_gpio_set_die(u32 gpio, int value)
{
    u32 mask;
    struct gpio_reg *g;

    g = gpio2reg(gpio);
    if (!g) {
        return -EINVAL;
    }

    mask = __gpio_mask(gpio);
    /* spin_lock(&lock); */
    if (value) {
        g->die |= mask;
    } else {
        g->die &= ~mask;
    }
    /* spin_unlock(&lock); */

    return 0;
}
#endif

// CS 控制
AT_ST77903_RAM
static void spi_cs_ctrl(u8 val)
{
    if (lcd_dat->pin_cs == NO_CONFIG_PORT) {
        return;
    }
#if USE_OLD_GPIO_CTRL
    lcd_gpio_set_die(lcd_dat->pin_cs, 1);
    lcd_gpio_direction_output(lcd_dat->pin_cs, val);
#else
    gpio_set_die(lcd_dat->pin_cs, 1);
    gpio_direction_output(lcd_dat->pin_cs, val);
#endif
}

// DC 控制
static void spi_dc_ctrl(u8 val)
{
    if (lcd_dat->pin_dc == NO_CONFIG_PORT) {
        return;
    }
    gpio_set_die(lcd_dat->pin_dc, 1);
    gpio_direction_output(lcd_dat->pin_dc, val);
}

// TE 控制
static int spi_te_stat()
{
    if (lcd_dat->pin_te == NO_CONFIG_PORT) {
        return -1;
    }
#if (PRODUCT_TEST_ENABLE && (!PT_LCD_TP_ENABLE))
    if (product_test_check_run()) {
        return -1;
    }
#endif /* #if (PRODUCT_TEST_ENABLE && (!PT_LCD_TP_ENABLE)) */
    gpio_set_pull_up(lcd_dat->pin_te, 1);
    gpio_set_pull_down(lcd_dat->pin_te, 0);
    gpio_set_die(lcd_dat->pin_te, 1);
    gpio_direction_input(lcd_dat->pin_te);

    return gpio_read(lcd_dat->pin_te);
}


/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD DEVICE RESET
 *
 * Description: LCD 设备复位
 *
 * Arguments  : none
 *
 * Returns    : none
 *
 * Notes      : 1、判断是否在屏驱有重新定义LCD复位函数，
 * 					是使用屏驱上定义的LCD复位函数，
 * 					否使用GPIO控制板级配置的LCD复位IO
 *********************************************************************************************************
 */

static void lcd_reset(struct lcd_drive *lcd)
{
    if (lcd->reset) {
        lcd->reset();
    } else {
        gpio_set_die(lcd_dat->pin_reset, 1);
        /* gpio_direction_output(lcd_dat->pin_reset, 1); */
        /* os_time_dly(10); */
        gpio_direction_output(lcd_dat->pin_reset, 0);
        /* os_time_dly(10); */
        udelay(50);
        gpio_direction_output(lcd_dat->pin_reset, 1);
        os_time_dly(10);
        /* mdelay(100); */
    }
}



/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD BACKLIGHT CONTROL
 *
 * Description: LCD 背光控制
 *
 * Arguments  : on LCD 背光开关标志，0为关，其它值为开
 *
 * Returns    : none
 *
 * Notes      : 1、判断是否在屏驱有重新定义背光控制函数，
 * 					是使用屏驱上定义的背光控制函数，
 * 					否使用GPIO控制板级配置的背光IO
 *
 *              2、配置背光状态标志，打开为true，关闭为false
 *********************************************************************************************************
 */

static void lcd_mcpwm_init()
{
#if (TCFG_BACKLIGHT_PWM_MODE == 2)
    extern void mcpwm_init(struct pwm_platform_data * arg);
    lcd_pwm_p_data.pwm_aligned_mode = pwm_edge_aligned;         //边沿对齐
    lcd_pwm_p_data.pwm_ch_num = pwm_ch0;                        //通道
    lcd_pwm_p_data.frequency = 10000;                           //Hz
    lcd_pwm_p_data.duty = 10000;                                //占空比
    lcd_pwm_p_data.h_pin = lcd_dat->pin_bl;                     //任意引脚
    lcd_pwm_p_data.l_pin = -1;                                  //任意引脚,不需要就填-1
    lcd_pwm_p_data.complementary_en = 1;                        //两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&lcd_pwm_p_data);
#endif
}

int lcd_drv_backlight_ctrl_base(u8 percent)
{
    if (__lcd->backlight_ctrl) {
        __lcd->backlight_ctrl(percent);
    } else if (lcd_dat->pin_bl != -1) 
    {
#if (TCFG_BACKLIGHT_PWM_MODE == 0)
        if (percent > 0) {
            gpio_direction_output(lcd_dat->pin_bl, 1);
        } else {
            gpio_direction_output(lcd_dat->pin_bl, 0);
        }
#elif (TCFG_BACKLIGHT_PWM_MODE == 1)
        //注意：duty不能大于prd，并且prd和duty是非标准非线性的，建议用示波器看着来调
        extern int pwm_led_output_clk(u8 gpio, u8 prd, u8 duty);
        if (percent) {
            u32 light_level = (percent + 9) / 10;
            if (light_level > 10) {
                printf("lcd pwm full\n");
                light_level = 10;
            }
            pwm_led_output_clk(lcd_dat->pin_bl, 10, light_level);
        } else {
            pwm_led_output_clk(lcd_dat->pin_bl, 10, 0);
        }
#elif (TCFG_BACKLIGHT_PWM_MODE == 2)
        if (percent) {
            u32 pwm_duty = percent * 100;
            if (pwm_duty > 10000) {
                pwm_duty = 10000;
                printf("lcd pwm full\n");
            }
            mcpwm_set_duty(lcd_pwm_p_data.pwm_ch_num, pwm_duty);
        } else {
            mcpwm_set_duty(lcd_pwm_p_data.pwm_ch_num, 0);
        }
#endif
    } else {
        return -1;
    }

    return 0;
}

int lcd_drv_backlight_ctrl(u8 percent)
{
    int ret = lcd_drv_backlight_ctrl_base(percent);
    if (ret < 0) {
        backlight_status = 0;
    } else {
        backlight_status = percent;
    }
    return ret;
}

int lcd_drv_power_ctrl(u8 on)
{
    if (!__lcd) {
        __lcd = lcd_drv_get_hdl(TCFG_LCD_MATCH_MODE, LCD_LOGO);
    }

    if (!__this) {
        __this = __lcd->param;
    }

    if (__lcd && __lcd->power_ctrl) {
        __lcd->power_ctrl(on);
    }

    return 0;
}


//user 控制屏幕使能管教
int ldo_power_ctrl(uint8_t on)
{
    on = !!on;

    soff_gpio_protect(IO_PORTA_05);
    gpio_set_die(IO_PORTA_05, 1);
    gpio_direction_output(IO_PORTA_05, on);

    return 0;
}

struct lcd_platform_data *lcd_get_platform_data()
{
    return lcd_dat;
}

static int find_begin(u8 *begin, u8 *end, int pos)
{
    int i;
    u8 *p = &begin[pos];
    while ((p + 3) < end) {
        if ((p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]) == BEGIN_FLAG) {
            return (&p[4] - begin);
        }
        p++;
    }

    return -1;
}


static int find_end(u8 *begin, u8 *end, int pos)
{
    u8 *p = &begin[pos];
    while ((p + 3) < end) {
        if ((p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3]) == END_FLAG) {
            return (&p[0] - begin);
        }
        p++;
    }

    return -1;
}

void lcd_drv_cmd_list(u8 *cmd_list, int cmd_cnt)
{
    int i;
    int k;
    int cnt;
    u16 *p16;
    u8 *p8;

    u8 *temp = NULL;
    u16 temp_len = 5 * 64;
    u16 len;
    temp = (u8 *)malloc(temp_len);

    for (i = 0; i < cmd_cnt;) {
        p16 = (u16 *)&cmd_list[i];
        int begin = find_begin(cmd_list, &cmd_list[cmd_cnt], i);
        if ((begin != -1)) {
            int end = find_end(cmd_list, &cmd_list[cmd_cnt], begin);
            if (end != -1) {
                p8 = (u8 *)&cmd_list[begin];
                u8 *param;
                u32 addr;
                u8 cnt;
                if (lcd_get_param(CMD_MODE) == CMD_24BIT) {
                    cnt = end - begin - 3;
                    param = &p8[3];
                    addr = (p8[0] << 16) | (p8[1] << 8) | p8[2];
                } else if (lcd_get_param(CMD_MODE) == CMD_16BIT) {
                    cnt = end - begin - 2;
                    param = &p8[2];
                    addr = (p8[0] << 8) | p8[1];
                } else {
                    cnt = end - begin - 1;
                    param = &p8[1];
                    addr = p8[0];
                }

                if (((p8[0] << 24) | (p8[1] << 16) | (p8[2] << 8) | p8[3]) == REGFLAG_DELAY_FLAG) {
                    os_time_dly(p8[4] / 10);
                    /* printf("delay %d ms\n", p8[4]); */
                } else if (((p8[0] << 24) | (p8[1] << 16) | (p8[2] << 8) | p8[3]) == REGFLAG_CONFIRM_FLAG) {
                    u8 addr = p8[4];
                    u8 value = p8[5];
                    u8 timeout = p8[6];

                    /* printf("addr : 0x%x, value : 0x%x, timeout : %d\n", addr, value, timeout); */

                    u8 power_mode;
                    u32 begin = jiffies_msec();
                    u32 cnt = 0;
                    int wait_timeout = jiffies + msecs_to_jiffies(timeout); //超时时间设置
                    while (1) {
                        if (time_after(jiffies, wait_timeout)) {
                            printf("confirm fail! lcd power_mode status 0x%x wait timeout\n", value);
                            break;
                        }

                        lcd_read_cmd(addr, &power_mode, sizeof(power_mode));
                        /* printf("power_mode : 0x%0x\n", power_mode); */
                        if (power_mode == value) {
                            cnt++;
                            if (cnt > 10) {
                                break;
                            }
                        } else {
                            cnt = 0;
                        }
                    }
                    u32 end = jiffies_msec();
                    /* printf("0x%x wait %d ms\n", addr, end - begin); */
                }  else {
                    len = sprintf((char *)temp, "send : 0x%08x(%d), ", addr, cnt);
                    for (k = 0; k < cnt; k++) {
                        len += sprintf((char *)&temp[len], "0x%02x, ", param[k]);
                        if (len > (temp_len - 10)) {
                            len += sprintf((char *)&temp[len], "...");
                            break;
                        }
                    }
                    len += sprintf((char *)&temp[len], "\n");
                    /* printf("cmd:%s", temp); */

                    lcd_write_cmd(addr, param, cnt);
                }

                i = end + 4;
            }
        }
    }
    free(temp);
}


void lcd_wait_te()
{
#if 0//1
    //使用内部的推屏状态同步
    volatile struct lcd_st77903_info *priv = (struct lcd_st77903_info *)lcd_qspi_st77903_get_info();
    u32 wait_timeout = 0;

    wait_timeout = jiffies + msecs_to_jiffies(500);
    while (priv->draw_status != DRAW_END) { //wait low level
        if (time_after(jiffies, wait_timeout)) {
            printf("wait te timeout.\n");
            extern void wdt_clear(void);
            wdt_clear();
            wait_timeout = jiffies + msecs_to_jiffies(500);
            break;
        }
    }

#else
    //使用屏输出的te信号同步
    u32 wait_timeout = 0;
    if (spi_te_stat() == -1) {
        return;
    }

    wait_timeout = jiffies + msecs_to_jiffies(500);
    printf("%s:wait_timeout = %d\n", __func__, wait_timeout);
    printf("%s:jiffies = %d\n", __func__, jiffies);
    while(spi_te_stat()) 
    { 
        //wait low level
        if(time_after(jiffies, wait_timeout)) 
        {
            printf("wait te timeout.\n");
            extern void wdt_clear(void);
            wdt_clear();
            wait_timeout = jiffies + msecs_to_jiffies(500);
            break;
        }
    }

    wait_timeout = jiffies + msecs_to_jiffies(500);
    while (!spi_te_stat()) 
    {   
        //wait high level
        if (time_after(jiffies, wait_timeout)) 
        {
            printf("wait te timeout.\n");
            extern void wdt_clear(void);
            wdt_clear();
            wait_timeout = jiffies + msecs_to_jiffies(500);
            break;
        }
    }
#endif
}


static const u8 line_begin_cmd[] ALIGNED(4) = {
    0x11, 0x01, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void lcd_data_copy(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait)
{
    int h;
    u8 *p;
    static struct rect last_rect = {0};
    int buf_switch = 1;

    /* ASSERT(__this); */
    /* ASSERT(__lcd->lcd_buf); */
    if (!__this || !__lcd->lcd_buf) {
        return;
    }

    int bytes_per_pixel = (__this->in_format == FORMAT_RGB888) ? 3 : 2;
    int lcd_stride;

    if ((0 == lcd_qspi_st77903_refresh_mode_get()) || (2 == lcd_qspi_st77903_refresh_mode_get())) {
        lcd_stride = (__this->lcd_width * bytes_per_pixel + 3) / 4 * 4;
    } else {
        lcd_stride = ((__this->lcd_width + 8) * bytes_per_pixel + 3) / 4 * 4;
    }

    if ((type == 2) && (top == rect->top)) {
        if (__lcd->lcd_buf_num == 2) {
            if (memcmp(&last_rect, rect, sizeof(struct rect))) {
                dma_memcpy(PHY_ADDR(__lcd->lcd_dbuf[!__lcd->lcd_dbuf_index]), PHY_ADDR(__lcd->lcd_dbuf[__lcd->lcd_dbuf_index]), __lcd->lcd_buf_size / __lcd->lcd_buf_num);
            } else {
                buf_switch = 0;
            }
        }
    }

    p = __lcd->lcd_dbuf[!__lcd->lcd_dbuf_index];

    if ((left == 0) && (width == __this->lcd_width)) {
        if ((0 == lcd_qspi_st77903_refresh_mode_get()) || (2 == lcd_qspi_st77903_refresh_mode_get())) {
            dma_memcpy(PHY_ADDR(&p[top * lcd_stride]), lcd_buf, stride * height);
        } else {
            for (h = 0; h < height; h++) {
                dma_memcpy(PHY_ADDR(&p[16 + (top + h)*lcd_stride]), &lcd_buf[h * stride], width * bytes_per_pixel);
            }
        }
    } else {
        for (h = 0; h < height; h++) {
            if ((0 == lcd_qspi_st77903_refresh_mode_get()) || (2 == lcd_qspi_st77903_refresh_mode_get())) {
                dma_memcpy(PHY_ADDR(&p[(top + h)*lcd_stride + left * bytes_per_pixel]), &lcd_buf[h * stride], width * bytes_per_pixel);
            } else {
                dma_memcpy(PHY_ADDR(&p[16 + (top + h)*lcd_stride + left * bytes_per_pixel]), &lcd_buf[h * stride], width * bytes_per_pixel);
            }
        }
    }

    if ((top + height) == (rect->top + rect->height)) {
        if (__lcd->lcd_buf_num == 2) {
            if (buf_switch) {
                __lcd->lcd_dbuf_index = !__lcd->lcd_dbuf_index;
            }
        }
    }

    memcpy(&last_rect, rect, sizeof(struct rect));
}

void lcd_data_copy_wait()
{
    dma_memcpy_wait_idle();
}

static void lcd_buf_malloc()
{
    int lcd_w;
    int lcd_h;
    int lcd_stride;
    int h;
    int i;

    if ((0 == lcd_qspi_st77903_refresh_mode_get()) || (2 == lcd_qspi_st77903_refresh_mode_get())) {
        lcd_w = __this->lcd_width;
        lcd_h = __this->lcd_height;
        lcd_stride = (lcd_w * ((__this->in_format == FORMAT_RGB888) ? 3 : 2) + 3) / 4 * 4;
    } else {
        lcd_w = __this->lcd_width + 8;
        lcd_h = __this->lcd_height;
        lcd_stride = (lcd_w * ((__this->in_format == FORMAT_RGB888) ? 3 : 2) + 3) / 4 * 4;
    }

    if (!__lcd->lcd_buf) {
#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
        extern const int ENABLE_PSRAM_UI_FRAME;
        ASSERT(ENABLE_PSRAM_UI_FRAME == 0, ", st77903 not support ui effect yet!");
        __lcd->lcd_buf_num = 2;
        __lcd->lcd_buf_size = lcd_stride * lcd_h * __lcd->lcd_buf_num;
        __lcd->lcd_buf = (u8 *)malloc_psram(__lcd->lcd_buf_size);
        __lcd->lcd_dbuf[0] = __lcd->lcd_buf;
        __lcd->lcd_dbuf[1] = &__lcd->lcd_buf[lcd_stride * lcd_h];
        __lcd->lcd_dbuf_index = 0;
        printf("psram lcd_buf:0x%x, nocache buf:0x%x\n", __lcd->lcd_buf, psram_cache2nocache_addr(__lcd->lcd_buf));
#else
        __lcd->lcd_buf_num = 1;
        __lcd->lcd_buf_size = lcd_stride * lcd_h;
        __lcd->lcd_buf = (u8 *)br28_malloc(__lcd->lcd_buf_size);
        __lcd->lcd_dbuf[0] = __lcd->lcd_buf;
        __lcd->lcd_dbuf[1] = __lcd->lcd_buf;
        __lcd->lcd_dbuf_index = 0;
#endif // defined

    }
    ASSERT(__lcd->lcd_buf);

    memset(__lcd->lcd_buf, 0x00, __lcd->lcd_buf_size);
#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
    psram_flush_invaild_cache(__lcd->lcd_buf, __lcd->lcd_buf_size);
#endif // defined


    if (1 == lcd_qspi_st77903_refresh_mode_get()) {
        for (i = 0; i < __lcd->lcd_buf_num; i++) {
            u8 *p = __lcd->lcd_dbuf[i];

            for (h = 0; h < lcd_h; h++) {
                dma_memcpy(PHY_ADDR(&p[h * lcd_stride]), line_begin_cmd, sizeof(line_begin_cmd));
            }

        }
        dma_memcpy_wait_idle();
    }
}

static void lcd_buf_free()
{
    if (__lcd->lcd_buf) {
#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
        free_psram(__lcd->lcd_buf);
#else
        br28_free(__lcd->lcd_buf);
#endif // defined
        __lcd->lcd_buf = NULL;
        __lcd->lcd_dbuf[0] = NULL;
        __lcd->lcd_dbuf[1] = NULL;
    }
}

void lcd_buffer_info_get(int *lcd_w, int *lcd_h, int *lcd_stride)
{
    if ((0 == lcd_qspi_st77903_refresh_mode_get()) || (2 == lcd_qspi_st77903_refresh_mode_get())) {
        *lcd_w = __this->lcd_width;
        *lcd_h = __this->lcd_height;
        *lcd_stride = (*lcd_w * ((__this->in_format == FORMAT_RGB888) ? 3 : 2) + 3) / 4 * 4;
    } else {
        *lcd_w = __this->lcd_width + 8;
        *lcd_h = __this->lcd_height;
        *lcd_stride = (*lcd_w * ((__this->in_format == FORMAT_RGB888) ? 3 : 2) + 3) / 4 * 4;
    }
}


static void lcd_sem_check()
{
    volatile struct lcd_st77903_info *lcd_priv = (struct lcd_st77903_info *)lcd_qspi_st77903_get_info();
    if (lcd_priv->lcd_sem) {
        lcd_priv->lcd_sem = 0;
        os_sem_post(&lcd_sem);
    }
}

___interrupt
static void timer4_isr()
{
    static int cnt = 0;
    JL_TIMER4->CON |= BIT(14);
    lcd_sem_check();
}

void timer4_init(void)
{
    JL_TIMER4->CON = BIT(14);
    request_irq(IRQ_TIME4_IDX, 1, timer4_isr, 0);
    JL_TIMER4->CNT = 0;
    JL_TIMER4->PRD = 3000000 * 0.0005;//500us
    JL_TIMER4->CON = (5 << 10) | (1 << 4) | BIT(0); //std_12M
    printf("PRD : 0x%x / %d", JL_TIMER4->PRD, clk_get("timer"));
}

void timer4_uninit(void)
{
    JL_TIMER4->CON = 0;
}

static void lcd_task(void *arg)
{
    int lcd_w;
    int lcd_h;
    int lcd_stride;
    int lcd_enable = 0;

    ASSERT(__this);
    lcd_buffer_info_get(&lcd_w, &lcd_h, &lcd_stride);

    while (1) {
        os_sem_pend(&lcd_sem, 0);

        if (__lcd->lcd_enter) {
            __lcd->lcd_enter = 0;
            lcd_enable = 1;
        }

        if (__lcd->lcd_exit) {
            __lcd->lcd_exit = 0;
            lcd_enable = 0;
        }

        if (lcd_enable == 0) {
            continue;
        }

        if (__lcd->lcd_switch_mode) {//单帧无缝切换到连续帧
            __lcd->lcd_switch_mode = 0;

            printf("switch to continue frame.\n");

            lcd_enable = 0;
            timer4_uninit();
            //切换为连续帧模式
            lcd_qspi_st77903_frame_mode_set(CONTINUE_FRAME_MODE);
            st77903_frame_mode = CONTINUE_FRAME_MODE;
        }

        if (__lcd->lcd_buf) {
            lcd_qspi_st77903_start(__lcd->lcd_dbuf[__lcd->lcd_dbuf_index], lcd_w, lcd_h, lcd_stride, 8, 8);
        }
    }
}

/*
 * 弱函数重定义(连续帧双buffer情况下，用于切换buffer)
 * */
AT_ST77903_RAM
void *lcd_qspi_st77903_get_buffer()
{
    return __lcd->lcd_dbuf[__lcd->lcd_dbuf_index];
}

#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
void lcd_draw_hori_slide_test()//左右滑动
{
    int lcd_w;
    int lcd_h;
    int lcd_stride;
    u8 bytes_per_pixel;
    u8 *p;
    int w, h;

    ASSERT(__this);
    lcd_w = __this->lcd_width;
    lcd_h = __this->lcd_height;
    bytes_per_pixel = (__this->in_format == FORMAT_RGB888) ? 3 : 2;
    lcd_stride = (lcd_w * 2 * bytes_per_pixel + 3) / 4 * 4;

    u32 lcd_buf_size = lcd_stride * lcd_h;
    u8 *lcd_buf = (u8 *)malloc_psram(lcd_buf_size);
    ASSERT(lcd_buf);

    for (h = 0; h < lcd_h; h++) {
        for (w = 0; w < lcd_w * 2; w++) {
            if (w < lcd_w) {
                lcd_buf[h * lcd_stride + 2 * w] = 0xf8;
                lcd_buf[h * lcd_stride + 2 * w + 1] = 0x00;
            } else {
                lcd_buf[h * lcd_stride + 2 * w] = 0x00;
                lcd_buf[h * lcd_stride + 2 * w + 1] = 0x1f;
            }
        }
    }
    psram_flush_invaild_cache(lcd_buf, lcd_buf_size);

    //向左滑屏
    for (w = 0; w < lcd_w; w++) {
        lcd_draw_area_stride(lcd_buf, 0, 0, lcd_w, lcd_h, lcd_stride, w, 0, true);
        wdt_clear();
    }
    //向右滑屏
    for (w = lcd_w - 1; w >= 0; w--) {
        lcd_draw_area_stride(lcd_buf, 0, 0, lcd_w, lcd_h, lcd_stride, w, 0, true);
        wdt_clear();
    }

    lcd_wait();//dma非阻塞方式推屏, 退出需要待屏幕推完

    free_psram(lcd_buf);
}


void lcd_draw_vert_slide_test()//上下滑动
{
    int lcd_w;
    int lcd_h;
    int lcd_stride;
    u8 bytes_per_pixel;
    u8 *p;
    int w, h;

    ASSERT(__this);
    lcd_w = __this->lcd_width;
    lcd_h = __this->lcd_height;
    bytes_per_pixel = (__this->in_format == FORMAT_RGB888) ? 3 : 2;
    lcd_stride = (lcd_w * bytes_per_pixel + 3) / 4 * 4;

    u32 lcd_buf_size = lcd_stride * lcd_h * 2;
    u8 *lcd_buf = (u8 *)malloc_psram(lcd_buf_size);
    ASSERT(lcd_buf);

    for (h = 0; h < lcd_h * 2; h++) {
        for (w = 0; w < lcd_w; w++) {
            if (h < lcd_h) {
                lcd_buf[h * lcd_stride + 2 * w] = 0xf8;
                lcd_buf[h * lcd_stride + 2 * w + 1] = 0x00;
            } else {
                lcd_buf[h * lcd_stride + 2 * w] = 0x00;
                lcd_buf[h * lcd_stride + 2 * w + 1] = 0x1f;
            }
        }
    }
    psram_flush_invaild_cache(lcd_buf, lcd_buf_size);

    //向下滑屏
    for (h = 0; h < lcd_h; h++) {
        lcd_draw_area_stride(lcd_buf, 0, 0, lcd_w, lcd_h, lcd_stride, 0, h, true);
        wdt_clear();
    }
    //向上滑屏
    for (h = lcd_h - 1; h >= 0; h--) {
        lcd_draw_area_stride(lcd_buf, 0, 0, lcd_w, lcd_h, lcd_stride, 0, h, true);
        wdt_clear();
    }

    lcd_wait();//dma非阻塞方式推屏, 退出需要待屏幕推完

    free_psram(lcd_buf);
}
#endif




/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD DEVICE MATCH
 *
 * Description: LCD 设备初始化
 *
 * Arguments  : 匹配模式， 屏驱LOGO
 *
 * Returns    : 屏设备句柄
 *
 * Notes      : 1、当只有一个屏驱时，不进行匹配，直接返回该屏驱句柄
 *
 *              2、两种匹配方式 LOGO 或者 ID
 *
 *              3、若存在屏设备句柄时直接返回
 *********************************************************************************************************
 */
struct lcd_drive *lcd_drv_get_hdl(u8 mode, const char *logo)
{
    if (__lcd) {
        return __lcd;
    } else {
        if (mode == LCD_MATCH_BY_LOGO) {
            return lcd_drv_get_hdl_by_logo(logo);
        } else {
            return lcd_drv_get_hdl_by_id();
        }
    }
}


/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD DEVICE INIT
 *
 * Description: LCD 设备初始化
 *
 * Arguments  : *p 板级配置的 LCD SPI 信息
 *
 * Returns    : 0 初始化成功
 * 				-1 初始化失败
 *
 * Notes      : 1、判断是否在板级文件配置SPI，是继续，否进入断言，
 *
 *              2、配置SPI可操作IO给IMD操作
 *
 *              3、LCD设备复位
 *
 *              4、SPI模块初始化，IMD模块初始化
 *********************************************************************************************************
 */
int lcd_drv_init(void *p)
{
    printf("lcd_drv_init ...\n");

    int err = 0;
    struct ui_devices_cfg *cfg = (struct ui_devices_cfg *)p;
    /* lcd_dat = (struct lcd_platform_data *)cfg->private_data; */
    memcpy(lcd_dat, cfg->private_data, sizeof(struct lcd_platform_data));

    ASSERT(lcd_dat, "Error! spi io not config");
    
    printf("spi pin rest:%d, cs:%d, dc:%d, en:%d, spi:%d\n", \
        lcd_dat->pin_reset, lcd_dat->pin_cs, lcd_dat->pin_dc, \
            lcd_dat->pin_en, lcd_dat->spi_cfg);
    
    //可通过屏幕logo或者屏幕id匹配屏驱，任选一种匹配方式
    if (!__lcd) {
        __lcd = lcd_drv_get_hdl(TCFG_LCD_MATCH_MODE, LCD_LOGO);
    }
    if (!__this) {
        __this = __lcd->param;
    }

    ASSERT(__lcd, ", don't find lcd_device");
    ASSERT(__this, ", don't find imd_param");

    /* 如果有使能IO，设置使能IO输出高电平 */
    lcd_en_ctrl(true);

#if 1
    /* 把 CS、DC IO 的控制配置到IMD */
    lcd_set_ctrl_pin_func(spi_dc_ctrl, spi_cs_ctrl, spi_te_stat);

    if (__lcd->row_addr_align && __lcd->column_addr_align) {
        lcd_set_align(__lcd->row_addr_align, __lcd->column_addr_align);
    } else {
        lcd_set_align(1, 1);
    }

    __this->pap.wr_sel = lcd_dat->mcu_pins.pin_wr;
    __this->pap.rd_sel = lcd_dat->mcu_pins.pin_rd;
    __this->rgb.hsync_sel = lcd_dat->rgb_pins.pin_hsync;
    __this->rgb.vsync_sel = lcd_dat->rgb_pins.pin_vsync;
    __this->rgb.den_sel = lcd_dat->rgb_pins.pin_den;

    lcd_reset(__lcd); /* lcd复位 */

    /* 初始化imd和硬件SPI等 */
    lcd_init(__this);

    //qspi接口支持24位命令
    /* lcd_set_param(CMD_MODE, CMD_24BIT); */

    /* SPI发送屏幕初始化代码 */
    lcd_drv_cmd_list(__lcd->lcd_cmd, __lcd->cmd_cnt);

    printf("****************lcd_drv_cmd_list end\n");

#if 0
    if (lcd_qspi_st77903_mode()) 
    {
        printf("-------------------%s\n", __func__);

        ASSERT(CONFIG_CPU_UNMASK_IRQ_ENABLE == 1, ", please enable unmask irq.");

        /* lcd_qspi_st77903_refresh_mode_set(0); */
        lcd_qspi_st77903_refresh_mode_set(1);
        /* lcd_qspi_st77903_refresh_mode_set(2); */

        lcd_qspi_st77903_idle_line_set(10); //帧间空闲时间(以line为单位)

        os_sem_create(&lcd_sem, 0);

        int err = task_create(lcd_task, NULL, "lcd");
        ASSERT(!err);

        if (st77903_frame_mode == SINGAL_FRAME_MODE) {
            lcd_qspi_st77903_frame_mode_set(FRAME_SINGAL);

            lcd_buf_malloc();

            os_sem_post(&lcd_sem);
            __lcd->lcd_enter = 1;
            __lcd->lcd_exit = 0;

            timer4_init();
        } else {
            lcd_qspi_st77903_frame_mode_set(FRAME_CONTINUE);

            int lcd_w;
            int lcd_h;
            int lcd_stride;
            lcd_buffer_info_get(&lcd_w, &lcd_h, &lcd_stride);
            lcd_buf_malloc();
            if (__lcd->lcd_buf) {
                lcd_qspi_st77903_start(__lcd->lcd_dbuf[__lcd->lcd_dbuf_index], lcd_w, lcd_h, lcd_stride, 8, 8);
            }
        }
    }
#endif

#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
    //psram 双buffer滑动截取显示demo
    /* lcd_draw_hori_slide_test(); */
    /* lcd_draw_vert_slide_test(); */
#endif

    if (check_ram1_size()) {
        imb_set_memory_func(malloc_vlt, free_vlt);
    }

    lcd_mcpwm_init();

#endif

    return 0;
}


/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       GET LCD DEVICE INFO
 *
 * Description: 获取 LCD 设备信息
 *
 * Arguments  : *info LCD 设备信息缓存结构体，根据结构体内容赋值即可
 *
 * Returns    : 0 获取成功
 * 				-1 获取失败
 *
 * Notes      : 1、根据参数结构体的内容，将LCD对应信息赋值给结构体元素
 *********************************************************************************************************
 */

static int lcd_drv_get_screen_info(struct lcd_info *info)
{
    /* imb的宽高 */
    ASSERT(__this);
    info->width = __this->in_width;
    info->height = __this->in_height;

    /* imb的输出格式 */
    info->color_format = __this->in_format;//OUTPUT_FORMAT_RGB565;
    if (info->color_format == OUTPUT_FORMAT_RGB565) {
        info->stride = (info->width * 2 + 3) / 4 * 4;
    } else if (info->color_format == OUTPUT_FORMAT_RGB888) {
        info->stride = (info->width * 3 + 3) / 4 * 4;
    }

    /* 屏幕类型 */
    info->interface = __this->lcd_type;

    /* 对齐 */
    info->col_align = __lcd->column_addr_align;
    info->row_align = __lcd->row_addr_align;
    if (!info->col_align) {
        info->col_align = 1;
    }
    if (!info->row_align) {
        info->row_align = 1;
    }

    /* 背光状态 */
    info->bl_status = !!backlight_status;
    info->buf_num = __this->buffer_num;

    info->buffer = __this->buffer;
    info->buffer_size = __this->buffer_total_size;

    ASSERT(info->col_align, " = 0, lcd driver column address align error, default value is 1");
    ASSERT(info->row_align, " = 0, lcd driver row address align error, default value is 1");

    return 0;
}


/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       MALLOC DISPLAY BUFFER
 *
 * Description: 申请 LCD 显存 buffer
 *
 * Arguments  : **buf 保存显存buffer指针
 * 				*size 保存显存buffer大小
 *
 * Returns    : 0 成功
 * 				-1 失败
 *
 * Notes      : 1、根据LCD驱动中配置的显存大小和数量申请显存BUFFER
 *
 *				2、将显存buffer指针赋值给参数**buf，显存buffer大小赋值给参数*size
 *
 *				注意：buffer默认是lock状态，此时不能推屏，需由UI框架获取并写入数据后才能推屏
 *********************************************************************************************************
 */

static int lcd_drv_buffer_malloc(u8 **buf, u32 *size)
{
    /* int buf_size = ((__this->lcd_width * __this->lcd_height * 2) + 3) / 4 * 4; */
    ASSERT(__this);
    int buf_size = (__this->buffer_size + 3) / 4 * 4;	// 把buffer大小做四字节对齐

    if (check_ram1_size() == 0) {
        *buf = (u8 *)malloc(buf_size * __this->buffer_num);
    } else {
        *buf = (u8 *)malloc_vlt(buf_size * __this->buffer_num);
    }

    /* *buf = (u8 *)malloc(__this->buffer_size * __this->buffer_num); */

    if (!buf) {
        // 如果buffer申请失败
        *buf = NULL;
        *size = 0;
        return -1;
    }

    /* *size = __this->buffer_size; */
    *size = buf_size * __this->buffer_num;
    __this->buffer = *buf;
    __this->buffer_total_size = *size;

    return 0;
}



/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       FREE DISPLAY BUFFER
 *
 * Description: 释放 LCD 显存 buffer
 *
 * Arguments  : *buf 显存buffer指针
 *
 * Returns    : 0 成功
 * 				-1 失败
 *
 * Notes      : 1、使用memory API 释放显存buffer
 *********************************************************************************************************
 */

static int lcd_drv_buffer_free(u8 *buf)
{
    if (buf) {
        if (check_ram1_size() == 0) {
            free(buf);
        } else {
            free_vlt(buf);
        }
        buf = NULL;
    }

    return 0;
}


/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD DRAW BUFFER
 *
 * Description: 把显存 buf 推送到屏幕
 *
 * Arguments  : *buf 显存buffer指针
 * 				len 显存buffer的数据量
 * 				wait 是否等待
 *
 * Returns    : 0 成功
 * 				-1 失败
 *
 * Notes      : 1、使用 IMD 模块将显存buffer推给屏幕
 *********************************************************************************************************
 */

static int lcd_drv_draw(u8 *buf, u32 len, u8 wait)
{
    lcd_draw(LCD_DATA_MODE, (u32)buf);
    return 0;
}


/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD DRAW PAGE
 *
 * Description: 将UI页面buffer推给屏幕
 *
 * Arguments  : *buf 页面缓存buffer指针
 *
 * Returns    : 0 成功
 * 				-1 失败
 *
 * Notes      :
 *********************************************************************************************************
 */

static int lcd_draw_page(u8 *buf, u8 page_star, u8 page_len)
{
    // 绘制页面功能暂时没有
    return 0;
}

/*
 *********************************************************************************************************
 *                                       GET LCD BACKLIGHT STATUS
 *
 * Description: 获取 LCD sleep状态
 *
 * Arguments  : none
 *
 * Returns    : 0 sleep out
 * 				1 sleep in
 *
 * Notes      :
 *********************************************************************************************************
 */

int lcd_sleep_status()
{
    return lcd_sleep_in;
}

// 触摸屏sleep处理
__attribute__((weak)) void ctp_enter_sleep(void)
{
}
__attribute__((weak)) void ctp_exit_sleep(void)
{
}

static void lcd_st77903_stop(void)
{
    if (lcd_qspi_st77903_mode()) 
    {
        printf("------------%s\n", __func__);
        if (st77903_frame_mode == SINGAL_FRAME_MODE) {
            __lcd->lcd_exit = 1;
            while (__lcd->lcd_exit) {
                os_time_dly(1);
            }

            timer4_uninit();

            lcd_buf_free();
        } else {
            lcd_qspi_st77903_stop();
            lcd_buf_free();
        }
    }
}

void lcd_st77903_singal_to_continue()
{
    if (lcd_qspi_st77903_mode()) {
        if (st77903_frame_mode == SINGAL_FRAME_MODE) {
            __lcd->lcd_switch_mode = 1;
        }
    }
}

int lcd_st77903_mode_switch(int mode)
{
    if (lcd_qspi_st77903_mode()) {
        if (mode == st77903_frame_mode) {
            return -1;
        }

        if (mode > CONTINUE_FRAME_MODE) {
            return -2;
        }

        printf("st77903 mode switch, curr_mode : %d, set_mode : %d\n", st77903_frame_mode, mode);

        int lcd_w;
        int lcd_h;
        int lcd_stride;
        lcd_buffer_info_get(&lcd_w, &lcd_h, &lcd_stride);

        if (mode == SINGAL_FRAME_MODE) { //连续帧 -> 单帧
            //停止连续帧
            lcd_qspi_st77903_stop();
            //切换为单帧模式
            lcd_qspi_st77903_frame_mode_set(mode);
            //启动单帧
            __lcd->lcd_enter = 1;
            os_sem_post(&lcd_sem);
            while (__lcd->lcd_enter) {
                os_time_dly(1);
            }
            timer4_init();
        } else {//单帧 -> 连续帧
            //停止单帧
            __lcd->lcd_exit = 1;
            while (__lcd->lcd_exit) {
                os_time_dly(1);
            }
            timer4_uninit();
            //切换为连续帧模式
            lcd_qspi_st77903_frame_mode_set(mode);
            //启动连续帧
            lcd_qspi_st77903_start(__lcd->lcd_dbuf[__lcd->lcd_dbuf_index], lcd_w, lcd_h, lcd_stride, 8, 8);
        }
        st77903_frame_mode = mode;
        return 0;
    } else {
        return -1;
    }
}

/*
 * 弱函数重定义(升级复位前停止刷屏)
 * */
void update_process_before_cpu_reset(void)
{
    if (lcd_sleep_in) {

    } else {
        lcd_st77903_stop();
    }
}

static void lcd_st77903_start(void)
{
    if (lcd_qspi_st77903_mode()) 
    {
        if (st77903_frame_mode == SINGAL_FRAME_MODE) 
        {
            lcd_buf_malloc();

            __lcd->lcd_enter = 1;
            os_sem_post(&lcd_sem);

            while (__lcd->lcd_enter) {
                os_time_dly(1);
            }

            timer4_init();
        }else 
        {
            int lcd_w;
            int lcd_h;
            int lcd_stride;
            lcd_buffer_info_get(&lcd_w, &lcd_h, &lcd_stride);

            lcd_buf_malloc();
            if (__lcd->lcd_buf) {
                lcd_qspi_st77903_start(__lcd->lcd_dbuf[__lcd->lcd_dbuf_index], lcd_w, lcd_h, lcd_stride, 8, 8);
            }
        }
    }
}


#if LCD_POWER_DOWN_EN
static void lcd_drv_power_off()
{
    /* lcd断电 */
    printf("lcd power off\n");
    lcd_drv_power_ctrl(false);
    struct lcd_platform_data *lcd_dat = lcd_get_platform_data();
    if (lcd_dat->pin_te != NO_CONFIG_PORT) {
        gpio_set_pull_up(lcd_dat->pin_te, 0);
        gpio_set_pull_down(lcd_dat->pin_te, 0);
        gpio_direction_input(lcd_dat->pin_te);
        gpio_set_die(lcd_dat->pin_te, 0);
    }

    if (lcd_dat->pin_cs != NO_CONFIG_PORT) {
        gpio_set_pull_up(lcd_dat->pin_cs, 0);
        gpio_set_pull_down(lcd_dat->pin_cs, 0);
        gpio_direction_output(lcd_dat->pin_cs, 0);
    }
}

static void lcd_init_task(void *p)
{
    /* lcd上电 */
    lcd_drv_power_ctrl(true);

    /* lcd复位 */
    lcd_reset(__lcd);

    /* SPI发送屏幕初始化代码 */
    lcd_drv_cmd_list(__lcd->lcd_cmd, __lcd->cmd_cnt);

    os_sem_post(&__lcd->init_sem);

    while (1) {
        os_time_dly(100);
    }
}

static void lcd_drv_power_on()
{
    int err;
    __lcd->lcd_reinit = 1;
    err = task_create(lcd_init_task, NULL, "lcd_init");
    ASSERT(err == 0);
}
#endif

#if TP_POWER_DOWN_EN
extern void ctp_poweron(void);
extern void ctp_poweroff(void);

static void tp_drv_power_off()
{
    /* TP掉电 */
    ctp_poweroff();
}

static void tp_init_task(void *p)
{
    /* TP上电 */
    ctp_poweron();

#if TCFG_TP_BL6133_ENABLE
    extern void bl6133_test();
    extern void bl6133_int_en();

    /* TP初始化 */
    bl6133_test();

    /* TP int中断使能 */
    bl6133_int_en();
#endif

    os_sem_post(&__lcd->init_sem);
    while (1) {
        os_time_dly(100);
    }
}

static void tp_drv_power_on()
{
    int err;
    __lcd->tp_reinit = 1;
    err = task_create(tp_init_task, NULL, "tp_init");
    ASSERT(err == 0);
}
#endif

/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       LCD SLEEP CONTROL
 *
 * Description: LCD 休眠控制
 *
 * Arguments  : enter 是否进入休眠，true 进入休眠，false 退出休眠
 *
 * Returns    : 0 成功
 * 				-1 失败
 *
 * Notes      : 1、判断 LCD 是否正在使用，是等待使用结束，否进入下一步
 *
 * 				2、enter是否进入休眠，是使用LCD休眠函数进入休眠状态，否使用LCD退出休眠函数退出休眠状态
 *
 * 				3、lcd_sleep_in 记录LCD的休眠状态
 *********************************************************************************************************
 */
int lcd_sleep_ctrl(u8 enter)
{
    //struct lcd_sleep_headler *p;
    if((!!enter) == lcd_sleep_in) {
        return -1;
    }

    while(is_lcd_busy);

    is_lcd_busy = 0x11;

    if (enter) 
    {
        //ui_effect_exit();
#if TCFG_TP_SLEEP_EN
        //ctp_enter_sleep();
        extern void chsc6x_suspend(void);
        chsc6x_suspend();
#endif /* #if TCFG_TP_SLEEP_EN */
#if TP_POWER_DOWN_EN
        tp_drv_power_off();
#endif
        //lcd_st77903_stop();

        lcd_drv_backlight_ctrl_base(0);

#if LCD_POWER_DOWN_EN == 0
        if (__lcd->entersleep) {
            __lcd->entersleep();
            lcd_sleep_in = true;
        }
#else
        if (__lcd->entersleep) {
            __lcd->entersleep();
        }
        lcd_drv_power_off();
        lcd_sleep_in = true;
#endif
        clock_remove_set(DEC_UI_CLK);
    } else 
    {
        clock_add_set(DEC_UI_CLK);

        norflash_flash_power_check();
#if LCD_POWER_DOWN_EN || TP_POWER_DOWN_EN
        os_sem_create(&__lcd->init_sem, 0);
#endif

#if TP_POWER_DOWN_EN
        tp_drv_power_on();
#endif
#if TCFG_TP_SLEEP_EN
        //ctp_exit_sleep();
        extern void chsc6x_resume(void);
        chsc6x_resume();
#endif /* #if TCFG_TP_SLEEP_EN */

#if LCD_POWER_DOWN_EN == 0
        if (__lcd->exitsleep) {
            __lcd->exitsleep();
            lcd_sleep_in = false;
        }
#else
        lcd_drv_power_on();
        lcd_sleep_in = false;
#endif
    }

    is_lcd_busy = 0;

    return 0;
}

/*********************************************************************************************************
 *                                       LCD POWER CONTROL
 *
 * Description: LCD 关机控制,关机使用
 *
 *
 * Returns    : 0 成功
 * 				-1 失败
 *
 *********************************************************************************************************/



#if 0
int lcd_poweroff()
{
    if (lcd_sleep_in) {
        return -1;
    }

    if (is_lcd_busy) {
        return -1;
    }

    is_lcd_busy = 0x11;
    lcd_drv_backlight_ctrl_base(0);
#if TCFG_TP_SLEEP_EN
    ctp_enter_sleep();
#endif /* #if TCFG_TP_SLEEP_EN */
    if (__lcd->entersleep) {
        __lcd->entersleep();
    }
    lcd_drv_entersleep();
    lcd_sleep_in = true;

    lcd_en_ctrl(0);
    is_lcd_busy = 0;
    return 0;
}
#endif

/*$PAGE*/
/*
 *********************************************************************************************************
 *                                       GET LCD DRIVE HANDLER
 *
 * Description: 获取LCD驱动句柄
 *
 * Arguments  : none
 *
 * Returns    : struct lcd_interface* LCD驱动接口句柄
 *
 * Notes      : 1、从LCD接口列表中找到LCD接口句柄并返回
 *********************************************************************************************************
 */

struct lcd_interface *lcd_get_hdl()
{
    struct lcd_interface *p;

    ASSERT(lcd_interface_begin != lcd_interface_end, "don't find lcd interface!");
    for (p = lcd_interface_begin; p < lcd_interface_end; p++) {
        return p;
    }
    return NULL;
}

static void lcd_drv_set_draw_area(u16 xs, u16 xe, u16 ys, u16 ye)
{
    lcd_set_draw_area(xs, xe, ys, ye);
}

static void lcd_drv_clear_screen(u32 color)
{
    lcd_full_clear(color);
}

struct lcd_drive *lcd_drv_get_hdl_by_logo(const char *logo)
{
    struct lcd_drive *p;
    int lcd_type;
    int spi_mode;
    int spi_submode;
    int lcd_num = 0;

    printf("find logo %s\n", logo);
    ASSERT(lcd_device_begin != lcd_device_end, "don't find lcd device!");

    //统计屏驱的个数
    for (p = lcd_device_begin; p < lcd_device_end; p++) {
        lcd_num++;
    }

    //只有一个屏驱时不进行匹配
    if (lcd_num == 1) {
        p = lcd_device_begin;
        printf("Due to lcd_num = %d, don't match any lcd device. Only one lcd device %s is selected by default.\n", lcd_num, p->logo ? p->logo : "null");
        return p;
    }

    //需确保所有使能的屏驱接口保持一致
    for (p = lcd_device_begin; p < lcd_device_end; p++) {
        if (p == lcd_device_begin) {
            lcd_type = ((struct imd_param *)p->param)->lcd_type;
            spi_mode = ((struct imd_param *)p->param)->spi.spi_mode & 0xf0;
            spi_submode = ((struct imd_param *)p->param)->spi.spi_mode & 0x0f;
        } else {
            ASSERT(lcd_type == ((struct imd_param *)p->param)->lcd_type, ", all lcd interface must the same");
            ASSERT(spi_mode == (((struct imd_param *)p->param)->spi.spi_mode & 0xf0), ", all spi_mode must the same");
            ASSERT(spi_submode == (((struct imd_param *)p->param)->spi.spi_mode & 0x0f), ", all spi_submode must the same");
        }
    }

    for (p = lcd_device_begin; p < lcd_device_end; p++) {
        printf("p->logo : %s\n", p->logo);
        if (p->logo && logo && !strcmp(p->logo, logo)) {
            return p;
        }
    }
    return NULL;
}

struct lcd_drive *lcd_drv_get_hdl_by_id()
{
    struct lcd_drive *p;
    int lcd_type;
    int spi_mode;
    int spi_submode;
    int lcd_num = 0;

    ASSERT(lcd_device_begin != lcd_device_end, "don't find lcd device!");

    //统计屏驱的个数
    for (p = lcd_device_begin; p < lcd_device_end; p++) {
        lcd_num++;
    }

    //只有一个屏驱时不进行匹配
    if (lcd_num == 1) {
        p = lcd_device_begin;
        printf("Due to lcd_num = %d, don't match any lcd device. Only one lcd device %s is selected by default.\n", lcd_num, p->logo ? p->logo : "null");
        return p;
    }


    //需确保所有使能的屏驱接口保持一致
    for (p = lcd_device_begin; p < lcd_device_end; p++) {
        if (p == lcd_device_begin) {
            lcd_type = ((struct imd_param *)p->param)->lcd_type;
            spi_mode = ((struct imd_param *)p->param)->spi.spi_mode & 0xf0;
            spi_submode = ((struct imd_param *)p->param)->spi.spi_mode & 0x0f;
        } else {
            ASSERT(lcd_type == ((struct imd_param *)p->param)->lcd_type, ", all lcd interface must the same");
            ASSERT(spi_mode == (((struct imd_param *)p->param)->spi.spi_mode & 0xf0), ", all spi_mode must the same");
            ASSERT(spi_submode == (((struct imd_param *)p->param)->spi.spi_mode & 0x0f), ", all spi_submode must the same");
        }
    }


    for (p = lcd_device_begin; p < lcd_device_end; p++) {
        struct imd_param *this = (struct imd_param *)p->param;
        this->pap.wr_sel = lcd_dat->mcu_pins.pin_wr;
        this->pap.rd_sel = lcd_dat->mcu_pins.pin_rd;
        this->rgb.hsync_sel = lcd_dat->rgb_pins.pin_hsync;
        this->rgb.vsync_sel = lcd_dat->rgb_pins.pin_vsync;
        this->rgb.den_sel = lcd_dat->rgb_pins.pin_den;

        extern volatile struct imd_variable imd_var;
        imd_var.clock_init = false;

        lcd_en_ctrl(true);

        lcd_set_ctrl_pin_func(spi_dc_ctrl, spi_cs_ctrl, spi_te_stat);

        lcd_reset(p); /* lcd复位 */
        lcd_init(this); /* 初始化lcd控制器 */

        u32 lcd_read_id = 0;
        if (p->read_id) {
            lcd_read_id = p->read_id();
        }

        printf("p->logo : %s, p->lcd_id : 0x%x, read_id : 0x%x\n", p->logo ? p->logo : "null", p->lcd_id, lcd_read_id);
        if (p->lcd_id == lcd_read_id) {
            return p;
        }
    }

    return NULL;
}


int lcd_drv_get_info(void *info)
{
    struct lcd_interface *lcd;
    lcd = lcd_get_hdl();
    ASSERT(lcd);
    if (lcd->get_screen_info) {
        lcd->get_screen_info(info);
    }

    return 0;
}

REGISTER_LCD_INTERFACE(lcd) = {
    .init               = lcd_drv_init,
    .draw               = lcd_drv_draw,
    .get_screen_info	= lcd_drv_get_screen_info,
    .buffer_malloc		= lcd_drv_buffer_malloc,
    .buffer_free		= lcd_drv_buffer_free,
    .backlight_ctrl		= lcd_drv_backlight_ctrl,
    .power_ctrl		    = lcd_drv_power_ctrl,
    .set_draw_area	    = lcd_drv_set_draw_area,
    .clear_screen	    = lcd_drv_clear_screen,
};


static u8 lcd_idle_query(void)
{
    return !is_lcd_busy;
}

REGISTER_LP_TARGET(lcd_lp_target) = {
    .name = "lcd",
    .is_idle = lcd_idle_query,
};



