/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef IOT_SEC_H
#define IOT_SEC_H

#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>

typedef enum {
    CSI_OK          =  0,
    CSI_ERROR       = -1,
    CSI_BUSY        = -2,
    CSI_TIMEOUT     = -3,
    CSI_UNSUPPORTED = -4
} csi_error_t;

typedef enum {
    AES_KEY_LEN_BITS_128  = 128,
    AES_KEY_LEN_BITS_256  = 256
} csi_aes_key_bits_t;

#define CHECK_UNUSED_ARG(a)

/*○ description:
    ■ get device id(mac addres format in hex separated ':')，id must ended with ‘\0’, and len_did not include '\0'.All letters should be capital，len_did must be 17.For example："AA:BB:CC:00:11:22"
  ○ param
    ■ buf_did - buffer to store device id
    ■ len_did - the length of device id
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_deviceid(uint8_t *buf_did, uint32_t *len_did);

/*○ description:
    ■ get system timestamp(unix timestamp format)
  ○ param
    ■ tm - the var which store the timestamp
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_timestamp(uint32_t *tm);

/*○ description:
    ■ get serial number of device(the sn which factory printed on the card)，len of sn <= 32 bytes,sn could only include letters or numbers or underline.Only product(type of card) which need to show sn on small routine to realize this function，otherwise set buf_sn to "" and len_sn to 0
  ○ param
    ■ buf_sn - buffer to store device serial number
    ■ len_sn - length of device serial number
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_sn(uint8_t *buf_sn, uint32_t *len_sn);

/*○ description:
    ■ get device company name
  ○ param
    ■ buffer - buffer to store company name
    ■ len - length of company name
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_companyname(uint8_t *buffer, uint32_t *len);

/*○ description:
    ■ get protocal type of device
  ○ param
    ■ ptype - var which store protocal type of device
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_protoctype(uint32_t *ptype);

/*○ description:
    ■ get device product model
  ○ param
    ■ buffer - buffer to store product model
    ■ len - length of product model
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_productmodel(uint8_t *buffer, uint32_t *len);

/*○ description:
    ■ send bluetooth data
  ○ param
    ■ data - buffer which store data to send
    ■ len - length of data to send(len<=20)
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_ble_write(uint8_t *data, uint16_t len);

/*○ description:
    ■ print log
  ○ param
    ■ level - log level
    ■ format - log string in format
    ■ value - output value
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
void csi_log(int level, const char *format, int32_t value);

/*○ description:
    ■ print log
  ○ param
    ■ format - log string in format
    ■ val_list - ouput values
  ○ return
    ■ none*/
void csi_log_ext(const char *format, va_list *val_list);

/*○ description:
    ■ open asset and get asset handler
  ○ param
    ■ fd  - asset handler
    ■ asset_name - asset name
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_open_asset(void **fd, char asset_name[36]);

/*○ description:
    ■ write data to asset
  ○ param
    ■ fd  - asset handler
    ■ offset - asset offset
    ■ buffer - buffer which store data to write
    ■ data_len - length of buffer to write to asset
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_write_asset(void *fd, uint32_t offset, void *data, uint32_t data_len);

/*○ description:
    ■ read asset data
  ○ param
    ■ fd  - asset handler
    ■ offset - asset offset
    ■ buffer - buffer which store data to read
    ■ data_len - length of data read to buffer
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_read_asset(void *fd, uint32_t offset, void *buffer, uint32_t *data_len);

/*○ description:
    ■ close asset
  ○ param
    ■ fd - asset handler
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_close_asset(void *fd);

/*○ description:
    ■ check if asset can be accessed
  ○ param
    ■ access_flag  - asset flag
		■ asset_name  - asset name
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_access_asset(uint32_t *access_flag, char asset_name[36]);

/*○ description:
    ■ remove asset
  ○ param
		■ asset_name  - asset name
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_remove_asset(char asset_name[36]);

/*○ description:
    ■ clear asset data
  ○ param
		■ none
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_clear_assets(void);

/*○ description:
    ■ get system monotonous counter
  ○ param
    ■ counter- counter which monotonous increased
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_monotonous_count(uint32_t *counter);

/*○ description:
    ■ derived key by seed and salt in ANS X9.63
  ○ param
    ■ seed - seed for derived
    ■ salt - salt for derived，may be NULL
    ■ derived_key - buffer to store key generated
    ■ derived_key_len - length of derived_key to generated
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_ssk(uint8_t *seed, uint8_t *salt, uint8_t *derived_key, uint8_t derived_key_len);

/*○ description:
    ■ get unique id(16 bytes)
  ○ param
    ■ huk - buffer to store unique id
    ■ len - length of unique id
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_get_huk(uint8_t *huk, uint32_t *len);

/*○ description:
    ■ get compile timestamp
  ○ param
  ○ return
    ■ compile timestamp*/
csi_error_t csi_get_compile_timestamp(uint32_t *timestamp);

