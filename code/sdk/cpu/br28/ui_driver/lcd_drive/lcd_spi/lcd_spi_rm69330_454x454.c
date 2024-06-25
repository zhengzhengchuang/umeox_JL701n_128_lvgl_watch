/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"


/*
** 驱动代码的宏开关
*/
//<<<[qspi屏 454x454]>>>//
#if TCFG_LCD_SPI_RM69330_ENABLE

/* #define LCD_DRIVE_CONFIG                    QSPI_RGB565_SUBMODE2_1T2B */
#define LCD_DRIVE_CONFIG                    QSPI_RGB565_SUBMODE1_1T2B
// #define LCD_DRIVE_CONFIG                    QSPI_RGB565_SUBMODE0_1T8B

/*
** 包含imd头文件，屏驱相关的变量和结构体都定义在imd.h
*/
#include "asm/imd.h"
#include "asm/imb.h"
#include "includes.h"
#include "ui/ui_api.h"


#if 0
#define SCR_X 120
#define SCR_Y 110
#define SCR_W 240
#define SCR_H 240
#define LCD_W 240
#define LCD_H 240
#define LCD_BLOCK_W 240
#define LCD_BLOCK_H 60
#define BUF_NUM 2
#else
#define SCR_X 0
#define SCR_Y 0
#define SCR_W 480
#define SCR_H 480
#define LCD_W 480
#define LCD_H 480
#define LCD_BLOCK_W 480
#define LCD_BLOCK_H 60
#define BUF_NUM 2
#endif


