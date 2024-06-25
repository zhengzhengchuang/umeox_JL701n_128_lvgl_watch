/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"


/*
** 驱动代码的宏开关
*/
//<<<[qspi屏 320x386]>>>//
#if TCFG_LCD_QSPI_ST77916_ENABLE

#define LCD_DRIVE_CONFIG					QSPI_RGB565_SUBMODE2_1T2B


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
#define SCR_W LCD_WIDTH
#define SCR_H LCD_HEIGHT
#define LCD_W LCD_WIDTH
#define LCD_H LCD_HEIGHT
#define LCD_BLOCK_W LCD_WIDTH
#define LCD_BLOCK_H 152
#define BUF_NUM 2

#define LCD_FORMAT OUTPUT_FORMAT_RGB565

/*
** 初始化代码
*/
static const u8 lcd_qspi_st77916_cmdlist[] ALIGNED(4) = {

    _BEGIN_, 0xF0, 0x08, _END_,
    _BEGIN_, 0xF2, 0x08, _END_,
    _BEGIN_, 0x9B, 0x51, _END_,
    _BEGIN_, 0x86, 0x53, _END_,
    _BEGIN_, 0xF2, 0x80, _END_,
    _BEGIN_, 0xF0, 0x00, _END_,
    _BEGIN_, 0xF0, 0x01, _END_,
    _BEGIN_, 0xF1, 0x01, _END_,
    _BEGIN_, 0xB0, 0x5E, _END_,
    _BEGIN_, 0xB1, 0x3D, _END_,
    _BEGIN_, 0xB2, 0x30, _END_,
    _BEGIN_, 0xB4, 0x27, _END_,
    _BEGIN_, 0xB5, 0x34, _END_,
    _BEGIN_, 0xB6, 0xA5, _END_,
    _BEGIN_, 0xB7, 0x20, _END_,
    _BEGIN_, 0xBA, 0x00, _END_,
    _BEGIN_, 0xBB, 0x08, _END_,
    _BEGIN_, 0xBC, 0x08, _END_,
    _BEGIN_, 0xBD, 0x00, _END_,
    _BEGIN_, 0xC0, 0x80, _END_,
    _BEGIN_, 0xC1, 0x0B, _END_,
    _BEGIN_, 0xC2, 0x34, _END_,
    _BEGIN_, 0xC3, 0x80, _END_,
    _BEGIN_, 0xC4, 0x0B, _END_,
    _BEGIN_, 0xC5, 0x34, _END_,
    _BEGIN_, 0xC6, 0xA9, _END_,
    _BEGIN_, 0xC7, 0x41, _END_,
    _BEGIN_, 0xC8, 0x51, _END_,
    _BEGIN_, 0xC9, 0xA9, _END_,
    _BEGIN_, 0xCA, 0x41, _END_,
    _BEGIN_, 0xCB, 0x51, _END_,
    _BEGIN_, 0xD0, 0xD1, _END_,
    _BEGIN_, 0xD1, 0x40, _END_,
    _BEGIN_, 0xD2, 0x83, _END_,
    _BEGIN_, 0xF5, 0x00, 0xA5, _END_,
    _BEGIN_, 0xDD, 0x33, _END_,
    _BEGIN_, 0xDE, 0x33, _END_,
    _BEGIN_, 0xF1, 0x10, _END_,
    _BEGIN_, 0xF0, 0x00, _END_,
    _BEGIN_, 0xF0, 0x02, _END_,

    _BEGIN_, 0xE0, 0xF0, 0x0A, 0x10, 0x0A, 0x09, 0x15, 0x37, 0x33, 0x4F, 0x38, 0x14, 0x14, 0x32, 0x36, _END_,
    _BEGIN_, 0xE1, 0xF0, 0x09, 0x0F, 0x0A, 0x09, 0x25, 0x37, 0x33, 0x4F, 0x37, 0x13, 0x14, 0x31, 0x35, _END_,

    _BEGIN_, 0xF0, 0x10, _END_,
    _BEGIN_, 0xF3, 0x10, _END_,
    _BEGIN_, 0xE0, 0x0C, _END_,
    _BEGIN_, 0xE1, 0x00, _END_,
    _BEGIN_, 0xE2, 0x00, _END_,
    _BEGIN_, 0xE3, 0x00, _END_,
    _BEGIN_, 0xE4, 0xE0, _END_,
    _BEGIN_, 0xE5, 0x06, _END_,
    _BEGIN_, 0xE6, 0x21, _END_,
    _BEGIN_, 0xE7, 0x10, _END_,
    _BEGIN_, 0xE8, 0x85, _END_,
    _BEGIN_, 0xE9, 0x83, _END_,
    _BEGIN_, 0xEA, 0x16, _END_,
    _BEGIN_, 0xEB, 0x00, _END_,
    _BEGIN_, 0xEC, 0x00, _END_,
    _BEGIN_, 0xED, 0x14, _END_,
    _BEGIN_, 0xEE, 0xFF, _END_,
    _BEGIN_, 0xEF, 0x00, _END_,
    _BEGIN_, 0xF8, 0xFF, _END_,
    _BEGIN_, 0xF9, 0x00, _END_,
    _BEGIN_, 0xFA, 0x00, _END_,
    _BEGIN_, 0xFB, 0x30, _END_,
    _BEGIN_, 0xFC, 0x00, _END_,
    _BEGIN_, 0xFD, 0x00, _END_,
    _BEGIN_, 0xFE, 0x00, _END_,
    _BEGIN_, 0xFF, 0x00, _END_,
    _BEGIN_, 0x60, 0x43, _END_,
    _BEGIN_, 0x61, 0x1A, _END_,
    _BEGIN_, 0x62, 0x40, _END_,
    _BEGIN_, 0x63, 0x40, _END_,
    _BEGIN_, 0x64, 0x02, _END_,
    _BEGIN_, 0x65, 0x00, _END_,
    _BEGIN_, 0x66, 0x40, _END_,
    _BEGIN_, 0x67, 0x03, _END_,
    _BEGIN_, 0x68, 0x00, _END_,
    _BEGIN_, 0x69, 0x00, _END_,
    _BEGIN_, 0x6A, 0x00, _END_,
    _BEGIN_, 0x6B, 0x00, _END_,
    _BEGIN_, 0x70, 0x43, _END_,
    _BEGIN_, 0x71, 0x1A, _END_,
    _BEGIN_, 0x72, 0x40, _END_,
    _BEGIN_, 0x73, 0x40, _END_,
    _BEGIN_, 0x74, 0x02, _END_,
    _BEGIN_, 0x75, 0x00, _END_,
    _BEGIN_, 0x76, 0x40, _END_,
    _BEGIN_, 0x77, 0x03, _END_,
    _BEGIN_, 0x78, 0x00, _END_,
    _BEGIN_, 0x79, 0x00, _END_,
    _BEGIN_, 0x7A, 0x00, _END_,
    _BEGIN_, 0x7B, 0x00, _END_,
    _BEGIN_, 0x80, 0x48, _END_,
    _BEGIN_, 0x81, 0x00, _END_,
    _BEGIN_, 0x82, 0x07, _END_,
    _BEGIN_, 0x83, 0x03, _END_,
    _BEGIN_, 0x84, 0x17, _END_,
    _BEGIN_, 0x85, 0x00, _END_,
    _BEGIN_, 0x86, 0x00, _END_,
    _BEGIN_, 0x87, 0x00, _END_,
    _BEGIN_, 0x88, 0x48, _END_,
    _BEGIN_, 0x89, 0x00, _END_,
    _BEGIN_, 0x8A, 0x09, _END_,
    _BEGIN_, 0x8B, 0x03, _END_,
    _BEGIN_, 0x8C, 0x19, _END_,
    _BEGIN_, 0x8D, 0x00, _END_,
    _BEGIN_, 0x8E, 0x00, _END_,
    _BEGIN_, 0x8F, 0x00, _END_,
    _BEGIN_, 0x90, 0x48, _END_,
    _BEGIN_, 0x91, 0x00, _END_,
    _BEGIN_, 0x92, 0x0B, _END_,
    _BEGIN_, 0x93, 0x03, _END_,
    _BEGIN_, 0x94, 0x13, _END_,
    _BEGIN_, 0x95, 0x00, _END_,
    _BEGIN_, 0x96, 0x00, _END_,
    _BEGIN_, 0x97, 0x00, _END_,
    _BEGIN_, 0x98, 0x48, _END_,
    _BEGIN_, 0x99, 0x00, _END_,
    _BEGIN_, 0x9A, 0x0D, _END_,
    _BEGIN_, 0x9B, 0x03, _END_,
    _BEGIN_, 0x9C, 0x15, _END_,
    _BEGIN_, 0x9D, 0x00, _END_,
    _BEGIN_, 0x9E, 0x00, _END_,
    _BEGIN_, 0x9F, 0x00, _END_,
    _BEGIN_, 0xA0, 0x48, _END_,
    _BEGIN_, 0xA1, 0x00, _END_,
    _BEGIN_, 0xA2, 0x06, _END_,
    _BEGIN_, 0xA3, 0x03, _END_,
    _BEGIN_, 0xA4, 0x16, _END_,
    _BEGIN_, 0xA5, 0x00, _END_,
    _BEGIN_, 0xA6, 0x00, _END_,
    _BEGIN_, 0xA7, 0x00, _END_,
    _BEGIN_, 0xA8, 0x48, _END_,
    _BEGIN_, 0xA9, 0x00, _END_,
    _BEGIN_, 0xAA, 0x08, _END_,
    _BEGIN_, 0xAB, 0x03, _END_,
    _BEGIN_, 0xAC, 0x18, _END_,
    _BEGIN_, 0xAD, 0x00, _END_,
    _BEGIN_, 0xAE, 0x00, _END_,
    _BEGIN_, 0xAF, 0x00, _END_,
    _BEGIN_, 0xB0, 0x48, _END_,
    _BEGIN_, 0xB1, 0x00, _END_,
    _BEGIN_, 0xB2, 0x0A, _END_,
    _BEGIN_, 0xB3, 0x03, _END_,
    _BEGIN_, 0xB4, 0x12, _END_,
    _BEGIN_, 0xB5, 0x00, _END_,
    _BEGIN_, 0xB6, 0x00, _END_,
    _BEGIN_, 0xB7, 0x00, _END_,
    _BEGIN_, 0xB8, 0x48, _END_,
    _BEGIN_, 0xB9, 0x00, _END_,
    _BEGIN_, 0xBA, 0x0C, _END_,
    _BEGIN_, 0xBB, 0x03, _END_,
    _BEGIN_, 0xBC, 0x14, _END_,
    _BEGIN_, 0xBD, 0x00, _END_,
    _BEGIN_, 0xBE, 0x00, _END_,
    _BEGIN_, 0xBF, 0x00, _END_,
    _BEGIN_, 0xC0, 0x45, _END_,
    _BEGIN_, 0xC1, 0x54, _END_,
    _BEGIN_, 0xC2, 0x67, _END_,
    _BEGIN_, 0xC3, 0x76, _END_,
    _BEGIN_, 0xC4, 0x88, _END_,
    _BEGIN_, 0xC5, 0xAA, _END_,
    _BEGIN_, 0xC6, 0x22, _END_,
    _BEGIN_, 0xC7, 0xBB, _END_,
    _BEGIN_, 0xC8, 0x00, _END_,
    _BEGIN_, 0xC9, 0x99, _END_,
    _BEGIN_, 0xD0, 0x45, _END_,
    _BEGIN_, 0xD1, 0x54, _END_,
    _BEGIN_, 0xD2, 0x67, _END_,
    _BEGIN_, 0xD3, 0x76, _END_,
    _BEGIN_, 0xD4, 0x88, _END_,
    _BEGIN_, 0xD5, 0xAA, _END_,
    _BEGIN_, 0xD6, 0x11, _END_,
    _BEGIN_, 0xD7, 0xBB, _END_,
    _BEGIN_, 0xD8, 0x00, _END_,
    _BEGIN_, 0xD9, 0x99, _END_,
    _BEGIN_, 0xF3, 0x01, _END_,
    _BEGIN_, 0xF0, 0x00, _END_,
    _BEGIN_, 0xF0, 0x01, _END_,
    _BEGIN_, 0xF1, 0x01, _END_,
    _BEGIN_, 0xA0, 0x0B, _END_,
    _BEGIN_, 0xA3, 0x2A, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x2B, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x2C, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x2D, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x2E, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x2F, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x30, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x31, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA3, 0x32, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 100, _END_,
    _BEGIN_, 0xA3, 0x33, _END_,
    _BEGIN_, 0xA5, 0xC3, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0xA0, 0x09, _END_,
    _BEGIN_, 0xF1, 0x10, _END_,
    _BEGIN_, 0xF0, 0x00, _END_,

    _BEGIN_, 0x2A, 0x00, 0x00, 0x01, 0x3F, _END_,
    _BEGIN_, 0x2B, 0x01, 0x82, 0x01, 0x82, _END_,
    _BEGIN_, 0x4D, 0x00, _END_,
    _BEGIN_, 0x4E, 0x00, _END_,
    _BEGIN_, 0x4F, 0x00, _END_,
    _BEGIN_, 0x4C, 0x01, _END_,

    _BEGIN_, REGFLAG_DELAY, 10, _END_,
    _BEGIN_, 0x2A, 0x00, 0x00, 0x01, 0x3F, _END_,
    _BEGIN_, 0x2B, 0x00, 0x00, 0x01, 0x81, _END_,

    _BEGIN_, 0x21, _END_,
    _BEGIN_, 0x35, 0x00, _END_,

    _BEGIN_, 0x11, _END_,

    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x3A, 0x55, _END_,

    _BEGIN_, 0x29, _END_,

    _BEGIN_, REGFLAG_DELAY, 50, _END_,
};


