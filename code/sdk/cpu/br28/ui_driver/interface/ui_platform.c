#include "ui_new/includes.h"
#include "timer.h"
#include "asm/crc16.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "ascii.h"
#include "font/font_textout.h"
#include "res/new_rle.h"
#include "res/resfile.h"
#include "res/mem_var.h"
#include "res/zz.h"
#include "res/jpeg_decoder.h"
#include "ui/res_config.h"
#include "app_config.h"
#include "dev_manager.h"
#include "app_task.h"
#include "smartbox/smartbox_task.h"
#include "fs/fs.h"
#include "asm/imb.h"
#include "asm/lcd_buffer_manager.h"
#include "ui/ui_measure.h"
#include "ui/watch_syscfg_manage.h"
#include "font/font_all.h"
#include "font/language_list.h"
#include "asm/psram_api.h"
#include "ui/qr_code.h"
#include "ui_draw/ui_basic.h"

#if TCFG_SPI_LCD_ENABLE


#define INT_MAX 2147483647
#define INT_MIN (-INT_MAX - 1)

#define imb_create_background() \
        imb_tsk_info.prior          = 1; \
        imb_tsk_info.data_src       = DATA_SRC_NONE; \
        imb_tsk_info.zip_en         = 0; \
        imb_tsk_info.in_format      = LAYER_FORMAT_SOLID; \
        imb_tsk_info.out_format     = __this->info.color_format; \
        imb_tsk_info.x_offset       = 0; \
        imb_tsk_info.y_offset       = 0; \
        imb_tsk_info.src_w          = dc->width; \
        imb_tsk_info.src_h          = dc->height; \
        imb_tsk_info.des_w          = dc->width; \
        imb_tsk_info.des_h          = dc->height; \
        imb_tsk_info.priv           = 0; \
        imb_task_list_init(imb_task_head_get(dc->index), &imb_tsk_info);

#define imb_create_image(file_info) \
        imb_tsk_info.elm_id = dc->elm->id; \
        imb_tsk_info.prior = dc->elm->prior; \
        imb_tsk_info.id = (page << 26) | (0 << 24) | (id << 8) | elm_index; \
        if (imb_tsk_info.rotate_en) { \
            imb_tsk_info.data_src       = DATA_SRC_SRAM; \
            imb_tsk_info.cur_in_flash   = 1; \
            imb_tsk_info.zip_en         = 0; \
        } else { \
            imb_tsk_info.data_src       = DATA_SRC_FLASH; \
            imb_tsk_info.cur_in_flash   = 1; \
            if (file.format == PIXEL_FMT_L1) { \
                imb_tsk_info.zip_en     = 0; \
            } else { \
                imb_tsk_info.zip_en     = 1; \
            } \
        } \
        \
        imb_tsk_info.in_format      = file.format; \
        imb_tsk_info.x_offset       = x - dc->rect_ref.left; \
        imb_tsk_info.y_offset       = y - dc->rect_ref.top; \
        imb_tsk_info.src_w          = file.width; \
        imb_tsk_info.src_h          = file.height; \
        imb_tsk_info.des_w          = /*file.width*/new_width; \
        imb_tsk_info.des_h          = /*file.height*/new_height; \
        imb_tsk_info.ff_info        = file_info; \
        \
        if (imb_tsk_info.in_format == PIXEL_FMT_JPEG) { \
            imb_tsk_info.data_src       = DATA_SRC_FLASH; \
            imb_tsk_info.cur_in_flash   = 1; \
            imb_tsk_info.zip_en         = 0; \
            imb_tsk_info.lcd_width      = dc->width; \
            imb_tsk_info.lcd_height     = dc->height; \
            imb_tsk_info.lcd_format     = __this->info.color_format; \
            imb_tsk_info.cb             = imb_jpeg_cb; \
            imb_tsk_info.quote          = 1; \
        } else if ((imb_tsk_info.in_format == PIXEL_FMT_L1) || (imb_tsk_info.in_format == PIXEL_FMT_A8)) { \
            imb_tsk_info.l1_type        = L1_IMAGE; \
        } \
        \
        if ((imb_tsk_info.data_src == DATA_SRC_SRAM) || (imb_tsk_info.data_src == DATA_SRC_PSRAM)) { \
            if ((p) && (!strcmp(p->name, "task00"))) { \
                if (p->id != imb_tsk_info.id) { \
                    imb_task_file_info_release(p); \
                    imb_task_lut_tab_release(p); \
                    imb_tsk_info.priv       = (u32)&file; \
                } \
            } else if ((p) && (p->id == imb_tsk_info.id)) { \
                if(imb_tsk_info.in_format != PIXEL_FMT_JPEG) { \
                    ASSERT(p->dat_src_adr); \
                } \
                if(p->adr_map_adr) { /* 存放在flash中的图片从非旋转状态切换到旋转状态 */ \
                    imb_task_file_info_release(p); \
                    imb_task_lut_tab_release(p); \
                    p->adr_map_adr = 0;\
                    p->dat_src_adr = 0;\
                    imb_tsk_info.priv = (u32)&file; \
                } else { \
                    imb_tsk_info.priv = p->dat_src_adr; \
                    imb_tsk_info.cur_in_flash   = 0; \
                } \
            } else { \
                if (p) { \
                    imb_task_src_adr_release(p); \
                    imb_task_lut_tab_release(p); \
                } \
                imb_tsk_info.priv       = (u32)&file; \
            } \
        } else if (imb_tsk_info.data_src == DATA_SRC_FLASH) { \
            if ((p) && (p->id != imb_tsk_info.id || p->src_w != file.width || p->src_h != file.height || p->cur_format != file.format)) { \
                imb_task_file_info_release(p); \
                imb_task_lut_tab_release(p); \
            } \
            imb_tsk_info.priv       = (u32)&file; \
        } \
        \
        if (p) { \
            imb_task_reset(imb_task_head_get(dc->index), p, &imb_tsk_info); \
        } else { \
            imb_task_list_add(imb_task_head_get(dc->index), &imb_tsk_info); \
        }

#define imb_create_strpic() \
        imb_tsk_info.elm_id         = dc->elm->id; \
        imb_tsk_info.prior          = dc->elm->prior; \
        imb_tsk_info.id             = (dc->page << 26) | (1 << 24) | (ui_language_get() << 19) | (id << 8) | elm_index; \
        imb_tsk_info.data_src       = DATA_SRC_FLASH; \
        imb_tsk_info.cur_in_flash   = 1; \
        if (file.compress == 1) { \
            imb_tsk_info.in_format  = LAYER_FORMAT_A8; \
            imb_tsk_info.zip_en     = 1; \
        } else { \
            imb_tsk_info.in_format  = LAYER_FORMAT_L1; \
            imb_tsk_info.zip_en     = 0; \
        } \
        imb_tsk_info.x_offset       = x - dc->rect_ref.left; \
        imb_tsk_info.y_offset       = y - dc->rect_ref.top; \
        imb_tsk_info.src_w          = file.width; \
        imb_tsk_info.src_h          = file.height; \
        imb_tsk_info.des_w          = file.width; \
        imb_tsk_info.des_h          = file.height; \
        imb_tsk_info.text_color     = text->color; \
        imb_tsk_info.ff_info        = ui_get_str_file_info_by_pj_id(dc->prj); \
        if((file.format == TEXT_1BPP) || (file.format == PIXEL_FMT_L1)) { \
            imb_tsk_info.cb         = NULL; \
            imb_tsk_info.outcb      = NULL; \
            imb_tsk_info.in_format  = LAYER_FORMAT_L1; \
            imb_tsk_info.l1_type    = L1_TEXT; \
            imb_tsk_info.zip_en     = 0; \
        } else if ((file.format != TEXT_1BPP) && (file.compress == 1)) { \
            imb_tsk_info.cb         = NULL; \
            imb_tsk_info.outcb      = NULL; \
            imb_tsk_info.in_format  = LAYER_FORMAT_A8; \
            imb_tsk_info.zip_en     = 1; \
            imb_tsk_info.l1_type    = L1_TEXT; \
        } else {\
            imb_tsk_info.in_format  = LAYER_FORMAT_A8; \
            imb_tsk_info.l1_type    = L1_TEXT; \
            imb_tsk_info.zip_en     = 0; \
            imb_tsk_info.cb         = strpic_cb_in; \
            imb_tsk_info.outcb      = strpic_cb_out; \
        } \
        if ((imb_tsk_info.data_src == DATA_SRC_SRAM) || (imb_tsk_info.data_src == DATA_SRC_PSRAM)) { \
            if ((p) && (p->id == imb_tsk_info.id)) { \
                if(p->adr_map_adr) { /* 存放在flash中的图片从非旋转状态切换到旋转状态 */ \
                    imb_task_file_info_release(p); \
                    imb_task_lut_tab_release(p); \
                    p->adr_map_adr = 0;\
                    p->dat_src_adr = 0;\
                    imb_tsk_info.priv = (u32)&file; \
                } else { \
                    ASSERT(p->dat_src_adr); \
                    imb_tsk_info.priv = p->dat_src_adr; \
                    imb_tsk_info.cur_in_flash = 0; \
                } \
            } else { \
                if (p) { \
                    imb_task_src_adr_release(p); \
                    imb_task_lut_tab_release(p); \
                } \
                imb_tsk_info.priv       = (u32)&file; \
            } \
        } else if (imb_tsk_info.data_src == DATA_SRC_FLASH) { \
            if ((p) && (p->id != imb_tsk_info.id || p->src_w != file.width || p->src_h != file.height || p->cur_format != file.format)) { \
                imb_task_file_info_release(p); \
                imb_task_lut_tab_release(p); \
            } \
            imb_tsk_info.priv       = (u32)&file; \
        } \
        if (p) { \
            imb_task_reset(imb_task_head_get(dc->index), p, &imb_tsk_info); \
        } else { \
            imb_task_list_add(imb_task_head_get(dc->index), &imb_tsk_info); \
        }

#define imb_create_color() \
        imb_tsk_info.elm_id         = dc->elm->id; \
        imb_tsk_info.prior          = dc->elm->prior; \
        imb_tsk_info.id             = (dc->page << 26) | (2 << 24) | (id << 8) | elm_index; \
        imb_tsk_info.data_src       = DATA_SRC_NONE; \
        imb_tsk_info.zip_en         = 0; \
        imb_tsk_info.in_format      = LAYER_FORMAT_SOLID; \
        if(imb_tsk_info.task_invisible || ((dc->rect.width < 1024) && (dc->rect.height < 512))) { \
            imb_tsk_info.x_offset       = dc->rect.left - dc->rect_ref.left; \
            imb_tsk_info.y_offset       = dc->rect.top - dc->rect_ref.top; \
            imb_tsk_info.src_w          = dc->rect.width; \
            imb_tsk_info.src_h          = dc->rect.height; \
            imb_tsk_info.des_w          = dc->rect.width; \
            imb_tsk_info.des_h          = dc->rect.height; \
        } else { \
            imb_tsk_info.x_offset       = dc->draw.left - dc->rect_ref.left; \
            imb_tsk_info.y_offset       = dc->draw.top - dc->rect_ref.top; \
            imb_tsk_info.src_w          = dc->draw.width; \
            imb_tsk_info.src_h          = dc->draw.height; \
            imb_tsk_info.des_w          = dc->draw.width; \
            imb_tsk_info.des_h          = dc->draw.height; \
        } \
        imb_tsk_info.priv           = acolor; \
        \
        if (p) { \
            imb_task_reset(imb_task_head_get(dc->index), p, &imb_tsk_info); \
        } else { \
            imb_task_list_add(imb_task_head_get(dc->index), &imb_tsk_info); \
        }


#define imb_create_text(_width, _height) \
        imb_tsk_info.prior = dc->elm->prior; \
        imb_tsk_info.rect.left = x - dc->rect_ref.left; \
        imb_tsk_info.rect.top = y - dc->rect_ref.top; \
        imb_tsk_info.rect.width = (_width + 31) / 32 * 32; \
        imb_tsk_info.rect.height = _height; \
        \
        struct rect draw; \
        draw.left = dc->draw.left - dc->rect_ref.left; \
        draw.top = dc->draw.top - dc->rect_ref.top; \
        draw.width = dc->draw.width; \
        draw.height = dc->draw.height; \
        \
        if (!get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.draw)) { \
            if (p) { \
                imb_task_enable(p, false); \
                if (pixbuf) { \
                    br28_free(pixbuf); \
                    pixbuf = NULL;\
                } \
                return -EFAULT; \
            } else { \
                imb_tsk_info.task_invisible = true; \
            } \
        } \
        get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.crop); \
        \
        imb_tsk_info.elm_id         = dc->elm->id; \
        imb_tsk_info.id             = (dc->page << 26) | (3 << 24) | (id << 8) | elm_index; \
        imb_tsk_info.cur_in_flash   = 0; \
        imb_tsk_info.data_src       = DATA_SRC_SRAM; \
        imb_tsk_info.zip_en         = 0; \
        imb_tsk_info.in_format      = LAYER_FORMAT_L1; \
        imb_tsk_info.x_offset       = x - dc->rect_ref.left; \
        imb_tsk_info.y_offset       = y - dc->rect_ref.top; \
        imb_tsk_info.src_w          = (_width + 31) / 32 * 32; \
        imb_tsk_info.src_h          = _height; \
        imb_tsk_info.des_w          = (_width + 31) / 32 * 32; \
        imb_tsk_info.des_h          = _height; \
        imb_tsk_info.text_color     = text->color; \
        imb_tsk_info.ui_malloc = 1; \
        if ((p) && (p->id == imb_tsk_info.id) && !p->scroll) { \
            ASSERT(p->dat_src_adr); \
            imb_tsk_info.priv           = p->dat_src_adr; \
            ASSERT(imb_tsk_info.priv, ",%s, dat_src_adr : 0x%x\n", p->name, p->dat_src_adr); \
        } else { \
            if (p) { \
                imb_task_src_adr_release(p); \
                imb_task_lut_tab_release(p); \
            } \
            imb_tsk_info.priv           = (u32)pixbuf; \
            ASSERT(imb_tsk_info.priv, ", pixbuf : 0x%x\n", pixbuf); \
        } \
        if (p) { \
            imb_task_reset(imb_task_head_get(dc->index), p, &imb_tsk_info); \
        } else { \
            imb_task_list_add(imb_task_head_get(dc->index), &imb_tsk_info); \
        }


#define imb_create_draw() \
        imb_tsk_info.elm_id         = elm->id; \
        imb_tsk_info.prior          = dc->elm->prior; \
        imb_tsk_info.id             = (dc->page << 26) | (0 << 24) | (0 << 8) | elm_index; \
        imb_tsk_info.data_src       = DATA_SRC_SRAM; \
        imb_tsk_info.cur_in_flash   = 0; \
        imb_tsk_info.zip_en         = 0; \
        imb_tsk_info.in_format      = 0; \
        imb_tsk_info.x_offset       = x; \
        imb_tsk_info.y_offset       = y; \
        imb_tsk_info.src_w          = width; \
        imb_tsk_info.src_h          = height; \
        imb_tsk_info.des_w          = width; \
        imb_tsk_info.des_h          = height; \
        imb_tsk_info.ff_info        = NULL; \
        imb_tsk_info.cb             = imb_draw_cb; \
        imb_tsk_info.cb_priv        = cb ? cb : buf; \
        imb_tsk_info.quote          = 1; \
        imb_tsk_info.user_cb        = cb ? 1 : 0; \
        imb_tsk_info.cb_id          = id; \
        \
        if (p) { \
            imb_task_reset(imb_task_head_get(dc->index), p, &imb_tsk_info); \
            if (priv && priv_len) { \
                ASSERT(imb_task_check_crc(p)); \
                memcpy(p->priv_buf, priv, priv_len); \
                imb_task_calc_crc(p); \
            } \
        } else { \
            p = imb_task_list_add(imb_task_head_get(dc->index), &imb_tsk_info); \
            if (priv && priv_len) { \
                ASSERT(imb_task_check_crc(p)); \
                memcpy(p->priv_buf, priv, priv_len); \
                imb_task_calc_crc(p); \
            } \
        }

/***************************************************************/
//此表为语言偏移表,设定指定语言对应指定id，读取指定文件偏移量
/* static const LANG_TABLE lange_offset_table[16] = { */
/*     [CP874] = { */
/*         CP874,  //指向语言id */
/*         0,//  指向pixel文件的偏移 */
/*         0xf400,  //指向tabfile的偏移 */
/*     }, */
/*     [CP937] = { */
/*         CP937, */
/*         0, */
/*         0,// */
/*     }, */
/*     [CPKSC] = { */
/*         CPKSC, */
/*         0xbc050, */
/*         0x13560,// */
/*     }, */
/*     [CP1252] = { */
/*         CP1252, */
/*         0, */
/*         0x45864, */
/*     }, */
/*     [CP1258] = { */
/*         CP1258, */
/*         0, */
/*         0x2cac0,// */
/*     }, */
/*     [CP1256] = { */
/*         CP1256, */
/*         0, */
/*         0x34f4c,// */
/*     }, */
/*     [CP1251] = { */
/*         CP1251, */
/*         0, */
/*         0x39192,// */
/*     }, */
/*     [CP1254] = { */
/*         CP1254, */
/*         0, */
/*         0x3d3d8,// */
/*     }, */
/*     [CP1250] = { */
/*         CP1250, */
/*         0, */
/*         0x4161e,// */
/*     }, */
/*     [CP1255] = { */
/*         CP1255, */
/*         0, */
/*         0x49aaa,// */
/*     }, */
/* }; */
/* 字号控制,混合语言不同字号选择对应偏移表:16-20-24-28-32 */
#define FONT_SIZE 16
#if (FONT_SIZE == 16)
static const LANG_TABLE lange_offset_table[16] = {
    /***************F_Mix.PIX***************/
    [CP937] = {
        CP937,
        0,
        0,
    },
    [CPBIG5] = {
        CPBIG5,
        0x3FE46,
        0xAFD0,
    },
    [CPSJIS] = {
        CPSJIS,
        0xAD0EC,
        0x196D0,
    },
    [CPKSC] = {
        CPKSC,
        0xF67F2,
        0x27AD0,
    },
    /***************F_Mix.PIX***************/

    /***************F_AscMix.PIX***************/
    [CP1252] = {
        CP1252,
        0x0,
        0x3CDD0,
    },
    [CP1250] = {
        CP1250,
        0x12DE,
        0x41016,
    },
    [CP1251] = {
        CP1251,
        0x25A6,
        0x4525C,
    },
    [CP1253] = {
        CP1253,
        0x3954,
        0x494A2,
    },
    [CP1254] = {
        CP1254,
        0x4B60,
        0x4D6E8,
    },
    [CP1257] = {
        CP1257,
        0x5E04,
        0x5192E,
    },
    [CP1258] = {
        CP1258,
        0x7056,
        0x55B74,
    },
    [CP874] = {
        CP874,
        0x8BC6,
        0x59DBA,
    },
    /***************F_AscMix.PIX***************/
};
#elif (FONT_SIZE == 20)
static const LANG_TABLE lange_offset_table[16] = {
    /***************F_Mix.PIX***************/
    [CP937] = {
        CP937,
        0,
        0,
    },
    [CPBIG5] = {
        CPBIG5,
        0x77CBE,
        0xAFD0,
    },
    [CPSJIS] = {
        CPSJIS,
        0x1447B0,
        0x196D0,
    },
    [CPKSC] = {
        CPKSC,
        0x1CE2D6,
        0x27AD0,
    },
    /***************F_Mix.PIX***************/

    /***************F_AscMix.PIX***************/
    [CP1252] = {
        CP1252,
        0x0,
        0x3CDD0,
    },
    [CP1250] = {
        CP1250,
        0x1F7A,
        0x41016,
    },
    [CP1251] = {
        CP1251,
        0x3E89,
        0x4525C,
    },
    [CP1253] = {
        CP1253,
        0x5FA4,
        0x494A2,
    },
    [CP1254] = {
        CP1254,
        0x7DB0,
        0x4D6E8,
    },
    [CP1257] = {
        CP1257,
        0x9CB5,
        0x5192E,
    },
    [CP1258] = {
        CP1258,
        0xBB2D,
        0x55B74,
    },
    [CP874] = {
        CP874,
        0xE950,
        0x59DBA,
    },
    /***************F_AscMix.PIX***************/
};
#elif (FONT_SIZE == 24)
static const LANG_TABLE lange_offset_table[16] = {
    /***************F_Mix.PIX***************/
    [CP937] = {
        CP937,
        0,
        0,
    },
    [CPBIG5] = {
        CPBIG5,
        0x8FC16,
        0xAFD0,
    },
    [CPSJIS] = {
        CPSJIS,
        0x185604,
        0x196D0,
    },
    [CPKSC] = {
        CPKSC,
        0x22A9CA,
        0x27AD0,
    },
    /***************F_Mix.PIX***************/

    /***************F_AscMix.PIX***************/
    [CP1252] = {
        CP1252,
        0x0,
        0x3CDD0,
    },
    [CP1250] = {
        CP1250,
        0x2346,
        0x41016,
    },
    [CP1251] = {
        CP1251,
        0x4630,
        0x4525C,
    },
    [CP1253] = {
        CP1253,
        0x6B74,
        0x494A2,
    },
    [CP1254] = {
        CP1254,
        0x8CFE,
        0x4D6E8,
    },
    [CP1257] = {
        CP1257,
        0xAFBA,
        0x5192E,
    },
    [CP1258] = {
        CP1258,
        0xD1E6,
        0x55B74,
    },
    [CP874] = {
        CP874,
        0x105D9,
        0x59DBA,
    },
    /***************F_AscMix.PIX***************/
};
#elif (FONT_SIZE == 28)
static const LANG_TABLE lange_offset_table[16] = {
    /***************F_Mix.PIX***************/
    [CP937] = {
        CP937,
        0,
        0,
    },
    [CPBIG5] = {
        CPBIG5,
        0xDF9E6,
        0xAFD0,
    },
    [CPSJIS] = {
        CPSJIS,
        0x25DB1C,
        0x196D0,
    },
    [CPKSC] = {
        CPKSC,
        0x35EBA2,
        0x27AD0,
    },
    /***************F_Mix.PIX***************/

    /***************F_AscMix.PIX***************/
    [CP1252] = {
        CP1252,
        0x0,
        0x3CDD0,
    },
    [CP1250] = {
        CP1250,
        0x348A,
        0x41016,
    },
    [CP1251] = {
        CP1251,
        0x6858,
        0x4525C,
    },
    [CP1253] = {
        CP1253,
        0x9F6A,
        0x494A2,
    },
    [CP1254] = {
        CP1254,
        0xD104,
        0x4D6E8,
    },
    [CP1257] = {
        CP1257,
        0x104AE,
        0x5192E,
    },
    [CP1258] = {
        CP1258,
        0x13770,
        0x55B74,
    },
    [CP874] = {
        CP874,
        0x1865A,
        0x59DBA,
    },
    /***************F_AscMix.PIX***************/
};
#elif (FONT_SIZE == 32)
static const LANG_TABLE lange_offset_table[16] = {
    /***************F_Mix.PIX***************/
    [CP937] = {
        CP937,
        0,
        0,
    },
    [CPBIG5] = {
        CPBIG5,
        0xFF906,
        0xAFD0,
    },
    [CPSJIS] = {
        CPSJIS,
        0x2B438C,
        0x196D0,
    },
    [CPKSC] = {
        CPKSC,
        0x3D9F92,
        0x27AD0,
    },
    /***************F_Mix.PIX***************/

    /***************F_AscMix.PIX***************/
    [CP1252] = {
        CP1252,
        0x0,
        0x3CDD0,
    },
    [CP1250] = {
        CP1250,
        0x3BD6,
        0x41016,
    },
    [CP1251] = {
        CP1251,
        0x76D4,
        0x4525C,
    },
    [CP1253] = {
        CP1253,
        0xB5B6,
        0x494A2,
    },
    [CP1254] = {
        CP1254,
        0xEE44,
        0x4D6E8,
    },
    [CP1257] = {
        CP1257,
        0x12916,
        0x5192E,
    },
    [CP1258] = {
        CP1258,
        0x162D4,
        0x55B74,
    },
    [CP874] = {
        CP874,
        0x1BCB2,
        0x59DBA,
    },
    /***************F_AscMix.PIX***************/
};
#endif
/***************************************************************/
#define UI_DEBUG 0
/* #define UI_BUF_CALC */

#if (UI_DEBUG == 1)

#define UI_PUTS puts
#define UI_PRINTF printf

#else

#define UI_PUTS(...)
#define UI_PRINTF(...)

#endif

#define _RGB565(r,g,b)  (u16)((((r)>>3)<<11)|(((g)>>2)<<5)|((b)>>3))
#define UI_RGB565(c)  \
        _RGB565((c>>16)&0xff,(c>>8)&0xff,c&0xff)

#define TEXT_MONO_CLR 0x555aaa
#define TEXT_MONO_INV 0xaaa555
#define RECT_MONO_CLR 0x555aaa
#define BGC_MONO_SET  0x555aaa


struct fb_map_user {
    u16 xoffset;
    u16 yoffset;
    u16 width;
    u16 height;
    u8  *baddr;
    u8  *yaddr;
    u8  *uaddr;
    u8  *vaddr;
    u8 transp;
    u8 format;
};

struct fb_var_screeninfo {
    u16 s_xoffset;            //显示区域x坐标
    u16 s_yoffset;            //显示区域y坐标
    u16 s_xres;               //显示区域宽度
    u16 s_yres;               //显示区域高度
    u16 v_xoffset;      //屏幕的虚拟x坐标
    u16 v_yoffset;      //屏幕的虚拟y坐标
    u16 v_xres;         //屏幕的虚拟宽度
    u16 v_yres;         //屏幕的虚拟高度
    u16 rotate;
};

struct window_head {
    u32 offset;
    u32 len;
    u32 ptr_table_offset;
    u16 ptr_table_len;
    u16 crc_data;
    u16 crc_table;
    u16 crc_head;
};

struct ui_file_head {
    u8  res[16];
    u8 type;
    u8 window_num;
    u16 prop_len;
    u8 rotate;
    u8 rev[3];
};



#if     TCFG_VIRFAT_INSERT_FLASH_ENABLE
#define UI_FAT_PHY_FLASH  PHY_JL_INSERT_FLASH
#else
#define UI_FAT_PHY_FLASH  PHY_JL_EXTERN_FLASH
#endif


#if     TCFG_VIRFAT_INSERT_FLASH_ENABLE
#define UI_FAT_PHY_BASE  TCFG_VIRFAT_INSERT_FLASH_BASE
#else
#define UI_FAT_PHY_BASE  0
#endif




struct ui_load_info ui_load_info_table[] = {
#if UI_WATCH_RES_ENABLE
    {0, UI_FAT_PHY_FLASH, UI_FAT_PHY_BASE, RES_PATH"JL/JL.sty", NULL},
    {1, UI_FAT_PHY_FLASH, UI_FAT_PHY_BASE, RES_PATH"watch/watch.sty", NULL},
    {2, UI_FAT_PHY_FLASH, UI_FAT_PHY_BASE, RES_PATH"sidebar/sidebar.sty", NULL},
    {3, UI_FAT_PHY_FLASH, UI_FAT_PHY_BASE, NULL, NULL},
    {4, PHY_JL_INSERT_FLASH, 0, UPGRADE_PATH"upgrade.sty", NULL},
#endif
    {-1, PHY_JL_EXTERN_FLASH, 0, NULL, NULL},
};



int ui_res_flash_info_get(struct flash_file_info *file_info, char *path, char *str)
{
    extern int __ui_res_flash_info_get(struct flash_file_info * file_info, char *path, char *str, u8 phy_dev, u32 phy_addr);
    return __ui_res_flash_info_get(file_info, path, str, UI_FAT_PHY_FLASH, UI_FAT_PHY_BASE);
}


static u32 ui_rotate = false;
static u32 ui_hori_mirror = false;
static u32 ui_vert_mirror = false;
static int malloc_cnt = 0;
static RESFILE *ui_file = NULL;
static RESFILE *ui_file1 = NULL;
static RESFILE *ui_file2 = NULL;
static u32 ui_file_len = 0;
static u32 last_elm_id = 0xffffffff;
/* static u8 elm_index = 0; */

static int open_resource_file();

extern __attribute__((noinline)) u32 check_ram1_size(void);
extern void *malloc_vlt(size_t size);
extern void free_vlt(void *pv);
extern void imb_task_list_set_prior(struct imb_task_head *root, int elm_id, int prior);
extern void imb_task_lock();
extern void imb_task_unlock();
static const struct ui_platform_api br28_platform_api;

struct ui_priv {
    struct ui_platform_api *api;
    struct lcd_interface *lcd;
    int window_offset;
    struct lcd_info info;
    int ui_res_init;
};
static struct ui_priv priv ALIGNED(4) = {0};
#define __this (&priv)

#ifdef UI_BUF_CALC
struct buffer {
    struct list_head list;
    u8 *buf;
    int size;
};
struct buffer buffer_used = {
    .list = {
        .next = &buffer_used.list,
        .prev = &buffer_used.list,
    },
};
#endif



char *file_name[] = {
    "JL",
    "WATCH",
    "WATCH1",
    "WATCH2",
    "WATCH3",
    "WATCH4",
    "WATCH5",
    "FONT",
};

void __attribute__((weak)) virfat_flash_get_dirinfo(void *file_buf, u32 *file_num)
{
    int i;
    for (i = 0; i < sizeof(file_name) / sizeof(file_name[0]); i++) {
        if (file_buf) {
            memcpy(file_buf + 12 * i, file_name[i], strlen(file_name[i]));
        }
    }
    *file_num = sizeof(file_name) / sizeof(file_name[0]);
}

void __attribute__((weak)) virfat_flash_erase_watch(int cmd, u32 arg)
{

}

u32 __attribute__((weak)) virfat_flash_write_watch(void *buf, u32 addr_sec, u32 len)
{
    return 0;
}

u32 __attribute__((weak)) virfat_flash_get_real_capacity() //获取实际flash容量
{
    return 0;
}

u32 __attribute__((weak)) virfat_flash_capacity() //fat上容量
{
    return 0;
}


void *br28_malloc_psram(int size)
{
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    return malloc_psram(size);
#else
    return NULL;
#endif
}


void br28_free_psram(void *buf)
{
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    free_psram(buf);
#endif
}

void br28_psram_flush_invaild_cache(u32 *begin, u32 len)
{
#if (TCFG_PSRAM_DEV_ENABLE == ENABLE_THIS_MOUDLE)
    psram_flush_invaild_cache(begin, len);
#endif
}

void *br28_malloc(int size)
{
    void *buf;
    malloc_cnt++;

    if (check_ram1_size() == 0) {
        buf = (void *)malloc(size);
    } else {
        buf = (void *)malloc_vlt(size);
    }

    /* printf("platform_malloc : 0x%x, %d\n", buf, size); */
#ifdef UI_BUF_CALC
    struct buffer *new = (struct buffer *)malloc(sizeof(struct buffer));
    new->buf = buf;
    new->size = size;
    list_add_tail(new, &buffer_used);
    printf("platform_malloc : 0x%x, %d\n", buf, size);

    struct buffer *p;
    int buffer_used_total = 0;
    list_for_each_entry(p, &buffer_used.list, list) {
        buffer_used_total += p->size;
    }
    printf("used buffer size:%d\n\n", buffer_used_total);
#endif

    return buf;
}

void *br28_zalloc(int size)
{
    void *p = br28_malloc(size);
    if (p) {
        memset(p, 0x00, size);
    }
    return p;
}

void br28_free(void *buf)
{
    /* printf("platform_free : 0x%x\n",buf); */
    if (check_ram1_size() == 0) {
        free(buf);
    } else {
        free_vlt(buf);
    }
    malloc_cnt--;

#ifdef UI_BUF_CALC
    struct buffer *p, *n;
    list_for_each_entry_safe(p, n, &buffer_used.list, list) {
        if (p->buf == buf) {
            printf("platform_free : 0x%x, %d\n", p->buf, p->size);
            __list_del_entry(p);
            free(p);
        }
    }

    int buffer_used_total = 0;
    list_for_each_entry(p, &buffer_used.list, list) {
        buffer_used_total += p->size;
    }
    printf("used buffer size:%d\n\n", buffer_used_total);
#endif
}



static void *strpic_cb_in(void *priv)
{
    struct imb_task *p = (struct imb_task *)priv;
    struct flash_file_info *file_info = p->flash_file_info;
    struct imb_task_head *head = imb_task_head_get(p->root_index);
    u32 addr = file_info->tab[0] + file_info->offset;
    u32 len = 4096 * file_info->tab_size / 4 - file_info->offset - file_info->last_tab_data_len;

    struct rect r;
    if (get_rect_cover(&head->curr_draw, &p->rect, &r) && p->cur_tsk_valid && !p->priv_buf[0]) {
        int str_len = (p->src_w + 3) / 4 * 4 * p->src_h;
        u8 *str_buf = (u8 *)br28_malloc(str_len);
        p->priv_buf[0] = (u32)str_buf;

        imb_task_lock();
        int ret = text_decompress(str_buf, str_len, addr, len);
        imb_task_unlock();

        ASSERT(!ret);

        p->cur_in_flash = 0;
        p->cur_format = LAYER_FORMAT_A8;
        p->dat_src_adr = (u32)str_buf;
        p->adr_map_adr = 0;
        u32 text_color = p->lut_tab[0];
        p->a_from_tsk = 0;
        p->r_from_tsk = ((text_color >> 11) & 0x1f) << 3;
        p->g_from_tsk = ((text_color >> 5) & 0x3f) << 2;
        p->b_from_tsk = ((text_color >> 0) & 0x1f) << 3;
        /* put_buf(writer.buffer, writer.size); */
    }

    return NULL;
}

static void *strpic_cb_out(void *priv)
{
    struct imb_task *p = (struct imb_task *)priv;

    if (p->priv_buf[0]) {
        br28_free(p->priv_buf[0]);
        p->priv_buf[0] = 0;
        p->dat_src_adr = 0;
    }

    return NULL;
}

static void *watch_malloc(int size)
{
    void *buf = (void *)malloc(size);
    return buf;
}
static void *watch_zalloc(int size)
{
    void *p = watch_malloc(size);
    if (p) {
        memset(p, 0x00, size);
    }
    return p;
}
static void watch_free(void *buf)
{
    free(buf);
}

int ui_platform_ok()
{
    return (malloc_cnt == 0);
}

static void draw_rect_range_check(struct rect *r, struct fb_map_user *map)
{
    if (r->left < map->xoffset) {
        r->left = map->xoffset;
    }
    if (r->left > (map->xoffset + map->width)) {
        r->left = map->xoffset + map->width;
    }
    if ((r->left + r->width) > (map->xoffset + map->width)) {
        r->width = map->xoffset + map->width - r->left;
    }
    if (r->top < map->yoffset) {
        r->top = map->yoffset;
    }
    if (r->top > (map->yoffset + map->height)) {
        r->top = map->yoffset + map->height;
    }
    if ((r->top + r->height) > (map->yoffset + map->height)) {
        r->height = map->yoffset + map->height - r->top;
    }

    ASSERT(r->left >= map->xoffset);
    ASSERT(r->top  >= map->yoffset);
    ASSERT((r->left + r->width) <= (map->xoffset + map->width));
    ASSERT((r->top + r->height) <= (map->yoffset + map->height));
}



__attribute__((always_inline))
void __font_pix_copy(struct font_info *info, u8 *pix, int x, int y, int height, int width)
{
    int i, j, h;
    u16 xpos;
    u16 ypos;
    if (info->flags & FONT_SHOW_SCROLL) {
        int font_lang_id = font_lang_get();
        if (font_lang_id == Arabic || font_lang_id == Hebrew || font_lang_id == UnicodeMixLeftword) { //显示方向从右到左
            if (x > info->text_width - info->xpos_offset) {
                return;
            }
        } else {
            if (x + width < info->xpos_offset) { //x + width防止字符被中断
                return;  //当解析字符串累计buf宽度未到指定位置时不填充字库buf
            } else {
                x -= info->xpos_offset; //偏移x的值，使其对应到字库buf起始位置
            }
        }
    }

    for (j = 0; j < (height + 7) / 8; j++) { /* 纵向8像素为1字节 */
        for (i = 0; i < width; i++) {
            u8 pixel = pix[j * width + i];
            int hh = height - (j * 8);
            if (hh > 8) {
                hh = 8;
            }

            if (x + i < 0) {
                continue; //x的值在文本框边界的时候可能为负，将负的部分舍弃
            }
            xpos = x + i;
            for (h = 0; h < hh; h++) {
                u16 clr = pixel & BIT(h) ? 1 : 0;
                if (clr) {
                    if (info->text_image_buf) {
                        u8 *pdisp = info->text_image_buf;
                        ypos = y + j * 8 + h;
                        if ((ypos < info->text_image_height) && (xpos < info->text_image_width)) {
                            pdisp[ypos * info->text_image_stride + xpos / 8] |= BIT(7 - xpos % 8);
                        }
                    }
                }
            } /* endof for h */
        }/* endof for i */
    }/* endof for j */
}

void l1_data_transformation(u8 *pix, u8 *pix_buf, u32 stride, int x, int y, int height, int width)
{
    int i, j, k = -1, h;

    for (j = 0; j < (height + 7) / 8; j++) { /* 纵向8像素为1字节 */
        for (i = 0; i < width; i++) {
            if ((i % 8) == 0) {
                k++;
            }
            u8 pixel = pix[j * width + i];
            int hh = height - (j * 8);
            if (hh > 8) {
                hh = 8;
            }
            for (h = 0; h < hh; h++) {
                u16 clr = pixel & BIT(h) ? 1 : 0;
                if (clr) {
                    pix_buf[(j * 8 + h) * stride + k] |= (1 << (7 - (i % 8)));
                }
            } /* endof for h */
        }/* endof for i */
        k = -1;
    }/* endof for j */
}

void __font_pix_copy2(struct draw_context *dc, int format, struct fb_map_user *map, u8 *pix, struct rect *draw, int x, int y,
                      int height, int width, int color)
{

    int w, h, z;
    u16 osd_color;
    u32 size;

    osd_color = (format == DC_DATA_FORMAT_OSD8) || (format == DC_DATA_FORMAT_OSD8A) ? color & 0xff : color & 0xffff ;

    for (h = 0; h < height; h++) { /* 纵向8像素为1字节 */
        if (((y + h) < draw->top) || ((y + h) > (draw->top + draw->height - 1))) { /* x在绘制区域，要绘制 */
            continue;
        }
        for (w = 0; w < ((width + 7) / 8 * 8); w += 8) {
            u8 pixel = pix[h * ((width + 7) / 8) + w / 8];
            int ww = width - w;
            if (ww > 8) {
                ww = 8;
            }

            for (z = 0; z < ww; z++) {
                if (((x + w + z) < draw->left) || ((x + w + z) > (draw->left + draw->width - 1))) { /* y在绘制区域，要绘制 */
                    continue;
                }
                u16 clr = pixel & BIT(7 - z) ? osd_color : 0;
                if (clr) {
                    if (platform_api->draw_point) {
                        platform_api->draw_point(dc, x + w + z, y + h, clr);
                    }
                }
            } /* endof for h */
        }/* endof for w */
    }/* endof for h */
}

static int image_str_size_check(struct draw_context *dc, int page_num, const char *txt, int *width, int *height)
{

    u16 id = ((u8)txt[1] << 8) | (u8)txt[0];
    u16 cnt = 0;
    struct image_file file = {0};
    int w = 0, h = 0;
    while (id != 0x00ff) {
        struct mem_var *list;
        if ((list = mem_var_search(0, 0, id, page_num, dc->prj)) != NULL) {
            mem_var_get(list, (u8 *)&file, sizeof(struct image_file));
        } else {
            if (open_image_by_id(dc->prj, NULL, &file, id, page_num) != 0) {
                return -EFAULT;
            }
            mem_var_add(0, 0, id, page_num, dc->prj, (u8 *)&file, sizeof(struct image_file));
        }
        w += file.width;
        cnt += 2;
        id = ((u8)txt[cnt + 1] << 8) | (u8)txt[cnt];
        if (file.height  > h) {
            h = file.height;
        }
    }
    *width = w;
    *height = h;
    return 0;
}

void platform_putchar(struct font_info *info, u8 *pixel, u16 width, u16 height, u16 x, u16 y)
{
    __font_pix_copy(info,
                    pixel,
                    (int)x,
                    (int)y,
                    height,
                    width);
}

struct file_dev {
    const char *logo;
    const char *root_path;
};


struct file_browser {
    int show_mode;
    struct rect r;
    struct vfscan *fscan;
    /* struct server *server; */
    struct ui_file_browser bro;
    struct file_dev dev;//支持三个设备
};


static int check_file_ext(const char *ext_table, const char *ext)
{
    const char *str;

    for (str = ext_table; *str != '\0'; str += 3) {
        if (0 == ASCII_StrCmpNoCase(ext, str, 3)) {
            return true;
        }
    }
    return false;
}

static const u8 MUSIC_SCAN_PARAM[] = "-t"
                                     "MP1MP2MP3"
                                     " -sn -d"
                                     ;




static int platform_file_get_dev_total()
{
    return dev_manager_get_total(1);
}



static void platform_file_browser_get_dev_info(struct ui_file_browser *_bro, u8 index)
{
    struct file_browser *bro = container_of(_bro, struct file_browser, bro);
    struct file_dev *file_dev;//支持三个设备


    if (!bro) {
        return;
    }
    struct __dev *dev = dev_manager_find_by_index(index, 0);//storage_dev_find_by_index(index);
    if (dev) {
        file_dev = &bro->dev;
        file_dev->logo = dev_manager_get_logo(dev);//获取设备logo
        file_dev->root_path = dev_manager_get_root_path_by_logo((char *)file_dev->logo);//获取设备路径
    }
}



static struct ui_file_browser *platform_file_browser_open(struct rect *r,
        const char *path, const char *ftype, int show_mode)
{
    int err;
    struct file_browser *bro;
    struct __dev *dev = 0;
    bro = (struct file_browser *)br28_malloc(sizeof(*bro));
    if (!bro) {
        return NULL;
    }
    bro->bro.file_number = 0;
    bro->show_mode = show_mode;

    if (!path) {
        dev = dev_manager_find_active(0);///storage_dev_last();//获取最后一个设备的路径
        if (!dev) {
            br28_free(bro);
            return NULL;
        }
        path = dev_manager_get_root_path_by_logo(dev_manager_get_logo(dev));//dev->root_path;
    }

    if (!ftype) {
        ftype = MUSIC_SCAN_PARAM;
    }

    bro->fscan = fscan(path, ftype, 9);
    bro->bro.dev_num =  dev_manager_get_total(1);//获取在线设备总数

    if (bro->fscan) {
        bro->bro.file_number = bro->fscan->file_number;
        if (bro->bro.file_number == 0) {
            return &bro->bro;
        }
    }

    if (r) {
        memcpy(&bro->r, r, sizeof(struct rect));
    }

    return &bro->bro;

__err:
    fscan_release(bro->fscan);
    br28_free(bro);
    return NULL;
}



