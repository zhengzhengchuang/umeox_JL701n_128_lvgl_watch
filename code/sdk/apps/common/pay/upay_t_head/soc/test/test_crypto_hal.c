#undef _DEBUG_H_
#define LOG_TAG_CONST       T_CRY_HAL          //修改成文件名
#define LOG_TAG             "[T_CRY_HAL]"
#include "debug.h"
#define LOG_v(t)  log_tag_const_v_ ## t
#define LOG_i(t)  log_tag_const_i_ ## t
#define LOG_d(t)  log_tag_const_d_ ## t
#define LOG_w(t)  log_tag_const_w_ ## t
#define LOG_e(t)  log_tag_const_e_ ## t
#define LOG_c(t)  log_tag_const_c_ ## t
#define LOG_tag(tag, n) n(tag)
const char LOG_tag(LOG_TAG_CONST, LOG_v) AT(.LOG_TAG_CONST) = 0;
const char LOG_tag(LOG_TAG_CONST, LOG_i) AT(.LOG_TAG_CONST) = 1; //log_info
const char LOG_tag(LOG_TAG_CONST, LOG_d) AT(.LOG_TAG_CONST) = 1; //log_debug
const char LOG_tag(LOG_TAG_CONST, LOG_w) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_e) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_c) AT(.LOG_TAG_CONST) = 1;

#include <stdio.h>
#include <string.h>
#include "alipay_iot.h"
#include "../port/iotsec.h"
#include "../softse/sha256.h"
#include "../softse/aes.h"
#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)

//#include "include.h"
//#include "yc_debug.h"
void test_case_trng(void);
int test_case_aes(void);
int test_case_sha(void);

typedef struct _TRNG_TEST_DATA {
    uint8_t rand_num[6];
} TRNG_TEST_DATA;

TRNG_TEST_DATA gRandNum[5];
void trng_test_init(void)
{
    memset(gRandNum, 0x00, sizeof(gRandNum));
}

void test_case_trng(void)
{
    int i = 0;
    log_debug("================trng-test-start================\n");
    trng_test_init();
    for (i = 0; i < 5; i++) {
        hal_rng_get_data((void *)(&gRandNum[i]), 6);
        log_debug("cycle %d csi rand_num: %4d %4d %4d %4d %4d %4d\n", i,
                  gRandNum[i].rand_num[0],
                  gRandNum[i].rand_num[1],
                  gRandNum[i].rand_num[2],
                  gRandNum[i].rand_num[3],
                  gRandNum[i].rand_num[4],
                  gRandNum[i].rand_num[5]);
    }

    trng_test_init();
    for (i = 0; i < 5; i++) {
        hal_rng_get_data((void *)(&gRandNum[i]), 6);
        log_debug("cycle %d hal rand_num: %4d %4d %4d %4d %4d %4d\n", i,
                  gRandNum[i].rand_num[0],
                  gRandNum[i].rand_num[1],
                  gRandNum[i].rand_num[2],
                  gRandNum[i].rand_num[3],
                  gRandNum[i].rand_num[4],
                  gRandNum[i].rand_num[5]);
    }
    log_debug("================trng-test-over================\n\n");
}

#if 1

static uint8_t out_result[128] __attribute__((aligned(64)));
/********************aes-256-64*******************/
/*
static uint8_t aes256_cbc_key[] __attribute__((aligned(64))) = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C,
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C,
};
static uint8_t aes256_cbc_64_in[] __attribute__((aligned(64))) = {
    0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
    0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
    0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C,
    0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
    0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11,
    0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF,
    0xF6, 0x9F, 0x24, 0x45, 0xDF, 0x4F, 0x9B, 0x17,
    0xAD, 0x2B, 0x41, 0x7B, 0xE6, 0x6C, 0x37, 0x10,
};
static uint8_t aes256_cbc_64_iv[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
static uint8_t aes256_cbc_64_iv2[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
static uint8_t aes256_cbc_64_expect[]  __attribute__((aligned(64))) = {
    0xE7, 0x38, 0x23, 0x9C, 0x4A, 0x27, 0xFB, 0x76,
    0xA7, 0xF1, 0x64, 0x57, 0x81, 0xAE, 0x7C, 0xB0,
    0x0C, 0x2B, 0x7B, 0xD2, 0x31, 0xB0, 0xD0, 0xBA,
    0xAE, 0x61, 0xE4, 0x34, 0xFC, 0x08, 0x41, 0x58,
    0x46, 0xCB, 0x1C, 0xAC, 0x7C, 0xE4, 0x04, 0xBB,
    0x74, 0x44, 0x55, 0x80, 0x6A, 0x74, 0x7F, 0xF6,
    0xB2, 0x40, 0x0F, 0x5B, 0x54, 0x79, 0x57, 0x52,
    0x33, 0xBC, 0xD0, 0xE2, 0xBE, 0xDA, 0xDE, 0x97,
};*/


