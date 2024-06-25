/*
** 包含board的头文件，确定baord里面开关的屏驱宏
*/
#include "app_config.h"
#include "clock_cfg.h"
#include "asm/psram_api.h"


/*
** 驱动代码的宏开关
*/
//<<<[qspi屏 368x448]>>>//
#if TCFG_LCD_QSPI_FT2308_ENABLE

// #define LCD_DRIVE_CONFIG                    QSPI_RGB565_SUBMODE0_1T8B
/* #define LCD_DRIVE_CONFIG                    QSPI_RGB565_SUBMODE1_1T2B */
#define LCD_DRIVE_CONFIG                    QSPI_RGB565_SUBMODE2_1T2B

/*
** 包含imd头文件，屏驱相关的变量和结构体都定义在imd.h
*/
#include "asm/imd.h"
#include "asm/imb.h"
#include "includes.h"
#include "ui/ui_api.h"


#if 0
#define SCR_X 100
#define SCR_Y 100
#define SCR_W 240
#define SCR_H 240
#define LCD_W 240
#define LCD_H 240
#define LCD_BLOCK_W 240
#define LCD_BLOCK_H 240
#define BUF_NUM 2
#else
#define SCR_X 0
#define SCR_Y 0
#define SCR_W 368
#define SCR_H 448
#define LCD_W 368
#define LCD_H 448
#define LCD_BLOCK_W 368
#if (defined(TCFG_SMART_VOICE_ENABLE) && \
    (TCFG_SMART_VOICE_ENABLE == 1))
#define LCD_BLOCK_H 20
#else /*TCFG_SMART_VOICE_ENABLE == 0*/
#define LCD_BLOCK_H 152
#endif /*TCFG_SMART_VOICE_ENABLE*/
#define BUF_NUM 2
#endif

#if (TCFG_PSRAM_DEV_ENABLE)
#define LCD_FORMAT OUTPUT_FORMAT_RGB565
#else
#define LCD_FORMAT OUTPUT_FORMAT_RGB565
#endif


/*
** 初始化代码
*/
static const u8 lcd_qspi_ft2308_cmdlist[] ALIGNED(4) = 
{
#if 0
    _BEGIN_, 0xB2, 0x83, 0x12, _END_,
    //_BEGIN_, 0x51, 0xFF, 0x00, _END_,
    _BEGIN_, 0x2A, 0x00, 0x00, 0x01, 0x70, _END_,
    _BEGIN_, 0x2B, 0x00, 0x00, 0x01, 0xC0, _END_,
    _BEGIN_, 0x35, 0x00, _END_,
    _BEGIN_, 0x3A, 0x55, _END_,

    _BEGIN_, 0x11, 0x00, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x29, 0x00, _END_,
#endif

#if 1
    _BEGIN_, 0xFF, 0x00, 0x23, 0x08, 0x01, _END_,
    _BEGIN_, 0xFF, 0x80, 0x23, 0x08, _END_,

    _BEGIN_, 0xB2, 0x83, 0x12, _END_,
    //_BEGIN_, 0x51, 0x00, 0xff, 0x00, _END_,
    _BEGIN_, 0x2A, 0x00, 0x00, 0x00, 0x01, 0x70, _END_,
    _BEGIN_, 0x2B, 0x00, 0x00, 0x00, 0x01, 0xC0, _END_,
    _BEGIN_, 0x35, 0x00, 0x00, _END_,
    _BEGIN_, 0x3A, 0x55, _END_,
    
    _BEGIN_, 0x11, 0x00, 0x00, _END_,
    _BEGIN_, REGFLAG_DELAY, 120, _END_,
    _BEGIN_, 0x29, 0x00, 0x00, _END_,
#endif
};


static void lcd_adjust_display_brightness(u8 percent)
{
    u16 brightness;
    u8 para[2] = {0};

    brightness = (u16)((percent*(1.0f)/TCFG_BACKLIGHT_MAX_VAL) * \
        (0x1ff) + 0.5f);

    //printf("%s:0x%x\n", __func__, brightness);

    para[0] = brightness & 0xff;
    para[1] = (brightness >> 8) & 0x80;

    lcd_write_cmd(0x51, para, 2);

    return;
}

/*
** lcd背光控制
** 考虑到手表应用lcd背光控制需要更灵活自由，可能需要pwm调光，随时亮灭等
** 因此内部不操作lcd背光，全部由外部自行控制
*/
static int lcd_spi_ft2308_backlight_ctrl(u8 percent)
{
    if(percent)
    { 
        percent = percent < TCFG_BACKLIGHT_MIN_VAL? \
            TCFG_BACKLIGHT_MIN_VAL:percent;
        percent = percent > TCFG_BACKLIGHT_MAX_VAL? \
            TCFG_BACKLIGHT_MAX_VAL:percent;
    }

    lcd_adjust_display_brightness(percent);

    return 0;
}


/*
** lcd电源控制
*/
static int lcd_spi_ft2308_power_ctrl(u8 onoff)
{
    lcd_en_ctrl(onoff);

    return 0;
}


/*
** 设置lcd进入睡眠
*/
static void lcd_spi_ft2308_entersleep(void)
{
    lcd_write_cmd(0x28, NULL, 0);
    lcd_write_cmd(0x10, NULL, 0);
    delay_2ms(120 / 2);	// delay 120ms

    return;
}


/*
** 设置lcd退出睡眠
*/
static void lcd_spi_ft2308_exitsleep(void)
{
    lcd_write_cmd(0x11, NULL, 0);
    delay_2ms(5);
    lcd_write_cmd(0x29, NULL, 0);
    delay_2ms(5);

    return;
}

static u32 lcd_spi_ft2308_read_id(void)
{
    u8 id[3];
    lcd_read_cmd(0xDA, id, sizeof(id));

    return ((id[0] << 16) | (id[1] << 8) | id[2]);
}


struct imd_param lcd_spi_ft2308_param = 
{
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

REGISTER_LCD_DEVICE(ft2308) = 
{
    .logo = "ft2308",
    .row_addr_align    = 2,
    .column_addr_align = 2,

    .lcd_cmd = (void *) &lcd_qspi_ft2308_cmdlist,
    .cmd_cnt = sizeof(lcd_qspi_ft2308_cmdlist) / \
        sizeof(lcd_qspi_ft2308_cmdlist[0]),
    .param   = (void *) &lcd_spi_ft2308_param,

    .reset = NULL,	// 没有特殊的复位操作，用内部普通复位函数即可
    .backlight_ctrl = lcd_spi_ft2308_backlight_ctrl,
    .power_ctrl = lcd_spi_ft2308_power_ctrl,
    .entersleep = lcd_spi_ft2308_entersleep,
    .exitsleep = lcd_spi_ft2308_exitsleep,
    .read_id = lcd_spi_ft2308_read_id,
    .lcd_id = 0x000000,
};
#endif