/*
** 初始化代码
*/
const u8 lcd_spi_rm69330_cmdlist[] ALIGNED(64) = {
    _BEGIN_, 0xFE, 0x01, _END_,
    _BEGIN_, 0x05, 0x00, _END_,
    _BEGIN_, 0x06, 0x72, _END_,
    _BEGIN_, 0x0D, 0x00, _END_,
    _BEGIN_, 0x0E, 0x81, _END_,
    _BEGIN_, 0x0F, 0x81, _END_,
    _BEGIN_, 0x10, 0x11, _END_,
    _BEGIN_, 0x11, 0x81, _END_,
    _BEGIN_, 0x12, 0x81, _END_,
    _BEGIN_, 0x13, 0x80, _END_,
    _BEGIN_, 0x14, 0x80, _END_,
    _BEGIN_, 0x15, 0x81, _END_,
    _BEGIN_, 0x16, 0x81, _END_,
    _BEGIN_, 0x18, 0x66, _END_,
    _BEGIN_, 0x19, 0x88, _END_,
    _BEGIN_, 0x5B, 0x10, _END_,
    _BEGIN_, 0x5C, 0x55, _END_,
    _BEGIN_, 0x62, 0x19, _END_,
    _BEGIN_, 0x63, 0x19, _END_,
    _BEGIN_, 0x70, 0x54, _END_,
    _BEGIN_, 0x74, 0x0C, _END_,
    _BEGIN_, 0xC5, 0x10, _END_,
    _BEGIN_, 0xFE, 0x01, _END_,
    _BEGIN_, 0x25, 0x03, _END_,
    _BEGIN_, 0x26, 0x32, _END_,
    _BEGIN_, 0x27, 0x0A, _END_,
    _BEGIN_, 0x28, 0x08, _END_,
    _BEGIN_, 0x2A, 0x03, _END_,
    _BEGIN_, 0x2B, 0x32, _END_,
    _BEGIN_, 0x2D, 0x0A, _END_,
    _BEGIN_, 0x2F, 0x08, _END_,
    _BEGIN_, 0x30, 0x43, _END_,
    _BEGIN_, 0x66, 0x90, _END_,
    _BEGIN_, 0x72, 0x1A, _END_,
    _BEGIN_, 0x73, 0x13, _END_,
    _BEGIN_, 0xFE, 0x01, _END_,
    _BEGIN_, 0x6A, 0x17, _END_,
    _BEGIN_, 0x1B, 0x00, _END_,
    _BEGIN_, 0x1D, 0x03, _END_,
    _BEGIN_, 0x1E, 0x03, _END_,
    _BEGIN_, 0x1F, 0x03, _END_,
    _BEGIN_, 0x20, 0x03, _END_,
    _BEGIN_, 0xFE, 0x01, _END_,
    _BEGIN_, 0x36, 0x00, _END_,
    _BEGIN_, 0x6C, 0x80, _END_,
    _BEGIN_, 0x6D, 0x19, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x63, 0x00, _END_,
    _BEGIN_, 0x64, 0x0E, _END_,
    _BEGIN_, 0xFE, 0x02, _END_,
    _BEGIN_, 0xA9, 0x30, _END_,
    _BEGIN_, 0xAA, 0xB9, _END_,
    _BEGIN_, 0xAB, 0x01, _END_,
    _BEGIN_, 0xFE, 0x03, _END_,
    _BEGIN_, 0xA9, 0x30, _END_,
    _BEGIN_, 0xAA, 0x90, _END_,
    _BEGIN_, 0xAB, 0x01, _END_,
    _BEGIN_, 0xFE, 0x0C, _END_,
    _BEGIN_, 0x07, 0x1F, _END_,
    _BEGIN_, 0x08, 0x2F, _END_,
    _BEGIN_, 0x09, 0x3F, _END_,
    _BEGIN_, 0x0A, 0x4F, _END_,
    _BEGIN_, 0x0B, 0x5F, _END_,
    _BEGIN_, 0x0C, 0x6F, _END_,
    _BEGIN_, 0x0D, 0xFF, _END_,
    _BEGIN_, 0x0E, 0xFF, _END_,
    _BEGIN_, 0x0F, 0xFF, _END_,
    _BEGIN_, 0x10, 0xFF, _END_,
    _BEGIN_, 0xFE, 0x01, _END_,
    _BEGIN_, 0x42, 0x14, _END_,
    _BEGIN_, 0x43, 0x41, _END_,
    _BEGIN_, 0x44, 0x25, _END_,
    _BEGIN_, 0x45, 0x52, _END_,
    _BEGIN_, 0x46, 0x36, _END_,
    _BEGIN_, 0x47, 0x63, _END_,
    _BEGIN_, 0x48, 0x41, _END_,
    _BEGIN_, 0x49, 0x14, _END_,
    _BEGIN_, 0x4A, 0x52, _END_,
    _BEGIN_, 0x4B, 0x25, _END_,
    _BEGIN_, 0x4C, 0x63, _END_,
    _BEGIN_, 0x4D, 0x36, _END_,
    _BEGIN_, 0x4E, 0x16, _END_,
    _BEGIN_, 0x4F, 0x61, _END_,
    _BEGIN_, 0x50, 0x25, _END_,
    _BEGIN_, 0x51, 0x52, _END_,
    _BEGIN_, 0x52, 0x34, _END_,
    _BEGIN_, 0x53, 0x43, _END_,
    _BEGIN_, 0x54, 0x61, _END_,
    _BEGIN_, 0x55, 0x16, _END_,
    _BEGIN_, 0x56, 0x52, _END_,
    _BEGIN_, 0x57, 0x25, _END_,
    _BEGIN_, 0x58, 0x43, _END_,
    _BEGIN_, 0x59, 0x34, _END_,
    _BEGIN_, 0xFE, 0x01, _END_,
    _BEGIN_, 0x3A, 0x00, _END_,
    _BEGIN_, 0x3B, 0x00, _END_,
    _BEGIN_, 0x3D, 0x12, _END_,
    _BEGIN_, 0x3F, 0x37, _END_,
    _BEGIN_, 0x40, 0x12, _END_,
    _BEGIN_, 0x41, 0x0F, _END_,
    _BEGIN_, 0x37, 0x0C, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x5D, 0x01, _END_,
    _BEGIN_, 0x75, 0x08, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x5E, 0x0F, _END_,
    _BEGIN_, 0x5F, 0x12, _END_,
    _BEGIN_, 0x60, 0xFF, _END_,
    _BEGIN_, 0x61, 0xFF, _END_,
    _BEGIN_, 0x62, 0xFF, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x76, 0xFF, _END_,
    _BEGIN_, 0x77, 0xFF, _END_,
    _BEGIN_, 0x78, 0x49, _END_,
    _BEGIN_, 0x79, 0xF3, _END_,
    _BEGIN_, 0x7A, 0xFF, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x00, 0x9D, _END_,
    _BEGIN_, 0x01, 0x00, _END_,
    _BEGIN_, 0x02, 0x00, _END_,
    _BEGIN_, 0x03, 0x00, _END_,
    _BEGIN_, 0x04, 0x00, _END_,
    _BEGIN_, 0x05, 0x01, _END_,
    _BEGIN_, 0x06, 0x01, _END_,
    _BEGIN_, 0x07, 0x01, _END_,
    _BEGIN_, 0x08, 0x00, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x09, 0xDC, _END_,
    _BEGIN_, 0x0A, 0x00, _END_,
    _BEGIN_, 0x0B, 0x02, _END_,
    _BEGIN_, 0x0C, 0x00, _END_,
    _BEGIN_, 0x0D, 0x08, _END_,
    _BEGIN_, 0x0E, 0x01, _END_,
    _BEGIN_, 0x0F, 0xCE, _END_,
    _BEGIN_, 0x10, 0x16, _END_,
    _BEGIN_, 0x11, 0x00, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x12, 0xDC, _END_,
    _BEGIN_, 0x13, 0x00, _END_,
    _BEGIN_, 0x14, 0x02, _END_,
    _BEGIN_, 0x15, 0x00, _END_,
    _BEGIN_, 0x16, 0x08, _END_,
    _BEGIN_, 0x17, 0x02, _END_,
    _BEGIN_, 0x18, 0xCE, _END_,
    _BEGIN_, 0x19, 0x16, _END_,
    _BEGIN_, 0x1A, 0x00, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x1B, 0xDC, _END_,
    _BEGIN_, 0x1C, 0x00, _END_,
    _BEGIN_, 0x1D, 0x02, _END_,
    _BEGIN_, 0x1E, 0x00, _END_,
    _BEGIN_, 0x1F, 0x08, _END_,
    _BEGIN_, 0x20, 0x01, _END_,
    _BEGIN_, 0x21, 0xCE, _END_,
    _BEGIN_, 0x22, 0x16, _END_,
    _BEGIN_, 0x23, 0x00, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x24, 0xDC, _END_,
    _BEGIN_, 0x25, 0x00, _END_,
    _BEGIN_, 0x26, 0x02, _END_,
    _BEGIN_, 0x27, 0x00, _END_,
    _BEGIN_, 0x28, 0x08, _END_,
    _BEGIN_, 0x29, 0x02, _END_,
    _BEGIN_, 0x2A, 0xCE, _END_,
    _BEGIN_, 0x2B, 0x16, _END_,
    _BEGIN_, 0x2D, 0x00, _END_,
    _BEGIN_, 0xFE, 0x04, _END_,
    _BEGIN_, 0x53, 0x8a, _END_,
    _BEGIN_, 0x54, 0x00, _END_,
    _BEGIN_, 0x55, 0x03, _END_,
    _BEGIN_, 0x56, 0x01, _END_,
    _BEGIN_, 0x58, 0x01, _END_,
    _BEGIN_, 0x59, 0x00, _END_,
    _BEGIN_, 0x65, 0x76, _END_,
    _BEGIN_, 0x66, 0x19, _END_,
    _BEGIN_, 0x67, 0x00, _END_,
    _BEGIN_, 0xFE, 0x07, _END_,
    _BEGIN_, 0x15, 0x04, _END_,
    _BEGIN_, 0xFE, 0x05, _END_,
    _BEGIN_, 0x4C, 0x01, _END_,
    _BEGIN_, 0x4D, 0x82, _END_,
    _BEGIN_, 0x4E, 0x04, _END_,
    _BEGIN_, 0x4F, 0x00, _END_,
    _BEGIN_, 0x50, 0x20, _END_,
    _BEGIN_, 0x51, 0x10, _END_,
    _BEGIN_, 0x52, 0x04, _END_,
    _BEGIN_, 0x53, 0x41, _END_,
    _BEGIN_, 0x54, 0x0A, _END_,
    _BEGIN_, 0x55, 0x08, _END_,
    _BEGIN_, 0x56, 0x00, _END_,
    _BEGIN_, 0x57, 0x28, _END_,
    _BEGIN_, 0x58, 0x00, _END_,
    _BEGIN_, 0x59, 0x80, _END_,
    _BEGIN_, 0x5A, 0x04, _END_,
    _BEGIN_, 0x5B, 0x10, _END_,
    _BEGIN_, 0x5C, 0x20, _END_,
    _BEGIN_, 0x5D, 0x00, _END_,
    _BEGIN_, 0x5E, 0x04, _END_,
    _BEGIN_, 0x5F, 0x0A, _END_,
    _BEGIN_, 0x60, 0x01, _END_,
    _BEGIN_, 0x61, 0x08, _END_,
    _BEGIN_, 0x62, 0x00, _END_,
    _BEGIN_, 0x63, 0x20, _END_,
    _BEGIN_, 0x64, 0x40, _END_,
    _BEGIN_, 0x65, 0x04, _END_,
    _BEGIN_, 0x66, 0x02, _END_,
    _BEGIN_, 0x67, 0x48, _END_,
    _BEGIN_, 0x68, 0x4C, _END_,
    _BEGIN_, 0x69, 0x02, _END_,
    _BEGIN_, 0x6A, 0x12, _END_,
    _BEGIN_, 0x6B, 0x00, _END_,
    _BEGIN_, 0x6C, 0x48, _END_,
    _BEGIN_, 0x6D, 0xA0, _END_,
    _BEGIN_, 0x6E, 0x08, _END_,
    _BEGIN_, 0x6F, 0x04, _END_,
    _BEGIN_, 0x70, 0x05, _END_,
    _BEGIN_, 0x71, 0x92, _END_,
    _BEGIN_, 0x72, 0x00, _END_,
    _BEGIN_, 0x73, 0x18, _END_,
    _BEGIN_, 0x74, 0xA0, _END_,
    _BEGIN_, 0x75, 0x00, _END_,
    _BEGIN_, 0x76, 0x00, _END_,
    _BEGIN_, 0x77, 0xE4, _END_,
    _BEGIN_, 0x78, 0x00, _END_,
    _BEGIN_, 0x79, 0x04, _END_,
    _BEGIN_, 0x7A, 0x02, _END_,
    _BEGIN_, 0x7B, 0x01, _END_,
    _BEGIN_, 0x7C, 0x00, _END_,
    _BEGIN_, 0x7D, 0x00, _END_,
    _BEGIN_, 0x7E, 0x24, _END_,
    _BEGIN_, 0x7F, 0x4C, _END_,
    _BEGIN_, 0x80, 0x04, _END_,
    _BEGIN_, 0x81, 0x0A, _END_,
    _BEGIN_, 0x82, 0x02, _END_,
    _BEGIN_, 0x83, 0xC1, _END_,
    _BEGIN_, 0x84, 0x02, _END_,
    _BEGIN_, 0x85, 0x18, _END_,
    _BEGIN_, 0x86, 0x90, _END_,
    _BEGIN_, 0x87, 0x60, _END_,
    _BEGIN_, 0x88, 0x88, _END_,
    _BEGIN_, 0x89, 0x02, _END_,
    _BEGIN_, 0x8A, 0x09, _END_,
    _BEGIN_, 0x8B, 0x0C, _END_,
    _BEGIN_, 0x8C, 0x18, _END_,
    _BEGIN_, 0x8D, 0x90, _END_,
    _BEGIN_, 0x8E, 0x10, _END_,
    _BEGIN_, 0x8F, 0x08, _END_,
    _BEGIN_, 0x90, 0x00, _END_,
    _BEGIN_, 0x91, 0x10, _END_,
    _BEGIN_, 0x92, 0xA8, _END_,
    _BEGIN_, 0x93, 0x00, _END_,
    _BEGIN_, 0x94, 0x04, _END_,
    _BEGIN_, 0x95, 0x0A, _END_,
    _BEGIN_, 0x96, 0x00, _END_,
    _BEGIN_, 0x97, 0x08, _END_,
    _BEGIN_, 0x98, 0x10, _END_,
    _BEGIN_, 0x99, 0x28, _END_,
    _BEGIN_, 0x9A, 0x08, _END_,
    _BEGIN_, 0x9B, 0x04, _END_,
    _BEGIN_, 0x9C, 0x02, _END_,
    _BEGIN_, 0x9D, 0x03, _END_,
    _BEGIN_, 0xFE, 0x0C, _END_,
    _BEGIN_, 0x25, 0x00, _END_,
    _BEGIN_, 0x31, 0xEF, _END_,
    _BEGIN_, 0x32, 0xE3, _END_,
    _BEGIN_, 0x33, 0x00, _END_,
    _BEGIN_, 0x34, 0xE3, _END_,
    _BEGIN_, 0x35, 0xE3, _END_,
    _BEGIN_, 0x36, 0x80, _END_,
    _BEGIN_, 0x37, 0x00, _END_,
    _BEGIN_, 0x38, 0x79, _END_,
    _BEGIN_, 0x39, 0x00, _END_,
    _BEGIN_, 0x3A, 0x00, _END_,
    _BEGIN_, 0x3B, 0x00, _END_,
    _BEGIN_, 0x3D, 0x00, _END_,
    _BEGIN_, 0x3F, 0x00, _END_,
    _BEGIN_, 0x40, 0x00, _END_,
    _BEGIN_, 0x41, 0x00, _END_,
    _BEGIN_, 0x42, 0x00, _END_,
    _BEGIN_, 0x43, 0x01, _END_,
    _BEGIN_, 0xFE, 0x00, _END_,
    _BEGIN_, 0x35, 0x00, _END_,

#if (SPI_SUBMODE(LCD_DRIVE_CONFIG) == DSPI_MODE)

#if (PIXEL_nPnT(LCD_DRIVE_CONFIG) == PIXEL_1P1T)
    _BEGIN_, 0xC4, 0x80 | (2 << 4) | 1, _END_, //00:1P1T for 1wire 2:1P1T for 2wire 3:2P3T for 2wire
#elif (PIXEL_nPnT(LCD_DRIVE_CONFIG) == PIXEL_2P3T)
    _BEGIN_, 0xC4, 0x80 | (3 << 4) | 1, _END_,
#endif

#else

#if (PIXEL_nPnT(LCD_DRIVE_CONFIG) == PIXEL_1P1T)
    _BEGIN_, 0xC4, 0x80 | (0 << 4) | 1, _END_,
#else
    _BEGIN_, 0xC4, 0x80, _END_,
#endif

#endif

    _BEGIN_, 0x44, 0x0C, _END_,
    _BEGIN_, 0x44, 0x00, _END_,
    _BEGIN_, 0x51, 0xAF, _END_,

#if (OUT_FORMAT(LCD_DRIVE_CONFIG) == FORMAT_RGB565)
    _BEGIN_, 0x3A, 0x55, _END_,
#elif (OUT_FORMAT(LCD_DRIVE_CONFIG) == FORMAT_RGB666)
    _BEGIN_, 0x3A, 0x66, _END_,
#elif (OUT_FORMAT(LCD_DRIVE_CONFIG) == FORMAT_RGB888)
    _BEGIN_, 0x3A, 0x77, _END_,
#endif

    /* _BEGIN_,0x2A, XSTART>>8,XSTART,XEND>>8,XEND,_END_, */
    /* _BEGIN_,0x2B, YSTART>>8,YSTART,YEND>>8,YEND,_END_, */
    _BEGIN_, 0x11, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x29, _END_,
    _BEGIN_, REGFLAG_DELAY, 20, _END_,
    /* _BEGIN_,0x23, 0x00,_END_, */
};





