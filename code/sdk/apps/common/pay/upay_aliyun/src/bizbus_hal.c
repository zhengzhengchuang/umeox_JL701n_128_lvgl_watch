/*
 * @Description:
 * @Author: HJY
 * @Date: 2022-12-07 15:20:43
 * @LastEditTime: 2023-02-03 10:26:30
 * @LastEditors: HJY
 */
/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <string.h>

#include "bizbus_common.h"

#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

extern int aliyun_bt_data_send(u8 *data, u16 len);

bizbus_ret_t bizbus_hal_send(char *sid, uint8_t *data, uint32_t data_len)
{
    //TODO: 发送蓝牙数据
    printf("%s[len=%d]\n", __func__, data_len);
    // put_buf(data, data_len);
    int ret = aliyun_bt_data_send(data, data_len);
    if (ret == false) {
        return BIZBUS_ERR_FAILED;
    }
    return BIZBUS_SUCCESS;
}


void aliyun_upay_recv(void *priv, u8 *data, int len)
{
    printf("%s[len=%d]\n", __func__, len);
    // put_buf(data, len);
    bizbus_ret_t ret = bizbus_recv(data, len);
    if (ret) {
        printf("aliyun_upay_recv err:%d \n", ret);
    }
}


bizbus_ret_t bizbus_hal_recv(uint8_t *data, uint32_t data_len)
{
    printf("%s[len=%d]\n", __func__, data_len);
    put_buf(data, data_len);
    return bizbus_recv(data, data_len);
}
// for chengchema
// bizbus_ret_t wearos_ble_recv(uint8_t *data, uint32_t data_len)
// {
// 	return bizbus_hal_recv(data, data_len);
// }

bizbus_ret_t bizbus_set_time(uint8_t *time_str)
{
    return 0;
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */

