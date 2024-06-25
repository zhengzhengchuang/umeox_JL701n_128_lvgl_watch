/* Copyright(C)
 * not free
 * All right reserved
 *
 * @file imd.h
 * @brief IMD模块驱动API头文件。IMD模块为专门用于推屏的独立硬件模块，与硬件SPI无关。其配置仅在屏幕驱动的结构体中，推屏时钟的频率为内部通过推屏帧率计算而得，但时钟是由PLL进行分频而来，因此实际时钟频率仅会在分频组合允许的前提下尽可能接近所需的目标频率(实际测量的推屏时钟与计算并非绝对一致)。
 * @author zhuhaifang@zh-jieli.com
 * @version V201
 * @date 2023-01-10
 */

#ifndef __IMD_H__
#define __IMD_H__

#include "generic/typedef.h"


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 是否使用杰理UI框架，true 使用杰理UI框架，false 使用用户UI框架
 */
/* ------------------------------------------------------------------------------------*/
extern const int ENABLE_JL_UI_FRAME;


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 是否使用PSRAM，true 使用PSRAM，false 不使用PSRAM
 */
/* ------------------------------------------------------------------------------------*/
extern const int ENABLE_PSRAM_UI_FRAME;


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 显存buf组索引，卡片界面划屏时，多个页面同时存在，需使用多组显存buf，用此变量
 * 管理。注意：该变量内部使用，外部禁止操作，避免导致同步问题！
 */
/* ------------------------------------------------------------------------------------*/
extern u8 curr_index;


/* ------------------------------------------------------------------------------------*/
/**
 * @brief IMD模块私有变量。注意：该变量内部使用，外部禁止操作，否则会导致不可预测的问题。
 */
/* ------------------------------------------------------------------------------------*/
extern volatile struct imd_variable imd_var;


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 模块驱动接口组，IMD模块下管理了SPI, MCU, RGB三组驱动接口，具体驱动接口在这里
 * 注册给IMD模块管理使用。注意：该变量内部使用，外部无需修改，否则会导致不可预测的问题。
 */