static void platform_file_browser_close(struct ui_file_browser *_bro)
{
    struct file_browser *bro = container_of(_bro, struct file_browser, bro);

    if (!bro) {
        return;
    }
    if (bro->fscan) {
        fscan_release(bro->fscan);
    }
    br28_free(bro);
}

static int platform_get_file_attrs(struct ui_file_browser *_bro,
                                   struct ui_file_attrs *attrs)
{
    int i, j;
    struct vfs_attr attr;
    struct file_browser *bro = container_of(_bro, struct file_browser, bro);

    if (!bro->fscan) {
        return -ENOENT;
    }

    FILE *file = fselect(bro->fscan, FSEL_BY_NUMBER, attrs->file_num + 1);
    if (!file) {
        return -ENOENT;
    }

    attrs->format = "ascii";

    fget_attrs(file, &attrs->attr);
    /* printf(" attr = %x, fsize =  %x,sclust = %x\n",attrs->attr.attr,attrs->attr.fsize,attrs->attr.sclust); */

    struct sys_time *time;
    time =  &(attrs->attr.crt_time);
    /* printf("y =%d  m =%d d = %d,h = %d ,m = %d ,s =%d\n",time->year,time->month,time->day,time->hour,time->min,time->sec);  */

    time =  &(attrs->attr.wrt_time);
    /* printf("y =%d  m =%d d = %d,h = %d ,m = %d ,s =%d\n",time->year,time->month,time->day,time->hour,time->min,time->sec);  */


    int len = fget_name(file, (u8 *)attrs->fname, 16);//长文件获取有问题
    if (len < 0) {
        fclose(file);
        return -EINVAL;
    }

    for (i = 0; i < len; i++) {
        if ((u8)attrs->fname[i] >= 0x80) {
            attrs->format = "uft16";
            goto _next;
        }
    }


    /* ASCII_ToUpper(attrs->fname, strlen(attrs->fname)); */

_next:

#if 0//文件系统接口不完善，临时解决
    for (i = 0; i < len; i++) {
        if (attrs->fname[i] == '.') {
            break;
        }
    }

    if (i == len) {
        attrs->ftype = UI_FTYPE_DIR;
    } else {
        char *ext = attrs->fname + i + 1;

        if (check_file_ext("JPG", ext)) {
            attrs->ftype = UI_FTYPE_IMAGE;
        } else if (check_file_ext("MOVAVI", ext)) {
            attrs->ftype = UI_FTYPE_VIDEO;
        } else if (check_file_ext("MP3WMAWAV", ext)) {
            attrs->ftype = UI_FTYPE_AUDIO;
        } else {
            attrs->ftype = UI_FTYPE_UNKNOW;
        }
    }
#else
    /* printf("name = %d %d \n",strlen(attrs->fname),len); */
    /* put_buf(attrs->fname,len); */

    if (attrs->attr.attr & F_ATTR_DIR) {
        attrs->ftype = UI_FTYPE_DIR;
    } else {
        char *ext = attrs->fname + strlen(attrs->fname) - 3;
        if (check_file_ext("JPG", ext)) {
            attrs->ftype = UI_FTYPE_IMAGE;
        } else if (check_file_ext("MOVAVI", ext)) {
            attrs->ftype = UI_FTYPE_VIDEO;
        } else if (check_file_ext("MP3WMAWAV", ext)) {
            attrs->ftype = UI_FTYPE_AUDIO;
        } else {
            attrs->ftype = UI_FTYPE_UNKNOW;
        }

    }

#endif

    fclose(file);

    return 0;
}

static int platform_set_file_attrs(struct ui_file_browser *_bro,
                                   struct ui_file_attrs *attrs)
{
    int attr = 0;
    struct file_browser *bro = container_of(_bro, struct file_browser, bro);

    if (!bro->fscan) {
        return -ENOENT;
    }

    FILE *file = fselect(bro->fscan, FSEL_BY_NUMBER, attrs->file_num + 1);
    if (!file) {
        return -EINVAL;
    }

    fget_attr(file, &attr);
    if (attrs->attr.attr & F_ATTR_RO) {
        attr |= F_ATTR_RO;
    } else {
        attr &= ~F_ATTR_RO;
    }
    fset_attr(file, attr);

    fclose(file);

    return 0;
}

static void *platform_open_file(struct ui_file_browser *_bro,
                                struct ui_file_attrs *attrs)
{
    struct file_browser *bro = container_of(_bro, struct file_browser, bro);

    if (!bro->fscan) {
        return NULL;
    }

    return fselect(bro->fscan, FSEL_BY_NUMBER, attrs->file_num + 1);
}

static int platform_delete_file(struct ui_file_browser *_bro,
                                struct ui_file_attrs *attrs)
{
    struct file_browser *bro = container_of(_bro, struct file_browser, bro);

    if (!bro->fscan) {
        return -EINVAL;
    }

    FILE *file = fselect(bro->fscan, FSEL_BY_NUMBER, attrs->file_num + 1);
    if (!file) {
        return -EFAULT;
    }
    fdelete(file);

    return 0;
}

void test_browser()
{
    struct ui_file_browser *browser = NULL;
    static struct ui_file_attrs attrs = {0};
    if (!browser) {
        browser = platform_file_browser_open(NULL, NULL, MUSIC_SCAN_PARAM, 0);
    }

    if (browser) {
        printf("file num = %d \n", browser->file_number);
        platform_get_file_attrs(browser, &attrs);
        printf("format =%s name =%s type = %x \n", attrs.format, attrs.fname, attrs.ftype);
        platform_delete_file(browser, &attrs);
        attrs.file_num ++;
        if (attrs.file_num >= browser->file_number) {
            attrs.file_num = 0;
        }
    }
}

static void *br28_set_timer(void *priv, void (*callback)(void *), u32 msec)
{
    return (void *)sys_timer_add(priv, callback, msec);
}

static int br28_del_timer(void *fd)
{
    if (fd) {
        sys_timer_del((int)fd);
    }

    return 0;
}

u32 __attribute__((weak)) set_retry_cnt()
{
    return 10;
}

const char *WATCH_VERSION_LIST = {
    "W002"

};

/* static const char *WATCH_STY_CHECK_LIST[] = { */
/* RES_PATH"JL/JL.sty", */
/* RES_PATH"watch/watch.sty", */
/* RES_PATH"watch1/watch1.sty", */
/* RES_PATH"watch2/watch2.sty", */
/* RES_PATH"watch3/watch3.sty", */
/* RES_PATH"watch4/watch4.sty", */
/* RES_PATH"watch5/watch5.sty", */
/* }; */

#define WATCH_ITEMS_LIMIT       40
#define BGP_ITEMS_LIMIT         (WATCH_ITEMS_LIMIT + 10)//(确保 >= WATCH_ITEMS_LIMIT就可以)

struct __WATCH_STYLE {
    u8 watch_style;
    s8 standby_watch_style;
};

static struct __WATCH_STYLE dial_sty;
static u8 watch_items;
static volatile u8 watch_update_over = 0;

static char *watch_res[WATCH_ITEMS_LIMIT] = {0};
static char *watch_bgp_related[WATCH_ITEMS_LIMIT] = {0};
static char *watch_bgp[BGP_ITEMS_LIMIT] = {0};
static u8 watch_bgp_items;

static char *watch_bgpic_path = NULL;

extern u8 smartbox_eflash_flag_get(void);
extern u8 smartbox_eflash_update_flag_get(void);
extern void smartbox_eflash_flag_set(u8 eflash_state_type);
extern int smartbox_eflash_resume_opt(void);
static int watch_mem_bgp_related();
extern void smartbox_update_flag_clear(void);
extern void smartbox_ex_flash_file_flag(u8 flag);

u32 watch_bgp_get_nums()
{
    return watch_bgp_items;
}

char *watch_bgp_get_item(u8 sel_item)
{
    if (sel_item >= watch_bgp_items) {
        printf("\n\n\nwatch_bgp items overflow %d\n\n\n\n", sel_item);
        return NULL;
    }

    return watch_bgp[sel_item];
}

char *watch_bgp_add(char *bgp)
{
    char *root_path = RES_PATH;
    char *new_bgp_item = NULL;
    u32 len;
    u32 i;

    if ((watch_bgp_items + 1) >= BGP_ITEMS_LIMIT) {
        printf("\n\n\nwatch_bgp items overflow %d\n\n\n\n", watch_bgp_items);
        return NULL;
    }

    len = strlen(bgp) + strlen(root_path) + 1;
    new_bgp_item = watch_malloc(len);
    if (new_bgp_item == NULL) {
        printf("\n\n\nwatch_bgp items malloc err %d\n\n\n\n", len);
        return NULL;
    }

    ASCII_ToLower(bgp, strlen(bgp));
    strcpy(new_bgp_item, root_path);
    /* strcpy(&new_bgp_item[strlen(root_path)], &bgp[1]); */
    strcpy(&new_bgp_item[strlen(root_path)], bgp);
    new_bgp_item[len - 1] = '\0';

    //如果已经存在这个背景图片路径，则直接返回对应的地址
    for (i = 0; i < watch_bgp_items; i++) {
        /* printf("num %s\n", watch_bgp[i]); */
        if (strncmp(new_bgp_item, watch_bgp[i], strlen(new_bgp_item)) == 0) {
            watch_free(new_bgp_item);
            new_bgp_item = watch_bgp[i];
            printf("already has %s\n", new_bgp_item);
            return new_bgp_item;
        }
    }

    watch_bgp[watch_bgp_items] = new_bgp_item;
    watch_bgp_items++;

    printf("add new_bgp_item succ %d, %s\n", watch_bgp_items, new_bgp_item);

    return new_bgp_item;
}

int watch_bgp_del(char *bgp)
{
    u32 i;
    char watch_bgp_item[64];
    u32 cur_items = watch_bgp_items;
    char *root_path = RES_PATH;
    char *bgp_item = NULL;

    ASSERT(((strlen(bgp) + strlen(root_path) + 1) < sizeof(watch_bgp_item)), "bgp err name0 %s\n", bgp);

    ASCII_ToLower(bgp, strlen(bgp));
    strcpy(watch_bgp_item, root_path);
    /* strcpy(&watch_bgp_item[strlen(root_path)], &bgp[1]); */
    strcpy(&watch_bgp_item[strlen(root_path)], bgp);
    watch_bgp_item[strlen(bgp) + strlen(root_path)] = '\0';
    printf("watch_bgp_item %s\n", watch_bgp_item);

    for (i = 0; i < cur_items; i++) {
        if (strncmp(watch_bgp_item, watch_bgp[i], strlen(watch_bgp_item)) == 0) {
            bgp_item = watch_bgp[i];
            watch_bgp[i] = NULL;
            watch_free(bgp_item);
            watch_bgp_items--;
            break;
        }
    }

    if (bgp_item == NULL) {
        printf("can not find bgp_item %s\n", watch_bgp_item);
        return -1;
    }

    for (; i < cur_items; i++) {
        if (watch_bgp[i + 1] != NULL) {
            watch_bgp[i] = watch_bgp[i + 1];
        } else {
            watch_bgp[i] = NULL;
            break;
        }
    }

    //del related item
    cur_items = sizeof(watch_bgp_related) / sizeof(watch_bgp_related[0]);
    for (i = 0; i < cur_items; i++) {
        if (bgp_item == watch_bgp_related[i]) {
            watch_bgp_related[i] = NULL;
        }
    }

    for (i = 0; i < cur_items; i++) {
        printf("cur related items %d, %s\n", i, watch_bgp_related[i]);
    }
    for (i = 0; i < watch_bgp_items; i++) {
        printf("cur bgp items %d, %s\n", i, watch_bgp[i]);
    }

    return 0;
}


//替换背景图片,(1)如果某个表盘没有背景图片，则增加，(2)如果有就替换
//如果被替换的这个背景图片已经不关联任何表盘，则将它从表watch_bgp删除
int watch_bgp_set_related(char *bgp, u8 cur_watch, u8 del)
{
    u32 i;
    u32 cur_items;
    char *root_path = RES_PATH;
    char *bgp_item = NULL;
    char *new_bgp_item = NULL;
    u32 total_relate_items;
    char old_bgp[16];
    u32 len;

    total_relate_items = sizeof(watch_bgp_related) / sizeof(watch_bgp_related[0]);
    if (cur_watch >= total_relate_items) {
        printf("\n\n\nwatch_bgp_related items overflow %d\n\n\n\n", cur_watch);
        return -1;
    }

    //提取旧的背景图片名字
    if (watch_bgp_related[cur_watch]) {
        bgp_item = watch_bgp_related[cur_watch];
        len = strlen(bgp_item) - strlen(root_path) + 1;
        if (len >= sizeof(old_bgp)) {
            return -1;
        }
        memcpy(old_bgp, &bgp_item[strlen(root_path)], len);
        printf("old bpg %s\n", old_bgp);
    }

    if (bgp != NULL) {
        new_bgp_item = watch_bgp_add(bgp);
        if (new_bgp_item == NULL) {
            printf("add bgp item err %s\n", bgp);
            return -1;
        }
    }

    //如果没有背景图片
    if (watch_bgp_related[cur_watch] == NULL) {
        watch_bgp_related[cur_watch] = new_bgp_item;
        /* printf("a1\n"); */
    } else {
        bgp_item = watch_bgp_related[cur_watch];
        watch_bgp_related[cur_watch] = new_bgp_item;
        cur_items = sizeof(watch_bgp_related) / sizeof(watch_bgp_related[0]);
        /* printf("a2 %s\n", bgp_item); */
        for (i = 0; i < cur_items; i++) {
            if (bgp_item == watch_bgp_related[i]) {
                /* printf("a3 %d\n", i); */
                break;
            }
        }
        if ((i == cur_items) && del) { //被替换的这个背景图片已经不关联任何表盘
            /* printf("a4\n"); */
            watch_bgp_del(old_bgp);
        }
    }

    watch_mem_bgp_related();

    return 0;
}

//根据某个表盘获取对应的背景图片
char *watch_bgp_get_related(u8 cur_watch)
{
    u32 total_relate_items;

    total_relate_items = sizeof(watch_bgp_related) / sizeof(watch_bgp_related[0]);
    if (cur_watch >= total_relate_items) {
        printf("\n\n\nwatch_bgp_related items overflow %d\n\n\n\n", cur_watch);
        return NULL;
    }

    return watch_bgp_related[cur_watch];
}

int watch_bgp_related_del_all(char *bgp)
{
    watch_bgp_del(bgp);
    watch_mem_bgp_related();
    return 0;
}

int watch_get_style_by_name(char *name)
{
    u32 i;
    u32 ret;
    printf("watch find %s\n", name);

    for (i = 0; i < watch_items; i++) {
        /* printf("watch finding %s\n", watch_res[i]); */
        if (strncmp(name, watch_res[i], strlen(name)) == 0) {
            printf("find watch style %d, %s\n\n\n", i, watch_res[i]);
            return i;
        }
    }

    printf("watch find faile\n");
    return -1;
}

#define WATCH_MEM_BGP       0x55aa66bb
static u32 wmem_last = 0;
static u32 wmem_area_num = 0;
static void *wmem_file = NULL;

int watch_mem_new(u32 area)
{
    wmem_last = area;
    wmem_area_num++;
    return 0;
}

void *watch_mem_open()
{
    if (wmem_file == NULL) {
        /* wmem_file = fopen(RES_PATH"wmem.bin", "w+"); */
        wmem_file = fopen("storage/sd1/C/download/wmem.bin", "w+");
    }
    return wmem_file;
}

void watch_mem_close()
{
    if (wmem_file) {
        fclose(wmem_file);
        wmem_file = NULL;
    }
}

int watch_mem_write(u32 offset, u32 len, u8 *buf, u32 area)
{
    u32 ret;
    u8 tmp_buf[8];
    u32 area_offset = 0;
    u32 area_len = 0;
    u32 find_tag = 0;

    if (wmem_file == NULL) {
        return -1;
    }

    if ((flen(wmem_file) == 0) || (wmem_area_num <= 1)) {
        area_offset = 0;
    } else {
        do {
            fseek_fast(wmem_file, area_offset, SEEK_SET);
            ret = fread_fast(wmem_file, tmp_buf, 8);
            if (ret != 8) {
                printf("wmem find tag err end\n");
                return -1;
            }
            memcpy(&ret, tmp_buf + 4, 4); //flag
            if (ret == area) {
                memcpy(&area_len, tmp_buf, 4);//len
                find_tag = 1;
                break;
            }
            memcpy(&ret, tmp_buf, 4);//len
            area_offset += ret;
        } while (find_tag == 0);
    }

    fseek_fast(wmem_file, area_offset + offset, SEEK_SET);
    if (area == wmem_last) {
        ret = fwrite(wmem_file, buf, len);
        if (ret != len) {
            return -1;
        }
    } else {
        if ((offset + len) <= area_len) {
            ret = fwrite(wmem_file, buf, len);
            if (ret != len) {
                return -1;
            }
        } else {
            //要将这个区域的内容先搬迁到文件最后，使这个区域成为最后的区域
            //再写数据

        }
    }

    return 0;
}

/* static u32 wmem_test_flag = 0; */
int watch_mem_read(u32 offset, u32 len, u8 *buf, u32 area)
{
    u32 area_offset = 0;
    u32 area_len = 0;
    u32 find_tag = 0;
    u32 ret;
    u8 tmp_buf[8];

    if (wmem_file == NULL) {
        return -1;
    }

    if (flen(wmem_file) == 0) {
        /* wmem_test_flag = 1; */
        return 0;
    }


    if (wmem_area_num <= 1) {
        area_offset = 0;
    } else {
        do {
            fseek_fast(wmem_file, area_offset, SEEK_SET);
            ret = fread_fast(wmem_file, tmp_buf, 8);
            if (ret != 8) {
                printf("wmem find tag err end\n");
                return -1;
            }
            memcpy(&ret, tmp_buf + 4, 4); //flag
            if (ret == area) {
                memcpy(&area_len, tmp_buf, 4);//len
                find_tag = 1;
                break;
            }
            memcpy(&ret, tmp_buf, 4);//len
            area_offset += ret;
        } while (find_tag == 0);

        if ((offset + len) > area_len) {
            return -1;
        }
    }

    fseek_fast(wmem_file, area_offset + offset, SEEK_SET);
    ret = fread_fast(wmem_file, buf, len);
    if (ret != len) {
        printf("wmem read err %d\n", ret);
        return -1;
    }

    return 0;
}


static int watch_mem_bgp_related()
{
    int ret = 0;
    u8 *related_buf;
    u32 total_relate_items = sizeof(watch_bgp_related) / sizeof(watch_bgp_related[0]);
    u32 len;
    char *related_item;
    u32 area = WATCH_MEM_BGP;
    u32 i;

    if (watch_bgp_items == 0) {
        return -1;
    }

    len = 64 * (total_relate_items + 1);
    /* related_buf = br28_malloc(len); */
    related_buf = br28_zalloc(len);
    if (related_buf == NULL) {
        return -1;
    }

    if (watch_mem_open() == NULL) {
        br28_free(related_buf);
        return -1;
    }

    memcpy(related_buf, &len, 4);
    memcpy(related_buf + 4, &area, 4);
    for (i = 0; i < total_relate_items; i++) {
        related_item = watch_bgp_related[i];
        if (related_item) {
            printf("related item : %s, %d, %d\n", related_item, strlen(related_item) + 1, i);
            memcpy(&related_buf[64 + 64 * i], related_item, strlen(related_item) + 1);
        }
    }
    ret = watch_mem_write(0, len, related_buf, area);
    if (ret != 0) {
        printf("watch mem werr %x\n", ret);
    } else {
        printf("watch mem succ\n");
    }

    watch_mem_close();

    br28_free(related_buf);

    return ret;
}

static int watch_bgp_related_init()
{
    static u8 flag = 0;
    int ret = 0;
    u8 *related_buf;
    u32 total_relate_items = sizeof(watch_bgp_related) / sizeof(watch_bgp_related[0]);
    u32 len;
    char *related_item;
    u32 area = WATCH_MEM_BGP;
    u32 i, j;

    if (flag == 0) {
        watch_mem_new(area);
        flag = 1;
    }

    if (watch_bgp_items == 0) {
        return -1;
    }

    len = 64 * (total_relate_items + 1);
    related_buf = br28_malloc(len);
    if (related_buf == NULL) {
        return -1;
    }

    if (watch_mem_open() == NULL) {
        br28_free(related_buf);
        return -1;
    }

    ret = watch_mem_read(0, len, related_buf, area);
    if (ret != 0) {
        br28_free(related_buf);
        return -1;
    }

    for (i = 0; i < total_relate_items; i++) {
        watch_bgp_related[i] = NULL;
        related_item = &related_buf[64 + 64 * i];
        if (related_item[0]) {
            printf("related item : %s, %d, %d\n", related_item, strlen(related_item), i);
            for (j = 0; j < watch_bgp_items; j++) {
                if (strncmp(related_item, watch_bgp[j], strlen(watch_bgp[j])) == 0) {
                    watch_bgp_related[i] =  watch_bgp[j];
                    printf("find bgp related %d\n", j);
                    break;
                }
            }
        }
    }

    watch_mem_close();

    br28_free(related_buf);

    return 0;
}



char *watch_get_background()
{
    if (watch_bgpic_path == NULL) {
        return NULL;
    }
    return watch_bgpic_path;
}

