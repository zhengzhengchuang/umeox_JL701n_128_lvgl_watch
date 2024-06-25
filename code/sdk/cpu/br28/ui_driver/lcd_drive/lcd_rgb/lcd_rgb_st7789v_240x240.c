/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"


/*
** 驱动代码的宏开关
** 注意：rgb屏需要一直推才能正常显示
*/
#if TCFG_LCD_RGB_ST7789V_ENABLE


#define LCD_DRIVE_CONFIG                    RGB_SPI_3WIRE_RGB565

/*
** 包含imd头文件，屏驱相关的变量和结构体都定义在imd.h
*/
#include "asm/imd.h"
#include "asm/imb.h"
#include "includes.h"
#include "ui/ui_api.h"


#define SCR_X 0
#define SCR_Y 0
#define SCR_W 240
#define SCR_H 240
#define LCD_W 240
#define LCD_H 240


#define ld_depth_sf        0
/* #define hpw_prd            10 */
#define dhbw_prd            50
/* #define hfw_prd            38 */
/* #define vpw_prd            4 */
#define dvbw_prd            4
/* #define vfw_prd            8 */
/* #define hsync_out  0b100 */
/* #define vsync_out  0b101 */
/* #define dsync_out  0b110 */


#define HV_MODE            0  //1:hv_mode 0:de_mode
/* #define xor_edge           0 */
/* #define debug_mode         1 */
/* #define dclk_continue      0 */
/* #define dat_l2h            0 */
/* #if (OUT_FORMAT == FORMAT_RGB666) || (OUT_FORMAT == FORMAT_RGB565) */
/* #define dat_sf             1 */
/* #else */
/* #define dat_sf             0 */
/* #endif */
/* #define int_en             0 */
/* #define dither_en          1 */


static const u8 lcd_rgb_st7789v_cmdlist[] ALIGNED(4) = {
    _BEGIN_, 0x01, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x11, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x36, 0x00, _END_,

#if (OUT_FORMAT(LCD_DRIVE_CONFIG) == FORMAT_RGB666)
    _BEGIN_, 0x3A, 0x66, _END_,
#elif (OUT_FORMAT(LCD_DRIVE_CONFIG) == FORMAT_RGB565)
    _BEGIN_, 0x3A, 0x55, _END_,
#endif

    _BEGIN_, 0xB0, 0x11, 0xf4 | 0, _END_,

#ifdef HV_MODE
    _BEGIN_, 0xB1, 0xe0, dvbw_prd, dhbw_prd, _END_,   //without ram
    /* _BEGIN_, 0xB1, 0xe0, 0x02, 0x14, _END_,   //without ram */
    /* _BEGIN_, 0xB1 , 0x60 , vbw_prd , hbw_prd, _END_,//with ram */
    /* _BEGIN_, 0xB1 , 0x60 , 0x02 , 0x14, _END_,//with ram */
#else
    /* _BEGIN_, 0xB1, 0xc0, vbw_prd, hbw_prd, _END_,   //without ram */
    /* _BEGIN_, 0xB1, 0xc0, 0x02, 0x14, _END_,   //without ram */
    _BEGIN_, 0xB1, 0x40, dvbw_prd, dhbw_prd, _END_,   //with ram
    /* _BEGIN_, 0xB1 , 0x40 , 0x02 , 0x14, _END_,//with ram */
#endif

    _BEGIN_, 0xB2, 0x0C, 0x0C, 0x00, 0x33, 0x33, _END_,
    _BEGIN_, 0xBB, 0x1A, _END_,
    _BEGIN_, 0xC0, 0x2C, _END_,
    _BEGIN_, 0xC2, 0x01, _END_,
    _BEGIN_, 0xC3, 0x0F, _END_,
    _BEGIN_, 0xC4, 0x20, _END_,
    _BEGIN_, 0xC6, 0x0F, _END_,   //Dot INV, 60Hz
    _BEGIN_, 0xD0, 0xA4, 0xA1, _END_,
    _BEGIN_, 0xE0, 0xD0, 0x13, 0x1A, 0x0A, 0x0A, 0x26, 0x3F, 0x54, 0x54, 0x18, 0x14, 0x14, 0x30, 0x33, _END_,
    _BEGIN_, 0xE1, 0xD0, 0x13, 0x1A, 0x0A, 0x0A, 0x26, 0x3F, 0x54, 0x54, 0x1A, 0x16, 0x16, 0x32, 0x35, _END_,
    //_BEGIN_, 0xE7 , 0x00, _END_,  //10-2DATA   00-1DATA
    _BEGIN_, 0x21, _END_,
    _BEGIN_, 0x2a, 0x00, 0x00, 0x00, 0xEF, _END_,
    _BEGIN_, 0x2b, 0x00, 0x00, 0x00, 0xEF, _END_,

    _BEGIN_, 0x29, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    /* _BEGIN_, 0x2c, _END_, */
};



/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_rgb_st7789v_backlight_ctrl(u8 onoff)
{
    JL_PORTC->DIR &= ~BIT(8);
    if (onoff) {
        JL_PORTC->OUT |= BIT(8);
    } else {
        JL_PORTC->OUT &= ~BIT(8);
    }
    return 0;
}


/*
** 设置lcd进入睡眠
*/
static void lcd_rgb_st7789v_entersleep(void)
{
    //TODO
}

/*
** 设置lcd退出睡眠
*/
static void lcd_rgb_st7789v_exitsleep(void)
{
    //TODO
}

struct imd_param lcd_rgb_st7789v_param = {
    .scr_x    = SCR_X,
    .scr_y	  = SCR_Y,
    .scr_w	  = SCR_W,
    .scr_h	  = SCR_H,
    .in_width  = SCR_W,
    .in_height = SCR_H,

    .lcd_width  = LCD_W,
    .lcd_height = LCD_H,

    .in_format = OUTPUT_FORMAT_RGB565,//-1,
    .lcd_type = LCD_TYPE_RGB,

    .buffer_num	= 2,
    .buffer_size = LCD_W * LCD_H * 2,
    .fps = 60,

    .spi = {
        .spi_mode = SPI_IF_MODE(LCD_DRIVE_CONFIG),
        .port = SPI_PORTB,
        .spi_dat_mode = SPI_MODE_UNIDIR,
    },
    .rgb = {
        .out_format = OUT_FORMAT(LCD_DRIVE_CONFIG),
        .continue_frames = 0,
        .dat_l2h = 0,
        .dat_sf = 1,
        .hv_mode = HV_MODE,
        .xor_edge = 0,

        .hpw_prd = 10,
        .hbw_prd = 50,
        .hfw_prd = 38,
        .vpw_prd = 4,
        .vbw_prd = 4,
        .vfw_prd = 8,
    },

    .debug_mode_en = false,
    .debug_mode_color = 0xff0000,

    .te_en = false,
};


REGISTER_LCD_DEVICE(st7789v_rgb) = {
    .logo = "st7789v_rgb",
    .row_addr_align		= 1,
    .column_addr_align	= 1,

    .lcd_cmd = (void *) &lcd_rgb_st7789v_cmdlist,
    .cmd_cnt = sizeof(lcd_rgb_st7789v_cmdlist) / sizeof(lcd_rgb_st7789v_cmdlist[0]),
    .param	 = (void *) &lcd_rgb_st7789v_param,

    .reset			= NULL,// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl = NULL,//lcd_rgb_st7789v_backlight_ctrl,
    .entersleep		= NULL,//lcd_rgb_st7789v_entersleep,
    .exitsleep		= NULL,//lcd_rgb_st7789v_exitsleep,
};


#endif
