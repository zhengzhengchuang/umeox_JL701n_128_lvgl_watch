/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
*/

//#include "../../../port/iotsec.h"
#include "iotsec.h"
#include "../../softse/aes.h"
#include "../../softse/sha256.h"
#include "../../softse/kdf.h"
//#include "driver_trng.h"
//#include <stdio.h>
#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)

static sec_aes_context aes_ctx;

/*○ 功能描述
    ■ 初始化aes上下文操作
  ○ 接口参数
    ■ context - aes上下文地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_init(void *context)
{
    CHECK_UNUSED_ARG(context);

    sec_aes_init(&aes_ctx);
    return CSI_OK;
}

/*○ 功能描述
    ■ 释放aes上下文操作
  ○ 接口参数
    ■ context - aes上下文地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_uninit(void *context)
{
    CHECK_UNUSED_ARG(context);

    sec_aes_free(&aes_ctx);
    return CSI_OK;
}

/*○ 功能描述
    ■ 设置aes加密密钥
  ○ 接口参数
    ■ context - aes上下文地址
    ■ key - aes加密密钥地址
    ■ key_len - aes加密密钥bit长度
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_set_encrypt_key(void *context, void *key, uint32_t key_len)
{
    CHECK_UNUSED_ARG(context);
    if (!key || (key_len != AES_KEY_LEN_BITS_256 && key_len != AES_KEY_LEN_BITS_128)) {
        return CSI_ERROR;
    }

    sec_aes_setkey_enc(&aes_ctx, key, key_len);
    return CSI_OK;
}

/*○ 功能描述
    ■ 设置aes解密密钥
  ○ 接口参数
    ■ context - aes上下文地址
    ■ key - aes解密密钥地址
    ■ key_len - aes解密密钥bit长度
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_set_decrypt_key(void *context, void *key, uint32_t key_len)
{
    CHECK_UNUSED_ARG(context);
    if (!key || (key_len != AES_KEY_LEN_BITS_256 && key_len != AES_KEY_LEN_BITS_128)) {
        return CSI_ERROR;
    }

    sec_aes_setkey_dec(&aes_ctx, key, key_len);
    return CSI_OK;
}

/*○ 功能描述
    ■ aes ecb模式加密
  ○ 接口参数
    ■ context - aes上下文地址
    ■ in - 明文数据地址
    ■ out - 密文数据地址
    ■ size - 加密数据长度
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_ecb_encrypt(void *context, void *in, void *out, uint32_t size)
{
    CHECK_UNUSED_ARG(context);

    int i = 0;

    if (size % 16) {
        return CSI_ERROR;
    }

    for (i = 0; i < (size / 16); i ++) {
        sec_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, (const unsigned char *)in + 16 * i, (unsigned char *)out + 16 * i);
    }

    return CSI_OK;
}

/*○ 功能描述
    ■ aes ecb模式解密
  ○ 接口参数
    ■ context - aes上下文地址
    ■ in - 密文数据地址
    ■ out - 明文数据地址
    ■ size -解密数据长度
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_ecb_decrypt(void *context, void *in, void *out, uint32_t size)
{
    CHECK_UNUSED_ARG(context);

    int i = 0;

    if (size % 16) {
        return CSI_ERROR;
    }

    for (i = 0; i < (size / 16); i ++) {
        sec_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, (const unsigned char *)in + 16 * i, (unsigned char *)out + 16 * i);
    }

    return CSI_OK;
}

/*○ 功能描述
    ■ aes cbc模式加密
  ○ 接口参数
    ■ context - aes上下文地址
    ■ in - 明文数据地址
    ■ out - 密文数据地址
    ■ size - 加密数据长度
    ■ void - 初始化向量地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_cbc_encrypt(void *context, void *in, void *out, uint32_t size, void *iv)
{
    CHECK_UNUSED_ARG(context);

    int i = 0;

    if (size % 16) {
        return CSI_ERROR;
    }

    sec_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_ENCRYPT, size, iv, (const unsigned char *)in + 16 * i, (unsigned char *)out + 16 * i);

    return CSI_OK;
}

/*○ 功能描述
    ■ aes cbc模式加密
  ○ 接口参数
    ■ context - aes上下文地址
    ■ in - 密文数据地址
    ■ out - 明文数据地址
    ■ size - 加密数据长度
    ■ iv - 初始化向量地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_aes_cbc_decrypt(void *context, void *in, void *out, uint32_t size, void *iv)
{
    CHECK_UNUSED_ARG(context);

    int i = 0;

    if (size % 16) {
        return CSI_ERROR;
    }

    sec_aes_crypt_cbc(&aes_ctx, MBEDTLS_AES_DECRYPT, size, iv, (const unsigned char *)in + 16 * i, (unsigned char *)out + 16 * i);

    return CSI_OK;
}

/*○ 功能描述
    ■ 获取随机数
  ○ 接口参数
    ■ random- 随机数地址
    ■ size - 获取随机数数据长度
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
//csi_error_t csi_rng_get_data(void *random, uint32_t size)
//{
//    trng_read_rand_num(random, size);
//    return CSI_OK;
//}

/*○ 功能描述
    ■ 初始化sha上下文操作
  ○ 接口参数
    ■ context - sha上下文地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_sha_init(void *context)
{
    sec_sha256_init(context);
    return CSI_OK;
}

/*○ 功能描述
    ■ 释放sha上下文操作
  ○ 接口参数
    ■ context - sha上下文地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_sha_uninit(void *context)
{
    sec_sha256_free(context);
    return CSI_OK;
}

/*○ 功能描述
    ■ 启动sha操作
  ○ 接口参数
    ■ context - sha上下文地址
    ■ mode - sha模式(sha256)
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_sha_start(void *context, uint32_t mode)
{
    sec_sha256_starts(context, 0);
    return CSI_OK;
}

/*○ 功能描述
    ■ 更新sha操作
  ○ 接口参数
    ■ context - sha上下文地址
    ■ in - 哈希数据地址
    ■ size -哈希数据长度
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_sha_update(void *context, void *in, uint32_t size)
{
    sec_sha256_update(context, (const unsigned char *)in, size);
    return CSI_OK;
}

/*○ 功能描述
    ■ 结束sha操作
  ○ 接口参数
    ■ context - sha上下文地址
    ■ in - 哈希数据地址
    ■ size -哈希数据长度
    ■ digest - 哈希值地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_sha_finish(void *context, void *in, uint32_t size, void *digest)
{
    if (size != 0) {
        sec_sha256_update(context, (const unsigned char *)in, size);
    }
    sec_sha256_finish(context, (unsigned char *)digest);

    return CSI_OK;
}

/*○ 功能描述
    ■ 执行一次sha哈希操作
  ○ 接口参数
    ■ context - sha上下文地址
    ■ in - 哈希数据地址
    ■ size -哈希数据长度
    ■ digest - 哈希值地址
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t __attribute__((weak)) csi_sha_digest(void *context, void *in, uint32_t size, void *digest)
{
    sec_sha256_init(context);
    sec_sha256_starts(context, 0);
    sec_sha256_update(context, (const unsigned char *)in, size);
    sec_sha256_finish(context, (unsigned char *)digest);
    sec_sha256_free(context);
    return CSI_OK;
}

/*○ description:
    ■ based on the input seed and salt,the derived key of the spectified length is derived according to the ANS X9.63-KDF
  ○ param:
    ■ seed - seed of derivation algorithm
    ■ salt - optional,user specified salt,can be NULL
    ■ derived_key - the pointer to the derived key
    ■ derived_key_len - the length of the derived key
  ○ return:
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_ansx963_kdf(uint8_t *seed, uint8_t *salt, uint8_t *derived_key, uint32_t derived_key_len)
{
    int ret;
    ret = sec_ansx963_kdf(seed, salt, derived_key, derived_key_len);
    if (ret != 0) {
        return CSI_ERROR;
    }

    return CSI_OK;
}

#endif
