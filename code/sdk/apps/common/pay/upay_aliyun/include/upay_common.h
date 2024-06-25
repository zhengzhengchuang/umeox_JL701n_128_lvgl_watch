#ifndef UPAY_COMMON_H
#define UPAY_COMMON_H

#include <string.h>
// #include "cpu.h"
#include "system/includes.h"

typedef enum {
    UPAY_RV_OK = 0,
    UPAY_RV_WRITE_ERROR,
    UPAY_RV_READ_ERROR,
    UPAY_RV_DEL_ERROR,
    UPAY_RV_NOT_FOUND,
    UPAY_RV_IO_ERROR,
    UPAY_RV_NOT_INITIALISED,
    UPAY_RV_NETWORK_ERROR,
    UPAY_RV_ECC_GENERATE_ERROR,
    UPAY_RV_ECC_SHARE_ERROR,
    UPAY_RV_BUF_TOO_SHORT,
    UPAY_RV_ENCRYPTION_ERRO,
    UPAY_RV_DECRYPTION_ERRO,
    UPAY_RV_WRONG_TIMESTAMP,
    UPAY_RV_WRONG_PARAM,
    UPAY_RV_PRODUCT_MODEL_ERROR,
    UPAY_RV_BINDING_REPEAT,
    UPAY_RV_SEMAPHORE_CREATE_ERROR,
    UPAY_RV_DATA_WAITING_TIMEOUT,
    UPAY_RV_BINDING_DATA_FORMAT_ERROR,
    UPAY_RV_UNKNOWN
} upay_retval_t;

typedef enum {
    UPAY_STATUS_BINDING_FAIL = 0x0,
    UPAY_STATUS_UNBINDED,
    UPAY_STATUS_START_BINDING,       //准备环境，进度0
    UPAY_STATUS_GETTING_PROFILE,     //进度10%，进度>0时说明BLE连接已经OK，可以关闭绑定码，显示进度
    UPAY_STATUS_SAVING_DATA,         //进度30%
    UPAY_STATUS_SAVING_DATA_OK,      //进度70%
    UPAY_STATUS_FINISH_BINDING,      //进度90%
    UPAY_STATUS_FINISH_BINDING_OK,
    UPAY_STATUS_BINDING_OK = 0xA5,
    UPAY_STATUS_UNKNOWN = 0xFF
} upay_binding_status_e;

#if 0
typedef unsigned short     uint16_t;
typedef unsigned char      uint8_t;
typedef long long          int64_t;
typedef unsigned long long uint64_t;
typedef int                int32_t;
typedef unsigned int       uint32_t;
#endif

extern char *upay_get_version(void);

#if !defined(__cplusplus)
#ifndef bool
typedef unsigned char bool;
#endif
#ifndef false
#define false ((bool)0)
#define true  ((bool)1)
#endif
#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifndef __DEBUG
#define STATIC static
#else
#define STATIC
#endif

#endif /* common_h */

