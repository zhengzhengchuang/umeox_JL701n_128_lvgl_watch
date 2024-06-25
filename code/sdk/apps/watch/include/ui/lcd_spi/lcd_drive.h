#ifndef __SPI_LCD_DRIVER_H
#define __SPI_LCD_DRIVER_H


#include "asm/spi.h"
#include "os/os_api.h"

#define LCD_WIDTH  (320)
#define LCD_HEIGHT (386)

#ifdef Reset
#undef Reset
#endif

#ifndef SPI_LCD_DEBUG_ENABLE
#define SPI_LCD_DEBUG_ENABLE	0
#endif

#if (SPI_LCD_DEBUG_ENABLE == 0)
#define lcd_d(...)
#define lcd_w(...)
#define lcd_e(fmt, ...)	printf("[LCD ERROR]: "fmt, ##__VA_ARGS__)
#elif (SPI_LCD_DEBUG_ENABLE == 1)
#define lcd_d(...)
#define lcd_w(fmt, ...)	printf("[LCD WARNING]: "fmt, ##__VA_ARGS__)
#define lcd_e(fmt, ...)	printf("[LCD ERROR]: "fmt, ##__VA_ARGS__)
#else
#define lcd_d(fmt, ...)	printf("[LCD DEBUG]: "fmt, ##__VA_ARGS__)
#define lcd_w(fmt, ...)	printf("[LCD WARNING]: "fmt, ##__VA_ARGS__)
#define lcd_e(fmt, ...)	printf("[LCD ERROR]: "fmt, ##__VA_ARGS__)
#endif


// 注意：以下配置组合为固定搭配，不可随意更改
// 切换配置使用上面的宏选择
#if ((defined CONFIG_CPU_BR27) || (defined CONFIG_CPU_BR28))
#include "asm/imd.h"
#endif

#if defined(CONFIG_CPU_BR28) || defined(CONFIG_CPU_BR27) || defined(CONFIG_CPU_BR23)
#include "app_config.h"

