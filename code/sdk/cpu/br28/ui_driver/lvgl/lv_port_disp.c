/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lvgl.h"
#include "ui/ui_api.h"
#include "asm/imd.h"
#include "app_config.h"

#if LVGL_TEST_ENABLE

/*********************
 *      DEFINES
 *********************/
#if 0
#define MY_DISP_HOR_RES 		454//240
#define MY_DISP_VER_RES			454//240
#define MY_DISP_VLOCK_H			60
#endif

extern void lvgl_disp_init(void *arg, void **buf1, void **buf2, \
    int *lcd_w,int *lcd_h, int *colums, int *lines);

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void *lcd);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
extern int lcd_draw_area(u8 index, u8 *lcd_buf, int left, int top, int width, int height, int wait);

void lv_port_disp_init(void *lcd)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
#if USR_LVGL_IMB2D_EN
    //  LCD句柄     BUF1    BUF2    内存行  内存列
    void *buf_2_1;
    void *buf_2_2;
    int lcd_w;
    int lcd_h;
    int colums;
    int lines;
    lvgl_disp_init(lcd, &buf_2_1, &buf_2_2, &lcd_w, &lcd_h, &colums, &lines);
#else
    disp_init(lcd);
#endif

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* Example for 1) */
    /* static lv_disp_draw_buf_t draw_buf_dsc_1; */
    /* static lv_color_t buf_1[MY_DISP_HOR_RES * 10];                          [>A buffer for 10 rows<] */
    /* lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 10);   [>Initialize the display buffer<] */

    /* Example for 2) */
#if USR_LVGL_IMB2D_EN
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    if(buf_2_2 == NULL){    //单BUFF整屏
        lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, NULL, colums * lines);   /*Initialize the display buffer*/
    } else {    //双BUFF分块
        lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, colums * lines);   /*Initialize the display buffer*/
    }
#else 
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[MY_DISP_HOR_RES * MY_DISP_VLOCK_H];
    static lv_color_t buf_2_2[MY_DISP_HOR_RES * MY_DISP_VLOCK_H];
    /* y_printf("buf_2_1:0x%x, 0x%x \n", buf_2_1, buf_2_2); */
    lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * MY_DISP_VLOCK_H);   /*Initialize the display buffer*/
#endif
    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
#if USR_LVGL_IMB2D_EN
    disp_drv.hor_res = lcd_w;
    disp_drv.ver_res = lcd_h;
#else 
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
#endif

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_2;

    /*Required for Example 3)*/
    // disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

extern int lcd_drv_init(void *p);
/*Initialize your display and the required peripherals.*/
static void disp_init(void *lcd)
{
    /*You code here*/
    lcd_drv_init(lcd);
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

    /* printf("%s,%d \n", __func__, __LINE__); */
#if 0
    int32_t x;
    int32_t y;
    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            /*Put a pixel to the display. For example:*/
            /*put_px(x, y, *color_p)*/
            color_p++;
        }
    }
#else
    int left	= area->x1;
    int top		= area->y1;
    int width	= area->x2 - area->x1 + 1;
    int height	= area->y2 - area->y1 + 1;
    //g_printf("[%d,%d,%d,%d]", left, top, width, height);

//  休眠判断,休眠不能跑硬件推屏
    if(lcd_sleep_status()){
        g_printf("sleep can't run imd");
        return ;
    }

    extern volatile u8 usr_full_screen;

    if (PSRAM_FULL_SCREEN && usr_full_screen){
        // lcd_wait();
        extern void lv_lcd_data_copy(u8 *lcd_buf, int left, int top, int width, int height);
        lv_lcd_data_copy(color_p,  left,  top,  width,  height);
    }else    
    {
        if((color_p >= 0x2000000) && (color_p < (0x2000000 + 0x1000000))){
            #if TCFG_PSRAM_DEV_ENABLE
            psram_flush_invaild_cache(color_p, width*height*2);        
            lcd_draw_area(0, psram_cache2nocache_addr(color_p), left, top, width, height, top==0);
            #endif
        } else 
        {
            u8 WaitTe = GetUsrWaitTe();
            lcd_draw_area(0, color_p, left, top, width, height, WaitTe?top==0:0);   
            if(disp_drv->draw_buf->buf2 == NULL){   //单buf整屏需要等待，双buf不需要等
                lcd_wait();  
            }
        }
    }
    
    /* lcd_draw_area(0, NULL, left, top, width, height); */
#endif

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

#endif /* #if LVGL_TEST_ENABLE */

