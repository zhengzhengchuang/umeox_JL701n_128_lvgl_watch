/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"
#include "clock_cfg.h"


/*
** 驱动代码的宏开关
*/
//<<<[qspi屏 400x400]>>>//
#if TCFG_LCD_QSPI_ST77903_V2_ENABLE


#define LCD_DRIVE_CONFIG                    QSPI_RGB565_ST77903_1T2B

/*
** 包含imd头文件，屏驱相关的变量和结构体都定义在imd.h
*/
#include "asm/imd.h"
#include "asm/imb.h"
#include "includes.h"
#include "ui/ui_api.h"

#define SCR_X 0
#define SCR_Y 0
#define SCR_W 400
#define SCR_H 400
#define LCD_W 400
#define LCD_H 400
#define LCD_BLOCK_W 400
#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
#define LCD_BLOCK_H 40
#else
#define LCD_BLOCK_H 2
#endif

#define BUF_NUM 2

#define LCD_FORMAT OUTPUT_FORMAT_RGB565

/*
** 初始化代码
*/
static const u8 lcd_qspi_st77903_v2_cmd_list_poweron[] ALIGNED(4) = {
    _BEGIN_, 0xF0, 0xC3, _END_,
    _BEGIN_, 0xF0, 0x96, _END_,
    _BEGIN_, 0xED, 0xC3, _END_,
    _BEGIN_, 0xE4, 0x40, 0x0F, _END_,
    _BEGIN_, 0xE7, 0x83, _END_,
    _BEGIN_, 0xC3, 0x33, 0x02, 0x25, 0x04, _END_,
    _BEGIN_, 0xE5, 0xB2, 0xF5, 0xBD, 0x24, 0x22, 0x25, 0x10, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, _END_,
    _BEGIN_, 0xEC, 0x00, 0x55, 0x00, 0x00, 0x00, 0x49, 0x22, _END_,
    _BEGIN_, 0x36, 0x0C, _END_,
    _BEGIN_, 0xB2, 0x00, _END_,
    _BEGIN_, 0x3A, 0x05, _END_,
    _BEGIN_, 0xC5, 0x6B, _END_,
    _BEGIN_, 0xE0, 0x88, 0x0B, 0x10, 0x08, 0x07, 0x03, 0x2C, 0x33, 0x43, 0x08, 0x16, 0x16, 0x2A, 0x2E, _END_,
    _BEGIN_, 0xE1, 0x88, 0x0B, 0x10, 0x08, 0x06, 0x02, 0x2B, 0x32, 0x42, 0x09, 0x16, 0x15, 0x2A, 0x2E, _END_,
    _BEGIN_, 0xA4, 0xC0, 0x7B, _END_,
    _BEGIN_, 0xD9, 0x02, _END_,
    _BEGIN_, 0xB6, 0xC7, 0x31, _END_,
    _BEGIN_, 0xB3, 0x01, _END_,
    _BEGIN_, 0xC1, 0x77, 0x07, 0xC2, 0x07, _END_,
    _BEGIN_, 0xA5, 0x00, 0x00, 0x00, 0x00, 0x20, 0x16, 0x2A, 0x8A, 0x02, _END_,
    _BEGIN_, 0xBA, 0x0A, 0x5A, 0x23, 0x10, 0x25, 0x02, 0x00, _END_,
    _BEGIN_, 0xBB, 0x00, 0x27, 0x00, 0x29, 0x82, 0x87, 0x18, 0x00, _END_,
    _BEGIN_, 0xA6, 0x00, 0x00, 0x00, 0x00, 0x20, 0x16, 0x2A, 0x8A, 0x02, _END_,
    _BEGIN_, 0xBC, 0x00, 0x27, 0x00, 0x29, 0x82, 0x87, 0x18, 0x00, _END_,
    _BEGIN_, 0xBD, 0xA1, 0xB2, 0x2B, 0x1A, 0x56, 0x43, 0x34, 0x65, 0xFF, 0xFF, 0x0F, _END_,
    _BEGIN_, 0x21, _END_,
    _BEGIN_, 0x35, 0x00, _END_,
    _BEGIN_, 0x11, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x29, _END_,
    _BEGIN_, REGFLAG_DELAY, 100, _END_,

#if 0
    //bist mode
    _BEGIN_, 0xB0, 0xA5, _END_,
    _BEGIN_, 0xCC, 0x44, 0x00, 0x3F, 0x00, 0x0A, 0x0A, 0xEA, 0xEA, 0x03, _END_,
#endif
};



/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_qspi_st77903_v2_backlight_ctrl(u8 percent)
{
    if (percent) {

    } else {

    }

    return 0;
}


/*
** lcd电源控制
*/
static int lcd_qspi_st77903_v2_power_ctrl(u8 onoff)
{
    lcd_en_ctrl(onoff);
    return 0;
}



/*
** 设置lcd进入睡眠
*/
static void lcd_qspi_st77903_v2_entersleep(void)
{
    lcd_write_cmd(0x28, NULL, 0);
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
}



/*
** 设置lcd退出睡眠
*/
static void lcd_qspi_st77903_v2_exitsleep(void)
{
    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(5);	// delay 120ms
    lcd_write_cmd(0x29, NULL, 0);
}


struct imd_param lcd_qspi_st77903_v2_param = {
    .scr_x    = SCR_X,
    .scr_y	  = SCR_Y,
    .scr_w	  = SCR_W,
    .scr_h	  = SCR_H,

    .in_width  = SCR_W,
    .in_height = SCR_H,
    .in_format = LCD_FORMAT,

    .lcd_width  = LCD_W,
    .lcd_height = LCD_H,

    .lcd_type = LCD_TYPE_SPI,

    .buffer_num = BUF_NUM,
    .buffer_size = LCD_BLOCK_W * LCD_BLOCK_H * 2,

    .fps = 61,

    .spi = {
        .spi_mode = SPI_IF_MODE(LCD_DRIVE_CONFIG),
        .pixel_type = PIXEL_TYPE(LCD_DRIVE_CONFIG),
        .out_format = OUT_FORMAT(LCD_DRIVE_CONFIG),
        .port = SPI_PORTA,
        .spi_dat_mode = SPI_MODE_UNIDIR,
        .qspi_cmd = {
            .write_cmd = 0xde,
            .read_cmd = 0xdd,
        },
    },

    .debug_mode_en = false,
    .debug_mode_color = 0x00ff00,
};

REGISTER_LCD_DEVICE(st77903_v2) = {
    .logo = "st77903_v2",
    .row_addr_align    = 1,
    .column_addr_align = 1,

    .lcd_cmd = (void *) &lcd_qspi_st77903_v2_cmd_list_poweron,
    .cmd_cnt = sizeof(lcd_qspi_st77903_v2_cmd_list_poweron) / sizeof(lcd_qspi_st77903_v2_cmd_list_poweron[0]),
    .param   = (void *) &lcd_qspi_st77903_v2_param,

    .reset = NULL,	// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl = NULL, //lcd_qspi_st77903_v2_backlight_ctrl,
    .power_ctrl = lcd_qspi_st77903_v2_power_ctrl,
    .entersleep = lcd_qspi_st77903_v2_entersleep,
    .exitsleep = lcd_qspi_st77903_v2_exitsleep,
};


#endif




