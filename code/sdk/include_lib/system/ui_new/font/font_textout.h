#ifndef __FONT_OUT_H__
#define __FONT_OUT_H__

#include "generic/typedef.h"
#include "font/font_all.h"

/**
 * @brief  打开字库
 *
 * @param info:字库信息
 * @param language:语言
 *
 * @returns   TRUE:打开成功 FALSE:打开失败
 */
struct font_info *font_open(struct font_info *info, u8 language);
/**
 * @brief 获取字符宽度
 *
 * @param info
 * @param str
 * @param strlen
 *
 * @returns
 */
u16 font_text_width(struct font_info *info, u8 *str, u16 strlen);
u16 font_textw_width(struct font_info *info, u8 *str, u16 strlen);
u16 font_textu_width(struct font_info *info, u8 *str, u16 strlen);
void font_lang_set(int language);
int font_lang_get();

/**
 * @brief  字库内码显示接口
 *
 * @param info
 * @param str
 * @param strlen
 *
 * @returns
 */
u16 font_textout(struct font_info *info, u8 *str, u16 strlen, u16 x, u16 y);
/**
 * @brief  字库unicode显示接口
 *
 * @param info
 * @param str
 * @param strlen
 * @param x
 * @param y
 *
 * @returns
 */
u16 font_textout_unicode(struct font_info *info, u8 *str, u16 strlen, u16 x, u16 y);
/**
 * @brief  字库utf8显示接口
 *
 * @param info
 * @param str
 * @param strlen
 * @param x
 * @param y
 *
 * @returns
 */
u16 font_textout_utf8(struct font_info *info, u8 *str, u16 strlen, u16 x, u16 y);
/**
 * @brief  utf8转内码
 *
 * @param info
 * @param utf8
 * @param utf8len
 * @param ansi
 *
 * @returns
 */
u16 font_utf8toansi(struct font_info *info, u8 *utf8, u16 utf8len, u8 *ansi);
/**
 * @brief utf16转内码
 *
 * @param info
 * @param utf
 * @param len
 * @param ansi
 *
 * @returns
 */
u16 font_utf16toansi(struct font_info *info, u8 *utf, u16 len, u8 *ansi);
/**
 * @brief utf8转utf16
 *
 * @param info
 * @param utf8
 * @param utf8len
 * @param utf16
 *
 * @returns
 */
u16 font_utf8toutf16(struct font_info *info, u8 *utf8, u16 utf8len, u16 *utf16);
/**
 * @brief  字库关闭
 *
 * @param info
 */
void font_close(struct font_info *info);

void *font_malloc(size_t size);
void *font_zalloc(size_t size);
void font_free(void *pv);

int ui_font_arabic_init();
int ui_font_arabic_uninit();

/* 0xe00->0xe7f:Thai    Unicode编码范围 */
/* 0x900->0x97f:Indic   Unicode编码范围 */
/* 0x600->0x6ff:Arabic  Unicode编码范围 */
/* 0x590->0x5ff:Hebrew  Unicode编码范围 */
/* 0xf00->0xfff:Tibetan Unicode编码范围 */
enum unic_type {
    UNIC_ERR,
    UNIC_THAI,
    UNIC_INDIC,
    UNIC_ARABIC,
    UNIC_HEBREW,
    UNIC_TIBETAN,
    UNIC_MYANMAR,
    UNIC_OTHER
};

enum myanmar_type {
    myanmar_top,
    myanmar_below,
    myanmar_103c,
    myanmar_half,
    myanmar_follow,
    myanmar_other,
};

u16 TextW_TibetanReplace(struct font_info *info, u16 *str_buf, u16 len);
void find_syllables_indic(uint16_t *buffer, uint32_t len, uint8_t *syllable); //音节切分函数
u16 TextW_IndicReorder(struct font_info *info, u8 *str, u16 len);
u16 TextW_IndicReplace(struct font_info *info, u8 *str, u16 len);
u16 font_indic_drawbuf(struct font_info *info, u16(*buffer)[2], u16 len, int x, int y, int *offset_xy);
u16 font_thai_drawbuf(struct font_info *info, u16(*buffer)[2], u16 len, int x, int y, int *offset_xy);
u16 font_tibetan_drawbuf(struct font_info *info, u16(*buffer)[2], u16 len, int x, int y, int *offset_xy);
u16 font_left_thai_drawbuf(struct font_info *info, u16(*buffer)[2], int *buffer_data, int *offset_xy);
void find_syllables_myanmar(uint16_t *buffer, uint32_t len, uint8_t *syllable);

#endif
