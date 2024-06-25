#include "font/font_all.h"
#include "font/language_list.h"
#include "app_config.h"
#include "ui/res_config.h"

#if TCFG_UI_ENABLE
#if TCFG_SPI_LCD_ENABLE

extern void platform_putchar(struct font_info *info, u8 *pixel, u16 width, u16 height, u16 x, u16 y);//字库输出显示函数
/* 语言字库配置 */

#define LANGUAGE    (BIT(Chinese_Simplified) \
                    | BIT(English) \
                    | BIT(Chinese_Traditional) \
                    | BIT(Unicode))

const struct font_info font_info_table[] = {

#if LANGUAGE&BIT(Chinese_Simplified)
    //gb2312
    {
        .language_id = Chinese_Simplified,   //简体中文
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_GB2312.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX",
        .tabfile.name = (char *)FONT_PATH"F_GB2312.TAB",
        /* .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX", */
        /* .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX", */
        /* .tabfile.name = (char *)FONT_PATH"F_Mix.TAB", */
        .isgb2312 = true,
        .bigendian = false,
        .putchar = platform_putchar,
    },
    {
        .language_id = Chinese_Traditional,  //繁体中文
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_BIG5.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX",
        .tabfile.name = (char *)FONT_PATH"F_BIG5.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },
    {
        .language_id = Arabic,   //阿拉伯语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        /* .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX", */
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1256.PIX",
        .tabfile.name = (char *)FONT_PATH"F_CP1256.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = Vietnam,  //越南语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1258.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mix.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },
    {
        .language_id = Tibetan,  //藏语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_GB2312.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX",
        .tabfile.name = (char *)FONT_PATH"F_GB2312.TAB",
        .extfile.name = (char *)FONT_PATH"F_TIBETA.PIX",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },
    {
        .language_id = Indic,  //印地语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_GB2312.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX",
        .tabfile.name = (char *)FONT_PATH"F_GB2312.TAB",
        .extfile.name = (char *)FONT_PATH"F_UNICO.PIX",
        .gposfile.name = (char *)FONT_PATH"gpos.bin",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = UnicodeMixRightword,  //混合
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_UNIC.PIX",
        .gposfile.name = (char *)FONT_PATH"gpos.bin",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = UnicodeMixLeftword,  //混合
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_UNIC.PIX",
        .gposfile.name = (char *)FONT_PATH"gpos.bin",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = Turkey,   //土耳其语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1254.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mix.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = Russian,  //俄罗斯语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1251.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mix.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = Hebrew,   //希伯来语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        /* .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX", */
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1255.PIX",
        .tabfile.name = (char *)FONT_PATH"F_CP1255.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },
    {
        .language_id = Thai,  //泰语
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_CP874.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mix.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },


    /* { */
    /*     .language_id = Japanese, */
    /*     .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE, */
    /*     .pixel.file.name = (char *)FONT_PATH"F_SJIS.PIX", */
    /*     .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX", */
    /*     .tabfile.name = (char *)FONT_PATH"F_SJIS.TAB", */
    /*     .isgb2312 = false, */
    /*     .bigendian = false, */
    /*     .putchar = platform_putchar, */
    /* }, */

    {
        .language_id = Polish,   //所有共用CP1250的语言都统一用Polish
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1250.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mix.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    {
        .language_id = Danish,   //所有共用CP1252的语言都统一用Danish
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mix.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_CP1252.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mix.TAB",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },
    {
        .language_id = MixAllLanguage,   //所有共用CP1252的语言都统一用Danish
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_Mixall.PIX",
        .ascpixel.file.name = (char *)FONT_PATH"F_AscMix.PIX",
        .tabfile.name = (char *)FONT_PATH"F_Mixall.TAB",
        .isgb2312 = true,
        .bigendian = false,
        .putchar = platform_putchar,
    },

    //gbk
    /* { */
    /*     .language_id = Chinese_Simplified, */
    /*     .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE, */
    /*     .pixel.file.name = (char *)"mnt/sdfile/res/F_GBK.PIX", */
    /*     .ascpixel.file.name = (char *)"mnt/sdfile/res/F_ASCII.PIX", */
    /*     .tabfile.name = (char *)"mnt/sdfile/res/F_GBK.TAB", */
    /*     .isgb2312 = false, */
    /*     .bigendian = false, */
    /*     .putchar = platform_putchar, */
    /* }, */
#endif

#if LANGUAGE&BIT(Chinese_Traditional)
    /* { */
    /*     .language_id = Chinese_Traditional, */
    /*     .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE, */
    /*     .pixel.file.name = (char *)"mnt/sdfile/res/F_GBK.PIX", */
    /*     .ascpixel.file.name = (char *)"mnt/sdfile/res/F_ASCII.PIX", */
    /*     .tabfile.name = (char *)"mnt/sdfile/res/F_GBK.TAB", */
    /*     .isgb2312 = false, */
    /*     .bigendian = false, */
    /*     .putchar = platform_putchar, */
    /* }, */
#endif

#if LANGUAGE&BIT(English)
    {
        .language_id = English,
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .ascpixel.file.name = (char *)FONT_PATH"F_ASCII.PIX",
        .isgb2312 = false,
        .bigendian = false,
        .putchar = platform_putchar,
    },
#endif

#if LANGUAGE&BIT(Unicode)
    {
        .language_id = Unicode,
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)FONT_PATH"F_UNIC.PIX",
        .bigendian = false,
        .putchar = platform_putchar,
    },
#endif

    {
        .language_id = 0,//不能删
    },

};




#endif
#endif
