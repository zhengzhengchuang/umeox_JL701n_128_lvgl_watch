/**
 * @copyright (c) 2003 - 2024, Goodix Co., Ltd. All rights reserved.
 *
 * @file    gh30x_demo_common.h
 *
 * @brief   gh30x algo common define
 *
 * @version ref gh30x_example_common.h
 *
 */

#ifndef _GH30X_DEMO_COMMON_H_
#define _GH30X_DEMO_COMMON_H_

#include "gh30x_demo_algo_config.h"
#include "../../call/inc/goodix_algo.h"

/* extern log function */
extern void GH30X_AlgoLog(GCHAR *chLogString);
/// log support len
#define GH30X_LOG_ALGO_SUP_LEN     (128)

#if (__ALGO_LOG_CONFIG__)
#define   GH30X_ALGO_LOG_PARAM(...)       do {\
                                                GCHAR chDebugStr[GH30X_LOG_ALGO_SUP_LEN] = {0};\
                                                snprintf(chDebugStr, GH30X_LOG_ALGO_SUP_LEN, \
                                                        "[gh30x_algo]: "__VA_ARGS__);\
                                                GH30X_AlgoLog(chDebugStr);\
                                             } while (0)
#else
#define   GH30X_ALGO_LOG_PARAM(...)
#endif

/// channel map max channel support to unpack rawdata
#define   GH30X_CHANNEL_MAP_MAX_CH         (32)

/// fifo rawdata bytes max len
#define   GH30X_FIFO_DATA_BYTES_MAX_LEN    (3200)    // GH30X_FIFO_CNT_CALC(800)

/// fifo rawdata size
#define   GH30X_FIFO_RAWDATA_SIZE          (4)

/// fifo rawdataindex size
#define   GH30X_FIFO_RAWDATAINDEX_SIZE     (2)

/// fifo gsensordata size
#define   GH30X_FIFO_GSENSORDATA_SIZE      (6)

/// fifo agcdata size
#define   GH30X_FIFO_AGCDATA_SIZE          (4)

/// fifo cnt calc
#define   GH30X_FIFO_CNT_CALC(x)           ((x) * GH30X_FIFO_RAWDATA_SIZE)

/// g sensor data size
#define   GH30X_GSENSOR_DATA_SIZE          (6)

/// agc data size
#define   GH30X_AGC_DATA_SIZE              (4)

/// agc data cnt calc
#define   GH30X_AGC_CNT_CALC(x)            ((x) * GH30X_AGC_DATA_SIZE)

/// algorithm result size
#define   GH30X_ALGO_RESULT_SIZE           (4)

/// algorithm result calc
#define   GH30X_ALGO_RESULT_CALC(x)        ((x) * GH30X_ALGO_RESULT_SIZE)

/// algorithm result with tag calc.
#define   GH30X_ALGO_RESULT_WITH_TAG_CALC(x) ((x) * (GH30X_ALGO_RESULT_SIZE + 1))

/// g sensor data size
#define   GH30X_GSENSOR_DATA_SIZE          (6)

/// agc data size
#define   GH30X_AGC_DATA_SIZE              (4)

/// agc data cnt calc
#define   GH30X_AGC_CNT_CALC(x)            ((x) * GH30X_AGC_DATA_SIZE)

/// algorithm result size
#define   GH30X_ALGO_RESULT_SIZE           (4)

/// algorithm result calc
#define   GH30X_ALGO_RESULT_CALC(x)        ((x) * GH30X_ALGO_RESULT_SIZE)

/// algorithm result with tag calc.
#define   GH30X_ALGO_RESULT_WITH_TAG_CALC(x)           ((x) * (GH30X_ALGO_RESULT_SIZE + 1))

/* rawdata macro */

/// rawdata clear slot & adc num
#define   GH30X_RAWDATA_CLEAR_SLOT_ADC_NUM(x)          ((x) & 0x07FFFFFF)

/// rawdata get slot num from dword
#define   GH30X_DWORD_RAWDATA_GET_SLOT_NUM(x)          (((x) & 0xE0000000) >> 29)

/// rawdata get adc num from dword
#define   GH30X_DWORD_RAWDATA_GET_ADC_NUM(x)           (((x) & 0x18000000) >> 27)

/// rawdata get slot&adc num from dword
#define   GH30X_DWORD_RAWDATA_GET_SLOT_ADC_NUM(x)      (((x) & 0xF8000000) >> 27)

/// rawdata get slot num from byte
#define   GH30X_BYTE_RAWDATA_GET_SLOT_NUM(x)           (((x) & 0xE0) >> 5)

/// rawdata get adc num from byte
#define   GH30X_BYTE_RAWDATA_GET_ADC_NUM(x)            (((x) & 0x18) >> 3)

/// slot & adc num bits of rawdata byte
#define   GH30X_SLOT_ADC_NUM_BITS                      (0xF8)

