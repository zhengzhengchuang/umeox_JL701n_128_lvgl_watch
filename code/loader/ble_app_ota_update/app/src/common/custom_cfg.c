#include "lib_include.h"
#include "custom_cfg.h"
#include "asm/uart.h"
#include "list.h"
#include "norflash.h"

#define CUSTOM_CFG_DEBUG_EN     1
#if CUSTOM_CFG_DEBUG_EN
#define cfg_puts    puts
#define cfg_printf  printf
#define cfg_printf_buf printf_buf
#else
#define cfg_puts(...)
#define cfg_printf(...)
#define cfg_printf_buf(...)
#endif

//#define EXIF_START_ADDR		(0xee000L)
//#define EXIF_LEN			(0x1000L)
extern void norflash_read_dec(u32 isdec, void *buf, u32 addr, u32 len, u32 init);
extern u16 chip_crc16(void *ptr, u32  len); //CRC校验
static u32 exif_addr;

typedef struct _adv_data_t {
    u16 crc;
    u16 len;
    u8 data[31];
} adv_data_cfg_t;

typedef struct _ble_name_t {
    u16 crc;
    u16 len;
    u8 data[31 - 2];
} ble_name_t;

typedef struct _bt_name_t {
    u16 crc;
    u16 len;
    u8  data[31];
} bt_name_t;

typedef struct _bt_pin_code_t {
    u16 crc;
    u16 len;
    char data[4];
} bt_pin_code_t;

typedef struct UPDATE_FILE_ID_CFG {
    u16 crc;
    u16 len;
    update_file_id_t data;
} update_file_id_cfg_t;

typedef struct LOW_POWER_VOLTAGE {
    u16 crc;
    u16 len;
    u8 data[2];
} low_power_voltage_t;

typedef struct _bt_addr_cfg_t {
    u16 crc;
    u16 len;
    u8 data[6];
} bt_addr_cfg_t;

typedef struct _gatt_profile_cfg_t {
    u16 crc;
    u16 len;
    u8 data[512 + 256];
} gatt_profile_cfg_t;

typedef struct _reset_io_info_cfg_t {
    u16 crc;
    u16 len;
    u8 data[1];
} reset_io_info_cfg_t;

typedef struct _polit_lamp_io_info_cfg_t {
    u16 crc;
    u16 len;
    u8 data[4];
} polit_lamp_io_info_cfg_t;

typedef struct _link_key_info_cfg_t {
    u16 crc;
    u16 len;
    u8 data[16];
} link_key_info_cfg_t;

typedef struct _power_io_on_off_cfg_t {
    u16 crc;
    u16 len;
    u8 data[6];
} power_io_on_off_cfg_t;

#if VER_INFO_EXT_CONUNT
typedef struct _ver_info_ext_cfg_t {
    u16 crc;
    u16 len;
    u8 data[VER_INFO_EXT_MAX_LEN];
} ver_info_ext_cfg_t;
#endif

typedef struct _last_device_connect_linkkey_cfg_t {
    u16 crc;
    u16 len;
    u8 data[16];
} last_device_connect_linkkey_cfg_t;

typedef struct _read_write_uuid_cfg_t {
    u16 crc;
    u16 len;
    u8 data[4];
} read_write_uuid_cfg_t;

typedef struct _custom_ver_info_cfg_t {
    u16 crc;
    u16 len;
    u8 data[16];
} custom_ver_info_cfg_t;

typedef struct _pid_vid_cfg_t {
    u16 crc;
    u16 len;
    u8 data[4];
} pid_vid_cfg_t;


typedef struct _md5_cfg_t {
    u16 crc;
    u16 len;
    u8 data[32];
} md5_cfg_t;

typedef struct _sdk_type_cfg_t {
    u16 crc;
    u16 len;
    u8 data;
} sdk_type_cfg_t;

