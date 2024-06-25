/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdint.h>
#define IDENT_HASH_LEN 32
typedef enum {
    HAL_OK = 0,
    HAL_ERROR = -1,
    HAL_INVALID_PARAM = -2,
    HAL_BIND_VERIFY_FAILED = -3,
    HAL_OPEN_ASSET_ERROR = -4,
    HAL_ASSET_READ_ERROR = -5,
    HAL_KEY_INIT_ERROR = -6,
    HAL_KEY_READ_ERROR = -7,
    HAL_MALLOC_ERROR = -8,
    HAL_READ_CREDENT_ERROR = -9,
    HAL_DIGEST_GET_ERROR = -10,
    HAL_CREDENT_HASH_ERROR = -11,
    HAL_HUK_GET_ERROR = -12,
    HAL_HUK_CHECK_ERROR = -13,
    HAL_SAD_VERIFY_ERROR = -14,
} hal_error_t;

typedef struct {
    uint16_t type : 1;
    uint16_t rsvr : 15;
} parser_key_flag_t;

typedef struct {
    uint16_t          type;
    uint16_t          size;
    parser_key_flag_t flag;
    uint16_t          rsvr;
    uint32_t          offset;
    uint32_t          rsvr2;
} parser_key_info_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t rsv;
    uint16_t keynum;
    uint16_t digest_type;
    uint32_t total_size;
} parser_head_t;

typedef struct {
    parser_head_t     head;
    parser_key_info_t key[1];
} parser_t;

typedef struct {
    uint32_t credent_len;
    uint8_t hash[IDENT_HASH_LEN];
} credent_info_t;

typedef struct {
    uint8_t *cid;
    uint32_t cid_len;
    uint8_t *huk;
    uint32_t huk_len;
    uint8_t *sad;
    uint32_t sad_len;
    credent_info_t credent_info;
} kp_key_t;

hal_error_t hal_open_asset(void **fd, char asset_name[36]);
hal_error_t hal_write_asset(void *fd, uint32_t offset, void *data, uint32_t data_len);
hal_error_t hal_read_asset(void *fd, uint32_t offset, void *buffer, uint32_t *data_len);
hal_error_t hal_close_asset(void *fd);
hal_error_t hal_access_asset(uint32_t *access_flag, char asset_name[36]);
hal_error_t hal_remove_asset(char asset_name[36]);
hal_error_t hal_clear_assets(void);

hal_error_t hal_get_huk(uint8_t *huk, uint32_t *len);
hal_error_t hal_get_ssk(uint8_t *seed, uint8_t *salt, uint8_t *derived_key, uint8_t derived_key_len);

hal_error_t hal_aes_ecb_encrypt(void *context, void *key, uint32_t key_len, void *in, void *out, uint32_t size);
hal_error_t hal_aes_ecb_decrypt(void *context, void *key, uint32_t key_len, void *in, void *out, uint32_t size);
hal_error_t hal_aes_cbc_encrypt(void *context, void *key, uint32_t key_len, void *in, void *out, uint32_t size, void *iv);
hal_error_t hal_aes_cbc_decrypt(void *context, void *key, uint32_t key_len, void *in, void *out, uint32_t size, void *iv);

hal_error_t hal_rng_get_data(void *random, uint32_t size);
hal_error_t hal_sha_digest(void *context, uint32_t mode, void *in, uint32_t size, void *digest);

hal_error_t hal_get_deviceid(uint8_t *buf_did, uint32_t *len_did);
hal_error_t hal_get_fw_version(char *version);
hal_error_t hal_get_timestamp(uint32_t *tm);
hal_error_t hal_get_sn(uint8_t *buf_sn, uint32_t *len_sn);
hal_error_t hal_get_companyname(uint8_t *buffer, uint32_t *len);
hal_error_t hal_get_productmodel(uint8_t *buffer, uint32_t *len);
hal_error_t hal_hardware_compat_check(void);
hal_error_t hal_get_credential_data(uint8_t *data, uint32_t *len);
hal_error_t hal_get_compile_timestamp(uint32_t *timestamp);

hal_error_t hal_get_protoctype(uint32_t *ptype);
hal_error_t hal_ble_write(uint8_t *data, uint16_t len);
void hal_log(int level, const char *format, int32_t value);
void hal_log_ext(const char *format, ...);

void *hal_malloc(uint32_t size);
void *hal_calloc(uint32_t nblock, uint32_t size);
void hal_free(void *pt);
void *hal_realloc(void *pt, uint32_t size);