/* ------------------------------------------------------------------------------------*/
struct imd_driver {
    void (*init)(struct imd_variable *priv);
    void (*write)(u32, u8 *, u8);
    void (*read)(u32, u8 *, u8);
    void (*set_draw_area)(int, int, int, int);
    void (*draw)();
    void (*isr)();
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief 屏幕驱动接口类型定义，根据屏幕的接口类型在屏驱内配置使用
 */
/* ------------------------------------------------------------------------------------*/
enum {
    IMD_DRV_SPI,	// SPI接口屏
    IMD_DRV_MCU,	// MCU接口屏
    IMD_DRV_RGB,	// RGB接口屏
    IMD_DRV_MAX,	// 无效，IMD不支持的接口类型
};


// <<<[lcd接口配置]>>>
#define SPI_MODE  (0<<4)
#define DSPI_MODE (1<<4)
#define QSPI_MODE (2<<4)

// SPI线数标志(clk + dat)
#define SPI_WIRE3  0
#define SPI_WIRE4  1

// QSPI的子模式标志(除SUBMODE0为1 dat线之外，其余模式都为4 dat线)
#define QSPI_SUBMODE0 0//0x02
#define QSPI_SUBMODE1 1//0x32
#define QSPI_SUBMODE2 2//0x12
#define QSPI_ST77903  3//0xde

// LCD时序标志(根据LCD数据手册配置，常用时序均已组合到LCD_DRIVE_CONFIG宏，在LCD驱动文件配置)
#define PIXEL_1P1T (0<<5)
#define PIXEL_1P2T (1<<5)
#define PIXEL_1P3T (2<<5)
#define PIXEL_2P3T (3<<5)

#define PIXEL_1T2B  1
#define PIXEL_1T6B  5
#define PIXEL_1T8B  7
#define PIXEL_1T9B  8
#define PIXEL_1T12B 11
#define PIXEL_1T16B 15
#define PIXEL_1T18B 17
#define PIXEL_1T24B 23

#define FORMAT_RGB565 1//0//1P2T
#define FORMAT_RGB666 2//1//1P3T
#define FORMAT_RGB888 0//2//1P3T

// IMD模块的SPI驱动IO组选择标志
#define SPI_PORTA 0
#define SPI_PORTB 1

// IMD模块及其管理的SPI、MCU、RGB模块中断操作宏
#define IMD_SPI_PND_IE()	SFR(JL_IMD->IMDSPI_CON0, 29, 1, 1)
#define IMD_SPI_PND_DIS()	SFR(JL_IMD->IMDSPI_CON0, 29, 1, 0)
#define IMD_SPI_PND_STA()   (JL_IMD->IMDSPI_CON0 & BIT(29))

#define IMD_PAP_PND_IE()	SFR(JL_IMD->IMDPAP_CON0, 29, 1, 1)
#define IMD_PAP_PND_DIS()	SFR(JL_IMD->IMDPAP_CON0, 29, 1, 0)
#define IMD_PAP_PND_STA()   (JL_IMD->IMDPAP_CON0 & BIT(29))

#define IMD_RGB_PND_IE()	SFR(JL_IMD->IMDRGB_CON0, 29, 1, 1)
#define IMD_RGB_PND_DIS()	SFR(JL_IMD->IMDRGB_CON0, 29, 1, 0)
#define IMD_RGB_PND_STA()   (JL_IMD->IMDRGB_CON0 & BIT(29))

#define IMD_PND_IE()        SFR(JL_IMD->IMD_CON0, 29, 1, 1)
#define IMD_PND_DIS()       SFR(JL_IMD->IMD_CON0, 29, 1, 0)
#define IMD_PND_STA()       (JL_IMD->IMD_CON0 & BIT(29))


#define SPI_MODE_UNIDIR  0//半双工，d0分时发送接收
#define SPI_MODE_BIDIR   1//全双工，d0发送、d1接收

enum {
    PIN_NONE,
    PIN_SYNC0_PA4,
    PIN_SYNC1_PA5,
    PIN_SYNC2_PA6,
};

enum {
    SYNC0_SEL_NONE,
    SYNC0_SEL_RGB_SYNC0,
    SYNC0_SEL_PAP_WR,
    SYNC0_SEL_PAP_RD,
};

enum {
    SYNC1_SEL_NONE,
    SYNC1_SEL_RGB_SYNC1,
    SYNC1_SEL_PAP_WR,
    SYNC1_SEL_PAP_RD,
};

enum {
    SYNC2_SEL_NONE,
    SYNC2_SEL_RGB_SYNC2,
    SYNC2_SEL_PAP_WR,
    SYNC2_SEL_PAP_RD,
};

enum {
    CMD_MODE,
};

enum {
    CMD_8BIT,
    CMD_16BIT,
    CMD_24BIT,
};

/*
** imd私有全局变量定义
*/

struct L_WEAxx_timing {
    u8 enable;
    u8 state;
    u8 mode;
    u8 *buf;
    u16 width;
    u16 height;
    u16 stride;
};

struct imd_variable {
    u8 imd_pnd;		// imd中断标志
    volatile u8 imd_busy;	// imd忙碌标志
    volatile u8 imd_lock;
    u8 te_ext;		// te中断标志
    u8 clock_init;	// imd时钟初始化标志
    u8 sfr_save;	// 寄存器保存标志
    u8 row_align;   // 行对齐
    u8 column_align;// 列对齐
    u32 imd_sfr[17];
    u32 clock_source;
    void (*dc_ctrl)(u8 val);
    void (*cs_ctrl)(u8 val);
    int (*te_stat)();
    struct imd_param *param;
    void (*imd_callback)(int err);//imd帧回调
    struct L_WEAxx_timing l_wea;
    u8 cmd_mode;
};

// LCD屏幕类型
typedef enum lcd_type_cfg {
    LCD_TYPE_SPI,	// SPI屏
    LCD_TYPE_MCU,	// MCU屏
    LCD_TYPE_RGB,	// RGB屏
} LCD_TYPE;

struct imd_param {
    // 显示区域相关(用于配置推屏区域，可不推全屏)
    int scr_x;
    int scr_y;
    int scr_w;
    int scr_h;

    // lcd配置
    int lcd_width;
    int lcd_height;
    LCD_TYPE lcd_type;	// LCD接口类型：MCU, RGB, SPI

    // 显存配置
    int buffer_num;
    int buffer_size;
    int buffer_total_size;
    u8 *buffer;

    // imd模块的输入，与imb模块一起用时，也是imb模块的输出
    int in_width;
    int in_height;
    int in_format;
    int in_stride;

    // debug模式(推新屏时，可使能debug模式，先推纯色测试)
    int debug_mode_en;
    int debug_mode_color;

    // te功能(目前已将TE功能移植到板卡的LCD IO配置中，这里配置无效)
    int te_en;		// TE信号使能
    int te_port;	// te的io