/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
// static int lcd_qspi_st77916_backlight_ctrl(u8 onoff)
// {
//     //lcd_bl_ctrl(onoff);
//     return 0;
// }


/*
** lcd电源控制
*/
static int lcd_qspi_st77916_power_ctrl(u8 onoff)
{
    printf("=========%s\n", __func__);
    
    lcd_en_ctrl(onoff);
    return 0;
}

/*
** 设置lcd进入睡眠
*/
static void lcd_qspi_st77916_entersleep(void)
{
    lcd_write_cmd(0x28, NULL, 0);
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
}

/*
** 设置lcd退出睡眠
*/
static void lcd_qspi_st77916_exitsleep(void)
{
    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
    lcd_write_cmd(0x29, NULL, 0);
}


// 配置参数
struct imd_param lcd_qspi_st77916_param = {
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

    .fps = 60,

    .spi = {
        .spi_mode = SPI_IF_MODE(LCD_DRIVE_CONFIG),
        .pixel_type = PIXEL_TYPE(LCD_DRIVE_CONFIG),
        .out_format = OUT_FORMAT(LCD_DRIVE_CONFIG),
        .port = SPI_PORTA,
        .spi_dat_mode = SPI_MODE_UNIDIR,
    },

    .debug_mode_en = false,
    .debug_mode_color = 0xff0000,
};


REGISTER_LCD_DEVICE(st77916) = {
    .logo = "st77916",
    .row_addr_align		= 1,
    .column_addr_align	= 1,

    .lcd_cmd = (void *) &lcd_qspi_st77916_cmdlist,
    .cmd_cnt = sizeof(lcd_qspi_st77916_cmdlist) / sizeof(lcd_qspi_st77916_cmdlist[0]),
    .param   = (void *) &lcd_qspi_st77916_param,

    .reset			= NULL,	// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl = NULL,
    .power_ctrl = lcd_qspi_st77916_power_ctrl,
    .entersleep     = lcd_qspi_st77916_entersleep,
    .exitsleep      = lcd_qspi_st77916_exitsleep,
};
#endif



