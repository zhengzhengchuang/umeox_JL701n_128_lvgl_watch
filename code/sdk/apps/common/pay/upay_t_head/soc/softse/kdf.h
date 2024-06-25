/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef KDF_H
#define KDF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int sec_ansx963_kdf(uint8_t *seed, uint8_t *salt, uint8_t *derived_key, uint32_t derived_key_len);

#ifdef __cplusplus
}
#endif


#endif /* kdf.h */