#define SPI_SUBMODE(config)     (((config)>>16)&0xf0)
#define SPI_WIRE(config)        (((config)>>16)&0x0f)
#define PIXEL_nPnT(config)      (((config))&0xe0)
#define PIXEL_nTnB(config)      (((config))&0x1f)
#define SPI_IF_MODE(config)     (((config)>>16)&0xff)
#define OUT_FORMAT(config)      (((config)>>8)&0xff)
#define PIXEL_TYPE(config)      (((config))&0xff)
#define LCD_CONFIG(mode, format, type) (((mode)<<16) | ((format)<<8) | (type))
/////////////////////////////////////////////spi 3wire///////////////////////////////////////////////
#define SPI_3WIRE_RGB888_1T8B  LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB888, PIXEL_1P3T|PIXEL_1T8B)
#define SPI_3WIRE_RGB888_1T24B LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB888, PIXEL_1P1T|PIXEL_1T24B)
#define SPI_3WIRE_RGB666_1T8B  LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB666, PIXEL_1P3T|PIXEL_1T8B)
#define SPI_3WIRE_RGB666_1T18B LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB666, PIXEL_1P1T|PIXEL_1T18B)
#define SPI_3WIRE_RGB565_1T8B  LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB565, PIXEL_1P2T|PIXEL_1T8B)
#define SPI_3WIRE_RGB565_1T16B LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB565, PIXEL_1P1T|PIXEL_1T16B)
/////////////////////////////////////////////spi 4wire///////////////////////////////////////////////
#define SPI_4WIRE_RGB888_1T8B  LCD_CONFIG(SPI_MODE|SPI_WIRE4, FORMAT_RGB888, PIXEL_1P3T|PIXEL_1T8B)
#define SPI_4WIRE_RGB888_1T24B LCD_CONFIG(SPI_MODE|SPI_WIRE4, FORMAT_RGB888, PIXEL_1P1T|PIXEL_1T24B)
#define SPI_4WIRE_RGB666_1T8B  LCD_CONFIG(SPI_MODE|SPI_WIRE4, FORMAT_RGB666, PIXEL_1P3T|PIXEL_1T8B)
#define SPI_4WIRE_RGB666_1T18B LCD_CONFIG(SPI_MODE|SPI_WIRE4, FORMAT_RGB666, PIXEL_1P1T|PIXEL_1T18B)
#define SPI_4WIRE_RGB565_1T8B  LCD_CONFIG(SPI_MODE|SPI_WIRE4, FORMAT_RGB565, PIXEL_1P2T|PIXEL_1T8B)
#define SPI_4WIRE_RGB565_1T16B LCD_CONFIG(SPI_MODE|SPI_WIRE4, FORMAT_RGB565, PIXEL_1P1T|PIXEL_1T16B)
////////////////////////////////////////////dspi 3wire///////////////////////////////////////////////
#define DSPI_3WIRE_RGB565_1T8B  LCD_CONFIG(DSPI_MODE|SPI_WIRE3, FORMAT_RGB565, PIXEL_1P1T|PIXEL_1T8B)
#define	DSPI_3WIRE_RGB666_1T9B  LCD_CONFIG(DSPI_MODE|SPI_WIRE3, FORMAT_RGB666, PIXEL_1P1T|PIXEL_1T9B)
#define DSPI_3WIRE_RGB666_1T6B  LCD_CONFIG(DSPI_MODE|SPI_WIRE3, FORMAT_RGB666, PIXEL_2P3T|PIXEL_1T6B)
#define DSPI_3WIRE_RGB888_1T12B LCD_CONFIG(DSPI_MODE|SPI_WIRE3, FORMAT_RGB888, PIXEL_1P1T|PIXEL_1T12B)
#define	DSPI_3WIRE_RGB888_1T8B  LCD_CONFIG(DSPI_MODE|SPI_WIRE3, FORMAT_RGB888, PIXEL_2P3T|PIXEL_1T8B)
////////////////////////////////////////////dspi 4wire///////////////////////////////////////////////
#define	DSPI_4WIRE_RGB565_1T8B  LCD_CONFIG(DSPI_MODE|SPI_WIRE4, FORMAT_RGB565, PIXEL_1P1T|PIXEL_1T8B)
#define DSPI_4WIRE_RGB666_1T9B  LCD_CONFIG(DSPI_MODE|SPI_WIRE4, FORMAT_RGB666, PIXEL_1P1T|PIXEL_1T9B)
#define DSPI_4WIRE_RGB666_1T6B  LCD_CONFIG(DSPI_MODE|SPI_WIRE4, FORMAT_RGB666, PIXEL_2P3T|PIXEL_1T6B)
#define DSPI_4WIRE_RGB888_1T12B LCD_CONFIG(DSPI_MODE|SPI_WIRE4, FORMAT_RGB888, PIXEL_1P1T|PIXEL_1T12B)
#define DSPI_4WIRE_RGB888_1T8B  LCD_CONFIG(DSPI_MODE|SPI_WIRE4, FORMAT_RGB888, PIXEL_2P3T|PIXEL_1T8B)
///////////////////////////////////////////////qspi//////////////////////////////////////////////////
#define QSPI_RGB565_SUBMODE0_1T8B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE0, FORMAT_RGB565, PIXEL_1P2T|PIXEL_1T8B)
#define QSPI_RGB666_SUBMODE0_1T8B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE0, FORMAT_RGB666, PIXEL_1P3T|PIXEL_1T8B)
#define QSPI_RGB888_SUBMODE0_1T8B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE0, FORMAT_RGB888, PIXEL_1P3T|PIXEL_1T8B)
#define QSPI_RGB565_SUBMODE1_1T2B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE1, FORMAT_RGB565, PIXEL_1P2T|PIXEL_1T2B)
#define QSPI_RGB666_SUBMODE1_1T2B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE1, FORMAT_RGB666, PIXEL_1P3T|PIXEL_1T2B)
#define QSPI_RGB888_SUBMODE1_1T2B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE1, FORMAT_RGB888, PIXEL_1P3T|PIXEL_1T2B)
#define QSPI_RGB565_SUBMODE2_1T2B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE2, FORMAT_RGB565, PIXEL_1P2T|PIXEL_1T2B)
#define QSPI_RGB666_SUBMODE2_1T2B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE2, FORMAT_RGB666, PIXEL_1P3T|PIXEL_1T2B)
#define QSPI_RGB888_SUBMODE2_1T2B LCD_CONFIG(QSPI_MODE|QSPI_SUBMODE2, FORMAT_RGB888, PIXEL_1P3T|PIXEL_1T2B)
#define QSPI_RGB565_ST77903_1T2B  LCD_CONFIG(QSPI_MODE|QSPI_ST77903,  FORMAT_RGB565, PIXEL_1P2T|PIXEL_1T2B)
#define QSPI_RGB666_ST77903_1T2B  LCD_CONFIG(QSPI_MODE|QSPI_ST77903,  FORMAT_RGB666, PIXEL_1P3T|PIXEL_1T2B)
#define QSPI_RGB888_ST77903_1T2B  LCD_CONFIG(QSPI_MODE|QSPI_ST77903,  FORMAT_RGB888, PIXEL_1P3T|PIXEL_1T2B)
#define RGB_SPI_3WIRE_RGB565      LCD_CONFIG(SPI_MODE|SPI_WIRE3, FORMAT_RGB565, 0)
#define MCU_8BITS_RGB565  LCD_CONFIG(0, FORMAT_RGB565, 0)
#define MCU_8BITS_RGB666  LCD_CONFIG(0, FORMAT_RGB666, 0)
#define MCU_8BITS_RGB888  LCD_CONFIG(0, FORMAT_RGB888, 0)


