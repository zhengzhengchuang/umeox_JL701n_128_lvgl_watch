#include "system/includes.h"
#include "clock_cfg.h"
#include "asm/imb.h"
#include "asm/lcd_buffer_manager.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "ui/ui_pic_index.h"
#include "ui/ui_str_index.h"
#include "res/resfile.h"
#include "res/jpeg_decoder.h"
#include "asm/math_fast_function.h"


#ifdef JPEG_DEMO_ENABLE

#if 0// JPEG_DEMO_ENABLE


extern void imb_task_lock();
extern void imb_task_unlock();

struct jpeg_api_out_lines {
    struct rect jpeg_page_rect;
    struct rect cur_draw;
    struct rect control_draw;//控件相对背景的实际范围，图片可能比控件大
    u8 *outbuf;
};

struct jpeg_api_info {
    int id;

    u8 out_format;
    void *fat_info;
    jpg_stream_input jpg_ginfo_cb;
    struct jpeg_decoder_fd *jpg_fd;
};

static struct flash_file_info g_ui_resfile_info = {0};
static struct flash_file_info g_jpg_flash_info = {0};
static RESFILE *g_ui_resfile = NULL;

int jpeg_api_init()
{
    int ret = 0;
    clock_add_set(DEC_UI_CLK);

    //打开图片资源文件
    g_ui_resfile = res_fopen(RES_PATH"ui/ui.res", "r");
    ASSERT(g_ui_resfile);
    //获取图片资源文件flash存储信息
    ret = ui_res_flash_info_get(&g_ui_resfile_info, RES_PATH"ui/ui.res", "res");
    ASSERT(!ret);

    return 0;
}

int jpeg_api_uninit()
{
    if (g_ui_resfile) {
        res_fclose(g_ui_resfile);
        g_ui_resfile = NULL;
        ui_res_flash_info_free(&g_ui_resfile_info, "res");
    }

    return 0;
}

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

    int ret = open_image_by_id(0, g_ui_resfile, f, id & 0xff, id >> 16);
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


int jpeg_api_open(struct jpeg_api_info *info)
{
    int ret;
    struct image_file file;

    info->fat_info = jpeg_api_get_finfo(info->id, &file);
    if (info->fat_info == NULL) {
        return -1;
    }

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

extern int lcd_draw_area(u8 index, u8 *lcd_buf, int left, int top, int width, int height, int wait);

struct ui_display {
    struct lcd_interface *lcd;
};
static struct ui_display display = {0};
#define __this (&display)

static void jpeg_api_task(void *arg)
{
    int msg[32];
    int ret;

    printf("jpeg_api_task.\n");

    //屏初始化
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

    u16 colums = info.width;
    u16 lines = len / info.buf_num / info.stride;

    struct jpeg_api_info api_info = {0};
    struct jpeg_api_out_lines out = {0};
    struct jpeg_decoder_fd *jpg_fd = NULL;
    int h, width, height;
    u32 pixel_lines;
    u32 pixel_len;
    u8 plane_num;
    u8 *pre_baddr;

    api_info.id = PAGE0_e4c4_TEST0;
    api_info.out_format = info.color_format;
    api_info.jpg_ginfo_cb = NULL;
    api_info.jpg_fd = NULL;


    printf("jpeg out_format %d\n", api_info.out_format);

    jpeg_api_init();


    api_info.id = PAGE0_550b_TEST3;
    ret = jpeg_api_get_info(&api_info);
    if (ret != 0) {
        printf("get info err\n");
        while (1);
    }

    ret = jpeg_api_open(&api_info);
    if (ret != 0) {
        while ("jpg open err\n");
    }

    jpg_fd = api_info.jpg_fd;
    width = jpg_fd->info.x;//jpeg图片本身的宽度
    height = jpg_fd->info.y;//jpeg图片本身的高度


    //jpeg图片原本的范围，相对于背景
    out.jpeg_page_rect.left = 0;
    out.jpeg_page_rect.top = 0;
    out.jpeg_page_rect.width = width;
    out.jpeg_page_rect.height = height;

    //需要合成的范围，该范围必须在jpeg范围内，相对于背景
    memcpy(&out.control_draw, &out.jpeg_page_rect, sizeof(struct rect));


#if 0//只解码到BUF

    plane_num = (jpg_fd->rgb_format) ? 2 : 3;//JPEG输出的RGB格式
    pixel_lines = jpg_fd->row_mcu_num * jpg_fd->info.mcu_h;//一次解码的像素行
    pixel_len = jpg_fd->info.x * plane_num;//一次解码的一行像素行长度

    if (len < (pixel_len * pixel_lines)) {
        printf("not enough buf\n");
        while (1);
    }

    printf("width x height : %d x %d\n", width, height);
    for (h = 0; h < height; h += pixel_lines) {
        pixel_lines = ((h + pixel_lines) > height) ? (height - h) : pixel_lines;
        printf("h %d, pix %d\n", h, pixel_lines);

        out.cur_draw.left = 0;
        out.cur_draw.top = h;
        out.cur_draw.width = width;//jpeg的宽
        out.cur_draw.height = pixel_lines;

        out.outbuf = buf;

        /* memset(out.outbuf, 0, pixel_len * pixel_lines); */
        ret = jpeg_api_read_lines(&api_info, &out);
        if (ret != 0) {
            printf("jpeg dec lines err\n");
            break;
        }

        put_buf(out.outbuf, pixel_len * pixel_lines);
    }


#else //解码并推屏

    pre_baddr = buf;
    pixel_lines = lines;

    for (h = 0; h < info.height; h += pixel_lines) {
        pixel_lines = ((h + pixel_lines) > info.height) ? (info.height - h) : pixel_lines;

        printf("h %d, pix %d\n", h, pixel_lines);

        out.cur_draw.left = 0;
        out.cur_draw.top = h;
        out.cur_draw.width = info.width;//屏幕的宽
        out.cur_draw.height = pixel_lines;//屏幕推一次的高

        out.outbuf = lcd_buffer_get(0, pre_baddr);

        memset(out.outbuf, 0, len / info.buf_num);
        ret = jpeg_api_read_lines(&api_info, &out);

        pre_baddr = out.outbuf;
        lcd_buffer_pending(0, out.outbuf);
        lcd_draw_area(0, out.outbuf, 0, h, info.width, pixel_lines, 0);
    }

    lcd_wait();
#endif

    jpeg_api_close(&api_info);

    jpeg_api_uninit();


    if (__this->lcd->buffer_free) {
        if (buf) {
            __this->lcd->buffer_free(buf);
            buf = NULL;
        }
    }

    while (1) {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        if (ret != OS_TASKQ) {
            continue;
        }
    }

}


int jpeg_demo(void *arg)
{
    int err = 0;

    err = task_create(jpeg_api_task, arg, "jpeg_demo");

    return err;
}


#endif

#endif