/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_spi_rm69330_backlight_ctrl(u8 onoff)
{
    lcd_bl_ctrl(onoff);
    return 0;
}


/*
** 设置lcd进入睡眠
*/
static void lcd_spi_rm69330_entersleep(void)
{
    lcd_write_cmd(0x28, NULL, 0);
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
}

/*
** 设置lcd退出睡眠
*/
static void lcd_spi_rm69330_exitsleep(void)
{
    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms
    lcd_write_cmd(0x29, NULL, 0);
}


struct imd_param lcd_spi_rm69330_param = {
    .scr_x    = SCR_X,
    .scr_y	  = SCR_Y,
    .scr_w	  = SCR_W,
    .scr_h	  = SCR_H,

    .in_width  = SCR_W,
    .in_height = SCR_H,
    .in_format = OUTPUT_FORMAT_RGB565,

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

REGISTER_LCD_DEVICE(rm69330) = {
    .logo = "rm69330",
    .row_addr_align    = 1,
    .column_addr_align = 1,

    .lcd_cmd = (void *) &lcd_spi_rm69330_cmdlist,
    .cmd_cnt = sizeof(lcd_spi_rm69330_cmdlist) / sizeof(lcd_spi_rm69330_cmdlist[0]),
    .param   = (void *) &lcd_spi_rm69330_param,

    .reset			= NULL,// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl	= lcd_spi_rm69330_backlight_ctrl,
    .entersleep		= lcd_spi_rm69330_entersleep,
    .exitsleep		= lcd_spi_rm69330_exitsleep,
};


#endif