#if 0
////////////////////////////////spi 3wire///////////////////////////
#if (LCD_DRIVE_CONFIG == SPI_3WIRE_RGB888_1T8B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == SPI_3WIRE_RGB888_1T24B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T24B)

#elif (LCD_DRIVE_CONFIG == SPI_3WIRE_RGB666_1T8B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == SPI_3WIRE_RGB666_1T18B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T18B)

#elif (LCD_DRIVE_CONFIG == SPI_3WIRE_RGB565_1T8B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P2T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == SPI_3WIRE_RGB565_1T16B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T16B)


////////////////////////////////spi 4wire///////////////////////////
#elif (LCD_DRIVE_CONFIG == SPI_4WIRE_RGB888_1T8B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == SPI_4WIRE_RGB888_1T24B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T24B)

#elif (LCD_DRIVE_CONFIG == SPI_4WIRE_RGB666_1T8B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == SPI_4WIRE_RGB666_1T18B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T18B)

#elif (LCD_DRIVE_CONFIG == SPI_4WIRE_RGB565_1T8B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P2T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == SPI_4WIRE_RGB565_1T16B)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T16B)

////////////////////////////////dspi 3wire///////////////////////////
#elif (LCD_DRIVE_CONFIG == 	DSPI_3WIRE_RGB565_1T8B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == 	DSPI_3WIRE_RGB666_1T9B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T9B)

#elif (LCD_DRIVE_CONFIG == 	DSPI_3WIRE_RGB666_1T6B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_2P3T|PIXEL_1T6B)

#elif (LCD_DRIVE_CONFIG == 	DSPI_3WIRE_RGB888_1T12B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T12B)

#elif (LCD_DRIVE_CONFIG == 	DSPI_3WIRE_RGB888_1T8B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_2P3T|PIXEL_1T8B)

//////////////////////////////dspi 4wire///////////////////////////
#elif (LCD_DRIVE_CONFIG == 	DSPI_4WIRE_RGB565_1T8B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == DSPI_4WIRE_RGB666_1T9B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T9B)

#elif (LCD_DRIVE_CONFIG == DSPI_4WIRE_RGB666_1T6B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_2P3T|PIXEL_1T6B)

#elif (LCD_DRIVE_CONFIG == DSPI_4WIRE_RGB888_1T12B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P1T|PIXEL_1T12B)

#elif (LCD_DRIVE_CONFIG == DSPI_4WIRE_RGB888_1T8B)
#define SPI_IF_MODE     (DSPI_MODE|SPI_WIRE4)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_2P3T|PIXEL_1T8B)

//////////////////////////////qspi///////////////////////////
#elif (LCD_DRIVE_CONFIG == QSPI_RGB565_SUBMODE0_1T8B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE0)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P2T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB666_SUBMODE0_1T8B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE0)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB888_SUBMODE0_1T8B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE0)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T8B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB565_SUBMODE1_1T2B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE1)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P2T|PIXEL_1T2B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB666_SUBMODE1_1T2B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE1)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T2B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB888_SUBMODE1_1T2B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE1)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T2B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB565_SUBMODE2_1T2B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE2)
#define OUT_FORMAT      FORMAT_RGB565
#define PIXEL_TYPE      (PIXEL_1P2T|PIXEL_1T2B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB666_SUBMODE2_1T2B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE2)
#define OUT_FORMAT      FORMAT_RGB666
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T2B)