typedef struct _ex_cfg_t {
    adv_data_cfg_t adv_data_cfg;
    adv_data_cfg_t scan_rsp_cfg;
    ble_name_t ble_name_cfg;
    bt_name_t bt_name_cfg;
    bt_pin_code_t pin_code_cfg;
    update_file_id_cfg_t local_ver_cfg;
    low_power_voltage_t low_power_vol_cfg;
    bt_addr_cfg_t edr_addr_cfg;
    bt_addr_cfg_t ble_addr_cfg;
    gatt_profile_cfg_t gatt_profile_cfg;
    reset_io_info_cfg_t local_reset_io_cfg;
    polit_lamp_io_info_cfg_t local_polit_lamp_io_cfg;
    link_key_info_cfg_t local_link_key_cfg;
    power_io_on_off_cfg_t power_io_on_off_cfg;
    last_device_connect_linkkey_cfg_t last_device_connect_linkkey_cfg;
    read_write_uuid_cfg_t ble_reset_io_info_cfg;
#if VER_INFO_EXT_CONUNT
    ver_info_ext_cfg_t ver_info_authkey_cfg;
    ver_info_ext_cfg_t ver_info_procode_cfg;
#endif
    pid_vid_cfg_t       pid_vid_cfg;
    md5_cfg_t           md5_cfg;
    sdk_type_cfg_t      sdk_type_cfg;
} ex_cfg_t;

typedef struct BT_ADDR_T {
    u16 crc;
    u8 data[6];
} bt_addr_t;


typedef struct _flash_cfg_read_parm_t {
    u32 cfg_addr;
    u16 cfg_len;
    u8 cfg_crc_offset;
    u8 cfg_len_offset;
    u8 cfg_data_offset;

    u8 cfg_buf_len;
    u8 *cfg_buf;
} flash_cfg_read_param_t;

u8 temp_buf[0x200];

static u32 ex_cfg_read_data_from_flash(flash_cfg_read_param_t *param)
{
    u32 err = EX_CFG_ERR_NONE;

    if (param->cfg_buf_len + 4 > sizeof(temp_buf)) {
        err = EX_CFG_LEN_OVER_TEMP_BUF_LEN_ERR;
        goto _EX_CFG_ERR_RET;
    }

    memset(temp_buf, 0x00, sizeof(temp_buf));

    if (sizeof(temp_buf) < param->cfg_len) {
        err = EX_CFG_TMP_BUF_ERR;
        goto _EX_CFG_ERR_RET;
    }


    norflash_read(temp_buf, param->cfg_addr, param->cfg_len);
    //norflash_read_dec(0, temp_buf, param->cfg_addr, param->cfg_len, 0);

    if (*(u16 *)(temp_buf + param->cfg_crc_offset) != chip_crc16((u8 *)(temp_buf + param->cfg_data_offset), *(u16 *)(temp_buf + param->cfg_len_offset))) {
        err = EX_CFG_CRC_ERR;
        goto _EX_CFG_ERR_RET;
    }


    memset(param->cfg_buf, 0, param->cfg_buf_len);
    memcpy(param->cfg_buf, temp_buf + param->cfg_data_offset, param->cfg_buf_len);

_EX_CFG_ERR_RET:
    return err;
}

#define MEMBER_OFFSET_OF_STRUCT(type,member)	((u32)&(((type *)0)->member))

static ex_cfg_t ex_cfg;
#define ex_cfg_hdl  (&ex_cfg)

