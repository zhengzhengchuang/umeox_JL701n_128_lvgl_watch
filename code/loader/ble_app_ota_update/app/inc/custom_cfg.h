#ifndef _CUSTOM_CFG_
#define _CUSTOM_CFG_

#define VER_INFO_EXT_CONUNT     2
#define VER_INFO_EXT_MAX_LEN    24

//是否支持鸿巨昌客户HID方案的配置
#define CUSTOM_CFG_SUPPORT_HJC_HID_CASE_EN	1

enum {
    EX_CFG_ERR_NONE = 0,
    EX_CFG_INDEX_ERR,
    EX_CFG_LEN_OVER_TEMP_BUF_LEN_ERR,
    EX_CFG_CRC_ERR,
    EX_CFG_TMP_BUF_ERR,
};

enum {
    CFG_ITEM_ADV_IND = 0,
    CFG_ITEM_SCAN_RSP,
    CFG_ITEM_BLE_NAME,
    CFG_ITEM_BT_ADDR,

    CFG_ITEM_BT_NAME,
    CFG_ITEM_PIN_CODE,
    CFG_ITEM_VER_INFO,
    CFG_ITEM_LOWPOWER_VOLTAGE,

    CFG_ITEM_EDR_ADDR,
    CFG_ITEM_BLE_ADDR,
    CFG_ITEM_GATT_PROFILE,
    CFG_ITEM_RESET_IO_INFO,

    CFG_ITEM_PILOT_LAMP_IO_INFO,
    CFG_ITEM_LINK_KEY_INFO,
    CFG_ITEM_POWER_IO_OFF,
    CFG_ITEM_LAST_DEVICE_CONNECT_LINKKEY_INFO,

    CFG_ITEM_BLE_READ_WRITE_UUID_INFO = 16,
#if VER_INFO_EXT_CONUNT
    CFG_ITEM_VER_INFO_AUTHKEY,
    CFG_ITEM_VER_INFO_PROCODE,
#endif
    CFG_ITEM_PVID,
    CFG_ITEM_MD5,
    CFG_ITEM_SDK_TYPE,
};

typedef struct _update_file_id {
    u8 vid[2];
    u8 pid[2];
    u8 ver[2];
    u8 len[4];
    u8 reserve[4];
    u8 crc[2];
} update_file_id_t;

typedef struct _update_file_ext_id {
    update_file_id_t update_file_id_info;
#if VER_INFO_EXT_CONUNT
    u8 ext[VER_INFO_EXT_CONUNT * (VER_INFO_EXT_MAX_LEN + 1)];
#endif
} update_file_ext_id_t;

enum {
    GET_VID_FROM_CFG = 0,
    GET_PID_FROM_CFG,
    GET_VER_FROM_CFG,
};

void ex_cfg_start_addr_set(u32 addr);
u32 ex_cfg_get_content_api(u8 cfg_index, u8 *data, u16 len);
u16 ex_cfg_get_local_version_info(u8 type);
#if 0
u16 ex_cfg_get_local_version_info(void);
u16 ex_cfg_get_local_pid_info(void);
u16 ex_cfg_get_local_vid_info(void);
#endif
#endif