int watch_set_background(char *bg_pic)
{
    u32 bg_strlen = strlen(bg_pic);
    u32 root_path_strlen = strlen(RES_PATH);

    /* if ((bg_strlen + root_path_strlen) >= sizeof(watch_bgpic_path)) { */
    /* printf("set background err %d, %d\n", bg_strlen, root_path_strlen); */
    /* return -1; */
    /* } */

    if (bg_pic == NULL) {
        /* memset(watch_bgpic_path, 0, sizeof(watch_bgpic_path)); */
        if (watch_bgpic_path != NULL) {
            br28_free(watch_bgpic_path);
        }
        watch_bgpic_path = NULL;
    } else {

        if (watch_bgpic_path != NULL) {
            br28_free(watch_bgpic_path);
            watch_bgpic_path = NULL;
        }

        watch_bgpic_path = br28_malloc(bg_strlen + root_path_strlen + 1);
        if (watch_bgpic_path == NULL) {
            printf("set background err %d, %d\n", bg_strlen, root_path_strlen);
            return -1;
        }

        strcpy(watch_bgpic_path, RES_PATH);
        strcpy(watch_bgpic_path + root_path_strlen, bg_pic);
        watch_bgpic_path[root_path_strlen + bg_strlen] = '\0';
    }

    return 0;
}

void watch_update_finish()
{
    printf("\n\nwatch update finish\n\n");
    watch_update_over = 1;
}

int watch_get_update_status()
{
    return watch_update_over;
}

char *watch_get_item(int style)
{
    if (style >= watch_items) {
        return NULL;
    }

    return watch_res[style];
}
char *watch_get_root_path()
{
    return RES_PATH;
}

int watch_set_style(int style)
{
#if UI_WATCH_RES_ENABLE
    /* if (style == 0) { */
    /* style = 1; */
    /* } */
    /* if (style > sizeof(watch_res) / sizeof(watch_res[0])) { */
    if (style >= watch_items) {
        return false;
    }
    dial_sty.watch_style = style;
#endif
    return true;
}

static int watch_get_msg_by_path(char *key, char *data, u32 data_len, u32 offset)
{
    int ret = 0;
    u32 i = 0;
    u32 tmp_strlen = 0;
    u8 *tmp_data = NULL;
    RESFILE *file = res_fopen(data, "r");
    if (NULL == file) {
        ret = -1;
        goto _end;
    }
    // 获取长度
    tmp_strlen = res_flen(file);
    // 用来装路径名字
    tmp_data = br28_malloc(strlen(data) + 1);
    if (NULL == tmp_data) {
        ret = -1;
        goto _end;
    }
    memcpy(tmp_data, data, strlen(data) + 1);
    memset(data, 0, data_len);
    // 读出数据
    res_fseek(file, offset, SEEK_SET);
    res_fread(file, data, data_len);

    // 关闭数据
    if (file) {
        res_fclose(file);
        file = NULL;
    }

    // 判断表盘的json文件一开始是否符合规范，不符合直接返回(该判断针对读出来是全f的数据)
    if ((0 == offset) && (0x7B != data[0] || 0x0D != data[1] || 0x0A != data[2])) {
        ret = -1;
        goto _end;
    }

    char *tver = NULL;
    for (i = 0; i < data_len; i++) {
        // 换行
        if (0xd == data[i] && 0x0a == data[i + 1]) {
            offset += i + 1 + 1;
            break;
        }
        // 文件结束
        if (0 == data[i] || (offset + i >= tmp_strlen)) {
            offset += i;
            tmp_strlen = 0;
            break;
        }
        if ('"' == data[i]) {
            if (tver) {
                tver = data + i + 1;
                break;
            }
            if (0 == strncmp(key, data + i + 1, strlen(key))) {
                i += strlen(key) + 1 + 1;
                tver = data + i - 1;
                if ('"' != tver[0]) {
                    tver = NULL;
                }
            }
        }
    }

    if (tver) {
        tmp_strlen = index(tver, '"') - tver;
        memcpy(data, tver, tmp_strlen);
        data[tmp_strlen] = '\0';
    } else {
        // 还原路径，然后把路径作为参数再次递归传入
        memcpy(data, tmp_data, strlen(tmp_data) + 1);
        if (i == data_len) {
            // 没有匹配到数据或没有获取到换行标记也要更新offset值
            offset += data_len;
        }
        ret = -1;
    }

    if (tmp_data) {
        br28_free(tmp_data);
        tmp_data = NULL;
    }

    if (offset < tmp_strlen && NULL == tver) {
        ret = watch_get_msg_by_path(key, data, data_len, offset);
    }

_end:
    if (tmp_data) {
        br28_free(tmp_data);
    }

    if (file) {
        res_fclose(file);
    }

    return ret;
}

static int watch_get_msg_from_json(char *watch_item, char *value, char *key)
{
    int ret = 0;
    char *sty_suffix = ".sty";
    char *json_suffix = ".json";
    u32 tmp_strlen;
    char tmp_name[64];
    u32 sty_strlen;

    if (watch_item == NULL) {
        return -1;
    }

    sty_strlen = strlen(sty_suffix);
    tmp_strlen = strlen(watch_item);
    strcpy(tmp_name, watch_item);
    strcpy(&tmp_name[tmp_strlen - sty_strlen], json_suffix);
    tmp_name[tmp_strlen - sty_strlen + strlen(json_suffix)] = '\0';
    printf("key %s, path %s\n", key, tmp_name);
    ret = watch_get_msg_by_path(key, tmp_name, sizeof(tmp_name), 0);
    if (0 == ret) {
        memcpy(value, tmp_name, strlen(tmp_name) + 1);
    }
    return ret;
}

int watch_get_uuid(char *watch_item, char *uuid)
{
    return watch_get_msg_from_json(watch_item, uuid, "prj_uuid");
}

int watch_get_version(char *watch_item, char *version)
{
#if 1
    return watch_get_msg_from_json(watch_item, version, "version_id");
#else
    char *sty_suffix = ".sty";
    char *json_suffix = ".json";
    u32 tmp_strlen;
    char tmp_name[64];
    u32 sty_strlen;
    RESFILE *file;
    /* char *version; */
    char *tver;
    u32 i;

    if (watch_item == NULL) {
        return -1;
    }

    sty_strlen = strlen(sty_suffix);
    tmp_strlen = strlen(watch_item);
    strcpy(tmp_name, watch_item);
    strcpy(&tmp_name[tmp_strlen - sty_strlen], json_suffix);
    tmp_name[tmp_strlen - sty_strlen + strlen(json_suffix)] = '\0';
    printf("version name %s\n", tmp_name);
    file = res_fopen(tmp_name, "r");
    if (!file) {
        printf("open_jsonfile fail %s\n", tmp_name);
        return -1;
    }
    memset(tmp_name, 0, sizeof(tmp_name));
    res_fread(file, tmp_name, sizeof(tmp_name));
    res_fclose(file);
    /* printf("json buf : %s\n", tmp_name); */
    tmp_name[26] = '\0';
    /* version = &tmp_name[22]; */
    strcpy(version, &tmp_name[22]);

    return 0;
#endif
}


int watch_version_juge(char *watch_item)
{
#if 1
    char *tver;
    char version[64] = {0};
    if (0 == watch_get_version(watch_item, version)) {
        for (u8 i = 0; i < strlen(WATCH_VERSION_LIST); i += 5) {
            tver = (char *)&WATCH_VERSION_LIST[i];
            if (0 == strncmp(version, tver, strlen(version))) {
                return 0;
            }
        }
    }
    printf("juge watch version err\n");

    return -1;
#else
    char *sty_suffix = ".sty";
    char *json_suffix = ".json";
    u32 tmp_strlen;
    char tmp_name[64];
    u32 sty_strlen;
    RESFILE *file;
    char *version;
    char *tver;
    u32 i;

    if (watch_item == NULL) {
        return -1;
    }

    sty_strlen = strlen(sty_suffix);
    tmp_strlen = strlen(watch_item);
    strcpy(tmp_name, watch_item);
    strcpy(&tmp_name[tmp_strlen - sty_strlen], json_suffix);
    tmp_name[tmp_strlen - sty_strlen + strlen(json_suffix)] = '\0';
    printf("version name %s\n", tmp_name);
    file = res_fopen(tmp_name, "r");
    if (!file) {
        printf("open_jsonfile fail %s\n", tmp_name);
        return -1;
    }
    memset(tmp_name, 0, sizeof(tmp_name));
    res_fread(file, tmp_name, sizeof(tmp_name));
    res_fclose(file);
    printf("json buf : %s\n", tmp_name);
    tmp_name[0x2f] = '\0';
    version = &tmp_name[0x2b];
    printf("ve %s\n", version);

    for (i = 0; i < strlen(WATCH_VERSION_LIST); i += 5) {
        tver = &WATCH_VERSION_LIST[i];
        /* printf("ver %s, %d\n", tver, strlen(version)); */
        printf("tver %s, %s\n", tver, version);
        if (strncmp(version, tver, strlen(version)) == 0) {
            return 0;
        }
    }

    printf("juge watch version err\n");

    return -1;
#endif
}


int watch_set_style_by_name(char *name)
{
    u32 i;
    u32 ret;
    printf("watch find %s\n", name);

    for (i = 0; i < watch_items; i++) {
        /* printf("watch finding %s\n", watch_res[i]); */
        if (strncmp(name, watch_res[i], strlen(name)) == 0) {
            printf("find watch style %d, %s\n\n\n", i, watch_res[i]);

            ret = watch_version_juge(watch_res[i]);
            if (ret != 0) {
                break;
            }
            watch_set_style(i);
            return 0;
        }
    }

    printf("watch find faile\n");
    return -1;
}

int standby_watch_set_style(int style)
{
    dial_sty.standby_watch_style = style;
    /* printf("standby_watch_style:%d",dial_sty.standby_watch_style); */
    return style;
}

int standby_watch_get_style()
{
    return dial_sty.standby_watch_style;
}

int watch_get_style()
{
    return dial_sty.watch_style;
}

int watch_get_items_num()
{
    return watch_items;
}

char *watch_get_cur_path()
{
    static char path[16];
    u8 i;
    char *tmp = &watch_res[dial_sty.watch_style][strlen(RES_PATH)];

    path[0] = '/';
    for (i = 0; i < 15; i++) {
        if (tmp[i] == '/') {
            break;
        }
        path[i + 1] = tmp[i];
        putchar(tmp[i]);
    }
    path[i + 1] = '\0';

    ASCII_ToUpper(path, strlen(path));
    /* printf("cur : %s, %s, %d\n", tmp, path, i); */

    /* return watch_res[watch_style]; */
    return path;
}

int watch_get_cur_path_len()
{
    return strlen(watch_get_cur_path()) + 1;

    /* char *path = watch_res[watch_style]; */
    /* u32 i; */

    /* for (i = strlen(path) - 1; i > 0; i--) { */
    /* if (path[i] == '/') { */
    /* break; */
    /* } */
    /* } */

    /* return i + 1; */
}

extern void virfat_flash_get_dirinfo(void *file_buf, u32 *file_num);
void watch_add_item_deal(void)
{
    u32 i, j, k;
    u32 index = -1;
    u32 file_num;
    u8 *fname_buf = NULL;
    u8 *fname = NULL;
    u8 *bgp_fname = NULL;
    u8 fname_len;

    virfat_flash_get_dirinfo(NULL, &file_num);

    fname_buf = watch_malloc(file_num * 12);
    if (NULL == fname_buf) {
        goto __watch_add_item_deal_end;
    }

    virfat_flash_get_dirinfo(fname_buf, &file_num);

    for (i = 0, k = 0; i < file_num; i++) {
        fname = fname_buf + i * 12;
        for (j = 0; j < 12; j++) {
            if (' ' == fname[j]) {
                fname[j] = '\0';
                break;
            }
        }
        fname_len = strlen(fname);
        ASCII_ToLower(fname, fname_len);
        if (0 == strncmp(fname, "watch", strlen("watch"))) {
            fname_len = rindex(watch_res[k], '.') - rindex(watch_res[k], '/') - 1;
            if (0 != strncmp(fname, rindex(watch_res[k], '/') + 1, fname_len)) {
                // 假如名字不一样，则往下找一样的名字，并进行交换
                for (index = k + 1; index < sizeof(watch_res) / sizeof(watch_res[0]); index++) {
                    fname_len = rindex(watch_res[index], '.') - rindex(watch_res[index], '/') - 1;
                    if (0 == strncmp(fname, rindex(watch_res[index], '/') + 1, fname_len)) {
                        fname = watch_res[index];
                        bgp_fname = watch_bgp_related[index];
                        for (; index > k; index --) {
                            watch_res[index] = watch_res[index - 1];
                            watch_bgp_related[index] = watch_bgp_related[index - 1];
                            if (dial_sty.watch_style == (index - 1)) {
                                dial_sty.watch_style = index;
                            }
                        }
                        watch_res[k] = fname;
                        watch_bgp_related[k] = bgp_fname;
                        break;
                    }
                }
            }
            k++;
        }
    }

__watch_add_item_deal_end:
    if (fname_buf) {
        watch_free(fname_buf);
    }
    watch_mem_bgp_related();
}

int watch_add_item(char *s)
{
    char *new_item = NULL;
    /* char watch_name[16]; */
    u32 len;
    char *suffix = ".sty";


    char *root_path = RES_PATH;
    char watch[64];

    ASSERT(((strlen(s) + strlen(root_path) + 1) < sizeof(watch)), "err name3 %s\n", s);
    /* if ((strlen(s) + strlen(root_path) + 1) >= sizeof(watch)) { */
    /* printf("err name3 %s\n", s); */
    /* while (1); */
    /* } */

    ASCII_ToLower(s, strlen(s));
    strcpy(watch, root_path);
    strcpy(&watch[strlen(root_path)], &s[1]);
    s = watch;

    printf("watch_path %s\n", s);

    ASSERT(((watch_items + 1) < WATCH_ITEMS_LIMIT), "watch items overflow %d\n\n\n\n", watch_items);
    /* if ((watch_items + 1) >= WATCH_ITEMS_LIMIT) { */
    /* printf("watch items overflow %d\n\n\n\n", watch_items); */
    /* while (1); */
    /* } */
    len = strlen(s) - strlen(RES_PATH);
    /* printf("test len %d, %d, %d, %s, %s\n", strlen(s), strlen(RES_PATH), len, RES_PATH, s, s[len]); */

    /* strcpy(watch_name, &s[strlen(s) - len]); */
    /* printf("end str %s\n", watch_name); */

    new_item = watch_malloc(strlen(s) + 1 + len + strlen(suffix) + 1);
    if (new_item == NULL) {
        printf("watch add item fail\n");
        return -1;
    }
    strcpy(new_item, s);
    new_item[strlen(s)] = '/';
    strcpy(&new_item[strlen(s) + 1], &s[strlen(s) - len]);
    strcpy(&new_item[strlen(s) + 1 + len], suffix);
    new_item[strlen(s) + 1 + len + strlen(suffix)] = '\0';

    for (u8 i = 0; i < watch_items; i++) {
        if (0 == strcmp(new_item, watch_res[i])) {
            printf("repeat : %s, %s\n", new_item, watch_res[i]);
            watch_free(new_item);
            return 0;
        }
    }

    watch_res[watch_items] = new_item;
    watch_items++;

    watch_add_item_deal();

    printf("watch add item succ %d, %s\n", watch_items, new_item);

    return 0;
}

int watch_del_item(char *s)
{
    u32 i;
    char *item;
    u32 cur_items = watch_items;

    char *root_path = RES_PATH;
    char watch[64];
    char *cur_watch = watch_res[dial_sty.watch_style];

    ASSERT(((strlen(s) + strlen(root_path) + 1 + 1) < sizeof(watch)), "err name5 %s\n", s);
    /* if ((strlen(s) + strlen(root_path) + 1) >= sizeof(watch)) { */
    /* printf("err name5 %s\n", s); */
    /* while (1); */
    /* } */
    ASCII_ToLower(s, strlen(s));
    strcpy(watch, root_path);
    strcpy(&watch[strlen(root_path)], &s[1]);

    if (watch[strlen(watch) - 1] != '/') {
        strcat(&watch[strlen(root_path)], "/");//解决下面表盘匹配，watch 和 watch1半匹配撞车
    }
    s = watch;


    printf("watch_path %s\n", s);

    if (watch_items <= 2) {
        return -1;
    }

    for (i = 0; i < cur_items; i++) {
        if (strncmp(s, watch_res[i], strlen(s)) == 0) {
            item = watch_res[i];
            watch_res[i] = NULL;
            watch_free(item);
            watch_items--;

            watch_bgp_related[i] = NULL;
            break;
        }
    }

    for (; i < cur_items; i++) {
        if (watch_res[i + 1] != NULL) {
            watch_res[i] = watch_res[i + 1];

            watch_bgp_related[i] = watch_bgp_related[i + 1];
        } else {
            watch_res[i] = NULL;

            watch_bgp_related[i] = NULL;
            break;
        }
    }

    for (i = 0; i < watch_items; i++) {
        if (cur_watch == watch_res[i]) {
            printf("del set style %d, %s\n", i, watch_res[i]);
            watch_set_style(i);
            break;
        }
    }
    if (i == watch_items) {
        printf("end style\n");
        watch_set_style(0);
    }

    watch_mem_bgp_related();

    for (i = 0; i < watch_items; i++) {
        printf("cur watch item %d, %s\n", watch_items, watch_res[i]);
    }

    return 0;
}

int watch_select_wtrite_vm(void *priv)
{
#if UI_WATCH_RES_ENABLE
    int ret = 0;
    if ((int)priv == (int)SYSCFG_WRITE_ERASE_STATUS) {
        dial_sty.watch_style = 0;
        dial_sty.standby_watch_style = -1;
    }
    ret = syscfg_write(VM_WATCH_SELECT, &dial_sty, sizeof(struct __WATCH_STYLE));
    if (ret != sizeof(struct __WATCH_STYLE)) {
        printf("watch_select_wtrite_vm err\n");
        return -1;
    }
    return ret;
#endif
}

int watch_select_read_vm()
{
#if UI_WATCH_RES_ENABLE
    int ret = 0;
    ret = syscfg_read(VM_WATCH_SELECT, &dial_sty, sizeof(struct __WATCH_STYLE));
    if ((sizeof(struct __WATCH_STYLE) != ret) ||
        (dial_sty.watch_style >= watch_items) ||
        (dial_sty.standby_watch_style >= watch_items)) {
        printf("write watch_select_read_vm err\n");
        ret = -1;
    }
    return ret;
#endif
}

/* 说明 ：申请表盘以及表盘背景管理所需的内存, 内存不释放，内存的生命周期为整个ui的生命周期 */
int watch_set_init()
{
#if UI_WATCH_RES_ENABLE
    u32 i, j;
    u32 len;
    u32 file_num;
    char *fname_buf;
    char *suffix = ".sty";
    u8 root_path_len = strlen(RES_PATH);
    u8 suffix_len = strlen(suffix);
    u8 fname_len;
    char *fname;


    for (i = 0; i < WATCH_ITEMS_LIMIT; i++) {
        if (watch_res[i] != NULL) {
            watch_free(watch_res[i]);
            watch_res[i] = NULL;
        }
    }
    dial_sty.watch_style = 0;
    dial_sty.standby_watch_style = -1;
    watch_items = 0;


    for (i = 0; i < BGP_ITEMS_LIMIT; i++) {
        if (watch_bgp[i]) {
            watch_free(watch_bgp[i]);
            watch_bgp[i] = NULL;
        }
    }
    watch_bgp_items = 0;


    virfat_flash_get_dirinfo(NULL, &file_num);

    fname_buf = watch_malloc(file_num * 12);
    if (fname_buf == NULL) {
        printf("fname_buf br28_zalloc faile %d\n", file_num);
        return -1;
    }

    virfat_flash_get_dirinfo(fname_buf, &file_num);
    printf("file_num : %d\n", file_num);

    if (file_num >= BGP_ITEMS_LIMIT) {
        printf("file num overflow %d\n", file_num);
        watch_free(fname_buf);
        return -1;
    }

    for (i = 0; i < file_num; i++) {
        fname = &fname_buf[i * 12];
        for (j = 0; j < 12; j++) {
            if (fname[j] == ' ') {
                fname[j] = '\0';
                break;
            }
        }
        if (j == 12) {
            printf("\n\n\n\nfname overflow\n\n\n\n\n");
            watch_free(fname_buf);
            return -1;
        }
        fname_len = strlen(fname);
        ASCII_ToLower(fname, fname_len);

        if (strncmp(fname, "watch", strlen("watch")) == 0) {
            len = root_path_len + fname_len + 1 + fname_len + suffix_len;
            watch_res[watch_items] = watch_malloc(len + 1);
            if (watch_res[watch_items] == NULL) {
                printf("\n\nmalloc watch list err\n\n");
                watch_free(fname_buf);
                return -1;
            }
            strcpy(watch_res[watch_items], RES_PATH);
            strcpy(&watch_res[watch_items][root_path_len], fname);
            watch_res[watch_items][root_path_len + fname_len] = '/';
            strcpy(&watch_res[watch_items][root_path_len + fname_len + 1], fname);
            strcpy(&watch_res[watch_items][root_path_len + fname_len + 1 + fname_len], suffix);
            watch_res[watch_items][len] = '\0';

            printf("watch list : %s, %d, %d\n", watch_res[watch_items], watch_items, len);

            watch_items++;

        } else if (strncmp(fname, "bgp_w", strlen("bgp_w")) == 0) {
            watch_bgp_add(fname);
        }

    }


    watch_free(fname_buf);

    if (watch_bgp_related_init() != 0) {
        printf("bgp_related_init fail\n");
    } else {
        printf("bgp_related_init succ\n");
    }

    if (watch_select_read_vm() < 0) {
        dial_sty.watch_style = 0;
        dial_sty.standby_watch_style = -1;
    }

#endif

    return 0;
}


