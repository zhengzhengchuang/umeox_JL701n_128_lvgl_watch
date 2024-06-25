/*********************************************************************************************
    *   Filename        : le_counter.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:17

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef    __LE_COMMON_H_
#define    __LE_COMMON_H_

typedef enum {
    BLE_ST_NULL = 0,
    BLE_ST_INIT_OK, //init ok
    BLE_ST_IDLE,
    BLE_ST_ADV,
    BLE_ST_CONNECT,
    BLE_ST_SEND_DISCONN,
    BLE_ST_NOTIFY_IDICATE,//server ok
    BLE_ST_SCAN,
    BLE_ST_SEARCH_COMPLETE,//client ok
} ble_state_e;


enum {
    APP_BLE_NO_ERROR = 0,
    APP_BLE_BUFF_ERROR = 1,
    APP_BLE_BUFF_FULL = 2,
    APP_BLE_OPERATION_ERROR = 3,
    APP_BLE_IS_DISCONN =  4,
};

typedef enum {
    HCI_EIR_DATATYPE_FLAGS =                                                 0x01,
    HCI_EIR_DATATYPE_MORE_16BIT_SERVICE_UUIDS =                              0x02,
    HCI_EIR_DATATYPE_COMPLETE_16BIT_SERVICE_UUIDS =                          0x03,
    HCI_EIR_DATATYPE_MORE_32BIT_SERVICE_UUIDS =                              0x04,
    HCI_EIR_DATATYPE_COMPLETE_32BIT_SERVICE_UUIDS =                          0x05,
    HCI_EIR_DATATYPE_MORE_128BIT_SERVICE_UUIDS =                             0x06,
    HCI_EIR_DATATYPE_COMPLETE_128BIT_SERVICE_UUIDS =                         0x07,
    HCI_EIR_DATATYPE_SHORTENED_LOCAL_NAME =                                  0x08,
    HCI_EIR_DATATYPE_COMPLETE_LOCAL_NAME =                                   0x09,
    HCI_EIR_DATATYPE_TX_POWER_LEVEL =                                        0x0A,
    HCI_EIR_DATATYPE_CLASS_OF_DEVICE =                                       0x0D,
    HCI_EIR_DATATYPE_SIMPLE_PAIRING_HASH_C =                                 0x0E,
    HCI_EIR_DATATYPE_SIMPLE_PAIRING_RANDOMIZER_R =                           0x0F,
    HCI_EIR_DATATYPE_SECURITY_MANAGER_TK_VALUE =                             0x10,
    HCI_EIR_DATATYPE_SECURITY_MANAGER_OOB_FLAGS =                            0x11,
    HCI_EIR_DATATYPE_SLAVE_CONNECTION_INTERVAL_RANGE =                       0x12,
    HCI_EIR_DATATYPE_16BIT_SERVICE_SOLICITATION_UUIDS =                      0x14,
    HCI_EIR_DATATYPE_128BIT_SERVICE_SOLICITATION_UUIDS =                     0x15,
    HCI_EIR_DATATYPE_SERVICE_DATA =                                          0x16,
    HCI_EIR_DATATYPE_MANUFACTURER_SPECIFIC_DATA =                            0xFF
} HCI_EIR_datatype_t;

#define ADV_RSP_PACKET_MAX        31

static inline u8 make_eir_packet_val(u8 *buf, u16 offset, u8 data_type, u32 val, u8 val_size)
{
    if (ADV_RSP_PACKET_MAX - offset < val_size + 2) {
        return offset + val_size + 2;
    }

    buf[0] = val_size + 1;
    buf[1] = data_type;
    memcpy(buf + 2, &val, val_size);
    return val_size + 2;
}

static inline u8 make_eir_packet_data(u8 *buf, u16 offset, u8 data_type, u8 *data, u8 data_len)
{
    if (ADV_RSP_PACKET_MAX - offset < data_len + 2) {
        return offset + data_len + 2;
    }

    buf[0] = data_len + 1;
    buf[1] = data_type;
    memcpy(buf + 2, data, data_len);
    return data_len + 2;
}
#if 0
#include "typedef.h"
#include <stdint.h>

//--------------------------------------------


struct conn_param_t {
    u16 interval;
    u16 latency;
    u16 timeout;
};

// #define NOTIFY_TYPE           1
// #define INDICATION_TYPE       2
// Minimum/default MTU



typedef enum {
    BLE_ST_NULL = 0,
    BLE_ST_INIT_OK, //init ok
    BLE_ST_IDLE,
    BLE_ST_ADV,
    BLE_ST_CONNECT,
    BLE_ST_SEND_DISCONN,
    BLE_ST_NOTIFY_IDICATE,//server ok
    BLE_ST_SCAN,
    BLE_ST_SEARCH_COMPLETE,//client ok
} ble_state_e;


enum {
    APP_BLE_NO_ERROR = 0,
    APP_BLE_BUFF_ERROR = 1,
    APP_BLE_BUFF_FULL = 2,
    APP_BLE_OPERATION_ERROR = 3,
    APP_BLE_IS_DISCONN =  4,
};

struct ble_server_operation_t {
    int(*adv_enable)(void *priv, u32 enable);
    int(*disconnect)(void *priv);
    int(*get_buffer_vaild)(void *priv);
    int(*send_data)(void *priv, void *buf, u16 len);
    int(*regist_wakeup_send)(void *priv, void *cbk);
    int(*regist_recieve_cbk)(void *priv, void *cbk);
    int(*regist_state_cbk)(void *priv, void *cbk);
};
void ble_get_server_operation_table(struct ble_server_operation_t **interface_pt);


struct ble_client_operation_t {
    int(*scan_enable)(void *priv, u32 enable);
    int(*disconnect)(void *priv);
    int(*get_buffer_vaild)(void *priv);
    int(*write_data)(void *priv, void *buf, u16 len);
    int(*read_do)(void *priv);
    int(*regist_wakeup_send)(void *priv, void *cbk);
    int(*regist_recieve_cbk)(void *priv, void *cbk);
    int(*regist_state_cbk)(void *priv, void *cbk);
};
void ble_get_client_operation_table(struct ble_client_operation_t **interface_pt);


extern void le_l2cap_register_packet_handler(void (*handler)(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));
#endif
#endif