u32 get_ex_cfg_info(u32 offset_addr, u16 temp_data_len, u8 *data, u16 data_len)
{
    printf("offet_addr:%d\n data_len:%d\n", offset_addr, data_len);
    if (data_len + 4 > sizeof(temp_buf)) {
        return EX_CFG_LEN_OVER_TEMP_BUF_LEN_ERR;
    }
    if (sizeof(temp_buf) < temp_data_len) {
        return EX_CFG_TMP_BUF_ERR;
    }
    memset(temp_buf, 0x00, sizeof(temp_buf));
    printf("exif_addr:%x offset_addr:%x\n", exif_addr, offset_addr);
    norflash_read(temp_buf, exif_addr + offset_addr, temp_data_len);
    /* norflash_read_dec(0, temp_buf, exif_addr + offset_addr, temp_data_len, 0); */
    printf("temp_buf:");
    put_buf(temp_buf, temp_data_len);
    u16 crc = *((u16 *)temp_buf);
    u16 len = *((u16 *)(temp_buf + 2));
    u8 *cfg_data = temp_buf + 4;
    if ((len > sizeof(temp_buf)) || (crc != chip_crc16(cfg_data, len))) {
        return EX_CFG_CRC_ERR;
    }
    memset(data, 0x00, data_len);
    memcpy(data, cfg_data, data_len > len ? len : data_len);
    return EX_CFG_ERR_NONE;
}

u32 ex_cfg_get_content_api(u8 cfg_index, u8 *data, u16 len)
{
    u32 err = EX_CFG_ERR_NONE;
    switch (cfg_index) {
    case CFG_ITEM_ADV_IND:
#if 1
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, adv_data_cfg), sizeof(adv_data_cfg_t), data, len);
#else
        offset_addr = (u8 *) & (ex_cfg_hdl->adv_data_cfg) - (u8 *)ex_cfg_hdl;
        flash_cfg_read_param.cfg_addr = exif_addr + offset_addr;
        flash_cfg_read_param.cfg_len = sizeof(adv_data_cfg_t);
        flash_cfg_read_param.cfg_data_offset = ((adv_data_cfg_t *)0)->data;
        flash_cfg_read_param.cfg_crc_offset = ((adv_data_cfg_t *)0)->crc;
        flash_cfg_read_param.cfg_len_offset = ((adv_data_cfg_t *)0)->len;
        err = ex_cfg_read_data_from_flash(&flash_cfg_read_param);
#endif
        break;

    case CFG_ITEM_SCAN_RSP:
#if 1
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, scan_rsp_cfg), sizeof(adv_data_cfg_t), data, len);
#else
        offset_addr = (u8 *) & (ex_cfg_hdl->scan_rsp_cfg) - (u8 *)ex_cfg_hdl;
        flash_cfg_read_param.cfg_addr = exif_addr + offset_addr;
        flash_cfg_read_param.cfg_len = sizeof(adv_data_cfg_t);
        flash_cfg_read_param.cfg_data_offset = ((adv_data_cfg_t *)0)->data;
        flash_cfg_read_param.cfg_crc_offset = ((adv_data_cfg_t *)0)->crc;
        flash_cfg_read_param.cfg_len_offset = ((adv_data_cfg_t *)0)->len;
        err = ex_cfg_read_data_from_flash(&flash_cfg_read_param);
#endif
        break;

    case CFG_ITEM_BLE_NAME:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, ble_name_cfg), sizeof(ble_name_t), data, len);
        break;

    case CFG_ITEM_PIN_CODE :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, pin_code_cfg), sizeof(bt_pin_code_t),  data, len);
        break;

    case CFG_ITEM_VER_INFO:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, local_ver_cfg), sizeof(update_file_id_cfg_t), data, len);
        break;

    case CFG_ITEM_LOWPOWER_VOLTAGE:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, low_power_vol_cfg), sizeof(low_power_voltage_t), data, len);
        break;

    case CFG_ITEM_EDR_ADDR:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, edr_addr_cfg), sizeof(bt_addr_cfg_t), data, len);
        break;

    case CFG_ITEM_BLE_ADDR:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, ble_addr_cfg), sizeof(bt_addr_cfg_t), data, len);
        break;

    case CFG_ITEM_RESET_IO_INFO :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, local_reset_io_cfg), sizeof(reset_io_info_cfg_t), data, len);
        break;
    case CFG_ITEM_PILOT_LAMP_IO_INFO :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, local_polit_lamp_io_cfg), sizeof(polit_lamp_io_info_cfg_t), data, len);
        break;
    case CFG_ITEM_LINK_KEY_INFO :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, local_link_key_cfg), sizeof(link_key_info_cfg_t), data, len);
        break;
    case CFG_ITEM_GATT_PROFILE :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, gatt_profile_cfg), sizeof(gatt_profile_cfg_t), data, len);
        break;
    case CFG_ITEM_POWER_IO_OFF :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, power_io_on_off_cfg), sizeof(power_io_on_off_cfg_t), data, len);
        break;