/*○ description:
    ■ init aes context
  ○ param
    ■ context - aes context to init
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_init(void *context);

/*○ description:
    ■ uninit aes context
  ○ param
    ■ context - aes context to uninit
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_uninit(void *context);

/*○ description:
    ■ set aes encrypt key
  ○ param
    ■ context - aes context
    ■ key - aes key
    ■ key_len - aes key len in bits
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_set_encrypt_key(void *context, void *key, uint32_t key_len);

/*○ description:
    ■ set aes decrypt key
  ○ param
    ■ context - aes context
    ■ key - aes key
    ■ key_len - aes key len in bits
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_set_decrypt_key(void *context, void *key, uint32_t key_len);

/*○ description:
    ■ aes encrypt in ecb mode
  ○ param
    ■ context - aes context
    ■ in - buffer which store data to be encrypted
    ■ out - buffer to store encrypted data
    ■ size - length of in
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_ecb_encrypt(void *context, void *in, void *out, uint32_t size);

/*○ description:
    ■ aes decrypt in ecb mode
  ○ param
    ■ context - aes context
    ■ in - buffer which store data to be decrypted
    ■ out - buffer to store decrypted data
    ■ size - length of in
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_ecb_decrypt(void *context, void *in, void *out, uint32_t size);

/*○ description:
    ■ aes encrypt in cbc mode
  ○ param
    ■ context - aes context
    ■ in - buffer which store data to be encrypted
    ■ out - buffer to store encrypted data
    ■ size - length of in
		■ iv - buffer which store vector
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_cbc_encrypt(void *context, void *in, void *out, uint32_t size, void *iv);

/*○ description:
    ■ aes decrypt in cbc mode
  ○ param
    ■ context - aes context
    ■ in - buffer which store data to be decrypted
    ■ out - buffer to store decrypted data
    ■ size - length of in
		■ iv - buffer which store vector
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_aes_cbc_decrypt(void *context, void *in, void *out, uint32_t size, void *iv);

/*○ description:
    ■ get random data
  ○ param
    ■ random - buffer to store random data
    ■ size - length of random data
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_rng_get_data(void *random, uint32_t size);

/*○ description:
    ■ init sha context
  ○ param
    ■ context - sha context to init
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_sha_init(void *context);

/*○ description:
    ■ uninit sha context
  ○ param
    ■ context - sha context to uninit
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_sha_uninit(void *context);

/*○ description:
    ■ start sha operate
  ○ param
    ■ context - sha context
		■ mode - sha mode(sha256)
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_sha_start(void *context, uint32_t mode);

/*○ description:
    ■ update sha digest
  ○ param
    ■ context - sha context
		■ in - hash data buffer
    ■ size - hash data len
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_sha_update(void *context, void *in, uint32_t size);

/*○ description:
    ■ finish sha operate
  ○ param
    ■ context - sha context
		■ in - hash data buffer
    ■ size - hash data len
		■ digest - digest data buffer
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_sha_finish(void *context, void *in, uint32_t size, void *digest);

/*○ description:
    ■ get sha digest
  ○ param
    ■ context - sha context
    ■ in - hash data buffer
    ■ size -hash data len
    ■ digest - digest data buffer
  ○ return
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_sha_digest(void *context, void *in, uint32_t size, void *digest);

/*○ description:
    ■ based on the input seed and salt,the derived key of the spectified length is derived according to the ANS X9.63-KDF
  ○ param:
    ■ seed - seed of derivation algorithm
    ■ salt - optional,user specified salt,can be NULL
    ■ derived_key - the pointer to the derived key
    ■ derived_key_len - the length of the derived key
  ○ return:
    ■ CSI_OK:success，otherwise:fail*/
csi_error_t csi_ansx963_kdf(uint8_t *seed, uint8_t *salt, uint8_t *derived_key, uint32_t derived_key_len);

/*○ description:
    ■ allocates space for an object whose size is specified by 'size' and whose value is indeterminate
  ○ param:
    ■ size - allocated space length
  ○ return:
    ■ either a null pointer or a pointer to the allocated space*/
void *csi_malloc(uint32_t size);

/*○ description:
    ■ allocates space for an array of nmemb objects, each of whose size is 'size'. The space is initialised to all bits zero
  ○ param:
    ■ nblock - number of parameters
    ■ size - parameter length
  ○ return:
    ■ either a null pointer or a pointer to the allocated space*/
void *csi_calloc(uint32_t nblock, uint32_t size);

/*○ description:
    ■ causes the space pointed to by ptr to be deallocated
  ○ param:
    ■ pt - the pointer to be deallocated
  ○ return:
    ■ */
void csi_free(void *pt);

/*○ description:
    ■ changes the size of the object pointed to by ptr to the size specified by size
  ○ param:
    ■ pt - the pointer to be changed
    ■ size - new length of pt
  ○ return:
    ■ either a null pointer or a pointer to the possibly moved allocated space*/
void *csi_realloc(void *pt, uint32_t size);

#endif
