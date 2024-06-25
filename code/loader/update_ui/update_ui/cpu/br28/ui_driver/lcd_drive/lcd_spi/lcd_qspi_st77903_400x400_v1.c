/*
** 包含board的头文件，确定baord里面开关的屏驱�?
*/
#include "app_config.h"
#include "clock_cfg.h"
#include "ui/lcd_spi/lcd_drive.h"

/*
** 驱动代码的宏开�?
*/
//<<<[qspi�?400x400]>>>//
#if TCFG_LCD_QSPI_ST77903_V1_ENABLE


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
#define LCD_BLOCK_H 20
#endif

#define BUF_NUM 2

#define LCD_FORMAT OUTPUT_FORMAT_RGB565

/*
** 初始化代�?
*/
static const u8 lcd_qspi_st77903_v1_cmd_list_poweron[] ALIGNED(4) = {
    _BEGIN_, 0xF0, 0xC3, _END_,
    _BEGIN_, 0xF0, 0x96, _END_,
    _BEGIN_, 0xF0, 0xA5, _END_,
    _BEGIN_, 0xC1, 0xBB, 0x02, 0x0F, 0x0B, _END_,
    _BEGIN_, 0xC2, 0xBB, 0x02, 0x0F, 0x0B, _END_,
    _BEGIN_, 0xC3, 0x56, 0x06, 0x04, 0x03, _END_,
    _BEGIN_, 0xC4, 0x42, 0x05, 0x24, 0x03, _END_,
    _BEGIN_, 0xC5, 0x2c, _END_,
    _BEGIN_, 0xE0, 0xF0, 0x05, 0x09, 0x09, 0x0A, 0x25, 0x2C, 0x33, 0x44, 0x38, 0x14, 0x14, 0x29, 0x2F, _END_,
    _BEGIN_, 0xE1, 0xF0, 0x05, 0x0A, 0x0A, 0x0A, 0x25, 0x2D, 0x33, 0x43, 0x38, 0x14, 0x14, 0x29, 0x2F, _END_,
    _BEGIN_, 0xE4, 0xC8, 0x20, _END_,
    _BEGIN_, 0xE5, 0x0F, 0xF5, 0x00, 0x11, 0x22, 0x25, 0x10, 0x55, 0x55, 0x55, 0x55, 0x55, _END_,
    _BEGIN_, 0xE6, 0x0F, 0xF5, 0x00, 0x11, 0x22, 0x25, 0x10, 0x55, 0x55, 0x55, 0x55, 0x55, _END_,
    _BEGIN_, 0xE7, 0x00, 0x77, 0x1F, 0xCC, 0x2A, 0x80, 0x33, _END_,
    _BEGIN_, 0xEC, 0x00, 0x55, 0x00, 0x00, 0x00, 0x49, 0x22, _END_,
    _BEGIN_, 0xB2, 0x10, _END_,
    _BEGIN_, 0xB3,/*0x02*/0x03, _END_,
    _BEGIN_, 0xB4, 0x02, _END_,
    //_BEGIN_, 0xB5, 0x00, 0x54, 0x00, 0x54, _END_,
    _BEGIN_, 0xB5, 0x00, 0x06, 0x00, 0x06, _END_,
    _BEGIN_, 0xB6, 0xC7, 0x31, _END_,
    _BEGIN_, 0xBA, 0x5B, 0xAA, 0x02, 0x00, 0x21, 0x01, 0x00, _END_,
    _BEGIN_, 0xBB, 0x00, 0x24, 0x00, 0x27, 0x08, 0x07, 0x07, 0x04, _END_,
    _BEGIN_, 0xBC, 0x00, 0x36, 0x00, 0x28, 0x08, 0x07, 0x07, 0x04, _END_,
    _BEGIN_, 0xBD, 0x77, 0xFF, 0xFF, 0xFF, 0xBA, 0xAB, 0xFF, 0xFF, 0x16, 0xFF, 0x02, _END_,
    _BEGIN_, 0xA4, 0xC0, 0x4A, 0xE0, _END_,
    _BEGIN_, 0xA5, 0x20, 0x12, 0x40, 0x01, 0x00, 0x06, 0x2A, 0x0A, 0x00, _END_,
    _BEGIN_, 0xA6, 0x20, 0x12, 0x40, 0x01, 0x00, 0x06, 0x2A, 0x0A, 0x00, _END_,
    _BEGIN_, 0xd9, 0x22, _END_,
    _BEGIN_, 0x21, _END_,
    _BEGIN_, 0x35, 0x00, _END_,
    _BEGIN_, 0x36, 0x0C, _END_,
    _BEGIN_, 0x3A, 0x05, _END_,
    _BEGIN_, 0x11, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x29, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,

    //test bist mode
    //_BEGIN_, 0XB0, 0XA5, _END_,
    //_BEGIN_, 0XCC, 0X40, 0X00, 0X3F, 0X00, 0X0A, 0X0A, 0XEA, 0XEA, 0X03, _END_,
};