/// rawdata get slot adc num from byte
#define   GH30X_BYTE_RAWDATA_GET_SLOT_ADC_NUM(x)       (((x) & GH30X_SLOT_ADC_NUM_BITS) >> 3)

/// get slot index from channel num
#define   GH30X_GET_SLOT_FROM_CHANNEL_NUM(x)           (((x) & 0x1F) >> 2)

/// get adc index from channel num
#define   GH30X_GET_ADC_FROM_CHANNEL_NUM(x)            ((x) & 0x3)

/// rawdata clear all flag
#define   GH30X_RAWDATA_CLEAR_ALL_FLAG(x)              ((x) & 0x00FFFFFF)

/// rawdata clear data
#define   GH30X_RAWDATA_CLEAR_DATA(x)                  ((x) & 0xFF000000)

/// rawdata get adj flag0
#define   GH30X_RAWDATA_GET_ADJ_FLAG0(x)               (((x) & 0x4000000) >> 26)

/// rawdata get adj flag1
#define   GH30X_RAWDATA_GET_ADJ_FLAG1(x)               (((x) & 0x2000000) >> 25)

/// get rawdata for algorithm
#define   GH30X_GET_RAWDATA_FOR_ALGO(x)                ((x) >> 7)

/// rawdata get fast recovery flag
#define   GH30X_RAWDATA_GET_FAST_RECOVERY_FLAG(x)      (((x) & 0x4000000) >> 26)

/// rawdata real offset
#define   GH30X_RAWDATA_REAL_OFFSET                    (0x00800000)

/* utils macro */

/// byte true flag val
#define   GH30X_BYTE_FLAG_TRUE                     (1)

/// byte false flag val
#define   GH30X_BYTE_FLAG_FALSE                    (0)

/// GF32 0 val
#define   GH30X_GF32_0                             (0.0f)

/// null val
#define   GH30X_PTR_NULL                           ((void *) 0)

/// get high 4bits from byte
#define   GH30X_GET_HIGH_4BITS(val)                ((val) & 0xF0)

/// get low 4bits from byte
#define   GH30X_GET_LOW_4BITS(val)                 ((val) & 0x0F)

/// get first 4bits from byte
#define   GH30X_GET_FIRST_4BITS(val)               ((val) & 0x0000000FU)

/// get high byte from word
#define   GH30X_GET_HIGH_BYTE_FROM_WORD(sValue)    ((GU8)(((sValue) >> 8) & 0xFFU))

/// get low byte from word
#define   GH30X_GET_LOW_BYTE_FROM_WORD(sValue)     ((GU8)((sValue) & 0xFFU))

/// get byte3 from dword
#define   GH30X_GET_BYTE3_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 24) & 0x000000FFU))

/// get byte2 from dword
#define   GH30X_GET_BYTE2_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 16) & 0x000000FFU))

/// get byte1 from dword
#define   GH30X_GET_BYTE1_FROM_DWORD(unValue)      ((GU8)(((unValue) >> 8) & 0x000000FFU))

/// get byte0 from dword
#define   GH30X_GET_BYTE0_FROM_DWORD(unValue)      ((GU8)((unValue) & 0x000000FFU))

/// get high word from dword
#define   GH30X_GET_HIGH_WORD_FROM_DWORD(unValue)  ((GU16)(((unValue) >> 16) & 0x0000FFFFU))

/// get low word from dword
#define   GH30X_GET_LOW_WORD_FROM_DWORD(unValue)   ((GU16)((unValue) & 0x0000FFFFU))

/// makeup word from bytes
#define   GH30X_MAKEUP_WORD(uchHighByte, uchLowByte)              ((GU16)(((((GU16)(uchHighByte)) << 8)& 0xFF00) |\
                                                                            (((GU16)(uchLowByte))& 0x00FF)))

/// makeup dword from bytes
#define   GH30X_MAKEUP_DWORD(uchByte3, uchByte2, uchByte1, uchByte0)     (((((GU32)(uchByte3)) << 24) & 0xFF000000U)|\
                                                                          ((((GU32)(uchByte2)) << 16) & 0x00FF0000U) |\
                                                                          ((((GU32)(uchByte1)) << 8) & 0x0000FF00U) |\
                                                                          (((GU32)(uchByte0)) & 0x000000FFU))

/// makeup dword from words
#define   GH30X_MAKEUP_DWORD2(usHighWord, usLowWord)                  (((((GU32)(usHighWord)) << 16) & 0xFFFF0000U) |\
                                                                            (((GU32)(usLowWord)) & 0x0000FFFFU))

/// macro of hook function call
/* NOLINTNEXTLINE(501) */

/// macro of handle function call
/* NOLINTNEXTLINE(501) */
#define   HANDLE_FUNC_CALL(func, argv)     do { if ((func) != GH30X_PTR_NULL) { func argv; } } while (0)