    // 帧率配置(用于计算IMD模块时钟频率，但实际推屏帧率仅会接近配置，并非100%一致)
    int fps;

    // 以下为三种屏驱动相关配置，out_format为屏幕的像素格式类型
    struct spi_param {
        int spi_mode;
        int pixel_type;
        int out_format;
        int port;
        int spi_dat_mode;
        struct qspi_cmd {
            u8 write_cmd;
            u8 read_cmd;
            u8 submode0_cmd;//0x2c/0x3c/数据单线, default value 0x02
            u8 submode1_cmd;//0x2c/0x3c单线，数据四线，default value 0x32
            u8 submode2_cmd;//0x2c/0x3c/数据四线, default value 0x12
        } qspi_cmd;
        u8 caset_cmd;//Column Address Set, default value 0x2a
        u8 paset_cmd;//Page Address Set, default value 0x2b
        u8 ramwr_cmd;//Memory Write Start, default value 0x2c
        u8 ramwrc_cmd;//Memory Write Continue, default valus 0x3c
    } spi;

    struct pap_param {
        int out_format;
        int right_shift_2bit;
        int dat_l2h_en;
        int wr_sel;
        int rd_sel;
    } pap;

    struct rgb_param {
        int dat_l2h;
        int dat_sf;
        int hv_mode;
        int xor_edge;
        int out_format;
        int continue_frames;

        int hsync_sel;
        int vsync_sel;
        int den_sel;

