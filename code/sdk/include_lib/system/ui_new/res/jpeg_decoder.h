#ifndef _JPEG_DECODER_H_
#define _JPEG_DECODER_H_

#include "typedef.h"
#include "system/task.h"
#include "generic/rect.h"

extern u16 QT_TBL[0x80];
extern u16 STD_HUFFMAN_TBL[258];

#define QTAB_ADDR       ((s16 *)(QT_TBL))
#define HTAB_DC0_ADDR   ((u16 *)((u8*)STD_HUFFMAN_TBL))                         //  0x30
#define HTAB_AC0_ADDR   ((u16 *)((u8*)STD_HUFFMAN_TBL + 0x30))                  //  0xd2
#define HTAB_DC1_ADDR   ((u16 *)((u8*)STD_HUFFMAN_TBL + 0x30 + 0xd2))           //  0x30
#define HTAB_AC1_ADDR   ((u16 *)((u8*)STD_HUFFMAN_TBL + 0x30 + 0xd2 + 0x30))    //  0xd2


#define JPG_SAMP_FMT_INVALID     0x0
#define JPG_SAMP_FMT_YUV444      0x1
#define JPG_SAMP_FMT_YUV422      0x2
#define JPG_SAMP_FMT_YUV420      0x3


#define jpg_be16_to_cpu(x) (((x)[0] << 8) | (x)[1])

#define SINGLE_BUFFER_MODE    0x0
#define DOUBLE_BUFFER_MODE    0x1


enum {
    RGB888,
    RGB565,
};

struct jpeg_yuv {
    u8 *y;
    u8 *u;
    u8 *v;
};

struct jpeg_data {
    u8 *buf;
    u32 len;
};

struct jpeg_codec_handle {
    JL_JPG_TypeDef *reg;
    struct jpeg_decoder_fd *decoder_fd;
    OS_SEM sem;
};

enum std_markers {
    DQT  = 0xDB,
    SOF  = 0xC0,
    DHT  = 0xC4,
    SOI  = 0xD8,
    SOS  = 0xDA,
    RST  = 0xD0,
    RST7 = 0xD7,
    EOI  = 0xD9,
    DRI  = 0xDD,
    APP0 = 0xE0,
};

struct decoder_info {
    u16 x;
    u16 y;
    u16 x_mcu_num;
    u16 y_mcu_num;
    u16 x_mcu_cnt;
    u16 y_mcu_cnt;
    u8 mcu_h;
    u8 samp_Y;
    u8 htab_Y;
    u8 htab_Cr;
    u8 htab_Cb;
    u8 qtab_Y;
    u8 qtab_Cr;
    u8 qtab_Cb;
    u8 y_cnt;
};


typedef	u8 *(*jpg_stream_input)(void *__fat_info, u32 *len, u32 offset_4k);

struct jpeg_decoder_fd {
    void *parent;
    struct decoder_info info;
    struct jpeg_yuv yuv;
    struct jpeg_yuv yuv0;
    struct jpeg_yuv yuv1;
    struct jpeg_data data;
    u32 mcu_num;
    u32 head_len;
    u8  *stream;
    //u8  *stream_end;
    u8  *cur_stream;
    u32 bits_cnt;
    u32 cur_bits_cnt;
    u8  yuv_type;
    u8  last_rst_marker_seen;
    u8  DRI_enable;
    u8  next_rst;
    u8  rst_flag;
    u8  mcupnd_flag;
    u32 next_rst_bits_cnt;
    u32 restart_interval;
    u32 rst_mcu_cnt;
    u16 row_mcu_num;
    u8  rgb_format;
    u16 seg_idx;
    u16 seg_num;
    // u16 lcd_width;
    // u16 lcd_height;
    u16 need_dec_mcu_num;
    u8 data_out_mode;
    u8 buf_flag;
    u8 *fb;
    u8 *fb0;
    u8 *fb1;
    u8 *hbuf;
    u8 fend;
    u8 stop_flag;
    u8 head_buf[512];
    u8 fd_free_enable;
    void *fat_info;
    jpg_stream_input jpg_ginfo_cb;
    u32 fb_size;
    u8 *jpg_obuf;
    struct rect cur_draw;
    struct rect draw_rect;
    struct rect jpeg_rect;
};

int jpeg_dec_init(void *fat_info, u8 lcd_format, jpg_stream_input jpg_ginfo_cb);
int jpeg_dec_start(struct rect *cur_draw, struct rect *draw_rect, struct rect *jpeg_rect, void *dec_out_buf);
struct jpeg_decoder_fd *jpeg_dec_get_fd();
void jpeg_dec_stop();
void jpeg_dec_uninit();
int jpeg_decoder_open(struct jpeg_decoder_fd *fd);
int _jpeg_parse_header(struct jpeg_decoder_fd *fd, u8 *buf, int len);
int jpeg_decode_area(struct jpeg_decoder_fd *decoder_fd);
int jpeg_decoder_start(void *_fd);
int jpeg_decoder_init(struct jpeg_decoder_fd *fd);
int decoder_bits_pnd_handler(struct jpeg_decoder_fd *fd);
int decoder_mcu_pnd_handler(struct jpeg_decoder_fd *fd);
int jpeg_decoder_manual_start(void *_fd);
int jpeg_decoder_close(void *fd);
int jpeg_codec_init(void);

extern u8 *jpg_ginfo(void *__fat_info, u32 *len, u32 offset_4k);

int ui_jpeg_decoder_init();
int ui_jpeg_decoder_uninit();

#endif
