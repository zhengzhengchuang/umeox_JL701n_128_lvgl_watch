/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"


/*
** 驱动代码的宏开关
*/
//<<<[4-fire屏 240x240]>>>//
#if defined(TCFG_LCD_GC9307_240X283) && TCFG_LCD_GC9307_240X283

#define LCD_DRIVE_CONFIG				SPI_4WIRE_RGB565_1T8B//	SPI_4WIRE_RGB565_1T8B


/*
** 包含imd头文件，屏驱相关的变量和结构体都定义在imd.h
** 包含imb头文件，imb输出格式定义在imb.h
*/
#include "asm/imd.h"
#include "asm/imb.h"
#include "includes.h"
#include "ui/ui_api.h"



#define SCR_X 0
#define SCR_Y 0
#define SCR_W 240
#define SCR_H 283//210
#define LCD_W 240
#define LCD_H 283//210
#define LCD_BLOCK_W 240
#define LCD_BLOCK_H 30//105
#define BUF_NUM 2

#if 0
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

/*
** 初始化代码
*/
static const u8 lcd_cmd_t_nv9307[] ALIGNED(4) = {
    // _BEGIN_, 0x01, _END_,				// soft reset
    // _BEGIN_, REGFLAG_DELAY, 120, _END_,	// delay 120ms
    // _BEGIN_, 0x11, _END_,				// sleep out
    // _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0xfe, _END_,
    _BEGIN_, 0xef, _END_,
    _BEGIN_, 0x36, 0x08, _END_,
    _BEGIN_, 0x3a, 0x05, _END_,

    _BEGIN_, 0x84, 0x40, _END_,
    _BEGIN_, 0x86, 0x98, _END_,
    _BEGIN_, 0x89, 0x13, _END_,
    _BEGIN_, 0x8b, 0x80, _END_,
    _BEGIN_, 0x8d, 0x33, _END_,
    _BEGIN_, 0x8e, 0x0f, _END_,
    //_BEGIN_, 0xec, 0x33, 0x02, 0x33, _END_,

    _BEGIN_, 0xc9, 0x2d, _END_,
    _BEGIN_, 0xe8, 0x12, 0x13, _END_,
    _BEGIN_, 0xb6, 0x00, 0x36, _END_,
    _BEGIN_, 0xc3, 0x16, _END_,
    _BEGIN_, 0xc4, 0x15, _END_,

    _BEGIN_, 0xff, 0x62, _END_,
    _BEGIN_, 0x99, 0x3e, _END_,
    _BEGIN_, 0x9d, 0x4b, _END_,

    _BEGIN_, 0x98, 0x3e, _END_,
    _BEGIN_, 0x9c, 0x4b, _END_,

    _BEGIN_, 0xF0, 0x55, 0x16, 0x0e, 0x09, 0x06, 0x3c, _END_,
    _BEGIN_, 0xF2, 0x55, 0x16, 0x0e, 0x09, 0x06, 0x3c, _END_,
    _BEGIN_, 0xF1, 0x4f, 0x8F, 0x8F, 0x36, 0x39, 0x5F, _END_,
    _BEGIN_, 0xF3, 0x4f, 0x8F, 0x8F, 0x36, 0x39, 0x5F, _END_,

    _BEGIN_, 0x35, 0x00, _END_,
    _BEGIN_, 0x44, 0x00, 0x30, _END_,

    _BEGIN_, 0X11, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,	// delay 120ms
    _BEGIN_, 0x29, _END_,
    _BEGIN_, 0x2c, _END_,
    _BEGIN_, REGFLAG_DELAY, 10, _END_,	// delay 120ms

};



// 配置参数
struct imd_param param_t = {
    .scr_x    = SCR_X,
    .scr_y	  = SCR_Y,
    .scr_w	  = SCR_W,
    .scr_h	  = SCR_H,
// 1
    // imb配置相关
    .in_width  = SCR_W,
    .in_height = SCR_H,
    .in_format = OUTPUT_FORMAT_RGB565,//-1,

    // 屏幕相关
    .lcd_width  = LCD_W,
    .lcd_height = LCD_H,
    .lcd_type   = LCD_TYPE_SPI,

    .buffer_num  = BUF_NUM,
    .buffer_size = LCD_BLOCK_W * LCD_BLOCK_H * 2,
    .fps = 60,
    // clk = 240*280*2*2*60 = 16128000Hz

    .spi = {
        .spi_mode	= SPI_IF_MODE(LCD_DRIVE_CONFIG),
        .pixel_type = PIXEL_TYPE(LCD_DRIVE_CONFIG),
        .out_format = OUT_FORMAT(LCD_DRIVE_CONFIG),
        .port		= SPI_PORTA,
        .spi_dat_mode = SPI_MODE_BIDIR,
    },

    .debug_mode_en = false,
    .debug_mode_color = 0xffff00,

    .te_en = false,
};


/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_backlight_ctrl(u8 onoff)
{
    lcd_bl_ctrl(onoff);
    return 0;
}


/*
** 设置lcd进入睡眠
*/
static void lcd_entersleep(void)
{
    r_printf("LCD进入睡眠..................");



    lcd_write_cmd(0x28, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(50 / 2);	// delay 120ms
#if 0

    // 把TE设置成高阻态，降低功耗
    extern struct lcd_platform_data *lcd_get_platform_data();
    struct lcd_platform_data *lcd_dat = lcd_get_platform_data();
    if (lcd_dat->pin_te != NO_CONFIG_PORT) {
        gpio_set_pull_up(lcd_dat->pin_te, 0);
        gpio_set_pull_down(lcd_dat->pin_te, 0);
        gpio_direction_input(lcd_dat->pin_te);
        gpio_set_die(lcd_dat->pin_te, 0);
    }
#endif
}

/*
** 设置lcd退出睡眠
*/
static void lcd_exitsleep(void)
{
    r_printf("LCD进入工作..................");

    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
    lcd_write_cmd(0x29, NULL, 0);
}




REGISTER_LCD_DEVICE(nv9307) = {
    .logo = "nv9307",
    .row_addr_align		= 1,
    .column_addr_align	= 1,

    .lcd_cmd = (void *) &lcd_cmd_t_nv9307,
    .cmd_cnt = sizeof(lcd_cmd_t_nv9307) / sizeof(lcd_cmd_t_nv9307[0]),
    .param   = (void *) &param_t,

    .reset			= NULL,	// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl = NULL,//lcd_backlight_ctrl,
    .entersleep     = lcd_entersleep,
    .exitsleep      = lcd_exitsleep,
};


#endif




