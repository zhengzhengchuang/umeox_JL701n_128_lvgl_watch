/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"


/*
** 驱动代码的宏开关
*/
//<<<[4-fire屏 240x240]>>>//
#if TCFG_LCD_SPI_ST7789_BOE1_54_ENABLE

/* #define LCD_DRIVE_CONFIG					SPI_3WIRE_RGB565_1T8B */
#define LCD_DRIVE_CONFIG					DSPI_3WIRE_RGB666_1T9B

/*
** 包含imd头文件，屏驱相关的变量和结构体都定义在imd.h
** 包含imb头文件，imb输出格式定义在imb.h
*/
#include "asm/imd.h"
#include "asm/imb.h"
#include "includes.h"
#include "ui/ui_api.h"


#ifdef CONFIG_CPU_BR28
extern void mdelay(u32 ms);
#define lcd_dly_2ms(x)	mdelay((x)*2)
#else /* #ifdef CONFIG_CPU_BR28 */
#define lcd_dly_2ms		delay_2ms
#endif /* #ifdef CONFIG_CPU_BR28 */


#define SCR_X 0
#define SCR_Y 0
#define SCR_W 240
#define SCR_H 240
#define LCD_W 240
#define LCD_H 240
#define LCD_BLOCK_W 240
#define LCD_BLOCK_H 60
#define BUF_NUM 2

/*
** 初始化代码
*/
static const u8 lcd_spi_st7789_cmdlist[] ALIGNED(4) = {
    _BEGIN_, 0x01, _END_,				// soft reset
    _BEGIN_, REGFLAG_DELAY, 120, _END_,	// delay 120ms
    _BEGIN_, 0x11, _END_,				// sleep out
    _BEGIN_, REGFLAG_DELAY, 120, _END_,

    _BEGIN_, 0x35, 0x00, _END_, //te
    _BEGIN_, 0x36, 0x00, _END_,

    _BEGIN_, 0x3A, 0x06, _END_,

    _BEGIN_, 0xB2, 0x0c, 0x0c, 0x00, 0x33, 0x33, _END_,

    _BEGIN_, 0xBB, 0x1A, _END_,
    _BEGIN_, 0xC0, 0x2C, _END_,
    _BEGIN_, 0xC2, 0x01, _END_,
    _BEGIN_, 0xC3, 0x0F, _END_,
    _BEGIN_, 0xC4, 0x20, _END_,
    _BEGIN_, 0xC6, 0x0F, _END_,
    _BEGIN_, 0xD0, 0xA4, 0xA1, _END_,
    _BEGIN_, 0xE0, 0xD0, 0x13, 0x1A, 0x0A, 0x0A, 0x26, 0x3F, 0x54, 0x54, 0x18, 0x14, 0x14, 0x30, 0x33, _END_,
    _BEGIN_, 0xE1, 0xD0, 0x13, 0x1A, 0x0A, 0x0A, 0x26, 0x3F, 0x54, 0x54, 0x1A, 0x16, 0x16, 0x32, 0x35, _END_,

#if LCD_DRIVE_CONFIG == DSPI_3WIRE_RGB666_1T9B
    _BEGIN_, 0xE7, 0x10, _END_,//spi2 enable
#endif

    _BEGIN_, 0X21, _END_,
    _BEGIN_, 0X2A, 0x00, 0x00, 0x00, 0xEF, _END_,
    _BEGIN_, 0X2B, 0x00, 0x00, 0x00, 0xEF, _END_,
    _BEGIN_, 0X29, _END_,
    _BEGIN_, REGFLAG_DELAY, 20, _END_,
    _BEGIN_, 0X2C, _END_,
    _BEGIN_, REGFLAG_DELAY, 20, _END_,
};





/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_spi_st7789_backlight_ctrl(u8 onoff)
{
    lcd_bl_ctrl(onoff);
    return 0;
}


/*
** 设置lcd进入睡眠
*/
static void lcd_spi_st7789_entersleep(void)
{
    lcd_write_cmd(0x28, NULL, 0);
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
}

/*
** 设置lcd退出睡眠
*/
static void lcd_spi_st7789_exitsleep(void)
{
    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
    lcd_write_cmd(0x29, NULL, 0);
}


// 配置参数
struct imd_param lcd_spi_st7789_param = {
    .scr_x    = SCR_X,
    .scr_y	  = SCR_Y,
    .scr_w	  = SCR_W,
    .scr_h	  = SCR_H,

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

    .spi = {
        .spi_mode = SPI_IF_MODE(LCD_DRIVE_CONFIG),
        .pixel_type = PIXEL_TYPE(LCD_DRIVE_CONFIG),
        .out_format = OUT_FORMAT(LCD_DRIVE_CONFIG),
        .port = SPI_PORTA,
        .spi_dat_mode = SPI_MODE_UNIDIR,
    },

    .debug_mode_en = false,
    .debug_mode_color = 0x0000ff,

    .te_en = false,
};


REGISTER_LCD_DEVICE(st7789) = {
    .logo = "st7789",
    .row_addr_align		= 1,
    .column_addr_align	= 1,

    .lcd_cmd = (void *) &lcd_spi_st7789_cmdlist,
    .cmd_cnt = sizeof(lcd_spi_st7789_cmdlist) / sizeof(lcd_spi_st7789_cmdlist[0]),
    .param   = (void *) &lcd_spi_st7789_param,

    .reset			= NULL,// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl = NULL,//lcd_spi_st7789_backlight_ctrl,
    .entersleep     = lcd_spi_st7789_entersleep,
    .exitsleep      = lcd_spi_st7789_exitsleep,
};


#endif




