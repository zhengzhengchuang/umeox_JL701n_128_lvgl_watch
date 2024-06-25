#include "kdf.h"
#include "sha256.h"
#include <stddef.h>
#include "app_config.h"
#include "system/includes.h"
#include "../port/iotsec.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)

#define Big2Little32(A)   ( (( (uint32_t)(A) & 0xff000000) >> 24)  | \
                            (( (uint32_t)(A) & 0x00ff0000) >> 8)   | \
                            (( (uint32_t)(A) & 0x0000ff00) << 8)   | \
                            (( (uint32_t)(A) & 0x000000ff) << 24))

/*○ description:
    ■ based on the input seed and salt,the derived key of the spectified length is derived according to the ANS X9.63-KDF
  ○ param:
    ■ seed - seed of derivation algorithm
    ■ salt - optional,user specified salt,can be NULL
    ■ derived_key - the pointer to the derived key
    ■ derived_key_len - the length of the derived key
  ○ return:
    ■ 0- success,-1 failed*/
int sec_ansx963_kdf(uint8_t *seed, uint8_t *salt, uint8_t *derived_key, uint32_t derived_key_len)
{
    sec_sha256_context ctx_hal;
    uint8_t hash_buf[48] = {0};
    uint8_t hash_result[32] = {0};
    uint8_t *p_buf = NULL;

    int i = 0;
    int ret = 0;
    uint32_t counter = 0x01;
    uint8_t seed_len = 16;
    uint8_t salt_len = 16;
    uint16_t hash_len;

    if (seed == NULL || derived_key == NULL) {
        return -1;
    }

    hash_len = seed_len + sizeof(counter) + salt_len;

    if (derived_key_len < sizeof(hash_result)) {
        return -1;
    }

    for (i = 0; i < derived_key_len / sizeof(hash_result); i++) {
        p_buf = hash_buf;

        memcpy((void *)p_buf, (void *)seed, 16); /*copy seed*/
        p_buf += seed_len;

        counter = Big2Little32(counter);
        memcpy((void *)p_buf, (void *)&counter, sizeof(counter)); /*copy seed*/
        p_buf += sizeof(counter);
        counter = Big2Little32(counter);

        if (salt != NULL) {
            memcpy((void *)p_buf, (void *)salt, 16); /*copy salt*/
            p_buf += salt_len;
        }

        /*get hash of buf*/
        ret = csi_sha_digest(&ctx_hal, hash_buf, hash_len, hash_result);
        if (ret != 0) {
            return -1;
        }

        memcpy((void *)(derived_key + i * sizeof(hash_result)), hash_result, sizeof(hash_result));

        counter++;
    }

    return 0;
}
#endif
