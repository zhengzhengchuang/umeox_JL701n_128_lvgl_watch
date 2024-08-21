#include "system/includes.h"
#include "clock_cfg.h"
#include "asm/imb.h"
#include "asm/img.h"
#include "asm/lcd_buffer_manager.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "ui/ui_pic_index.h"
#include "ui/ui_str_index.h"
#include "res/resfile.h"
#include "asm/psram_api.h"
#include "res/jpeg_decoder.h"
#include "asm/psram_hw.h"
#include "asm/psram_api.h"
#include "imb_demo.h"
#include "../../../apps/common/ui/lv_watch/poc_modem/poc_modem_vm.h"
#include "../../../apps/common/ui/lv_watch/poc_modem/poc_modem_cache.h"

OS_MUTEX jpg_cache_MUTEX;

struct api_ciner_img_info {
    int width;//输入图片的宽和高，可以不是全屏
    int height;//输入图片的宽和高，可以不是全屏
    int format;//输入图片的格式，只支持RGB565/RGB888
    u8 *img_buf;//输入图片
};

struct api_src_block_info {
    struct rect rect;
    u8 *buf;
    u32 buf_size;
};
struct api_scale_info {
    struct rect src;
    struct api_src_block_info *src_block;
    struct rect dst;
    struct rect *dst_block;
    float ratio_w;//缩放比例，必须ratio_w == ratio_h
    float ratio_h;//缩放比例，必须ratio_w == ratio_h
    int width;//背景的宽和高，一般是全屏
    int height;//背景的宽和高，一般是全屏
    int format;//输入图片的格式，只支持RGB565/RGB888
    int bytesperpixel;
    int backcolor;
    int h_block_n;
    int v_block_n;
    u8 to_argb8565;
    u8 *cut_buf[4];
    u32 cut_buf_size[4];
    int max_block_n;
};
struct api_scale_info g_sca_hdl[2];
struct api_scale_info *api_get_blk_sca_info2(struct api_scale_info *sca, int width, int height, float ratio_w, float ratio_h, int x_offset, int y_offset, int limit_h, struct api_ciner_img_info *sca_img, int status);

//文件缓存
struct CacheFile {
    int len;
    u8  data[0];
};
struct CacheFile* readFromCache(char* indexfile);

#if 0
const char * lv_fs_get_num(const char * fn)
{
    size_t i;
    for(i = strlen(fn); i > 0; i--) {
        if(fn[i] == '=') {
            return &fn[i + 1];
        }
        else if(fn[i] == '/' || fn[i] == '\\') {
            return ""; /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name.*/
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief    创建背景任务
   @param    format:合成的格式, 屏驱配置
   @param    width:背景宽度
   @param    height:背景高度
   @param    color:背景颜色(RGB565格式)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_bg_task(u8 format, u16 width, u16 height, u16 color)
{
    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = format;
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.src_w          = width;
    imb_tsk_info.src_h          = height;
    imb_tsk_info.des_w          = width;
    imb_tsk_info.des_h          = height;
    imb_tsk_info.priv           = color;//纯色格式，该参数表示颜色0xr5g6b5
    imb_tsk_info.osd0_in_buf    = 1;
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);
}
#if IMB_DEMO_ENABLE
struct bg_buffer {
    u8 *buf;
    int len;
};

struct image_scale_info {
    float ratio_w;
    float ratio_h;
    int image_w;
    int image_h;
    int x;
    int y;
    u8 *image;
    u8 format;
};


struct src_block_info {
    struct rect rect;
    u8 *buf;
    u32 buf_size;
};

struct scale_info {
    u8 *curr_image;
    struct rect src;
    struct src_block_info *src_block;
    struct rect dst;
    struct rect *dst_block;
    float ratio_w;
    float ratio_h;
    int status;
    int width;
    int height;
    int src_format;
    int src_bpp;
    int backcolor;
    int h_block_n;
    int v_block_n;
    int dst_format;
    int dst_bpp;
};

struct ui_res_info {
    const char *path;
    struct flash_file_info ui_resfile_info;
    struct flash_file_info ui_strfile_info;
    RESFILE *ui_resfile;
    RESFILE *ui_strfile;
};

struct ui_display {
    struct lcd_interface *lcd;
};
static struct ui_display display = {0};
#define __this (&display)
/* 内存相关接口 */
void *br28_zalloc(int size);
void br28_free(void *buf);
void *imb_malloc(int size, int clr);
void imb_free(void *p);
void *malloc_vlt(size_t size);
void free_vlt(void *pv);

extern int open_image_by_id(int prj, RESFILE *specfile, struct image_file *f, int id, int page_num);
void set_lut_tab(struct imb_task *task);
int font_ascii_get_pix(char code, u8 *pixbuf, int buflen, int *height, int *width);
void l1_data_transformation(u8 *pix, u8 *pix_buf, u32 stride, int x, int y, int height, int width);
u16 ui_draw_get_mixed_pixel(u16 backcolor, u16 forecolor, u8 alpha);
void imb_buffer_unlock(u8 buf_index);
u8 *get_image_data(struct imb_task *task, struct image_file *f, struct imb_task_info *tsk_info);
void imb_task_blend();

/* 图像解压缩相关接口 */
u32 image_stride(int width, int format);
u16 image_palette_size(int format);
u32 image_pixel_size(int width, int height, int format);
void image_read_palette(struct image_file *f, struct imb_file_info *info, u8 *lut_tab);
int image_read_pixel(struct image_file *f, struct imb_file_info *info, u8 *dst, int left, int top, int width, int height);

/* dma拷贝相关接口 */
void dma_memcpy(void *dest, void *src, u32 len);
void dma_memcpy_wait_idle(void);

void imb_task_lock();
void imb_task_unlock();

enum LOAD_METHOD {
    LOAD_FROM_FLASH_WITH_TAB,       //flash数据带地址映射表选择该方式，该方式只支持压缩数据(L1格式除外)
    LOAD_FROM_FLASH_WITHOUT_TAB,    //flash数据不带地址映射表选择该方式，该方式支持压缩数据或者非压缩数据
    LOAD_FROM_RAM,                  //malloc申请的内存选择该方式, imb任务销毁的时候会调用free释放内存
    LOAD_FROM_VLT_RAM,              //malloc_vlt申请的内存选择该方式,imb任务销毁的时候会调用free_vlt释放内存
    LOAD_FROM_PSRAM,                //malloc_psram申请的内存选择该方式,imb任务销毁的时候会调用free_psram释放内存
    LOAD_FROM_RAM_AUTO,             //imb_malloc申请的内存选择该方式(若存在掉电RAM，会优先从掉电RAM里申请)，imb任务销毁的时候会调用imb_free释放内存
    LOAD_FROM_STATIC_RAM,           //若希望外部去释放malloc/malloc_vlt/malloc_psram申请的内存或者或者是一个静态
    //数组时选择该方式，imb任务销毁的时候不对该内存做释放处理
};


//测试项
//<单选项>
#define LARGE_IMAGE_SCALE_OPTIMIZE_TEST     1 //大图片(宽或者高超过240)缩放显示[优化], 注意:这种方式只适用RGB565/RGB888
#define LARGE_IMAGE_SCALE_TEST              0 //大图片(宽或者高超过240)缩放显示
#define IMB_MIXED_TEST                      0 //外部背景数据叠加imb任务
#define IMB_WATCH_TEST                      1 //模拟时钟显示
//<可多选>
#define IMAGE_DECOMPRESS_TEST               1 //图像解压缩
#define IMAGE_TEST                          0 //图片显示
#define IMAGE_SCALE_TEST                    0 //图片缩放显示
#define IMAGE_ROTATE_TEST                   0 //图片旋转显示
#define IMAGE_ROTATE_CHUNK_TEST             0 //图片旋转(分块输入)显示
#define STRING_TEST                         0 //字符串显示
#define FONT_TEST                           0 //字库显示
#define CUSTOM_DRAW_TEST                    0 //CPU自定义绘图
#define COLOR_TEST                          0 //纯色显示
#define JPEG_TEST                           0 //jpeg显示

#if LARGE_IMAGE_SCALE_OPTIMIZE_TEST
#define SCALE_EXAMPLE1                      1 //单张图片缩放从小到大直到铺满屏幕
#define SCALE_EXAMPLE2                      2 //两张图片合成&推屏显示
#define SCALE_EXAMPLE3                      3 //两张图片左右滑动且缩放
#define SCALE_EXAMPLE                       SCALE_EXAMPLE3
#endif

#if TCFG_UI_ENABLE && defined(TCFG_UI_ENABLE_DEMO) && TCFG_UI_ENABLE_DEMO
extern u8 icon_48x49_rgb565[4704];//format:r5g3 g3b5
extern u8 icon_48x49_argb8565[7056];//format:a8 r5g3 g3b5
#else
static u8 icon_48x49_rgb565[4704] ALIGNED(4) = {//format:r5g3 g3b5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0x20, 0x60, 0x28, 0x81,
    0x28, 0x81, 0x20, 0x60, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x08, 0x00, 0x40, 0xE1, 0x81, 0xC3, 0xB2, 0x65, 0xDA, 0xE6, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xDA, 0xE6, 0xB2, 0x65, 0x81, 0xC3, 0x40, 0xE1, 0x08, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xA1, 0x9A, 0x04,
    0xE3, 0x06, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xE3, 0x06,
    0x9A, 0x04, 0x30, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xC1, 0xB2, 0x64, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xB2, 0x64, 0x38, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x10, 0x40, 0xA2, 0x04, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xA2, 0x04, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x40, 0xE1, 0xDA, 0xE6, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xDA, 0xE6, 0x40, 0xE1, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x79, 0x83, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x79, 0x83,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0xE3,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0x91, 0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0xE3, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0x89, 0xE3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x71, 0x83, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0x71, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xC1, 0xEB, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0xCA, 0xF3, 0xEA, 0xF3, 0xEA, 0xF3, 0xEA, 0xF3, 0xEA,
    0xF3, 0xEA, 0xF3, 0xEA, 0xF3, 0xEA, 0xF3, 0xEA, 0xF3, 0x67, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xEB, 0x26, 0x40, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0xDA, 0xC5, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xFE, 0x77, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xF3, 0xC9, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xDA, 0xC5, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91, 0xE3, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0xEF, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x91, 0xE3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x28, 0x81, 0xEB, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x67, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x31, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xEB, 0x26, 0x28, 0x81, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xA2, 0x24, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0xA9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xA2, 0x24, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x60, 0xEB, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xFE, 0x15, 0xFD, 0xB3,
    0xFD, 0xD4, 0xFE, 0x98, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB4, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xEB, 0x26, 0x20, 0x60, 0x00, 0x00,
    0x00, 0x00, 0x79, 0x83, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF4, 0x4C, 0xFF, 0xFF, 0xFE, 0x15, 0xF4, 0x4C, 0xF5, 0x31, 0xFE, 0x36, 0xFE, 0x98,
    0xFE, 0x77, 0xFD, 0xB3, 0xF4, 0x8D, 0xF5, 0x10, 0xFF, 0x9D, 0xFE, 0x15, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x79, 0xA3, 0x00, 0x00,
    0x00, 0x00, 0xCA, 0xA5, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0xC9, 0xF4, 0x6C, 0xF3, 0x68, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0xA9, 0xF4, 0xAE, 0xFE, 0x36, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3B, 0xF4, 0xCF, 0xF4, 0x4C, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0xEA, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xCA, 0xA5, 0x00, 0x00,
    0x20, 0x80, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFD, 0xF4, 0xFF, 0x3B, 0xF5, 0x10, 0xF3, 0x47, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF4, 0x0B, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x92, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0xAE, 0xFF, 0xFF, 0xFD, 0xB3,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x20, 0x80,
    0x61, 0x42, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFD, 0xB4, 0xFF, 0x9E, 0xFF, 0x5C, 0xF4, 0xCF, 0xF3, 0x47, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0xC9, 0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x51, 0xF4, 0x0B,
    0xF4, 0x2B, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0x4C, 0xFF, 0xDE, 0xFF, 0xFF, 0xFD, 0x92,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x61, 0x42,
    0x91, 0xE4, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0xA9, 0xFD, 0xF5, 0xFF, 0xDF, 0xFF, 0x3B, 0xF4, 0x8E, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFE, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB3,
    0xFE, 0xFA, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0x0B, 0xFF, 0xBE, 0xFF, 0xFF, 0xFD, 0xF5, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x91, 0xE4,
    0xBA, 0x64, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x68, 0xFD, 0x72, 0xFF, 0xBE, 0xFF, 0x7C, 0xF5, 0x10, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0x6D,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA,
    0xF5, 0x10, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x77,
    0xF3, 0xA9, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x10, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xBA, 0x64,
    0xD2, 0xC5, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF5, 0x31, 0xFF, 0x5C, 0xFF, 0x9D, 0xF5, 0x51, 0xF3, 0x88, 0xF3, 0x26, 0xF3, 0x26, 0xFE, 0x36,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xF3, 0x68,
    0xF3, 0xCA, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF3, 0x68, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF5, 0x31, 0xFF, 0xFF, 0xFF, 0xDF, 0xF4, 0x8D,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xD2, 0xC5,
    0xE3, 0x06, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFE, 0x98, 0xFF, 0xFF, 0xFF, 0x5C, 0xF4, 0x2B, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xFF, 0x5C,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xF3, 0x68, 0xF3, 0xCA,
    0xFF, 0x7D, 0xFF, 0xFF, 0xFE, 0x98, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF4, 0x8D, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x68, 0xFF, 0x9D, 0xFF, 0xFF, 0xFE, 0xB9,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xE3, 0x06,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF4, 0x6C, 0xFE, 0xD9, 0xFF, 0xDE, 0xFE, 0x57, 0xF3, 0xEA, 0xF3, 0x26, 0xF3, 0x26, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xF3, 0x68, 0xF3, 0xCA, 0xFF, 0x7D,
    0xFF, 0xFF, 0xFD, 0x72, 0xF3, 0x26, 0xFE, 0x57, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF5, 0x10, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x47, 0xFE, 0xB9, 0xFF, 0xFF, 0xFF, 0x5C, 0xF3, 0x88,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xEB, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x47, 0xF5, 0x10, 0xFF, 0xBE, 0xFF, 0x9D, 0xFD, 0x51, 0xF3, 0x26, 0xF3, 0x26, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x88, 0xF3, 0xCA, 0xFF, 0x7C, 0xFF, 0xFF,
    0xFD, 0x72, 0xF3, 0x26, 0xF5, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF5, 0x10, 0xF3, 0x26, 0xF3, 0x26, 0xFD, 0x51, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0x4C, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xEB, 0x26,
    0xE2, 0xE6, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF4, 0x6C, 0xFE, 0xD9, 0xFF, 0xDE, 0xFE, 0x57, 0xF3, 0xEA, 0xF3, 0x26, 0xF3, 0x26, 0xFF, 0x5C,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x98, 0xFF, 0x7D, 0xFF, 0xFF, 0xFD, 0x72,
    0xF3, 0x26, 0xF5, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF4, 0x8D, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x47, 0xFE, 0xD9, 0xFF, 0xFF, 0xFF, 0x5C, 0xF3, 0x88,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xE2, 0xE6,
    0xCA, 0xA5, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFE, 0x98, 0xFF, 0xFF, 0xFF, 0x5C, 0xF4, 0x2B, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xFE, 0x56,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xD4, 0xF3, 0x26,
    0xF5, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xF3, 0x88, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x68, 0xFF, 0x9D, 0xFF, 0xFF, 0xFE, 0xB9,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xCA, 0xA5,
    0xAA, 0x44, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF5, 0x31, 0xFF, 0x5C, 0xFF, 0x9D, 0xF5, 0x51, 0xF3, 0x88, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0x8D,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x16, 0xFD, 0x72,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x78,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF5, 0x31, 0xFF, 0xFF, 0xFF, 0xDF, 0xF4, 0x8D,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xAA, 0x24,
    0x81, 0xA3, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x68, 0xFD, 0x72, 0xFF, 0xBE, 0xFF, 0x7C, 0xF5, 0x10, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFE, 0xD9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x10,
    0xFE, 0x98, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x10, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x81, 0xA3,
    0x49, 0x02, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0xA9, 0xFD, 0xF5, 0xFF, 0xDF, 0xFF, 0x3B, 0xF4, 0x8E, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0xC9, 0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xF4, 0xCF,
    0xF5, 0x10, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0x0B, 0xFF, 0xBE, 0xFF, 0xFF, 0xFD, 0xF5, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x49, 0x02,
    0x08, 0x20, 0xEB, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFD, 0xB4, 0xFF, 0x9E, 0xFF, 0x5C, 0xF4, 0xCF, 0xF3, 0x47, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF4, 0x2B, 0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB3, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0x4C, 0xFF, 0xDE, 0xFF, 0xFF, 0xFD, 0x92,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xEB, 0x26, 0x08, 0x20,
    0x00, 0x00, 0xB2, 0x44, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xFD, 0xF4, 0xFF, 0x3B, 0xF5, 0x10, 0xF3, 0x47, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0xA9, 0xF4, 0xCF, 0xFE, 0x57, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5C, 0xF4, 0xF0, 0xF4, 0x6D, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF4, 0xAE, 0xFF, 0xFF, 0xFD, 0xB3,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xB2, 0x44, 0x00, 0x00,
    0x00, 0x00, 0x59, 0x22, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0xC9, 0xF4, 0x6C, 0xF3, 0x68, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF4, 0x4C, 0xFF, 0xFF, 0xFE, 0x15, 0xF4, 0x6D, 0xFD, 0x72, 0xFE, 0x57, 0xFE, 0xB9,
    0xFE, 0x98, 0xFD, 0xD4, 0xF4, 0xAE, 0xF5, 0x31, 0xFF, 0x9D, 0xFE, 0x15, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0xEA, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x59, 0x22, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0xDA, 0xE6, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0xFE, 0x15, 0xFD, 0xB3,
    0xFD, 0xD4, 0xFE, 0x98, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB4, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xDA, 0xE6, 0x08, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x71, 0x83, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0xA9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x71, 0x83, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x20, 0xDA, 0xE6, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x67, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x31, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xDA, 0xE6, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x22, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0xEF, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x59, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0x44, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xFE, 0x77, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9D, 0xF3, 0xA9, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xAA, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x20, 0xD2, 0xC5, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0xC9, 0xF3, 0xCA, 0xF3, 0xCA, 0xF3, 0xCA, 0xF3, 0xCA,
    0xF3, 0xCA, 0xF3, 0xCA, 0xF3, 0xCA, 0xF3, 0xCA, 0xF3, 0x67, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xD2, 0xC5, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xA1, 0xE3, 0x06, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xE3, 0x06, 0x30, 0xA1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xE1, 0xEB, 0x06,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xEB, 0x06, 0x40, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xC1,
    0xE3, 0x06, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xE2, 0xE6,
    0x38, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x28, 0x81, 0xCA, 0xA5, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xCA, 0xA5, 0x28, 0x81,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x20, 0x9A, 0x04, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0x9A, 0x04, 0x08, 0x20, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0xE2, 0xCA, 0xA5, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xCA, 0xA5, 0x48, 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x59, 0x42, 0xCA, 0xA5, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xCA, 0xA5, 0x59, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xC1,
    0x91, 0xE4, 0xDA, 0xE6, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26,
    0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xF3, 0x26, 0xDA, 0xE6, 0x91, 0xE4,
    0x38, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x28, 0x81, 0x59, 0x22, 0x81, 0xA3, 0xA2, 0x24, 0xBA, 0x65, 0xC2, 0x85,
    0xC2, 0x85, 0xBA, 0x65, 0xA2, 0x24, 0x81, 0xA3, 0x59, 0x22, 0x28, 0x81, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static u8 icon_48x49_argb8565[7056] ALIGNED(4) = {//format:a8 r5g3 g3b5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D,
    0xEB, 0x07, 0x22, 0xF3, 0x46, 0x2B, 0xF3, 0x26, 0x2B, 0xF3, 0x26, 0x22, 0xF3, 0x46, 0x0D, 0xFB,
    0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x08, 0xFB, 0x08, 0x4A, 0xF3, 0x26, 0x8B, 0xF3, 0x26, 0xC1, 0xF3, 0x26, 0xEC, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xEC, 0xF3, 0x26, 0xC1, 0xF3, 0x26, 0x8B, 0xF3, 0x26, 0x4A, 0xF3, 0x26, 0x08, 0xFB, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xF3, 0x26, 0xA0, 0xF3, 0x26,
    0xF2, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xF2, 0xF3, 0x26,
    0xA0, 0xF3, 0x26, 0x38, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3E, 0xF3, 0x26, 0xBD, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xBD, 0xF3, 0x26, 0x3E, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xF3,
    0x46, 0xAB, 0xF3, 0x26, 0xFE, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFE, 0xF3, 0x26, 0xAB, 0xF3, 0x26, 0x19,
    0xEB, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0xF3, 0x26, 0xEB, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xEB,
    0xF3, 0x26, 0x48, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF3, 0x26, 0xFD, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFD, 0xF3, 0x26, 0x80, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0xFC, 0x00, 0x99, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x9A, 0xF3, 0x26, 0x01, 0xF8, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x97, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x96, 0xF3, 0x26, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x78, 0xF3,
    0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0xF3, 0x26, 0xFC,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xCA, 0xFF, 0xF3, 0xEA, 0xFF,
    0xF3, 0xEA, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3,
    0xEA, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0x67, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFC, 0xF3,
    0x26, 0x44, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xEB, 0x06, 0xE5, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFE, 0x77, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF, 0xF3, 0xC9, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xE5, 0xF3, 0x26, 0x10, 0xEB, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0xEF, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0x98, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0xF3, 0x46, 0xFC, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x67, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x31, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFC, 0xF3, 0x26, 0x2C, 0xF3, 0x46, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAA, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xA9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xAA, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x21, 0xF3, 0x26, 0xFC, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFE, 0xFA, 0xFF, 0xFE, 0x15, 0xFF, 0xFD, 0xB3, 0xFF, 0xFD, 0xD4, 0xFF, 0xFE, 0x98, 0xFF, 0xFF,
    0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB4, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFC, 0xF3, 0x26, 0x22, 0xF3, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x83, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x4C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x15, 0xFF, 0xF4, 0x4C, 0xFF,
    0xF5, 0x31, 0xFF, 0xFE, 0x36, 0xFF, 0xFE, 0x98, 0xFF, 0xFE, 0x77, 0xFF, 0xFD, 0xB3, 0xFF, 0xF4,
    0x8D, 0xFF, 0xF5, 0x10, 0xFF, 0xFF, 0x9D, 0xFF, 0xFE, 0x15, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x83, 0xF3, 0x26, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xDA, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xC9, 0xFF, 0xF4, 0x6C, 0xFF, 0xF3,
    0x68, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xA9, 0xFF, 0xF4, 0xAE, 0xFF, 0xFE, 0x36, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x3B, 0xFF, 0xF4, 0xCF, 0xFF, 0xF4, 0x4C, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xDB, 0xF3, 0x26, 0x00, 0x00, 0x00,
    0x27, 0xF3, 0x46, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFD, 0xF4, 0xFF, 0xFF, 0x3B, 0xFF, 0xF5,
    0x10, 0xFF, 0xF3, 0x47, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x0B, 0xFF, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x92, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF4, 0xAE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB3, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x27, 0xF3, 0x46,
    0x66, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFD, 0xB4, 0xFF, 0xFF, 0x9E, 0xFF, 0xFF,
    0x5C, 0xFF, 0xF4, 0xCF, 0xFF, 0xF3, 0x47, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0xC9, 0xFF, 0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x51, 0xFF, 0xF4, 0x0B,
    0xFF, 0xF4, 0x2B, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x4C, 0xFF,
    0xFF, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x92, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x66, 0xF3, 0x26,
    0x9A, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xA9, 0xFF, 0xFD, 0xF5, 0xFF, 0xFF,
    0xDF, 0xFF, 0xFF, 0x3B, 0xFF, 0xF4, 0x8E, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xFE, 0xB9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB3,
    0xFF, 0xFE, 0xFA, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x0B, 0xFF, 0xFF, 0xBE, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFD, 0xF5, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x9A, 0xF3, 0x26,
    0xC3, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x68, 0xFF, 0xFD, 0x72, 0xFF, 0xFF,
    0xBE, 0xFF, 0xFF, 0x7C, 0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x6D,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xFF, 0xF5, 0x10, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x77,
    0xFF, 0xF3, 0xA9, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xC3, 0xF3, 0x26,
    0xE1, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF5, 0x31, 0xFF, 0xFF, 0x5C, 0xFF, 0xFF,
    0x9D, 0xFF, 0xF5, 0x51, 0xFF, 0xF3, 0x88, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFE, 0x36,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xFF, 0xF3, 0x68, 0xFF, 0xF3, 0xCA, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xF3, 0x68, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF5, 0x31, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xF4, 0x8D, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xE1, 0xF3, 0x26,
    0xF4, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFE, 0x98, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x5C, 0xFF, 0xF4, 0x2B, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFF, 0x5C,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFE, 0xFA, 0xFF, 0xF3, 0x68, 0xFF, 0xF3, 0xCA, 0xFF, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
    0x98, 0xFF, 0xFF, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xF4, 0x8D, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x68, 0xFF,
    0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xB9, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xF4, 0xF3, 0x26,
    0xFD, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x6C, 0xFF, 0xFE, 0xD9, 0xFF, 0xFF,
    0xDE, 0xFF, 0xFE, 0x57, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFA, 0xFF,
    0xF3, 0x68, 0xFF, 0xF3, 0xCA, 0xFF, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xFF, 0xF3,
    0x26, 0xFF, 0xFE, 0x57, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x47, 0xFF, 0xFE, 0xB9, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x5C, 0xFF, 0xF3, 0x88, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFD, 0xF3, 0x26,
    0xFA, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x47, 0xFF, 0xF5, 0x10, 0xFF, 0xFF,
    0xBE, 0xFF, 0xFF, 0x9D, 0xFF, 0xFD, 0x51, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x88, 0xFF,
    0xF3, 0xCA, 0xFF, 0xFF, 0x7C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xFF, 0xF3, 0x26, 0xFF, 0xF5,
    0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFD, 0x51, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xF4, 0x4C, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFA, 0xF3, 0x26,
    0xEE, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x6C, 0xFF, 0xFE, 0xD9, 0xFF, 0xFF,
    0xDE, 0xFF, 0xFE, 0x57, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFF, 0x5C,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x98, 0xFF,
    0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xFF, 0xF3, 0x26, 0xFF, 0xF5, 0x10, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xF4, 0x8D, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x47, 0xFF, 0xFE, 0xD9, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x5C, 0xFF, 0xF3, 0x88, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xEE, 0xF3, 0x26,
    0xD7, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFE, 0x98, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x5C, 0xFF, 0xF4, 0x2B, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFE, 0x56,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFD, 0xD4, 0xFF, 0xF3, 0x26, 0xFF, 0xF5, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xF3, 0x88, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x68, 0xFF,
    0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xB9, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xD7, 0xF3, 0x26,
    0xB5, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF5, 0x31, 0xFF, 0xFF, 0x5C, 0xFF, 0xFF,
    0x9D, 0xFF, 0xF5, 0x51, 0xFF, 0xF3, 0x88, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x8D,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFE, 0x16, 0xFF, 0xFD, 0x72, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x78,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF5, 0x31, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xF4, 0x8D, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xB5, 0xF3, 0x26,
    0x88, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x68, 0xFF, 0xFD, 0x72, 0xFF, 0xFF,
    0xBE, 0xFF, 0xFF, 0x7C, 0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xFE, 0xD9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x10,
    0xFF, 0xFE, 0x98, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x88, 0xF3, 0x26,
    0x50, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xA9, 0xFF, 0xFD, 0xF5, 0xFF, 0xFF,
    0xDF, 0xFF, 0xFF, 0x3B, 0xFF, 0xF4, 0x8E, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0xC9, 0xFF, 0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xFF, 0xF4, 0xCF,
    0xFF, 0xF5, 0x10, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x0B, 0xFF, 0xFF, 0xBE, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFD, 0xF5, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x50, 0xF3, 0x26,
    0x10, 0xEB, 0x06, 0xFB, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFD, 0xB4, 0xFF, 0xFF, 0x9E, 0xFF, 0xFF,
    0x5C, 0xFF, 0xF4, 0xCF, 0xFF, 0xF3, 0x47, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x2B, 0xFF, 0xFF, 0x9D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB3, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x4C, 0xFF,
    0xFF, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x92, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFB, 0xF3, 0x26, 0x10, 0xEB, 0x06,
    0x00, 0x00, 0x00, 0xBB, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFD, 0xF4, 0xFF, 0xFF, 0x3B, 0xFF, 0xF5,
    0x10, 0xFF, 0xF3, 0x47, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xA9, 0xFF, 0xF4, 0xCF, 0xFF, 0xFE, 0x57, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x5C, 0xFF, 0xF4, 0xF0, 0xFF, 0xF4, 0x6D, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF4, 0xAE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB3, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xBB, 0xF3, 0x26, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x5F, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xC9, 0xFF, 0xF4, 0x6C, 0xFF, 0xF3,
    0x68, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF4, 0x4C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x15, 0xFF, 0xF4, 0x6D, 0xFF,
    0xFD, 0x72, 0xFF, 0xFE, 0x57, 0xFF, 0xFE, 0xB9, 0xFF, 0xFE, 0x98, 0xFF, 0xFD, 0xD4, 0xFF, 0xF4,
    0xAE, 0xFF, 0xF5, 0x31, 0xFF, 0xFF, 0x9D, 0xFF, 0xFE, 0x15, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0xEA, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x5F, 0xF3, 0x26, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x09, 0xE2, 0xA7, 0xEA, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xEA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0xFF, 0xFE, 0x15, 0xFF, 0xFD, 0xB3, 0xFF, 0xFD, 0xD4, 0xFF, 0xFE, 0x98, 0xFF, 0xFF,
    0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xB4, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xEA, 0xF3, 0x26, 0x09, 0xE2, 0xA7, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xA9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0x72, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0x7C, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xEB, 0x07, 0xE7, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x67, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF5, 0x31, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xE7, 0xF3, 0x26, 0x0D, 0xEB, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5E, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF4, 0xEF, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0x5F, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB4, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xFE, 0x77, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9D, 0xFF, 0xF3, 0xA9, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xB4, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0xF3, 0x06, 0xE2,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0xC9, 0xFF, 0xF3, 0xCA, 0xFF,
    0xF3, 0xCA, 0xFF, 0xF3, 0xCA, 0xFF, 0xF3, 0xCA, 0xFF, 0xF3, 0xCA, 0xFF, 0xF3, 0xCA, 0xFF, 0xF3,
    0xCA, 0xFF, 0xF3, 0xCA, 0xFF, 0xF3, 0x67, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xE3, 0xF3,
    0x26, 0x15, 0xF3, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x34,
    0xF3, 0x26, 0xF3, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xF3, 0xF3, 0x26, 0x34, 0xF3,
    0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x46, 0xF3, 0x26, 0xF7, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xF7, 0xF3, 0x26, 0x45, 0xF3, 0x26, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xF3, 0x26, 0xF1, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xEF, 0xF3, 0x26, 0x3E, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2E, 0xF3, 0x26, 0xDA, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xDA, 0xF3, 0x26, 0x2D, 0xF3, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xEB, 0x07, 0xA4, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xA4,
    0xF3, 0x26, 0x0D, 0xFB, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x4C, 0xF3, 0x26, 0xDB, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xDB, 0xF3, 0x26, 0x4C, 0xF3, 0x46, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0xCB, 0x26, 0x64, 0xF3, 0x26, 0xD7, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26,
    0xFF, 0xF3, 0x26, 0xD7, 0xF3, 0x26, 0x64, 0xF3, 0x26, 0x05, 0xCB, 0x26, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x41, 0xF3, 0x26,
    0x9B, 0xF3, 0x26, 0xE8, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF,
    0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3,
    0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xFF, 0xF3, 0x26, 0xE8, 0xF3, 0x26, 0x9B, 0xF3, 0x26,
    0x41, 0xF3, 0x26, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0xF8, 0x00, 0x2B, 0xFB, 0x26, 0x62, 0xF3, 0x26, 0x8D, 0xF3, 0x26, 0xAD,
    0xF3, 0x26, 0xC2, 0xF3, 0x26, 0xCC, 0xF3, 0x26, 0xCC, 0xF3, 0x26, 0xC2, 0xF3, 0x26, 0xAD, 0xF3,
    0x26, 0x8D, 0xF3, 0x26, 0x62, 0xF3, 0x26, 0x2C, 0xF3, 0x46, 0x01, 0xF8, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

static u8 icon_64x32_a8[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x44, 0x77, 0xBB, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0x99, 0xDD, 0xFF, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0xAA, 0xFF, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xCC, 0xFF, 0xFF, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x33, 0xFF, 0xFF, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x77, 0xFF, 0xFF, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x77, 0x77, 0x55, 0x44, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x77, 0xFF, 0xFF, 0xFF, 0x55, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x77, 0x77, 0x77, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xAA, 0x99, 0x88, 0x88, 0x88, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77, 0xBB, 0xFF, 0xFF, 0x99, 0x77, 0x77, 0x77, 0x77, 0x88, 0x88, 0x88, 0x99,
    0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0xFF, 0xEE, 0xEE, 0xFF, 0xFF,
    0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x11, 0x11, 0x00, 0x44, 0xFF, 0xFF,
    0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x11, 0x11, 0x11,
    0x22, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0xFF, 0xFF,
    0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0xCC, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0x00, 0x11, 0x11, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD, 0xFF,
    0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xFF, 0xFF, 0x99, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE, 0xFF, 0xCC, 0x00, 0xAA, 0xAA, 0xAA, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xFF,
    0xFF, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0xFF, 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x33, 0x33, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0xFF,
    0xFF, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xFF, 0xFF, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0x44, 0x44, 0x44, 0x99, 0xFF, 0xFF, 0xFF, 0xBB, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD,
    0xFF, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0xFF, 0xFF, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x11, 0x99, 0xFF, 0xFF, 0xEE, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77,
    0xFF, 0xFF, 0x66, 0x00, 0x00, 0x00, 0x00, 0x88, 0xFF, 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x88, 0xEE, 0xFF, 0xFF, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
    0xEE, 0xFF, 0xDD, 0x00, 0x00, 0x00, 0x11, 0xEE, 0xFF, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x88, 0xFF, 0xFF, 0x88, 0x00, 0x00, 0x99, 0xFF, 0xFF, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x77, 0x77, 0x77, 0x66, 0x44, 0x44, 0x44, 0x44,
    0x44, 0x44, 0xEE, 0xFF, 0xEE, 0x44, 0x44, 0x44, 0x44, 0x44, 0x66, 0x77, 0x77, 0x77, 0x88, 0x88,
    0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x11, 0xEE, 0xFF, 0xFF, 0x44, 0x55, 0xFF, 0xFF, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x55, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF, 0xFF, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x33, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x33, 0x33,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x11, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
    0x77, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x99, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x88, 0xEE,
    0xFF, 0xFF, 0xFF, 0xCC, 0x33, 0x66, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xAA, 0x55, 0x11, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x33, 0x66, 0x99, 0xDD, 0xFF, 0xFF, 0xFF,
    0xFF, 0xEE, 0x77, 0x00, 0x00, 0x00, 0x22, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCC, 0x99,
    0x88, 0x77, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE,
    0x88, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xEE, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x22, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0xFF, 0xFF, 0xFF, 0xEE, 0x77, 0x11,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x99, 0xEE, 0xFF, 0xFF, 0xFF,
    0xFF, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD, 0xAA, 0x55, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x99, 0xCC,
    0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDD,
    0xFF, 0xFF, 0xFF, 0xFF, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99,
    0xFF, 0xDD, 0xAA, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11,
    0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};


/*----------------------------------------------------------------------------*/
/**@brief   获取外挂flash资源文件的物理地址
   @param   path:文件路径, e.g "storage/virfat_flash/C/ui/scale454.bin"
   @return  文件的flash地址
   @note
*/
/*----------------------------------------------------------------------------*/
u32 get_file_addr(char *path)
{
    struct flash_file_info ui_resfile_info;
    int ret = ui_res_flash_info_get(&ui_resfile_info, path, "res");
    ASSERT(!ret);
    u32 file_addr = ui_resfile_info.tab[0] + ui_resfile_info.offset;//计算文件的物理地址
    ui_res_flash_info_free(&ui_resfile_info, "res");

    return file_addr;
}


/*----------------------------------------------------------------------------*/
/**@brief   获取外挂flash资源文件的物理地址
   @param   path:文件路径, e.g "storage/virfat_flash/C/ui/scale454.bin"
   @param   id : 图片id，由sdk自带工具生成
   @return  图片的flash地址
   @note
*/
/*----------------------------------------------------------------------------*/
u32 get_image_addr(RESFILE *specfile, char *path, u32 id)
{
    struct flash_file_info ui_resfile_info;
    struct image_file file;
    int ret = ui_res_flash_info_get(&ui_resfile_info, path, "res");
    ASSERT(!ret);

    ret = open_image_by_id(0, specfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);

    u32 file_addr = ui_resfile_info.tab[0] + ui_resfile_info.offset + file.offset;//计算图片的物理地址

    ui_res_flash_info_free(&ui_resfile_info, "res");

    return file_addr;
}

int imb_task_get_id()
{
    static int task_id = 0;
    return ++task_id;
}

/*----------------------------------------------------------------------------*/
/**@brief    创建背景任务
   @param    format:合成的格式, 屏驱配置
   @param    width:背景宽度
   @param    height:背景高度
   @param    color:背景颜色(RGB565格式)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_bg_task(u8 format, u16 width, u16 height, u16 color)
{
    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = format;
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.src_w          = width;
    imb_tsk_info.src_h          = height;
    imb_tsk_info.des_w          = width;
    imb_tsk_info.des_h          = height;
    imb_tsk_info.priv           = color;//纯色格式，该参数表示颜色0xr5g6b5
    /* imb_tsk_info.osd0_in_buf    = 1; */
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);
}


/*----------------------------------------------------------------------------*/
/**@brief    创建图片任务
   @param    task_id : 任务id
   @param    file : 图片信息
   @param    priv : LOAD_FROM_FLASH_WITH_TAB方式加载时为struct flash_file_info结构信息, 其他方式时为图像数据的地址
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:图片显示的宽度，必须小于或者等于图片的实际宽度，小于实际宽度则截取显示
   @param    height:图片显示的高度，必须小于或者等于图片的实际高度，小于实际高度则截取显示
   @param    crop : 为空时不对图片进行裁剪, 非空时为图片的裁剪区域
   @param    zip : 是否压缩数据
   @param    load_method : 数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note     图片存储在flash, 指定flash地址映射表(可以支持数据不连续存储，以4k为单位，中间4k必须填满)
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_task(int task_id, struct image_file *file, u32 priv, int x, int y, int width, int height, struct rect *crop, int zip, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src = DATA_SRC_FLASH;//直接从flash取数
        imb_tsk_info.cur_in_flash = 1;//数据存在flash
    } else {
        imb_tsk_info.data_src = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash = 0;
    }
    if (file->format == PIXEL_FMT_L1) {
        imb_tsk_info.zip_en = 0;
    } else {
        imb_tsk_info.zip_en = zip;
    }
    if (load_method == LOAD_FROM_FLASH_WITH_TAB || load_method == LOAD_FROM_FLASH_WITHOUT_TAB) {
        imb_tsk_info.addr_source = SOURCE_FLASH;
    } else if (load_method == LOAD_FROM_RAM_AUTO) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_RAM) {
        imb_tsk_info.addr_source = SOURCE_RAM;
    } else if (load_method == SOURCE_VLT_RAM) {
        imb_tsk_info.addr_source = SOURCE_VLT_RAM;
    } else if (load_method == SOURCE_PSRAM) {
        imb_tsk_info.addr_source = SOURCE_PSRAM;
    } else if (load_method == SOURCE_STATIC_RAM) {
        imb_tsk_info.addr_source = SOURCE_STATIC_RAM;
    }
    imb_tsk_info.in_format = file->format;
    imb_tsk_info.x_offset = x;
    imb_tsk_info.y_offset = y;
    imb_tsk_info.src_w = file->width;
    imb_tsk_info.src_h = file->height;
    imb_tsk_info.des_w = width;
    imb_tsk_info.des_h = height;
    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.ff_info = priv;
        imb_tsk_info.priv = (u32)file;
    } else {
        imb_tsk_info.priv = (u32)priv;
    }

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type = L1_IMAGE;
    }

    //裁剪区域
    if (crop) {
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info); //imb_task参数重置
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

static void *imb_jpeg_cb(void *priv)
{
    struct imb_task_out *out = (struct imb_task_out *)priv;
    struct jpeg_decoder_fd *jpg_fd;
    struct rect *jpeg_page_rect = out->priv;
    struct rect *cur_draw = &out->rect;
    struct rect jpeg_rect;
    struct rect draw_rect;
    int ret;
    struct imb_task *task = out->task;

    if (task->task_en == false) {
        printf("jpeg task_en == false\n");
        return NULL;
    }

    jpg_fd = jpeg_dec_get_fd();

    if (jpg_fd && (jpg_fd->fat_info != task->flash_file_info)) {
        printf("redec_init\n");
        imb_task_lock();
        jpeg_dec_init(task->flash_file_info, out->format, NULL);
        imb_task_unlock();
    }

    struct rect r;

    if (!get_rect_cover(jpeg_page_rect, cur_draw, &r)) {
        printf("jpeg not in area\n");
        return NULL;
    }

    jpeg_rect.left = r.left - jpeg_page_rect->left;
    jpeg_rect.top = r.top - jpeg_page_rect->top;
    jpeg_rect.width = r.width;
    jpeg_rect.height = r.height;

    if ((jpeg_rect.left < 0) || (jpeg_rect.top < 0)
        || (jpeg_rect.left + jpeg_rect.width > jpg_fd->info.x)
        || (jpeg_rect.top + jpeg_rect.height > jpg_fd->info.y)) {
        printf("\nerr_draw_jpeg %d, %d, %d, %d, %d, %d\n", jpeg_rect.left, jpeg_rect.top, jpeg_rect.width, jpeg_rect.height, jpg_fd->info.x, jpg_fd->info.y);
        return NULL;
    }


    imb_task_lock();
    ret = jpeg_dec_start(cur_draw, &r, &jpeg_rect, out->outbuf);
    imb_task_unlock();

    return NULL;
}

/*----------------------------------------------------------------------------*/
/**@brief    创建jpeg图片任务
   @param    task_id : 任务id
   @param    file : 图片信息
   @param    priv : LOAD_FROM_FLASH_WITH_TAB时为flash_file_info, 其他为图像数据的地址
   @param    lcd_format : 屏接口的格式，这里要与屏接口的格式保持一致
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:图片的宽度
   @param    height:图片的高度
   @param    load_method : 数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_jpeg_task(int task_id, struct image_file *file, u32 priv, int lcd_format, int x, int y, int width, int height, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src = DATA_SRC_FLASH;//直接从flash取数
        imb_tsk_info.cur_in_flash = 1;//数据存在flash
    } else {
        imb_tsk_info.data_src = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash = 0;
    }
    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.x_offset       = x;
    imb_tsk_info.y_offset       = y;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.des_w          = width;
    imb_tsk_info.des_h          = height;
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.lcd_format     = lcd_format;
    imb_tsk_info.cb             = imb_jpeg_cb;
    imb_tsk_info.quote          = 1;
    imb_tsk_info.ff_info        = priv;
    imb_tsk_info.priv           = (u32)file;

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info); //imb_task参数重置
    } else {
        p = imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);

        struct rect jpeg_page_rect;
        jpeg_page_rect.left = x;
        jpeg_page_rect.top = y;
        jpeg_page_rect.width = width;
        jpeg_page_rect.height = height;

        memcpy(p->priv_buf, &jpeg_page_rect, sizeof(struct rect));
        /* memcpy((u8 *)p->priv_buf + sizeof(struct rect), &draw, sizeof(struct rect)); */
        p->cb_priv = p->priv_buf;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief     创建纯色任务(矩形填充)
   @param     task_id : 任务id
   @param     x : 相对背景的x坐标
   @param     y : 相对背景的y坐标
   @param     width : 矩形的宽度
   @param     height: 矩形的高度
   @param     color : argb8565格式 (alpha 0~100)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_color_task(int task_id, int x, int y, int width, int height, u32 color)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.data_src = DATA_SRC_NONE;
    imb_tsk_info.zip_en = 0;
    imb_tsk_info.in_format = LAYER_FORMAT_SOLID;
    imb_tsk_info.x_offset = x;
    imb_tsk_info.y_offset = y;
    imb_tsk_info.src_w = width;
    imb_tsk_info.src_h = height;
    imb_tsk_info.des_w = width;
    imb_tsk_info.des_h = height;
    imb_tsk_info.priv = color;

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief    创建字符串任务
   @param    task_id : 任务id
   @param    file:字符串信息
   @param    file_info:字符串存储信息
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:字符串显示的宽度，必须小于或者等于字符串的实际宽度，小于实际宽度则截取显示
   @param    height:字符串显示的高度，必须小于或者等于字符串的实际高度，小于实际高度则截取显示
   @param    color:字符串的颜色(RGB565格式)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_strpic_task(int task_id, struct image_file *file, u32 priv, int x, int y, int width, int height, u16 color)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;


    imb_tsk_info.elm_id         = task_id;
    imb_tsk_info.data_src       = DATA_SRC_FLASH;
    imb_tsk_info.cur_in_flash   = 1;
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_L1;//file->format;
    imb_tsk_info.x_offset       = x;
    imb_tsk_info.y_offset       = y;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.des_w          = width;
    imb_tsk_info.des_h          = height;
    imb_tsk_info.text_color     = color;
    imb_tsk_info.l1_type        = L1_TEXT;
    imb_tsk_info.ff_info        = (u32)priv;
    imb_tsk_info.priv           = (u32)file;


    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}


/*----------------------------------------------------------------------------*/
/**@brief    创建图片旋转任务
   @param    task_id : 任务id
   @param    file:图片信息
   @param    priv:图片存储信息
   @param    cx:图片的旋转中心x坐标，相对于图片
   @param    cy:图片的旋转中心y坐标，相对于图片
   @param    angle:旋转角度(0~359,最小单位1度)
   @param    dx:输出的旋转中心点x坐标，相对于背景
   @param    dy:输出的旋转中心点y坐标，相对于背景
   @param    crop : 为空时不对图片进行裁剪, 非空时为图片的裁剪区域
   @param    load_method : 数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_rotate_task(int task_id, struct image_file *file, u32 priv, int cx, int cy, int angle, int dx, int dy, struct rect *crop, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.rotate_en = 1;
    if (imb_tsk_info.rotate_en) {
        imb_tsk_info.x_src_offset   = 0;
        imb_tsk_info.y_src_offset   = 0;
        imb_tsk_info.rotate_c_x     = cx;
        imb_tsk_info.rotate_c_y     = cy;
        imb_tsk_info.rotate_angle   = angle;
        imb_tsk_info.rotate_dx      = dx;
        imb_tsk_info.rotate_dy      = dy;
        imb_tsk_info.rotate_chunk   = 0;
    }

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src       = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash   = 1;//数据存放在flash，内部会先将压缩数据解压到sram, 然后imb从sram取数
    } else {
        imb_tsk_info.data_src       = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash   = 0;
    }

    imb_tsk_info.zip_en         = 0;//旋转不支持压缩格式

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_FLASH_WITHOUT_TAB) {
        imb_tsk_info.addr_source = SOURCE_FLASH;
    } else if (load_method == LOAD_FROM_RAM_AUTO) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_RAM) {
        imb_tsk_info.addr_source = SOURCE_RAM;
    } else if (load_method == SOURCE_VLT_RAM) {
        imb_tsk_info.addr_source = SOURCE_VLT_RAM;
    } else if (load_method == SOURCE_PSRAM) {
        imb_tsk_info.addr_source = SOURCE_PSRAM;
    } else if (load_method == SOURCE_STATIC_RAM) {
        imb_tsk_info.addr_source = SOURCE_STATIC_RAM;
    }

    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    //旋转这组参数设置无效，默认为背景任务大小
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.des_w          = 0;
    imb_tsk_info.des_h          = 0;

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.ff_info = priv;
        imb_tsk_info.priv = (u32)file;
    } else {
        imb_tsk_info.priv = (u32)priv;
    }

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type        = L1_IMAGE;
    }

    //裁剪区域
    if (crop) {
        imb_tsk_info.rotate_scale_crop_en = 1;
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    图片旋转分块输入回调
   @param    priv:任务信息
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_rotate_chunk_cb(void *priv)
{
    struct imb_task_info *tsk_info = (struct imb_task_info *)priv;
    struct imb_task *task = (struct imb_task *)tsk_info->task;
    struct imb_task_head *head = imb_task_head_get(task->root_index);
    struct rect *rect = &head->curr_draw;

    task->x_src_offset = tsk_info->x_src_offset;
    task->src_w = tsk_info->src_w;

    task->y_src_offset = tsk_info->y_src_offset + tsk_info->rotate_chunk_offset_cur;
    task->src_h = (tsk_info->src_h - tsk_info->y_src_offset - tsk_info->rotate_chunk_offset_cur) > tsk_info->rotate_chunk ?
                  tsk_info->rotate_chunk : (tsk_info->src_h - tsk_info->y_src_offset - tsk_info->rotate_chunk_offset_cur);
    task->src_w_pre = tsk_info->src_w;
    task->src_h_pre = tsk_info->src_h;

    /* printf("input rect : [%d, %d, %d x %d]\n", task->x_src_offset, task->y_src_offset, task->src_w, task->src_h); */
    /* printf("output rect: [%d, %d, %d x %d]\n", rect->left, rect->top, rect->width, rect->height); */

    task->cur_format = LAYER_FORMAT_ARGB8565;
    static RESFILE *file = NULL;
    if (!file) {
        file = res_fopen(RES_PATH"ui/compass.bin", "r");//454x454, argb8565
        ASSERT(file);
    }
    int stride = image_stride(task->src_w, task->cur_format);
    u16 lut_tab_size = image_palette_size(task->cur_format);

    if (lut_tab_size) {
        if (!task->lut_tab) {
            task->lut_tab = imb_malloc(lut_tab_size, true);
            res_fseek(file, 0, SEEK_SET);
            res_fread(file, task->lut_tab, lut_tab_size);
        }
    }

    if (!tsk_info->rotate_buffer) {
        int height = tsk_info->rotate_chunk + 1;
        tsk_info->rotate_buffer = (u8 *)imb_malloc(stride * height, false);
    }

    res_fseek(file, lut_tab_size + stride * task->y_src_offset, SEEK_SET);
    if ((task->y_src_offset + task->src_h) < task->src_h_pre) {
        res_fread(file, tsk_info->rotate_buffer, stride * (task->src_h + 1));//为了解决分块输入导致的相位问题，非最后一个分块需要多读取一行像素数据
    } else {
        res_fread(file, tsk_info->rotate_buffer, stride * task->src_h);
    }
    task->dat_src_adr = tsk_info->rotate_buffer;
}

/*----------------------------------------------------------------------------*/
/**@brief    创建图片旋转任务(图片分块拷贝到sram作为输入)
   @param    task_id : 任务id
   @param    file:图片信息
   @param    file_info:图片存储信息
   @param    cx:图片的旋转中心x坐标，相对于图片
   @param    cy:图片的旋转中心y坐标，相对于图片
   @param    angle:旋转角度
   @param    dx:输出的旋转中心点x坐标，相对于背景
   @param    dy:输出的旋转中心点y坐标，相对于背景
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_rotate_chunk_task(int task_id, struct image_file *file, u32 priv, int cx, int cy, int angle, int dx, int dy)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.rotate_en = 1;//旋转使能
    if (imb_tsk_info.rotate_en) {
        imb_tsk_info.x_src_offset   = 0;
        imb_tsk_info.y_src_offset   = 0;
        imb_tsk_info.rotate_c_x     = cx;
        imb_tsk_info.rotate_c_y     = cy;
        imb_tsk_info.rotate_angle   = angle;
        imb_tsk_info.rotate_dx      = dx;
        imb_tsk_info.rotate_dy      = dy;
        imb_tsk_info.rotate_chunk   = 1;
        if (imb_tsk_info.rotate_chunk) {
            imb_tsk_info.rotate_chunk_offset = 0;
            u8 chunk_num = 20;//输入分块数量(分块数量越少，合成速度越快，消耗ram越多)
            imb_tsk_info.rotate_chunk = (file->height / chunk_num + 9) / 10 * 10;//每个分块高度
            imb_tsk_info.rotate_chunk_num = (file->height + imb_tsk_info.rotate_chunk - 1) / imb_tsk_info.rotate_chunk;
        }
    }

    imb_tsk_info.data_src       = DATA_SRC_SRAM;
    imb_tsk_info.cur_in_flash   = 1;
    imb_tsk_info.zip_en         = 0;

    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.des_w          = file->width;
    imb_tsk_info.des_h          = file->height;
    imb_tsk_info.ff_info        = (u32)priv;
    imb_tsk_info.priv           = (u32)file;

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type        = L1_IMAGE;
    }

    /* imb_tsk_info.cb         = imb_rotate_chunk_cb; //注释该句则跑内部的分块流程 */

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    创建图片缩放任务
   @param    task_id : 任务id
   @param    file:图片信息
   @param    priv : LOAD_FROM_FLASH_WITH_TAB方式加载时为struct flash_file_info结构信息, 其他方式时为图像数据的地址
   @param    ratio_w:水平缩放比率
   @param    ratio_h:垂直缩放比率
   @param    x:图片相对背景的x坐标
   @param    y:图片相对背景的y坐标
   @param    width:显示的图片宽度（缩放后的宽度）
   @param    height:显示的图片高度（缩放后的高度）
   @param    crop:为空时不对图片进行裁剪, 非空时为图片的裁剪区域
   @param    zip:是否压缩数据
   @param    load_method:数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_scale_task(int task_id, struct image_file *file, u32 priv, float ratio_w, float ratio_h, int x, int y, int width, int height, struct rect *crop, int zip, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.scale_en = true;//缩放使能
    if (imb_tsk_info.scale_en) {
        imb_tsk_info.ratio_w = ratio_w;
        imb_tsk_info.ratio_h = ratio_h;
        ASSERT(imb_tsk_info.ratio_w);
        ASSERT(imb_tsk_info.ratio_h);
    }

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src = DATA_SRC_FLASH;
        imb_tsk_info.cur_in_flash = 1;
    } else {
        imb_tsk_info.data_src = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash = 0;
    }

    if (file->format == PIXEL_FMT_L1) {
        imb_tsk_info.zip_en     = 0;
    } else {
        imb_tsk_info.zip_en     = zip;
    }

    if (load_method == LOAD_FROM_FLASH_WITH_TAB || load_method == LOAD_FROM_FLASH_WITHOUT_TAB) {
        imb_tsk_info.addr_source = SOURCE_FLASH;
    } else if (load_method == LOAD_FROM_RAM_AUTO) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_RAM) {
        imb_tsk_info.addr_source = SOURCE_RAM;
    } else if (load_method == SOURCE_VLT_RAM) {
        imb_tsk_info.addr_source = SOURCE_VLT_RAM;
    } else if (load_method == SOURCE_PSRAM) {
        imb_tsk_info.addr_source = SOURCE_PSRAM;
    } else if (load_method == SOURCE_STATIC_RAM) {
        imb_tsk_info.addr_source = SOURCE_STATIC_RAM;
    }

    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.x_offset       = x;
    imb_tsk_info.y_offset       = y;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.des_w          = width;
    imb_tsk_info.des_h          = height;

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.ff_info = priv;
        imb_tsk_info.priv = (u32)file;
    } else {
        imb_tsk_info.priv = (u32)priv;
    }

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type        = L1_IMAGE;
    }

    //裁剪区域
    if (crop) {
        imb_tsk_info.rotate_scale_crop_en = 1;
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}




#define DRAW_IMAGE              0x0
#define DRAW_IMAGE_WITH_ALPHA   0x1

struct image_buf {
    u8 *buf;
    u32 len;
};

static void ui_custom_draw_cb(int id, u8 *dst_buf, struct rect *dst_r, struct rect *src_r, u8 bytes_per_pixel, void *priv)
{
    int w, h;
    struct rect r;
    int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
    int src_stride = (src_r->width * bytes_per_pixel + 3) / 4 * 4;

    if (id == DRAW_IMAGE) {
        struct image_buf *image_b = (struct image_buf *)priv;
        /* printf("ui_custom_draw_cb 0x%x, %d\n", image_b->buf, image_b->len); */
        /* printf("dst [%d, %d, %d, %d],  src [%d, %d, %d, %d]\n", dst_r->left, dst_r->top, dst_r->width, dst_r->height, */
        /* src_r->left, src_r->top, src_r->width, src_r->height); */

        if (get_rect_cover(src_r, dst_r, &r)) {
            for (h = 0; h < r.height; h++) {
                memcpy(&dst_buf[(r.top + h - dst_r->top) * dst_stride + (r.left - dst_r->left) * 2],
                       &image_b->buf[(r.top + h - src_r->top) * src_stride + (r.left - src_r->left) * 2],
                       r.width * 2);
            }
        }
    } else if (id == DRAW_IMAGE_WITH_ALPHA) {
        int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
        /* int src_stride = (src_r->width * 3 + 3) / 4 * 4; */
        struct image_buf *image_b = (struct image_buf *)priv;
        u8 alpha;
        u16 forecolor;
        u16 backcolor;
        u16 mixedcolor;
        u32 offset_src;
        u32 offset_dst;

        if (get_rect_cover(src_r, dst_r, &r)) {
            for (h = 0; h < r.height; h++) {
                for (w = 0; w < r.width; w++) {
                    offset_src = (r.top + h - src_r->top) * src_r->width * 3 + (r.left + w - src_r->left) * 3;
                    alpha = image_b->buf[offset_src];
                    forecolor = (image_b->buf[offset_src + 1] << 8) | image_b->buf[offset_src + 2];

                    if (alpha) {
                        offset_dst = (r.top + h - dst_r->top) * dst_stride + (r.left + w - dst_r->left) * 2;
                        backcolor = (dst_buf[offset_dst + 1] << 8) | dst_buf[offset_dst];
                        mixedcolor = ui_draw_get_mixed_pixel((backcolor >> 8) | ((backcolor & 0xff) << 8), forecolor, alpha);
                        dst_buf[offset_dst] = mixedcolor;
                        dst_buf[offset_dst + 1] = mixedcolor >> 8;
                    }
                }
            }
        }
    }
}

static void *imb_draw_cb(void *priv)
{
    struct imb_task_out *out = (struct imb_task_out *)priv;
    struct imb_task *task = out->task;

    if (out->format == OUTPUT_FORMAT_RGB888) {
        task->cur_format = LAYER_FORMAT_RGB888;
    } else if (out->format == OUTPUT_FORMAT_RGB565) {
        task->cur_format = LAYER_FORMAT_RGB565;
    } else {
        ASSERT(0, "curr format is not support!\n");
    }
    task->x_offset = 0;
    task->y_offset = 0;
    task->src_w = out->rect.width;
    task->src_h = out->rect.height;
    task->des_w = out->rect.width;
    task->des_h = out->rect.height;
    task->dat_src_adr = (u32)out->outbuf;

    ui_custom_draw_cb(task->cb_id, out->outbuf, &out->rect, &task->rect, (out->format == OUTPUT_FORMAT_RGB888) ? 3 : 2, task->cb_priv);//不带alpha的显示

    return NULL;
}

/*----------------------------------------------------------------------------*/
/**@brief    创建自定义绘图任务
   @param    task_id : 任务id
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:显示的宽度
   @param    height:显示的高度
   @param    cb:回调函数
   @param    priv:回调函数参数
   @param    priv_len:参数长度
   @param    id:回调函数id号（自定义）
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_custom_task(int task_id, int x, int y, int width, int height, void *cb, void *priv, int priv_len, int id)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.data_src = DATA_SRC_SRAM;
    imb_tsk_info.cur_in_flash = 0;
    imb_tsk_info.zip_en = 0;
    imb_tsk_info.in_format = 0;
    imb_tsk_info.x_offset = x;
    imb_tsk_info.y_offset = y;
    imb_tsk_info.src_w = width;
    imb_tsk_info.src_h = height;
    imb_tsk_info.des_w = width;
    imb_tsk_info.des_h = height;
    imb_tsk_info.ff_info = NULL;
    imb_tsk_info.cb = cb;
    imb_tsk_info.cb_priv = priv;
    imb_tsk_info.quote = 1;
    imb_tsk_info.user_cb = cb ? 1 : 0;
    imb_tsk_info.cb_id = id;

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}


//ascii字模拼接
u8 *ascii_image_splice(char *str, int *str_width,  int *str_height)
{
    int width;
    int height;
    int realsize_w;
    int realsize_h;
    u8 pixel[128];
    char *p;
    int offset = 0;
    u8 *pixbuf = NULL;
    u8 *pixbuf_temp;

    realsize_w = 0;
    realsize_h = 0;
    p = str;
    while (*p) {//计算ascii的总长度
        font_ascii_get_pix(*p, pixel, sizeof(pixel), &height, &width);
        realsize_w += width;
        p++;
    }
    realsize_h = height;

    u16 stride = (realsize_w + 31) / 32 * 32 / 8;
    pixbuf = br28_zalloc(stride * realsize_h);
    if (!pixbuf) {
        return 0;
    }

    pixbuf_temp = br28_zalloc((realsize_w + 7) / 8 * realsize_h);
    if (!pixbuf_temp) {
        return 0;
    }

    p = str;
    while (*p) {//获取ascii字模
        int i;
        /*
        字库字模排列
        L ... L
        . ... .
        . ... .
        . ... .
        M ... M

        L ... L
        . ... .
        . ... .
        . ... .
        M ... M
        */
        font_ascii_get_pix(*p, pixel, sizeof(pixel), &height, &width);
        for (i = 0; i < height / 8; i++) {
            memcpy(pixbuf_temp + realsize_w * i + offset, pixel + width * i, width);
        }
        offset += width;
        p++;
    }

    /*
    L1图像排列
    M ... L M ... L
    . ... . . ... .
    . ... . . ... .
    . ... . . ... .
    M ... L M ... L
    */
    l1_data_transformation(pixbuf_temp, pixbuf, stride, 0, 0, realsize_h, realsize_w);//ascii字模转换成L1格式的图像

    if (pixbuf_temp) {
        br28_free(pixbuf_temp);
    }

    *str_width = realsize_w;
    *str_height = realsize_h;

    return pixbuf;
}

/*----------------------------------------------------------------------------*/
/**@brief    创建字库文字显示任务
   @param    task_id : 任务id
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:显示的宽度
   @param    height:显示的高度
   @param    pixbuf:内存图片的起始地址
   @param    color:文字的颜色(RGB565格式)
   @param    crop : 裁剪区域，为NULL时不裁剪
   @return
   @note     从效率角度考虑，建议先把字库的文字拼接成图片，再创建任务从sram加载显示
*/
/*----------------------------------------------------------------------------*/
void imb_create_text_task(int task_id, int x, int y, int width, int height, u8 *pixbuf, u16 color, struct rect *crop)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.data_src = DATA_SRC_SRAM;
    imb_tsk_info.cur_in_flash = 0;
    imb_tsk_info.zip_en = 0;
    imb_tsk_info.in_format = LAYER_FORMAT_L1;
    imb_tsk_info.x_offset = x;
    imb_tsk_info.y_offset = y;
    imb_tsk_info.src_w = (width + 15) / 16 * 16;
    imb_tsk_info.src_h = height;
    imb_tsk_info.des_w = (width + 15) / 16 * 16;
    imb_tsk_info.des_h = height;
    imb_tsk_info.text_color = color;
    imb_tsk_info.priv = pixbuf;
    imb_tsk_info.ui_malloc = 1;

    if (crop) {
        imb_tsk_info.rect.left = x;
        imb_tsk_info.rect.top = y;
        imb_tsk_info.rect.width = (width + 15) / 16 * 16;
        imb_tsk_info.rect.height = height;
        if (get_rect_cover(&imb_tsk_info.rect, crop, &imb_tsk_info.crop)) {
        }
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
static u8 *full_screen_buf[2] = {NULL, NULL};
static int full_screen_buf_index = 0;

/*----------------------------------------------------------------------------*/
/**@brief   imb缩放接口，外部输入背景数据
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_effect_output(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait)
{
    /* printf("[%d, %d, %d, %d], block [%d, %d, %d, %d], type:%d, buf:0x%x, stride:%d, wait:%d\n", rect->left, rect->top, rect->width, rect->height, left, top, width, height, type, lcd_buf, stride, wait); */
    /* put_buf(lcd_buf, stride * height); */
    /* os_time_dly(50); */

    u8 *p = full_screen_buf[full_screen_buf_index];
    dma_memcpy(psram_cache2nocache_addr(&p[top * stride]), lcd_buf, stride * height);
}


/*----------------------------------------------------------------------------*/
/**@brief   若imb_data_output里面的接口为非阻塞接口(例如dma操作)， 这里需要等待处理完成， 若为阻塞接口，这里不需要改动
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_effect_output_wait_finish(u8 buf_index)
{
    dma_memcpy_wait_idle();
    imb_buffer_unlock(buf_index);/* 解锁buffer(必须) */
}



/* ------------------------------------------------------------------------------------*/
/**
 * @brief imb_scale_blend 创建具体缩放任务
 *
 * @Params priv 缩放配置，参考imb_create_large_image_scale_task函数内配置
 */
/* ------------------------------------------------------------------------------------*/
static void imb_scale_blend(void *priv)
{
    int ret;
    int i;

    struct scale_info *info = (struct scale_info *)priv;

    printf("scale ratio : %f, %d x %d -> %d x %d\n",
           info->ratio_w,
           info->src.width, info->src.height, info->dst.width, info->dst.height
          );

    /* 获取LCD操作接口，在lcd_drive.c中实现 */
    struct lcd_info lcd_info = {0};
    lcd_drv_get_info(&lcd_info);
    ASSERT(lcd_info.width);
    ASSERT(lcd_info.height);

    u16 colums = lcd_info.width;
    u16 lines = lcd_info.buffer_size / lcd_info.buf_num / lcd_info.stride;

    /* 使用杰理UI buffer管理模块进行buffer管理 */
    lcd_buffer_init(0, lcd_info.buffer,  lcd_info.buffer_size / lcd_info.buf_num, lcd_info.buf_num);

    /* 创建背景任务(必须) */
    imb_create_bg_task(lcd_info.color_format, lcd_info.width, lcd_info.height, info->backcolor);


    /* 创建每个分块的缩放任务 */
    int h, v;
    for (v = 0; v < info->v_block_n; v++) {
        for (h = 0; h < info->h_block_n; h++) {
            u8 block_index = v * info->h_block_n + h;
            struct image_file tmp_file;
            tmp_file.width = info->src_block[block_index].rect.width;
            tmp_file.height = info->src_block[block_index].rect.height;
            tmp_file.format = info->dst_format;
            tmp_file.offset = 0;
            tmp_file.len = (tmp_file.width * info->dst_bpp + 3) / 4 * 4 * tmp_file.height;
            /* 创建每个分块的缩放任务 */
            imb_create_image_scale_task(imb_task_get_id(), &tmp_file, info->src_block[block_index].buf, info->ratio_w, info->ratio_h, info->dst_block[block_index].left, info->dst_block[block_index].top, info->dst_block[block_index].width, info->dst_block[block_index].height, NULL, false, LOAD_FROM_STATIC_RAM);
        }
    }

    /* 打印所有任务信息 */
    /* imb_task_info_dump(imb_task_head_get(0)); */

    /* 启动合成&推屏 */
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = lcd_info.width;
    screen_rect.height = lcd_info.height;

    struct rect rect;//当前刷新区域
    rect.left = 0;
    rect.top = 0;
    rect.width = lcd_info.width;
    rect.height = lcd_info.height;

    extern u8 *get_psram_frame_buffer();
    extern u8 *get_psram_obuf();
    full_screen_buf[0] = get_psram_obuf();
    full_screen_buf[1] = get_psram_obuf();
    ASSERT(full_screen_buf[0]);
    ASSERT(full_screen_buf[1]);

    /* 设置回调 */
    imb_set_output_cb(imb_effect_output, imb_effect_output_wait_finish);

    /* 配置输出 */
    imb_task_head_set_buf(imb_task_head_get(0), lcd_info.buffer, lcd_info.buffer_size, lcd_info.width, lcd_info.height, lcd_info.stride, lines, lcd_info.buf_num);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
    /* 启动合成 */
    imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, NULL);

    /* 清空回调 */
    imb_set_output_cb(NULL, NULL);

    /* 启动推屏 */
    extern int lcd_draw_area(u8 index, u8 * lcd_buf, int left, int top, int width, int height, int wait);
    lcd_draw_area(0, psram_cache2nocache_addr(full_screen_buf[full_screen_buf_index]), 0, 0, lcd_info.width, lcd_info.height, 0x21);
    /* 等待推屏完成 */
    lcd_wait();
    full_screen_buf_index = !full_screen_buf_index;

    /* 销毁IMB任务链 */
    imb_task_all_destroy(imb_task_head_get(0));

    /* 释放LCD buffer */
    lcd_buffer_release(0);
}



/* ------------------------------------------------------------------------------------*/
/**
 * @brief imb_create_large_image_scale_task 创建大尺寸图片分块缩放任务
 *
 * 本demo主要提供大尺寸图片分块缩放的实现思路，开发者可根据自身项目需求进行调整。
 * 如果需要实现较大尺寸图片的缩放功能
 *  1. 先将图片从flash读取到PSRAM
 *  2. 使用本接口模式对图片进行缩放
 *
 * 如果需要在划屏过程中实现缩放过度效果：
 *	1. 开始时，用普通的imb任务链合成页面，并将页面输出到PSRAM中缓存(开始瞬间把输出由原来的屏幕切换到PSRAM作为大图缓存)
 *  2. 划屏时，用大尺寸分块缩放，将PSRAM中缓存的页面作为一整张大图片，使用本缩放模式进行缩放，并输出到屏幕
 *  3. 结束时，切换回普通imb任务链形式，将合成的页面数据输出到屏幕
 *
 * @Params image 大尺寸图片地址，内部ram或PSRAM
 * @Params width 原图片宽度
 * @Params height 源图片高度
 * @Params src_format 源图片像素格式
 * @Params src_bpp 源图片像素格式bit数，如demo中使用的图片为RGB565格式，每个像素2byte，则src_bpp为2，主要用于缓存buf大小的计算
 * @Params ratio_w 宽度缩放倍率
 * @Params ratio_h 高度缩放倍率
 * @Params status 状态标志(主要用于计算和申请缓存)，0表示申请并释放(适用一次缩放就结束的场景)。1、2、3适用需多次缩放的场景，1表示只计算、申请缓存，不释放；2表示不申请、也不释放；3表示只释放，不申请。需多次缩放时，开始为1，中间为2，结束为3即可。
 *
 * @return
 */
/* ------------------------------------------------------------------------------------*/
int imb_create_large_image_scale_task(u8 *image, int width, int height, int src_format, int src_bpp, float ratio_w, float ratio_h, int status)
{
    static struct scale_info info = {0};
    info.width = width;
    info.height = height;
    info.src_format = src_format;
    info.src_bpp = src_bpp;
    info.backcolor = 0xffff;
    info.dst_format = src_format;
    info.dst_bpp = src_bpp;
    info.status = status;

    info.src.left = 0;
    info.src.top = 0;
    info.src.width = width;
    info.src.height = height;

    info.dst.left = info.src.width * (1.0 - ratio_w) / 2;
    info.dst.top = info.src.height * (1.0 - ratio_h) / 2;
    info.dst.width = info.src.width * ratio_w;
    info.dst.height = info.src.height * ratio_h;

    info.ratio_w = ratio_w;
    info.ratio_h = ratio_h;

    /* 2D模块输入最大分辨率240x240，这里把全屏尺寸分割成多个240以内的小图 */
    int h_block_n = (width + 239) / 240;
    int v_block_n = (height + 239) / 240;
    int h, v;
    info.h_block_n = h_block_n;
    info.v_block_n = v_block_n;

    /* 状态为0或1时，计算分块参数并申请分块的缓存buffer */
    if ((info.status == 0) || (info.status == 1)) {
        info.src_block = (struct src_block_info *)zalloc(h_block_n * v_block_n * sizeof(struct src_block_info));
        if (!info.src_block) {
            return -1;
        }
        info.dst_block = (struct rect *)zalloc(h_block_n * v_block_n * sizeof(struct rect));
        if (!info.dst_block) {
            return -1;
        }
        int h_block_len = width / h_block_n;
        int v_block_len = height / v_block_n;


        if (info.src_format == info.dst_format) {
            struct image_file tmp_file;
            tmp_file.width = info.width;
            tmp_file.height = info.height;
            tmp_file.format = info.src_format;
            tmp_file.offset = 0;
            tmp_file.len = (tmp_file.width * info.src_bpp + 3) / 4 * 4 * tmp_file.height;
            img_set_input(DATA_SRC_SRAM, DATA_NOZIP, tmp_file.width, tmp_file.height, tmp_file.format, 0, image);
        } else {
            ASSERT(0, "not support!");
        }

        /* 计算源图片分块参数 */
        for (v = 0; v < v_block_n; v++) {
            for (h = 0; h < h_block_n; h++) {
                u8 block_index = v * h_block_n + h;
                info.src_block[block_index].rect.left = h * h_block_len;
                info.src_block[block_index].rect.top = v * v_block_len;
                if (h == (h_block_n - 1)) {
                    info.src_block[block_index].rect.width = width - info.src_block[block_index].rect.left;
                } else {
                    info.src_block[block_index].rect.width = h_block_len;
                }
                if (v == (v_block_n - 1)) {
                    info.src_block[block_index].rect.height = height - info.src_block[block_index].rect.top;
                } else {
                    info.src_block[block_index].rect.height = v_block_len;
                }

                info.src_block[block_index].buf_size = ((info.src_block[block_index].rect.width * info.dst_bpp + 3) / 4 * 4) * info.src_block[block_index].rect.height;
                info.src_block[block_index].buf = (u8 *)malloc_psram(info.src_block[block_index].buf_size);
                ASSERT(info.src_block[block_index].buf);

                if (info.src_format == info.dst_format) {
                    img_set_output(info.src_block[block_index].buf, info.src_block[block_index].rect.left, info.src_block[block_index].rect.top, info.src_block[block_index].rect.width, info.src_block[block_index].rect.height);
                    img_kick_sync();
                } else {
                    ASSERT(0, "not support!");
                }
            }
        }
    }

    /* 计算目标缩放分块参数 */
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            u8 block_index = v * h_block_n + h;
            info.dst_block[block_index].left = info.dst.left + (h ? 1 : 0) * info.dst_block[(v) * h_block_n + (h - 1)].width;
            info.dst_block[block_index].top = info.dst.top + (v ? 1 : 0) * info.dst_block[(v - 1) * h_block_n + (h)].height;
            info.dst_block[block_index].width = info.src_block[block_index].rect.width * ratio_w;
            info.dst_block[block_index].height = info.src_block[block_index].rect.height * ratio_h;
        }
    }

    /* 创建分块缩放任务 */
    imb_scale_blend(&info);

    /* 状态为0或3时，释放缓存buffer */
    if ((info.status == 0) || (info.status == 3)) {
        for (v = 0; v < v_block_n; v++) {
            for (h = 0; h < h_block_n; h++) {
                u8 block_index = v * h_block_n + h;
                if (info.src_block[block_index].buf) {
                    free_psram(info.src_block[block_index].buf);
                    info.src_block[block_index].buf = NULL;
                }
            }
        }

        if (info.src_block) {
            free(info.src_block);
        }
        if (info.dst_block) {
            free(info.dst_block);
        }
    }

    return 0;
}
#endif

/*----------------------------------------------------------------------------*/
/**@brief    打印任务链的所有任务
   @param    root:任务列表头
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_task_info_dump(struct imb_task_head *root)
{
    struct imb_task *p;

    printf("------------------imb_task info dump begin--------------------\n");
    list_for_each_entry(p, &root->entry, entry) {
        printf("[%d]%d, %s, %d x %d, [%d, %d, %d, %d], %d\n", root->index, p->index, p->name, p->src_w, p->src_h, p->rect.left, p->rect.top, p->rect.width, p->rect.height, p->cur_format);
    }
    printf("-------------------imb_task info dump end--------------------\n");
}


/*----------------------------------------------------------------------------*/
/**@brief
   @param   type : 1 - 全屏区域,  2 - 局部区域
   @param   rect : 当前帧需要刷新的区域(该区域会根据buffer大小分成n块)
   @param   lcd_buf : 当前分块buffer的地址
   @param   stride : 当前分块一行像素的字节数（2D硬件加速生成的是4字节对齐，不足4字节则补齐）
   @param   left, top, width, height : 当前分块的范围
   @param   wait : 0 - 中间分块， 1 - 第一个分块， 2 - 最后一个分块(若只有一个分块则为1， 若只有两个分块,则第一个分块为1，第二个分块为2)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_data_output(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait)
{
    /* printf("[%d, %d, %d, %d], block [%d, %d, %d, %d], type:%d, buf:0x%x, stride:%d, wait:%d\n", rect->left, rect->top, rect->width, rect->height, left, top, width, height, type, lcd_buf, stride, wait); */
    /* put_buf(lcd_buf, stride * height); */
    /* os_time_dly(50); */
}


/*----------------------------------------------------------------------------*/
/**@brief   若imb_data_output里面的接口为非阻塞接口(例如dma操作)， 这里需要等待处理完成， 若为阻塞接口，这里不需要改动
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_data_output_wait_finish(u8 buf_index)
{
    imb_buffer_unlock(buf_index); /* 解锁buffer(必须) */
}


/*----------------------------------------------------------------------------*/
/**@brief   imb背景输入接口
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void *bg_task_in(void *priv)
{
    struct imb_task *task = (struct imb_task *)priv;
    struct bg_buffer *b = task->cb_priv;
    struct imb_task_head *head = imb_task_head_get(task->root_index);
    struct rect *rect = &head->curr_draw;
    u8 *buf = get_imb_output_buf(head);
    u32 len = head->buf_stride * rect->height;
    int w, h;

    printf("bg_task_in [%d, %d, %d, %d]\n", rect->left, rect->top, rect->width, rect->height);
    //TO DO 这里可以执行清背景操作或者把背景数据拷贝到buf
    /* memset(buf, 0xff, len); */
    for (h = 0; h < rect->height; h++) {
        for (w = 0; w < rect->width; w++) {
            buf[h * head->buf_stride + 2 * w] = 0xf8;
            buf[h * head->buf_stride + 2 * w + 1] = 0x00;
        }
    }

    return NULL;
}




void imb_image_scale(int bg_width, int bg_height, struct image_scale_info *scale)
{
    struct image_file file;

    struct lcd_info info = {0};
    static struct bg_buffer bg = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }
    bg.buf = info.buffer;
    bg.len = info.buffer_size;

    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.osd0_in_buf    = 1; //0:背景任务生效 1:背景任务不生效,使用外部背景数据
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;//输出格式
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.src_w          = bg_width;
    imb_tsk_info.src_h          = bg_height;
    imb_tsk_info.des_w          = bg_width;
    imb_tsk_info.des_h          = bg_height;
    imb_tsk_info.priv           = 0;
    imb_tsk_info.cb             = bg_task_in;
    imb_tsk_info.cb_priv        = &bg;
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);

    //从sram加载argb8565格式图片缩放测试 注意:每行字节数需4对齐,若不足4字节则补齐4字节
    file.width = scale->image_w;
    file.height = scale->image_h;
    file.format = scale->format;
    file.offset = 0;
    imb_create_image_scale_task(imb_task_get_id(), &file, scale->image, scale->ratio_w, scale->ratio_h,
                                scale->x, scale->y, scale->image_w * scale->ratio_w, scale->image_h * scale->ratio_h,
                                NULL, false, LOAD_FROM_STATIC_RAM);

    //打印所有任务信息
    imb_task_info_dump(imb_task_head_get(0));

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = bg_width;
    screen_rect.height = bg_height;

    struct rect rect;//当前刷新区域(可局部刷新，也可全屏刷新)
    rect.left = 0;
    rect.top = 0;
    rect.width = bg_width;
    rect.height = bg_height;

    /* imb_set_output_cb(imb_data_output, imb_data_output_wait_finish); */

    u16 bg_stride = (bg_width * 2 + 3) / 4 * 4;
    u16 colums = bg_width;
    u16 lines = info.buffer_size / info.buf_num / bg_stride;
    imb_task_head_set_buf(imb_task_head_get(0), info.buffer, info.buffer_size, bg_width, bg_height, bg_stride, lines, info.buf_num);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
    imb_start(imb_task_head_get(0), &screen_rect, disp, bg_width, lines, NULL);

    imb_task_all_destroy(imb_task_head_get(0));
}


/*----------------------------------------------------------------------------*/
/**@brief    构造flash地址映射表(不依赖文件系统)，只适用于地址连续的情况
   @param    flash_file_info : 文件地址信息
   @param    file_base_addr : 文件存放在flash的物理地址
   @param    filelen : 文件长度
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int flash_tab_info_get(struct flash_file_info *file_info, u32 file_base_addr, u32 filelen)
{
    int i;
    u32 flash_map_tab_num;

    flash_map_tab_num = (file_base_addr % 4096 + filelen + 4096 - 1) / 4096;

    file_info->tab_size = flash_map_tab_num * sizeof(u32);
    file_info->tab = (u32 *)malloc(file_info->tab_size);
    file_info->offset = file_base_addr % 4096;
    file_info->last_tab_data_len = flash_map_tab_num * 4096 - filelen - file_info->offset;
    for (i = 0; i < flash_map_tab_num; i++) {
        file_info->tab[i] = file_base_addr / 4096 * 4096 + 4096 * i;
        printf("info->tab[%d] : 0x%x\n", i, file_info->tab[i]);
    }

    return 0;
}

void flash_tab_info_release(struct flash_file_info *file_info)
{
    if (file_info->tab) {
        free(file_info->tab);
        file_info->tab = NULL;
    }
}


/*----------------------------------------------------------------------------*/ /**@brief    imb调试接口
   @param    root : 任务链句柄
   @param    p : 任务句柄
   @return
   @note     该接口由库调用，可以打印imb_task的相关参数
*/
/*----------------------------------------------------------------------------*/
int imb_task_debug(struct imb_task_head *root, struct imb_task *p)
{
    //调试时取消注释以下打印
    /* printf("[%d] : %d x %d, [%d, %d, %d x %d], [%d, %d, %d x %d], [%d, %d, %d x %d], %d, 0x%x, 0x%x, 0x%x\n", */
    /* p->index, p->src_w, p->src_h, */
    /* p->rect.left, p->rect.top, p->rect.width, p->rect.height, */
    /* p->crop.left, p->crop.top, p->crop.width, p->crop.height, */
    /* p->x_offset, p->y_offset, p->des_w, p->des_h, */
    /* p->cur_format, p->adr_map_adr, p->adr_map_adr ? ((u32 *)p->adr_map_adr)[0] : 0, p->dat_src_adr); */
    return 0;
}



/*----------------------------------------------------------------------------*/
/**@brief    flash地址映射表空间释放
   @param    flash_file_info : 文件地址信息
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int flash_tab_info_free(struct flash_file_info *file_info)
{
    if (file_info->tab) {
        free(file_info->tab);
        file_info->tab = NULL;
    }
    return 0;
}


static void *imb_draw_cb_test(void *priv)
{
    struct imb_task_out *out = (struct imb_task_out *)priv;
    struct imb_task *task = out->task;
    struct rect *rect = &out->rect;
    u8 *pbuf = (u8 *)out->outbuf;
    u8 bytes_per_pixel = (out->format == OUTPUT_FORMAT_RGB888) ? 3 : 2;
    int dst_stride = (rect->width * bytes_per_pixel + 3) / 4 * 4;
    int w, h;

    for (h = 0; h < rect->height; h++) {
        for (w = 0; w < rect->width; w++) {
            pbuf[h * dst_stride + 2 * w] = 0xff;
            pbuf[h * dst_stride + 2 * w + 1] = 0xff;
        }
    }

    return NULL;
}

struct rect *image_set_crop(int x, int y, int width, int height)
{
    static struct rect crop = {0};
    crop.left = x;
    crop.top = y;
    crop.width = width;
    crop.height = height;
    return &crop;
}

void imb_image_decompress_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;
    int pixel_size;

    //1.图片解压测试(依赖文件系统)
    u8 *palette = NULL;
    u8 *pixel = NULL;
    id = PAGE0_4e97_04_MENU_ICON_HEART_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    struct rect crop = {0};//截取区域,不能超出原图区域
    crop.left = 0;
    crop.top = 0;
    crop.width = file.width;
    crop.height = file.height;

    u16 palette_size = image_palette_size(file.format);//获取图片颜色表大小，返回0表示无颜色表
    if (palette_size) {
        palette = imb_malloc(palette_size, false);
        image_read_palette(&file, &info->ui_resfile_info, palette);
    }

    pixel_size = image_pixel_size(crop.width, crop.height, file.format);
    /*
     *  注意 ：每行像素字节数需4字节对齐，不足4字节则需要补齐
     * */
    if (pixel_size != -1) {
        pixel = (u8 *)imb_malloc(pixel_size, false);
        image_read_pixel(&file, &info->ui_resfile_info, pixel,  crop.left, crop.top, crop.width, crop.height);
    }

    printf("lut_tab:\n");
    put_buf(palette, palette_size);
    printf("image_pixel: %d x %d, %d\n", crop.width, crop.height, file.format);
    put_buf(pixel, pixel_size);

    if (palette) {
        imb_free(palette);
        palette = NULL;
    }
    if (pixel) {
        imb_free(pixel);
        pixel = NULL;
    }


    u32 addr;
    u32 image_size;
    u8 *image_buf;
    //2.图片阻塞式解压测试(不依赖文件系统)
    id = PAGE0_976c_ICON1;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    addr = get_image_addr(info->ui_resfile, info->path, id);//获取图片的flash物理地址
    struct flash_file_info tmp_file_info;
    flash_tab_info_get(&tmp_file_info, addr, file.len);//构造地址映射表(假设数据是连续存放的, 若数据是非连续存放，需根据实际情况获取)
    image_size = image_pixel_size(file.width, file.height, file.format);
    image_buf = (u8 *)malloc(image_size);
    img_set_input(DATA_SRC_FLASH, DATA_ZIP, file.width, file.height, file.format, tmp_file_info.tab, tmp_file_info.offset);
    img_set_output(image_buf, 0, 0, file.width, file.height);
    img_kick_sync();
    printf("decompress image pixel: %d x %d\n", file.width, file.height);
    put_buf(image_buf, image_size);
    free(image_buf);
    flash_tab_info_release(&tmp_file_info);

    //3.图片非阻塞式解压测试(不依赖文件系统)
    id = PAGE0_26a3_ICON2;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    addr = get_image_addr(info->ui_resfile, info->path, id);//获取图片的flash物理地址
    if (addr % 4 == 0) {//起始地址必须4对齐
        image_size = image_pixel_size(file.width, file.height, file.format);
        image_buf = (u8 *)malloc(image_size);
        img_set_input(DATA_SRC_SRAM, DATA_ZIP, file.width, file.height, file.format, NULL, addr);
        img_set_output(image_buf, 0, 0, file.width, file.height);
        img_kick_async();
        img_kick_async_wait_complete();
        printf("decompress image pixel: %d x %d\n", file.width, file.height);
        put_buf(image_buf, image_size);
        free(image_buf);
    }
}

void imb_image_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;
    int pixel_size;
    u8 *image_pixel;
    u32 file_addr;
    u32 file_len;

    //1.图片普通输出测试(依赖文件系统)
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_image_task(imb_task_get_id(), &file, &info->ui_resfile_info, 50, 200, file.width, file.height, NULL, true, LOAD_FROM_FLASH_WITH_TAB);//width,height参数可小于file.width, file.height,若小于则裁剪

    //2.直接指定图片的flash地址以及长度显示测试(若知道图片的flash存放地址，可不依赖文件系统)
    id = PAGE0_4e97_04_MENU_ICON_HEART_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    struct flash_file_info temp_file_info;
    file_addr = info->ui_resfile_info.tab[0] + info->ui_resfile_info.offset + file.offset;//获取文件的物理地址
    file_len = file.len;//文件长度
    flash_tab_info_get(&temp_file_info, file_addr, file_len);//通过文件的物理地址和长度得到文件的地址映射表
    struct image_file tmp_file;
    tmp_file.width = file.width;
    tmp_file.height = file.height;
    tmp_file.format = file.format;
    tmp_file.offset = 0;
    tmp_file.len = file.len;
    imb_create_image_task(imb_task_get_id(), &tmp_file, &temp_file_info, 155, 200, tmp_file.width, tmp_file.height, NULL, true, LOAD_FROM_FLASH_WITH_TAB);

    //3.直接指定图片的flash地址(不需要地址映射表，数据需要连续存放)
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    file_addr = info->ui_resfile_info.tab[0] + info->ui_resfile_info.offset + file.offset;//获取文件的物理地址
    imb_create_image_task(imb_task_get_id(), &file, file_addr, 260, 200, file.width, file.height, NULL, true, LOAD_FROM_FLASH_WITHOUT_TAB);

    //4.从flash解压图像数据到非掉电RAM, imb从非掉电RAM访问数据
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    pixel_size = image_pixel_size(file.width, file.height, file.format);
    image_pixel = (u8 *)malloc(pixel_size);
    image_read_pixel(&file, &info->ui_resfile_info, image_pixel,  0, 0, file.width, file.height);
    imb_create_image_task(imb_task_get_id(), &file, image_pixel, 365, 200, file.width, file.height, NULL, false, LOAD_FROM_RAM);

    //5.从flash解压图像数据到掉电RAM, imb从掉电RAM访问数据
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    pixel_size = image_pixel_size(file.width, file.height, file.format);
    image_pixel = (u8 *)malloc_vlt(pixel_size);
    image_read_pixel(&file, &info->ui_resfile_info, image_pixel,  0, 0, file.width, file.height);
    imb_create_image_task(imb_task_get_id(), &file, image_pixel, 50, 95, file.width, file.height, NULL, false, LOAD_FROM_VLT_RAM);

    //6.从flash解压图像数据到PSRAM, imb从PSRAM访问数据
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    pixel_size = image_pixel_size(file.width, file.height, file.format);
    image_pixel = (u8 *)malloc_psram(pixel_size);
    image_read_pixel(&file, &info->ui_resfile_info, image_pixel,  0, 0, file.width, file.height);
    imb_create_image_task(imb_task_get_id(), &file, image_pixel, 155, 95, file.width, file.height, NULL, false, LOAD_FROM_PSRAM);
#endif

    //7.从flash解压图像数据到掉电RAM或非掉电RAM, imb从RAM访问数据
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    pixel_size = image_pixel_size(file.width, file.height, file.format);
    image_pixel = (u8 *)imb_malloc(pixel_size, false);
    image_read_pixel(&file, &info->ui_resfile_info, image_pixel,  0, 0, file.width, file.height);
    imb_create_image_task(imb_task_get_id(), &file, image_pixel, 260, 95, file.width, file.height, NULL, false, LOAD_FROM_RAM_AUTO);

    //8.图片裁剪
    id = PAGE0_4994_03_MENU_ICON_STATE_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    pixel_size = image_pixel_size(file.width, file.height, file.format);
    image_pixel = (u8 *)imb_malloc(pixel_size, false);
    image_read_pixel(&file, &info->ui_resfile_info, image_pixel,  0, 0, file.width, file.height);
    imb_create_image_task(imb_task_get_id(), &file, image_pixel, 365, 95, file.width, file.height, image_set_crop(365, 95, file.width / 2, file.height), false, LOAD_FROM_RAM_AUTO);

    //9.从静态ram加载图片数据
    tmp_file.width = 48;
    tmp_file.height = 49;
    tmp_file.format = LAYER_FORMAT_RGB565;
    imb_create_image_task(imb_task_get_id(), &tmp_file, icon_48x49_rgb565, 100, 304, tmp_file.width, tmp_file.height, NULL, false, LOAD_FROM_STATIC_RAM);

    tmp_file.width = 48;
    tmp_file.height = 49;
    tmp_file.format = LAYER_FORMAT_ARGB8565;
    imb_create_image_task(imb_task_get_id(), &tmp_file, icon_48x49_argb8565, 150, 304, tmp_file.width, tmp_file.height, NULL, false, LOAD_FROM_STATIC_RAM);
}

void imb_image_scale_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;

    //1.图片缩放输出测试(其他方式可参考图片任务接口的用法，跟图片任务类似)
    id = PAGE0_5dad_05_MENU_ICON_BLOOD_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_image_scale_task(imb_task_get_id(), &file, &info->ui_resfile_info, 1.5, 1.5, 180, 280, file.width * 1.5, file.height * 1.5, NULL, true, LOAD_FROM_FLASH_WITH_TAB);

    id = PAGE0_976c_ICON1;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    float ratio_w = 75 * 1.0f / 70;
    float ratio_h = 75 * 1.0f / 70;
    imb_create_image_scale_task(imb_task_get_id(), &file, &info->ui_resfile_info, ratio_w, ratio_h, 100, 200, file.width * ratio_w, file.height * ratio_h, NULL, true, LOAD_FROM_FLASH_WITH_TAB);

    id = PAGE0_26a3_ICON2;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    ratio_w = 77 * 1.0f / 70;
    ratio_h = 77 * 1.0f / 70;
    imb_create_image_scale_task(imb_task_get_id(), &file, &info->ui_resfile_info, ratio_w, ratio_h, 180, 200, file.width * ratio_w, file.height * ratio_h, NULL, true, LOAD_FROM_FLASH_WITH_TAB);

    //2.图片缩放+裁剪测试
    id = PAGE0_5dad_05_MENU_ICON_BLOOD_SEL;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);

    int scale_x = 50;
    int scale_y = 50;
    float scale_ratio_w = 1.9;
    float scale_ratio_h = 1.9;
    imb_create_image_scale_task(imb_task_get_id(), &file, &info->ui_resfile_info, scale_ratio_w, scale_ratio_h, scale_x, scale_y, file.width * scale_ratio_w, file.height * scale_ratio_h, image_set_crop(scale_x + file.width * 0.1f, scale_y + file.height * 0.1f, file.width * 1.7f, file.height * 1.7f), true, LOAD_FROM_FLASH_WITH_TAB);
}

#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
void imb_large_image_scale_test(struct ui_res_info *info)
{
    struct image_file file;
    int id = PAGE0_18e2_CLOCK;
    int ret;
    int i;

    /* 杰理UI框架，根据图片ID打开一个全屏大小的图片 */
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);

    /* 杰理UI框架，获取图片地址(flash) */
    u32 fileaddr = get_image_addr(info->ui_resfile, info->path, id);

    /* 2D加速模块，计算stride参数 */
    int stride = image_stride(file.width, file.format);

    /* PSRAM，申请PSRAM buffer作为缓存 */
    u8 *image_buf = malloc_psram(stride * file.height);
    ASSERT(image_buf);

    /* img，从flash读取图片资源并放到psram buffer缓存中 */
    if (fileaddr % 4 == 0) {
        img_set_input(DATA_SRC_SRAM, DATA_ZIP, file.width, file.height, file.format, NULL, fileaddr);//起始地址需4对齐
        img_set_output(image_buf, 0, 0, file.width, file.height);
        img_kick_sync();
    } else {
        struct flash_file_info file_info;
        flash_tab_info_get(&file_info, fileaddr, file.len);
        img_set_input(DATA_SRC_FLASH, DATA_ZIP, file.width, file.height, file.format, file_info.tab, fileaddr % 4096);
        img_set_output(image_buf, 0, 0, file.width, file.height);
        img_kick_sync();
        flash_tab_info_release(&file_info);
    }
    /* 大尺寸图片分块缩放测试 */
    while (1) {
        float ratio_table1[] = {2.0, 1.6, 1.3, 1.1, 1.0};
        for (i = 0; i < (sizeof(ratio_table1) / sizeof(ratio_table1[0])); i++) {
            int status = 0;
            if (i == 0) {
                status = 1;
            } else if (i == (sizeof(ratio_table1) / sizeof(ratio_table1[0]) - 1)) {
                status = 3;
            } else {
                status = 2;
            }
            /* 创建大尺寸图片分块缩放任务 */
            imb_create_large_image_scale_task(image_buf, file.width, file.height, file.format, 2, ratio_table1[i], ratio_table1[i], status);
        }
        os_time_dly(100);

        float ratio_table3[] = {1.0, 0.9, 0.8, 0.7, 0.5};
        for (i = 0; i < (sizeof(ratio_table3) / sizeof(ratio_table3[0])); i++) {
            int status = 0;
            if (i == 0) {
                status = 1;
            } else if (i == (sizeof(ratio_table3) / sizeof(ratio_table3[0]) - 1)) {
                status = 3;
            } else {
                status = 2;
            }
            imb_create_large_image_scale_task(image_buf, file.width, file.height, file.format, 2, ratio_table3[i], ratio_table3[i], status);
        }
        os_time_dly(100);


        float ratio_table4[] = {0.5, 0.7, 0.8, 0.9, 1.0};
        for (i = 0; i < (sizeof(ratio_table4) / sizeof(ratio_table4[0])); i++) {
            int status = 0;
            if (i == 0) {
                status = 1;
            } else if (i == (sizeof(ratio_table4) / sizeof(ratio_table4[0]) - 1)) {
                status = 3;
            } else {
                status = 2;
            }
            imb_create_large_image_scale_task(image_buf, file.width, file.height, file.format, 2, ratio_table4[i], ratio_table4[i], status);
        }
        os_time_dly(100);


        float ratio_table2[] = {1.0, 1.1, 1.3, 1.6, 2.0};
        for (i = 0; i < (sizeof(ratio_table2) / sizeof(ratio_table2[0])); i++) {
            int status = 0;
            if (i == 0) {
                status = 1;
            } else if (i == (sizeof(ratio_table2) / sizeof(ratio_table2[0]) - 1)) {
                status = 3;
            } else {
                status = 2;
            }
            imb_create_large_image_scale_task(image_buf, file.width, file.height, file.format, 2, ratio_table2[i], ratio_table2[i], status);
        }
        os_time_dly(100);
    }

    free_psram(image_buf);
}
#endif




void imb_image_rotate_chunk_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;

    //图片分块输入旋转输出测试
#if 1
    id = PAGE0_7b17_COMPASS;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);

    imb_create_image_rotate_chunk_task(imb_task_get_id(), &file, &info->ui_resfile_info, 227, 227, 90, 227, 227);
#else
    id = PAGE0_7b17_COMPASS;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    static struct flash_file_info temp_file_info;
    u32 file_addr = info->ui_resfile_info.tab[0] + info->ui_resfile_info.offset + file.offset;//获取文件的物理地址
    u32 file_len = file.len;//文件长度
    flash_tab_info_get(&temp_file_info, file_addr, file_len);//通过文件的物理地址和长度得到文件的地址映射表
    struct image_file tmp_file;
    tmp_file.width = file.width;
    tmp_file.height = file.height;
    tmp_file.format = file.format;
    tmp_file.offset = 0;
    tmp_file.len = file.len;

    imb_create_image_rotate_chunk_task(imb_task_get_id(), &tmp_file, &temp_file_info, 227, 227, 90, 227, 227);
#endif
}

void imb_image_rotate_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;
    int pixel_size;
    u8 *image_pixel;
    //1.图片旋转输出测试
    id = PAGE0_9d77_HOUR;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_image_rotate_task(imb_task_get_id(), &file, &info->ui_resfile_info, 23, 133, 45, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);

    id = PAGE0_5fd9_MINUTE_SEC;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);

    //2.图片旋转输出测试(从sram加载)
    //将flash图像数据解压到sram
    pixel_size = image_pixel_size(file.width, file.height, file.format);
    image_pixel = (u8 *)imb_malloc(pixel_size, false);
    image_read_pixel(&file, &info->ui_resfile_info, image_pixel,  0, 0, file.width, file.height);
    //从sram里加载图片数据
    imb_create_image_rotate_task(imb_task_get_id(), &file, image_pixel, 22, 202, 180, 227, 227, NULL, LOAD_FROM_RAM_AUTO);

    //3.图片旋转+裁剪测试(旋转裁剪时裁剪区域必须包含圆心)
    imb_create_image_rotate_task(imb_task_get_id(), &file, &info->ui_resfile_info, 22, 202, 270, 227, 227, image_set_crop(150, 200, 104, 54), LOAD_FROM_FLASH_WITH_TAB);
}

void imb_string_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;

    //多国语言字符串输出测试
    id = M26;
    ret = open_string_by_id(0, info->ui_strfile, &file, id);
    ASSERT(!ret);
    imb_create_strpic_task(imb_task_get_id(), &file, &info->ui_strfile_info, 100, 150, file.width, file.height, 0xf800); //width,height参数可小于file.width, file.height,若小于则裁剪

    id = M27;
    ret = open_string_by_id(0, info->ui_strfile, &file, id);
    ASSERT(!ret);
    imb_create_strpic_task(imb_task_get_id(), &file, &info->ui_strfile_info, 150, 150, file.width, file.height, 0x001f);
}

void imb_font_test()
{
    //字库显示测试（为了提高效率，建议先将字库拼接成一张单色的图像）
    int width;
    int height;
    int ret;
    int id;

    ret = font_ascii_init(RES_PATH"font/ascii.res");
    ASSERT(!ret);
    u8 *pixbuf = ascii_image_splice("0123456789", &width, &height);
    ASSERT(pixbuf);
    imb_create_text_task(imb_task_get_id(), 120, 120, width, height, pixbuf, 0xf800, NULL);
}

void imb_custom_draw_test()
{
    //自定义绘图测试
    struct image_buf image1;
    image1.buf = icon_48x49_rgb565;
    image1.len = sizeof(icon_48x49_rgb565);
    imb_create_custom_task(imb_task_get_id(), 100, 250, 48, 49, imb_draw_cb, &image1, sizeof(struct image_buf), DRAW_IMAGE);

    struct image_buf image2;
    image2.buf = icon_48x49_argb8565;
    image2.len = sizeof(icon_48x49_argb8565);
    imb_create_custom_task(imb_task_get_id(), 150, 250, 48, 49, imb_draw_cb, &image2, sizeof(struct image_buf), DRAW_IMAGE_WITH_ALPHA);
}


/*----------------------------------------------------------------------------*/
/**@brief    纯色任务测试，用于填充矩形
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_color_test()
{
    //color[31:24] : alpha 范围0~100, color[15:0] : rgb565
    imb_create_color_task(imb_task_get_id(), 47, 47, 360, 360, 100 << 24 | 0xf800);
    imb_create_color_task(imb_task_get_id(), 77, 77, 300, 300, 100 << 24 | 0x07e0);
    imb_create_color_task(imb_task_get_id(), 107, 107, 240, 240, 100 << 24 | 0x001f);
}

void imb_jpeg_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file;

    struct lcd_info lcd_info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&lcd_info);
    }

    //1.jpeg图片输出测试(依赖文件系统)
    id = PAGE0_e4c4_TEST0;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_jpeg_task(imb_task_get_id(), &file, &info->ui_resfile_info, lcd_info.color_format, 0, 0, file.width, file.height, LOAD_FROM_FLASH_WITH_TAB);

    //2.直接指定jpeg图片的flash地址显示测试
    id = PAGE0_3a4e_TEST2;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16); //获取jpeg图片的信息
    ASSERT(!ret);
    u32 addr = get_image_addr(info->ui_resfile, RES_PATH"ui/ui.res", PAGE0_3a4e_TEST2);//获取jpeg图片的flash物理地址
    imb_create_jpeg_task(imb_task_get_id(), &file, addr, lcd_info.color_format, 70, 70, file.width, file.height, LOAD_FROM_FLASH_WITHOUT_TAB);

    u8 *filebuf;
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    //3.将jpeg图像数据拷贝到PSRAM, jpeg解码器从PSRAM访问数据
    filebuf = malloc_psram(file.len);
    memcpy(filebuf, addr, file.len);
    imb_create_jpeg_task(imb_task_get_id(), &file, filebuf, lcd_info.color_format, 240, 70, file.width, file.height, LOAD_FROM_PSRAM);
#endif

    //4.将jpeg图像数据拷贝到非掉电RAM, jpeg解码器从非掉电RAM访问数据
    filebuf = malloc(file.len);
    memcpy(filebuf, addr, file.len);
    imb_create_jpeg_task(imb_task_get_id(), &file, filebuf, lcd_info.color_format, 70, 240, file.width, file.height, LOAD_FROM_RAM);

    //5.将jpeg图像数据拷贝到掉电RAM, jpeg解码器从掉电RAM访问数据
    filebuf = malloc_vlt(file.len);
    memcpy(filebuf, addr, file.len);
    imb_create_jpeg_task(imb_task_get_id(), &file, filebuf, lcd_info.color_format, 240, 240, file.width, file.height, LOAD_FROM_VLT_RAM);
}
/*----------------------------------------------------------------------------*/
/**@brief    imb任务与外部背景数据叠加测试
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_task_mixed_with_extern_background()
{
    /* 缩放demo */
    struct image_scale_info scale_info;
    scale_info.ratio_w = 2.0;
    scale_info.ratio_h = 2.0;
    scale_info.image_w = 48;
    scale_info.image_h = 49;
    scale_info.x = 100;
    scale_info.y = 100;
    scale_info.image = icon_48x49_argb8565;
    scale_info.format = LAYER_FORMAT_ARGB8565;
    imb_image_scale(240, 240, &scale_info);
}

void imb_watch_test(struct ui_res_info *info)
{
    int id;
    int ret;
    struct image_file file1;
    struct image_file file2;
    struct image_file file3;
    int bg_id = imb_task_get_id();
    int hour_id = imb_task_get_id();
    int min_id = imb_task_get_id();
    int sec_id = imb_task_get_id();

    struct lcd_info lcd_info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&lcd_info);
    }

    //创建背景任务(必须且需要在其他task之前创建)
    imb_create_bg_task(lcd_info.color_format, lcd_info.width, lcd_info.height, 0xFFFF);

    id = PAGE0_060f_BIRD_1;
    ret = open_image_by_id(0, info->ui_resfile, &file1, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_image_task(bg_id, &file1, &info->ui_resfile_info, 0, 0, file1.width, file1.height, NULL, true, LOAD_FROM_FLASH_WITH_TAB);//width,height参数可小于file.width, file.height,若小于则裁剪

    id = PAGE0_9d77_HOUR;
    ret = open_image_by_id(0, info->ui_resfile, &file2, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_image_rotate_task(hour_id, &file2, &info->ui_resfile_info, 23, 133, 0, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);

    id = PAGE0_5fd9_MINUTE_SEC;
    ret = open_image_by_id(0, info->ui_resfile, &file3, id & 0xff, id >> 16);
    ASSERT(!ret);
    imb_create_image_rotate_task(min_id, &file3, &info->ui_resfile_info, 22, 202, 0, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);

    imb_create_image_rotate_task(sec_id, &file3, &info->ui_resfile_info, 22, 202, 0, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);

    imb_task_blend();
    os_time_dly(100);

    int hour = 0;
    int min = 0;
    int sec = 0;
    while (1) {
        ++sec;
        if (sec == 60) {
            sec = 0;
            ++min;
            if (min == 60) {
                min = 0;
                ++hour;
                if (hour == 12) {
                    hour = 0;
                }
            }
        }
        imb_create_image_rotate_task(hour_id, &file2, &info->ui_resfile_info, 23, 133, hour * 30 + min * 30 / 60 + sec * 30 / 60 / 60, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);
        imb_create_image_rotate_task(min_id, &file3, &info->ui_resfile_info, 22, 202, min * 6 + sec / 10, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);
        imb_create_image_rotate_task(sec_id, &file3, &info->ui_resfile_info, 22, 202, sec * 6, 227, 227, NULL, LOAD_FROM_FLASH_WITH_TAB);
        imb_task_blend();
        os_time_dly(100);
    }
}


void lcd_disp_init(void *arg)
{
    struct lcd_info info = {0};
    __this->lcd = lcd_get_hdl();
    ASSERT(__this->lcd);

    if (__this->lcd->init) {
        __this->lcd->init(arg);
    }

    if (__this->lcd->clear_screen) {
        __this->lcd->clear_screen(0x000000);
    }

    if (__this->lcd->backlight_ctrl) {
        __this->lcd->backlight_ctrl(100);
    }

    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

    u8 *buf = NULL;
    u32 len;
    if (__this->lcd->buffer_malloc) {
        __this->lcd->buffer_malloc(&buf, &len);
    }

    lcd_buffer_init(0, buf, len / info.buf_num, info.buf_num);
}

void lcd_disp_uninit()
{
    lcd_buffer_release(0);
}


struct ui_res_info *ui_res_init()
{
    static struct ui_res_info ui_res_info_t = {0};
    int ret;

    ui_res_info_t.path = RES_PATH"ui/ui.res";

    //打开图片资源文件
    ui_res_info_t.ui_resfile = res_fopen(RES_PATH"ui/ui.res", "r");
    ASSERT(ui_res_info_t.ui_resfile);
    //获取图片资源文件flash存储信息
    ret = ui_res_flash_info_get(&ui_res_info_t.ui_resfile_info, RES_PATH"ui/ui.res", "res");
    ASSERT(!ret);

    ui_res_info_t.ui_strfile = res_fopen(RES_PATH"ui/ui.str", "r");
    ASSERT(ui_res_info_t.ui_strfile);
    ret = ui_res_flash_info_get(&ui_res_info_t.ui_strfile_info, RES_PATH"ui/ui.str", "res");
    ASSERT(!ret);

    return &ui_res_info_t;
}

void ui_res_uninit(struct ui_res_info *info)
{
    res_fclose(info->ui_resfile);
    res_fclose(info->ui_strfile);

    ui_res_flash_info_free(&info->ui_resfile_info, "res");
    ui_res_flash_info_free(&info->ui_strfile_info, "res");
}

void imb_task_blend()
{
    struct lcd_info info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }
    u16 colums = info.width;
    u16 lines = info.buffer_size / info.buf_num / info.stride;

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = info.width;
    screen_rect.height = info.height;

    struct rect rect;//当前刷新区域
    rect.left = 0;
    rect.top = 0;
    rect.width = info.width;
    rect.height = info.height;

    //imb注册输出回调(未注册默认执行推屏操作)
    /* imb_set_output_cb(imb_data_output, imb_data_output_wait_finish); */

    imb_task_head_set_buf(imb_task_head_get(0), info.buffer, info.buffer_size, info.width, info.height, info.stride, lines, info.buf_num);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
    imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, NULL);
}


#if LARGE_IMAGE_SCALE_OPTIMIZE_TEST

#include "ui/ui_effect.h"

#define API_CUSTOM_SCA_LIMIT_W		(240)
#define API_SCA_LIMIT_W		(240)


struct api_src_block_info {
    struct rect rect;
    u8 *buf;
    u32 buf_size;
};

struct api_scale_info {
    struct rect src;
    struct api_src_block_info *src_block;
    struct rect dst;
    struct rect *dst_block;
    float ratio_w;//缩放比例，必须ratio_w == ratio_h
    float ratio_h;//缩放比例，必须ratio_w == ratio_h
    int width;//背景的宽和高，一般是全屏
    int height;//背景的宽和高，一般是全屏
    int format;//输入图片的格式，只支持RGB565/RGB888
    int bytesperpixel;
    int backcolor;
    int h_block_n;
    int v_block_n;
    u8 to_argb8565;
    u8 *cut_buf[4];
    u32 cut_buf_size[4];
    int max_block_n;
};

struct api_ciner_img_info {
    int width;//输入图片的宽和高，可以不是全屏
    int height;//输入图片的宽和高，可以不是全屏
    int format;//输入图片的格式，只支持RGB565/RGB888
    u8 *img_buf;//输入图片
};

extern void *br28_malloc_psram(int size);
extern void br28_free_psram(void *buf);
extern int lcd_draw_area(u8 index, u8 *lcd_buf, int left, int top, int width, int height, int wait);
extern u8 *get_psram_obuf();
extern void free_psram_obuf();


extern struct effect_ctrl ectl;


struct api_scale_info g_sca_hdl[2];


static u32 sca_pixel_size(u32 format)
{
    const u8 bits[] = {32, 24, 16, 8, 16, 8, 8, 1, 24, 16, 0, 0};
    if (format > sizeof(bits)) {
        printf("sca format err %d\n", format);
        return 0;
    }

    return bits[format];
}


/**
 * @brief api_blk_sca : 处理分块缩放的合成函数
 *
 * @param scale[]:分块图片的数组，一个数组代表一张大图片的分块信息
 * @param page_num:图片数组的个数
 * @param base_img:底图,可以为NULL
 * @param sw_buf:NULL表示合成完联动推屏直接显示，否则其他psram的DMA地址值
 *
 * @return:0成功 其他失败
 */
int api_blk_sca(struct api_scale_info *scale[], int page_num, struct api_ciner_img_info *base_img, u8 *sw_buf)
{
    int ret;
    int i;
    int h, v;
    u8 block_index;
    struct rect cache_rect;
    struct rect screen_rect;//屏幕有效显示区域
    struct image_file tmp_file = {0};
    struct imb_task_head *new_root = imb_task_head_get(0);
    struct api_scale_info *sca = NULL;

    if ((scale == NULL) || (page_num <= 0)) {
        return -1;
    }

    new_root->ring_buf[0] = NULL;
    new_root->ring_buf[1] = NULL;
    for (i = 0; i < RING_MAX_TASK; i++) {
        if (new_root->task_tab[i]) {
            new_root->task_tab[i] = NULL;
        }
    }

    if (sw_buf) {
        new_root->copy_to_psram = 3;
    } else {
        new_root->copy_to_psram = 0;
    }
    new_root->state = STATE_SYNTHESIS_AND_DRAW;
    new_root->new_copy = 0;

    //创建背景任务(必须)
    imb_create_bg_task(new_root->format, new_root->screen_width, new_root->screen_height, 0x0);

    if (base_img) {
        tmp_file.width = base_img->width;
        tmp_file.height = base_img->height;
        tmp_file.format = base_img->format;
        imb_create_image_task(imb_task_get_id(), &tmp_file, base_img->img_buf, 0, 0, base_img->width, base_img->height, NULL, false, LOAD_FROM_STATIC_RAM);
    }

    i = 0;
    do {
        sca = scale[i];

        for (v = 0; v < sca->v_block_n; v++) {
            for (h = 0; h < sca->h_block_n; h++) {
                block_index = v * sca->h_block_n + h;
                tmp_file.width = sca->src_block[block_index].rect.width;
                tmp_file.height = sca->src_block[block_index].rect.height;
                tmp_file.format = sca->format;
                imb_create_image_scale_task(imb_task_get_id(), &tmp_file, psram_cache2nocache_addr(sca->src_block[block_index].buf), sca->ratio_w, sca->ratio_h, sca->dst_block[block_index].left, sca->dst_block[block_index].top, sca->dst_block[block_index].width, sca->dst_block[block_index].height, NULL, false, LOAD_FROM_STATIC_RAM);
            }
        }

        i++;
    } while (--page_num);

    printf("start meld\n");
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = new_root->screen_width;
    screen_rect.height = new_root->screen_height;

    /* imb_task_info_dump(new_root); */

    /* lcd_wait(); */
    if (sw_buf) {
        lcd_wait();
        ectl.ps_obuf = sw_buf;
    }
    imb_start(new_root, &screen_rect, cache_rect, new_root->screen_width, new_root->lines, NULL);

    new_root->copy_to_psram = 0;

    if (sw_buf) {
        int wait = 0x20;//推单独的一屏
        lcd_draw_area(0, ectl.sca_obuf, 0, 0, new_root->screen_width, new_root->screen_height, wait);//推屏
        ectl.ps_obuf = NULL;
        ectl.sca_obuf = NULL;
    }

    imb_task_all_destroy(new_root);

    return 0;
}




/**
 * @brief api_get_blk_sca_info:计算输入输出的缩放分块信息
 *
 *
 * @param sca:输出参数，由该函数内部初始化,必须静态或者全局
 * @param width:背景的宽和高，一般是全屏
 * @param height:背景的宽和高，一般是全屏
 * @param ratio_w:缩放比例，必须ratio_w == ratio_h
 * @param ratio_h:缩放比例，必须ratio_w == ratio_h
 * @param x_offset:居中缩放之后的水平偏移
 * @param y_offset:居中缩放之后的垂直偏移
 * @param limit_h:该值必须等于屏分块的高度
 * @param sca_img:输入图片的信息
 * @param status: 0:表示初始状态，会将图片分割成多个小图片，并计算输入分块信息，其他:表示只计算输出分块信息
 *
 * @return:返回初始化之后的sca句柄
 */
struct api_scale_info *api_get_blk_sca_info(struct api_scale_info *sca, int width, int height, float ratio_w, float ratio_h, int x_offset, int y_offset, int limit_h, struct api_ciner_img_info *sca_img, int status)
{
    sca->width = width;
    sca->height = height;
    sca->format = sca_img->format;
    sca->bytesperpixel = sca_pixel_size(sca_img->format) / 8;
    sca->backcolor = 0xffff;

    sca->src.left = 0;
    sca->src.top = 0;
    sca->src.width = sca_img->width;
    sca->src.height = sca_img->height;

    sca->dst.width = sca->src.width * ratio_w;
    sca->dst.height = sca->src.height * ratio_h;
    sca->dst.left = (width - sca->dst.width) / 2 + x_offset;
    sca->dst.top = (height - sca->dst.height) / 2 + y_offset;

    printf("dst_left %d, dst_top %d\n", sca->dst.left, sca->dst.top);

    sca->ratio_w = ratio_w;
    sca->ratio_h = ratio_h;

    if (sca->src.height <= 240) {
        limit_h = sca->src.height;
    }
    //最后一块的高度不能太小(例如1就太极端)，否则合成超时
    int res = sca->src.height - (sca->src.height / limit_h) * limit_h;
    float ratio_tmp = ratio_h * res;
    if (ratio_tmp && (ratio_tmp < 1)) {//微调
        printf("res too small %d\n", res);
        limit_h += 1;
    }

    printf("new_h %d, ratio %f\n", limit_h, ratio_h);

    int h_block_n = (sca->src.width + API_CUSTOM_SCA_LIMIT_W - 1) / API_CUSTOM_SCA_LIMIT_W;
    int v_block_n = (sca->src.height + limit_h - 1) / limit_h;
    int h, v;
    u8 block_index;
    int start;

    sca->h_block_n = h_block_n;
    sca->v_block_n = v_block_n;
    //status: 0:
    if (status == 0) {

        int kstart = jiffies_msec();

        sca->src_block = (struct api_src_block_info *)imb_malloc(h_block_n * v_block_n * sizeof(struct api_src_block_info), true);
        if (!sca->src_block) {
            return -1;
        }
        sca->dst_block = (struct rect *)imb_malloc(h_block_n * v_block_n * sizeof(struct rect), true);
        if (!sca->dst_block) {
            return -1;
        }

        int h_block_len = sca->src.width / h_block_n;
        int v_block_len = limit_h;

        img_set_input(DATA_SRC_SRAM, DATA_NOZIP, sca->src.width, sca->src.height, sca_img->format, NULL, psram_cache2nocache_addr(sca_img->img_buf));

        /* printf("%d, %d, %d, %d\n", h_block_n, v_block_n, h_block_len, v_block_len); */

        //计算源图片分块参数
        for (v = 0; v < v_block_n; v++) {
            for (h = 0; h < h_block_n; h++) {
                block_index = v * h_block_n + h;
                sca->src_block[block_index].rect.left = h * h_block_len;
                sca->src_block[block_index].rect.top = v * v_block_len;
                if (h == (h_block_n - 1)) {
                    sca->src_block[block_index].rect.width = sca->src.width - sca->src_block[block_index].rect.left;
                } else {
                    sca->src_block[block_index].rect.width = h_block_len;
                }
                if (v == (v_block_n - 1)) {
                    sca->src_block[block_index].rect.height = sca->src.height - sca->src_block[block_index].rect.top;
                } else {
                    sca->src_block[block_index].rect.height = v_block_len;
                }

                sca->src_block[block_index].buf_size = ((sca->src_block[block_index].rect.width * sca->bytesperpixel + 3) / 4 * 4) * sca->src_block[block_index].rect.height;
                sca->src_block[block_index].buf = (u8 *)br28_malloc_psram(sca->src_block[block_index].buf_size);

                /* printf("%d, %d, %d, %d, %d, %x\n", block_index, sca->src_block[block_index].rect.width, sca->src_block[block_index].rect.height, sca->bytesperpixel, sca->src_block[block_index].buf_size, (u32)sca->src_block[block_index].buf);	 */
                ASSERT(sca->src_block[block_index].buf, "src_block malloc %d fail, %d\n", sca->src_block[block_index].buf_size, block_index);

                flushinv_dcache(sca->src_block[block_index].buf, sca->src_block[block_index].buf_size);
                img_set_output(psram_cache2nocache_addr(sca->src_block[block_index].buf), sca->src_block[block_index].rect.left, sca->src_block[block_index].rect.top, sca->src_block[block_index].rect.width, sca->src_block[block_index].rect.height);
                img_kick_sync();

            }
        }

        printf("img_cut time : %d ms\n", jiffies_msec() - kstart);
    }


    //计算目标缩放分块参数
    int h_offset = 0, v_offset = 0;
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            sca->dst_block[block_index].left = sca->dst.left + h_offset;
            sca->dst_block[block_index].top = sca->dst.top + v_offset;
            sca->dst_block[block_index].width = sca->src_block[block_index].rect.width * ratio_w;
            sca->dst_block[block_index].height = sca->src_block[block_index].rect.height * ratio_h;

            h_offset += sca->dst_block[block_index].width;

            /* printf("index %d, %d, %d, %d, %d\n", block_index, sca->dst_block[block_index].left, sca->dst_block[block_index].top, sca->dst_block[block_index].width, sca->dst_block[block_index].height); */

        }
        h_offset = 0;
        v_offset += sca->dst_block[block_index].height;
    }

    return sca;
}

void api_free_blk_sca_info(struct api_scale_info *sca)
{
    int h, v;
    u8 block_index;
    int h_block_n = sca->h_block_n;
    int v_block_n = sca->v_block_n;

    if (sca->src_block == NULL) {
        return;
    }

    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            if (sca->src_block[block_index].buf) {
                br28_free_psram(sca->src_block[block_index].buf);
                sca->src_block[block_index].buf = NULL;
            }
        }
    }

    if (sca->src_block) {
        imb_free(sca->src_block);
        sca->src_block = NULL;
    }
    if (sca->dst_block) {
        imb_free(sca->dst_block);
        sca->dst_block = NULL;
    }
}

/**
 * @brief api_get_blk_sca_info2:计算输入输出的缩放分块信息
 * 该函数与api_get_blk_sca_info()的区别是，根据不同的缩放
 * 比例调整不同的分块块数，目的的减轻移动过程中的抖动，但
 * 不同的分块块数会导致需要重复的申请和释放内存，因此一般
 * 是在连续移动时调用该函数，例如左右滑屏，在不移动时还是
 * 调用api_get_blk_sca_info()好
 *
 *
 * @param sca:输出参数，由该函数内部初始化,必须静态或者全局
 * @param width:背景的宽和高，一般是全屏
 * @param height:背景的宽和高，一般是全屏
 * @param ratio_w:缩放比例，必须ratio_w == ratio_h
 * @param ratio_h:缩放比例，必须ratio_w == ratio_h
 * @param x_offset:居中缩放之后的水平偏移
 * @param y_offset:居中缩放之后的垂直偏移
 * @param limit_h:该值必须等于屏分块的高度
 * @param sca_img:输入图片的信息
 * @param status: 0:表示初始状态，会将图片分割成多个小图片，并计算输入分块信息，其他:表示只计算输出分块信息
 *
 * @return:返回初始化之后的sca句柄
 */
struct api_scale_info *api_get_blk_sca_info2(struct api_scale_info *sca, int width, int height, float ratio_w, float ratio_h, int x_offset, int y_offset, int limit_h, struct api_ciner_img_info *sca_img, int status)
{
    sca->width = width;
    sca->height = height;
    sca->format = sca_img->format;
    sca->bytesperpixel = sca_pixel_size(sca_img->format) / 8;
    sca->backcolor = 0xffff;

    sca->src.left = 0;
    sca->src.top = 0;
    sca->src.width = sca_img->width;
    sca->src.height = sca_img->height;

    if (sca->src.height <= 240) {
        limit_h = sca->src.height;
    }
    ASSERT(ratio_w == ratio_h);
    float ratio_tmp;
    int limit_h_tmp;
    int res;

    /* 计算输出分块高度对应的输入分块高度 */
    ratio_tmp = (float)limit_h / ratio_h;
    limit_h_tmp = (int)ratio_tmp;//取整

    //最后一块的高度不能太小(例如1就太极端)，否则合成超时
    res = sca->src.height - (sca->src.height / limit_h_tmp) * limit_h_tmp;//最后一个分块的高度
    ratio_tmp = ratio_h * res;//最后一个分块缩放后的高度
    if (ratio_tmp && (ratio_tmp < 1)) {//微调
        printf("res too small %d\n", res);
        limit_h_tmp += 1;
    }

    ratio_h = (float)limit_h / (float)limit_h_tmp;
    ratio_w = ratio_h;
    limit_h = limit_h_tmp;

    printf("new_h %d, ratio_h %f\n", limit_h, ratio_h);

    sca->dst.width = sca->src.width * ratio_w;
    sca->dst.height = sca->src.height * ratio_h;
    sca->dst.left = (width - sca->dst.width) / 2 + x_offset;
    sca->dst.top = (height - sca->dst.height) / 2 + y_offset;

    printf("dst_left %d, dst_top %d\n", sca->dst.left, sca->dst.top);

    sca->ratio_w = ratio_w;
    sca->ratio_h = ratio_h;

    int h_block_n = (sca->src.width + API_SCA_LIMIT_W - 1) / API_SCA_LIMIT_W;//水平分块数量
    int v_block_n = (sca->src.height + limit_h - 1) / limit_h;//垂直分块数量
    int h, v;
    u8 block_index;
    int start;

    /* printf("v_block_n %d\n", v_block_n);	 */

    sca->h_block_n = h_block_n;
    sca->v_block_n = v_block_n;

    int h_block_len = sca->src.width / h_block_n;
    int v_block_len = limit_h;

    if (status == 0) {
        int kstart = jiffies_msec();

        img_set_input(DATA_SRC_SRAM, DATA_NOZIP, sca->src.width, sca->src.height, sca_img->format, NULL, psram_cache2nocache_addr(sca_img->img_buf));

        int cut_w, cut_left;
        for (h = 0; h < h_block_n; h++) {
            cut_left = h * h_block_len;
            if (h == (h_block_n - 1)) {
                cut_w = sca->src.width - cut_left;
            } else {
                cut_w = h_block_len;
            }
            sca->cut_buf_size[h] = ((cut_w * sca->bytesperpixel + 3) / 4 * 4) * sca->src.height;
            sca->cut_buf[h] = (u8 *)br28_malloc_psram(sca->cut_buf_size[h]);
            ASSERT(sca->cut_buf[h], "src_block malloc %d fail, %d\n", sca->cut_buf_size[h], h);

            flushinv_dcache(sca->cut_buf[h], sca->cut_buf_size[h]);
            img_set_output(psram_cache2nocache_addr(sca->cut_buf[h]), cut_left, 0, cut_w, sca->src.height);
            img_kick_sync();
        }

        printf("img_cut time : %d ms\n", jiffies_msec() - kstart);
    }


    if (sca->max_block_n < h_block_n * v_block_n) {
        if (sca->src_block) {
            imb_free(sca->src_block);
            sca->src_block = NULL;
        }
        if (sca->dst_block) {
            imb_free(sca->dst_block);
            sca->dst_block = NULL;
        }
        sca->max_block_n = h_block_n * v_block_n;
    }
    if (sca->src_block == NULL) {
        sca->src_block = (struct api_src_block_info *)imb_malloc(h_block_n * v_block_n * sizeof(struct api_src_block_info), true);
        if (!sca->src_block) {
            return -1;
        }
    }
    if (sca->dst_block == NULL) {
        sca->dst_block = (struct rect *)imb_malloc(h_block_n * v_block_n * sizeof(struct rect), true);
        if (!sca->dst_block) {
            return -1;
        }
    }

    /* printf("%d, %d, %d, %d\n", h_block_n, v_block_n, h_block_len, v_block_len); */


    int offset[4] = {0};
    //计算源图片分块参数
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            sca->src_block[block_index].rect.left = h * h_block_len;
            sca->src_block[block_index].rect.top = v * v_block_len;
            if (h == (h_block_n - 1)) {
                sca->src_block[block_index].rect.width = sca->src.width - sca->src_block[block_index].rect.left;
            } else {
                sca->src_block[block_index].rect.width = h_block_len;
            }
            if (v == (v_block_n - 1)) {
                sca->src_block[block_index].rect.height = sca->src.height - sca->src_block[block_index].rect.top;
            } else {
                sca->src_block[block_index].rect.height = v_block_len;
            }

            sca->src_block[block_index].buf_size = ((sca->src_block[block_index].rect.width * sca->bytesperpixel + 3) / 4 * 4) * sca->src_block[block_index].rect.height;
            sca->src_block[block_index].buf = sca->cut_buf[h] + offset[h];

            ASSERT(((u32)sca->src_block[block_index].buf + sca->src_block[block_index].buf_size) <= ((u32)sca->cut_buf[h] + sca->cut_buf_size[h]), "src_small_block malloc %d fail, %d, %x, %x, %x\n", sca->src_block[block_index].buf_size, block_index, (u32)sca->src_block[block_index].buf, (u32)sca->cut_buf[h], sca->cut_buf_size[h]);
            offset[h] += sca->src_block[block_index].buf_size;
        }
    }


    //计算目标缩放分块参数
    int h_offset = 0, v_offset = 0;
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            sca->dst_block[block_index].left = sca->dst.left + h_offset;
            sca->dst_block[block_index].top = sca->dst.top + v_offset;
            sca->dst_block[block_index].width = sca->src_block[block_index].rect.width * ratio_w;
            sca->dst_block[block_index].height = sca->src_block[block_index].rect.height * ratio_h;

            h_offset += sca->dst_block[block_index].width;
        }
        h_offset = 0;
        v_offset += sca->dst_block[block_index].height;
    }

    return sca;
}


void api_free_blk_sca_info2(struct api_scale_info *sca)
{
    int h, v;
    u8 block_index;
    int h_block_n = sca->h_block_n;
    int v_block_n = sca->v_block_n;

    if (sca->src_block == NULL) {
        return;
    }

    for (h = 0; h < h_block_n; h++) {
        if (sca->cut_buf[h]) {
            br28_free_psram(sca->cut_buf[h]);
            sca->cut_buf[h] = NULL;
        }
    }

    if (sca->src_block) {
        imb_free(sca->src_block);
        sca->src_block = NULL;
    }
    if (sca->dst_block) {
        imb_free(sca->dst_block);
        sca->dst_block = NULL;
    }
    sca->max_block_n = 0;
}


void imb_large_image_scale_optimize_test(struct ui_res_info *info)
{
    struct image_file file;
    int ret;
    int i;
    int pixel_size;
    int id;
    u16 lines;
    struct lcd_info lcd_info = {0};

    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&lcd_info);
    }

    lines = lcd_info.buffer_size / lcd_info.buf_num / lcd_info.stride;//计算合成输出分块的高度，实际输入分块的高度由该参数计算出来

    //宽或者高超过240的图片缩放,注意，这种方式一般只适用RGB565/RGB888,并且必须在psram使能的情况下才能用


    struct rect api_rect;//当前刷新区域
    struct rect api_screen_rect;//屏幕有效显示区域
    struct rect api_page_draw = {0};

    //预先配置好root的基础信息
    api_rect.left = 0;
    api_rect.top = 0;
    api_rect.width = lcd_info.width;
    api_rect.height = lcd_info.height;
    memcpy(&api_screen_rect, &api_rect, sizeof(struct rect));
    imb_task_head_set_buf(imb_task_head_get(0), lcd_info.buffer, lcd_info.buffer_size, lcd_info.width, lcd_info.height, lcd_info.stride, lines, lcd_info.buf_num);
    imb_task_head_config(imb_task_head_get(0), &api_rect, &api_screen_rect, &api_screen_rect, &api_page_draw, STATE_SYNTHESIS_AND_DRAW);
    struct imb_task_head *new_root = imb_task_head_get(0);
    new_root->format = lcd_info.color_format;


    //先将flash的图片压缩数据解压到到psram(这里只是为了方便测试，做缩放特效时实际的图片数据来源于页面的合成数据)
    //客户也可以通过其他方式将图片存放到psram
    //注意：要确保图片存放到psram的DMA地址，而不是psram的cache地址!!!下面的示例是一直存放到psram的DMA地址(通过psram_cache2nocache_addr()将psram的cache地址转成DMA地址)
    u8 *palette = NULL;
    u8 *pixel = NULL;
    id = PAGE0_38d6_SCA1;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);

    struct rect crop = {0};//截取区域,不能超出原图区域
    crop.left = 0;
    crop.top = 0;
    crop.width = file.width;
    crop.height = file.height;

    u16 palette_size = image_palette_size(file.format);
    if (palette_size) {
        palette = imb_malloc(palette_size, false);
        image_read_palette(&file, &info->ui_resfile_info, palette);
    }

    pixel_size = image_pixel_size(crop.width, crop.height, file.format);
    if (pixel_size != -1) {
        pixel = (u8 *)br28_malloc_psram(pixel_size);
        ASSERT(pixel);
        image_read_pixel(&file, &info->ui_resfile_info, psram_cache2nocache_addr(pixel), crop.left, crop.top, crop.width, crop.height);
    }

    /* int wait = 0x20;//推单独的一屏 */
    /* lcd_draw_area(0, psram_cache2nocache_addr(pixel), 0, 0, crop.width, crop.height, wait);//推屏 */
    /* while(1); */


    u8 *palette2 = NULL;
    u8 *pixel2 = NULL;
    struct rect crop2 = {0};//截取区域,不能超出原图区域
    id = PAGE0_8919_SCA2;
    ret = open_image_by_id(0, info->ui_resfile, &file, id & 0xff, id >> 16);
    ASSERT(!ret);
    crop2.left = 0;
    crop2.top = 0;
    crop2.width = file.width;
    crop2.height = file.height;

    palette_size = image_palette_size(file.format);
    if (palette_size) {
        palette2 = imb_malloc(palette_size, false);
        image_read_palette(&file, &info->ui_resfile_info, palette2);
    }
    pixel_size = image_pixel_size(crop2.width, crop2.height, file.format);
    if (pixel_size != -1) {
        pixel2 = (u8 *)br28_malloc_psram(pixel_size);
        ASSERT(pixel2);
        image_read_pixel(&file, &info->ui_resfile_info, psram_cache2nocache_addr(pixel2),  crop2.left, crop2.top, crop2.width, crop2.height);
    }

    /* int wait = 0x20;//推单独的一屏 */
    /* lcd_draw_area(0, psram_cache2nocache_addr(pixel2), 0, 0, crop2.width, crop2.height, wait);//推屏 */
    /* while(1); */



    //缩放示例
    struct api_scale_info *sca[2] = {0};
    struct api_ciner_img_info sca_img = {0};
    int x_offset = 0;
    int y_offset = 0;
    int cnt = 0;
    int status = 0;
    float ratio;
    float ratio_tab[] = {0.52, 0.83, 1.0, 1.3, 2.0};
    int kstart = jiffies_msec();
    u8 *obuf = NULL;

#if (SCALE_EXAMPLE == SCALE_EXAMPLE1)
    /*********************example1***********************/
    printf("\n\nsca example1\n\n");
    sca_img.width = crop.width;
    sca_img.height = crop.height;
    sca_img.format = LAYER_FORMAT_RGB565;
    sca_img.img_buf = pixel;
    x_offset = 0;
    y_offset = 0;

    while (1) {
        for (cnt = 0; cnt < 5; cnt++) {
            ratio = ratio_tab[cnt];
            status = (cnt == 0) ? 0 : 1;//对同一张图片多次缩放时要注意，0是第一次，第一次会将原图分块裁剪成多个小块,因此多了裁剪的时间

            kstart = jiffies_msec();

            sca[0] = api_get_blk_sca_info(&g_sca_hdl[0], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);

            api_blk_sca(sca, 1, NULL, NULL);

            printf("sca&disp time : %d ms\n", jiffies_msec() - kstart);//第一次的时间是加上了一次裁剪的时间
        }
        os_time_dly(30);

        for (cnt = 4; cnt >= 0; cnt--) {
            ratio = ratio_tab[cnt];
            status = 1;//对同一张图片多次缩放时要注意，0是第一次，第一次会将原图分块裁剪成多个小块,因此多了裁剪的时间

            kstart = jiffies_msec();

            sca[0] = api_get_blk_sca_info(&g_sca_hdl[0], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);

            api_blk_sca(sca, 1, NULL, NULL);

            printf("sca&disp time : %d ms\n", jiffies_msec() - kstart);//第一次的时间是加上了一次裁剪的时间
        }
        os_time_dly(30);

        if (sca[0]) {
            api_free_blk_sca_info(sca[0]);
            sca[0] = NULL;
        }
    }

#endif


#if (SCALE_EXAMPLE == SCALE_EXAMPLE2)
    /*********************example2***********************/
    printf("\n\nsca example2\n\n");
    //如果两张输入的图片，缩放之后的位置有重叠，
    //会相应的增加合成的时间

    kstart = jiffies_msec();

    sca_img.width = crop.width;
    sca_img.height = crop.height;
    sca_img.format = LAYER_FORMAT_RGB565;
    sca_img.img_buf = pixel;
    x_offset = -200;
    y_offset = 0;
    status = 0;
    ratio = 0.92;
    sca[0] = api_get_blk_sca_info(&g_sca_hdl[0], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);

    sca_img.width = crop2.width;
    sca_img.height = crop2.height;
    sca_img.format = LAYER_FORMAT_RGB565;
    sca_img.img_buf = pixel2;
    x_offset = 250;
    y_offset = 0;
    status = 0;
    ratio = 0.83;
    sca[1] = api_get_blk_sca_info(&g_sca_hdl[1], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);

    //申请一块输出buf,合成时拷贝到这块buf里面，然后
    //整帧推屏这块buf,防止带宽不足时推屏切线
    obuf = psram_cache2nocache_addr(get_psram_obuf());
    api_blk_sca(sca, 2, NULL, obuf);

    printf("sca&disp time : %d ms\n", jiffies_msec() - kstart);//第一次的时间是加上了两次裁剪的时间
    if (sca[0]) {
        api_free_blk_sca_info(sca[0]);
        sca[0] = NULL;
    }
    if (sca[1]) {
        api_free_blk_sca_info(sca[1]);
        sca[1] = NULL;
    }
    if (obuf) {
        free_psram_obuf();
        obuf = NULL;
    }
#endif


#if (SCALE_EXAMPLE == SCALE_EXAMPLE3)
    /*********************example3***********************/
    printf("\n\nsca example3\n\n");
    int x, tmp_x, frozen_x;
    float start_scale = 0.6;

    frozen_x = lcd_info.width / 2;
    while (1) {
        x = 0;
        do {
            status = (x == 0) ? 0 : 1;//对同一张图片多次缩放时要注意，0是第一次，第一次会将原图分块裁剪成多个小块,因此多了裁剪的时间

            tmp_x = lcd_info.width - x;
            if (tmp_x < frozen_x) {
                ratio = start_scale;
            } else {
                ratio = start_scale + (float)(tmp_x - frozen_x) * (1.0f - start_scale) / (float)(lcd_info.width - frozen_x);
            }
            /* printf("offset %d, %f\n", tmp_x, ratio); */
            sca_img.width = crop.width;
            sca_img.height = crop.height;
            sca_img.format = LAYER_FORMAT_RGB565;
            sca_img.img_buf = pixel;
            x_offset = -x;
            y_offset = 0;
            sca[0] = api_get_blk_sca_info2(&g_sca_hdl[0], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);


            if (x < frozen_x) {
                ratio = start_scale;
            } else {
                ratio = start_scale + (float)(x - frozen_x) * (1.0f - start_scale) / (float)(lcd_info.width - frozen_x);
            }
            /* printf("offset %d, %f\n", x, ratio); */
            sca_img.width = crop2.width;
            sca_img.height = crop2.height;
            sca_img.format = LAYER_FORMAT_RGB565;
            sca_img.img_buf = pixel2;
            x_offset = (lcd_info.width - x);
            y_offset = 0;
            sca[1] = api_get_blk_sca_info2(&g_sca_hdl[1], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);

            //申请一块输出buf,合成时拷贝到这块buf里面，然后
            //整帧推屏这块buf,防止带宽不足时推屏切线
            obuf = psram_cache2nocache_addr(get_psram_obuf());
            api_blk_sca(sca, 2, NULL, obuf);

            //os_time_dly(10);//这里延时只是为了看清楚滑动过程

            x += 20;
        } while (x <= lcd_info.width);

        if (sca[0]) {
            api_free_blk_sca_info2(sca[0]);
            sca[0] = NULL;
        }
        if (sca[1]) {
            api_free_blk_sca_info2(sca[1]);
            sca[1] = NULL;
        }

        os_time_dly(100);

        x = lcd_info.width;
        do {
            status = (x == lcd_info.width) ? 0 : 1;//对同一张图片多次缩放时要注意，0是第一次，第一次会将原图分块裁剪成多个小块,因此多了裁剪的时间

            tmp_x = lcd_info.width - x;
            if (tmp_x < frozen_x) {
                ratio = start_scale;
            } else {
                ratio = start_scale + (float)(tmp_x - frozen_x) * (1.0f - start_scale) / (float)(lcd_info.width - frozen_x);
            }
            /* printf("offset %d, %f\n", tmp_x, ratio); */
            sca_img.width = crop.width;
            sca_img.height = crop.height;
            sca_img.format = LAYER_FORMAT_RGB565;
            sca_img.img_buf = pixel;
            x_offset = -x;
            y_offset = 0;
            sca[0] = api_get_blk_sca_info2(&g_sca_hdl[0], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);


            if (x < frozen_x) {
                ratio = start_scale;
            } else {
                ratio = start_scale + (float)(x - frozen_x) * (1.0f - start_scale) / (float)(lcd_info.width - frozen_x);
            }
            /* printf("offset %d, %f\n", x, ratio); */
            sca_img.width = crop2.width;
            sca_img.height = crop2.height;
            sca_img.format = LAYER_FORMAT_RGB565;
            sca_img.img_buf = pixel2;
            x_offset = (lcd_info.width - x);
            y_offset = 0;
            sca[1] = api_get_blk_sca_info2(&g_sca_hdl[1], lcd_info.width, lcd_info.height, ratio, ratio, x_offset, y_offset, lines, &sca_img, status);

            //申请一块输出buf,合成时拷贝到这块buf里面，然后
            //整帧推屏这块buf,防止带宽不足时推屏切线
            obuf = psram_cache2nocache_addr(get_psram_obuf());
            api_blk_sca(sca, 2, NULL, obuf);

            //os_time_dly(10);//这里延时只是为了看清楚滑动过程

            x -= 20;
        } while (x >= 0);

        if (sca[0]) {
            api_free_blk_sca_info2(sca[0]);
            sca[0] = NULL;
        }
        if (sca[1]) {
            api_free_blk_sca_info2(sca[1]);
            sca[1] = NULL;
        }

        os_time_dly(100);
    }

    if (obuf) {
        free_psram_obuf();
        obuf = NULL;
    }


#endif

    if (palette) {
        imb_free(palette);
        palette = NULL;
    }
    if (pixel) {
        br28_free_psram(pixel);
        pixel = NULL;
    }
    if (palette2) {
        imb_free(palette2);
        palette2 = NULL;
    }
    if (pixel2) {
        br28_free_psram(pixel2);
        pixel2 = NULL;
    }
}


#endif



static void imb_task(void *arg)
{
    int msg[32];
    int ret;

    printf("imb_task.\n");

    extern char *ui_lib_version();
    printf("lib ver : %s\n", ui_lib_version());

    struct ui_res_info *res = ui_res_init();

    //屏初始化
    lcd_disp_init(arg);
    struct lcd_info info = {0};

#if LARGE_IMAGE_SCALE_OPTIMIZE_TEST
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    imb_large_image_scale_optimize_test(res);
#else
#error "TCFG_PSRAM_DEV_ENABLE Need Enable"
#endif
#elif LARGE_IMAGE_SCALE_TEST
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    imb_large_image_scale_test(res);
#else
#error "TCFG_PSRAM_DEV_ENABLE Need Enable"
#endif
#elif IMB_MIXED_TEST
    imb_task_mixed_with_extern_background();
#elif IMB_WATCH_TEST
    imb_watch_test(res);
#else

    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }
    //创建背景任务(必须且需要在其他task之前创建)
    imb_create_bg_task(info.color_format, info.width, info.height, 0xFFFF);

#if IMAGE_DECOMPRESS_TEST
    imb_image_decompress_test(res);
#endif

#if IMAGE_TEST
    imb_image_test(res);
#endif

#if IMAGE_SCALE_TEST
    imb_image_scale_test(res);
#endif


#if IMAGE_ROTATE_CHUNK_TEST
    imb_image_rotate_chunk_test(res);
#endif

#if IMAGE_ROTATE_TEST
    imb_image_rotate_test(res);
#endif

#if STRING_TEST
    imb_string_test(res);
#endif

#if FONT_TEST
    imb_font_test();
#endif

#if CUSTOM_DRAW_TEST
    imb_custom_draw_test();
#endif

#if COLOR_TEST
    imb_color_test();
#endif

#if JPEG_TEST
    imb_jpeg_test(res);
#endif

    //打印所有任务信息
    imb_task_info_dump(imb_task_head_get(0));

    imb_task_blend();

    imb_task_all_destroy(imb_task_head_get(0));
#endif

    ui_res_uninit(res);

    lcd_disp_uninit();

    while (1) {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg)); //500ms_reflash
        if (ret != OS_TASKQ) {
            continue;
        }
    }
}





/*********************************imb api begin**********************************************/


struct imb_api_sca_info {
    int id;//旋转图片的ID
    struct image_file *file;//旋转的图片文件信息

    //图像从sram取数时，将首地址赋该值
    //注意：这时候每行字节数需4字节对齐,例如50X60的ARGB8565格式的图片，他一行实际的字节数是
    //50*3=150，150不是4字节对齐，所以需要补齐一行的空间为152，所以图片所需的空间大小为
    //((50*3 + 3) / 4 * 4) * 60 = 152 * 60 = 9120
    u8 *buf;//起始地址也需要4字节对齐

    float ratio_w;//水平放大倍率
    float ratio_h;//垂直放大倍率
    int x;//相对背景的x坐标
    int y;//相对背景的y坐标
    int width;//显示的图片宽度（放大后的宽度）
    int height;//显示的图片高度（放大后的高度）
};


struct imb_api_cut_info {
    int id;//旋转图片的ID

    struct image_file *file;//旋转的图片文件信息

    //图像从sram取数时，将首地址赋该值
    //注意：这时候每行字节数需4字节对齐,例如50X60的ARGB8565格式的图片，他一行实际的字节数是
    //50*3=150，150不是4字节对齐，所以需要补齐一行的空间为152，所以图片所需的空间大小为
    //((50*3 + 3) / 4 * 4) * 60 = 152 * 60 = 9120
    u8 *buf;//起始地址也需要4字节对齐

    int x;//相对背景的x坐标
    int y;//相对背景的y坐标
    int width;//图片显示的宽度，必须小于或者等于图片的实际宽度，小于实际宽度则截取显示
    int height;//图片显示的高度，必须小于或者等于图片的实际高度，小于实际高度则截取显示

    struct rect *crop;//可裁剪图片任意区域
};

struct imb_api_rotate_info {
    int id;//旋转图片的ID
    struct image_file *file;//旋转的图片文件信息

    //图像从sram取数时，将首地址赋该值
    //注意：这时候每行字节数需4字节对齐,例如50X60的ARGB8565格式的图片，他一行实际的字节数是
    //50*3=150，150不是4字节对齐，所以需要补齐一行的空间为152，所以图片所需的空间大小为
    //((50*3 + 3) / 4 * 4) * 60 = 152 * 60 = 9120
    u8 *buf;//起始地址也需要4字节对齐

    int rotate_chunk;//是否分块输入方式来旋转
    int cx;//图片的旋转中心x坐标，相对于图片
    int cy;//图片的旋转中心y坐标，相对于图片
    int angle;//旋转角度
    int dx;//输出的旋转中心点x坐标，相对于背景
    int dy;//输出的旋转中心点y坐标，相对于背景
};

struct imb_api_info {
    u16 width; //背景的宽，相当于合成输出的总体宽度
    u16 height;//背景的高，相当于合成输出的总体高度
    u16 stride;//合成输出BUF一行的字节数，4字节对齐
    u8 color_format;//合成输出格式，只支持RGB565/RGB888
    u8 buf_num;//合成输出的buf数，buf管理模块内部的buf数
    u8 *buf; //合成输出的BUF首地址
    u32 len;///合成输出的BUF总体长度
    u16 lines;//当小于一帧的行数时，表示分块合成输出
    u16 colums;//只支持等于背景宽度的值
    u16 backcolor;//背景颜色

    struct imb_cb_interface itf;//合成输出的接口，用于合成内部获取输出buf的相关管理函数，以及合成后调用类似推屏的接口
};


static struct flash_file_info g_ui_resfile_info;
static struct flash_file_info g_ui_strfile_info;
static RESFILE *g_ui_resfile = NULL;
static RESFILE *g_ui_strfile = NULL;

int imb_api_init()
{
    int ret = 0;

    //打开图片资源文件
    g_ui_resfile = res_fopen(RES_PATH"ui/ui.res", "r");
    ASSERT(g_ui_resfile);
    //获取图片资源文件flash存储信息
    ret = ui_res_flash_info_get(&g_ui_resfile_info, RES_PATH"ui/ui.res", "res");
    ASSERT(!ret);

    g_ui_strfile = res_fopen(RES_PATH"ui/ui.str", "r");
    ASSERT(g_ui_strfile);
    ret = ui_res_flash_info_get(&g_ui_strfile_info, RES_PATH"ui/ui.str", "res");
    ASSERT(!ret);

    return 0;
}

int imb_api_uninit()
{
    imb_task_all_destroy(imb_task_head_get(0));
    if (g_ui_resfile) {
        res_fclose(g_ui_resfile);
        g_ui_resfile = NULL;
        ui_res_flash_info_free(&g_ui_resfile_info, "res");
    }

    if (g_ui_strfile) {
        res_fclose(g_ui_strfile);
        g_ui_strfile = NULL;
        ui_res_flash_info_free(&g_ui_strfile_info, "res");
    }

    return 0;
}



/**
 * @brief imb_api_del 删除合成的所有任务，包括背景任务
 *
 * @param info
 *
 * @return
 */
int imb_api_del(struct imb_api_info *info)
{
    imb_task_all_destroy(imb_task_head_get(0));

    return 0;
}



/**
 * @brief imb_api_get_finfo 配合裁剪、旋转、缩放接口使用，用于获取图片信息
 *
 * @param id 图片ID
 * @param file 存储图片信息的句柄
 *
 * @return 0:配置成功 其他:失败
 */
int imb_api_get_finfo(int id, struct image_file *file)
{
    int ret = open_image_by_id(0, g_ui_resfile, file, id & 0xff, id >> 16);
    ASSERT(!ret);

    if (!ret) {
        return -1;
    }

    return 0;
}


/**
 * @brief imb_api_blend 启动合成接口，可配置多个图片之后再启动
 *
 * @param info 合成的总体信息，具体看结构体注释
 *
 * @return 0:配置成功 其他:失败
 */
int imb_api_blend(struct imb_api_info *info)
{
    struct imb_cb_interface *itf = &info->itf;

    if ((!itf->buf_get) || (!itf->buf_set_pending) ||
        (!itf->draw_area) || (!itf->buf_check) ||
        (!itf->buf_set_idle) || (!itf->buf_wait_finish)) {
        return -1;
    }

    //打印所有任务信息
    /* imb_task_info_dump(imb_task_head_get(0)); */

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = info->width;
    screen_rect.height = info->height;

    struct rect rect;//当前刷新区域(可局部刷新，也可全屏刷新)
    rect.left = 0;
    rect.top = 0;
    rect.width = info->width;
    rect.height = info->height;

    imb_task_head_set_buf(imb_task_head_get(0), info->buf, info->len, info->width, info->height, info->stride, info->lines, info->buf_num);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
    imb_start(imb_task_head_get(0), &screen_rect, disp, info->colums, info->lines, itf);

    return 0;
}


/**
 * @brief imb_api_bg 背景创建接口，合成必须先创建一个背景
 *
 * @param info合成的总体信息，具体看结构体注释
 *
 * @return 0:配置成功 其他:失败
 */
int imb_api_bg(struct imb_api_info *info)
{
    //创建背景任务(必须)
    imb_create_bg_task(info->color_format, info->width, info->height, info->backcolor);

    return 0;
}

/**
 * @brief imb_api_rotate 图片旋转合成接口
 *
 * @param info : 合成的总体信息，具体看结构体注释
 * @param rot : 旋转相关参数，具体看结构体注释
 *
 * @return 0:配置成功 其他:失败
 */
int imb_api_rotate(struct imb_api_info *info, struct imb_api_rotate_info *rot)
{
    int ret;
    /* struct image_file file; */

    //图片分块输入旋转输出测试
    /* int id = rot->id; */
    /* ret = open_image_by_id(0, g_ui_resfile, &file, id & 0xff, id >> 16); */
    /* if (!ret) { */
    /* return -1; */
    /* } */

    if (rot->file == NULL) {
        return -1;
    }

    if (rot->rotate_chunk) {
        imb_create_image_rotate_chunk_task(imb_task_get_id(), rot->file, &g_ui_resfile_info, rot->cx, rot->cy, rot->angle, rot->dx, rot->dy);
    } else {
        if (rot->buf) {
            imb_create_image_rotate_task(imb_task_get_id(), rot->file, rot->buf, rot->cx, rot->cy, rot->angle, rot->dx, rot->dy, NULL, LOAD_FROM_STATIC_RAM);
        } else {
            imb_create_image_rotate_task(imb_task_get_id(), rot->file, &g_ui_resfile_info, rot->cx, rot->cy, rot->angle, rot->dx, rot->dy, NULL, LOAD_FROM_FLASH_WITH_TAB);
        }
    }

    return 0;
}

/**
 * @brief imb_api_cut 图片裁剪合成接口
 *
 * @param info : 合成的总体信息，具体看结构体注释
 * @param cut : 裁剪相关参数，具体看结构体注释
 *
 * @return 0:配置成功 其他:失败
 */
int imb_api_cut(struct imb_api_info *info, struct imb_api_cut_info *cut)
{
    /* int ret; */
    /* int id = cut->id; */
    /* ret = open_image_by_id(0, g_ui_resfile, &file, id & 0xff, id >> 16); */
    /* if (!ret) { */
    /* return -1; */
    /* } */

    if (cut->file == NULL) {
        return -1;
    }

    if (cut->buf) {
        imb_create_image_task(imb_task_get_id(), cut->file, cut->buf, cut->x, cut->y, cut->width, cut->height, cut->crop, false, LOAD_FROM_STATIC_RAM);
    } else {
        imb_create_image_task(imb_task_get_id(), cut->file, &g_ui_resfile_info, cut->x, cut->y, cut->width, cut->height, cut->crop, true, LOAD_FROM_FLASH_WITH_TAB); //width,height参数可小于file.width, file.height,若小于则裁剪
    }


    return 0;
}


/**
 * @brief imb_api_sca 图片缩放合成接口
 *
 * @param info : 合成的总体信息，具体看结构体注释
 * @param sca : 缩放相关参数，具体看结构体注释
 *
 * @return 0:配置成功 其他:失败
 */
int imb_api_sca(struct imb_api_info *info, struct imb_api_sca_info *sca)
{
    /* int ret; */
    /* int id = sca->id; */
    /* //图片缩放输出测试 */
    /* ret = open_image_by_id(0, g_ui_resfile, &file, id & 0xff, id >> 16); */
    /* if (!ret) { */
    /* return -1; */
    /* } */

    if (sca->file == NULL) {
        return -1;
    }

    if (sca->buf) {
        imb_create_image_scale_task(imb_task_get_id(), sca->file, sca->buf, sca->ratio_w, sca->ratio_h, sca->x, sca->y, sca->width, sca->height, NULL, false, LOAD_FROM_STATIC_RAM);
    } else {
        imb_create_image_scale_task(imb_task_get_id(), sca->file, &g_ui_resfile_info, sca->ratio_w, sca->ratio_h, sca->x, sca->y, sca->width, sca->height, NULL, true, LOAD_FROM_FLASH_WITH_TAB); //width/height参数小于放大后的宽高则为截取
    }

    return 0;
}


extern int lcd_draw_area(u8 index, u8 *lcd_buf, int left, int top, int width, int height, int wait);
extern int lcd_draw_area_direct(u8 *buf0, u8 *buf1, u8 *lcd_buf, int left, int top, int width, int height, int wait);

static void imb_api_task(void *arg)
{
    int msg[32];
    int ret;

    printf("imb_api_task.\n");

    //屏初始化
    lcd_disp_init(arg);

    struct lcd_info info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

    u16 colums = info.width;
    u16 lines = info.buffer_size / info.buf_num / info.stride;

    //imb config
    struct imb_api_info api_info;
    struct imb_cb_interface *itf = &api_info.itf;
    struct image_file file;

    int id;
    struct imb_api_cut_info cut = {0};
    struct imb_api_rotate_info rot = {0};
    struct imb_api_sca_info sca = {0};

    memset(&api_info, 0, sizeof(struct imb_api_info));
    api_info.width = info.width;
    api_info.height = info.height;
    api_info.stride = info.stride;
    api_info.color_format = info.color_format;
    api_info.buf_num = info.buf_num;
    api_info.buf = info.buffer;
    api_info.len = info.buffer_size;
    api_info.lines = lines;
    api_info.colums = colums;
    api_info.backcolor = 0xffff;

    itf->buf_get = lcd_buffer_get;
    itf->buf_set_pending = lcd_buffer_pending;
    itf->draw_area = lcd_draw_area;
    itf->buf_check = lcd_buffer_check;
    itf->buf_set_idle = lcd_buffer_idle;
    itf->draw_meld_area = lcd_draw_area_direct;
    itf->buf_wait_finish = lcd_wait;

    imb_api_init();

    imb_api_bg(&api_info);

    //图片旋转输出
    id = PAGE0_daca_COMPASS1;
    imb_api_get_finfo(id, &file);
    /* memset(&file, 0, sizeof(&file)); */
    /* file.format = PIXEL_FMT_ARGB8565; */
    /* file.width = 48; */
    /* file.height = 49; */
    /* rot.buf = icon_48x49_argb8565; */
    rot.id = id;
    rot.file = &file;
    rot.rotate_chunk = 1;//分块输入
    rot.cx = 227;
    rot.cy = 227;
    rot.angle = 90;
    rot.dx = 227;
    rot.dy = 227;
    imb_api_rotate(&api_info, &rot);


    //图片裁剪输出
    id = PAGE0_5dad_05_MENU_ICON_BLOOD_SEL;
    imb_api_get_finfo(id, &file);
    /* memset(&file, 0, sizeof(&file)); */
    /* file.format = PIXEL_FMT_ARGB8565; */
    /* file.width = 48; */
    /* file.height = 49; */
    /* cut.buf = icon_48x49_argb8565; */
    cut.id = id;
    cut.file = &file;
    cut.x = 200;
    cut.y = 200;
    cut.width = file.width;
    cut.height = file.height;
    imb_api_cut(&api_info, &cut);

    //图片缩放输出
    id = PAGE0_5dad_05_MENU_ICON_BLOOD_SEL;
    imb_api_get_finfo(id, &file);
    /* memset(&file, 0, sizeof(&file)); */
    /* file.format = PIXEL_FMT_ARGB8565; */
    /* file.width = 48; */
    /* file.height = 49; */
    /* sca.buf = icon_48x49_argb8565; */
    sca.id = id;
    sca.file = &file;
    sca.ratio_w = 2.5;
    sca.ratio_h = 2.5;
    sca.x = 100;
    sca.y = 280;
    sca.width = file.width * sca.ratio_w;
    sca.height = file.height * sca.ratio_h;
    imb_api_sca(&api_info, &sca);

    imb_api_blend(&api_info);

    imb_api_del(&api_info);
    imb_api_uninit();

    lcd_disp_uninit();

    while (1) {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        if (ret != OS_TASKQ) {
            continue;
        }
    }
}


/*********************************imb api end**********************************************/



int imb_demo(void *arg)
{
    int err = 0;
    clock_add_set(DEC_UI_CLK);

    err = task_create(imb_task, arg, "imb");
    /* err = task_create(imb_api_task, arg, "imb"); */

    return err;
}

#endif

#if USR_LVGL_IMB2D_EN
#include "ui/ui_core.h"
#include "ui/lvgl/src/draw/sw/lv_draw_sw.h"
#include "ui/lvgl/src/core/lv_refr.h"
#include "ui/lvgl/src/draw/lv_img_decoder.h"
// #include "lv_img_decoder.h"
#include "ui/lvgl/src/misc/lv_assert.h"
#include "ui/lvgl/src/draw/lv_draw_img.h"
#include "ui/lvgl/src/misc/lv_ll.h"
#include "ui/lvgl/src/misc/lv_gc.h"
typedef struct {
    int offset;
    int length;
} IndexEntry;
typedef struct _ui_res_t {
    u8 letter;   //盘符
    u8 mode;   //标识资源名
    u8 *imbindex;  //索引表路径（IMB资源）
    u8 *imbres;    //资源路径（IMB资源）
    RESFILE *resfile;   //资源文件句柄（IMB资源）
    RESFILE *indexfile; //索引表文件句柄（IMB资源）
    int filelen;    //文件数
    int namelen;    //文件长度
    void *cache;    //索引表sram缓存
    struct flash_file_info flash_file_info; //tab表

    void *phyaddr;//    自定义区起始地址
    void *reladdr;//    资源相对于自定义区的地址

} ui_res_t;

//  ui资源分组my_fid
//eg. B:ui/ui1/test.jpg
//eg. B:JL2DZIP/test.jpg

#include "ui/ui/jl_images/usr_pic_index.h"

/* IMB资源 */
//  ID资源
#define IN_RES_WITH_ID   0  //不支持
// #define IN_ADDR_WITH_ID   1 //不支持
#define IMG_BIN_JPG_FILE    1

// //  名字搜索
// #define IN_CODE_TABLE_WITH_NAME   3
// #define IN_ADDR_TABLE_WITH_NAME   4

// //  JPG动画
// #define JPG_ANIM_WITH_FS    6
// #define JPG_ANIM_WITH_ADDR    7
// #define PSRAM_IMG           8
/* 非IMB资源 */
// #define 


static ui_res_t ui_res_gropu[] = {
    //  纯ID去搜索    
    {.letter = '\0',    .mode = IN_RES_WITH_ID,    .imbres = RES_PATH"ui/ui.res",                                                    },    //文件系统资源
    
    {.letter = 'A',     .mode = IN_RES_WITH_ID,    .imbres = RES_PATH"ui/ui.res",     },    //code索引表
    {.letter = 'B',     .mode = IMG_BIN_JPG_FILE,       },    //文件系统资源

    //  以下未完善
    // //  PSRAM整屏截图缓存
    // // H:0 H:1 ...
    // {.letter = 'H',     .mode = PSRAM_IMG,    },

    // //  SD播放JPG动画
    // {.letter = 'G',     .mode = JPG_ANIM_WITH_FS,    },
};

//  P->lv普通文件访问

#if 1
struct rect *image_set_crop(int x, int y, int width, int height)
{
    static struct rect crop = {0};
    crop.left = x;
    crop.top = y;
    crop.width = width;
    crop.height = height;
    return &crop;
}
#endif

enum LOAD_METHOD {
    LOAD_FROM_FLASH_WITH_TAB,       //flash数据带地址映射表选择该方式，该方式只支持压缩数据(L1格式除外)
    LOAD_FROM_FLASH_WITHOUT_TAB,    //flash数据不带地址映射表选择该方式，该方式支持压缩数据或者非压缩数据
    LOAD_FROM_RAM,                  //malloc申请的内存选择该方式, imb任务销毁的时候会调用free释放内存
    LOAD_FROM_VLT_RAM,              //malloc_vlt申请的内存选择该方式,imb任务销毁的时候会调用free_vlt释放内存
    LOAD_FROM_PSRAM,                //malloc_psram申请的内存选择该方式,imb任务销毁的时候会调用free_psram释放内存
    LOAD_FROM_RAM_AUTO,             //imb_malloc申请的内存选择该方式(若存在掉电RAM，会优先从掉电RAM里申请)，imb任务销毁的时候会调用imb_free释放内存
    LOAD_FROM_STATIC_RAM,           //若希望外部去释放malloc/malloc_vlt/malloc_psram申请的内存或者或者是一个静态
    //数组时选择该方式，imb任务销毁的时候不对该内存做释放处理
};

static const struct ui_platform_api br28_platform_api = {
    .malloc             = malloc,
    .free               = free,
};
struct bg_buffer {
    u8 *buf;
    int len;
};
static struct flash_file_info g_ui_resfile_info;
static struct flash_file_info g_ui_strfile_info;
static RESFILE *g_ui_resfile = NULL;
static RESFILE *g_ui_strfile = NULL;
struct ui_res_info {
    const char *path;
    struct flash_file_info ui_resfile_info;
    struct flash_file_info ui_strfile_info;
    RESFILE *ui_resfile;
    RESFILE *ui_strfile;
};

struct ui_display {
    struct lcd_interface *lcd;
};
static struct ui_display display = {0};
#define __this (&display)
struct ui_res_info *__this_res = NULL;
u8 *usr_buf = NULL;
//合成一次lines前，获取存放合成输出的buf，根据合成配置的lines(当lines=height，就表示一次合成一帧)而变化,必须要实现
u8 *usr_buf_get(u8 index, u8 *pre_baddr){
    // if((usr_buf >= 0x2000000) && (usr_buf < (0x2000000 + 0x1000000))){
        // psram_flush_invaild_cache(usr_buf, 392*392*2);
    //     return psram_cache2nocache_addr(usr_buf);
    // }


    return usr_buf;    //获取当前正在合成的BUF
}
//如果有buf管理,该接口可用于设置buf写好数据后调用本接口设置成等待状态才能推,不是必要
u8 usr_buf_set_pending(u8 index, u8 *buf){
    // r_printf("%s %d",__func__,__LINE__);
}
//合成一次lines之后，输出到外部的buf,如果lines不等于height（分块合成的时候），
//最后一次合成的数据可能没有lines那么多行，例如454X454的屏，lines=40，最后一次合成的lines只有454-40*11=14行,必须要实现
int usr_draw_area(u8 index, u8 *buf, int left, int top, int width, int height, int wait){
    // r_printf("%s %d ltwh %d %d %d %d",__func__,__LINE__,
    //     left, top, width, height
    // );
    // lcd_draw_area(0, buf, left, top, width, height, 0); 
    // lcd_wait();
}
//判断buffer是不是可以推屏，当没有用我们原本的推屏buf管理模块的时候，不是必要
u8 *usr_buf_check(u8 index, u8 *pre_baddr){
    r_printf("%s %d",__func__,__LINE__);
    return usr_buf;    //获取当前正在合成的BUF
}
//修改buffer状态为空闲,当没有用我们原本的推屏buf管理模块的时候，不是必要
void usr_buf_set_idle(u8 index){
    r_printf("%s %d",__func__,__LINE__);
}
//等待推屏完成，当没有用我们原本的推屏buf管理模块的时候，不是必要
void usr_buf_wait_finish(){
    r_printf("%s %d",__func__,__LINE__);
}
//当使用我们原本的推屏buf管理模块的时候,可以用lcd_draw_area_direct赋值，不是必要
int usr_draw_meld_area(u8 *buf0, u8 *buf1, u8 *meld_buf, int left, int top, int width, int height, int wait){
    r_printf("%s %d",__func__,__LINE__);
}

static u8 *psram_buf1;
static u8 *psram_buf2;
static u8 *psram_act;

/*----------------------------------------------------------------------------*/
/**@brief
   @param   type : 1 - 全屏区域,  2 - 局部区域
   @param   rect : 当前帧需要刷新的区域(该区域会根据buffer大小分成n块)
   @param   lcd_buf : 当前分块buffer的地址
   @param   stride : 当前分块一行像素的字节数（2D硬件加速生成的是4字节对齐，不足4字节则补齐）
   @param   left, top, width, height : 当前分块的范围
   @param   wait : 0 - 中间分块， 1 - 第一个分块， 2 - 最后一个分块(若只有一个分块则为1， 若只有两个分块,则第一个分块为1，第二个分块为2)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_data_output(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait)
{
    // printf("[%d, %d, %d, %d], block [%d, %d, %d, %d], type:%d, buf:0x%x, stride:%d, wait:%d\n", rect->left, rect->top, rect->width, rect->height, left, top, width, height, type, lcd_buf, stride, wait); 
    
    u8 *p = psram_buf2;
    
    dma_memcpy(psram_cache2nocache_addr(&p[top * stride]), lcd_buf, stride * height);
    // lcd_draw_area(0, lcd_buf, left, top, width, height, 0); 
    /* put_buf(lcd_buf, stride * height); */
    /* os_time_dly(50); */
}


/*----------------------------------------------------------------------------*/
/**@brief   若imb_data_output里面的接口为非阻塞接口(例如dma操作)， 这里需要等待处理完成， 若为阻塞接口，这里不需要改动
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_data_output_wait_finish(u8 buf_index)
{
    imb_buffer_unlock(buf_index); /* 解锁buffer(必须) */
}

void lcd_disp_init(void *arg, void **buf1, void **buf2, int *lenp)
{
    struct lcd_info info = {0};
    __this->lcd = lcd_get_hdl();
    ASSERT(__this->lcd);

    if(__this->lcd->init) 
        __this->lcd->init(arg);

    if(__this->lcd->clear_screen) 
        __this->lcd->clear_screen(0x000000);

    // u8 lcd_backlight = \
    //     GetVmParaCacheByLabel(vm_label_backlight);

    if(__this->lcd->backlight_ctrl)
        __this->lcd->backlight_ctrl(0);

    if(__this->lcd->get_screen_info) 
        __this->lcd->get_screen_info(&info);

    u8 *buf = NULL;
    u32 len;

#if 0   //  单个整BUF(适用屏幕比较小的情况，如240*240)
        *buf1 = malloc(info.width*info.height*2);      
        *buf2 = NULL;
        *lenp = info.width*info.height*2;        
#elif 1 //  2个小BUFFER（屏幕比较大）    
    if (__this->lcd->buffer_malloc) {
        __this->lcd->buffer_malloc(&buf, &len);
    }

    printf("%s:len = %d\n", __func__, len);

    *buf2 = lcd_buffer_init(0, buf, len / info.buf_num, info.buf_num);
    *buf1 = buf;
    *lenp = len;    

#else   //PSRAM整帧
    len = info.height*info.width*2;
    *lenp = len*2;
    *buf2 = malloc_psram(len);
    *buf1 = malloc_psram(len);
    psram_flush_invaild_cache(*buf2, len);
    psram_flush_invaild_cache(*buf1, len);
#endif

    // struct rect rect;
    // rect.left   = 0;
    // rect.top    = 0;
    // rect.width  = info.width;
    // rect.height = info.height;
    // ui_core_init(&br28_platform_api, &rect);

    return;
}

int imb_api_init()
{
    int ret = 0;

    //打开图片资源文件
    g_ui_resfile = res_fopen(RES_PATH"ui/ui.res", "r");
    ASSERT(g_ui_resfile);
    //获取图片资源文件flash存储信息
    ret = ui_res_flash_info_get(&g_ui_resfile_info, RES_PATH"ui/ui.res", "res");
    ASSERT(!ret);

    // g_ui_strfile = res_fopen(RES_PATH"ui/ui.str", "r");
    // ASSERT(g_ui_strfile);
    // ret = ui_res_flash_info_get(&g_ui_strfile_info, RES_PATH"ui/ui.str", "res");
    // ASSERT(!ret);

    return 0;
}

int imb_api_uninit()
{
    imb_task_all_destroy(imb_task_head_get(0));
    if (g_ui_resfile) {
        res_fclose(g_ui_resfile);
        g_ui_resfile = NULL;
        ui_res_flash_info_free(&g_ui_resfile_info, "res");
    }

    if (g_ui_strfile) {
        res_fclose(g_ui_strfile);
        g_ui_strfile = NULL;
        ui_res_flash_info_free(&g_ui_strfile_info, "res");
    }

    return 0;
}
struct ui_res_info *ui_res_init()
{
    static struct ui_res_info ui_res_info_t = {0};
    int ret;

    ui_res_info_t.path = RES_PATH"ui/ui.res";

    //打开图片资源文件
    ui_res_info_t.ui_resfile = res_fopen(RES_PATH"ui/ui.res", "r");
    ASSERT(ui_res_info_t.ui_resfile);
    //获取图片资源文件flash存储信息
    ret = ui_res_flash_info_get(&ui_res_info_t.ui_resfile_info, RES_PATH"ui/ui.res", "res");
    ASSERT(!ret);

    ui_res_info_t.ui_strfile = res_fopen(RES_PATH"ui/ui.str", "r");
    ASSERT(ui_res_info_t.ui_strfile);
    ret = ui_res_flash_info_get(&ui_res_info_t.ui_strfile_info, RES_PATH"ui/ui.str", "res");
    ASSERT(!ret);

    __this_res = &ui_res_info_t;
    return &ui_res_info_t;
}

void imb_large_image_scale_optimize_test(struct ui_res_info *info);

void lvgl_disp_init(void *arg, void **buf1, void **buf2, int *lcd_w,int *lcd_h, int *colums, int *lines){
    printf("lvgl_task.\n");

    extern char *ui_lib_version();
    printf("lib ver : %s\n", ui_lib_version());

    // struct ui_res_info *res = ui_res_init();
    // imb_api_init();
    
    //屏初始化
    int buflen=0;
    lcd_disp_init(arg, buf1, buf2, &buflen);
    printf("buf1:%x buf2:%x",*buf1,*buf2 );

    struct lcd_info info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

    *lcd_w = info.width;
    *lcd_h = info.height;
    *colums = info.width;
    *lines = buflen / info.buf_num / info.stride;

#if PSRAM_FULL_SCREEN
    psram_buf1 = malloc_psram(info.width*info.height*2);
    psram_buf2 = malloc_psram(info.width*info.height*2);

    printf("psram_buf1 = %p, psram_buf2 = %p\n", \
        psram_buf1, psram_buf2);

    psram_act =psram_buf1;
#endif


    // struct ui_res_info *res = ui_res_init();
    // imb_large_image_scale_optimize_test(res);
}

#if defined(TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE
#define PHY_ADDR(addr) psram_cache2nocache_addr(addr)
#else
#define PHY_ADDR(addr) addr
#endif

volatile u8 usr_wait_te = 0;
volatile u8 usr_full_screen = 0;

void full_screen_flush(){
    struct lcd_info info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

//  测试PSRAM单双BUF刷新的区别不是很明显
#if 1   //单PSRAM BUFFER
    //  等同步
    // flushinv_dcache(psram_act,info.width*info.height*2);
    u8 WaitTe = GetUsrWaitTe();
    lcd_draw_area(0, PHY_ADDR(psram_act), 0, 0, info.width, info.height, WaitTe);
#else   //双PSRAM BUFFER
    if(psram_act == psram_buf1){ 
        lcd_draw_area(0, PHY_ADDR(psram_act), 0, 0, info.width, info.height, WaitTe);
        psram_act = psram_buf2;
    }else{
        lcd_draw_area(0, PHY_ADDR(psram_act), 0, 0, info.width, info.height, WaitTe);
        psram_act = psram_buf1;
    }
#endif
}

void lv_lcd_data_copy(u8 *lcd_buf, int left, int top, int width, int height)
{
    struct lcd_info info = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

    int h;
    u8 *p;

    int bytes_per_pixel = 2;
    int lcd_stride = info.width*2;

    p = psram_act;

    // lcd_wait();
    dma_memcpy_wait_idle();    
    if ((left == 0) && (width == info.width)) {
        dma_memcpy(PHY_ADDR(&p[top * lcd_stride]), lcd_buf, width * height * bytes_per_pixel);
    } else {
        for (h = 0; h < height; h++) {
            dma_memcpy(PHY_ADDR(&p[(top + h)*lcd_stride + left * bytes_per_pixel]), &lcd_buf[h * width * bytes_per_pixel], width * bytes_per_pixel);
        }
    }
    // psram_flush_invaild_cache(p, info.width*info.height*2);
}


struct jpeg_api_info {
    int id;

    u8 out_format;
    void *fat_info;
    jpg_stream_input jpg_ginfo_cb;
    struct jpeg_decoder_fd *jpg_fd;
};

struct jpeg_api_out_lines {
    struct rect jpeg_page_rect;
    struct rect cur_draw;
    struct rect control_draw;//控件相对背景的实际范围，图片可能比控件大
    u8 *outbuf;
};
// static struct flash_file_info g_ui_resfile_info = {0};
static struct flash_file_info g_jpg_flash_info = {0};

static struct flash_file_info *jpeg_api_get_finfo(int id, struct image_file *f)
{
    struct flash_file_info *flash_info = NULL;
    u32 filelen;
    u32 flash_map_tab_num;
    u32 file_base_addr;

    if (!g_ui_resfile_info.tab) {
        printf("e1\n");
        return NULL;
    }

    int ret = open_image_by_id(0, g_ui_resfile, f, id & 0xffff, id >> 16);
    if (ret != 0) {
        printf("e2\n");
        return NULL;
    }

    flash_info = &g_jpg_flash_info;

    imb_task_lock();

    filelen = f->len;
    flash_info->tab = &g_ui_resfile_info.tab[(f->offset + g_ui_resfile_info.offset) / 4096];
    file_base_addr = flash_info->tab[0] + (f->offset + g_ui_resfile_info.offset) % 4096;
    flash_map_tab_num = (file_base_addr % 4096 + filelen + 4096 - 1) / 4096;
    flash_info->offset = file_base_addr % 4096;
    flash_info->tab_size = flash_map_tab_num * sizeof(u32);
    flash_info->last_tab_data_len = flash_map_tab_num * 4096 - filelen - flash_info->offset;

    imb_task_unlock();

    return flash_info;
}
void *test_vuf;
static u8 *usr_jpg_dec_stream_input(void *__fat_info, u32 *len, u32 offset_4k);
int jpeg_api_get_info(struct jpeg_api_info *info)
{
    struct jpeg_decoder_fd *jpg_fd;
    struct image_file file;
    int ret;

    jpg_fd = jpeg_dec_get_fd();
    if ((!jpg_fd) || (!jpg_fd->fat_info) || (!jpg_fd->info.x)
        || (jpg_fd->fat_info != info->fat_info)) {

        info->fat_info = jpeg_api_get_finfo(info->id, &file);
        if (info->fat_info == NULL) {
            printf("jpg get info err1\n");
            return -1;
        // }
        }

        imb_task_lock();
        ret = jpeg_dec_init(info->fat_info, info->out_format, info->jpg_ginfo_cb);
        imb_task_unlock();
        if (ret != 0) {
            printf("jpg get info err2\n");
            return -1;
        }
    }

    /* info->out_format = info->out_format;  */
    /* info->jpg_ginfo_cb = info->jpg_ginfo_cb; */
    info->jpg_fd = jpeg_dec_get_fd();

    return 0;
}

// static u8 *jpg_dec_stream_input(void *__fat_info, u32 *len, u32 offset_4k)
// {
//     struct jpeg_decfile_info *info = (struct jpeg_decfile_info *)__fat_info;
//     u8 *jpg_stream = info->jpg_stream;
//     int file_len = info->jpg_file_len;
//     u32 offset = offset_4k * 4096;
//     u32 res_len;

//     if (offset >= file_len) {
//         *len = 0;
//         return NULL;
//     }
//     res_len = file_len - offset;
//     *len = (res_len > 4096) ? 4096 : res_len;

//     return jpg_stream + offset;
// }




int jpeg_api_open(struct jpeg_api_info *info)
{
    int ret;
    struct image_file file;

    info->fat_info = jpeg_api_get_finfo(info->id, &file);
    if (info->fat_info == NULL) {
        return -1;
    // }
    }
// struct flash_file_info *p = info->fat_info;
    // printf("llen offset tab tabsize %d %d %d %d",
    //     p->last_tab_data_len,
    //     p->offset,
    //     p->tab,
    //     p->tab_size
    // );
    

    imb_task_lock();
    ret = jpeg_dec_init(info->fat_info, info->out_format, info->jpg_ginfo_cb);
    imb_task_unlock();
    if (ret != 0) {
        printf("jpg get info err\n");
        return -1;
    }
    info->jpg_fd = jpeg_dec_get_fd();

    return 0;
}
int jpeg_api_close(struct jpeg_api_info *info)
{
    jpeg_dec_uninit();

    memset(&g_jpg_flash_info, 0, sizeof(struct flash_file_info));
    return 0;
}
int jpeg_api_read_lines(struct jpeg_api_info *info, struct jpeg_api_out_lines *out)
{
    struct jpeg_decoder_fd *jpg_fd;
    struct rect *jpeg_page_rect = &out->jpeg_page_rect;
    struct rect *cur_draw = &out->cur_draw;
    struct rect *control_draw = &out->control_draw;//控件相对背景的实际范围，图片可能比控件大
    struct rect jpeg_rect;
    struct rect draw_rect;
    struct rect r;
    int ret;

    if (!get_rect_cover(jpeg_page_rect, cur_draw, &r)) {
        /* printf("jpeg not in area1\n"); */
        return -1;
    }
    if (!get_rect_cover(control_draw, &r, &draw_rect)) {
        /* printf("jpeg not in area2\n"); */
        return -1;
    }

    jpg_fd = jpeg_dec_get_fd();
    jpeg_rect.left = draw_rect.left - jpeg_page_rect->left;
    jpeg_rect.top = draw_rect.top - jpeg_page_rect->top;
    jpeg_rect.width = draw_rect.width;
    jpeg_rect.height = draw_rect.height;

    if ((jpeg_rect.left < 0) || (jpeg_rect.top < 0)
        || (jpeg_rect.left + jpeg_rect.width > jpg_fd->info.x)
        || (jpeg_rect.top + jpeg_rect.height > jpg_fd->info.y)) {
        printf("\nerr_draw_jpeg %d, %d, %d, %d, %d, %d\n", jpeg_rect.left, jpeg_rect.top, jpeg_rect.width, jpeg_rect.height, jpg_fd->info.x, jpg_fd->info.y);
        return -1;
    }

    /* printf("top %d, %d\n", jpeg_rect.top, cur_draw->top); */
    /* printf("top %d\n", jpeg_rect.top); */
    /* printf("top %d\n", cur_draw->top); */

    imb_task_lock();
    ret = jpeg_dec_start(cur_draw, &draw_rect, &jpeg_rect, out->outbuf);
    imb_task_unlock();

    return 0;
}
void imb_jpeg_draw(struct rect rect,
    int px,int py,int pw, int ph, 
    int id, void *buf, int bufsize){

    struct lcd_info info = {0};
    __this->lcd = lcd_get_hdl();
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }
    static struct jpeg_api_info api_info = {0};
    static struct jpeg_api_out_lines out = {0};
    static struct jpeg_decoder_fd *jpg_fd = NULL;
    int h, width, height;
    u32 pixel_lines;
    u32 pixel_len;
    u8 plane_num;
    u8 *pre_baddr;
    memset(&api_info, 0 ,sizeof(struct jpeg_api_info));
    /* api_info.id = PAGE0_0000_KK; */
    api_info.id = id;
    api_info.out_format = OUTPUT_FORMAT_RGB565;//支持565
    api_info.jpg_ginfo_cb = NULL;
    api_info.jpg_fd = NULL;

    /* api_info.id = PAGE0_0000_TEST3; */
    int ret = jpeg_api_get_info(&api_info);
    if (ret != 0) {
        printf("get info err\n");
        while (1);
    }

    ret = jpeg_api_open(&api_info);
    if (ret != 0) {
        printf("jpg open err\n");
        while (1);
    }

    jpg_fd = api_info.jpg_fd;
    width = jpg_fd->info.x;//jpeg图片本身的宽度
    height = jpg_fd->info.y;//jpeg图片本身的高度


    //jpeg图片原本的范围，相对于背景
    out.jpeg_page_rect.left = px;
    out.jpeg_page_rect.top = py;
    out.jpeg_page_rect.width = pw;
    out.jpeg_page_rect.height = ph;

    //需要合成的范围，该范围必须在jpeg范围内，相对于背景
    memcpy(&out.control_draw, &out.jpeg_page_rect, sizeof(struct rect));

    // printf("w %d, h %d\n", width, height);

    //合成区域
    out.cur_draw.left = rect.left;
    out.cur_draw.top = rect.top;
    out.cur_draw.width = rect.width;//屏幕的宽
    out.cur_draw.height = rect.height;//屏幕推一次的高
    out.outbuf = buf;

    // printf("rect %d %d %d %d", rect.left, rect.top, rect.width, rect.height);
    // printf("pxywh %d %d %d %d %x", px,py,pw,ph , buf);
    lcd_buffer_idle(0);
    ret = jpeg_api_read_lines(&api_info, &out);
    lcd_buffer_pending(0, out.outbuf);
    jpeg_api_close(&api_info);



}
const char *fs_get_equre(const char *fn)
{
    size_t i;
    for (i = strlen(fn); i > 0; i--) {
        if (fn[i] == '=') {
            return &fn[i + 1];
        } else if (fn[i] == '/' || fn[i] == '\\') {
            return ""; /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name.*/
}

int hex_to_int(char *hex_string) {
    int result = 0;
    int len = strlen(hex_string);
    for (int i = 0; i < len; i++) {
        char c = hex_string[i];
        if (c >= '0' && c <= '9') {
            result = result * 16 + (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            result = result * 16 + (c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            result = result * 16 + (c - 'A' + 10);
        } else {
            printf("Invalid hex string!\n");
            return -1;
        }
    }
    return result;
}

#include "ui/ui/jl_images/usr_pic_index.h"
// int get_jl_image_info_by_src(const void *src, lv_img_header_t *header, struct image_file *file)
// {
//     int ret;
//     if (memcmp("JL2DZIP/", src, strlen("JL2DZIP/"))){
//         return 0;
//     }
//     int i,id=0;
    
//     char *name = lv_fs_get_last(src);

//     char *idstr = fs_get_equre(src);
//     if(*idstr){
//         id = hex_to_int(idstr);
//         // printf("idstr SUCCEED %s %d", name,id);
//     } else {
//         for(i=0;i<get_img_num();i++){
//             extern const struct fid my_fid[];
//             if(!strcmp(my_fid[i].name, src)){
//                 id = my_fid[i].id;
//                 // printf("noidstr SUCCEED %s %d", name,id);
//                 break;
//             }
//         }
//     }

//     if(i == get_img_num()){
//         return 0;
//     }
    
//     if(!__this_res){
//         return 0;   
//     }

//     struct image_file file_t;
//     if(file==NULL){
//         file = &file_t;
//     }
//     ret = open_image_by_id(0, __this_res->ui_resfile, file, id & 0xffff, id >> 16);
//     if(ret){
//         return 0;
//     }

//     if(!header){
//         return id;
//     }   
//     // printf("wh format: %d %d %d", file->width, file->height, file->format);
//     header->w  = file->width;
//     header->h  = file->height;
//     if(file->format == 8){
//         header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
//     } else {
//         header->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
//     }

//     return id;
// }
int imb_task_get_id()
{
    static int task_id = 0;
    return ++task_id;
}
/*----------------------------------------------------------------------------*/
/**@brief    创建图片缩放任务
   @param    task_id : 任务id
   @param    file:图片信息
   @param    priv : LOAD_FROM_FLASH_WITH_TAB时为flash_file_info, 其他为图像数据的地址
   @param    ratio_w:水平放大倍率
   @param    ratio_h:垂直放大倍率
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:显示的图片宽度（放大后的宽度）
   @param    height:显示的图片高度（放大后的高度）
   @param    crop:为空时不对图片进行裁剪, 非空时为图片的裁剪区域
   @param    zip:是否压缩数据
   @param    load_method:数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_scale_task(int task_id, struct image_file *file, u32 priv, float ratio_w, float ratio_h, int x, int y, int width, int height, struct rect *crop, int zip, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.scale_en = 1;//true;//缩放使能
    if (imb_tsk_info.scale_en) {
        imb_tsk_info.ratio_w = ratio_w;
        imb_tsk_info.ratio_h = ratio_h;
        ASSERT(imb_tsk_info.ratio_w);
        ASSERT(imb_tsk_info.ratio_h);
    }

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src = DATA_SRC_FLASH;
        imb_tsk_info.cur_in_flash = 1;
    } else {
        imb_tsk_info.data_src = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash = 0;
    }

    if (file->format == PIXEL_FMT_L1) {
        imb_tsk_info.zip_en     = 0;
    } else {
        imb_tsk_info.zip_en     = zip;
    }

    if (load_method == LOAD_FROM_FLASH_WITH_TAB || load_method == LOAD_FROM_FLASH_WITHOUT_TAB) {
        imb_tsk_info.addr_source = SOURCE_FLASH;
    } else if (load_method == LOAD_FROM_RAM_AUTO) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_RAM) {
        imb_tsk_info.addr_source = SOURCE_RAM;
    } else if (load_method == SOURCE_VLT_RAM) {
        imb_tsk_info.addr_source = SOURCE_VLT_RAM;
    } else if (load_method == SOURCE_PSRAM) {
        imb_tsk_info.addr_source = SOURCE_PSRAM;
    } else if (load_method == SOURCE_STATIC_RAM) {
        imb_tsk_info.addr_source = SOURCE_STATIC_RAM;
    }

    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.x_offset       = x;
    imb_tsk_info.y_offset       = y;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.des_w          = width;
    imb_tsk_info.des_h          = height;

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.ff_info = priv;
        imb_tsk_info.priv = (u32)file;
    } else {
        imb_tsk_info.priv = (u32)priv;
    }

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type        = L1_IMAGE;
    }

    //裁剪区域
    if (crop) {
        imb_tsk_info.rotate_scale_crop_en = 1;
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    创建图片任务
   @param    task_id : 任务id
   @param    file : 图片信息
   @param    priv : LOAD_FROM_FLASH_WITH_TAB时为flash_file_info, 其他为图像数据的地址
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:图片显示的宽度，必须小于或者等于图片的实际宽度，小于实际宽度则截取显示
   @param    height:图片显示的高度，必须小于或者等于图片的实际高度，小于实际高度则截取显示
   @param    crop : 为空时不对图片进行裁剪, 非空时为图片的裁剪区域
   @param    zip : 是否压缩数据
   @param    load_method : 数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note     图片存储在flash, 指定flash地址映射表(可以支持数据不连续存储，以4k为单位，中间4k必须填满)
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_task(int task_id, struct image_file *file, u32 priv, int x, int y, int width, int height, struct rect *crop, int zip, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src = DATA_SRC_FLASH;//直接从flash取数
        imb_tsk_info.cur_in_flash = 1;//数据存在flash
    } else {
        imb_tsk_info.data_src = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash = 0;
    }

    if (file->format == PIXEL_FMT_L1) {
        imb_tsk_info.zip_en = 0;
    } else {
        imb_tsk_info.zip_en = zip;
    }


    if (load_method == LOAD_FROM_FLASH_WITH_TAB || load_method == LOAD_FROM_FLASH_WITHOUT_TAB) {
        imb_tsk_info.addr_source = SOURCE_FLASH;
    } else if (load_method == LOAD_FROM_RAM_AUTO) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_RAM) {
        imb_tsk_info.addr_source = SOURCE_RAM;
    } else if (load_method == SOURCE_VLT_RAM) {
        imb_tsk_info.addr_source = SOURCE_VLT_RAM;
    } else if (load_method == SOURCE_PSRAM) {
        imb_tsk_info.addr_source = SOURCE_PSRAM;
    } else if (load_method == SOURCE_STATIC_RAM) {
        imb_tsk_info.addr_source = SOURCE_STATIC_RAM;
    }
    imb_tsk_info.in_format = file->format;
    imb_tsk_info.x_offset = x;
    imb_tsk_info.y_offset = y;
    imb_tsk_info.src_w = file->width;
    imb_tsk_info.src_h = file->height;
    imb_tsk_info.des_w = width;
    imb_tsk_info.des_h = height;
    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.ff_info = priv;
        imb_tsk_info.priv = (u32)file;
    } else {
        imb_tsk_info.priv = (u32)priv;
    }

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type = L1_IMAGE;
    }

    //裁剪区域
    if (crop) {
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info); //imb_task参数重置
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    创建图片旋转任务
   @param    task_id : 任务id
   @param    file:图片信息
   @param    priv:图片存储信息
   @param    cx:图片的旋转中心x坐标，相对于图片
   @param    cy:图片的旋转中心y坐标，相对于图片
   @param    angle:旋转角度(0~359,最小单位1度)
   @param    dx:输出的旋转中心点x坐标，相对于背景
   @param    dy:输出的旋转中心点y坐标，相对于背景
   @param    crop : 为空时不对图片进行裁剪, 非空时为图片的裁剪区域
   @param    load_method : 数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_rotate_task(int task_id, struct image_file *file, u32 priv, int cx, int cy, int angle, int dx, int dy, struct rect *crop, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.rotate_en = 1;
    if (imb_tsk_info.rotate_en) {
        imb_tsk_info.x_src_offset   = 0;
        imb_tsk_info.y_src_offset   = 0;
        imb_tsk_info.rotate_c_x     = cx;
        imb_tsk_info.rotate_c_y     = cy;
        imb_tsk_info.rotate_angle   = angle;
        imb_tsk_info.rotate_dx      = dx;
        imb_tsk_info.rotate_dy      = dy;
        imb_tsk_info.rotate_chunk   = 0;
    }

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src       = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash   = 1;//数据存放在flash，内部会先将压缩数据解压到sram, 然后imb从sram取数
    } else {
        imb_tsk_info.data_src       = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash   = 0;
    }

    imb_tsk_info.zip_en         = 0;//旋转不支持压缩格式

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_FLASH_WITHOUT_TAB) {
        imb_tsk_info.addr_source = SOURCE_FLASH;
    } else if (load_method == LOAD_FROM_RAM_AUTO) {
        imb_tsk_info.addr_source = SOURCE_DEFAULT;
    } else if (load_method == LOAD_FROM_RAM) {
        imb_tsk_info.addr_source = SOURCE_RAM;
    } else if (load_method == SOURCE_VLT_RAM) {
        imb_tsk_info.addr_source = SOURCE_VLT_RAM;
    } else if (load_method == SOURCE_PSRAM) {
        imb_tsk_info.addr_source = SOURCE_PSRAM;
    } else if (load_method == SOURCE_STATIC_RAM) {
        imb_tsk_info.addr_source = SOURCE_STATIC_RAM;
    }

    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    //旋转这组参数设置无效，默认为背景任务大小
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.des_w          = 0;
    imb_tsk_info.des_h          = 0;

    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.ff_info = priv;
        imb_tsk_info.priv = (u32)file;
    } else {
        imb_tsk_info.priv = (u32)priv;
    }

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type        = L1_IMAGE;
    }

    //裁剪区域
    if (crop) {
        imb_tsk_info.rotate_scale_crop_en = 1;
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}
/*----------------------------------------------------------------------------*/
/**@brief    图片旋转分块输入回调
   @param    priv:任务信息
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void imb_rotate_chunk_cb(void *priv)
{
    struct imb_task_info *tsk_info = (struct imb_task_info *)priv;
    struct imb_task *task = (struct imb_task *)tsk_info->task;
    struct imb_task_head *head = imb_task_head_get(task->root_index);
    struct rect *rect = &head->curr_draw;

    task->x_src_offset = tsk_info->x_src_offset;
    task->src_w = tsk_info->src_w;
    task->y_src_offset = tsk_info->y_src_offset + tsk_info->rotate_chunk_offset_cur;
    task->src_h = (tsk_info->src_h - tsk_info->y_src_offset - tsk_info->rotate_chunk_offset_cur) > tsk_info->rotate_chunk ?
                  tsk_info->rotate_chunk : (tsk_info->src_h - tsk_info->y_src_offset - tsk_info->rotate_chunk_offset_cur);
    task->src_w_pre = tsk_info->src_w;
    task->src_h_pre = tsk_info->src_h;
    task->cur_format = tsk_info->in_format; //LAYER_FORMAT_ARGB8565;

    int stride = image_stride(task->src_w, task->cur_format);
    u16 lut_tab_size = image_palette_size(task->cur_format);

    if (lut_tab_size) {
        printf("lut_tab_size %d", lut_tab_size);
        return;
    }

    int height = tsk_info->rotate_chunk + 1;
    u8* buf1 = tsk_info->rotate_buffer;
    u8* buf2 = (u8*)tsk_info->rotate_buffer + stride * height;

    if (!tsk_info->rotate_buffer) {
        tsk_info->rotate_buffer = (u8 *)imb_malloc(stride * height*2, false);
        // printf("buf:%d",stride * height*2);
        buf1 = tsk_info->rotate_buffer;
        buf2 = (u8*)tsk_info->rotate_buffer + stride * height;

        task->dat_src_adr = buf2;
        u8 * p = (u8*)tsk_info->ff_info + lut_tab_size + stride * task->y_src_offset;

        if ((task->y_src_offset + task->src_h) < task->src_h_pre) {
            // res_fread(file, tsk_info->rotate_buffer, stride * (task->src_h + 1));//为了解决分块输入导致的相位问题，非最后一个分块需要多读取一行像素数据
            dma_memcpy(buf1, p, stride * (task->src_h + 1));
        } else {
            // res_fread(file, tsk_info->rotate_buffer, stride * task->src_h);
            dma_memcpy(buf1, p, stride * task->src_h);
        }
    }

    if(buf1==task->dat_src_adr){
        // 上一次显示的为buf1，上一次拷贝的是buf2
        dma_memcpy_wait_idle();
        // 等上次buf2 copy完了，现在可以用buf2了
        task->dat_src_adr = buf2;
        //  同时预存下一帧，存到buf1
        if ((task->y_src_offset + task->src_h ) >= task->src_h_pre){
            // printf("end");
            return;
        }

        u8 * p = (u8*)tsk_info->ff_info + lut_tab_size + stride * (task->y_src_offset +tsk_info->rotate_chunk);
        if ((task->y_src_offset + task->src_h +tsk_info->rotate_chunk) < task->src_h_pre) {
            // res_fread(file, tsk_info->rotate_buffer, stride * (task->src_h + 1));//为了解决分块输入导致的相位问题，非最后一个分块需要多读取一行像素数据
            dma_memcpy(buf1, p, stride * (task->src_h + 1));
        } else {
            // res_fread(file, tsk_info->rotate_buffer, stride * task->src_h);
            dma_memcpy(buf1, p, stride * task->src_h);
            
        }
    } else if(buf2==task->dat_src_adr) {
        // 上一次显示的为buf2，上一次拷贝的是buf1
        dma_memcpy_wait_idle();
        // 等上次buf1 copy完了，现在可以用buf1显示了
        task->dat_src_adr = buf1;
        //  同时预存下一帧，存到buf2
        if ((task->y_src_offset + task->src_h ) >= task->src_h_pre){
            // printf("end");
            return;
        }

        u8 * p = (u8*)tsk_info->ff_info + lut_tab_size + stride * (task->y_src_offset + tsk_info->rotate_chunk);
        if ((task->y_src_offset + task->src_h + tsk_info->rotate_chunk) < task->src_h_pre ) {
            // res_fread(file, tsk_info->rotate_buffer, stride * (task->src_h + 1));//为了解决分块输入导致的相位问题，非最后一个分块需要多读取一行像素数据
            dma_memcpy(buf2, p, stride * (task->src_h + 1));
        } else {
            // res_fread(file, tsk_info->rotate_buffer, stride * task->src_h);
            dma_memcpy(buf2, p, stride * task->src_h);
            
        }
    }
}
/*----------------------------------------------------------------------------*/
/**@brief    创建图片旋转任务(图片分块拷贝到sram作为输入)
   @param    task_id : 任务id
   @param    file:图片信息
   @param    file_info:图片存储信息
   @param    cx:图片的旋转中心x坐标，相对于图片
   @param    cy:图片的旋转中心y坐标，相对于图片
   @param    angle:旋转角度
   @param    dx:输出的旋转中心点x坐标，相对于背景
   @param    dy:输出的旋转中心点y坐标，相对于背景
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_image_rotate_chunk_task(int task_id, struct image_file *file, u32 priv, int cx, int cy, int angle, int dx, int dy, int load_mef)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.rotate_en = 1;//旋转使能
    if (imb_tsk_info.rotate_en) {
        imb_tsk_info.x_src_offset   = 0;
        imb_tsk_info.y_src_offset   = 0;
        imb_tsk_info.rotate_c_x     = cx;
        imb_tsk_info.rotate_c_y     = cy;
        imb_tsk_info.rotate_angle   = angle;
        imb_tsk_info.rotate_dx      = dx;
        imb_tsk_info.rotate_dy      = dy;
        imb_tsk_info.rotate_chunk   = 1;

        // printf("rotate:>>  %d  %d  %d  %d  %d  %d  %d",
        //     imb_tsk_info.x_src_offset   ,
        //     imb_tsk_info.y_src_offset   ,
        //     imb_tsk_info.rotate_c_x     ,
        //     imb_tsk_info.rotate_c_y     ,
        //     imb_tsk_info.rotate_angle   ,
        //     imb_tsk_info.rotate_dx      ,
        //     imb_tsk_info.rotate_dy               
        // );

        if (imb_tsk_info.rotate_chunk) {
            imb_tsk_info.rotate_chunk_offset = 0;
            u8 chunk_num = 4;//20;//输入分块数量(分块数量越少，合成速度越快，消耗ram越多)
            imb_tsk_info.rotate_chunk = (file->height / chunk_num + 9) / 10 * 10;//每个分块高度
            imb_tsk_info.rotate_chunk_num = (file->height + imb_tsk_info.rotate_chunk - 1) / imb_tsk_info.rotate_chunk;
        }
    }

    imb_tsk_info.data_src       = DATA_SRC_SRAM;
    imb_tsk_info.cur_in_flash   = 1;
    imb_tsk_info.zip_en         = 0;

    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.x_offset       = 0;
    imb_tsk_info.y_offset       = 0;
    imb_tsk_info.des_w          = file->width;
    imb_tsk_info.des_h          = file->height;
    imb_tsk_info.ff_info        = (u32)priv;
    imb_tsk_info.priv           = (u32)file;

    if (imb_tsk_info.in_format == PIXEL_FMT_L1) {
        imb_tsk_info.l1_type        = L1_IMAGE;
    }

    if(load_mef == LOAD_FROM_FLASH_WITHOUT_TAB){
        imb_tsk_info.cb         = imb_rotate_chunk_cb; //注释该句则跑内部的分块流程 
    }

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}

static void *imb_jpeg_cb(void *priv)
{
    struct imb_task_out *out = (struct imb_task_out *)priv;
    struct jpeg_decoder_fd *jpg_fd;
    struct rect *jpeg_page_rect = out->priv;
    struct rect *cur_draw = &out->rect;
    struct rect jpeg_rect;
    struct rect draw_rect;
    int ret;
    struct imb_task *task = out->task;

    if (task->task_en == false) {
        printf("jpeg task_en == false\n");
        return NULL;
    }

    jpg_fd = jpeg_dec_get_fd();

    if (jpg_fd && (jpg_fd->fat_info != task->flash_file_info)) {
        printf("redec_init\n");
        imb_task_lock();
        jpeg_dec_init(task->flash_file_info, out->format, NULL);
        imb_task_unlock();
    }

    struct rect r;

    if (!get_rect_cover(jpeg_page_rect, cur_draw, &r)) {
        printf("jpeg not in area\n");
        return NULL;
    }

    jpeg_rect.left = r.left - jpeg_page_rect->left;
    jpeg_rect.top = r.top - jpeg_page_rect->top;
    jpeg_rect.width = r.width;
    jpeg_rect.height = r.height;

    if ((jpeg_rect.left < 0) || (jpeg_rect.top < 0)
        || (jpeg_rect.left + jpeg_rect.width > jpg_fd->info.x)
        || (jpeg_rect.top + jpeg_rect.height > jpg_fd->info.y)) {
        printf("\nerr_draw_jpeg %d, %d, %d, %d, %d, %d\n", jpeg_rect.left, jpeg_rect.top, jpeg_rect.width, jpeg_rect.height, jpg_fd->info.x, jpg_fd->info.y);
        return NULL;
    }


    imb_task_lock();
    ret = jpeg_dec_start(cur_draw, &r, &jpeg_rect, out->outbuf);
    imb_task_unlock();

    return NULL;
}

/*----------------------------------------------------------------------------*/
/**@brief    创建jpeg图片任务
   @param    task_id : 任务id
   @param    file : 图片信息
   @param    priv : LOAD_FROM_FLASH_WITH_TAB时为flash_file_info, 其他为图像数据的地址
   @param    lcd_format : 屏接口的格式，这里要与屏接口的格式保持一致
   @param    x:相对背景的x坐标
   @param    y:相对背景的y坐标
   @param    width:图片的宽度
   @param    height:图片的高度
   @param    load_method : 数据的加载方式, 具体参考enum LOAD_METHOD
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_jpeg_task(int task_id, struct image_file *file, u32 priv, int lcd_format, int x, int y, int width, int height, struct rect *crop, int load_method)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    if (load_method == LOAD_FROM_FLASH_WITH_TAB) {
        imb_tsk_info.data_src = DATA_SRC_FLASH;//直接从flash取数
        imb_tsk_info.cur_in_flash = 1;//数据存在flash
    } else {
        imb_tsk_info.data_src = DATA_SRC_SRAM;
        imb_tsk_info.cur_in_flash = 0;
    }
    imb_tsk_info.in_format      = file->format;
    imb_tsk_info.x_offset       = x;
    imb_tsk_info.y_offset       = y;
    imb_tsk_info.src_w          = file->width;
    imb_tsk_info.src_h          = file->height;
    imb_tsk_info.des_w          = crop->width;
    imb_tsk_info.des_h          = crop->height;
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.lcd_format     = lcd_format;
    imb_tsk_info.cb             = imb_jpeg_cb;
    imb_tsk_info.quote          = 1;
    imb_tsk_info.ff_info        = priv;
    imb_tsk_info.priv           = (u32)file;

    //裁剪区域
    if (crop) {
        imb_tsk_info.crop.left = crop->left;
        imb_tsk_info.crop.top = crop->top;
        imb_tsk_info.crop.width = crop->width;
        imb_tsk_info.crop.height = crop->height;
    }


    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info); //imb_task参数重置
    } else {
        p = imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);

        struct rect jpeg_page_rect;
        jpeg_page_rect.left = x;
        jpeg_page_rect.top = y;
        jpeg_page_rect.width = width;
        jpeg_page_rect.height = height;

        memcpy(p->priv_buf, &jpeg_page_rect, sizeof(struct rect));
        /* memcpy((u8 *)p->priv_buf + sizeof(struct rect), &draw, sizeof(struct rect)); */
        p->cb_priv = p->priv_buf;
    }
}

// //  BUF显示区域    图片信息+资源组号     图片绝对区域+wh缩放       ID  BUF SIZE    剪切区域
// int imb_image_draw(struct rect rect, struct image_file file, int ui_group_n, 
//     const lv_area_t * coords, int prw, int prh, u16 angle,int dx, int dy,
//     void *src, void *buf, int bufsize, struct rect *crop, int load_mef)
// {
//     int px = coords->x1; //图片绝对区域x
//     int py = coords->y1; //图片绝对区域y
//     int pw = coords->x2-coords->x1+1; //图片绝对区域w
//     int ph = coords->y2-coords->y1+1;  //图片绝对区域h

//     struct lcd_info info = {0};
//     static struct bg_buffer bg = {0};
//     if (__this->lcd->get_screen_info) {
//         __this->lcd->get_screen_info(&info);
//     }

//     if(PIXEL_FMT_JPEG == file.format){  //JPEG图片
//         // printf("JPGKPG");
//         // imb_create_jpeg_task(imb_task_get_id(), &file, &ui_res_gropu[ui_group_n].flash_file_info, info.color_format, 
//         //     px,py,file.width,file.height, 
//         //     crop, load_mef);

//         struct flash_file_info *flash_info = NULL;
//         static struct jpeg_api_out_lines out = {0};
//         struct image_file *f = &file;
//         u32 filelen;
//         u32 flash_map_tab_num;
//         u32 file_base_addr;
//         static struct flash_file_info g_jpg_flash_info = {0};
//         flash_info = &g_jpg_flash_info;

//         imb_task_lock();

//         filelen = f->len;
//         flash_info->tab = &ui_res_gropu[ui_group_n].flash_file_info.tab[(f->offset + ui_res_gropu[ui_group_n].flash_file_info.offset) / 4096];
//         file_base_addr = flash_info->tab[0] + (f->offset + ui_res_gropu[ui_group_n].flash_file_info.offset) % 4096;
//         flash_map_tab_num = (file_base_addr % 4096 + filelen + 4096 - 1) / 4096;
//         flash_info->offset = file_base_addr % 4096;
//         flash_info->tab_size = flash_map_tab_num * sizeof(u32);
//         flash_info->last_tab_data_len = flash_map_tab_num * 4096 - filelen - flash_info->offset;

//         imb_task_unlock();

//         struct jpeg_decoder_fd *jpg_fd = jpeg_dec_get_fd();

//         int ref_w = rect.width;
//         static int last_w;
//         //  这里加判断可以节约初始化的时间，但目前找不到相关失效的条件
//         if(jpg_fd && jpg_fd->fat_info != flash_info || last_w != ref_w || 1){    
//             last_w = ref_w;
//             u8 rgb_format = 1;//1 :rgb_565 0:rgb_888
//             // printf("jpg init");
//             imb_task_lock();
//             char err = jpeg_dec_init(flash_info, rgb_format, NULL);
//             imb_task_unlock();
//         }       

//         struct rect jpeg_rect;
//         int width = jpg_fd->info.x;//jpeg图片本身的宽度
//         int heigh = jpg_fd->info.y;//jpeg图片本身的高度

//         //jpeg图片原本的范围，相对于背景
//         out.jpeg_page_rect.left = coords->x1;
//         out.jpeg_page_rect.top = coords->y1;
//         out.jpeg_page_rect.width = width;
//         out.jpeg_page_rect.height = heigh;

//         //需要合成的范围，该范围必须在jpeg范围内，相对于背景
//         memcpy(&out.control_draw, &out.jpeg_page_rect, sizeof(struct rect));

//         out.cur_draw.left = rect.left;
//         out.cur_draw.top = rect.top;
//         out.cur_draw.width = rect.width;//屏幕的宽
//         out.cur_draw.height = rect.height;//屏幕推一次的高

//         out.outbuf = buf;

//         // int a = jiffies_msec();
//         jpeg_api_read_lines(NULL, &out);
//         return 1;
//     }

// // if(ui_res_gropu[ui_group_n].mode == PSRAM_IMG && !PSRAM_FULL_SCREEN){
// //     return 1;
// // }

//     //以BUF左上角为起始
//     int xoffset = rect.left;
//     int yoffset = rect.top;
//     px = px - xoffset;
//     py = py - yoffset;
//     crop->left -= xoffset;
//     crop->top -= yoffset;
//     rect.left -= xoffset;
//     rect.top -= yoffset;

//     int compx = 0;
//     int compy = 0;

//     printf("dxy %d %d xyoffset %d %d",dx,dy, xoffset, yoffset);

//     //  旋转的圆心负坐标需要特殊处理
//     if(dy-yoffset<0){
//         compy = dy-yoffset;
//     }
//     if(dx-xoffset<0){
//         compx = dx-xoffset;
//     }


//     struct imb_task_info imb_tsk_info = {0};
//     imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
//     imb_tsk_info.zip_en         = 0;
//     imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
//     imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;
//     imb_tsk_info.x_offset       = compx;//rect.left;
//     imb_tsk_info.y_offset       = compy;//rect.top;
//     imb_tsk_info.src_w          = rect.width-compx;
//     imb_tsk_info.src_h          = rect.height-compy;
//     imb_tsk_info.des_w          = rect.width-compx;
//     imb_tsk_info.des_h          = rect.height-compy;
//     imb_tsk_info.priv           = 0;//纯色格式，该参数表示颜色0xr5g6b5
//     imb_tsk_info.osd0_in_buf    = 1;
//     imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);

//     //1.图片缩放输出测试(其他方式可参考图片任务接口的用法，跟图片任务类似)

//     //  PSRAM截图图片
//     if(ui_res_gropu[ui_group_n].mode == PSRAM_IMG){

//         printf("IMB PSRAM ");
//         struct api_ciner_img_info _sca_img = {0};
//         _sca_img.width   = 454;
//         _sca_img.height  = 454;
//         _sca_img.format  = LAYER_FORMAT_RGB565;
//         _sca_img.img_buf = NULL;//pic->data;
//         // struct api_ciner_img_info *sca_img = &_sca_img;
//         struct api_scale_info *sca = api_get_blk_sca_info2(&g_sca_hdl[0], 454, 454, 
//             (float)prw/(float)LV_IMG_ZOOM_NONE, (float)prw/(float)LV_IMG_ZOOM_NONE, 
//             px+xoffset, py+yoffset, 91, &_sca_img, 1);

//         u8 block_index;
//         int v,h;
//         struct image_file tmp_file = {0};
//         for (v = 0; v < sca->v_block_n; v++) {
//             for (h = 0; h < sca->h_block_n; h++) {
//                 block_index = v * sca->h_block_n + h;
//                 tmp_file.width = sca->src_block[block_index].rect.width;
//                 tmp_file.height = sca->src_block[block_index].rect.height;
//                 tmp_file.format = sca->format;
//                 imb_create_image_scale_task(imb_task_get_id(), &tmp_file, psram_cache2nocache_addr(sca->src_block[block_index].buf), 
//                 sca->ratio_w, sca->ratio_h, 
//                 sca->dst_block[block_index].left -xoffset , sca->dst_block[block_index].top-yoffset, 
//                 sca->dst_block[block_index].width, sca->dst_block[block_index].height, 
//                 NULL, false, LOAD_FROM_STATIC_RAM);
//                 // printf("分块缩放");
//             }
//         }
//     } else {
//         //  普通图片
//         if(LV_IMG_ZOOM_NONE != prw || LV_IMG_ZOOM_NONE != prh ){   //  缩放
//             y_printf("%s%d",__func__,__LINE__);
//             imb_create_image_scale_task(imb_task_get_id(), &file,  &ui_res_gropu[ui_group_n].flash_file_info, 
//                 (float)prw/(float)LV_IMG_ZOOM_NONE, (float)prh/(float)LV_IMG_ZOOM_NONE,
//                 px+dx*(LV_IMG_ZOOM_NONE-prw-128)/LV_IMG_ZOOM_NONE, 
//                 py+dy*(LV_IMG_ZOOM_NONE-prh-128)/LV_IMG_ZOOM_NONE, 
//                 pw*prw/LV_IMG_ZOOM_NONE,ph*prh/LV_IMG_ZOOM_NONE,
//                 crop, true, load_mef);         
//         } 
//         else if(angle){   //  不带TAB不占RAM但速度慢
//             if (file.height*file.width < 120*120*2 && 0) {  
//                 imb_create_image_rotate_task(imb_task_get_id(), &file, &ui_res_gropu[ui_group_n].flash_file_info, 
//                     dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy,
//                     NULL, load_mef
//                 );
//             } else {  //不占RAM速度慢
//                 imb_create_image_rotate_chunk_task(imb_task_get_id(), &file, &ui_res_gropu[ui_group_n].flash_file_info, 
//                     dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy
//                 );
//             }
//         } else{    //      原大小
//             imb_create_image_task(imb_task_get_id(),&file, &ui_res_gropu[ui_group_n].flash_file_info, 
//                 px,py,pw,ph,
//                 crop,true, load_mef);//width,height参数可小于file.width, file.height,若小于则裁剪 
//         }       
//     }
    
//     //打印所有任务信息
//     // imb_task_info_dump(imb_task_head_get(0));

//     //启动合成&推屏
//     struct rect disp;//unused
//     struct rect page_draw = {0};
//     struct rect screen_rect;//屏幕有效显示区域
//     screen_rect.left = 0;
//     screen_rect.top = 0;
//     screen_rect.width = rect.width;
//     screen_rect.height = rect.height;

//     rect.left  = 0;
//     rect.top = 0;

//     u16 bg_stride = (rect.width * 2 + 3) / 4 * 4;
//     u16 colums = rect.width;
//     u16 lines = rect.height;//info.buffer_size / info.buf_num / bg_stride;

//     imb_task_head_set_buf(imb_task_head_get(0), buf, bufsize, rect.width, rect.height, bg_stride, lines, 1);
//     imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);

//     struct imb_cb_interface itf = {0};
//     usr_buf = buf;
//     itf.buf_get = usr_buf_get;
//     itf.buf_check = usr_buf_check;
//     itf.buf_set_idle = usr_buf_set_idle;
//     itf.buf_set_pending = usr_buf_set_pending;
//     itf.buf_wait_finish = usr_buf_wait_finish;
//     itf.draw_area = usr_draw_area;
//     itf.draw_meld_area = usr_draw_meld_area;

//     // int a = jiffies_msec();
//     imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );
//     // int b = jiffies_msec();
//     // printf("%d",b-a);

//     imb_task_all_destroy(imb_task_head_get(0));

//     return 1;
// }


//
int imb_rgb565torgb888(struct rect rect, int bg_width, int bg_height, 
    void *buf, int bufsize, void *out_buf)
{
    // memset(buf, 0xf0, rect.width*rect.height);
    struct lcd_info info = {0};
    static struct bg_buffer bg = {0};
    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB888;
    imb_tsk_info.x_offset       = rect.left;
    imb_tsk_info.y_offset       = rect.top;
    imb_tsk_info.src_w          = rect.width;
    imb_tsk_info.src_h          = rect.height;
    imb_tsk_info.des_w          = rect.width;
    imb_tsk_info.des_h          = rect.height;
    imb_tsk_info.priv           = 0x0000;//纯色格式，该参数表示颜色0xr5g6b5
    imb_tsk_info.osd0_in_buf    = 1;
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);

    struct image_file tmp_file;
    tmp_file.width = bg_width;
    tmp_file.height = bg_height;
    tmp_file.format = LAYER_FORMAT_RGB565;
    imb_create_image_task(imb_task_get_id(), &tmp_file, buf, rect.left, rect.top, tmp_file.width, tmp_file.height, NULL, false, LOAD_FROM_STATIC_RAM);


    //打印所有任务信息
    // imb_task_info_dump(imb_task_head_get(0));

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = info.width;//bg_width;
    screen_rect.height = info.height;//bg_height;

    // imb_set_output_cb(imb_data_output, imb_data_output_wait_finish);
    // printf("buf %d len %d  n %d",buf,bufsize, info.buf_num);

    u16 bg_stride = (bg_width * 2 + 3) / 4 * 4;
    u16 colums = bg_width;
    u16 lines = info.buffer_size / info.buf_num / bg_stride;

    imb_task_head_set_buf(imb_task_head_get(0), out_buf/*info.buffer*/, bufsize/*info.buffer_size*/, bg_width, bg_height, bg_stride, lines, info.buf_num);
    
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
    
    // static u8 temp_buf[240*70*2];

    struct imb_cb_interface itf = {0};
    usr_buf = out_buf;
    itf.buf_get = usr_buf_get;
    itf.buf_check = usr_buf_check;
    itf.buf_set_idle = usr_buf_set_idle;
    itf.buf_set_pending = usr_buf_set_pending;
    itf.buf_wait_finish = usr_buf_wait_finish;
    itf.draw_area = usr_draw_area;
    itf.draw_meld_area = usr_draw_meld_area;
    imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );

    imb_task_all_destroy(imb_task_head_get(0));

    return 1;
}

void imb_jpg_draw(void* res, struct _lv_draw_ctx_t * draw_buf, const lv_area_t * coords, jpg_stream_input cb, lv_area_t * clip_area){
    static struct jpeg_api_info api_info = {0};
        static struct jpeg_api_out_lines out = {0};

    // struct CacheFile* res= readFromCache(src, RES_PATH"ui/temp.bin"); 
    // if(!res || res->len==0){
    //     printf("ccccccccc");
    //     return 0;
    // }

    // printf("JPG_RES %x %d", res->data, res->len);
    struct jpeg_decoder_fd *jpg_fd = jpeg_dec_get_fd();
    
    int ref_w = (draw_buf->buf_area->x2) - (draw_buf->buf_area->x1) +1;
    static int last_w;
    //  这里加判断可以节约初始化的时间，但目前找不到相关失效的条件
    // if(jpg_fd && jpg_fd->fat_info != res || last_w != ref_w){    
        last_w = ref_w;
        u8 rgb_format = 1;//1 :rgb_565 0:rgb_888
        char err = jpeg_dec_init(res, rgb_format, cb);
        // printf("xxxxxx");
    // }       
    if(draw_buf==NULL){
        return;
    }


    struct rect jpeg_rect;

    // jpg_fd = api_info.jpg_fd;
    int width = jpg_fd->info.x;//jpeg图片本身的宽度
    int heigh = jpg_fd->info.y;//jpeg图片本身的高度

    // printf("wh %d %d", width, heigh);

    //jpeg图片原本的范围，相对于背景
    out.jpeg_page_rect.left = coords->x1;
    out.jpeg_page_rect.top = coords->y1;
    out.jpeg_page_rect.width = width;
    out.jpeg_page_rect.height = heigh;

    //需要合成的范围，该范围必须在jpeg范围内，相对于背景
    memcpy(&out.control_draw, &out.jpeg_page_rect, sizeof(struct rect));

    out.control_draw.left = clip_area->x1;
    out.control_draw.top = clip_area->y1;
    out.control_draw.width = clip_area->x2-clip_area->x1+1;
    out.control_draw.height = clip_area->y2-clip_area->y1+1;

    // out.control_draw.width /= 2;
    // out.control_draw.height /= 2;

    out.cur_draw.left = draw_buf->buf_area->x1;
    out.cur_draw.top = draw_buf->buf_area->y1;;
    out.cur_draw.width = (draw_buf->buf_area->x2) - (draw_buf->buf_area->x1) +1;//屏幕的宽
    out.cur_draw.height = (draw_buf->buf_area->y2) - (draw_buf->buf_area->y1) +1;//屏幕推一次的高

    out.outbuf = draw_buf->buf;

    // int a = jiffies_msec();
    jpeg_api_read_lines(&api_info, &out);
    // int b = jiffies_msec();
    // printf("%d", b-a);
}
static u8 *usr_jpg_dec_stream_input2(void *__fat_info, u32 *len, u32 offset_4k);
void lv_draw_imb_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
    const lv_area_t * coords, const uint8_t * src_buf, lv_img_cf_t cf);

static lv_res_t lv_draw_jl_imb_img(struct _lv_draw_ctx_t * draw_buf, const lv_draw_img_dsc_t * draw_dsc,
    const lv_area_t * coords, const void * src)
{
    const lv_img_dsc_t * img_dsc = (const lv_img_dsc_t *)src;

//  判断是否为IMB头
    uint8_t * raw_imb_data = (uint8_t *)img_dsc->data;

    //put_buf(raw_imb_data,16);
    const uint32_t raw_imb_data_size = img_dsc->data_size;
    const uint8_t *imb_signature = "IMB";
    if(memcmp(imb_signature, raw_imb_data, sizeof(imb_signature))){
        return LV_RES_INV;
    }

    int cf = LV_IMG_CF_RAW;

    lv_area_t map_area_rot;
    lv_area_copy(&map_area_rot, coords);
    if(draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE)
    {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&map_area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

        map_area_rot.x1 += coords->x1;
        map_area_rot.y1 += coords->y1;
        map_area_rot.x2 += coords->x1;
        map_area_rot.y2 += coords->y1;
    }

    struct _lv_draw_ctx_t * draw_ctx = draw_buf;



    // printf("map_area_rot:x1 = %d, x2 = %d, y1 = %d, y2 = %d\n", map_area_rot.x1, \
    //     map_area_rot.x2, map_area_rot.y1, map_area_rot.y2);
    // printf("clip_area:x1 = %d, x2 = %d, y1 = %d, y2 = %d\n", draw_ctx->clip_area->x1, \
    //     draw_ctx->clip_area->x2, draw_ctx->clip_area->y1, draw_ctx->clip_area->y2);

    lv_area_t clip_com; /*Common area of mask and coords*/
    bool union_ok;
    union_ok = _lv_area_intersect(&clip_com, draw_ctx->clip_area, &map_area_rot);
        if (union_ok == false)
    {
        // printf("Out of mask. There is nothing to draw so the image is drawn successfully.");
        // draw_cleanup(cdsc);


        return LV_RES_OK;
    }

    const lv_area_t *clip_area_ori = draw_ctx->clip_area;
    draw_ctx->clip_area = &clip_com;
    lv_draw_imb_img_decoded(draw_ctx, draw_dsc, coords, raw_imb_data, cf);
    draw_ctx->clip_area = clip_area_ori;
    
    return LV_RES_OK;
}

u32 get_file_addr(char *path);
static lv_res_t lv_draw_jl_imb_img_by_fs(struct _lv_draw_ctx_t * draw_buf, const lv_draw_img_dsc_t * draw_dsc,
                         const lv_area_t * coords, const void * src){
    u8 *u8_p = src;//  盘符
    char * real_path = NULL;  //  真实路径
    if(u8_p[1] == ':') real_path = &u8_p[2]; 
    u8* lastname = NULL;//  文件名 

    //  查找资源组所属
    int n = -1;   
    if(u8_p[0]>='0' && u8_p[0]<='9'){
        lastname = src;
        n = 0;
    }else{
        for(u8 i=0;i<ARRAY_SIZE(ui_res_gropu);i++){
            if( u8_p[0] == ui_res_gropu[i].letter)   //盘符是否匹配
            {
                lastname = real_path;
                n = i;
                break;
            }
        }
        if(n==-1) {
            printf("没有需要加速的资源返回下一个解码器");
            return LV_RES_INV; //没有需要加速的资源返回下一个解码器
        } 
    }


    //  休眠判断,休眠不能跑硬件2D加速
    if(lcd_sleep_status()){
        g_printf("sleep can't run imb");
        return LV_RES_OK;
    }

    //  BUF地址判断，ARG8565输出BUF不支持，也就是图片改变ALAPHA是不支持的
    if(lv_disp_get_default()->driver->draw_buf->buf_act != draw_buf->buf){
        // printf("buf addr error");
        putchar('e');
        return LV_RES_OK;
    }

    lv_area_t map_area_rot;
    lv_area_copy(&map_area_rot, coords);
    if (draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE)
    {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&map_area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

        map_area_rot.x1 += coords->x1;
        map_area_rot.y1 += coords->y1;
        map_area_rot.x2 += coords->x1;
        map_area_rot.y2 += coords->y1;
    }

    struct _lv_draw_ctx_t * draw_ctx = draw_buf;
    lv_area_t clip_com; /*Common area of mask and coords*/
    bool union_ok;
    union_ok = _lv_area_intersect(&clip_com, draw_ctx->clip_area, &map_area_rot);
    /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
    if (union_ok == false)
    {
        // printf("Out of mask. There is nothing to draw so the image is drawn successfully.");
        // draw_cleanup(cdsc);
        return LV_RES_OK;
    }

    // struct flash_file_info ui_resfile_info;
    // int ret = ui_res_flash_info_get(&ui_resfile_info, lastname, "res");
    // ASSERT(!ret);
    // u32 file_addr = ui_resfile_info.tab[0] + ui_resfile_info.offset;//计算文件的物理地址
    // ui_res_flash_info_free(&ui_resfile_info, "res");

    const lv_area_t *clip_area_ori = draw_ctx->clip_area;
    draw_ctx->clip_area = &clip_com;

    int cf = LV_IMG_CF_RAW;
    void *user_data_ori = draw_buf->user_data;
    void *temp_src;
    if(ui_res_gropu[n].mode == IN_RES_WITH_ID) {
        draw_buf->user_data = 0x1234;   //文件系统标识
        temp_src = src;
        // printf("xxxxxxxxxxxxxx");
        lv_draw_imb_img_decoded(draw_ctx, draw_dsc, coords, temp_src, cf);
    }else if(ui_res_gropu[n].mode == IMG_BIN_JPG_FILE){
        draw_buf->user_data = 0;    //BIN或者JPG图片
        if(!memcmp(lv_fs_get_ext(src), "bin", strlen("bin"))){
            // cf = draw_dsc->;
            // temp_src = 0x42740b0;//
            static char  previous_lastname[50];
            static lv_img_dsc_t img_dst;
            if(strcmp(lastname, previous_lastname)){
                printf("uuuuuq");
                memset(previous_lastname,0,sizeof(previous_lastname));
                strcpy(previous_lastname,lastname);
                img_dst.data = get_file_addr(lastname); //bin非压缩数据只支持连续的，不支持带table表
                memcpy(&(img_dst.header) ,img_dst.data, sizeof(lv_img_header_t));
            }
            lv_draw_imb_img_decoded(draw_ctx, draw_dsc, coords, img_dst.data+4, img_dst.header.cf);
        } else if(!memcmp(lv_fs_get_ext(src), "jpg", strlen("jpg"))) {
            static char  previous_lastname[50];
            static struct flash_file_info jpg_ui_resfile_info;
            if(strcmp(lastname, previous_lastname)){
                printf("uuuuuq");
                memset(previous_lastname,0,sizeof(previous_lastname));
                strcpy(previous_lastname,lastname);
                ui_res_flash_info_free(&jpg_ui_resfile_info, "res");  
                ui_res_flash_info_get(&jpg_ui_resfile_info, lastname, "res");
            } 
            imb_jpg_draw(&jpg_ui_resfile_info, draw_buf, coords, NULL, draw_ctx->clip_area);     
        }
    }
    draw_buf->user_data = user_data_ori;
    
    draw_ctx->clip_area = clip_area_ori;

    return LV_RES_OK;
}
static lv_res_t lv_draw_jl_jpg_img(struct _lv_draw_ctx_t * draw_buf, const lv_draw_img_dsc_t * draw_dsc,
                         const lv_area_t * coords, const void * src){
    const lv_img_dsc_t * img_dsc = (const lv_img_dsc_t *)src;
//  判断是否为JPG头
    uint8_t * raw_jpeg_data = (uint8_t *)img_dsc->data;
    const uint32_t raw_jpeg_data_size = img_dsc->data_size;
    const uint8_t jpg_signature[] = {0xFF, 0xD8, 0xFF};
    if(memcmp(jpg_signature, raw_jpeg_data, sizeof(jpg_signature))){
        return LV_RES_INV;
    }
    
    struct file_index_t jpgres = {0};
    jpgres.addr = raw_jpeg_data;
    jpgres.len  = raw_jpeg_data_size;

    lv_area_t map_area_rot;
    lv_area_copy(&map_area_rot, coords);
    if (draw_dsc->angle || draw_dsc->zoom != LV_IMG_ZOOM_NONE)
    {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&map_area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

        map_area_rot.x1 += coords->x1;
        map_area_rot.y1 += coords->y1;
        map_area_rot.x2 += coords->x1;
        map_area_rot.y2 += coords->y1;
    }

    struct _lv_draw_ctx_t * draw_ctx = draw_buf;
    lv_area_t clip_com; /*Common area of mask and coords*/
    bool union_ok;
    union_ok = _lv_area_intersect(&clip_com, draw_ctx->clip_area, &map_area_rot);
    /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
    if (union_ok == false)
    {
        printf("Out of mask. There is nothing to draw so the image is drawn successfully.");
        // draw_cleanup(cdsc);
        return LV_RES_OK;
    }

    const lv_area_t *clip_area_ori = draw_ctx->clip_area;
    draw_ctx->clip_area = &clip_com;
    // lv_draw_img_decoded(draw_ctx, draw_dsc, coords, cdsc->dec_dsc.img_data, cf);
    
    imb_jpg_draw(&jpgres, draw_buf, coords, usr_jpg_dec_stream_input2, draw_ctx->clip_area);
    draw_ctx->clip_area = clip_area_ori;
    
    return LV_RES_OK;

}
static lv_res_t lv_draw_jl_dma2d_img_file(struct _lv_draw_ctx_t * draw_buf, const lv_draw_img_dsc_t * draw_dsc,
                         const lv_area_t * coords, const void * src){
// //  缩放旋转区域计算
    // lv_draw_jl_imb_img_by_fs();
    return LV_RES_OK;
}
static lv_res_t lv_draw_jl_dma2d_img(struct _lv_draw_ctx_t * draw_buf, const lv_draw_img_dsc_t * draw_dsc,
                         const lv_area_t * coords, const void * src){

    if (draw_dsc->opa <= LV_OPA_MIN)
        return LV_RES_OK;

    //printf("********%s\n", __func__);

    //  VAR类型的资源
    lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RES_INV;
    } else if(src_type == LV_IMG_SRC_FILE){
        //FILE类型走这
        return lv_draw_jl_imb_img_by_fs(draw_buf, draw_dsc,
                         coords, src);
    }

    //  VAR类型的资源
    //  RAW自定义解码器
    const lv_img_dsc_t * img_dsc = (const lv_img_dsc_t *)src;
    lv_img_cf_t cf = img_dsc->header.cf;
    if(cf != LV_IMG_CF_RAW){
        return LV_RES_INV;
    }

    //  同时缩放与旋转判断，IMB不支持同时缩放与旋转
    if(draw_dsc->angle && draw_dsc->zoom != LV_IMG_ZOOM_NONE ){
        printf("no support zoom and angle");
        return LV_RES_OK;   //不支持同时缩放与旋转
    }

    //  休眠判断,休眠不能跑硬件2D加速
    if(lcd_sleep_status()){
        g_printf("sleep can't run imb");
        return LV_RES_OK;
    }

    //  BUF地址判断，ARG8565输出BUF不支持，也就是图片改变ALAPHA是不支持的
    if(lv_disp_get_default()->driver->draw_buf->buf_act != draw_buf->buf){
        // printf("buf addr error");
        putchar('e');
        return LV_RES_OK;
    }

    //  JPG头识别并解码成功接管返回
    // if(LV_RES_OK == lv_draw_jl_jpg_img(draw_buf, draw_dsc, coords, src)){
    //     return LV_RES_OK;
    // }

    //  IMB头识别并解码成功接管返回
    if(LV_RES_OK == lv_draw_jl_imb_img(draw_buf, draw_dsc, coords, src)){
        // printf("IMB");
        return LV_RES_OK;
    }

return LV_RES_INV;
}

/*----------------------------------------------------------------------------*/
/**@brief     创建纯色任务(矩形填充)
   @param     task_id : 任务id
   @param     x : 相对背景的x坐标
   @param     y : 相对背景的y坐标
   @param     width : 矩形的宽度
   @param     height: 矩形的高度
   @param     color : argb8565格式 (alpha 0~100)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void imb_create_color_task(int task_id, int x, int y, int width, int height, u32 color)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;

    imb_tsk_info.elm_id = task_id;
    imb_tsk_info.data_src = DATA_SRC_NONE;
    imb_tsk_info.zip_en = 0;
    imb_tsk_info.in_format = LAYER_FORMAT_SOLID;
    imb_tsk_info.x_offset = x;
    imb_tsk_info.y_offset = y;
    imb_tsk_info.src_w = width;
    imb_tsk_info.src_h = height;
    imb_tsk_info.des_w = width;
    imb_tsk_info.des_h = height;
    imb_tsk_info.priv = color;

    /* elm_id、id(低8位有效)可作为任务的唯一标识，通过该标识搜索任务句柄，若搜到则重置任务信息，没搜到则创建任务 */
    p = imb_task_search_by_id(imb_task_head_get(0), imb_tsk_info.elm_id, imb_tsk_info.id);

    if (p) {
        imb_task_reset(imb_task_head_get(0), p, &imb_tsk_info);
    } else {
        imb_task_list_add(imb_task_head_get(0), &imb_tsk_info);
    }
}
//  1处理成功，0处理失败
int jl_draw_sw_blend_basic(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc){

    // if(dsc->src_buf){
    //     printf("%s %d",__func__,__LINE__);
    //     printf("mode %d opa %d mask %x", dsc->blend_mode, dsc->opa, dsc->mask_buf);
    // }
        

    lv_opa_t * mask;
    if(dsc->mask_buf == NULL) mask = NULL;
    if(dsc->mask_buf && dsc->mask_res == LV_DRAW_MASK_RES_TRANSP) return 1;
    else if(dsc->mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask = NULL;
    else mask = dsc->mask_buf;

    lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area);

    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return 1;

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_color_t * dest_buf = draw_ctx->buf;
    if(disp->driver->set_px_cb == NULL) {
        if(disp->driver->screen_transp) {
            return 0;
        }
    } else {
        return 0;
    }

    const lv_color_t * src_buf = dsc->src_buf;
    lv_coord_t src_stride;
    if(src_buf) {
        src_stride = lv_area_get_width(dsc->blend_area);
        src_buf += src_stride * (blend_area.y1 - dsc->blend_area->y1) + (blend_area.x1 - dsc->blend_area->x1);
    }
    else {
        src_stride = 0;
    }

    lv_coord_t mask_stride = 0;
    if(mask==NULL && dsc->blend_mode == LV_BLEND_MODE_NORMAL){
        if(dsc->src_buf == NULL) {
            // printf("fill_normal");
            //矩形纯色带A填充
            if((dsc->opa >= LV_OPA_MAX)){
                // fill_normal(dest_buf, &blend_area, dest_stride, dsc->color, dsc->opa, mask, mask_stride);
                return 0;
            } else {
                int32_t bw = lv_area_get_width(&blend_area);
                int32_t bh = lv_area_get_height(&blend_area);
                int32_t dw = lv_area_get_width(draw_ctx->buf_area);
                int32_t dh = lv_area_get_height(draw_ctx->buf_area);
                
                struct lcd_info info = {0};
                static struct bg_buffer bg = {0};
                if (__this->lcd->get_screen_info) {
                    __this->lcd->get_screen_info(&info);
                }

                struct imb_task_info imb_tsk_info = {0};
                imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
                imb_tsk_info.zip_en         = 0;
                imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
                imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;
                imb_tsk_info.x_offset       = draw_ctx->buf_area->x1;
                imb_tsk_info.y_offset       = draw_ctx->buf_area->y1;
                imb_tsk_info.src_w          = dw;
                imb_tsk_info.src_h          = dh;
                imb_tsk_info.des_w          = dw;
                imb_tsk_info.des_h          = dh;
                imb_tsk_info.priv           = dsc->color.full;//纯色格式，该参数表示颜色0xr5g6b5
                imb_tsk_info.osd0_in_buf    = 1;
                imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);
                imb_create_color_task(imb_task_get_id(), blend_area.x1, blend_area.y1, bw, bh, dsc->opa*100/256 << 24 | ((dsc->color.full&0xff<<8)+dsc->color.full>>8));

                //启动合成&推屏
                struct rect rect; 
                struct rect disp; //unused
                struct rect page_draw = {0};
                struct rect screen_rect;//屏幕有效显示区域
                screen_rect.left = 0;
                screen_rect.top = 0;
                screen_rect.width = 392;//bg_width;
                screen_rect.height = 392;//bg_height;

                rect.left = draw_ctx->buf_area->x1;
                rect.top = draw_ctx->buf_area->y1;
                rect.width = dw;//bg_width;
                rect.height = dh;//bg_height;

                // imb_set_output_cb(imb_data_output, imb_data_output_wait_finish);
                // printf("buf %d len %d  n %d",buf,bufsize, info.buf_num);

                u16 bg_stride = (dw * 2 + 3) / 4 * 4;
                u16 colums = dw;
                u16 lines = dh;//info.buffer_size / info.buf_num / bg_stride;

                imb_task_head_set_buf(imb_task_head_get(0), draw_ctx->buf/*info.buffer*/, dw*dh*2/*info.buffer_size*/, dw, dh, dw*2, dh, 1);
                
                imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
                
                // static u8 temp_buf[240*70*2];

                struct imb_cb_interface itf = {0};
                usr_buf = draw_ctx->buf;
                itf.buf_get = usr_buf_get;
                itf.buf_check = usr_buf_check;
                itf.buf_set_idle = usr_buf_set_idle;
                itf.buf_set_pending = usr_buf_set_pending;
                itf.buf_wait_finish = usr_buf_wait_finish;
                itf.draw_area = usr_draw_area;
                itf.draw_meld_area = usr_draw_meld_area;
                imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );

                imb_task_all_destroy(imb_task_head_get(0));

                return 1;

            }
            // 

        }
        else {
            // // printf("map_normal");
            // // printf(" %d",dsc->opa);
            // // RAM 矩形合成
            // if((dsc->opa >= LV_OPA_MAX)){
            //     int32_t bw = lv_area_get_width(&blend_area);
            //     int32_t bh = lv_area_get_height(&blend_area);
            //     int32_t dw = lv_area_get_width(draw_ctx->buf_area);
            //     int32_t dh = lv_area_get_height(draw_ctx->buf_area);
                
            //     struct lcd_info info = {0};
            //     static struct bg_buffer bg = {0};
            //     if (__this->lcd->get_screen_info) {
            //         __this->lcd->get_screen_info(&info);
            //     }

            //     struct imb_task_info imb_tsk_info = {0};
            //     imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
            //     imb_tsk_info.zip_en         = 0;
            //     imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
            //     imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;
            //     imb_tsk_info.x_offset       = draw_ctx->buf_area->x1;
            //     imb_tsk_info.y_offset       = draw_ctx->buf_area->y1;
            //     imb_tsk_info.src_w          = dw;
            //     imb_tsk_info.src_h          = dh;
            //     imb_tsk_info.des_w          = dw;
            //     imb_tsk_info.des_h          = dh;
            //     imb_tsk_info.priv           = dsc->color.full;//纯色格式，该参数表示颜色0xr5g6b5
            //     imb_tsk_info.osd0_in_buf    = 1;
            //     imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);

            //     struct image_file tmp_file;
            //     tmp_file.width = bw;
            //     tmp_file.height = bh;
            //     tmp_file.format = LAYER_FORMAT_RGB565;
            //     imb_create_image_task(imb_task_get_id(), &tmp_file, dsc->src_buf, 
            //         blend_area.x1, blend_area.y1, bw, bh, NULL, false, LOAD_FROM_STATIC_RAM);

            //     //启动合成&推屏
            //     struct rect rect; 
            //     struct rect disp; //unused
            //     struct rect page_draw = {0};
            //     struct rect screen_rect;//屏幕有效显示区域
            //     screen_rect.left = 0;
            //     screen_rect.top = 0;
            //     screen_rect.width = 392;//bg_width;
            //     screen_rect.height = 392;//bg_height;

            //     rect.left = draw_ctx->buf_area->x1;
            //     rect.top = draw_ctx->buf_area->y1;
            //     rect.width = dw;//bg_width;
            //     rect.height = dh;//bg_height;

            //     // imb_set_output_cb(imb_data_output, imb_data_output_wait_finish);
            //     // printf("buf %d len %d  n %d",buf,bufsize, info.buf_num);

            //     u16 bg_stride = (dw * 2 + 3) / 4 * 4;
            //     u16 colums = dw;
            //     u16 lines = dh;//info.buffer_size / info.buf_num / bg_stride;

            //     imb_task_head_set_buf(imb_task_head_get(0), draw_ctx->buf/*info.buffer*/, dw*dh*2/*info.buffer_size*/, dw, dh, dw*2, dh, 1);
                
            //     imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);
                
            //     // static u8 temp_buf[240*70*2];

            //     struct imb_cb_interface itf = {0};
            //     usr_buf = draw_ctx->buf;
            //     itf.buf_get = usr_buf_get;
            //     itf.buf_check = usr_buf_check;
            //     itf.buf_set_idle = usr_buf_set_idle;
            //     itf.buf_set_pending = usr_buf_set_pending;
            //     itf.buf_wait_finish = usr_buf_wait_finish;
            //     itf.draw_area = usr_draw_area;
            //     itf.draw_meld_area = usr_draw_meld_area;
            //     imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );

            //     imb_task_all_destroy(imb_task_head_get(0));

            //     return 0;
  

            // }
            // map_normal(dest_buf, &blend_area, dest_stride, src_buf, src_stride, dsc->opa, mask, mask_stride);
            return 0;
        }

        return 0;
    }

    return 0;
}

//  优先IMB接管
void lv_draw_jl_dma2d_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    return;
#if 0
    if(jl_draw_sw_blend_basic( draw_ctx,   dsc) == 1){
        // 预处理OK直接返回
        return;
    }

    //  IMB不支持直接走软件合成
    lv_draw_sw_blend_basic(draw_ctx, dsc);
#endif

}

void jl_gpu_fill(lv_color_t * buf, lv_coord_t dest_stride, lv_area_t * blend_area, lv_color_t color, lv_opa_t opa) {
    // printf("cc blend %d %d %d %d",
    //     blend_area->x1,
    //     blend_area->x2,
    //     blend_area->y1,
    //     blend_area->y2
    // );
    // LVGL的BUF刷新窗口计算
    struct rect rect;//当前刷新区域
    rect.left = 0;
    rect.top = 0;
    rect.width = dest_stride;//(blend_area->x2) - (blend_area->x1) +1;
    rect.height = (blend_area->y2) +1;//- (blend_area->y1) +1;    
    int bufsize = rect.width*rect.height*2;

    //以BUF左上角为起始
    int xoffset = rect.left;
    int yoffset = rect.top;
    rect.left -= xoffset;
    rect.top -= yoffset;
    int compx = 0;
    int compy = 0;

    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;
    imb_tsk_info.x_offset       = compx;
    imb_tsk_info.y_offset       = compy;
    imb_tsk_info.src_w          = rect.width - compx;
    imb_tsk_info.src_h          = rect.height - compy;
    imb_tsk_info.des_w          = rect.width - compx;
    imb_tsk_info.des_h          = rect.height - compy;
    imb_tsk_info.priv           = 0;//纯色格式，该参数表示颜色0xr5g6b5
    imb_tsk_info.osd0_in_buf    = 1;
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);
    
    //  纯色
    imb_create_color_task(imb_task_get_id(), 
                    blend_area->x1, blend_area->y1, 
                    lv_area_get_width(blend_area), 
                    lv_area_get_height(blend_area),
                     (100 - opa*100/255) << 24  | color.full);
    
    //打印所有任务信息
    // imb_task_info_dump(imb_task_head_get(0));

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = rect.width;
    screen_rect.height = rect.height;

    rect.left  = 0;
    rect.top = 0;

    u16 bg_stride = (rect.width * 2 + 3) / 4 * 4;
    u16 colums = rect.width;
    u16 lines = rect.height;//info.buffer_size / info.buf_num / bg_stride;

    imb_task_head_set_buf(imb_task_head_get(0), buf, bufsize, rect.width, rect.height, bg_stride, lines, 1);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);

    struct imb_cb_interface itf = {0};
    usr_buf = buf;
    itf.buf_get = usr_buf_get;
    itf.buf_check = usr_buf_check;
    itf.buf_set_idle = usr_buf_set_idle;
    itf.buf_set_pending = usr_buf_set_pending;
    itf.buf_wait_finish = usr_buf_wait_finish;
    itf.draw_area = usr_draw_area;
    itf.draw_meld_area = usr_draw_meld_area;

    int a = jiffies_msec();
    imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );
    int b = jiffies_msec();
    // printf("%d",b-a);

    imb_task_all_destroy(imb_task_head_get(0));

    return ;    
}

void my_draw_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc)
{
    /*Let's get the blend area which is the intersection of the area to fill and the clip area.*/
    lv_area_t blend_area;
    if(!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;  /*Fully clipped, nothing to do*/

    /*Fill only non masked, fully opaque, normal blended and not too small areas*/
    if(dsc->src_buf == NULL && dsc->mask_buf == NULL && dsc->opa < LV_OPA_MAX && dsc->opa > LV_OPA_MIN &&  /* 这里改成透明度经过IMB合成，非透明走软件memcpy合成速度更快 */
       dsc->blend_mode == LV_BLEND_MODE_NORMAL && lv_area_get_size(&blend_area) > 100) {

        /*Got the first pixel on the buffer*/
        lv_coord_t dest_stride = lv_area_get_width(draw_ctx->buf_area); /*Width of the destination buffer*/
        lv_color_t * dest_buf = draw_ctx->buf;
        // dest_buf += dest_stride * (blend_area.y1 - draw_ctx->buf_area->y1);// + (blend_area.x1 - draw_ctx->buf_area->x1);

        /*Make the blend area relative to the buffer*/      
        lv_area_move(&blend_area, -draw_ctx->buf_area->x1, -draw_ctx->buf_area->y1);
       
        /*Call your custom gou fill function to fill blend_area, on dest_buf with dsc->color*/  
        jl_gpu_fill(dest_buf, dest_stride, &blend_area, dsc->color, dsc->opa);

        printf("**********%s\n", __func__);
    }
    /*Fallback: the GPU doesn't support these settings. Call the SW renderer.*/
    else {
      lv_draw_sw_blend_basic(draw_ctx, dsc);
    }
}


void flash_tab_info_release(struct flash_file_info *file_info);
void lv_draw_imb_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
    const lv_area_t * coords_src, const uint8_t * src_buf, lv_img_cf_t cf)
{
    //  转成非CONST变量
    lv_area_t coords_temp = {0};
    lv_area_t  *coords;
    coords = &coords_temp;
    memcpy(coords, coords_src, sizeof(lv_area_t)); 

    struct _lv_draw_ctx_t * draw_buf = draw_ctx;
    int px = coords->x1; //图片绝对区域x
    int py = coords->y1; //图片绝对区域y
    int pw = coords->x2-coords->x1+1; //图片绝对区域w
    int ph = coords->y2-coords->y1+1;  //图片绝对区域h
    // printf("x = %d, y = %d, w = ")
    //  支持的
#if 0
    if(((LV_IMG_CF_TRUE_COLOR == cf || LV_IMG_CF_TRUE_COLOR_ALPHA == cf) && (src_buf >= PSRAM_CACHE_ADDR))
        || LV_IMG_CF_RAW == cf) /* 压缩图片 */
    {

    }else 
    {
        lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);
        return;        
    }
#else 
    if(LV_IMG_CF_RAW != cf) /* 压缩图片 */ 
    {
        lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);

        return;        
    }
#endif

#if 0
    //  但不支持
    if(0|| lv_disp_get_default()->driver->draw_buf->buf_act != draw_buf->buf /* ALPHA输出不支持 */
        || (draw_dsc->angle  && draw_dsc->zoom != LV_IMG_ZOOM_NONE)  /* 同时缩放旋转 */
        || ((pw > 240 || ph >240) && draw_dsc->zoom != LV_IMG_ZOOM_NONE) /* 缩放超过240*240 */
    )
    {
        if(cf == LV_IMG_CF_RAW) return; //RAW自定义格式LV软件不支持的，直接返回
        //LV格式的资源imb不支持的格式走LV软件合成
        lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);
        return;
    }
#endif

    //  IMB合成
    //  图片裁剪区域
    lv_area_t res_p={0};
    if(0 == (_lv_area_intersect(&res_p, draw_buf->buf_area, draw_buf->clip_area)))
    {
        printf("lv no intersect buf and clip");
        return;   //没有剪切区域
    }  

#if 0
    //  进行对齐处理
    if(LV_IMG_CF_TRUE_COLOR == cf){
        pw = pw+pw%2;
    }else if(LV_IMG_CF_TRUE_COLOR_ALPHA == cf){
        pw = (pw+3)/4*4;
    }
#endif  

    // 初始化imb的file信息
    struct image_file file = {0};
    int ui_group_n;
    file.compress = 0;
    struct flash_file_info file_info={0};

    if(memcmp(src_buf, "IMB", sizeof("IMB")) != 0)
    {
        printf("no IMB data");
        return;
    }
    memcpy(&file, (u8*)src_buf+4, sizeof(file));

    // printf("compress %d data_crc %d format %d width %d height %d len %d offse %d ulen %d uoffs %d",
    //     file.compress, file.data_crc, file.format, file.width, file.height, file.len, file.offset, file.unzipLen, file.unzipOffset);

#if 0
    if(LV_IMG_CF_TRUE_COLOR == cf){
        file.format = PIXEL_FMT_RGB565;
        file.width  = pw;
        file.height = ph;
        // printf("RGB");
    }else if(LV_IMG_CF_TRUE_COLOR_ALPHA == cf){
        file.format = PIXEL_FMT_ARGB8565;
        file.width  = pw;
        file.height = ph;
        // printf("ARGB");
    }else if(LV_IMG_CF_RAW == cf){
        //压缩资源，构建表
        if(0x1234 == draw_ctx->user_data){  //imb文件系统
            ui_group_n = get_jl_img_info_by_group_incache(src_buf, &file, NULL);
            if(ui_group_n  == -1){
                printf("ui_group_n ==-1");
                return ;
            }
        } else {    //  自定义地址跑这
            if(memcmp(src_buf, "IMB", sizeof("IMB"))){
                printf("no IMB data");
                return;
            }
            memcpy(&file, (u8*)src_buf+4, sizeof(file));
        }

        // printf("compress %d data_crc %d format %d height %d len %d offse %d ulen %d uoffs %d width %d",
        //     file.compress,
        //     file.data_crc,
        //     file.format,
        //     file.height,
        //     file.len,
        //     file.offset,
        //     file.unzipLen,
        //     file.unzipOffset,
        //     file.width
        // );
    }
#endif

    void * src_addr = NULL;
    int load_mef = LOAD_FROM_FLASH_WITH_TAB; 
    if(-1 == flash_tab_info_get(&file_info, src_buf+28, file.len)){
        printf("flash_tab_info_get no mem");
        return ;
    }
    src_addr = &file_info;
#if 0
    //  加载资源方式
    int load_mef = -1;
    void * src_addr = NULL;
    if(LV_IMG_CF_RAW == cf){
        //IMB压缩资源，构建表
        if(0x1234 == draw_ctx->user_data){  //imb文件系统
            load_mef = LOAD_FROM_FLASH_WITH_TAB; 
            src_addr = &ui_res_gropu[ui_group_n].flash_file_info;
        } else {
            load_mef = LOAD_FROM_FLASH_WITH_TAB; 
            if(-1 == flash_tab_info_get(&file_info, src_buf+28, file.len)){
                printf("flash_tab_info_get no mem");
                return ;
            }
            src_addr = &file_info;
        }
    } else if(src_buf >= 0x6000000){   //内挂flash
        load_mef = LOAD_FROM_FLASH_WITHOUT_TAB;
        src_addr = src_buf;
    }else if(src_buf >= 0x4000000){ //外挂flash
        load_mef = LOAD_FROM_FLASH_WITHOUT_TAB;
        src_addr = src_buf;
    }else if(src_buf >= PSRAM_CACHE_ADDR + PSRAM_MAPPING_SIZE) { //PSRAM DMA地址
        load_mef = LOAD_FROM_STATIC_RAM;
        src_addr = src_buf;
    }else if(src_buf >= PSRAM_CACHE_ADDR){  //PSRAM cache地址
        load_mef = LOAD_FROM_STATIC_RAM;  
        src_addr = psram_cache2nocache_addr(src_buf);
    }else{
        // load_mef = LOAD_FROM_STATIC_RAM;
        // src_addr = src_buf;
        //imb不支持的格式走软件合成
        // lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);
        return;
    }
#endif

    //printf("IMB DECORD %x", src_addr);

    // LVGL的BUF刷新窗口计算
    struct rect rect;//当前刷新区域
    rect.left = draw_buf->buf_area->x1;
    rect.top = draw_buf->buf_area->y1;
    rect.width = (draw_buf->buf_area->x2) - (draw_buf->buf_area->x1) +1;
    rect.height = (draw_buf->buf_area->y2) - (draw_buf->buf_area->y1) + 1;
    // printf("left = %d, top = %d, w = %d, h = %d", rect.left, rect.top, rect.width, rect.height);
    
    //  图片相关参数    
    int prw =  draw_dsc->zoom;
    int prh =  draw_dsc->zoom; //XY缩放
    u16 angle = draw_dsc->angle; //旋转角度
    int dx = draw_dsc->pivot.x;
    int dy = draw_dsc->pivot.y;   //旋转或缩放中心
    void *buf = draw_buf->buf;  //当前合成输出的BUF
    int bufsize = rect.width*rect.height*2; //当前合成输出BUF长度
    struct rect *crop = image_set_crop( res_p.x1, res_p.y1, res_p.x2-res_p.x1+1, res_p.y2-res_p.y1+1 );       

    //以BUF左上角为起始
    int xoffset = rect.left;
    int yoffset = rect.top;
    px = px - xoffset;
    py = py - yoffset;
    crop->left -= xoffset;
    crop->top -= yoffset;
    rect.left -= xoffset;
    rect.top -= yoffset;
    int compx = 0;
    int compy = 0;
    if(dy-yoffset<compy){//  旋转的圆心负坐标需要特殊处理
        compy = dy-yoffset;
    }
    if(dx-xoffset<compx){
        compx = dx-xoffset;
    }
    if(py+dy<compy){
        compy = py+dy;
    }    
    if(px+dx<compx){
        compx = px+dx;
    }

    int ss = TICK_CNT;
    int imb_task_get_id = 0;

    // printf("compx = %d, compy = %d\n", compx, compy);
    // printf("w = %d, h = %d\n", rect.width, rect.height);
    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;
    imb_tsk_info.x_offset       = compx;
    imb_tsk_info.y_offset       = compy;
    imb_tsk_info.src_w          = rect.width - compx;
    imb_tsk_info.src_h          = rect.height - compy;
    imb_tsk_info.des_w          = rect.width - compx;
    imb_tsk_info.des_h          = rect.height - compy;
    imb_tsk_info.priv           = 0;//纯色格式，该参数表示颜色0xr5g6b5
    imb_tsk_info.osd0_in_buf    = 1;
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);
    
    //  普通图片
    if(LV_IMG_ZOOM_NONE != prw || LV_IMG_ZOOM_NONE != prh)//缩放
    {  
        imb_create_image_scale_task(++imb_task_get_id, &file, src_addr, 
            (float)prw/(float)LV_IMG_ZOOM_NONE, (float)prh/(float)LV_IMG_ZOOM_NONE,
            px+dx*(LV_IMG_ZOOM_NONE-prw)/LV_IMG_ZOOM_NONE, 
            py+dy*(LV_IMG_ZOOM_NONE-prh)/LV_IMG_ZOOM_NONE, 
            pw*prw/LV_IMG_ZOOM_NONE,ph*prh/LV_IMG_ZOOM_NONE,
            NULL, file.compress, load_mef);
#if 0 
        imb_create_image_scale_task(imb_task_get_id(), &file, src_addr, 
            (float)prw/(float)LV_IMG_ZOOM_NONE, (float)prh/(float)LV_IMG_ZOOM_NONE,
            px+dx*(LV_IMG_ZOOM_NONE-prw)/LV_IMG_ZOOM_NONE, 
            py+dy*(LV_IMG_ZOOM_NONE-prh)/LV_IMG_ZOOM_NONE, 
            pw*prw/LV_IMG_ZOOM_NONE,ph*prh/LV_IMG_ZOOM_NONE,
            NULL, file.compress, load_mef);
#endif         
    }else if(angle)//旋转
    {  
        imb_create_image_rotate_task(++imb_task_get_id, &file, src_addr, 
            dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy, NULL, load_mef);
        // imb_create_image_rotate_chunk_task(imb_task_get_id(), &file, src_addr, 
        //     dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy, load_mef); 
#if 0
        if (file.height*file.width < 120*120*2 && 0) {  //占RAM速度快120*120图以内不分快
            imb_create_image_rotate_task(imb_task_get_id(), &file, src_addr, 
                dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy,
                NULL, load_mef
            );
        } else {  //不占RAM速度慢
            // printf("YYYY %d h %d",py+dy-compy, rect.height);
            imb_create_image_rotate_chunk_task(imb_task_get_id(), &file, src_addr, 
                dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy, load_mef
            );
        }
#endif
    }else
    {    //      原大小
        // imb_create_image_task(++imb_task_get_id, &file, src_addr, 
        //     px, py, pw, ph, crop, file.compress, load_mef);//width,height参数可小于file.width, file.height,若小于则裁剪 

        // printf("px = %d, py = %d\n", px, py);
        // printf("pw = %d, ph = %d\n", pw, ph);
        imb_create_image_task(++imb_task_get_id, &file, src_addr, 
            px, py, pw, ph, crop, file.compress, load_mef);//width,height参数可小于file.width, file.height,若小于则裁剪 
    }       
    
    //打印所有任务信息
    // imb_task_info_dump(imb_task_head_get(0));

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = rect.width;
    screen_rect.height = rect.height;

    rect.left  = 0;
    rect.top = 0;

    
    u16 bg_stride = (rect.width * 2 + 3) / 4 * 4;

    imb_task_head_set_buf(imb_task_head_get(0), buf, NULL, rect.width, rect.height, bg_stride, rect.height, NULL);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);

    struct imb_cb_interface itf = {0};
    usr_buf = buf;
    itf.buf_get = usr_buf_get;
    itf.buf_check = usr_buf_check;
    itf.buf_set_idle = usr_buf_set_idle;
    itf.buf_set_pending = usr_buf_set_pending;
    itf.buf_wait_finish = usr_buf_wait_finish;
    itf.draw_area = usr_draw_area;
    itf.draw_meld_area = usr_draw_meld_area;

    
    int a = jiffies_half_msec();// TICK_CNT;
    int aa = TICK_CNT;//jiffies_half_msec();
    imb_start(imb_task_head_get(0), &screen_rect, disp, rect.width, rect.height, &itf);
    // extern void imb_task_kick(struct imb_task_head *root, struct rect *limit, struct rect disp, u16 colums, u16 lines, struct imb_cb_interface *itf);
    // ____imb_task_kick(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );
    int bb = TICK_CNT;//jiffies_half_msec();
    int b = jiffies_half_msec();// TICK_CNT;
// TICK_CNT
    //  dump_imb_reg(imb_task_head_get(0));

    imb_task_all_destroy(imb_task_head_get(0));
    flash_tab_info_release(&file_info);

    int ee = TICK_CNT;

    // printf(" %d %d %d", aa-ss, bb-aa, ee-bb);
#if 0
    u16 bg_stride = (rect.width * 2 + 3) / 4 * 4;
    u16 colums = rect.width;
    u16 lines = rect.height;//info.buffer_size / info.buf_num / bg_stride;

    imb_task_head_set_buf(imb_task_head_get(0), buf, bufsize, rect.width, rect.height, bg_stride, lines, 1);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);

    struct imb_cb_interface itf = {0};
    usr_buf = buf;
    itf.buf_get = usr_buf_get;
    itf.buf_check = usr_buf_check;
    itf.buf_set_idle = usr_buf_set_idle;
    itf.buf_set_pending = usr_buf_set_pending;
    itf.buf_wait_finish = usr_buf_wait_finish;
    itf.draw_area = usr_draw_area;
    itf.draw_meld_area = usr_draw_meld_area;

    int a = jiffies_msec();
    imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf);
    int b = jiffies_msec();
    //printf("%s:%d",__func__, b-a);

    imb_task_all_destroy(imb_task_head_get(0));

    flash_tab_info_release(&file_info);
#endif

    return ;
    
}

//解码IMB压缩图片的数据（支持IMB，除了JPEG）
void lv_draw_imb_img_decoded2(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                                                  const lv_area_t * coords, const uint8_t * src_buf, lv_img_cf_t cf)
{
    if(0 || !(LV_IMG_CF_TRUE_COLOR == cf || LV_IMG_CF_TRUE_COLOR_ALPHA == cf) /* 格式不支持 */
        || LV_COLOR_16_SWAP == 0    /* 非SWAP */
        /* || lv_disp_get_default()->driver->draw_buf->buf_act != draw_buf->buf */
        || (draw_dsc->angle && draw_dsc->zoom != LV_IMG_ZOOM_NONE)  /* 同时缩放旋转 */
        || ((coords->x2-coords->x1-1 > 240 || coords->y2-coords->y1 >240) && draw_dsc->zoom != LV_IMG_ZOOM_NONE) /* 缩放超过240*240 */
    )
    {
        //imb不支持的格式走软件合成
        lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);
        return;
    }

    struct image_file file = {0};
    if(LV_IMG_CF_TRUE_COLOR == cf){
        file.format = PIXEL_FMT_RGB565;
        // printf("RGB");
    }else if(LV_IMG_CF_TRUE_COLOR_ALPHA == cf){
        file.format = PIXEL_FMT_ARGB8565;
        // printf("ARGB");
    }else if(LV_IMG_CF_RAW == cf){
        //压缩，构建表
    }

    struct _lv_draw_ctx_t * draw_buf = draw_ctx;
    int load_mef = -1;
    void * src_addr = NULL;
    if(src_buf >= 0x6000000){   //内挂flash
        load_mef = LOAD_FROM_FLASH_WITHOUT_TAB;
        src_addr = src_buf;
    }else if(src_buf >= 0x4000000){ //外挂flash
        load_mef = LOAD_FROM_FLASH_WITHOUT_TAB;
        src_addr = src_buf;
    }else if(src_buf >= PSRAM_CACHE_ADDR + PSRAM_MAPPING_SIZE) { //PSRAM DMA地址
        load_mef = LOAD_FROM_STATIC_RAM;
        src_addr = src_buf;
    }else if(src_buf >= PSRAM_CACHE_ADDR){  //PSRAM cache地址
        load_mef = LOAD_FROM_STATIC_RAM;  
        src_addr = psram_cache2nocache_addr(src_buf);
    }else{
        //imb不支持的格式走软件合成
        lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);
        return;
    }
    
    //  IMB合成
    //  图片裁剪区域
    lv_area_t  res_p={0};
    if(0==(_lv_area_intersect(&res_p, draw_buf->buf_area, draw_buf->clip_area))){
        printf("lv no intersect buf and clip");
        // lv_draw_sw_img_decoded(draw_ctx, draw_dsc,coords, src_buf, cf);
        return ;   //没有剪切区域
    }  

    // printf("IMB DECORD %x", src_addr);

    // LVGL的BUF刷新窗口计算
    struct rect rect;//当前刷新区域
    rect.left = draw_buf->buf_area->x1;
    rect.top = draw_buf->buf_area->y1;
    rect.width = (draw_buf->buf_area->x2) - (draw_buf->buf_area->x1) +1;
    rect.height = (draw_buf->buf_area->y2) - (draw_buf->buf_area->y1) +1;
    
    //  图片相关参数    
    int prw =  draw_dsc->zoom;
    int prh =  draw_dsc->zoom; //XY缩放
    u16 angle = draw_dsc->angle; //旋转角度
    int dx = draw_dsc->pivot.x;
    int dy = draw_dsc->pivot.y;   //旋转或缩放中心
    void *buf = draw_buf->buf;  //当前合成输出的BUF
    int bufsize = rect.width*rect.height*2; //当前合成输出BUF长度
    struct rect *crop = image_set_crop( res_p.x1, 
                                res_p.y1, 
                                res_p.x2-res_p.x1+1, 
                                res_p.y2-res_p.y1+1 );       

    int px = coords->x1; //图片绝对区域x
    int py = coords->y1; //图片绝对区域y
    int pw = coords->x2-coords->x1+1; //图片绝对区域w
    int ph = coords->y2-coords->y1+1;  //图片绝对区域h



    //以BUF左上角为起始
    int xoffset = rect.left;
    int yoffset = rect.top;
    px = px - xoffset;
    py = py - yoffset;
    crop->left -= xoffset;
    crop->top -= yoffset;
    rect.left -= xoffset;
    rect.top -= yoffset;
    int compx = 0;
    int compy = 0;
    if(dy-yoffset<compy){//  旋转的圆心负坐标需要特殊处理
        compy = dy-yoffset;
    }
    if(dx-xoffset<compx){
        compx = dx-xoffset;
    }
    if(py+dy<compy){
        compy = py+dy;
    }    
    if(px+dx<compx){
        compx = px+dx;
    }
    // int coy = 0;
    // printf("pdy %d h %d", py+dy, rect.height);
    // if((py+dy)>rect.height){
    //     // compy = py+dy - rect.height;
    // }

    // printf("pydy %d %d yoffset %d compy %d",py,dy, yoffset, compy);
    //  旋转BWIOUTTABLE特殊处理
    // static struct flash_file_info file_info = {0};
    // if(angle && load_mef == LOAD_FROM_FLASH_WITHOUT_TAB){
    //     src_addr = malloc(pw*ph*3);
    //     // memcpy(src_addr, src_addr)
    //     load_mef = LOAD_FROM_RAM;
    // }
    
    file.width  = pw;
    file.height = ph;

    struct imb_task_info imb_tsk_info = {0};
    imb_tsk_info.data_src       = DATA_SRC_NONE;//无数据源
    imb_tsk_info.zip_en         = 0;
    imb_tsk_info.in_format      = LAYER_FORMAT_SOLID;//纯色
    imb_tsk_info.out_format     = OUTPUT_FORMAT_RGB565;
    imb_tsk_info.x_offset       = compx;
    imb_tsk_info.y_offset       = compy;
    imb_tsk_info.src_w          = rect.width - compx;
    imb_tsk_info.src_h          = rect.height - compy;
    imb_tsk_info.des_w          = rect.width - compx;
    imb_tsk_info.des_h          = rect.height - compy;
    imb_tsk_info.priv           = 0;//纯色格式，该参数表示颜色0xr5g6b5
    imb_tsk_info.osd0_in_buf    = 1;
    imb_task_list_init(imb_task_head_get(0), &imb_tsk_info);
    
    //  普通图片
    if(LV_IMG_ZOOM_NONE != prw || LV_IMG_ZOOM_NONE != prh ){   //  缩放
        // y_printf("%s%d",__func__,__LINE__);
        imb_create_image_scale_task(imb_task_get_id(), &file,  src_addr, 
            (float)prw/(float)LV_IMG_ZOOM_NONE, (float)prh/(float)LV_IMG_ZOOM_NONE,
            px+dx*(LV_IMG_ZOOM_NONE-prw)/LV_IMG_ZOOM_NONE, 
            py+dy*(LV_IMG_ZOOM_NONE-prh)/LV_IMG_ZOOM_NONE, 
            pw*prw/LV_IMG_ZOOM_NONE,ph*prh/LV_IMG_ZOOM_NONE,
            crop, false, load_mef);         
    } 
    else if(angle){   //  旋转
        if (file.height*file.width < 120*120*2 && 0) {  //占RAM速度快120*120图以内不分快
            imb_create_image_rotate_task(imb_task_get_id(), &file, src_addr, 
                dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy,
                NULL, load_mef
            );
        } else {  //不占RAM速度慢
            // printf("YYYY %d h %d",py+dy-compy, rect.height);
            imb_create_image_rotate_chunk_task(imb_task_get_id(), &file, src_addr, 
                dx, dy, (angle/10)%360, px+dx-compx, py+dy-compy, load_mef
            );
        }
    } else{    //      原大小
        imb_create_image_task(imb_task_get_id(), &file, src_addr, 
            px,py,pw,ph,
            crop, false, load_mef);//width,height参数可小于file.width, file.height,若小于则裁剪 
    }       
    
    //打印所有任务信息
    // imb_task_info_dump(imb_task_head_get(0));

    //启动合成&推屏
    struct rect disp;//unused
    struct rect page_draw = {0};
    struct rect screen_rect;//屏幕有效显示区域
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = rect.width;
    screen_rect.height = rect.height;

    rect.left  = 0;
    rect.top = 0;

    u16 bg_stride = (rect.width * 2 + 3) / 4 * 4;
    u16 colums = rect.width;
    u16 lines = rect.height;//info.buffer_size / info.buf_num / bg_stride;

    imb_task_head_set_buf(imb_task_head_get(0), buf, bufsize, rect.width, rect.height, bg_stride, lines, 1);
    imb_task_head_config(imb_task_head_get(0), &rect, &screen_rect, &screen_rect, &page_draw, STATE_SYNTHESIS_AND_DRAW);

    struct imb_cb_interface itf = {0};
    usr_buf = buf;
    itf.buf_get = usr_buf_get;
    itf.buf_check = usr_buf_check;
    itf.buf_set_idle = usr_buf_set_idle;
    itf.buf_set_pending = usr_buf_set_pending;
    itf.buf_wait_finish = usr_buf_wait_finish;
    itf.draw_area = usr_draw_area;
    itf.draw_meld_area = usr_draw_meld_area;

    int a = jiffies_msec();
    imb_start(imb_task_head_get(0), &screen_rect, disp, colums, lines, &itf );
    int b = jiffies_msec();
    // printf("%d",b-a);

    imb_task_all_destroy(imb_task_head_get(0));

    // flash_tab_info_release(&file_info);

    return ;
    
}

void lv_draw_jl_dma2d_ctx_init(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    lv_draw_sw_init_ctx(drv, draw_ctx);

    lv_draw_sw_ctx_t * dma2d_draw_ctx = (lv_draw_sw_ctx_t *)draw_ctx;

    //dma2d_draw_ctx->blend = my_draw_blend;//lv_draw_jl_dma2d_blend; //  纯色灰度填充，预先IMB合成, 其中纯色0透明度好像没啥优化效果,392*392矩形带A优化效果：IMB 44fps  lv 25FPS
    // dma2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_jl_dma2d_img_decoded;//BUF图片合成
    dma2d_draw_ctx->base_draw.draw_img = lv_draw_jl_dma2d_img;//    杰理压缩图片或jpg硬解码 直接接管合成
    // // Note: currently it does not make sense use lv_gpu_jl_dma2d_wait_cb() since waiting starts right after the dma2d transfer
    // //dma2d_draw_ctx->base_draw.wait_for_finish = lv_gpu_jl_dma2d_wait_cb;
    // dma2d_draw_ctx->base_draw.buffer_copy = lv_draw_jl_dma2d_buffer_copy;

    dma2d_draw_ctx->base_draw.draw_img_decoded = lv_draw_imb_img_decoded;// 对于RAM或连续的FLASH的图片可以直接预先IMB合成
}

void lv_draw_jl_dma2d_ctx_deinit(lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(draw_ctx);
}
#endif


#if 1//JPG

//  释放资源
void close_ui_res_gropu(int n){
    if(ui_res_gropu[n].resfile != NULL){
        res_fclose(ui_res_gropu[n].resfile);
        ui_res_gropu[n].resfile = NULL;
    }
    if(ui_res_gropu[n].indexfile != NULL){
        res_fclose(ui_res_gropu[n].indexfile);
        ui_res_gropu[n].indexfile = NULL;
    }
    if(ui_res_gropu[n].cache != NULL){
        free(ui_res_gropu[n].cache);
        ui_res_gropu[n].cache = NULL;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    构造flash地址映射表(不依赖文件系统)，只适用于地址连续的情况
   @param    flash_file_info : 文件地址信息
   @param    file_base_addr : 文件存放在flash的物理地址
   @param    filelen : 文件长度
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int flash_tab_info_get(struct flash_file_info *file_info, u32 file_base_addr, u32 filelen)
{
    int i;
    u32 flash_map_tab_num;

    flash_map_tab_num = (file_base_addr % 4096 + filelen + 4096 - 1) / 4096;

    file_info->tab_size = flash_map_tab_num * sizeof(u32);
    file_info->tab = (u32 *)malloc(file_info->tab_size);
    if(!file_info->tab) return -1;
    file_info->offset = file_base_addr % 4096;
    file_info->last_tab_data_len = flash_map_tab_num * 4096 - filelen - file_info->offset;
    for (i = 0; i < flash_map_tab_num; i++) {
        file_info->tab[i] = file_base_addr / 4096 * 4096 + 4096 * i;
        // printf("info->tab[%d] : 0x%x\n", i, file_info->tab[i]);
    }

    return 0;
}

#if 0
s8 open_res(int n){
    if(ui_res_gropu[n].resfile == NULL && ui_res_gropu[n].imbres){
        g_printf("打开分组资源");
        ui_res_gropu[n].resfile = res_fopen(ui_res_gropu[n].imbres,"r");
        if(!ui_res_gropu[n].resfile){
            g_printf("打开IMB资源失败");
            return -1;
        }
        if(ui_res_flash_info_get(&ui_res_gropu[n].flash_file_info, ui_res_gropu[n].imbres, "res")){
            res_fclose(ui_res_gropu[n].imbres);
            ui_res_gropu[n].resfile = NULL;
            g_printf("打开IMB资源失败");
            return -1;
        }
    }
}

int A2I(char* str) {
    int result = 0;
    int i = 0;

    // 逐个字符进行转换
    while (str[i] != '\0') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

    return result;
}

//  IMB资源获取文件信息
int get_jl_img_info_by_group_incache(const void *src, struct image_file *file, lv_img_header_t * header)
{
    u8 *u8_p = src;//  盘符
    char * real_path = NULL;  //  真实路径
    if(u8_p[1] == ':') real_path = &u8_p[2]; 
    u8* lastname = NULL;//  文件名 

    //  查找资源组所属
    int n = -1;   
    if(u8_p[0]>='0' && u8_p[0]<='9'){
        lastname = src;
        n = 0;
    }else{
        for(u8 i=0;i<ARRAY_SIZE(ui_res_gropu);i++){
            if( u8_p[0] == ui_res_gropu[i].letter)   //盘符是否匹配
            {
                lastname = real_path;
                n = i;
                break;
            }
        }
        if(n==-1) return -1;
    }

    //  ID查找(文件系统)
    if(ui_res_gropu[n].mode == IN_RES_WITH_ID){
        open_res(n);
        // printf("id name %s", lastname);
        int id = A2I(lastname);
        open_image_by_id(0, ui_res_gropu[n].resfile, file, id&0xffff, id>>16);
        if(header){
            header->always_zero =0;
            header->cf = 1;
            header->reserved = 0;
            header->h = file->height;
            header->w = file->width;
        }
        // printf("id2 name %s", lastname);
        return n;
    }   

    if(ui_res_gropu[n].mode == IMG_BIN_JPG_FILE) {
        if(header){
            // printf("lastname %s",lastname);
            if(!memcmp(lv_fs_get_ext(src), "bin", strlen("bin"))){
                void *fd = fopen(lastname,"r");
                if(fd){
                    fread(fd, header, sizeof(lv_img_header_t));
                    fclose(fd);
                    return n;
                }
            }
            else if(!memcmp(lv_fs_get_ext(src), "jpg", strlen("jpg"))) {
                struct flash_file_info jpg_ui_resfile_info;
                int ret = ui_res_flash_info_get(&jpg_ui_resfile_info, lastname, "res");
                imb_jpg_draw(&jpg_ui_resfile_info, NULL, NULL, NULL, NULL);
                struct jpeg_decoder_fd *jpg_fd = jpeg_dec_get_fd();
                header->always_zero =0;
                header->cf = 1;
                header->reserved = 0;
                header->h = jpg_fd->info.y;
                header->w = jpg_fd->info.x;
                ui_res_flash_info_free(&jpg_ui_resfile_info, "res");       
                return n;
            }  
        }
    }

    return -1;
}
#endif

static u8 *usr_jpg_dec_stream_input(void *__fat_info, u32 *len, u32 offset_4k)
{
    struct CacheFile* info = (struct CacheFile*)__fat_info;
    u8 *jpg_stream = info->data;
    int file_len = info->len;
    u32 offset = offset_4k * 4096;
    u32 res_len;

    if (offset >= file_len) {
        *len = 0;
        return NULL;
    }
    res_len = file_len - offset;
    *len = (res_len > 4096) ? 4096 : res_len;

    return jpg_stream + offset;
}

static u8 *usr_jpg_dec_stream_input2(void *__fat_info, u32 *len, u32 offset_4k)
{
    struct file_index_t* info = (struct file_index_t*)__fat_info;
    u8 *jpg_stream = info->addr;
    int file_len = info->len;
    u32 offset = offset_4k * 4096;
    u32 res_len;

    if (offset >= file_len) {
        *len = 0;
        return NULL;
    }
    res_len = file_len - offset;
    *len = (res_len > 4096) ? 4096 : res_len;

    return jpg_stream + offset;
}

static lv_res_t decoder_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    if(lcd_sleep_status()){
        g_printf("休眠不能访问IMB资源");
        return LV_RES_INV;
    }
    // put_buf(src,16);
    lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/
    lv_res_t ret = LV_RES_OK;
    if(src_type == LV_IMG_SRC_VARIABLE) 
    {
        const lv_img_dsc_t * img_dsc = src;
        uint8_t * raw_jpeg_data = (uint8_t *)img_dsc->data;
        const uint32_t raw_jpeg_data_size = img_dsc->data_size;
        const uint8_t jpg_signature[] = {0xFF, 0xD8, 0xFF};
        const uint8_t *imb_signature = "IMB";
        if(!memcmp(jpg_signature, raw_jpeg_data, sizeof(jpg_signature))){
            header->always_zero = 0;
            header->h = img_dsc->header.h;
            header->w = img_dsc->header.w;
            return LV_RES_OK;
        }else if(!memcmp(imb_signature, raw_jpeg_data, sizeof(imb_signature))){
            header->always_zero = 0;
            header->h = img_dsc->header.h;
            header->w = img_dsc->header.w;
            return LV_RES_OK;
        }

        return LV_RES_INV;
    }

#if 0
    //userdata  路径    图片尺寸

    //判断是否是文件名
    // lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        return LV_RES_INV;
    }
    else if(src_type != LV_IMG_SRC_FILE) {
        return LV_RES_INV;
    }

    struct image_file imgfile={0};
    sizeof(struct image_file);
    int ui_group_n = get_jl_img_info_by_group_incache(src, &imgfile, header);
    if(ui_group_n  != -1){
        return LV_RES_OK;
    }

    printf("ui_group_n ==-1");
#endif

    return LV_RES_INV;
// printf("ui_group_n == %d", ui_group_n);
    // if(imgfile.len != 0){
    //     header->always_zero = 0;
    //     header->cf = 0;/*  */
    //     header->h = imgfile.height;
    //     header->w = imgfile.width;
    //     // header->cf = 
    //     // printf("SUCCEED");
    //     return LV_RES_OK;
    // }

    // /*Support only "*.anim" files*/
    // if(ui_res_gropu[ui_group_n].mode == JPG_ANIM_WITH_FS && !memcmp(lv_fs_get_ext(src), "bin", strlen("bin"))) {
    //     // printf("anmi src %s", src);
    //     u8 *u8_p = src;//  盘符
    //     u8 * real_path = NULL; 
    //     if(u8_p[1] == ':') real_path = &u8_p[2];  //  真实路径

    //     //  通过SRAM缓存解码或者PSRAM
    //     struct CacheFile* res= readFromCache(real_path);
    //     if(res==NULL){
    //         return LV_RES_OK;
    //     }

    //     struct jpeg_decoder_fd *jpg_fd = jpeg_dec_get_fd();
    //     if(jpg_fd->fat_info != res){
    //         u8 rgb_format = 1;//1 :rgb_565 0:rgb_888
    //         char err = jpeg_dec_init(res, rgb_format, usr_jpg_dec_stream_input);
    //     }       
        
    //     header->always_zero = 0;
    //     header->h = jpg_fd->info.y;
    //     header->w = jpg_fd->info.x;
    //     return LV_RES_OK;
    // }


    // if(ui_res_gropu[ui_group_n].mode == PSRAM_IMG){
    //     if(!PSRAM_FULL_SCREEN){
    //         return LV_RES_INV;
    //     }
    //     printf("PSRAMING...");
    //     header->always_zero = 0;
    //     header->cf = 0;/*  */
    //     header->h = 454;
    //     header->w = 454;
    //     return LV_RES_OK;
    // }
    

    //return LV_RES_INV;
}

void jl_imb_init(void)
{
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    // lv_img_decoder_set_open_cb(dec, NULL);
    // lv_img_decoder_set_close_cb(dec, NULL);
    // lv_img_decoder_set_read_line_cb(dec, NULL);
}

#if 0
//  CYCLE-BUFF 管理
#define CACHE_SIZE      3    //实际缓存个数CACHE_SIZE-1
#define CYCLE_MALLOC    malloc
#define CYCLE_FREE      free

typedef struct {
    int id;
    struct CacheFile* buffer;
    u8* idexfile;
    void* idexfile_fd;
} CacheEntry;

CacheEntry cache[CACHE_SIZE];
int currentEntry = 0;

void initCache() {
    int i;
    for (i = 0; i < CACHE_SIZE; i++) {
        cache[i].id = -1;
        cache[i].buffer = NULL;
    }
}

int nextEntry;

u8 Entryisbusy(){
    return (nextEntry != currentEntry);
}

void addEntry(int id, u8* idexfile) {
    nextEntry = (currentEntry + 1) % CACHE_SIZE;
    
    //  解析文件
    static void *fd = NULL;
    static void *tempfd = NULL;

    if(!fd){
        fd = fopen(idexfile, "r");  //SD内容
        // printf(">>>>>>>>>>>>>>>>>>fd %d %s", fd,idexfile);
        if(!fd){
            return;
        }
            
        tempfd = fopen("storage/virfat_flash/C/temp.bin", "w+");
        int head[4];
        fread(fd, head, 16);
        int tablelen = head[2]*head[3];
        u8 * headdata = malloc(head[2]*head[3]);
        printf("申请表长度 ：%d ", head[2]*head[3]);
        if(!headdata){
            printf("内存不足");
            return;
        }
        fread(fd, headdata, tablelen);
        fwrite(tempfd, headdata, tablelen);
        free(headdata);
    }

    u8* buffer;
    int len;

    if(!fd){
        return;
    }
    
    if(tempfd){
        fseek(tempfd, id*8, SEEK_SET);
        fread(tempfd, &buffer, 4);
        fread(tempfd, &len, 4);
        // printf("tempfd %x %d", buffer, len);
    }

    //  申请内存，拷贝内容
    cache[currentEntry].id = id;
    if(cache[currentEntry].buffer == NULL){
        cache[currentEntry].buffer = CYCLE_MALLOC(sizeof(struct CacheFile) + len);  
    }

    if(cache[currentEntry].buffer){
        
        fseek(fd, buffer, SEEK_SET);
        cache[currentEntry].buffer->len = len;
        fread(fd, cache[currentEntry].buffer->data, len);
    }

    //  释放下一个
    cache[nextEntry].id = -1;
    if(cache[nextEntry].buffer){
        CYCLE_FREE(cache[nextEntry].buffer);
        cache[nextEntry].buffer = NULL;
    }

    currentEntry = nextEntry;
}



void printCache() {
    int i;
    for (i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].id != -1) {
            printf("ID: %d, BUFFER\n", cache[i].id);
            // put_buf(cache[i].buffer, 16);
        }
    }
}


int isCached(int nowid) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache[i].id == nowid) {
            return i;
        }
    }
    return -1;
}



IndexEntry search_file_by_name(char*name, char*index);

void* outputfd;

spinlock_t jpgcache;

static int now_cache_num;

void jpg_prep_cache(int num, int next_num, char* indexfile){
    now_cache_num = num;
    // while(Entryisbusy()){
    //     // printf("xxxxx");
    //     // os_time_dly(1);  //延时会浪费10ms
    // }
    // printf("{jpg_prep_cache}%d %s", num, indexfile);
    // int a = jiffies_msec();
    os_mutex_pend(&jpg_cache_MUTEX, 0);
    os_taskq_post_msg("sd_jpg", 2, next_num, indexfile);
    os_mutex_post(&jpg_cache_MUTEX);
    // int b = jiffies_msec();
    // printf("%d", b-a);
}


struct CacheFile* readFromCache(char* indexfile) {
    // cycle-cache

    if(isCached(now_cache_num)!=-1){
        // printf("{isCached}%d", now_cache_num);
        return cache[isCached(now_cache_num)].buffer;
    } 
    
    // printf("{noCached}%d %s", now_cache_num,indexfile );
    // jpg_prep_cache(num, indexfile);

    // int cnt = 100;  //1s超时
    // while(Entryisbusy() && cnt--){
    //     os_time_dly(1);
    // }    

    // if(isCached(num)){
    //     return cache[isCached(num)].buffer;
    // }    

    return NULL;
}

//  二分法查找
IndexEntry search_file_by_name(char*name, char*index){
    static FILE* file=NULL;
    static int file_num=0;
    IndexEntry res={0};

    if(file==NULL){
        printf(">>>%s",index);
        file = fopen(index, "r");
        printf("1111file_num:%d", file_num);
    }
    if(file){
        fseek(file,0,SEEK_SET);
        fread(file, &file_num, sizeof(file_num));
        // printf("222file_num:%d", file_num);
    }
    printf("file_num:%d", file_num);

    int left = 0, right = file_num - 1;
int a =jiffies_msec();    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        int name_addr, name_addr_next;
        fseek(file, 4+mid*4,SEEK_SET);
        fread(file, &name_addr, sizeof(name_addr));
        fread(file, &name_addr_next, sizeof(name_addr_next));
        u8 len = name_addr_next - name_addr - 8;
        fseek(file, 4+file_num*4 + name_addr, SEEK_SET);
        u8* filename = malloc(len+1);
        memset(filename,0,len+1);
        fread(file, filename, len);    
        int cmp = strcmp(filename, name);
        free(filename);
        if (cmp == 0) {
            
            fread(file, &res, sizeof(IndexEntry));
            int b =jiffies_msec();
            // printf("filename :%s t:%d %x %d", name, b-a, res.offset, res.length);
            return res;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return res;
    

}

static void sd_jpg_task(void *p){
            if (app_get_curr_task() != 4) {
                music_set_start_auto_play(1);
                app_task_switch_to(4);
            } else {
                app_task_put_key_msg(134, 0);
            }

    initCache();
    os_time_dly(2);
    int msg[32];
    int ret;
    while(1){
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        if (ret != OS_TASKQ) {
            continue;
        }

        if (msg[1]<0){
            continue;
        }

        if(isCached(msg[1]) != -1){
            continue; 
        }
        
        os_mutex_pend(&jpg_cache_MUTEX, 0);
        // printf("id %d    name %s", msg[1], msg[2]);
        addEntry(msg[1], msg[2]);
        os_mutex_post(&jpg_cache_MUTEX);
    }
}

int sd_jpg_test() {
    os_mutex_create(&jpg_cache_MUTEX);
    task_create(sd_jpg_task,NULL,"sd_jpg"); //启动索引表
    return 0;
}
#endif
#endif

void flash_tab_info_release(struct flash_file_info *file_info)
{
    if (file_info->tab) 
    {
        free(file_info->tab);
        file_info->tab = NULL;
    }
}

#if 0
#include "ui/ui_effect.h"
extern struct effect_ctrl ectl;
#define API_CUSTOM_SCA_LIMIT_W		(240)
#define API_SCA_LIMIT_W		(240)
static u32 sca_pixel_size(u32 format)
{
    const u8 bits[] = {32, 24, 16, 8, 16, 8, 8, 1, 24, 16, 0, 0};
    if (format > sizeof(bits)) {
        printf("sca format err %d\n", format);
        return 0;
    }

    return bits[format];
}


/**
 * @brief api_get_blk_sca_info:计算输入输出的缩放分块信息
 *
 *
 * @param sca:输出参数，由该函数内部初始化,必须静态或者全局
 * @param width:背景的宽和高，一般是全屏
 * @param height:背景的宽和高，一般是全屏
 * @param ratio_w:缩放比例，必须ratio_w == ratio_h
 * @param ratio_h:缩放比例，必须ratio_w == ratio_h
 * @param x_offset:居中缩放之后的水平偏移
 * @param y_offset:居中缩放之后的垂直偏移
 * @param limit_h:该值必须等于屏分块的高度
 * @param sca_img:输入图片的信息
 * @param status: 0:表示初始状态，会将图片分割成多个小图片，并计算输入分块信息，其他:表示只计算输出分块信息
 *
 * @return:返回初始化之后的sca句柄
 */
struct api_scale_info *api_get_blk_sca_info(struct api_scale_info *sca, int width, int height, float ratio_w, float ratio_h, int x_offset, int y_offset, int limit_h, struct api_ciner_img_info *sca_img, int status)
{
    sca->width = width;
    sca->height = height;
    sca->format = sca_img->format;
    sca->bytesperpixel = sca_pixel_size(sca_img->format) / 8;
    sca->backcolor = 0xffff;

    sca->src.left = 0;
    sca->src.top = 0;
    sca->src.width = sca_img->width;
    sca->src.height = sca_img->height;

    sca->dst.width = sca->src.width * ratio_w;
    sca->dst.height = sca->src.height * ratio_h;
    sca->dst.left = (width - sca->dst.width) / 2 + x_offset;
    sca->dst.top = (height - sca->dst.height) / 2 + y_offset;

    printf("dst_left %d, dst_top %d\n", sca->dst.left, sca->dst.top);

    sca->ratio_w = ratio_w;
    sca->ratio_h = ratio_h;

    if (sca->src.height <= 240) {
        printf("sca->src.height <= 240");
        limit_h = sca->src.height;
    }
    //最后一块的高度不能太小(例如1就太极端)，否则合成超时
    int res = sca->src.height - (sca->src.height / limit_h) * limit_h;
    float ratio_tmp = ratio_h * res;
    if (ratio_tmp && (ratio_tmp < 1)) {//微调
        printf("res too small %d\n", res);
        limit_h += 1;
    }

    printf("new_h %d, ratio %f\n", limit_h, ratio_h);

    int h_block_n = (sca->src.width + API_CUSTOM_SCA_LIMIT_W - 1) / API_CUSTOM_SCA_LIMIT_W;
    int v_block_n = (sca->src.height + limit_h - 1) / limit_h;
    int h, v;
    u8 block_index;
    int start;

    sca->h_block_n = h_block_n;
    sca->v_block_n = v_block_n;
    //status: 0:
    if (status == 0) {

        int kstart = jiffies_msec();

        sca->src_block = (struct api_src_block_info *)imb_malloc(h_block_n * v_block_n * sizeof(struct api_src_block_info), true);
        if (!sca->src_block) {
            return -1;
        }
        sca->dst_block = (struct rect *)imb_malloc(h_block_n * v_block_n * sizeof(struct rect), true);
        if (!sca->dst_block) {
            return -1;
        }

        int h_block_len = sca->src.width / h_block_n;
        int v_block_len = limit_h;

        img_set_input(DATA_SRC_SRAM, DATA_NOZIP, sca->src.width, sca->src.height, sca_img->format, NULL, psram_cache2nocache_addr(sca_img->img_buf));

        /* printf("%d, %d, %d, %d\n", h_block_n, v_block_n, h_block_len, v_block_len); */

        //计算源图片分块参数
        for (v = 0; v < v_block_n; v++) {
            for (h = 0; h < h_block_n; h++) {
                block_index = v * h_block_n + h;
                sca->src_block[block_index].rect.left = h * h_block_len;
                sca->src_block[block_index].rect.top = v * v_block_len;
                if (h == (h_block_n - 1)) {
                    sca->src_block[block_index].rect.width = sca->src.width - sca->src_block[block_index].rect.left;
                } else {
                    sca->src_block[block_index].rect.width = h_block_len;
                }
                if (v == (v_block_n - 1)) {
                    sca->src_block[block_index].rect.height = sca->src.height - sca->src_block[block_index].rect.top;
                } else {
                    sca->src_block[block_index].rect.height = v_block_len;
                }

                sca->src_block[block_index].buf_size = ((sca->src_block[block_index].rect.width * sca->bytesperpixel + 3) / 4 * 4) * sca->src_block[block_index].rect.height;
                sca->src_block[block_index].buf = (u8 *)br28_malloc_psram(sca->src_block[block_index].buf_size);

                /* printf("%d, %d, %d, %d, %d, %x\n", block_index, sca->src_block[block_index].rect.width, sca->src_block[block_index].rect.height, sca->bytesperpixel, sca->src_block[block_index].buf_size, (u32)sca->src_block[block_index].buf);	 */
                ASSERT(sca->src_block[block_index].buf, "src_block malloc %d fail, %d\n", sca->src_block[block_index].buf_size, block_index);

                flushinv_dcache(sca->src_block[block_index].buf, sca->src_block[block_index].buf_size);
                img_set_output(psram_cache2nocache_addr(sca->src_block[block_index].buf), sca->src_block[block_index].rect.left, sca->src_block[block_index].rect.top, sca->src_block[block_index].rect.width, sca->src_block[block_index].rect.height);
                img_kick_sync();

            }
        }

        printf("img_cut time : %d ms\n", jiffies_msec() - kstart);
    }


    //计算目标缩放分块参数
    int h_offset = 0, v_offset = 0;
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            sca->dst_block[block_index].left = sca->dst.left + h_offset;
            sca->dst_block[block_index].top = sca->dst.top + v_offset;
            sca->dst_block[block_index].width = sca->src_block[block_index].rect.width * ratio_w;
            sca->dst_block[block_index].height = sca->src_block[block_index].rect.height * ratio_h;

            h_offset += sca->dst_block[block_index].width;

            /* printf("index %d, %d, %d, %d, %d\n", block_index, sca->dst_block[block_index].left, sca->dst_block[block_index].top, sca->dst_block[block_index].width, sca->dst_block[block_index].height); */

        }
        h_offset = 0;
        v_offset += sca->dst_block[block_index].height;
    }

    return sca;
}
/**
 * @brief api_get_blk_sca_info2:计算输入输出的缩放分块信息
 * 该函数与api_get_blk_sca_info()的区别是，根据不同的缩放
 * 比例调整不同的分块块数，目的的减轻移动过程中的抖动，但
 * 不同的分块块数会导致需要重复的申请和释放内存，因此一般
 * 是在连续移动时调用该函数，例如左右滑屏，在不移动时还是
 * 调用api_get_blk_sca_info()好
 *
 *
 * @param sca:输出参数，由该函数内部初始化,必须静态或者全局
 * @param width:背景的宽和高，一般是全屏
 * @param height:背景的宽和高，一般是全屏
 * @param ratio_w:缩放比例，必须ratio_w == ratio_h
 * @param ratio_h:缩放比例，必须ratio_w == ratio_h
 * @param x_offset:居中缩放之后的水平偏移
 * @param y_offset:居中缩放之后的垂直偏移
 * @param limit_h:该值必须等于屏分块的高度
 * @param sca_img:输入图片的信息
 * @param status: 0:表示初始状态，会将图片分割成多个小图片，并计算输入分块信息，其他:表示只计算输出分块信息
 *
 * @return:返回初始化之后的sca句柄
 */
struct api_scale_info *api_get_blk_sca_info2(struct api_scale_info *sca, int width, int height, float ratio_w, float ratio_h, int x_offset, int y_offset, int limit_h, struct api_ciner_img_info *sca_img, int status)
{
    sca->width = width;
    sca->height = height;
    sca->format = sca_img->format;
    sca->bytesperpixel = sca_pixel_size(sca_img->format) / 8;
    sca->backcolor = 0xffff;

    sca->src.left = 0;
    sca->src.top = 0;
    sca->src.width = sca_img->width;
    sca->src.height = sca_img->height;

    printf(">>>>>%d %d", sca->src.height, limit_h);

    if (sca->src.height <= 240) {
        limit_h = sca->src.height;
    }
    ASSERT(ratio_w == ratio_h);
    float ratio_tmp;
    int limit_h_tmp;
    int res;

    /* 计算输出分块高度对应的输入分块高度 */
    ratio_tmp = (float)limit_h / ratio_h;
    limit_h_tmp = (int)ratio_tmp;//取整

    printf("limit_h ratio_h limit_h_tmp %d %f %d",limit_h, ratio_h, limit_h_tmp);

    //最后一块的高度不能太小(例如1就太极端)，否则合成超时
    res = sca->src.height - (sca->src.height / limit_h_tmp) * limit_h_tmp;//最后一个分块的高度
    ratio_tmp = ratio_h * res;//最后一个分块缩放后的高度
    if (ratio_tmp && (ratio_tmp < 1)) {//微调
        printf("res too small %d\n", res);
        limit_h_tmp += 1;
    }

    ratio_h = (float)limit_h / (float)limit_h_tmp;
    ratio_w = ratio_h;
    limit_h = limit_h_tmp;

    printf("new_h %d, ratio_h %f\n", limit_h, ratio_h);

    sca->dst.width = sca->src.width * ratio_w;
    sca->dst.height = sca->src.height * ratio_h;
    sca->dst.left = (width - sca->dst.width) / 2 + x_offset;
    sca->dst.top = (height - sca->dst.height) / 2 + y_offset;

    printf("dst_left %d, dst_top %d\n", sca->dst.left, sca->dst.top);

    sca->ratio_w = ratio_w;
    sca->ratio_h = ratio_h;

    int h_block_n = (sca->src.width + API_SCA_LIMIT_W - 1) / API_SCA_LIMIT_W;//水平分块数量
    int v_block_n = (sca->src.height + limit_h - 1) / limit_h;//垂直分块数量
    int h, v;
    u8 block_index;
    int start;

    /* printf("v_block_n %d\n", v_block_n);	 */

    sca->h_block_n = h_block_n;
    sca->v_block_n = v_block_n;

    int h_block_len = sca->src.width / h_block_n;
    int v_block_len = limit_h;

    if (status == 0) {
        int kstart = jiffies_msec();

        img_set_input(DATA_SRC_SRAM, DATA_NOZIP, sca->src.width, sca->src.height, sca_img->format, NULL, psram_cache2nocache_addr(sca_img->img_buf));

        int cut_w, cut_left;
        for (h = 0; h < h_block_n; h++) {
            cut_left = h * h_block_len;
            if (h == (h_block_n - 1)) {
                cut_w = sca->src.width - cut_left;
            } else {
                cut_w = h_block_len;
            }
            sca->cut_buf_size[h] = ((cut_w * sca->bytesperpixel + 3) / 4 * 4) * sca->src.height;
            sca->cut_buf[h] = (u8 *)br28_malloc_psram(sca->cut_buf_size[h]);
            ASSERT(sca->cut_buf[h], "src_block malloc %d fail, %d\n", sca->cut_buf_size[h], h);

            flushinv_dcache(sca->cut_buf[h], sca->cut_buf_size[h]);
            img_set_output(psram_cache2nocache_addr(sca->cut_buf[h]), cut_left, 0, cut_w, sca->src.height);
            img_kick_sync();
        }

        printf("img_cut time : %d ms\n", jiffies_msec() - kstart);
    }


    if (sca->max_block_n < h_block_n * v_block_n) {
        if (sca->src_block) {
            imb_free(sca->src_block);
            sca->src_block = NULL;
        }
        if (sca->dst_block) {
            imb_free(sca->dst_block);
            sca->dst_block = NULL;
        }
        sca->max_block_n = h_block_n * v_block_n;
    }
    if (sca->src_block == NULL) {
        sca->src_block = (struct api_src_block_info *)imb_malloc(h_block_n * v_block_n * sizeof(struct api_src_block_info), true);
        if (!sca->src_block) {
            return -1;
        }
    }
    if (sca->dst_block == NULL) {
        sca->dst_block = (struct rect *)imb_malloc(h_block_n * v_block_n * sizeof(struct rect), true);
        if (!sca->dst_block) {
            return -1;
        }
    }

    /* printf("%d, %d, %d, %d\n", h_block_n, v_block_n, h_block_len, v_block_len); */


    int offset[4] = {0};
    //计算源图片分块参数
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            sca->src_block[block_index].rect.left = h * h_block_len;
            sca->src_block[block_index].rect.top = v * v_block_len;
            if (h == (h_block_n - 1)) {
                sca->src_block[block_index].rect.width = sca->src.width - sca->src_block[block_index].rect.left;
            } else {
                sca->src_block[block_index].rect.width = h_block_len;
            }
            if (v == (v_block_n - 1)) {
                sca->src_block[block_index].rect.height = sca->src.height - sca->src_block[block_index].rect.top;
            } else {
                sca->src_block[block_index].rect.height = v_block_len;
            }

            sca->src_block[block_index].buf_size = ((sca->src_block[block_index].rect.width * sca->bytesperpixel + 3) / 4 * 4) * sca->src_block[block_index].rect.height;
            sca->src_block[block_index].buf = sca->cut_buf[h] + offset[h];

            ASSERT(((u32)sca->src_block[block_index].buf + sca->src_block[block_index].buf_size) <= ((u32)sca->cut_buf[h] + sca->cut_buf_size[h]), "src_small_block malloc %d fail, %d, %x, %x, %x\n", sca->src_block[block_index].buf_size, block_index, (u32)sca->src_block[block_index].buf, (u32)sca->cut_buf[h], sca->cut_buf_size[h]);
            offset[h] += sca->src_block[block_index].buf_size;
        }
    }


    //计算目标缩放分块参数
    int h_offset = 0, v_offset = 0;
    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            sca->dst_block[block_index].left = sca->dst.left + h_offset;
            sca->dst_block[block_index].top = sca->dst.top + v_offset;
            sca->dst_block[block_index].width = sca->src_block[block_index].rect.width * ratio_w;
            sca->dst_block[block_index].height = sca->src_block[block_index].rect.height * ratio_h;

            h_offset += sca->dst_block[block_index].width;
        }
        h_offset = 0;
        v_offset += sca->dst_block[block_index].height;
    }

    return sca;
}
void api_free_blk_sca_info2(struct api_scale_info *sca)
{
    int h, v;
    u8 block_index;
    int h_block_n = sca->h_block_n;
    int v_block_n = sca->v_block_n;

    if (sca->src_block == NULL) {
        return;
    }

    for (h = 0; h < h_block_n; h++) {
        if (sca->cut_buf[h]) {
            br28_free_psram(sca->cut_buf[h]);
            sca->cut_buf[h] = NULL;
        }
    }

    if (sca->src_block) {
        imb_free(sca->src_block);
        sca->src_block = NULL;
    }
    if (sca->dst_block) {
        imb_free(sca->dst_block);
        sca->dst_block = NULL;
    }
    sca->max_block_n = 0;
}

void api_free_blk_sca_info(struct api_scale_info *sca)
{
    int h, v;
    u8 block_index;
    int h_block_n = sca->h_block_n;
    int v_block_n = sca->v_block_n;

    if (sca->src_block == NULL) {
        return;
    }

    for (v = 0; v < v_block_n; v++) {
        for (h = 0; h < h_block_n; h++) {
            block_index = v * h_block_n + h;
            if (sca->src_block[block_index].buf) {
                br28_free_psram(sca->src_block[block_index].buf);
                sca->src_block[block_index].buf = NULL;
            }
        }
    }

    if (sca->src_block) {
        imb_free(sca->src_block);
        sca->src_block = NULL;
    }
    if (sca->dst_block) {
        imb_free(sca->dst_block);
        sca->dst_block = NULL;
    }
}

/**
 * @brief api_blk_sca : 处理分块缩放的合成函数
 *
 * @param scale[]:分块图片的数组，一个数组代表一张大图片的分块信息
 * @param page_num:图片数组的个数
 * @param base_img:底图,可以为NULL
 * @param sw_buf:NULL表示合成完联动推屏直接显示，否则其他psram的DMA地址值
 *
 * @return:0成功 其他失败
 */
int api_blk_sca(struct api_scale_info *scale[], int page_num, struct api_ciner_img_info *base_img, u8 *sw_buf)
{
    int ret;
    int i;
    int h, v;
    u8 block_index;
    struct rect cache_rect;
    struct rect screen_rect;//屏幕有效显示区域
    struct image_file tmp_file = {0};
    struct imb_task_head *new_root = imb_task_head_get(0);
    struct api_scale_info *sca = NULL;

    if ((scale == NULL) || (page_num <= 0)) {
        return -1;
    }

    new_root->ring_buf[0] = NULL;
    new_root->ring_buf[1] = NULL;
    for (i = 0; i < RING_MAX_TASK; i++) {
        if (new_root->task_tab[i]) {
            new_root->task_tab[i] = NULL;
        }
    }

    if (sw_buf) {
        new_root->copy_to_psram = 3;
    } else {
        new_root->copy_to_psram = 0;
    }
    new_root->state = STATE_SYNTHESIS_AND_DRAW;
    new_root->new_copy = 0;

    //创建背景任务(必须)
    imb_create_bg_task(new_root->format, new_root->screen_width, new_root->screen_height, 0x0);

    if (base_img) {
        tmp_file.width = base_img->width;
        tmp_file.height = base_img->height;
        tmp_file.format = base_img->format;
        imb_create_image_task(imb_task_get_id(), &tmp_file, base_img->img_buf, 0, 0, base_img->width, base_img->height, NULL, false, LOAD_FROM_STATIC_RAM);
    }

    i = 0;
    do {
        sca = scale[i];

        for (v = 0; v < sca->v_block_n; v++) {
            for (h = 0; h < sca->h_block_n; h++) {
                block_index = v * sca->h_block_n + h;
                tmp_file.width = sca->src_block[block_index].rect.width;
                tmp_file.height = sca->src_block[block_index].rect.height;
                tmp_file.format = sca->format;
                imb_create_image_scale_task(imb_task_get_id(), &tmp_file, psram_cache2nocache_addr(sca->src_block[block_index].buf), sca->ratio_w, sca->ratio_h, sca->dst_block[block_index].left, sca->dst_block[block_index].top, sca->dst_block[block_index].width, sca->dst_block[block_index].height, NULL, false, LOAD_FROM_STATIC_RAM);
            }
        }

        i++;
    } while (--page_num);

    printf("start meld\n");
    screen_rect.left = 0;
    screen_rect.top = 0;
    screen_rect.width = new_root->screen_width;
    screen_rect.height = new_root->screen_height;

    /* imb_task_info_dump(new_root); */

    /* lcd_wait(); */
    if (sw_buf) {
        lcd_wait();
        ectl.ps_obuf = sw_buf;
    }
    
// imb_set_output_cb(imb_data_output, imb_data_output_wait_finish);
    // imb_task_head_set_buf(imb_task_head_get(0), lcd_info.buffer, lcd_info.buffer_size, lcd_info.width, lcd_info.height, lcd_info.stride, lines, lcd_info.buf_num);
    // imb_task_head_config(imb_task_head_get(0), &api_rect, &api_screen_rect, &api_screen_rect, &api_page_draw, STATE_SYNTHESIS_AND_DRAW);


    imb_start(new_root, &screen_rect, cache_rect, new_root->screen_width, new_root->lines, NULL);

    new_root->copy_to_psram = 0;

    if (sw_buf) {
        int wait = 0x20;//推单独的一屏
        lcd_draw_area(0, ectl.sca_obuf, 0, 0, new_root->screen_width, new_root->screen_height, wait);//推屏
        ectl.ps_obuf = NULL;
        ectl.sca_obuf = NULL;
    }

    imb_task_all_destroy(new_root);



    return 0;
}
#endif

#if 0
#include "lvgl.h"

static struct api_scale_info *sca[2] = {0};

//  剪切当前页面到PSRAM ，最多是g_sca_hdl大小
int cut_now_page_in_psram(u8 i){
    if(!PSRAM_FULL_SCREEN){
        return -1;
    }
    if (i>ARRAY_SIZE(g_sca_hdl)){
        printf("g_sca_hdl i error %d", i);
        return -1;
    }

    api_free_blk_sca_info2(&g_sca_hdl[i]);

    struct api_scale_info  *sca = &g_sca_hdl[i];
    struct api_ciner_img_info _sca_img = {0};
    _sca_img.width   = 454;
    _sca_img.height  = 454;
    _sca_img.format  = LAYER_FORMAT_RGB565;
    _sca_img.img_buf = psram_act;//pic->data;
    struct api_ciner_img_info *sca_img = &_sca_img;
      int limit_h = 91;  

    api_get_blk_sca_info2(sca, 454,454,1.0,1.0,0,0,91,sca_img, 0);

}
#endif


static void *fd = 0;
void open_fd(char*name)
{
    if(fd)
    {
        dev_close(fd);
        fd = NULL;
    }

    if(!fd)
        fd = dev_open(name, NULL);
}

void *get_res_fd(void)
{
    return fd;
}

void close_fd(void)
{
    if(fd)
    {
        dev_close(fd);
        fd = NULL;
    }
}

//  SD卡直接访问图片资源有两种方式
//  1.预先拷贝RAM内存
//  2.预先拷贝到内部flash
//  3.预先拷贝到PSRAM

//  分区句柄，分区所在的物理地址，bin文件在分区的偏移，图片在bin文件中的地址长度，
//#include "../../../apps/common/include/norflash_sfc.h"
void lv_open_res(void *fd, int phyaddr, int offset, struct file_index_t res, lv_img_dsc_t *img_dst)
{
    if(!img_dst) return;

    memset(img_dst, 0, sizeof(lv_img_dsc_t));

    u32 cpuaddr = sdfile_flash_addr2cpu_addr(phyaddr + res.addr);
    u8 *img_header = (u8 *)cpuaddr;
    
    memcpy(&(img_dst->header), img_header, sizeof(lv_img_header_t));
    
    img_dst->data = img_header + sizeof(lv_img_header_t);
    img_dst->data_size = res.len - sizeof(lv_img_header_t);

    return;
}

void lv_open_ex_res(int phyaddr, struct file_index_t res, lv_img_dsc_t *img_dst)
{
    if(!img_dst) return;

    memset(img_dst, 0, sizeof(lv_img_dsc_t));

    u32 cpuaddr = 0x4000000 + phyaddr + res.addr;
    u8 *img_header = (u8 *)cpuaddr;
    
    memcpy(&(img_dst->header), img_header, sizeof(lv_img_header_t));
    
    img_dst->data = img_header + sizeof(lv_img_header_t);
    img_dst->data_size = res.len - sizeof(lv_img_header_t);

    return;
}

#define FONT_LEN (1024)
static u8 g_font_buf[FONT_LEN];
void lv_open_font(int offset, int len)
{
    if(len > FONT_LEN) len = FONT_LEN;

    u8 *font_src_addr = (u8 *)offset;

    memcpy(g_font_buf, font_src_addr, len);

    return;
}

u8 *get_g_font_buf(void)
{
    return g_font_buf;
}

#if 0
// static void get_sys_time(struct sys_time *time)
// {
//     void *fd = dev_open("rtc", NULL);
//     if (!fd) {
//         // get_elapse_time(time);
//         return;
//     }
//     dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
//     /* log_info("get_sys_time : %d-%d-%d,%d:%d:%d\n", time->year, time->month, time->day, time->hour, time->min, time->sec); */
//     dev_close(fd);
// }
// static lv_img_dsc_t ui_test;
// static lv_img_dsc_t ui_test1;
#endif

#if 0
static lv_obj_t * ui_sec;
static lv_obj_t * ui_min;
static lv_obj_t * ui_hour;

void test_timer_cb11(lv_timer_t * timer){
    lv_obj_t * img_obj = timer->user_data;
    if(_lv_area_is_on(&img_obj->coords, &lv_scr_act()->coords)==false){
        return;
    }

    static int cnt = FILE_1199_jpg;
    if(cnt>=FILE_preview){
        cnt = FILE_1199_jpg;
    }

    if(cnt<=FILE_1204_jpg){
        lv_obj_set_size(img_obj, 277, 277);
    }else{
        lv_obj_set_size(img_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    }

    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[cnt++], &ui_test);
    lv_img_set_src(img_obj, &ui_test);
    

    struct sys_time time;

    get_sys_time(&time);

    // int angle= jiffies_msec()*60/1000;
    lv_img_set_angle(ui_sec, time.sec*60%3600 );
    lv_img_set_angle(ui_min, time.min*60%3600 );
    lv_img_set_angle(ui_hour, time.hour*150%3600 );
    
    // lv_obj_invalidate(lv_scr_act());
}
#endif

#if 0
void ui_event____initial_actions1(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * target = lv_event_get_target(e);

        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>%d", event_code);
    
}

static void event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);
    LV_LOG_USER("Button %s clicked", lv_msgbox_get_active_btn_text(obj));
}

void lv_example_msgbox_1(void)
{
    static const char * btns[] = {"Apply", "Close", ""};

    lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "This is a message box with two buttons.", btns, true);
    lv_obj_add_event_cb(mbox1, event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_center(mbox1);
}


static void value_changed_event_cb(lv_event_t * e);

void lv_example_arc_1(lv_obj_t * obj)
{
    lv_obj_t * label = lv_label_create(obj);

    /*Create an Arc*/
    lv_obj_t * arc = lv_arc_create(obj);
    lv_obj_set_size(arc, 300, 300);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_value(arc, 10);
    lv_obj_center(arc);
    lv_obj_add_event_cb(arc, value_changed_event_cb, LV_EVENT_VALUE_CHANGED, label);

    /*Manually update the label for the first time*/
    lv_event_send(arc, LV_EVENT_VALUE_CHANGED, NULL);
}

static void value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);

    lv_label_set_text_fmt(label, "%d%%", lv_arc_get_value(arc));

    /*Rotate the label to the current position of the arc*/
    lv_arc_rotate_obj_to_angle(arc, label, 25);
}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        lv_keyboard_set_textarea(kb, ta);
        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }

    if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
}

void lv_example_keyboard_1(lv_obj_t * obj)
{
    /*Create a keyboard to use it with an of the text areas*/
    lv_obj_t * kb = lv_keyboard_create(obj);

    /*Create a text area. The keyboard will write here*/
    lv_obj_t * ta;
    ta = lv_textarea_create(obj);
    lv_obj_align(ta, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(ta, "Hello");
    lv_obj_set_size(ta, 140, 80);

    ta = lv_textarea_create(obj);
    lv_obj_align(ta, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_set_size(ta, 140, 80);

    lv_keyboard_set_textarea(kb, ta);
}

#include "ui/ui/jl_images/usr_pic_index.h"
void lv_example_tileview_1(void)
{
#if 0
    open_fd("usr_nor");

    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_watchface_00_bg_dat], &ui_test);
    lv_obj_t *img_obj = lv_img_create(lv_scr_act());
    lv_img_set_src(img_obj, &ui_test);
    lv_obj_set_align(img_obj, LV_ALIGN_CENTER);

    // lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_menu_icon_80x80_00_dat], &ui_test1);
    // lv_obj_t *img_obj0 = lv_img_create(img_obj);
    // lv_img_set_src(img_obj0, &ui_test1);
    // lv_obj_set_align(img_obj0, LV_ALIGN_CENTER);

    
    // lv_obj_t *img_obj1 = lv_img_create(lv_scr_act());
    // lv_img_set_src(img_obj1, "A:"A_UI_CLOCKWISE_HOUR);//58hz
    // lv_obj_set_align(img_obj1, LV_ALIGN_CENTER);
#endif
#if 0
    // return;

    // lv_example_msgbox_1();
    open_fd("usr_nor");

    lv_obj_t * tv = lv_tileview_create(lv_scr_act());
    // lv_obj_set_style_bg_color(tv, lv_color_hex(0x0), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * tilep = lv_tileview_add_tile(tv, 2, 1, LV_DIR_ALL);


    lv_example_arc_1(tilep);

    lv_obj_t * tile0 = lv_tileview_add_tile(tv, 1, 2, LV_DIR_ALL);
    lv_example_keyboard_1(tile0);

    lv_obj_t * tiles = lv_tileview_add_tile(tv, 0, 1, LV_DIR_ALL);


    /*Tile1: just a label*/
    lv_obj_t * tile1 = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);
    // lv_obj_t *img_obj = lv_img_create(tile1);
    // lv_obj_add_event_cb(tile1, ui_event____initial_actions1, LV_EVENT_ALL, NULL);
    
    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_RGB565_1_dat], &ui_test);
    // put_buf(ui_test.data, 16);
    lv_obj_t *img_obj = lv_img_create(tile1);
    lv_img_set_src(img_obj, &ui_test);
    // lv_obj_set_size(img_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(img_obj, LV_ALIGN_CENTER);
    lv_timer_create(test_timer_cb11, 20, img_obj); // 每1000毫秒（1秒）触发一次
    lv_obj_t * label2 = lv_label_create(img_obj);
    lv_label_set_text(label2, "FILE_RGB565_1_dat");
    lv_obj_center(label2);

//     /*Tile2: a button*/
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 1, 0, LV_DIR_ALL);

    lv_obj_t * btn = lv_btn_create(tile2);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Scroll up or right");

    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn);

    /*Tile3: a list*/
    lv_obj_t * tile3 = lv_tileview_add_tile(tv, 1, 1, LV_DIR_ALL);

    
//     lv_obj_t * tile4 = lv_tileview_add_tile(tv, 1, 0, LV_DIR_ALL);
//     lv_obj_t * img4 = lv_img_create(tile4);
//     lv_img_set_src(img4, "A:bgjpg2.jpg");
//     lv_obj_set_size(img4, 240, 272);

//     lv_obj_t * tile5 = lv_tileview_add_tile(tv, 2, 0, LV_DIR_ALL);
//      img5 = lv_img_create(tile5);
//     lv_img_set_src(img5, "B:6");//bird_2.png
//     lv_obj_set_size(img5, 454, 454);

//     tile6 = lv_tileview_add_tile(tv, 2, 1, LV_DIR_ALL);
//     lv_obj_t * img6 = lv_img_create(tile6);
//     lv_img_set_src(img6, "G:storage/sd0/C/animal.bin");
//     lv_obj_set_size(img6, 454, 454);

#define LV_IMG_DECLARE_JL(var_name) extern volatile lv_img_dsc_t var_name;
LV_IMG_DECLARE_JL(ui_img_clockwise_sec_png);    // assets\bg1.png
LV_IMG_DECLARE_JL(ui_img_clockwise_min_png);    // assets\flash.png
LV_IMG_DECLARE_JL(ui_img_clockwise_hour_png);    // assets\weather_sun_cloud.png//     lv_timer_create(test_timer_cb, 1, img6);

    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_ARGB8565_clockwise_sec_dat], &ui_img_clockwise_sec_png);
    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_ARGB8565_clockwise_min_dat], &ui_img_clockwise_min_png);
    lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_ARGB8565_clockwise_hour_dat], &ui_img_clockwise_hour_png);

    ui_sec = lv_img_create(tile3);
    lv_img_set_src(ui_sec, &ui_img_clockwise_sec_png);
    lv_obj_set_width(ui_sec, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_sec, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_sec, 0);
    lv_obj_set_y(ui_sec, -67);
    lv_obj_set_align(ui_sec, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_sec, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_sec, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_pivot(ui_sec, 15, 155);
    lv_img_set_angle(ui_sec, 12000);
    // sec_Animation(ui_sec, 0);

   ui_min = lv_img_create(tile3);
    lv_img_set_src(ui_min, &ui_img_clockwise_min_png);
    lv_obj_set_width(ui_min, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_min, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_min, 0);
    lv_obj_set_y(ui_min, -75);
    lv_obj_set_align(ui_min, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_min, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_min, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_pivot(ui_min, 9, 153);

    ui_hour = lv_img_create(tile3);
    lv_img_set_src(ui_hour, &ui_img_clockwise_hour_png);
    lv_obj_set_width(ui_hour, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_hour, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_hour, 0);
    lv_obj_set_y(ui_hour, -45);
    lv_obj_set_align(ui_hour, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_hour, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_hour, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_img_set_pivot(ui_hour, 9, 93);

//     lv_obj_t * tile7 = lv_tileview_add_tile(tv, 3, 0, LV_DIR_ALL);
//     lv_obj_t *img7 = lv_img_create(tile7);
//     lv_img_set_src(img7, "C:6.png");
//     lv_obj_set_size(img7, 176, 220);

//     lv_obj_t * tile8 = lv_tileview_add_tile(tv, 3, 1, LV_DIR_ALL);
//     lv_obj_t *img8 = lv_img_create(tile8);
// // #define 4_150_PNG			"0"
// // #define 5_PNG			"1"
// // #define 5_150_PNG			"2"
// // #define 6_PNG			"3"
// #define AQ_PNG			"4"
// #define CHICKEN_1_PNG			"5"
//     lv_img_set_src(img8, "D:"CHICKEN_1_PNG);
//     lv_obj_set_size(img8, 750, 750);
#endif
}
#endif

#if 0
//  把JPG存到内部FLASH，测试文件是temp.jpg
void save_jpg_to_flash(void);
//  退出页面调用这个释放jpg内存
void free_dial_ui_draw_custom();
//  自定义绘JPG图，自动申请buf,放在布局POST事件即可
void dial_ui_draw_custom(void *_dc, int x, int y, int width, int height, u8 *name);



struct jpeg_decfile_info {
    u8 *jpg_stream;
    int jpg_file_len;
};
static struct jpeg_decfile_info testinfo;
static u8 *pic_buf=NULL;
static int run_jpg_time = 0;
static u8 *jpg_dec_stream_input(void *__fat_info, u32 *len, u32 offset_4k)
{
    struct jpeg_decfile_info *info = (struct jpeg_decfile_info *)__fat_info;
    u8 *jpg_stream = info->jpg_stream;
    int file_len = info->jpg_file_len;
    u32 offset = offset_4k * 4096;
    u32 res_len;

    if (offset >= file_len) {
        *len = 0;
        return NULL;
    }
    res_len = file_len - offset;
    *len = (res_len > 4096) ? 4096 : res_len;

    return jpg_stream + offset;
}
//  保存到内部flash测试
#define SD_TEST     1//SD卡图片测试，摄像头存flash需要关闭


void save_jpg_to_flash(void)
{
#if SD_TEST
    //从SD拿数据JPG到BUF
    char fname[100];
    FILE *fp;

    static int cnt = 400;
    sprintf(fname, "storage/sd0/C/output/o_%04d.jpg", cnt++);

    fp = fopen(fname, "r");
    if(!fp){
        printf("SD没有这个文件：%s", fname);
        return;
    }

    //  把SD卡读到BUF
    static struct jpeg_decfile_info info;
    int len = flen(fp);
    void *pp = malloc(len);
    fread(fp, pp, len);
    fclose(fp);
    fp = NULL;
#endif

    //JPG缓存写入FLASH
    // pp   JPG缓存地址
    // len  JPG长度

    //  把BUF存到FLASH
    fp =fopen("storage/virfat_flash/C/temp.jpg", "wb+");
    if (fp == NULL) {
        printf("[error]>>>>>>>NO FLASH");
        return ;
    }
    fwrite(fp, pp, len); 
    fclose(fp);
    fp = NULL;


#if SD_TEST
    free(pp);
    
#endif
}
#endif

#if 0
// static void dial_ui_custom_draw_cb(int id, u8 *dst_buf, struct rect *dst_r, struct rect *src_r, u8 bytes_per_pixel, void *priv)
// {
// //buf区域   图片绝对地址

//     // printf("%s id %d dst %x dstltwh %d %d %d %d srcltwh %d %d %d %d pix %d p %d", __func__,
//     //     id, 
//     //     dst_buf, 
//     //     dst_r->left, dst_r->top, dst_r->height, dst_r->width, 
//     //     src_r->left, src_r->top, src_r->height, src_r->width,  
//     //     bytes_per_pixel, 
//     //     priv);
    
//     int h;
//     struct rect r;
//     struct lbuf_test_head *rbuf = NULL;
//     int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
//     int src_stride = (src_r->width * bytes_per_pixel + 3) / 4 * 4;
//     static u8 time = 0;
//     u8 rgb_format = 1;//1 :rgb_565 0:rgb_888

//     static struct rect draw;
//     // static struct jpeg_decfile_info info;
//     static struct rect draw_rect;
//     static struct rect jpeg_rect;
//     static struct rect cur_draw;
//     static struct jpeg_api_out_lines out = {0};
    
//     static struct jpeg_decoder_fd *jpg_fd;

//     if (1) {

//         if (run_jpg_time == 0) {
//             run_jpg_time = 1;
//         }

//         if (run_jpg_time > 0) {
//             /* printf("dst [%d, %d, %d, %d],  src [%d, %d, %d, %d]\n", dst_r->left, dst_r->top, dst_r->width, dst_r->height, src_r->left, src_r->top, src_r->width, src_r->height); */
// #if USE_JPG_DEMO
//             cur_draw.left = dst_r->left;
//             cur_draw.top = dst_r->top;
//             cur_draw.width = dst_r->width;
//             cur_draw.height = dst_r->height;

//             out.jpeg_page_rect.left = src_r->left; //, , ,  ;
//             out.jpeg_page_rect.top = src_r->top;
//             out.jpeg_page_rect.width = src_r->width;//hSPI_CAMERA_W;
//             out.jpeg_page_rect.height = src_r->height;//280;

//             //需要合成的范围，该范围必须在jpeg范围内，相对于背景
//             memcpy(&out.control_draw, &out.jpeg_page_rect, sizeof(struct rect));

//             if (!get_rect_cover(&out.jpeg_page_rect, &cur_draw, &r)) {
//                 printf("jpeg not in area1\n");
//             }

//             if (!get_rect_cover(&out.control_draw, &r, &draw_rect)) {
//                 printf("jpeg 0not in area2\n");
//             }

//             jpg_fd = jpeg_dec_get_fd();
//             jpeg_rect.left = draw_rect.left - out.jpeg_page_rect.left;
//             jpeg_rect.top = draw_rect.top - out.jpeg_page_rect.top;
//             jpeg_rect.width = draw_rect.width;
//             jpeg_rect.height = draw_rect.height;

//             jpeg_dec_start(&cur_draw, &draw_rect, &jpeg_rect, dst_buf);


// #endif
//             run_jpg_time++;
//         }
//     }
// }




// //  退出页面调用这个释放jpg内存
// void free_dial_ui_draw_custom(){
//     if(run_jpg_time){
//         jpeg_dec_uninit();
//         run_jpg_time = 0;
//     }
//     if(pic_buf){
//         free(pic_buf);
//         pic_buf = NULL;
//     }
// }


// //  自定义绘JPG图，自动申请buf
// void dial_ui_draw_custom(void *_dc, int x, int y, int width, int height, u8 *name)
// {
//     if(name == NULL || name[0]=='\0'){
//         return;
//     }

//     printf("ui_draw_custom 0x%x, %d\n", testinfo.jpg_stream, testinfo.jpg_file_len); 

//     static u8 last_name[20];
//     if(strcmp(last_name, name)) {    
//         memset(last_name, 0, sizeof(last_name));
//         strcpy(last_name, name);

//         if(pic_buf){
//             free(pic_buf);
//             pic_buf = NULL;
//         }

//         int len;
//         void* fp = fopen(last_name, "r");
//         if(fp){
//             len = flen(fp);
//             pic_buf = malloc(len);
//             fread(fp, pic_buf, len);
//             fclose(fp);
//         } else {
//             printf("内部flash没有这个文件: %s", name);
//             return;
//         }        
//         testinfo.jpg_stream     =   pic_buf;//rbuf->data;
//         testinfo.jpg_file_len   =   len;//rbuf->len;
//     }    


//     jpeg_dec_uninit();

//     u8 rgb_format = 1;//1 :rgb_565 0:rgb_888
//     char err = jpeg_dec_init(&testinfo, rgb_format, jpg_dec_stream_input);
//     run_jpg_time = 0;

//     if (err == -1) {
//         printf("\n [ERROR] %s -[jpg_dec_init_fail] %d\n", __FUNCTION__, __LINE__);
//         return;
//     }

//     run_jpg_time++;
    
//     ui_draw(_dc, NULL, x, y, width, height, dial_ui_custom_draw_cb, NULL, NULL, DRAW_DEMO);

// }

#endif

/*----------------------------------------------------------------------------*/
/**@brief   获取外挂flash资源文件的物理地址
   @param   path:文件路径, e.g "storage/virfat_flash/C/ui/scale454.bin"
   @return  文件的flash地址
   @note
*/
/*----------------------------------------------------------------------------*/
u32 get_file_addr(char *path)
{
    struct flash_file_info ui_resfile_info;
    int ret = ui_res_flash_info_get(&ui_resfile_info, path, "res");
    ASSERT(!ret);
    u32 file_addr = ui_resfile_info.tab[0] + ui_resfile_info.offset;//计算文件的物理地址
    ui_res_flash_info_free(&ui_resfile_info, "res");

    return file_addr;
}