/// macro of set val via ptr
#define   SET_VAL_VIA_PTR(ptr, val)        do { if ((ptr) != GH30X_PTR_NULL) { (*(ptr)) = (val); } } while (0)

/* bit operation macro */

/// macro of set bits
#define   GH30X_SET_BIT(x, b)              ((x) | (b))

/// macro of val set bits
#define   GH30X_VAL_SET_BIT(x, b)          ((x) |= (b))

/// macro of clear bits
#define   GH30X_CLEAR_BIT(x, b)            ((x) & (~(b)))

/// macro of val clear bits
#define   GH30X_VAL_CLEAR_BIT(x, b)        ((x) &= (~(b)))

/// macro of val get bits
#define   GH30X_VAL_GET_BIT(x, b)          ((x) &= (b))

/// macro of check bits set
#define   GH30X_CHECK_BIT_SET(x, b)        (((x) & (b)) == (b))

/// macro of check bits not set
#define   GH30X_CHECK_BIT_NOT_SET(x, b)    (((x) & (b)) != (b))

/// macro of check bits is set
#define   GH30X_CHECK_BIT_IS_SET(x, b)     (((x) & (b)) == (b))

/// macro of get bits is set
#define   GH30X_GET_BIT_IS_SET(x)          ((x) & 0xf000)

/// macro of bit number in one byte
#define   GH30X_BIT_PER_BYTE               (4)

/// macro of bit in reg addr for check
#define   GH30X_REG_MODE_BIT               (3)

/// macro of check bits zero
#define   GH30X_CHECK_REG_MODE_BIT_ZERO(x) (((x) >> (GH30X_BIT_PER_BYTE * GH30X_REG_MODE_BIT)) == (0))

/// macro of check lsb set
#define   GH30X_CHECK_LSB_SET(x)           (GH30X_CHECK_BIT_SET((x), 0x01))

/// macro of check left shift bit set
#define   GH30X_CHECK_LEFT_BIT_SET(x, l)   (GH30X_CHECK_BIT_SET((x), (1u << (l))))

/// macro of check left shift bit not set
#define   GH30X_CHECK_LEFT_BIT_NOT_SET(x, l) (GH30X_CHECK_BIT_NOT_SET((x), (1u << (l))))

/// macro of val clear left shift bit
#define   GH30X_VAL_CLEAR_LEFT_BIT(x, l)   ((x) &= (~((1u << (l)))))

/// check ret error
#define   GH30X_RET_ERROR_CHECK(x)             do { \
                                                    if ((x) != GH30X_RET_OK)\
                                                    {\
                                                        return (x);\
                                                    }\
                                                } while (0)

/// check ret the error
#define   GH30X_RET_ERROR_CHECK_E(x, e)        do { \
                                                    if ((x) == (e))\
                                                    {\
                                                        return (x);\
                                                    }\
                                                } while (0)

/// check ret the error
#define   GH30X_RET_ERROR_CHECK_ER(x, e, r)    do { \
                                                    if ((x) == (e))\
                                                    {\
                                                        return (r);\
                                                    }\
                                                } while (0)

/// val rigth shfit
#define   GH30X_SET_VAL_RIGTH_SHIFT(x, r)      ((x) >>= (r))

/// val rigth shfit 1bit
#define   GH30X_SET_VAL_RIGTH_SHIFT_1BIT(x)    (GH30X_SET_VAL_RIGTH_SHIFT((x), 1u))

/// val left shfit
#define   GH30X_SET_VAL_LEFT_SHIFT(x, l)       ((x) <<= (l))

/// val left shfit 1bit
#define   GH30X_SET_VAL_LEFT_SHIFT_1BIT(x)     (GH30X_SET_VAL_LEFT_SHIFT((x), 1u))

/// get left shfit val
#define   GH30X_GET_LEFT_SHIFT_VAL(l)          (1u << (l))

/// val left shfit
#define   GH30X_VAL_LEFT_SHIFT(x, l)           ((x) << (l))

/// val cal diff value
#define   GH30X_VAL_DIFF_VALUE(a, b)           ((a) > (b) ? ((a) - (b)) : ((b) - (a)))

/// val cal diff status
#define   GH30X_VAL_DIFF_STA(a, b)             ((a) > (b) ? (0) : (1))

/// val cal diff ABS
#define   GH30X_VAL_ABS(x)                     ((x) >= 0 ? (x) : - (x))

/// val zip rawdata send cmd EVEN or ODD
#define   GH30X_VAL_ZIPSENDCMD(x)              ((x) == 1 ? (GH30X_UPROTOCOL_CMD_ZIPEVENRAWDATA) \
                                                 : (GH30X_UPROTOCOL_CMD_ZIPODDRAWDATA))

#endif /* _GH30X_DRV_COMMON_H_ */

/********END OF FILE********* Copyright (c) 2003 - 2024, Goodix Co., Ltd. ********/