REGISTER_WATCH_SYSCFG(watch_select_ops) = {
    .name = "watch_select",
    .read = watch_select_read_vm,
    .write = watch_select_wtrite_vm,
};

static u8 watch_need_load = 0;

void watch_set_need_reload(u8 need)
{
    watch_need_load = !!need;
}

int switch_ui_page(u8 page, u8 prj)
{
    static u8 last_watch_style = 0xff;
    if ((watch_need_load || (last_watch_style != dial_sty.watch_style)) && !page && (prj == 1)) {
        /* printf(">>>>>>>>>>>>>>>>>>%s %d %d %s\n",__FUNCTION__,__LINE__,dial_sty.watch_style,watch_res[dial_sty.watch_style]); */
        ui_set_sty_path_by_pj_id(1, NULL);
        ui_set_sty_path_by_pj_id(1, watch_res[dial_sty.watch_style]);
        if (!watch_res[dial_sty.watch_style] || !strlen(watch_res[dial_sty.watch_style])) {
            printf("watch res is null,watch style =%d\n", dial_sty.watch_style);
            /* ASSERT(0); */
        }
        last_watch_style = dial_sty.watch_style;
        watch_need_load = 0;
    }

#if UI_UPGRADE_RES_ENABLE//升级模式加载资源
    if (app_get_curr_task() == APP_WATCH_UPDATE_TASK ||
        app_get_curr_task() == APP_SMARTBOX_ACTION_TASK) {
        watch_set_need_reload(1);
    }
#endif

    return 0;
}

RESFILE *platform_get_file(int prj)
{
    return ui_load_sty_by_pj_id(prj);
}



static void *br28_load_window(int id)
{
    u8 *ui;
    int i;
    u32 *ptr;
    u16 *ptr_table;
    struct ui_file_head head ALIGNED(4);
    struct window_head window ALIGNED(4);
    int len = sizeof(struct ui_file_head);
    int retry;
    static const int rotate[] = {0, 90, 180, 270};


    if (!ui_file) {
        printf("ui_file : 0x%x\n", ui_file);
        return NULL;
    }
    ui_platform_ok();

    for (retry = 0; retry < set_retry_cnt(); retry++) {
        res_fseek(ui_file, 0, SEEK_SET);
        res_fread(ui_file, &head, len);

        if (id >= head.window_num) {
            return NULL;
        }

        res_fseek(ui_file, sizeof(struct window_head)*id, SEEK_CUR);
        res_fread(ui_file, &window, sizeof(struct window_head));

        u16 crc = CRC16(&window, (u32) & (((struct window_head *)0)->crc_data));
        if (crc == window.crc_head) {
            ui_rotate = rotate[head.rotate];
            ui_core_set_rotate(ui_rotate);
            switch (head.rotate) {
            case 1: /* 旋转90度 */
                ui_hori_mirror = true;
                ui_vert_mirror = false;
                break;
            case 3:/* 旋转270度 */
                ui_hori_mirror = false;
                ui_vert_mirror = true;
                break;
            default:
                ui_hori_mirror = false;
                ui_vert_mirror = false;
                break;
            }
            goto __read_data;
        }
    }

    return NULL;

__read_data:
    ui = (u8 *)br28_malloc(window.len);
    if (!ui) {
        return NULL;
    }
    for (retry = 0; retry < set_retry_cnt(); retry++) {
        res_fseek(ui_file, window.offset, SEEK_SET);
        res_fread(ui_file, ui, window.len);

        u16 crc = CRC16(ui, window.len);
        if (crc == window.crc_data) {
            goto __read_table;
        }
    }

    br28_free(ui);
    return NULL;

__read_table:
    ptr_table = (u16 *)br28_malloc(window.ptr_table_len);
    if (!ptr_table) {
        br28_free(ui);
        return NULL;
    }
    for (retry = 0; retry < set_retry_cnt(); retry++) {
        res_fseek(ui_file, window.ptr_table_offset, SEEK_SET);
        res_fread(ui_file, ptr_table, window.ptr_table_len);

        u16 crc = CRC16(ptr_table, window.ptr_table_len);
        if (crc == window.crc_table) {
            u16 *offset = ptr_table;
            for (i = 0; i < window.ptr_table_len; i += 2) {
                ptr = (u32 *)(ui + *offset++);
                if (*ptr != 0) {
                    *ptr += (u32)ui;
                }
            }
            br28_free(ptr_table);
            return ui;
        }
    }

    br28_free(ui);
    br28_free(ptr_table);

    return NULL;
}

static void br28_unload_window(void *ui)
{
    if (ui) {
        br28_free(ui);
    }
}

extern RESFILE *res_file;
extern RESFILE *str_file;

static int br28_load_style(struct ui_style *style)
{
    int err;
    int i, j;
    int len;
    struct vfscan *fs;
    char name[64];
    char style_name[16];
    static char cur_style = 0xff;


    if (!style->file && cur_style == 0) {
        return 0;
    }


    if (style->file == NULL) {
        ASSERT(0);
        cur_style = 0;
        err = open_resource_file();
        if (err) {
            return -EINVAL;
        }
        ui_file1 = res_fopen(RES_PATH"JL/JL.sty", "r");
        if (!ui_file1) {
            return -ENOENT;
        }
        ui_file = ui_file1;
        ui_file_len = 0x7fffffff;//res_flen(ui_file1);
        len = 6;
        strcpy(style_name, "JL.sty");
        if (len) {
            style_name[len - 4] = 0;
            ui_core_set_style(style_name);
        }
    } else {
#if 0
        cur_style = 1;
        ui_set_sty_path_by_pj_id(0, style->file);
        ui_file = ui_load_sty_by_pj_id(0);
        ASSERT(ui_file);

        ui_file_len = 0x7fffffff;//res_flen(ui_file1);
        for (i = 0; style->file[i] != '.'; i++) {
            name[i] = style->file[i];
        }
        printf("open resfile 0 :%s\n", name);
        name[i++] = '.';
        name[i++] = 'r';
        name[i++] = 'e';
        name[i++] = 's';
        name[i] = '\0';
        printf("open resfile %s\n", name);
        res_file = ui_load_res_by_pj_id(0);

        ASSERT(res_file);
        name[--i] = 'r';
        name[--i] = 't';
        name[--i] = 's';
        printf("open strfile %s\n", name);
        str_file = ui_load_str_by_pj_id(0);
        ASSERT(str_file);

        name[i++] = 'a';
        name[i++] = 's';
        name[i++] = 'i';
        printf("open asciifile %s\n", name);
#endif
        font_ascii_init(RES_PATH"font/ascii.res");

        for (i = strlen(style->file) - 5; i >= 0; i--) {
            if (style->file[i] == '/') {
                break;
            }
        }

        for (i++, j = 0; style->file[i] != '\0'; i++) {
            if (style->file[i] == '.') {
                name[j] = '\0';
                break;
            }
            name[j++] = style->file[i];
        }
        ASCII_ToUpper(name, j);
        if (!strncmp(name, "WATCH", strlen("WATCH"))) {
            strcpy(name, "JL");
        }
        if (!strncmp(name, "UPGRADE", strlen("UPGRADE"))) {
            strcpy(name, "JL");
        }

        err = ui_core_set_style(name);
        if (err) {
            printf("style_err: %s\n", name);
        }
    }

    return 0;

__err2:
    ASSERT(0);
    close_resfile();
__err1:
    res_fclose(ui_file1);
    ui_file1 = NULL;

    return err;
}


static int dc_index = 0;;
static u8 dc_flag = 0;
static int br28_open_draw_context(struct draw_context *dc)
{
    int i;
    for (i = 0; i < 2; i++) {
        if (!(dc_flag & BIT(i))) {
            dc->index = i;
            dc_flag |= BIT(i);
            break;
        }
    }
    /* printf(">>>>>>>>>>>>>%s %d %d %x page:%d\n", __func__, __LINE__, dc_index, dc_flag, ui_id2page(dc->elm->id)); */
    ASSERT(i != 2);

    ASSERT(imb_task_head_check(imb_task_head_get(dc->index)));

    dc_index++;
    ASSERT(dc_index < 3);

    dc->buf_num = 1;
    struct imb_task_info imb_tsk_info = {0};
    /* printf("br28_open_draw_context 0x%08x %d:%d\n", dc->elm->parent->id, dc->elm->parent->id >> 29, (dc->elm->parent->id >> 22) & 0x7f); */

    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&__this->info);
    }

    // 申请缓存buffer并交给imd初始化管理
    if (__this->lcd->buffer_malloc) {
        u8 *buf = NULL;
        u32 len;

        __this->lcd->buffer_malloc(&buf, &len);
        dc->buf = buf;
        dc->len = len;
        lcd_buffer_init(dc->index, buf, len / __this->info.buf_num, __this->info.buf_num);

        dc->buf0 = NULL;
        dc->buf1 = NULL;
    }

    dc->width = __this->info.width;
    dc->height = __this->info.height;
    dc->col_align = __this->info.col_align;
    dc->row_align = __this->info.row_align;
    dc->colums = dc->width;
    dc->lines = dc->len / __this->info.buf_num / __this->info.stride;

    imb_task_head_set_buf(imb_task_head_get(dc->index), dc->buf, dc->len, dc->width, dc->height, __this->info.stride, dc->lines, __this->info.buf_num);

    imb_create_background();

    dc->draw_state = 0;

    return 0;
}


static int br28_get_draw_context(struct draw_context *dc)
{
    dc->disp.left  = 0;
    dc->disp.width = dc->width;
    dc->disp.top   = 0;
    dc->disp.height = dc->height;

    return 0;
}



/*----------------------------------------------------------------------------*/
/**@brief    获取推屏数据接口
   @param    lcd_buf : 图像数据地址
   @param    stride  : 图像每行字节数(4字节对齐),有效图像数据长度为width * 2(RGB565), width * 3(RGB888)
   @param    left : 相对于屏的x坐标
   @param    top  : 相对于屏的y坐标
   @param    width : 图像宽度
   @param    height : 图像高度
   @param    wait : 是否等te信号
   @return
   @note
*/
/*----------------------------------------------------------------------------*/


extern void lcd_wait_te();
extern void lcd_data_copy(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait);
extern int lcd_qspi_st77903_mode();

static void imb_out_data(u8 type, struct rect *rect, u8 *lcd_buf, int stride, int left, int top, int width, int height, int wait)
{
    //printf("0x%x, %d, %d, %d, %d x %d\n", lcd_buf, stride, left, top, width, height);
    //put_buf(lcd_buf, 32);
    // lcd_draw_rect(lcd_buf, stride, left, top, width, height, wait);
    if (wait == 1) {
        lcd_wait_te();
    }
    lcd_data_copy(type, rect, lcd_buf, stride, left, top, width, height, wait);
}

void imb_buffer_unlock(u8 buf_index);
extern void lcd_data_copy_wait();
static void imb_out_over(u8 buf_index)
{
    lcd_data_copy_wait();
    //printf("buf_index : %d\n", buf_index);
    imb_buffer_unlock(buf_index);
}


static int br28_put_draw_context(struct draw_context *dc)
{
    struct rect disp;
    struct imb_task_head *head = imb_task_head_get(dc->index);

    if (!dc->refresh) {
        return -1;
    }

    head->just_record = dc->just_record;
    head->effect_priv = dc->effect_priv;
    head->effect_user = dc->effect_user;
    head->lr_status = dc->lr_status;
    head->new_copy = dc->new_copy;
    head->new_page = dc->new_page;
    head->slider = dc->slider;
    head->copy_to_psram = dc->copy_to_psram;
    head->effect_mode = dc->effect_mode;
    if (lcd_qspi_st77903_mode()) {
        imb_set_output_cb(imb_out_data, imb_out_over);
    }
    imb_task_head_config(head, &dc->rect_orig, &dc->rect, &dc->rect_ref, &dc->page_draw, dc->draw_state);

    struct lcd_drive *lcd = lcd_drv_get_hdl(TCFG_LCD_MATCH_MODE, LCD_LOGO);
    if ((lcd->lcd_reinit == 1) || (lcd->tp_reinit == 1)) {
        int i = lcd->lcd_reinit + lcd->tp_reinit;
        while (i--) {
            os_sem_pend(&lcd->init_sem, 0);
        }
        os_sem_del(&lcd->init_sem, OS_DEL_ALWAYS);
        if (lcd->lcd_reinit) {
            int ret = task_kill("lcd_init");
            ASSERT(!ret);
            lcd->lcd_reinit = 0;
        }
        if (lcd->tp_reinit) {
            int ret = task_kill("tp_init");
            ASSERT(!ret);
            lcd->tp_reinit = 0;
        }
    }

    imb_start(head, ui_core_get_screen_draw_rect(), disp, dc->colums, dc->lines, NULL);

    return 0;
}

void lcd_draw_test(int left, int top, int width, int height)
{
    struct draw_context dc;
    dc.disp.left = left;
    dc.disp.top  = top;
    dc.disp.width = width;
    dc.disp.height = height;

    dc.need_draw.left = left;
    dc.need_draw.top  = top;
    dc.need_draw.width = width;
    dc.need_draw.height = height;

    dc.data_format = DC_DATA_FORMAT_OSD16;
    /* br28_put_draw_context(&dc); */
    dc.buf = get_imb_output_buf(NULL);

    y_printf("dc.disp l:%d w:%d t:%d h:%d", dc.disp.left, dc.disp.width, dc.disp.top, dc.disp.height);
    if (__this->lcd->set_draw_area) {
        __this->lcd->set_draw_area(dc.disp.left, dc.disp.left + dc.disp.width - 1,
                                   dc.disp.top, dc.disp.top + dc.disp.height - 1);
    }

    u8 wait = ((dc.need_draw.top + dc.need_draw.height) == (dc.disp.top + dc.disp.height)) ? 1 : 0;
    if (__this->lcd->draw) {
        if (dc.data_format == DC_DATA_FORMAT_OSD16) {
            __this->lcd->draw(dc.buf, dc.disp.height * dc.disp.width * 2, wait);
            y_printf("dc.need_draw l:%d w:%d t:%d h:%d", dc.need_draw.left, dc.need_draw.width, dc.need_draw.top, dc.need_draw.height);
        } else if (dc.data_format == DC_DATA_FORMAT_MONO) {
            __this->lcd->draw(dc.buf, __this->info.width * __this->info.height / 8, wait);
        }
    }
}
static int br28_set_draw_context(struct draw_context *dc)
{
    return 0;
}

static int br28_close_draw_context(struct draw_context *dc)
{
    dc_index--;

    /* printf(">>>>>>>>>>>>>%s %d %d %x page:%d\n", __func__, __LINE__, dc_index, dc_flag, ui_id2page(dc->elm->id)); */

    ASSERT(dc, ", dc : 0x%x\n", dc);
    struct rect *orig = &dc->rect_orig;
    /* printf("br28_close_draw_context 0x%08x %d:%d [%d, %d, %d, %d]\n", dc->elm->parent->id, dc->elm->parent->id >> 29, (dc->elm->parent->id >> 22) & 0x7f, orig->left, orig->top, orig->width, orig->height); */
    imb_task_all_destroy(imb_task_head_get(dc->index));

    if (__this->lcd->buffer_free) {
        extern bool is_imd_buf_using(u8 index, u8 * lcd_buf);
        while (is_imd_buf_using(dc->index, dc->buf)) {
            os_time_dly(1);
        }
        lcd_buffer_release(dc->index);
        /* lcd_wait(); */
        void dma_memcpy_wait_idle(void);
        dma_memcpy_wait_idle();
        __this->lcd->buffer_free(dc->buf);
    }

    if (dc->fbuf) {
        br28_free(dc->fbuf);
        dc->fbuf = NULL;
        dc->fbuf_len = 0;
    }

    dc_flag &= ~BIT(dc->index);

    return 0;
}


static int br28_invert_rect(struct draw_context *dc, u32 acolor)
{
    int i;
    int len;
    int w, h;
    int color = acolor & 0xffff;

    if (dc->data_format == DC_DATA_FORMAT_MONO) {
        color |= BIT(31);
        for (h = 0; h < dc->draw.height; h++) {
            for (w = 0; w < dc->draw.width; w++) {
                if (platform_api->draw_point) {
                    platform_api->draw_point(dc, dc->draw.left + w, dc->draw.top + h, color);
                }
            }
        }
    }
    return 0;
}

static int br28_fill_rect(struct draw_context *dc, u32 acolor)
{
    struct imb_task_info imb_tsk_info = {0};
    u16 id = CRC16(&acolor, 4);
    struct imb_task *p = NULL;
    u8 elm_index = 0;

    p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);

    imb_tsk_info.group = dc->elm->group;
    imb_tsk_info.rect.left = dc->rect.left - dc->rect_ref.left;
    imb_tsk_info.rect.top = dc->rect.top - dc->rect_ref.top;
    imb_tsk_info.rect.width = dc->rect.width;
    imb_tsk_info.rect.height = dc->rect.height;


    struct rect draw;
    draw.left = dc->draw.left - dc->rect_ref.left;
    draw.top = dc->draw.top - dc->rect_ref.top;
    draw.width = dc->draw.width;
    draw.height = dc->draw.height;

    if (!get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.draw)) {
        if (p) {
            imb_task_enable(p, false);
            return -EFAULT;
        } else {
            imb_tsk_info.task_invisible = true;
        }
    }
    memcpy(&imb_tsk_info.crop, &imb_tsk_info.draw, sizeof(struct rect));

    imb_create_color();

    return 0;
}



__attribute__((always_inline))
static inline void __draw_vertical_line(struct draw_context *dc, int x, int y, int width, int height, int color, int format)
{
    int i, j;
    struct rect r = {0};
    struct rect disp = {0};

    disp.left  = x;
    disp.top   = y;
    disp.width = width;
    disp.height = height;
    if (!get_rect_cover(&dc->draw, &disp, &r)) {
        return;
    }

    switch (format) {
    case DC_DATA_FORMAT_OSD16:
        for (i = 0; i < r.width; i++) {
            for (j = 0; j < r.height; j++) {
                if (platform_api->draw_point) {
                    platform_api->draw_point(dc, r.left + i, r.top + j, color);
                }
            }
        }
        break;
    case DC_DATA_FORMAT_MONO:
        for (i = 0; i < r.width; i++) {
            for (j = 0; j < r.height; j++) {
                if (platform_api->draw_point) {
                    platform_api->draw_point(dc, r.left + i, r.top + j, color);
                }
            }
        }
        break;

    }
}


__attribute__((always_inline))
static inline void __draw_line(struct draw_context *dc, int x, int y, int width, int height, int color, int format)
{
    int i, j;
    struct rect r = {0};
    struct rect disp = {0};

    disp.left  = x;
    disp.top   = y;
    disp.width = width;
    disp.height = height;
    if (!get_rect_cover(&dc->draw, &disp, &r)) {
        return;
    }

    switch (format) {
    case DC_DATA_FORMAT_OSD16:
        for (i = 0; i < r.height; i++) {
            for (j = 0; j < r.width; j++) {
                if (platform_api->draw_point) {
                    platform_api->draw_point(dc, r.left + j, r.top + i, color);
                }
            }
        }
        break;
    case DC_DATA_FORMAT_MONO:
        for (i = 0; i < r.height; i++) {
            for (j = 0; j < r.width; j++) {
                if (platform_api->draw_point) {
                    platform_api->draw_point(dc, r.left + j, r.top + i, color);
                }
            }
        }
        break;
    }
}

static int br28_draw_rect(struct draw_context *dc, struct css_border *border)
{
    int err;
    int offset;
    int color = border->color & 0xffff;

    /* draw_rect_range_check(&dc->draw, map); */
    /* draw_rect_range_check(&dc->rect, map); */

    if (dc->data_format == DC_DATA_FORMAT_OSD16) {
        color = border->color & 0xffff;
    } else if (dc->data_format == DC_DATA_FORMAT_MONO) {
        color = (color != UI_RGB565(RECT_MONO_CLR)) ? (color ? color : 0xffff) : 0x55aa;
    }

    if (border->left) {
        if (dc->rect.left >= dc->draw.left &&
            dc->rect.left <= rect_right(&dc->draw)) {
            __draw_vertical_line(dc, dc->draw.left, dc->draw.top,
                                 border->left, dc->draw.height, color, dc->data_format);
        }
    }
    if (border->right) {
        if (rect_right(&dc->rect) >= dc->draw.left &&
            rect_right(&dc->rect) <= rect_right(&dc->draw)) {
            __draw_vertical_line(dc, dc->draw.left + dc->draw.width - border->right, dc->draw.top,
                                 border->right, dc->draw.height, color, dc->data_format);
        }
    }
    if (border->top) {
        if (dc->rect.top >= dc->draw.top &&
            dc->rect.top <= rect_bottom(&dc->draw)) {
            __draw_line(dc, dc->draw.left, dc->draw.top,
                        dc->draw.width, border->top, color, dc->data_format);
        }
    }
    if (border->bottom) {
        if (rect_bottom(&dc->rect) >= dc->draw.top &&
            rect_bottom(&dc->rect) <= rect_bottom(&dc->draw)) {
            __draw_line(dc, dc->draw.left, dc->draw.top + dc->draw.height - border->bottom,
                        dc->draw.width, border->bottom, color, dc->data_format);
        }
    }

    return 0;
}