#if VER_INFO_EXT_CONUNT
    case CFG_ITEM_VER_INFO_AUTHKEY :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, ver_info_authkey_cfg), sizeof(ver_info_ext_cfg_t), data, len);
        break;
    case CFG_ITEM_VER_INFO_PROCODE :
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, ver_info_procode_cfg), sizeof(ver_info_ext_cfg_t), data, len);
        break;
#endif
    case CFG_ITEM_BLE_READ_WRITE_UUID_INFO:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, ble_reset_io_info_cfg), sizeof(read_write_uuid_cfg_t), data, len);
        break;
    case CFG_ITEM_PVID:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, pid_vid_cfg), sizeof(pid_vid_cfg_t), data, len);
        break;
    case CFG_ITEM_MD5:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, md5_cfg), sizeof(md5_cfg_t), data, len);
        break;
    case CFG_ITEM_SDK_TYPE:
        err = get_ex_cfg_info(MEMBER_OFFSET_OF_STRUCT(ex_cfg_t, sdk_type_cfg), sizeof(sdk_type_cfg_t), data, len);
        break;
    default:
        err = EX_CFG_INDEX_ERR;
        break;
    }
    if (EX_CFG_ERR_NONE != err) {
        cfg_printf("READ INDEX:%x CFG ERR:%x\n", cfg_index, err);
    }
    return err;
}

//static update_file_id_t device_id_info;
u16 ex_cfg_get_local_version_info(u8 type)
{
    update_file_id_t device_id_info;
    u32 err = ex_cfg_get_content_api(CFG_ITEM_VER_INFO, (u8 *)&device_id_info, sizeof(device_id_info));
    if (EX_CFG_ERR_NONE == err) {
        switch (type) {
        case GET_VID_FROM_CFG:
            return ((u16)device_id_info.vid[0] << 8 | (u16)device_id_info.vid[1]);
        case GET_PID_FROM_CFG:
            return ((u16)device_id_info.pid[0] << 8 | (u16)device_id_info.pid[1]);
        case GET_VER_FROM_CFG:
            return ((u16)device_id_info.ver[0] << 8 | (u16)device_id_info.ver[1]);
        }
    }
    return err;
}
#if 0
u16 ex_cfg_get_local_version_info(void)
{
    ex_cfg_get_content_api(CFG_ITEM_VER_INFO, (u8 *)&device_id_info, sizeof(device_id_info));

    return ((u16)device_id_info.ver[0] << 8 | (u16)device_id_info.ver[1]);
}


u16 ex_cfg_get_local_pid_info(void)
{
    ex_cfg_get_content_api(CFG_ITEM_VER_INFO, (u8 *)&device_id_info, sizeof(device_id_info));

    return ((u16)device_id_info.pid[0] << 8 | (u16)device_id_info.pid[1]);

}

u16 ex_cfg_get_local_vid_info(void)
{
    ex_cfg_get_content_api(CFG_ITEM_VER_INFO, (u8 *)&device_id_info, sizeof(device_id_info));

    return ((u16)device_id_info.vid[0] << 8 | (u16)device_id_info.vid[1]);
}
#endif

void ex_cfg_start_addr_set(u32 addr)
{
    exif_addr = addr;
    cfg_printf("exif_start_addr:%x\n", exif_addr);
}

u32 ex_cfg_addr_get(void)
{
    return exif_addr;
}

