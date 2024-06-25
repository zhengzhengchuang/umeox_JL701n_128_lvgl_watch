
#ifndef _BIZBUS_HAL_H_
#define _BIZBUS_HAL_H_

// #include "cpu.h"
// #include <stdio.h>
// #include "cpu.h"
#include "system/includes.h"

typedef enum _bizbus_ret_t {
    BIZBUS_SUCCESS,
    BIZBUS_ERR_INIT_FAILED,
    BIZBUS_ERR_NOT_FOUND,
    BIZBUS_ERR_ALREADY,
    BIZBUS_ERR_UNEXPECTED,
    BIZBUS_ERR_TIMEOUT,
    BIZBUS_ERR_NO_MEM,
    BIZBUS_ERR_NOT_PERMITTED,
    BIZBUS_ERR_INVALID_PARAM,
    BIZBUS_ERR_IO,
    BIZBUS_ERR_FAILED,
    BIZBUS_ERR_POWER_BY_KEY = 0xFE,
} bizbus_ret_t;


#define BIZBUS_MAC_RAW_LEN                      6

#define BIZBUS_PK_LEN                           11

#define BIZBUS_VERSION_LEN                      9
#define BIZBUS_VENDOR_LEN                       8
#define BIZBUS_MAC_LEN                          12
#define BIZBUS_DN_LEN                           (BIZBUS_VERSION_LEN + BIZBUS_VENDOR_LEN + BIZBUS_MAC_LEN)

#define BIZBUS_SIGN_KEY_LEN                     32
#define BIZBUS_DS_LEN                           BIZBUS_SIGN_KEY_LEN
#define BIZBUS_PS_LEN                           16

#define BIZBUS_TS_LEN                           13
#define BIZBUS_SHA1HMAC_LEN                     40

#define BIZBUS_SONCE_LEN                        8 // 验签设备端随机数长度, for fgs
#define BIZBUS_SIGN_MAX_LEN                     256 //验签数据大小


typedef struct _bizbus_init_para_t {
    uint8_t pk_str[BIZBUS_PK_LEN + 1]; //三元组PK，请客户联系阿里BD申请
    uint8_t ps_str[BIZBUS_PS_LEN + 1]; //三元组PS，请客户联系阿里BD申请
    uint8_t version_str[BIZBUS_VERSION_LEN + 1];
    uint8_t vendor_str[BIZBUS_VENDOR_LEN + 1];
    uint8_t mac_str[BIZBUS_MAC_LEN + 1];
    uint8_t id2_pk[BIZBUS_PK_LEN + 1]; //安全PK，请客户联系阿里BD申请
    uint8_t dn_without_prefix;  // 如果客户实现了 bizbus_register_dn_get，此项设置无效
    // 如果未实现 bizbus_register_dn_get，0 表示：使用带前缀的DN,如 HAAS-3AX-XXX-YYY-C1B1637E0565，此项是为了兼容已有版本
//                                   1 表示：使用不带前缀作为DN，如 C1B1637E0565，此项为了方便后续客户自行导入DN到后台时，无需额手动添加前缀
} bizbus_init_para_t;

void bizbus_init(bizbus_init_para_t *para);

/***********************************************************
 * @fun        aos_get_idt_task_status
 * @breif      获取当前是否在进行安全认证
 * @param         无
 * @return        0:开始认证
 *                1:认证即将结束
 *                2:认证退出或者未进行认证
 **********************************************************/
int32_t aos_get_idt_task_status(void);
/***********************************************************
 * @fun        aos_get_auth_status
 * @breif
 * @param         无
 * @return        0：该设备已经认证通过
 *                -1：未进行安全认证或者认证未通过
 **********************************************************/
int32_t aos_get_auth_status(void);

// hal 需要厂商实现，与APP SDK端的数据收发
bizbus_ret_t bizbus_hal_send(char *sid, uint8_t *data, uint32_t data_len);
bizbus_ret_t bizbus_hal_recv(uint8_t *data, uint32_t data_len);

bizbus_ret_t bizbus_recv(uint8_t *json_string, uint32_t data_len);

//厂商实现，设置ble 系统UTC时间
bizbus_ret_t bizbus_set_time(uint8_t *time_str);

// for chengchema
// bizbus_ret_t wearos_ble_recv(uint8_t * data, uint32_t data_len);
bizbus_ret_t bizbus_send_ccm(uint8_t *data);

/*返回值为ds长度*/
typedef int (* bizbus_ds_get_func)(char device_secret[BIZBUS_DS_LEN + 1]);
/*返回值为dn长度*/
typedef int (* bizbus_dn_get_func)(char device_name[BIZBUS_DN_LEN + 1]);
/*返回值为pk长度*/
typedef int (* bizbus_pk_get_func)(char product_key[BIZBUS_PK_LEN + 1]);
/*若客户采用产线烧录三元组方式，需要注册一下函数，非产线烧录三元组模式无需实现*/
bizbus_ret_t bizbus_register_ds_get(bizbus_ds_get_func ds_get_func);
bizbus_ret_t bizbus_register_dn_get(bizbus_dn_get_func dn_get_func);
bizbus_ret_t bizbus_register_pk_get(bizbus_pk_get_func pk_get_func);

#endif