#elif (LCD_DRIVE_CONFIG == QSPI_RGB888_SUBMODE2_1T2B)
#define SPI_IF_MODE     (QSPI_MODE|QSPI_SUBMODE2)
#define OUT_FORMAT      FORMAT_RGB888
#define PIXEL_TYPE      (PIXEL_1P3T|PIXEL_1T2B)

//////////////////////////////rgb lcd///////////////////////////
#elif (LCD_DRIVE_CONFIG == RGB_3WIRE_RGB565)
#define SPI_IF_MODE     (SPI_MODE|SPI_WIRE3)
#define OUT_FORMAT      FORMAT_RGB565

//////////////////////////////pap lcd///////////////////////////
#elif (LCD_DRIVE_CONFIG == PAP_RGB888)
#define OUT_FORMAT      FORMAT_RGB888

#else
#error "error! undefined lcd drive!, see lcd_drive.h"
#endif
#endif

#endif // #if defined(CONFIG_CPU_BR28)
//////////////////////////////lcd mode end///////////////////////////
//~~~~~~~~~~~~~~~~~~~~~~~屏驱相关的参数和结构体~~~~~~~~~~~~~~~~~~~~~~~~~~//
// 屏幕初始化代码延时标志

#ifndef REGFLAG_DELAY
#define REGFLAG_DELAY_FLAG  0xff5aa5ff
#define REGFLAG_DELAY       ((REGFLAG_DELAY_FLAG>>24)&0xff),((REGFLAG_DELAY_FLAG>>16)&0xff),((REGFLAG_DELAY_FLAG>>8)&0xff),(REGFLAG_DELAY_FLAG&0xff)
#endif

#ifndef REGFLAG_CONFIRM
#define REGFLAG_CONFIRM_FLAG  0xff5bb5ff
#define REGFLAG_CONFIRM       ((REGFLAG_CONFIRM_FLAG>>24)&0xff),((REGFLAG_CONFIRM_FLAG>>16)&0xff),((REGFLAG_CONFIRM_FLAG>>8)&0xff),(REGFLAG_CONFIRM_FLAG&0xff)
#endif


// 区分屏幕初始化代码开始和结束的标志
#define BEGIN_FLAG          0x12345678
#define END_FLAG            0x87654321
#define _BEGIN_             ((BEGIN_FLAG>>24)&0xff),((BEGIN_FLAG>>16)&0xff),((BEGIN_FLAG>>8)&0xff),(BEGIN_FLAG&0xff)
#define _END_               ((END_FLAG>>24)&0xff),((END_FLAG>>16)&0xff),((END_FLAG>>8)&0xff),(END_FLAG&0xff)

// 初始化代码结构体
struct lcd_cmd {
    u8 addr;		// 地址
    u8 param_cnt;	// 参数个数
    u8 param[64];	// 参数
};

/* 定义初始化数据结构体 */
typedef struct {
    u8 cmd;		// 地址
    u8 cnt;		// 数据个数
    u8 dat[64];	// 数据
} InitCode;


struct spi_lcd_init {
    char *name;			// 名称
    u8 spi_pending;
    u8 soft_spi;
    u16 lcd_width;
    u16 lcd_height;
    u8  color_format;
    u8  interface;
    u8  column_addr_align;
    u8  row_addr_align;
    u8  backlight_status;
    u8  *dispbuf;
    u32 bufsize;
    InitCode *initcode;	// 初始化代码
    u16 initcode_cnt;	// 初始化代码条数
    void (*Init)(void);
    void (*WriteComm)(u16 cmd);	// 写命令
    void (*WriteData)(u16 dat);	// 写数据
    void (*WriteMap)(char *map, int size);	// 写整个buf
    void (*WritePAGE)(char *map, u8 page_star, u8 page_len);	// 写page
    void (*SetDrawArea)(int, int, int, int);
    void (*Reset)(void);
    void (*BackLightCtrl)(u8);
    void (*EnterSleep)();
    void (*ExitSleep)();
};

// LCD 初始化接口
#define REGISTER_LCD_DRIVE() \
	const struct spi_lcd_init dev_drive

struct lcd_drive {
    char *logo;

    u8 column_addr_align;
    u8 row_addr_align;

    // 初始化寄存器
    u8 *lcd_cmd;
    int cmd_cnt;