__attribute__((always_inline_when_const_args))
AT_UI_RAM
static u16 get_mixed_pixel(u16 backcolor, u16 forecolor, u8 alpha)
{
    u16 mixed_color;
    u8 r0, g0, b0;
    u8 r1, g1, b1;
    u8 r2, g2, b2;

    if (alpha == 255) {
        return (forecolor >> 8) | (forecolor & 0xff) << 8;
    } else if (alpha == 0) {
        return (backcolor >> 8) | (backcolor & 0xff) << 8;
    }

    r0 = ((backcolor >> 11) & 0x1f) << 3;
    g0 = ((backcolor >> 5) & 0x3f) << 2;
    b0 = ((backcolor >> 0) & 0x1f) << 3;

    r1 = ((forecolor >> 11) & 0x1f) << 3;
    g1 = ((forecolor >> 5) & 0x3f) << 2;
    b1 = ((forecolor >> 0) & 0x1f) << 3;

    r2 = (alpha * r1 + (255 - alpha) * r0) / 255;
    g2 = (alpha * g1 + (255 - alpha) * g0) / 255;
    b2 = (alpha * b1 + (255 - alpha) * b0) / 255;

    mixed_color = ((r2 >> 3) << 11) | ((g2 >> 2) << 5) | (b2 >> 3);

    return (mixed_color >> 8) | (mixed_color & 0xff) << 8;
}

static int br28_read_image_info(struct draw_context *dc, u32 id, u8 page, struct ui_image_attrs *attrs)
{
    struct image_file file = {0};

    if (((u16) - 1 == id) || ((u32) - 1 == id)) {
        return -1;
    }

    int err = open_image_by_id(dc->prj, NULL, &file, id, dc->page);
    if (err) {
        return -EFAULT;
    }
    attrs->width = file.width;
    attrs->height = file.height;

    return 0;
}

AT_UI_RAM
int line_update(u8 *mask, u16 y, u16 width)
{
    int i;
    if (!mask) {
        return true;
    }
    for (i = 0; i < (width + 7) / 8; i++) {
        if (mask[y * ((width + 7) / 8) + i]) {
            return true;
        }
    }
    return false;
}

extern void imb_task_lock();
extern void imb_task_unlock();
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

    /* printf("task_en %d\n", task->task_en); */
    //jpeg_rect是相对于图片本身的区域，其他rect是相对于背景

    /* printf("jpeg_page_rect %d, %d, %d, %d\n", jpeg_page_rect->left, jpeg_page_rect->top, jpeg_page_rect->width, jpeg_page_rect->height); */
    /* printf("cur_draw %d, %d, %d, %d\n", cur_draw->left, cur_draw->top, cur_draw->width, cur_draw->height); */

#if 0
    if (!get_rect_cover(jpeg_page_rect, cur_draw, &draw_rect)) {
        return NULL;
    }
#else
    //@add
    struct rect control_draw;//控件相对背景的实际范围，图片可能比控件大
    struct rect r;

    if (!get_rect_cover(jpeg_page_rect, cur_draw, &r)) {
        /* printf("jpeg not in area\n"); */
        return NULL;
    }
    memcpy(&control_draw, (u8 *)out->priv + sizeof(struct rect), sizeof(struct rect));
    if (!get_rect_cover(&control_draw, &r, &draw_rect)) {
        /* printf("jpeg not in area\n"); */
        return NULL;
    }

    /* printf("control_draw %d, %d, %d, %d\n", control_draw.left, control_draw.top, control_draw.width, control_draw.height); */
    /* printf("draw_rect %d, %d, %d, %d\n", draw_rect.left, draw_rect.top, draw_rect.width, draw_rect.height); */

#endif
    jpeg_rect.left = draw_rect.left - jpeg_page_rect->left;
    jpeg_rect.top = draw_rect.top - jpeg_page_rect->top;
    jpeg_rect.width = draw_rect.width;
    jpeg_rect.height = draw_rect.height;

    if ((jpeg_rect.left < 0) || (jpeg_rect.top < 0)
        || (jpeg_rect.left + jpeg_rect.width > jpg_fd->info.x)
        || (jpeg_rect.top + jpeg_rect.height > jpg_fd->info.y)) {
        printf("\nerr_draw_jpeg %d, %d, %d, %d, %d, %d\n", jpeg_rect.left, jpeg_rect.top, jpeg_rect.width, jpeg_rect.height, jpg_fd->info.x, jpg_fd->info.y);
        return NULL;
    }

    /* printf("cur_draw %d, %d, %d, %d\n", cur_draw->left, cur_draw->top, cur_draw->width, cur_draw->height); */
    /* printf("draw_rect %d, %d, %d, %d\n", draw_rect.left, draw_rect.top, draw_rect.width, draw_rect.height); */
    /* printf("jpeg_rect %d, %d, %d, %d\n", jpeg_rect.left, jpeg_rect.top, jpeg_rect.width, jpeg_rect.height); */

    imb_task_lock();
    ret = jpeg_dec_start(cur_draw, &draw_rect, &jpeg_rect, out->outbuf);
    /* if (ret == 1) { */
    /* printf("\ndec cline\n"); */
    /* jpeg_dec_init(task->flash_file_info, out->format, NULL); */
    /* jpeg_dec_start(cur_draw, &draw_rect, &jpeg_rect, out->outbuf); */
    /* } */
    imb_task_unlock();

    return NULL;
}

static int br28_draw_image(struct draw_context *dc, u32 src, u8 quadrant, u8 *mask)
{
    struct imb_task_info imb_tsk_info = {0};
    struct image_file file = {0};
    RESFILE *fp;
    int id;
    int page;
    struct flash_file_info *file_info;
    u16 new_width, new_height;

    if (dc->elm->css.invisible) {
        printf("image invisible\n");
        return -1;
    }

    imb_tsk_info.group = dc->elm->group;

    if (dc->preview.file) {
        fp = dc->preview.file;
        id = dc->preview.id;
        page = dc->preview.page;
        file_info = dc->preview.file_info;
    } else {
        fp = NULL;
        id = src;
        page = dc->page;
        file_info = ui_get_image_file_info_by_pj_id(dc->prj);
    }

    int x = dc->rect.left;
    int y = dc->rect.top;

    if (dc->draw_img.en) {
        id = dc->draw_img.id;
        page = dc->draw_img.page;
    }

    if (((u16) - 1 == id) || ((u32) - 1 == id)) {
        return -1;
    }

    /* printf("fp:0x%x, id:%d, page:%d\n", fp, id, page); */
    int err = open_image_by_id(dc->prj, fp, &file, id, page);
    if (err) {
        printf("%s, read file err\n", __func__);
        return -EFAULT;
    }

    ASSERT(file.width < 1024);
    ASSERT(file.height < 512);

    new_width = file.width;
    new_height = file.height;
    imb_tsk_info.scale_en       = dc->elm->css.ratio.en;
    if (imb_tsk_info.scale_en) {
        imb_tsk_info.ratio_w = dc->elm->css.ratio.ratio_w;
        imb_tsk_info.ratio_h = dc->elm->css.ratio.ratio_h;
        if (imb_tsk_info.ratio_w == 0) {
            imb_tsk_info.ratio_w = 1;
        }
        if (imb_tsk_info.ratio_h == 0) {
            imb_tsk_info.ratio_h = 1;
        }

        if (imb_tsk_info.ratio_w > 1) {
            new_width = file.width * imb_tsk_info.ratio_w;
        }
        if (imb_tsk_info.ratio_h > 1) {
            new_height = file.height * imb_tsk_info.ratio_h;
        }
    }

    imb_tsk_info.rotate_en      = dc->elm->css.rotate.en;
    if (imb_tsk_info.rotate_en) {
        imb_tsk_info.x_src_offset   = 0;
        imb_tsk_info.y_src_offset   = 0;
        imb_tsk_info.rotate_c_x     = dc->elm->css.rotate.cent_x;
        imb_tsk_info.rotate_c_y     = dc->elm->css.rotate.cent_y;
        imb_tsk_info.rotate_angle   = dc->elm->css.rotate.angle;
        imb_tsk_info.rotate_dx      = dc->elm->css.rotate.dx;
        imb_tsk_info.rotate_dy      = dc->elm->css.rotate.dy;
        if (dc->elm->css.rotate.chunk) {
            imb_tsk_info.rotate_chunk_offset = 0;
            imb_tsk_info.rotate_chunk_num = 8;
            imb_tsk_info.rotate_chunk = (file.height / imb_tsk_info.rotate_chunk_num + 9) / 10 * 10;
            imb_tsk_info.rotate_chunk_num = (file.height + imb_tsk_info.rotate_chunk - 1) / imb_tsk_info.rotate_chunk;
        } else {
            imb_tsk_info.rotate_chunk = 0;
        }
    }

    if (dc->align == UI_ALIGN_CENTER) {
        /* if (dc->rect.width > new_width) { */
        x += (dc->rect.width / 2 - new_width / 2);
        /* } */
        /* if (dc->rect.height > new_height) { */
        y += (dc->rect.height / 2 - new_height / 2);
        /* } */
    } else if (dc->align == UI_ALIGN_RIGHT) {
        x += dc->rect.width - new_width;
    }

    if (dc->draw_img.en) {
        x = dc->draw_img.x;
        y = dc->draw_img.y;
    }

    imb_tsk_info.scale_en       = dc->elm->css.ratio.en;
    if (imb_tsk_info.scale_en) {
        imb_tsk_info.ratio_w = dc->elm->css.ratio.ratio_w;
        imb_tsk_info.ratio_h = dc->elm->css.ratio.ratio_h;
        if (imb_tsk_info.ratio_w == 0) {
            imb_tsk_info.ratio_w = 1;
        }
        if (imb_tsk_info.ratio_h == 0) {
            imb_tsk_info.ratio_h = 1;
        }
        x = dc->rect.left;
        y = dc->rect.top;
        if (dc->align == UI_ALIGN_CENTER) {
            x += (dc->rect.width / 2 - (file.width * imb_tsk_info.ratio_w) / 2);
            y += (dc->rect.height / 2 - (file.height * imb_tsk_info.ratio_h) / 2);
        } else if (dc->align == UI_ALIGN_RIGHT) {
            x += dc->rect.width - new_width;
        }
    }

    u8 elm_index = 1;
    if (dc->draw_img.en) {
        elm_index = dc->elm_index++;
    }
    struct imb_task *p = NULL;
    p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);

    ASSERT((dc->elm->css.rotate.en == 0) || (dc->elm->css.ratio.en == 0), "rotate and ratio cannot enable at the same time");

    struct rect draw;
    if (dc->elm->css.rotate.en) {
        int cent_x = dc->rect.left + dc->elm->css.rotate.cent_x;
        int cent_y = dc->rect.top + dc->elm->css.rotate.cent_y;
        int radius = dc->elm->css.rotate.cent_y;

        imb_tsk_info.rect.left = cent_x - radius - dc->rect_ref.left;
        imb_tsk_info.rect.top = cent_y - radius - dc->rect_ref.top;
        imb_tsk_info.rect.width = 2 * radius;
        imb_tsk_info.rect.height = 2 * radius;

        draw.left = cent_x - radius - dc->rect_ref.left;
        draw.top = cent_y - radius - dc->rect_ref.top;
        draw.width = 2 * radius;
        draw.height = 2 * radius;
    } else if (imb_tsk_info.scale_en) {
        imb_tsk_info.rect.left = x - dc->rect_ref.left;
        imb_tsk_info.rect.top = y - dc->rect_ref.top;
        imb_tsk_info.rect.width = new_width;
        imb_tsk_info.rect.height = new_height;

        draw.left = x - dc->rect_ref.left;
        draw.top = y - dc->rect_ref.top;
        draw.width = new_width;
        draw.height = new_height;
    } else if (dc->draw_img.en) {
        imb_tsk_info.rect.left = x - dc->draw_img.rect.left - dc->rect_ref.left;
        imb_tsk_info.rect.top = y - dc->draw_img.rect.top - dc->rect_ref.top;
        imb_tsk_info.rect.width = new_width;
        imb_tsk_info.rect.height = new_height;

        if (dc->draw_img.rect.left > (new_width - 1)) {
            dc->draw_img.rect.left = new_width - 1;
        }

        if (dc->draw_img.rect.top > (new_height - 1)) {
            dc->draw_img.rect.top = new_height - 1;
        }

        if ((dc->draw_img.rect.left + dc->draw_img.rect.width) > new_width) {
            dc->draw_img.rect.width = new_width - dc->draw_img.rect.left;
        }
        if ((dc->draw_img.rect.top + dc->draw_img.rect.height) > new_height) {
            dc->draw_img.rect.height = new_height - dc->draw_img.rect.top;
        }

        draw.left = x - dc->rect_ref.left;
        draw.top = y - dc->rect_ref.top;
        draw.width = dc->draw_img.rect.width;
        draw.height = dc->draw_img.rect.height;

        x -= dc->draw_img.rect.left;
        y -= dc->draw_img.rect.top;
    } else {
        imb_tsk_info.rect.left = x - dc->rect_ref.left;
        imb_tsk_info.rect.top = y - dc->rect_ref.top;
        imb_tsk_info.rect.width = new_width;
        imb_tsk_info.rect.height = new_height;

        draw.left = dc->draw.left - dc->rect_ref.left;
        draw.top = dc->draw.top - dc->rect_ref.top;
        draw.width = dc->draw.width;
        draw.height = dc->draw.height;
    }

    if (!get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.draw)) {
        if (p) {
            imb_task_enable(p, false);
            return -EFAULT;
        } else {
            imb_tsk_info.task_invisible = true;
        }
    }

    memcpy(&imb_tsk_info.crop, &imb_tsk_info.draw, sizeof(struct rect));

    imb_create_image(file_info);

    if (file.format == PIXEL_FMT_JPEG) {
        /* printf("x: %d, ref %d, %d, %d\n", x, dc->rect_ref.left, dc->rect_ref.top, dc->draw_img.en); */
        /* printf("img0 %d, %d, %d, %d\n", imb_tsk_info.rect.left, imb_tsk_info.rect.top, imb_tsk_info.rect.width, imb_tsk_info.rect.height); */
        /* printf("img1 %d, %d, %d, %d\n", draw.left, draw.top, draw.width, draw.height); */
        struct rect jpeg_page_rect;
        jpeg_page_rect.left = imb_tsk_info.rect.left;
        jpeg_page_rect.top = imb_tsk_info.rect.top;
        jpeg_page_rect.width = imb_tsk_info.rect.width;
        jpeg_page_rect.height = imb_tsk_info.rect.height;

        p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);
        if (p) {
            memcpy(p->priv_buf, &jpeg_page_rect, sizeof(struct rect));
            //@add
            memcpy((u8 *)p->priv_buf + sizeof(struct rect), &draw, sizeof(struct rect));
            p->cb_priv = p->priv_buf;
        }
    }


    return 0;
}

void ui_draw_cb(u8 *dst_buf, struct rect *dst_r, u8 *src_buf, struct rect *src_r, u8 bytes_per_pixel)
{
    int w, h;
    struct rect r;
    int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
    int src_stride = (src_r->width * bytes_per_pixel + 3) / 4 * 4;

    if (get_rect_cover(dst_r, src_r, &r)) {
        for (h = 0; h < r.height; h++) {
            memcpy(&dst_buf[(r.top + h - dst_r->top) * dst_stride + (r.left - dst_r->left) * bytes_per_pixel],
                   &src_buf[(r.top + h - src_r->top)*src_stride + (r.left - src_r->left) * bytes_per_pixel],
                   r.width * bytes_per_pixel);
        }
    }
}


extern void imb_task_calc_crc(struct imb_task *task);
extern int imb_task_check_crc(struct imb_task *task);
static void *imb_draw_cb(void *priv)
{
    struct imb_task_out *out = (struct imb_task_out *)priv;
    struct imb_task *task = out->task;

    ASSERT(imb_task_check_crc(task));

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

    imb_task_calc_crc(task);

    if (task->user_cb) {
        void (*cb)(int id, u8 * dst_buf, struct rect * dst_r, struct rect * src_r, u8 bytes_per_pixel, void *priv) = out->priv;
        cb(task->cb_id, out->outbuf, &out->rect, &task->rect, (out->format == OUTPUT_FORMAT_RGB888) ? 3 : 2, task->priv_buf);//不带alpha的显示
    } else {
        ui_draw_cb(out->outbuf, &out->rect, out->priv, &task->rect, (out->format == OUTPUT_FORMAT_RGB888) ? 3 : 2);//不带alpha的显示
    }

    return NULL;
}


int ui_draw(struct draw_context *dc, u8 *buf, int x, int y, int width, int height, void *cb, void *priv, int priv_len, int id)
{
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;
    struct element *elm = dc->elm;
    struct rect rect;
    u8 elm_index = dc->elm_index++;

    p = imb_task_search_by_id(imb_task_head_get(dc->index), elm->id, elm_index);
    imb_tsk_info.group = dc->elm->group;

    imb_create_draw();

    return 0;
}

extern int open_string_pic2(int prj, struct image_file *file, int id);
static int get_strpic_width(struct draw_context *dc, u8 *str, int *str_width, int *str_height, int str_num)
{
    struct image_file file = {0};
    u16 *p = (u16 *)str;
    int width = 0;
    int max_h = 0;
    int i;

    for (i = 0; i < str_num; i++) {
        if (open_string_pic2(dc->prj, &file, *p)) {
            return -EINVAL;
        }

        printf("file_w %d, file_h %d\n", file.width, file.height);
        width += file.width;
        max_h = (MAX(file.height, max_h));
        p++;
    }
    *str_width = width;
    *str_height = max_h;

    if ((*str_width == 0) || (*str_height == 0)) {
        printf("get_strpic_width_err\n");
        return -EINVAL;
    }

    return 0;
}



static int get_multi_strs_width(struct draw_context *dc, u8 *str, int *str_width, int *str_height, int *strnum)
{
    struct image_file file = {0};
    u8 *str_index_buf = NULL;
    u16 *pstr = NULL;
    int max_h = 0;
    int w, h;
    u16 *p = (u16 *)str;
    int width = 0;
    int num = 0;

    while (*p != 0) {
        if (open_string_pic(dc->prj, &file, *p)) {
            printf("now here3\n");
            if ((width > 0) && (max_h > 0) && (num > 0)) {
                break;
            }
            return -EINVAL;
        }
        if (!file.len) {
            printf("now here2\n");
            p++;
            continue;
        }

        printf("ones len %d\n", file.len);

        w = file.width;
        h = file.height;

        /* str_index_buf = (u8 *)br28_malloc(file.len); */

        /* read_str_data(&file, str_index_buf, file.len); */
        /* pstr = (u16 *)str_index_buf; */

        /* if (get_strpic_width(dc, &pstr[1], &w, &h, pstr[0])) { */
        /* br28_free(str_index_buf); */
        /* printf("now here\n"); */
        /* p++; */
        /* continue; */
        /* } */

        width += w;
        max_h = (MAX(h, max_h));
        p++;
        num++;
        /* br28_free(str_index_buf); */
    }
    *str_width = width;
    *str_height = max_h;
    *strnum = num;

    if ((*str_width == 0) || (*str_height == 0)) {
        printf("now here4\n");
        return -EINVAL;
    }

    return 0;
}


int get_multi_string_width(struct draw_context *dc, u8 *str, int *str_width, int *str_height)
{
    struct image_file file = {0};
    u16 *p = (u16 *)str;
    int width = 0;
    while (*p != 0) {
        if (open_string_pic(dc->prj, &file, *p)) {
            return -EINVAL;
        }
        width += file.width;
        p++;
    }
    *str_width = width;
    *str_height = file.height;

    return 0;
}

struct font_info *text_font_init(u8 init)
{
    static struct font_info *info = NULL;
    static int language = 0;

    if (init) {
        if (!info || (language != font_lang_get())) {
            language = font_lang_get();
            if (info) {
                font_close(info);
            }
            info = font_open(NULL, language);
            ASSERT(info, "font_open fail!");
        }
    } else {
        if (info) {
            font_close(info);
            info = NULL;
        }
    }

    return info;
}

u16 ui_get_text_width_and_height(u8 encode, u8 *str, u16 strlen, u16 elm_width, u16 elm_height, u8 flags, char *value_type)
{
    int ret;
    int width = 0;
    int height = 0;
    struct font_info *info = text_font_init(true);
    if (info) {
        info->text_width  = elm_width;
        info->text_height = elm_height;
        info->flags       = flags;
        if (encode == FONT_ENCODE_ANSI) {
            width = font_text_width(info, str, strlen);
        } else if (encode == FONT_ENCODE_UNICODE) {
            width = font_textw_width(info, str, strlen);
        } else {
            width = font_textu_width(info, str, strlen);
        }
        height = info->string_height;
    }

    if (!strcmp(value_type, "width")) {
        ret = width;
    } else if (!strcmp(value_type, "height")) {
        ret = height;
    } else {
        ret = 0;
    }

    return ret;
}


int br28_show_element(struct draw_context *dc)
{
    imb_task_enable_by_id(imb_task_head_get(dc->index), dc->elm->id, true);

    return 0;
}

int br28_hide_element(struct draw_context *dc)
{
    imb_task_enable_by_id(imb_task_head_get(dc->index), dc->elm->id, false);

    return 0;
}

int br28_delete_element(struct draw_context *dc)
{
    imb_task_delete_by_elm(imb_task_head_get(dc->index), dc->elm->id);

    return 0;
}

int br28_set_element_prior(struct draw_context *dc)
{
    imb_task_list_set_prior(imb_task_head_get(dc->index), dc->elm->id, dc->elm->prior);

    return 0;
}