/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_qspi_st77903_v1_backlight_ctrl(u8 percent)
{
    if (percent) {

    } else {

    }

    return 0;
}


/*
** lcd电源控制
*/
static int lcd_qspi_st77903_v1_power_ctrl(u8 onoff)
{
    lcd_en_ctrl(onoff);
    return 0;
}

void lcd_test_mode()
{
    u8 buf[10];
    buf[0] = 0xa5;
    lcd_write_cmd(0xb0, buf, 1);

    buf[0] = 0x40;
    buf[1] = 0x00;
    buf[2] = 0x3f;
    buf[3] = 0x00;
    buf[4] = 0x0a;
    buf[5] = 0x0a;
    buf[6] = 0xea;
    buf[7] = 0xea;
    buf[8] = 0x03;
    lcd_write_cmd(0xcc, buf, 9);
}

void lcd_read_test()
{
    u8 power_mode = 0;
    lcd_read_cmd(0x0a, &power_mode, sizeof(power_mode));
    printf("power_mode : 0x%x\n", power_mode);

    u8 pixel_format ;

    pixel_format = 0x06;
    lcd_write_cmd(0x3a, &pixel_format, sizeof(pixel_format));

    pixel_format = 0;
    lcd_read_cmd(0x0c, &pixel_format, sizeof(pixel_format));
    printf("pixel_format : 0x%x\n", pixel_format);

    pixel_format = 0x05;
    lcd_write_cmd(0x3a, &pixel_format, sizeof(pixel_format));

    pixel_format = 0;
    lcd_read_cmd(0x0c, &pixel_format, sizeof(pixel_format));
    printf("pixel_format : 0x%x\n", pixel_format);
}

/*
** 设置lcd进入睡眠
*/
static void lcd_qspi_st77903_v1_entersleep(void)
{
    printf("enter sleep\n");

#if 0
    //lcd_test_mode();
    lcd_read_test();
    while (1) {
        os_time_dly(100);
    }
#endif

    lcd_write_cmd(0x28, NULL, 0);
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
}



/*
** 设置lcd退出睡�?
*/
static void lcd_qspi_st77903_v1_exitsleep(void)
{
    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(5);	// delay 120ms
    lcd_write_cmd(0x29, NULL, 0);
}


struct imd_param lcd_qspi_st77903_v1_param = {
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

    .fps = 61,//61

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

REGISTER_LCD_DEVICE() = {
    .logo = "st77903_v1",
    .row_addr_align    = 1,
    .column_addr_align = 1,

    .lcd_cmd = (void *) &lcd_qspi_st77903_v1_cmd_list_poweron,
    .cmd_cnt = sizeof(lcd_qspi_st77903_v1_cmd_list_poweron) / sizeof(lcd_qspi_st77903_v1_cmd_list_poweron[0]),
    .param   = (void *) &lcd_qspi_st77903_v1_param,

    .reset = NULL,	// 没有特殊的复位操作，用内部普通复位函数即�?
    .backlight_ctrl = NULL, //lcd_qspi_st77903_v1_backlight_ctrl,
    .power_ctrl = lcd_qspi_st77903_v1_power_ctrl,
    .entersleep = lcd_qspi_st77903_v1_entersleep,
    .exitsleep = lcd_qspi_st77903_v1_exitsleep,
};


#endif