    //显存
    u8 *lcd_buf;
    u8 *lcd_dbuf[2];
    u8 lcd_dbuf_index;
    u8 lcd_buf_num;
    u32 lcd_buf_size;
    u8 lcd_busy;
    u8 lcd_exit;
    u8 lcd_enter;
    u8 lcd_switch_mode;

    // 配置参数
    void *param;

    // 应用层函数
    void (*reset)(void); /* 复位函数 */
    int (*backlight_ctrl)(u8);
    int (*power_ctrl)(u8);
    void (*entersleep)(void);
    void (*exitsleep)(void);
    u32(*read_id)(void);

    OS_SEM init_sem;
    int lcd_reinit;
    int tp_reinit;

    u32 lcd_id; //屏幕id
};

#define REGISTER_LCD_DEVICE(lcd) \
	struct lcd_drive lcd sec(.lcd_device_info) __attribute__((used))

extern struct lcd_drive lcd_device_begin[];
extern struct lcd_drive lcd_device_end[];



struct lcd_platform_data {
    u32  pin_reset;
    u32  pin_cs;
    u32  pin_dc;
    u32  pin_en;
    u32  pin_bl;
    u32  pin_te;

    struct mcu_pins {
        u32 pin_wr;
        u32 pin_rd;
    } mcu_pins;

    struct rgb_pins {
        u32 pin_hsync;
        u32 pin_vsync;
        u32 pin_den;
    } rgb_pins;

    spi_dev spi_cfg;
    const struct spi_platform_data *spi_pdata;
};


enum LCD_COLOR {
    LCD_COLOR_RGB888,
    LCD_COLOR_RGB565,
    LCD_COLOR_MONO,
};

enum LCD_IF {
    LCD_SPI,
    LCD_MCU,
    LCD_RGB,
    LCD_EMI,
};

struct lcd_info {
    u16 width;
    u16 height;
    u16 stride;
    u8 color_format;
    u8 interface;
    u8 col_align;
    u8 row_align;
    u8 buf_num;
    u8 bl_status;
    u8 *buffer;
    int buffer_size;
};

struct lcd_interface {
    void (*init)(void *);
    void (*get_screen_info)(struct lcd_info *info);
    void (*buffer_malloc)(u8 **buf, u32 *size);
    void (*buffer_free)(u8 *buf);
    void (*draw)(u8 *buf, u32 len, u8 wait);
    void (*set_draw_area)(u16 xs, u16 xe, u16 ys, u16 ye);
    void (*clear_screen)(u32 color);
    int (*backlight_ctrl)(u8 on);
    int (*power_ctrl)(u8 on);
    void (*draw_page)(u8 *buf, u8 page_star, u8 page_len);//刷新页(点阵屏)
};

extern struct lcd_interface lcd_interface_begin[];
extern struct lcd_interface lcd_interface_end[];

#define REGISTER_LCD_INTERFACE(lcd) \
    static const struct lcd_interface lcd sec(.lcd_if_info) __attribute__((used))

struct lcd_interface *lcd_get_hdl();

#define LCD_SPI_PLATFORM_DATA_BEGIN(data) \
		const struct lcd_platform_data data = {

#define LCD_SPI__PLATFORM_DATA_END() \
};

extern struct pwm_platform_data lcd_pwm_p_data;



void lcd_reset_l();
void lcd_reset_h();
void lcd_cs_l();
void lcd_cs_h();
void lcd_rs_l();
void lcd_rs_h();
void lcd_bl_l();
void lcd_bl_h();
u8   lcd_bl_io();

//void lcd_bl_ctrl(u8 val);
void lcd_en_ctrl(u8 val);
int ldo_power_ctrl(uint8_t on);

int lcd_sleep_status();

// int lcd_drv_backlight_ctrl(u8 percent);
// int lcd_drv_power_ctrl(u8 on);
int lcd_backlight_ctrl(u8 on);
void lcd_drv_cmd_list(u8 *cmd_list, int cmd_cnt);
void lcd_mcpwm_init();

struct lcd_drive *lcd_drv_get_hdl(u8 mode, const char *logo);
struct lcd_drive *lcd_drv_get_hdl_by_id();
struct lcd_drive *lcd_drv_get_hdl_by_logo(const char *logo);
int lcd_drv_get_info(void *info);

// 两毫秒延时
extern void delay_2ms(int cnt);


#endif