void font_get_text_width(u8 encode, struct font_info *info, u8 *str, u16 strlen)
{
    if (encode == FONT_ENCODE_ANSI) {
        font_text_width(info, str, strlen);
    } else if (encode == FONT_ENCODE_UNICODE) {
        font_textw_width(info, str, strlen);
    } else {
        font_textu_width(info, str, strlen);
    }
}

u16 font_text_out(u8 encode, struct font_info *info, u8 *str,  u16 strlen,  u16 x,  u16 y)
{
    u16 len;
    if (encode == FONT_ENCODE_ANSI) {
        len = font_textout(info, str, strlen, x, y);
    } else if (encode == FONT_ENCODE_UNICODE) {
        len = font_textout_unicode(info, str, strlen, x, y);
    } else {
        len = font_textout_utf8(info, str, strlen, x, y);
    }

    return len;
}

//0:原来方式 1:拼接单个strpic方式 2:字库方式
//0：图片 1：索引 2：编码
extern int read_string_type(int prj);

static int br28_show_text(struct draw_context *dc, struct ui_text_attrs *text)
{
    int offset = 0;
    struct rect draw_r;
    int elm_task_num;
    /* 控件从绝对x,y 转成相对图层的x,y */
    int x = dc->rect.left;
    int y = dc->rect.top;
    struct image_file file;
    struct imb_task_info imb_tsk_info = {0};
    struct imb_task *p = NULL;
    u8 *pixbuf = NULL;
    int strpic_flag = 0;
    u8 *strpic_tbuf = NULL;
    u8 *strpic_idbuf = NULL;
    int org_lan = Chinese_Simplified;
    static char mulstr_tbuf[128] ALIGNED(4);

    if (dc->elm->css.invisible) {
        printf("text invisible\n");
        return -1;
    }
    imb_tsk_info.group = dc->elm->group;

    /* 绘制区域从绝对x,y 转成相对图层的x,y */
    draw_r.left   = dc->draw.left;
    draw_r.top    = dc->draw.top;
    draw_r.width  = dc->draw.width;
    draw_r.height = dc->draw.height;
    /* font_set_offset_table(lange_offset_table); */

    if (text->format && !strcmp(text->format, "text")) {

__strpic_with_text:

        if (!text->str || !text->strlen) {
            if (strpic_flag) {
                if (strpic_tbuf) {
                    br28_free(strpic_tbuf);
                    strpic_tbuf = NULL;
                }
                text->str = strpic_idbuf;
                font_lang_set(org_lan);
            }
            return -EFAULT;
        }
        /* printf("language_get: %d\n", font_lang_get()); */

        struct font_info *info = text_font_init(true);
        info->default_code = text->default_code;
        info->line_space = text->line_space;
        info->word_space = text->word_space;

        if (info && (FT_ERROR_NONE == (info->sta & (~(FT_ERROR_NOTABFILE | FT_ERROR_NOPIXFILE))))) {
            draw_r.left -= dc->rect_ref.left;
            draw_r.top -= dc->rect_ref.top;

            info->disp.map    = 0;
            info->disp.rect   = &draw_r;
            info->disp.format = dc->data_format;
            info->disp.color  = text->color;
            info->dc = dc;
            info->text_width  = dc->rect.width;//draw_r.width;
            info->text_height = dc->rect.height;//draw_r.height;
            info->flags       = text->flags;

            if (text->encode == FONT_ENCODE_UNICODE) {
                if (text->endian == FONT_ENDIAN_BIG) {
                    info->bigendian = true;
                } else {
                    info->bigendian = false;
                }
            }
            font_get_text_width(text->encode, info, (u8 *)text->str, text->strlen);
            text->width = info->string_width;
            text->height = info->string_height;

            /* 判断字符串长度是否大于文本框长度, 并且打开滚动标志 */
            if ((text->width > info->text_width) && (info->flags & FONT_SHOW_SCROLL)) {
                text->width = info->text_width / 2 * 3; //用于字库缓存buf大小计算
            }
            int width = text->width;
            int height = text->height;

            if (strpic_flag) {
                /* printf("w %d, h %d, as %d, ps %d\n", width, height, info->ascpixel.size, info->pixel.size); */
            }

            if (height == 0) {
                if (info->ascpixel.size) {
                    height = info->ascpixel.size;
                } else if (info->pixel.size) {
                    height = info->pixel.size;
                } else {
                    ASSERT(0, "can't get the height of font.");
                }
            }

            int roll = 0;
            if (width > dc->rect.width) {
                if (info->flags & FONT_SHOW_SCROLL) {
                    roll = 1;
                } else {
                    width = dc->rect.width;
                }
            }

            if (height > dc->rect.height) {
                height = dc->rect.height;
            }

            y += (dc->rect.height / 2 - height / 2);
            int font_lang_id = font_lang_get();
            if (dc->align == UI_ALIGN_CENTER && !roll) {
                if (font_lang_id == Arabic || font_lang_id == Hebrew || font_lang_id == UnicodeMixLeftword) { //显示方向从右到左
                    if (dc->rect.width > width) {
                        x -= (dc->rect.width / 2 - width / 2);
                        width = dc->rect.width;
                    }
                } else {
                    x += (dc->rect.width / 2 - width / 2);
                }
            } else if (dc->align == UI_ALIGN_RIGHT) {
                x += (dc->rect.width - width);
            }

            if (dc->draw_img.en) {//指定坐标刷新
                x = dc->draw_img.x;
                y = dc->draw_img.y;
            }

            info->x = x;
            info->y = y;

            u8 elm_index = 2;
            if (dc->draw_img.en) {
                elm_index = dc->elm_index++;
            }
            u16 id = CRC16(text->str, text->strlen);
            p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);

            u32 curr_id = ((dc->page << 26) | (3 << 24) | (id << 8) | elm_index);
            if (p) {
                if (roll && (p->id == curr_id) && (p->priv_buf[0] != text->offset || roll)) {
                    p->priv_buf[0] = text->offset;
                    p->scroll = 1;
                } else {
                    p->scroll = 0;
                }

                if (strpic_flag) {
                    /* printf("p->scroll %d, p->id %x, off %d, %d\n", p->scroll, p->id, p->priv_buf[0], text->offset); */
                    if (roll) {
                        p->scroll = 1;
                    }
                    /* printf("x %d, y %d, dc_w %d, dc_h %d, roll %d\n", x, y, dc->rect.width, dc->rect.height, roll); */
                }
            }

            if ((!p) || (p->id != curr_id) || p->scroll) {
                info->text_image_width = width;
                info->text_image_height = height;
                info->text_image_stride = (width + 31) / 32 * 32 / 8;
                info->text_image_buf_size = info->text_image_stride * height;
                info->text_image_buf = br28_zalloc(info->text_image_buf_size);
                if (!info->text_image_buf) {
                    if (strpic_flag) {
                        if (strpic_tbuf) {
                            br28_free(strpic_tbuf);
                            strpic_tbuf = NULL;
                        }
                        text->str = strpic_idbuf;
                        font_lang_set(org_lan);
                    }
                    return 0;
                }

                u16 recored_text_width = info->text_width;
                int len = 0;
                if ((info->flags & FONT_SHOW_SCROLL) && roll) {
                    u8 roll_step = 12;
                    if (font_lang_id == Arabic || font_lang_id == Hebrew || font_lang_id == UnicodeMixLeftword) { //显示方向从右到左
                        if (text->scroll_cnt == 0 || text->offsetx < 0) {
                            text->offsetx = recored_text_width / 2;  //重头刷新或者偏移到字库buf的尾部临界位置时,更新偏移变量
                            text->scroll_cnt++;
                        }
                        x -= text->offsetx;  //字库buf宽度大于文本框宽度，通过对字库buf坐标偏移实现滚动效果
                        text->offsetx -= roll_step; //滚动步进，单位1像素
                        //info->xpos_offset用在putchar函数中，用于判断在哪个位置开始填buf
                        info->xpos_offset = (text->scroll_cnt - 1) * recored_text_width / 2;
                        //主动改变info->text_width的值，让字库内部解析相应长度的字符串，后续还原
                        info->text_width = recored_text_width + recored_text_width / 2 * text->scroll_cnt;
                        len = font_text_out(text->encode, info, (u8 *)(text->str), text->strlen, x - dc->rect_ref.left, y - dc->rect_ref.top);
                        if (info->string_width < info->text_width) {
                            u8 extra_offset = info->text_width - info->string_width - info->pixel.size * 1;
                            extra_offset = extra_offset < 0 ? 0 : extra_offset;
                            if (text->offsetx < extra_offset) {
                                text->scroll_cnt = 0; //重头循环刷新,info->pixel.size * x 控制末尾结束循环的位置
                            }
                        }
                        text->displen = len;
                        info->text_width = recored_text_width; //还原文本框本身宽度
                    } else { //显示方向自左向右
                        if (text->scroll_cnt == 0 || text->offsetx > info->text_width / 2) {
                            text->offsetx = 0;  //重头刷新或者偏移到字库buf的尾部临界位置时,更新偏移变量
                            text->scroll_cnt++;
                        }
                        x -= text->offsetx;  //字库buf宽度大于文本框宽度，通过对字库buf坐标偏移实现滚动效果
                        text->offsetx += roll_step; //滚动步进，单位1像素
                        //info->xpos_offset用在putchar函数中，用于判断在哪个位置开始填buf
                        info->xpos_offset = (text->scroll_cnt - 1) * recored_text_width / 2;
                        //主动改变info->text_width的值，让字库内部解析相应长度的字符串，后续还原
                        info->text_width = recored_text_width + recored_text_width / 2 * text->scroll_cnt;
                        len = font_text_out(text->encode, info, (u8 *)(text->str), text->strlen, x - dc->rect_ref.left, y - dc->rect_ref.top);
                        if (info->string_width < info->text_width) {
                            u8 extra_offset = recored_text_width / 2 - (info->text_width - info->string_width) + info->pixel.size * 1;
                            extra_offset = extra_offset > recored_text_width / 2 ? recored_text_width / 2 : extra_offset;
                            if (text->offsetx > extra_offset) {
                                text->scroll_cnt = 0; //重头循环刷新,info->pixel.size * x 控制末尾结束循环的位置
                            }
                        }
                        text->displen = len;
                        info->text_width = recored_text_width; //还原文本框本身宽度
                    }
                } else {
                    len = font_text_out(text->encode, info, (u8 *)(text->str + roll * text->offset), text->strlen - roll * text->offset, x - dc->rect_ref.left, y - dc->rect_ref.top);
                }
                pixbuf = info->text_image_buf;

                if (strpic_flag) {
                    if (roll) {
                        text->flags |= FONT_SHOW_SCROLL;
                    }
                }
            }

            if (strpic_flag) {
                if (strpic_tbuf) {
                    br28_free(strpic_tbuf);
                    strpic_tbuf = NULL;
                }
                text->str = strpic_idbuf;
                font_lang_set(org_lan);
            }

            imb_create_text(width, height);
        } else {
            if (strpic_flag) {
                if (strpic_tbuf) {
                    br28_free(strpic_tbuf);
                    strpic_tbuf = NULL;
                }
                text->str = strpic_idbuf;
                font_lang_set(org_lan);
            }
        }

    } else if (text->format && !strcmp(text->format, "ascii")) {
        char *str;
        u32 w_sum;
        if (!text->str) {
            return 0;
        }
        if ((u8)text->str[0] == 0xff) {
            return 0;
        }

        if (dc->align == UI_ALIGN_CENTER) {
            w_sum = font_ascii_width_check(text->str);
            /* if (dc->rect.width > w_sum) { */
            x += (dc->rect.width / 2 - w_sum / 2);
            /* } */
        } else if (dc->align == UI_ALIGN_RIGHT) {
            w_sum = font_ascii_width_check(text->str);
            x += (dc->rect.width - w_sum);
        }

        if (dc->draw_img.en) {//指定坐标刷新
            x = dc->draw_img.x;
            y = dc->draw_img.y;
        }

        str = text->str;

        int width;
        int height;
        u32 realsize_w = 0;
        u32 realsize_h = 0;
        u8 *pixbuf_temp;
        u16 id = CRC16(str, strlen(str));

        u8 elm_index = 2;
        if (dc->draw_img.en) {
            elm_index = dc->elm_index++;
        }
        p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);

        if ((!p) || (p->id != ((dc->page << 26) | (3 << 24) | (id << 8) | elm_index))) {
            dc->fbuf_len = 128;
            if (!dc->fbuf) {
                dc->fbuf = br28_zalloc(dc->fbuf_len);
                if (!dc->fbuf) {
                    return 0;
                }
            }

            str = text->str;
            while (*str) {
                font_ascii_get_pix(*str, dc->fbuf, dc->fbuf_len, &height, &width);
                realsize_w += width;
                str++;
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

            str = text->str;
            while (*str) {
                int i;
                int color;

                memset(dc->fbuf, 0, dc->fbuf_len);
                font_ascii_get_pix(*str, dc->fbuf, dc->fbuf_len, &height, &width);
                for (i = 0; i < height / 8; i++) {
                    memcpy(pixbuf_temp + realsize_w * i + offset, dc->fbuf + width * i, width);
                }
                offset += width;
                str++;
            }

            l1_data_transformation(pixbuf_temp, pixbuf, stride, x, y, realsize_h, realsize_w);

            if (pixbuf_temp) {
                br28_free(pixbuf_temp);
            }

            if (dc->fbuf) {
                br28_free(dc->fbuf);
                dc->fbuf = NULL;
                dc->fbuf_len = 0;
            }
        } else {//计算ascii字符串的总宽度realsize_w以及高度realsize_h
            dc->fbuf_len = 128;
            if (!dc->fbuf) {
                dc->fbuf = br28_zalloc(dc->fbuf_len);
                if (!dc->fbuf) {
                    return 0;
                }
            }

            str = text->str;
            while (*str) {
                font_ascii_get_pix(*str, dc->fbuf, dc->fbuf_len, &height, &width);
                realsize_w += width;
                str++;
            }
            realsize_h = height;

            if (dc->fbuf) {
                br28_free(dc->fbuf);
                dc->fbuf = NULL;
                dc->fbuf_len = 0;
            }
        }

        imb_create_text(realsize_w, realsize_h);
    } else if (text->format && !strcmp(text->format, "strpic")) {
        u16 id = *((u16 *)text->str);

        if (open_string_pic(dc->prj, &file, id)) {
            return 0;
        }

        if (!file.width || !file.height) {
            return 0;
        }
        if (!file.len) {
            return 0;
        }

        ASSERT(file.width < 1024, "strpic text width can not over 1024 pix!");

        /* printf("strpic: %x, filelen %d, id %x, %x, %x\n", dc->elm->id, file.len, id, (u32)text, (u32)text->str); */

        int w, h;
        u8 *str_index_buf = NULL;
        u16 *pstr = NULL;
        int strpic_num;
        int i, j;

        //0：图片 1：索引 2：编码
        int strpic_mode = read_string_type(dc->prj);
        /* printf("strpic_mode %d\n", strpic_mode); */
        if (strpic_mode == 0) {
            w = file.width;
            h = file.height;
            strpic_num = 1;
        } else if (strpic_mode == 1) {
            str_index_buf = (u8 *)br28_malloc(file.len);
            if (!str_index_buf) {
                return 0;
            }
            read_str_data(&file, str_index_buf, file.len);
            pstr = (u16 *)str_index_buf;
            strpic_num = pstr[0];
            /* put_buf(str_index_buf, file.len); */

            w = file.width;
            h = file.height;
            /* if (get_strpic_width(dc, &pstr[1], &w, &h, pstr[0])) { */
            /* br28_free(str_index_buf); */
            /* return -EINVAL; */
            /* } */
        } else {
            /* static char fontstr[] = {"海拔气压计"}; */
            /* static u16 uincode_buf[32] = {0}; */
            /* int unicode_len = UTF82Unicode(fontstr, uincode_buf, strlen(fontstr)); */
            /* printf("unicode_len %d, strlen %d\n", unicode_len, strlen(fontstr)); */
            /* printf("unicode buf\n"); */
            /* put_buf(uincode_buf, unicode_len * 2); */

            strpic_idbuf = text->str;
            strpic_tbuf = (u8 *)br28_malloc(file.len);
            if (!strpic_tbuf) {
                return 0;
            }
            read_str_data(&file, strpic_tbuf, file.len);
            /* printf("mystr %d, %x\n", dc->data_format, text->color);	 */
            /* put_buf(&strpic_tbuf[2], file.len - 2); */
            /* put_buf(strpic_tbuf, file.len); */

            text->str = &strpic_tbuf[2];
            text->strlen = file.len - 2;
            text->encode = FONT_ENCODE_UNICODE;
            /* put_buf(text->str, text->strlen); */
            /* dc->data_format = DC_DATA_FORMAT_OSD16; */
            /* text->color = 0xffff; */
            text->flags = FONT_SHOW_PIXEL;
            text->endian = FONT_ENDIAN_SMALL;

            org_lan = font_lang_get();
            if ((org_lan == Arabic) || (org_lan == Hebrew)) {
                font_lang_set(UnicodeMixLeftword);
            } else {
                font_lang_set(UnicodeMixRightword);
            }
            org_lan = font_lang_get();
            strpic_flag = 1;
            goto __strpic_with_text;
        }

        /* printf("total_w %d, total_h %d, dc_w %d\n", w, h, dc->rect.width); */

        y += (dc->rect.height / 2 - /*file.height*/h / 2);
        if (dc->align == UI_ALIGN_CENTER) {
            if (dc->rect.width > /*file.width*/w) {
                x += (dc->rect.width / 2 - /*file.width*/w / 2);
            }
        } else if (dc->align == UI_ALIGN_RIGHT) {
            x += (dc->rect.width - /*file.width*/w);
        }

        if (/*file.width*/w > dc->rect.width) {
            text->displen = /*file.width*/w - dc->rect.width;
            if (!(text->flags & FONT_HIGHLIGHT_SCROLL)) {
                text->flags |= FONT_SHOW_SCROLL;
            }
            x -= text->offset;
        }

        if (dc->draw_img.en) {
            x = dc->draw_img.x;
            y = dc->draw_img.y;
        }

        u8 elm_index = 2;
        if (dc->draw_img.en) {
            elm_index = dc->elm_index++;
        }

        int start_x = x;
        for (i = 0; i < strpic_num; i++) {
            if (strpic_mode == 1) {
                id = pstr[1 + i];
                if (open_string_pic2(dc->prj, &file, id)) {
                    printf("err_pstr\n");
                    br28_free(str_index_buf);
                    return 0;
                }
                if (!file.width || !file.height) {
                    /* br28_free(str_index_buf); */
                    /* return 0; */
                    /* printf("line_break\n"); */
                    int nline = 1;
                    for (j = 0; j < strpic_num; j++) {
                        if (id == pstr[1 + j]) {
                            nline++;
                        }
                    }

                    x = start_x;
                    y += h / nline;
                    continue;
                }
            }

            p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);


            imb_tsk_info.rect.left = x - dc->rect_ref.left;
            imb_tsk_info.rect.top = y - dc->rect_ref.top;
            imb_tsk_info.rect.width = file.width;
            imb_tsk_info.rect.height = file.height;

            struct rect draw;
            draw.left = dc->draw.left - dc->rect_ref.left;
            draw.top = dc->draw.top - dc->rect_ref.top;
            draw.width = dc->draw.width;
            draw.height = dc->draw.height;

            if (!get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.crop)) {
                if (p) {
                    imb_task_enable(p, false);
                    /* printf("\ninh0\n\n"); */
                    if (strpic_mode == 0) {
                        br28_free(str_index_buf);
                        return -EFAULT;
                    } else if (strpic_mode == 1) {
                        goto __one_strpic_end;
                    }
                } else {
                    /* printf("\ninh1\n\n"); */
                    imb_tsk_info.task_invisible = true;
                }
            }

            memcpy(&imb_tsk_info.draw, &draw, sizeof(struct rect));

            imb_create_strpic();