/********************cbc-192-16*******************/
/*
static uint8_t aes192_cbc_key[] __attribute__((aligned(64))) = {
    0x3D,0xD3,0xBC,0xEF,0xD1,0xB6,0x8F,0x82,
    0xE8,0x53,0xD4,0x7F,0xDD,0xED,0xB4,0xED,
    0x39,0xF0,0x1D,0x51,0x9B,0xDC,0x5A,0xF7,
};
static uint8_t aes192_cbc_16_in[] __attribute__((aligned(64))) = {
    0x50,0x04,0x64,0x91,0x5D,0x48,0x04,0x65,
    0x2B,0xBC,0x93,0x73,0x27,0xA8,0xDD,0x8A,
};
static uint8_t aes192_cbc_16_iv[] = {
    0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
};
static uint8_t aes192_cbc_16_iv2[] = {
    0x01,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
    0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
};
static uint8_t aes192_cbc_16_expect[]  __attribute__((aligned(64))) = {
    0x6B,0x0D,0xBA,0x49,0x65,0x94,0x9A,0x4A,
    0xA5,0xFE,0x8A,0xD9,0xD9,0xE4,0x59,0xE5,
};
*/

/********************cbc-128-64*******************/
const static uint8_t aes128_cbc_key[] __attribute__((aligned(64))) = {
    0x1E, 0x02, 0x96, 0x1C, 0x2E, 0x8F, 0x2D, 0x27,
    0xAE, 0x55, 0xCF, 0x43, 0xE5, 0x2C, 0xE8, 0x62,
};
const static uint8_t aes128_cbc_64_in[] __attribute__((aligned(64))) = {
    0x13, 0x44, 0x83, 0x28, 0x9D, 0x11, 0x1E, 0x34,
    0x65, 0xDF, 0x9C, 0x51, 0x05, 0x4B, 0x05, 0x12,
    0xCE, 0xB9, 0x9F, 0x05, 0x75, 0x9E, 0xAD, 0x92,
    0xFA, 0x1C, 0x25, 0x5C, 0x5B, 0x83, 0x16, 0x98,
    0xCC, 0x0F, 0x7E, 0x3D, 0x04, 0xEA, 0xD9, 0x9C,
    0x69, 0xAA, 0x60, 0x4E, 0xD1, 0x0F, 0x96, 0x94,
    0x74, 0x1B, 0xC5, 0x07, 0xF7, 0xBA, 0x6A, 0x5B,
    0x67, 0x7E, 0x2A, 0x11, 0xD1, 0x78, 0xBC, 0x29,
};
static uint8_t aes128_cbc_64_iv[] = {
    0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
static uint8_t aes128_cbc_64_iv2[] = {
    0x01, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
};
const static uint8_t aes128_cbc_64_expect[]  __attribute__((aligned(64))) = {
    0xC2, 0xB6, 0x0E, 0x1F, 0xE2, 0xF2, 0x48, 0x90,
    0x3E, 0xE7, 0xBE, 0x7C, 0x2D, 0x3C, 0x1B, 0x10,
    0xB4, 0x7A, 0x0D, 0x88, 0xD5, 0x18, 0xE6, 0xAE,
    0x04, 0x7E, 0x9D, 0xA7, 0x88, 0xCA, 0xFE, 0x0D,
    0x2F, 0x65, 0xEB, 0x1E, 0xAB, 0x8F, 0x71, 0x00,
    0x7B, 0x89, 0x13, 0x0A, 0x41, 0x8B, 0xD8, 0x4F,
    0x5E, 0x07, 0x59, 0x78, 0x2B, 0x07, 0x13, 0x9F,
    0xAB, 0xB7, 0xFE, 0x10, 0x0B, 0x9B, 0x92, 0xB8
};

/********************ecb-256-16*******************/
/*
static uint8_t aes256_ecb_key[] __attribute__((aligned(64))) = {
    0x5D,0xDC,0x1B,0x35,0x61,0x71,0xCB,0x6F,
    0x27,0x64,0x61,0x8B,0x89,0x14,0xD6,0x37,
    0xE3,0x95,0xE6,0x1B,0x79,0x63,0x6B,0xF1,
    0xE9,0xA9,0x39,0x07,0x2E,0xF7,0x5A,0x8C,
};
static uint8_t aes256_ecb_64_in[] __attribute__((aligned(64))) = {
    0x4C,0xB3,0xE9,0xEB,0xA4,0x3C,0xE2,0x94,
    0xA3,0xF7,0x77,0x1B,0x86,0x35,0xF3,0xA9,
    0xA0,0x8D,0xB1,0x8B,0x61,0xC8,0x4B,0xCF,
    0xB3,0x85,0x16,0x23,0x16,0x54,0xCF,0xED,
    0xB7,0x8E,0x6D,0x66,0x1B,0x55,0x96,0x00,
    0x04,0x05,0xB5,0xB3,0x33,0xE1,0x7D,0x5A,
    0x50,0x56,0x49,0x3D,0x91,0x17,0x34,0x18,
    0x30,0xD1,0x2E,0xF8,0x3F,0xE0,0x11,0x61,
};

static uint8_t aes256_ecb_64_expect[]  __attribute__((aligned(64))) = {
    0x4B,0x44,0x1B,0x09,0xA0,0x0C,0x0D,0x40,
    0x66,0xD9,0xC1,0x46,0xED,0x91,0x4D,0x80,
    0x04,0xD5,0x2E,0x97,0xB0,0x40,0xCB,0x5A,
    0xC6,0xFE,0xF6,0xD7,0xD3,0x2F,0x25,0x7E,
    0x0B,0x4B,0x10,0xA6,0x5F,0x61,0x7F,0x7C,
    0x4A,0xE7,0xDE,0x07,0x44,0xAA,0xAB,0x05,
    0xA8,0x1E,0xDA,0x7F,0xF5,0x84,0x73,0x07,
    0x17,0xB0,0x64,0xE5,0xC0,0xAD,0x74,0x46,
};*/

/********************ecb-192-16*******************/
/*
static uint8_t aes192_ecb_key[] __attribute__((aligned(64))) = {
    0x98,0x20,0xFE,0x58,0xB8,0x8C,0x28,0xF8,
    0x6A,0x5E,0x16,0xE6,0x16,0xDB,0xC6,0x2E,
    0x98,0x20,0xFE,0x58,0xB8,0x8C,0x28,0xF8,
};

static uint8_t aes192_ecb_16_in[] __attribute__((aligned(64))) = {
    0x3C,0x00,0xB4,0xDB,0x0B,0x8E,0x87,0x45,
    0x39,0x75,0x34,0x2D,0x3B,0xAE,0x36,0xCC,
};

static uint8_t aes192_ecb_16_expect[]  __attribute__((aligned(64))) = {
    0x76,0x15,0x37,0xE6,0x24,0xFA,0x3B,0xC2,
    0x2A,0x0E,0xCC,0xE7,0xF7,0x4F,0x4E,0x89,
};*/

/********************ecb-128-16*******************/

const static uint8_t aes128_ecb_key[] __attribute__((aligned(64))) = {
    0x98, 0x20, 0xFE, 0x58, 0xB8, 0x8C, 0x28, 0xF8,
    0x6A, 0x5E, 0x16, 0xE6, 0x16, 0xDB, 0xC6, 0x2E,
};

const static uint8_t aes128_ecb_16_in[] __attribute__((aligned(64))) = {
    0x3C, 0x00, 0xB4, 0xDB, 0x0B, 0x8E, 0x87, 0x45,
    0x39, 0x75, 0x34, 0x2D, 0x3B, 0xAE, 0x36, 0xCC,
};

const static uint8_t aes128_ecb_16_expect[]  __attribute__((aligned(64))) = {
    0x76, 0x15, 0x37, 0xE6, 0x24, 0xFA, 0x3B, 0xC2,
    0x2A, 0x0E, 0xCC, 0xE7, 0xF7, 0x4F, 0x4E, 0x89,
};

typedef struct AES_TEST_DATA_ {
    uint32_t 	aes_key_size;
    uint32_t 	aes_original_size;
    uint32_t 	aes_encrypt_size;
    char 		*aes_key;
    char 		*aes_iv_en;
    char 		*aes_iv_de;
    char 		*aes_original;
    char 		*aes_encrypt;
} AES_TEST_DATA;


AES_TEST_DATA gAesData[2];
uint8_t AesEcbStartIndex = 0;
void gAesData_init(void)
{
    int i = 0;
    /*
       memset(&gAesData,0x00,sizeof(gAesData));
       gAesData[i].aes_key_size             = AES_KEY_LEN_BITS_256;
       gAesData[i].aes_original_size        = sizeof(aes256_cbc_64_in);
       gAesData[i].aes_encrypt_size         = sizeof(aes256_cbc_64_expect);
       gAesData[i].aes_key                  = (char *)aes256_cbc_key;
       gAesData[i].aes_iv_en                = (char *)aes256_cbc_64_iv;
       gAesData[i].aes_iv_de                = (char *)aes256_cbc_64_iv2;
       gAesData[i].aes_original             = (char *)aes256_cbc_64_in;
       gAesData[i].aes_encrypt              = (char *)aes256_cbc_64_expect;

       i++;
       gAesData[i].aes_key_size             = 0;//AES_KEY_LEN_BITS_192;
       gAesData[i].aes_original_size        = sizeof(aes192_cbc_16_in);
       gAesData[i].aes_encrypt_size         = sizeof(aes192_cbc_16_expect);
       gAesData[i].aes_key                  = (char *)aes256_cbc_key;
       gAesData[i].aes_iv_en                = (char *)aes192_cbc_16_iv;
       gAesData[i].aes_iv_de                = (char *)aes192_cbc_16_iv2;
       gAesData[i].aes_original             = (char *)aes192_cbc_16_in;
       gAesData[i].aes_encrypt              = (char *)aes192_cbc_16_expect;
       i++;
    */

    gAesData[i].aes_key_size             = AES_KEY_LEN_BITS_128;
    gAesData[i].aes_original_size        = sizeof(aes128_cbc_64_in);
    gAesData[i].aes_encrypt_size         = sizeof(aes128_cbc_64_expect);
    gAesData[i].aes_key                  = (char *)aes128_cbc_key;
    gAesData[i].aes_iv_en                = (char *)aes128_cbc_64_iv;
    gAesData[i].aes_iv_de                = (char *)aes128_cbc_64_iv2;
    gAesData[i].aes_original             = (char *)aes128_cbc_64_in;
    gAesData[i].aes_encrypt              = (char *)aes128_cbc_64_expect;


    AesEcbStartIndex = (++i);
    /*
        gAesData[i].aes_key_size             = AES_KEY_LEN_BITS_256;
        gAesData[i].aes_original_size        = sizeof(aes256_ecb_64_in);
        gAesData[i].aes_encrypt_size         = sizeof(aes256_ecb_64_expect);
        gAesData[i].aes_key                  = (char *)aes256_ecb_key;
        gAesData[i].aes_iv_en                = NULL;
        gAesData[i].aes_iv_de                = NULL;
        gAesData[i].aes_original             = (char *)aes256_ecb_64_in;
        gAesData[i].aes_encrypt              = (char *)aes256_ecb_64_expect;
        i++;

        gAesData[i].aes_key_size             = 0;//AES_KEY_LEN_BITS_192;
        gAesData[i].aes_original_size        = sizeof(aes192_ecb_16_in);
        gAesData[i].aes_encrypt_size         = sizeof(aes192_ecb_16_expect);
        gAesData[i].aes_key                  = (char *)aes192_ecb_key;
        gAesData[i].aes_iv_en                = NULL;
        gAesData[i].aes_iv_de                = NULL;
        gAesData[i].aes_original             = (char *)aes192_ecb_16_in;
        gAesData[i].aes_encrypt              = (char *)aes192_ecb_16_expect;
        i++;
    */
    gAesData[i].aes_key_size             = AES_KEY_LEN_BITS_128;
    gAesData[i].aes_original_size        = sizeof(aes128_ecb_16_in);
    gAesData[i].aes_encrypt_size         = sizeof(aes128_ecb_16_expect);
    gAesData[i].aes_key                  = (char *)aes128_ecb_key;
    gAesData[i].aes_iv_en                = NULL;
    gAesData[i].aes_iv_de                = NULL;
    gAesData[i].aes_original             = (char *)aes128_ecb_16_in;
    gAesData[i].aes_encrypt              = (char *)aes128_ecb_16_expect;
}

int aes_test_demo_hal(uint8_t index, uint8_t mode)
{
    int ret;
    sec_aes_context ctx;
    char *desc;
    uint16_t keylen;
    if (gAesData[index].aes_key_size == AES_KEY_LEN_BITS_256) {
        keylen = 256;
        return 0;
    } else if (gAesData[index].aes_key_size == 1) {
        keylen = 192;
        return 0;
    } else if (gAesData[index].aes_key_size == AES_KEY_LEN_BITS_128) {
        keylen = 128;
    } else {
        return -1;
    }

    if (mode) {
        desc = "cbc";
    } else {
        desc = "ecb";
    }

    //encrypt
    if (mode) {
        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[index].aes_key,
                                  gAesData[index].aes_key_size,
                                  gAesData[index].aes_original,
                                  out_result,
                                  gAesData[index].aes_original_size,
                                  gAesData[index].aes_iv_en);
        if (ret != CSI_OK) {
            return CSI_ERROR;
        }
    } else {
        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[index].aes_key,
                                  gAesData[index].aes_key_size,
                                  gAesData[index].aes_original,
                                  out_result,
                                  gAesData[index].aes_original_size);
        if (ret != CSI_OK) {
            return CSI_ERROR;
        }
    }
    ret = memcmp(gAesData[index].aes_encrypt, out_result, gAesData[index].aes_encrypt_size);
    if (ret == 0) {
        log_debug("case%d: aes_%s_%d data_len_%d encrypt success! Fuzzing testing pass!\n", index, desc, keylen, gAesData[index].aes_original_size);
    } else {
        log_debug("case%d: aes_%s_%d data_len_%d encrypt failed! Fuzzing testing not pass!\n", index, desc, keylen, gAesData[index].aes_original_size);
        return CSI_ERROR;
    }

    //decrypt
    memset(out_result, 0x00, sizeof(out_result));
    if (mode) {
        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[index].aes_key,
                                  gAesData[index].aes_key_size,
                                  gAesData[index].aes_encrypt,
                                  out_result,
                                  gAesData[index].aes_encrypt_size,
                                  gAesData[index].aes_iv_de);
        if (ret != CSI_OK) {
            return CSI_ERROR;
        }
    } else {
        ret = hal_aes_ecb_decrypt(&ctx,
                                  gAesData[index].aes_key,
                                  gAesData[index].aes_key_size,
                                  gAesData[index].aes_encrypt,
                                  out_result,
                                  gAesData[index].aes_encrypt_size);
        if (ret != CSI_OK) {
            return CSI_ERROR;
        }
    }
    ret = memcmp(gAesData[index].aes_original, out_result, gAesData[index].aes_original_size);
    if (ret != CSI_OK) {
        return CSI_ERROR;
    }
    if (ret == 0) {
        log_debug("case%d: aes_%s_%d data_len_%d decrypt success! Fuzzing testing pass!\n", index, desc, keylen, gAesData[index].aes_original_size);
    } else {
        log_debug("case%d: aes_%s_%d data_len_%d decrypt failed! Fuzzing testing not pass!\n", index, desc, keylen, gAesData[index].aes_original_size);
        return CSI_ERROR;
    }

    if (mode) {//cbc
        ret = hal_aes_cbc_encrypt(NULL,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test context is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test context is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  NULL,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test key is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[0].aes_key,
                                  0,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[0].aes_key,
                                  255,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 255 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 255 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  NULL,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test in is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test in is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  NULL,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test out is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test out is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  0,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test size is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test size is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_encrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  NULL);
        if (ret != CSI_OK) {
            log_debug("exception test iv is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test iv is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(NULL,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test context is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test context is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  NULL,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test key is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[0].aes_key,
                                  0,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[0].aes_key,
                                  255,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 255 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 255 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  NULL,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test in is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test in is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  NULL,
                                  gAesData[0].aes_original_size,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test out is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test out is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  0,
                                  gAesData[0].aes_iv_en);
        if (ret != CSI_OK) {
            log_debug("exception test size is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test size is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_cbc_decrypt(&ctx,
                                  gAesData[0].aes_key,
                                  gAesData[0].aes_key_size,
                                  gAesData[0].aes_original,
                                  out_result,
                                  gAesData[0].aes_original_size,
                                  NULL);
        if (ret != CSI_OK) {
            log_debug("exception test iv is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test iv is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

    } else {//ecb
        ret = hal_aes_ecb_encrypt(NULL,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test context is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test context is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  NULL,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test key is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  0,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  255,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 255 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 255 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  NULL,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test in is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test in is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  NULL,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test out is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test out is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  out_result,
                                  0);
        if (ret != CSI_OK) {
            log_debug("exception test size is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test size is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_decrypt(NULL,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test context is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test context is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_decrypt(&ctx,
                                  NULL,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test key is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_decrypt(&ctx,
                                  gAesData[1].aes_key,
                                  0,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_decrypt(&ctx,
                                  gAesData[1].aes_key,
                                  255,
                                  gAesData[1].aes_original,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test key_len is 255 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test key_len is 255 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  NULL,
                                  out_result,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test in is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test in is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  NULL,
                                  gAesData[1].aes_original_size);
        if (ret != CSI_OK) {
            log_debug("exception test out is NULL failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test out is NULL success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }

        ret = hal_aes_ecb_encrypt(&ctx,
                                  gAesData[1].aes_key,
                                  gAesData[1].aes_key_size,
                                  gAesData[1].aes_original,
                                  out_result,
                                  0);
        if (ret != CSI_OK) {
            log_debug("exception test size is 0 failed get out! Fuzzing testing pass!\n");
        } else {
            log_debug("exception test size is 0 success get out! Fuzzing testing not pass!\n");
            return CSI_ERROR;
        }
    }
    return CSI_OK;
}

int test_case_aes(void)
{
    int ret = 0;
    log_debug("================aes-test-start================\n");
    gAesData_init();
    for (int i = 0; i < sizeof(gAesData) / sizeof(AES_TEST_DATA); i++) {
        if (i < AesEcbStartIndex) {
            ret = aes_test_demo_hal(i, 1); //cbc
            if (ret != 0) {
                return -1;
            }
        } else {
            ret = aes_test_demo_hal(i, 0); //ecb
            if (ret != 0) {
                return -1;
            }
        }
    }
    log_debug("================aes-test-over================\n\n");
    return 0;
}

#endif

const char *msg = "HelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorldHelloWorld";
const uint8_t sum_result[] = {
    0xcf, 0x6f, 0x46, 0x8a, 0x8,  0x17, 0xc0, 0x86, 0xae, 0x4, 0x25,
    0x13, 0x97, 0xd2, 0xd3, 0xe5, 0x2a, 0x43, 0xf5, 0xe3, 0xc, 0xa9,
    0x76, 0x2,  0x47, 0x23, 0x9e, 0xc7, 0x1b, 0xcf, 0xd6, 0xd,
};
uint8_t sum[32];
int test_case_sha(void)
{
    log_debug("================sha-test-start================\n");
    int ret = 0;

    memset(sum, 0x00, 32);
    sec_sha256_context ctx_hal;
    memset(sum, 0x00, 32);
    ret = hal_sha_digest(&ctx_hal, 0, (void *)msg, strlen(msg), sum);
    if (ret != 0) {
        log_debug("sha256 digest faled! Fuzzing testing not pass!\n");
        return -1;
    }
    ret = memcmp(sum_result, sum, 32);
    if (ret == 0) {
        log_debug("normal test data_len_%d success! Fuzzing testing pass!\n", strlen(msg));
    } else {
        log_debug("normal test sha256 data_len_%d failed! Fuzzing testing not pass!\n", strlen(msg));
        return -1;
    }

    ret = hal_sha_digest(NULL, 0, (void *)msg, strlen(msg), sum);
    if (ret != 0) {
        log_debug("exception test context is NULL failed get sha256 digest!  Fuzzing testing pass!\n");
    } else {
        log_debug("exception test context is NULL success get sha256 digest!  Fuzzing testing not pass!\n");
        return -1;
    }

    ret = hal_sha_digest(&ctx_hal, 0, NULL, strlen(msg), sum);
    if (ret != 0) {
        log_debug("exception test in is NULL failed get sha256 digest!  Fuzzing testing pass!\n");
    } else {
        log_debug("exception test in is NULL success get sha256 digest!  Fuzzing testing not pass!\n");
        return -1;
    }

    ret = hal_sha_digest(&ctx_hal, 0, (void *)msg, 0, sum);
    if (ret != 0) {
        log_debug("exception test size is 0 failed get sha256 digest!  Fuzzing testing pass!\n");
    } else {
        log_debug("exception test size is 0 success get sha256 digest!  Fuzzing testing not pass!\n");
        return -1;
    }

    ret = hal_sha_digest(&ctx_hal, 0, (void *)msg, strlen(msg), NULL);
    if (ret != 0) {
        log_debug("exception test digest is NULL failed get sha256 digest!  Fuzzing testing pass!\n");
    } else {
        log_debug("exception test digest is NULL success get sha256 digest!  Fuzzing testing not pass!\n");
        return -1;
    }
    log_debug("================sha-test-over================\n\n");
    return 0;
}
#endif