        int hpw_prd;
        int hbw_prd;
        int hfw_prd;
        int vpw_prd;
        int vbw_prd;
        int vfw_prd;
    } rgb;
};

struct lcd_st77903_info {
    int lcd_sem;
    int refresh_mode;
    int lcd_draw_stage;
    int lcd_w;
    int lcd_h;
    int lcd_stride;
    int back_porch;
    int front_porch;
    int back_porch_cnt;
    int front_porch_cnt;
    int draw_status;
    u8 *pixel;
    int h;
    u8 *p;
    int idle;
    int idle_cnt;
    int stop_pending;
    int frame_mode;
};

enum FRAME_MODE {
    FRAME_SINGAL,
    FRAME_CONTINUE,
};

enum {
    DRAW_BEGIN,
    DRAW_END,
};
/* ------------------------------------------------------------------------------------*/
/**
 * @brief IMD向LCD写大量数据时的数据类型定义
 */
/* ------------------------------------------------------------------------------------*/
enum {
    LCD_COLOR_MODE,		// 颜色模式，该模式下IMD直接发送指定颜色值，无需使用内部RAM
    LCD_DATA_MODE,		// 数据模式，该模式下IMD向LCD发送指定BUF的数据，需要内部RAM
};


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_init IMD模块初始化(注意：需要操作IMD或从IMD获取信息时，必须先初始化)
 *
 * @param param IMD配置句柄
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int  lcd_init(struct imd_param *param);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_write_cmd IMD模块向LCD指定地址写数据
 *
 * @param cmd 待操作的LCD命令地址
 * @param buf 数据缓存buf（如果不需要写数据，可传入NULL）
 * @param len 数据缓存长度（如果不写数据，需将len配置为0）
 */
/* ------------------------------------------------------------------------------------*/
void lcd_write_cmd(u32 cmd, u8 *buf, u8 len);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_read_cmd IMD模块从LCD指定地址读数据
 *
 * @param cmd 待操作的LCD命令地址
 * @param buf 数据缓存buf（输出）
 * @param len 读取的数据长度（需与buf匹配，不可超过buf大小）
 */
/* ------------------------------------------------------------------------------------*/
void lcd_read_cmd(u32 cmd, u8 *buf, u8 len);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_set_draw_area 设置LCD显示区域（2A, 2B命令）
 *
 * @param xstart X起始像素
 * @param xend X结束像素
 * @param ystart Y起始像素
 * @param yend Y结束像素
 */
/* ------------------------------------------------------------------------------------*/
void lcd_set_draw_area(int xstart, int xend, int ystart, int yend);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_draw IMD硬件写数据到LCD（推buf或纯色填充）
 *
 * @param mode IMD写数据类型(LCD_COLOR_MODE或LCD_DATA_MODE)
 * @param priv 待写出数据
 * 如果mode为COLOR_MODE，priv为RGB888颜色值；
 * 如果mode为DATA_MODE，priv为待发送数据的缓存buf指针。
 */
/* ------------------------------------------------------------------------------------*/
void lcd_draw(u8 mode, u32 priv);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_full_clear 纯色填充LCD，自动将这个LCD填充为指定颜色
 *
 * @param color 待填充的RGB888颜色值
 */
/* ------------------------------------------------------------------------------------*/
void lcd_full_clear(u32 color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_clear 纯色填充LCD指定区域
 *
 * @param xstart 待填充的X起始像素
 * @param xend 待填充的X结束像素
 * @param ystart 待填充的Y起始像素
 * @param yend 待填充的Y结束像素
 * @param color 待填充的RGB888颜色值
 */
/* ------------------------------------------------------------------------------------*/
void lcd_clear(u16 xstart, u16 xend, u16 ystart, u16 yend, u32 color);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_set_ctrl_pin_func 配置LCD控制脚操作函数
 *
 * @param dc_ctrl DC脚控制函数
 * @param cs_ctrl CS脚控制函数
 * @param te_stat TE脚控制函数
 */
/* ------------------------------------------------------------------------------------*/
void lcd_set_ctrl_pin_func(void (*dc_ctrl)(u8), void (*cs_ctrl)(u8), int (*te_stat)());


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_set_align 设置LCD对齐参数
 *
 * @param row_align 行对齐
 * @param column_align 列对齐
 */
/* ------------------------------------------------------------------------------------*/
void lcd_set_align(u8 row_align, u8 column_align);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_wait LCD繁忙等待函数，外部需要操作LCD时，先调用该接口等待LCD空闲再操作，
 * 该函数退出后LCD即为空闲状态。
 */
/* ------------------------------------------------------------------------------------*/
void lcd_wait();


int  lcd_draw_set_callback(void (*callback)(int err));


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_draw_rect IMD将指定buf推送到LCD指定区域
 *
 * @param lcd_buf 待推送BUF
 * @param stride IMD 模块 stride 参数(像素宽度*颜色格式(byte)，再4字节对齐)
 * @param left LCD区域的X起始坐标
 * @param top LCD区域的Y起始坐标
 * @param width LCD区域的宽度
 * @param height LCD区域的高度
 * @param wait 是否等待TE信号同步
 *
 * @return 0
 */
/* ------------------------------------------------------------------------------------*/
int  lcd_draw_rect(u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait);
int  lcd_draw_area_stride(u8 *lcd_buf, int left, int top, int width, int height, int stride, int xoffset, int yoffset, int wait);
void lcd_set_param(u8 mode, int priv);
int lcd_get_param(u8 mode);
//st77903
void *lcd_qspi_st77903_get_info();
int lcd_qspi_st77903_mode();
void lcd_qspi_st77903_start(u8 *pixel, int lcd_w, int lcd_h, int lcd_stride, int back_porch, int front_porch);
void lcd_qspi_st77903_stop();
void lcd_qspi_st77903_refresh_mode_set(int mode);
int lcd_qspi_st77903_refresh_mode_get();
void lcd_qspi_st77903_idle_line_set(int idle);
void lcd_qspi_st77903_frame_mode_set(int mode);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_qspi_st77903_mode LCD驱动是否配置为QSPI的77903接口
 *
 * @return 1 是，0 否
 */
/* ------------------------------------------------------------------------------------*/
int  lcd_qspi_st77903_mode();


/* ------------------------------------------------------------------------------------*/
/**
 * @brief lcd_spi_draw_st77903 77903推屏API
 *
 * @param pixel
 * @param lcd_w
 * @param lcd_h
 * @param lcd_stride
 * @param back_porch
 * @param front_porch
 */
/* ------------------------------------------------------------------------------------*/
void lcd_spi_draw_st77903(u8 *pixel, int lcd_w, int lcd_h, int lcd_stride, int back_porch, int front_porch);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief get_lcd_width_from_imd 从IMD参数中获取LCD屏幕宽度(注意：需先初始化IMD才有效)
 *
 * @return 从屏驱中配置到IMD模块的LCD宽度
 */
/* ------------------------------------------------------------------------------------*/
u16 get_lcd_width_from_imd(void);


/* ------------------------------------------------------------------------------------*/
/**
 * @brief get_lcd_height_from_imd 从IMD参数中获取LCD屏幕高度(注意：需先初始化IMD才有效)
 *
 * @return 从屏驱中配置到IMD模块的LCD高度
 */
/* ------------------------------------------------------------------------------------*/
u16 get_lcd_height_from_imd(void);

int ui_st77903_init();
int ui_st77903_uninit();

#endif