__one_strpic_end:
            if (strpic_mode == 1) {
                elm_index++;
                x += file.width;
            }
        }

        if (strpic_mode == 1) {
            br28_free(str_index_buf);
            imb_task_delete_invalid(imb_task_head_get(dc->index), dc->elm->id, elm_index);
        }

    } else if (text->format && !strcmp(text->format, "mulstr")) {
        u16 id = (u8)text->str[0];
        int w;
        int h;
        u16 *str_p = text->str;
        int num = 0;
        int strpic_num = 0;

        /* printf("mulstr: %x\n", dc->elm->id); */
        //0：图片 1：索引 2：编码
        int strpic_mode = read_string_type(dc->prj);
        /* printf("strpic_mode %d\n", strpic_mode); */

        if (strpic_mode == 0) {
            if (get_multi_string_width(dc, text->str, &w, &h)) {
                return -EINVAL;
            }
        } else if (strpic_mode == 1) {
            if (get_multi_strs_width(dc, text->str, &w, &h, &num)) {
                return -EINVAL;
            }
        } else {
            u16 *p = (u16 *)text->str;
            int offset = 0;

            strpic_idbuf = text->str;
            while (*p != 0) {
                if (open_string_pic(dc->prj, &file, *p)) {
                    return 0;
                }
                strpic_tbuf = (u8 *)br28_malloc(file.len);
                if (!strpic_tbuf) {
                    return 0;
                }
                read_str_data(&file, strpic_tbuf, file.len);

                ASSERT(((offset + file.len - 2) < sizeof(mulstr_tbuf)), "offset %d, flen %d\n", offset, file.len);

                memcpy(mulstr_tbuf + offset, &strpic_tbuf[2], file.len - 2);
                br28_free(strpic_tbuf);
                strpic_tbuf = NULL;

                offset += file.len - 2;
                p++;
            }

            text->str = mulstr_tbuf;
            text->strlen = offset;
            text->encode = FONT_ENCODE_UNICODE;
            text->flags = FONT_SHOW_PIXEL;
            text->endian = FONT_ENDIAN_SMALL;

            org_lan = font_lang_get();
            if ((org_lan == Arabic) || (org_lan == Hebrew)) {
                font_lang_set(UnicodeMixLeftword);
            } else {
                font_lang_set(UnicodeMixRightword);
            }
            org_lan = font_lang_get();
            strpic_flag = 1;
            goto __strpic_with_text;
        }

        /* printf("total_w %d, total_h %d, dc_w %d\n", w, h, dc->rect.width); */


        y += (dc->rect.height / 2 - h / 2);
        if (dc->align == UI_ALIGN_CENTER) {
            x += (dc->rect.width / 2 - w / 2);
        } else if (dc->align == UI_ALIGN_RIGHT) {
            x += (dc->rect.width - w);
        }

        if (w > dc->rect.width) {
            text->displen = w - dc->rect.width;
            if (!(text->flags & FONT_HIGHLIGHT_SCROLL)) {
                text->flags |= FONT_SHOW_SCROLL;
            }
            x -= text->offset;
        }

        u8 *str_index_buf = NULL;
        u16 *pstr = NULL;
        struct rect draw;
        int i, j;

        int start_x = x;
        u8 elm_index = 2;
        str_p = text->str;
        while (*str_p != 0) {
            if (strpic_mode == 1) {
                if (num-- <= 0) {
                    break;
                }
            }

            id = *str_p;
            if ((id == 0xffff) || (id == 0)) {
                return 0;
            }

            if (open_string_pic(dc->prj, &file, id)) {
                return 0;
            }
            if (!file.len) {
                return 0;
            }

            if (strpic_mode == 0) {
                strpic_num = 1;
            } else if (strpic_mode == 1) {
                str_index_buf = (u8 *)br28_malloc(file.len);
                if (!str_index_buf) {
                    return 0;
                }
                read_str_data(&file, str_index_buf, file.len);
                pstr = (u16 *)str_index_buf;
                strpic_num = pstr[0];
            }

            for (i = 0; i < strpic_num; i++) {
                if (strpic_mode == 1) {
                    id = pstr[1 + i];
                    if (open_string_pic2(dc->prj, &file, id)) {
                        printf("err_open_mulstr2\n");
                        br28_free(str_index_buf);
                        return 0;
                    }
                    if (!file.width || !file.height) {
                        /* br28_free(str_index_buf); */
                        /* return 0; */
                        /* printf("line_break\n"); */
                        int nline = 1;
                        for (j = 0; j < strpic_num; j++) {
                            if (id == pstr[1 + j]) {
                                nline++;
                            }
                        }

                        x = start_x;
                        y += h / nline;
                        continue;
                    }
                }

                p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);

                imb_tsk_info.rect.left = x - dc->rect_ref.left;
                imb_tsk_info.rect.top = y - dc->rect_ref.top;
                imb_tsk_info.rect.width = file.width;
                imb_tsk_info.rect.height = file.height;

                draw.left = dc->draw.left - dc->rect_ref.left;
                draw.top = dc->draw.top - dc->rect_ref.top;
                draw.width = dc->draw.width;
                draw.height = dc->draw.height;

                if (!get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.crop)) {
                    if (p) {
                        imb_task_enable(p, false);
                        /* printf("\nmul inh0\n\n"); */
                        if (strpic_mode == 0) {
                            return -EFAULT;
                        } else if (strpic_mode == 1) {
                            goto __mul_strpic_end;
                        }
                    } else {
                        /* printf("\nmul inh1\n\n"); */
                        imb_tsk_info.task_invisible = true;
                    }
                }

                memcpy(&imb_tsk_info.draw, &draw, sizeof(struct rect));

                imb_create_strpic();

__mul_strpic_end:
                x += file.width;
                elm_index++;
            }
            if (strpic_mode == 1) {
                br28_free(str_index_buf);
            }
            str_p++;
        }
        imb_task_delete_invalid(imb_task_head_get(dc->index), dc->elm->id, elm_index);
    } else if (text->format && !strcmp(text->format, "image")) {
        u16 id = ((u8)text->str[1] << 8) | (u8)text->str[0];
        u16 cnt = 0;
        int image_task_cnt = 0;
        u32 w, h;
        int ww, hh;

        if (image_str_size_check(dc, dc->page, text->str, &ww, &hh) != 0) {
            return -EFAULT;
        }
        if (dc->align == UI_ALIGN_CENTER) {
            /* if (dc->rect.width > ww) { */
            x += (dc->rect.width / 2 - ww / 2);
            /* } */
        } else if (dc->align == UI_ALIGN_RIGHT) {
            x += (dc->rect.width - ww);
        }
        y += (dc->rect.height / 2 - hh / 2);

        id = ((u8)text->str[1] << 8) | (u8)text->str[0];
        cnt = 0;


        u8 elm_index = 2;
        while ((id != 0x00ff) && (id != 0xffff)) {
            if (open_image_by_id(dc->prj, NULL, &file, id, dc->page) != 0) {
                return -EFAULT;
            }
            p = imb_task_search_by_id(imb_task_head_get(dc->index), dc->elm->id, elm_index);

            u16 new_width, new_height;
            new_width = file.width;
            new_height = file.height;

            imb_tsk_info.rect.left = x - dc->rect_ref.left;
            imb_tsk_info.rect.top = y - dc->rect_ref.top;
            imb_tsk_info.rect.width = new_width;
            imb_tsk_info.rect.height = new_height;

            struct rect draw;
            draw.left = dc->draw.left - dc->rect_ref.left;
            draw.top = dc->draw.top - dc->rect_ref.top;
            draw.width = dc->draw.width;
            draw.height = dc->draw.height;

            if (!get_rect_cover(&imb_tsk_info.rect, &draw, &imb_tsk_info.draw)) {
                if (p) {
                    imb_task_enable(p, false);
                } else {
                    imb_tsk_info.task_invisible = true;
                }
            } else {
                imb_tsk_info.task_invisible = false;
            }

            memcpy(&imb_tsk_info.crop, &imb_tsk_info.draw, sizeof(struct rect));

            int page = dc->page;
            imb_create_image(ui_get_image_file_info_by_pj_id(dc->prj));

            x += (file.width + text->x_interval);
            cnt += 2;
            id = ((u8)text->str[cnt + 1] << 8) | (u8)text->str[cnt];
            elm_index++;
        }

        imb_task_delete_invalid(imb_task_head_get(dc->index), dc->elm->id, elm_index);
    }
    return 0;
}



#include "ui_draw/ui_circle.h"
void ui_draw_circle(struct draw_context *dc, int center_x, int center_y,
                    int radius_small, int radius_big, int angle_begin,
                    int angle_end, int color, int percent)
{
    struct circle_info info;
    info.center_x = center_x;
    info.center_y = center_y;
    info.radius_big = radius_big;
    info.radius_small = radius_small;
    info.angle_begin = angle_begin;
    info.angle_end = angle_end;
    info.left = dc->draw.left;
    info.top = dc->draw.top;
    info.width = dc->draw.width;
    info.height = dc->draw.height;

    info.x = 0;
    info.y = 0;
    info.disp_x = dc->disp.left;
    info.disp_y = dc->disp.top;

    info.color = color;
    info.bitmap_width = dc->disp.width;
    info.bitmap_height = dc->disp.height;
    info.bitmap_pitch = dc->disp.width * 2;
    info.bitmap = dc->buf;
    info.bitmap_size = dc->len;
    info.bitmap_depth = 16;

    draw_circle_by_percent(&info, percent);
}

AT_UI_RAM
u32 br28_read_point(struct draw_context *dc, u16 x, u16 y)
{
    u32 pixel;
    u16 *pdisp = dc->buf;
    if (dc->data_format == DC_DATA_FORMAT_OSD16) {
        int offset = (y - dc->disp.top) * dc->disp.width + (x - dc->disp.left);
        ASSERT((offset * 2 + 1) < dc->len, "dc->len:%d", dc->len);
        if ((offset * 2 + 1) >= dc->len) {
            return -1;
        }

        pixel = pdisp[offset];//(dc->buf[offset * 2] << 8) | dc->buf[offset * 2 + 1];
    } else {
        ASSERT(0);
    }

    return pixel;
}


__attribute__((always_inline_when_const_args))
AT_UI_RAM
int br28_draw_point(struct draw_context *dc, u16 x, u16 y, u32 pixel)
{
    if (dc->data_format == DC_DATA_FORMAT_OSD16) {
        int offset = (y - dc->disp.top) * dc->disp.width + (x - dc->disp.left);

        /* ASSERT((offset * 2 + 1) < dc->len, "dc->len:%d", dc->len); */
        if ((offset * 2 + 1) >= dc->len) {
            return -1;
        }

        dc->buf[offset * 2    ] = pixel >> 8;
        dc->buf[offset * 2 + 1] = pixel;
    } else if (dc->data_format == DC_DATA_FORMAT_MONO) {
        /* ASSERT(x < __this->info.width); */
        /* ASSERT(y < __this->info.height); */
        if ((x >= __this->info.width) || (y >= __this->info.height)) {
            return -1;
        }

        if (pixel & BIT(31)) {
            dc->buf[y / 8 * __this->info.width + x] ^= BIT(y % 8);
        } else if (pixel == 0x55aa) {
            dc->buf[y / 8 * __this->info.width + x] &= ~BIT(y % 8);
        } else if (pixel) {
            dc->buf[y / 8 * __this->info.width + x] |= BIT(y % 8);
        } else {
            dc->buf[y / 8 * __this->info.width + x] &= ~BIT(y % 8);
        }
    }

    return 0;
}

//清除自定义绘图任务
void ui_custom_draw_clear(struct draw_context *dc)
{
    imb_task_delete_invalid(imb_task_head_get(dc->index), dc->elm->id, 0x20);
}

int ui_draw_image(struct draw_context *dc, int page, int id, int x, int y)
{
    dc->draw_img.en = true;
    dc->draw_img.x = x;
    dc->draw_img.y = y;
    dc->draw_img.id = id;
    dc->draw_img.page = page;
    dc->draw_img.rect.left = 0;
    dc->draw_img.rect.top = 0;
    dc->draw_img.rect.width = INT_MAX;
    dc->draw_img.rect.height = INT_MAX;

    return br28_draw_image(dc, 0, 0, NULL);
}


int ui_draw_image_large(struct draw_context *dc, int page, int id, int x, int y, int width, int height, int image_x, int image_y)
{
    struct rect image_r;
    struct rect r;
    image_r.left = x;
    image_r.top = y;
    image_r.width = width;
    image_r.height = height;
    if (get_rect_cover(&dc->rect, &image_r, &r)) {
        dc->draw_img.en = true;
        dc->draw_img.x = r.left;
        dc->draw_img.y = r.top;
        dc->draw_img.id = id;
        dc->draw_img.page = page;
        ASSERT(image_x < 512);
        ASSERT(image_y < 512);
        dc->draw_img.rect.left = image_x;
        dc->draw_img.rect.top = image_y;
        dc->draw_img.rect.width = r.width;
        dc->draw_img.rect.height = r.height;
    } else {
        return -1;
    }

    return br28_draw_image(dc, 0, 0, NULL);
}


int ui_draw_ascii(struct draw_context *dc, char *str, int strlen, int x, int y, int color)
{
    static struct ui_text_attrs text = {0};
    text.str = str;
    text.format = "ascii";
    text.color = color;
    text.strlen = strlen;
    text.flags = FONT_DEFAULT;

    dc->draw_img.en = true;
    dc->draw_img.x = x;
    dc->draw_img.y = y;

    return br28_show_text(dc, &text);
}

int ui_draw_text(struct draw_context *dc, int encode, int endian, char *str, int strlen, int x, int y, int color)
{
    static struct ui_text_attrs text = {0};
    text.str = str;
    text.format = "text";
    text.color = color;
    text.strlen = strlen;
    text.encode = encode;
    text.endian = endian;
    text.flags = FONT_DEFAULT;

    dc->draw_img.en = true;
    dc->draw_img.x = x;
    dc->draw_img.y = y;

    return br28_show_text(dc, &text);
}

int ui_draw_strpic(struct draw_context *dc, int id, int x, int y, int color)
{
    static struct ui_text_attrs text = {0};
    static u16 strbuf ALIGNED(32);

    strbuf = id;
    text.str = &strbuf;
    text.format = "strpic";
    text.color = color;
    text.strlen = 0;
    text.encode = 0;
    text.endian = 0;
    text.flags = 0;

    dc->draw_img.en = true;
    dc->draw_img.x = x;
    dc->draw_img.y = y;

    return br28_show_text(dc, &text);
}

u32 ui_draw_get_pixel(struct draw_context *dc, int x, int y)
{
    return 0;
}

u16 ui_draw_get_mixed_pixel(u16 backcolor, u16 forecolor, u8 alpha)
{
    return get_mixed_pixel(backcolor, forecolor, alpha);
}

static const struct ui_platform_api br28_platform_api = {
    .malloc             = br28_malloc,
    .free               = br28_free,

    .load_style         = br28_load_style,
    .load_window        = br28_load_window,
    .unload_window      = br28_unload_window,

    .open_draw_context  = br28_open_draw_context,
    .get_draw_context   = br28_get_draw_context,
    .put_draw_context   = br28_put_draw_context,
    .close_draw_context = br28_close_draw_context,
    .set_draw_context   = br28_set_draw_context,

    .show_element       = br28_show_element,
    .hide_element       = br28_hide_element,
    .delete_element     = br28_delete_element,
    .set_element_prior  = br28_set_element_prior,

    .fill_rect          = br28_fill_rect,
    .draw_image         = br28_draw_image,
    .show_text          = br28_show_text,

    .draw_rect          = br28_draw_rect,
    .read_point         = br28_read_point,
    .draw_point         = br28_draw_point,
    .invert_rect        = br28_invert_rect,

    .read_image_info    = br28_read_image_info,

    .set_timer          = br28_set_timer,
    .del_timer          = br28_del_timer,
};


static int open_resource_file()
{
    int ret;

    printf("open_resouece_file...\n");

    ret = open_resfile(RES_PATH"JL/JL.res");
    if (ret) {
        return -EINVAL;
    }
    ret = open_str_file(RES_PATH"JL/JL.str");
    if (ret) {
        return -EINVAL;
    }
    ret = font_ascii_init(FONT_PATH"ascii.res");
    if (ret) {
        return -EINVAL;
    }
    return 0;
}

int __attribute__((weak)) lcd_get_scrennifo(struct fb_var_screeninfo *info)
{
    info->s_xoffset = 0;
    info->s_yoffset = 0;
    info->s_xres = 240;
    info->s_yres = 240;

    return 0;
}

int ui_platform_init(void *lcd)
{
    struct rect rect;
    struct lcd_info info = {0};

    /* #ifdef UI_BUF_CALC */
    /* INIT_LIST_HEAD(&buffer_used.list); */
    /* #endif */

    __this->api = &br28_platform_api;
    ASSERT(__this->api->open_draw_context);
    ASSERT(__this->api->get_draw_context);
    ASSERT(__this->api->put_draw_context);
    ASSERT(__this->api->set_draw_context);
    ASSERT(__this->api->close_draw_context);


    __this->lcd = lcd_get_hdl();
    ASSERT(__this->lcd);
    ASSERT(__this->lcd->init);
    ASSERT(__this->lcd->get_screen_info);
    ASSERT(__this->lcd->buffer_malloc);
    ASSERT(__this->lcd->buffer_free);
    ASSERT(__this->lcd->draw);
    ASSERT(__this->lcd->set_draw_area);
    ASSERT(__this->lcd->backlight_ctrl);

    /* if (__this->lcd->power_ctrl) { */
    /*     __this->lcd->power_ctrl(true); */
    /*     //下面init后才有句柄，这会操作null指针，并且init里面会打开电源，故注释 */
    /* } */
    /*  */
    if (__this->lcd->init) {
        __this->lcd->init(lcd);
    }

    if (__this->lcd->clear_screen) {
        __this->lcd->clear_screen(0x000000);
    }

    if (__this->lcd->get_screen_info) {
        __this->lcd->get_screen_info(&info);
    }

    if (__this->lcd->backlight_ctrl) {
        __this->lcd->backlight_ctrl(100);
    }

    rect.left   = 0;
    rect.top    = 0;
    rect.width  = info.width;
    rect.height = info.height;

    printf("ui_platform_init :: [%d,%d,%d,%d]\n", rect.left, rect.top, rect.width, rect.height);

    ui_core_init(__this->api, &rect);

    return 0;
}



int ui_style_file_version_compare(int version)
{
    int v;
    int len;
    struct ui_file_head head;
    static u8 checked = 0;

    if (checked == 0) {
        if (!ui_file) {
            puts("ui version_compare ui_file null!\n");
            ASSERT(0);
            return 0;
        }
        res_fseek(ui_file, 0, SEEK_SET);
        len = sizeof(struct ui_file_head);
        res_fread(ui_file, &head, len);
        printf("style file version is: 0x%x,UI_VERSION is: 0x%x\n", *(u32 *)(head.res), version);
        if (*(u32 *)head.res != version) {
            puts("style file version is not the same as UI_VERSION !!\n");
            ASSERT(0);
        }
        checked = 1;
    }
    return 0;
}





int ui_watch_poweron_update_check()
{
#if (CONFIG_UI_STYLE == STYLE_JL_WTACH) || (CONFIG_UI_STYLE == STYLE_JL_WTACH_NEW)
    //如果上次表盘升级异常，直接进入表盘升级模式等待升级完成
    //加入新标志位的判断，用于升级过程中断电后开机进入升级模式的情况
    if (smartbox_eflash_flag_get() != 0 ||
        smartbox_eflash_update_flag_get()) {
        printf("\n\ngoto watch update mode\n\n");

        //上电后，发现上一次表盘升级异常,先初始化升级需要的蓝牙相关部分,
        //再跳转到升级模式

        watch_update_over = 2;
        app_smartbox_task_prepare(0, SMARTBOX_TASK_ACTION_WATCH_TRANSFER, 0);
        return -1;
    }
#endif
    return 0;
}


int ui_upgrade_file_check_valid()
{
#if UI_UPGRADE_RES_ENABLE
    //简单实现
    //假设升级界面必须存在，调用了该接口证明资源不完整
    //需要进行升级
    smartbox_eflash_flag_set(1);//这个标志的清理需要注意
    return 0;
#endif
    return -ENOENT;
}

int ui_file_check_valid()
{

#if UI_WATCH_RES_ENABLE
    int ret;
    printf("open_resouece_file...\n");
    int i = 0;
    RESFILE *file = NULL;
    char *sty_suffix = ".sty";
    char *res_suffix = ".res";
    char *str_suffix = ".str";
    char tmp_name[100];
    /* u32 list_len = sizeof(WATCH_STY_CHECK_LIST) / sizeof(WATCH_STY_CHECK_LIST[0]); */
    u32 list_len;
    u32 tmp_strlen;
    u32 suffix_len;

    // 升级成功后清除标志位
    smartbox_update_flag_clear();
    //如果上次表盘升级异常，直接进入表盘升级模式等待升级完成
    /* if (smartbox_eflash_flag_get() != 0 || */
    /*     smartbox_eflash_update_flag_get()) { */
    if (smartbox_eflash_update_flag_get() != 0 ||
        smartbox_eflash_resume_opt()) {
        printf("\n\nneed goto watch update mode\n\n");
        return -EINVAL;
    } else {
        ret = watch_set_init();
        if (ret != 0) {
            return -EINVAL;
        }
    }

    file = res_fopen(RES_PATH"JL/JL.sty", "r");
    if (!file) {
        smartbox_ex_flash_file_flag(1);
        return -ENOENT;
    }
    res_fclose(file);

    list_len = watch_items;

    for (i = 0; i < list_len; i++) {
        /* file = res_fopen(WATCH_STY_CHECK_LIST[i], "r"); */
        file = res_fopen(watch_res[i], "r");
        if (!file) {
            smartbox_ex_flash_file_flag(1);
            return -ENOENT;
        }
        res_fclose(file);
        printf("now %s\n", watch_res[i]);
    }
    printf("%s : %d", __FUNCTION__, __LINE__);

#endif

    return 0;
}

int get_cur_srreen_width_and_height(u16 *screen_width, u16 *screen_height)
{
    memcpy((u8 *)screen_width, (u8 *)&__this->info.width, sizeof(__this->info.width));
    memcpy((u8 *)screen_height, (u8 *)&__this->info.height, sizeof(__this->info.height));
    return 0;
}

int ui_relate_init()
{
    if (__this->ui_res_init) {
        return 0;
    }
    printf("ui relate init.\n");
    font_ascii_init(RES_PATH"font/ascii.res");//打开ascii.res
    ui_st77903_init();
    ui_qrcode_init();
    ui_disp_line_buf_init();
    ui_font_arabic_init();
    ui_jpeg_decoder_init();
    __this->ui_res_init = 1;
    return 0;
}

int ui_relate_uninit()
{
    if (!__this->ui_res_init) {
        return 0;
    }
    printf("ui relate uninit.\n");
    font_ascii_init(NULL); //关闭ascii.res
    text_font_init(false); //关闭字库文件
    ui_unload_file_info(); //关闭ui_load_info_table里的所有文件
    ui_st77903_uninit();
    ui_qrcode_uninit();
    ui_disp_line_buf_uninit();
    ui_font_arabic_uninit();
    ui_jpeg_decoder_uninit();
    __this->ui_res_init = 0;
    return 0;
}

#endif


