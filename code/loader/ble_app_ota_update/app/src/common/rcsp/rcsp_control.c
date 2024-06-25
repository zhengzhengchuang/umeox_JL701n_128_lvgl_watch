#include "lib_include.h"
#include "rcsp_control.h"
#include "JL_rcsp_protocol.h"
#include "JL_rcsp_api.h"
#include "custom_cfg.h"
#include "update_main.h"
#include "msg.h"
#include "ltv_format.h"
#include "lbuf.h"
#include "sys_timer.h"
#include "asm/hwi.h"
#include "le_common.h"
//#define AT_JL_RCSP_CODE		AT(.jl_rcsp_code)

#define LOG_TAG     "[rcsp_server]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
//#define LOG_DUMP_ENABLE
#include "debug.h"


struct JL_AI_VAR jl_ai_var;

#define  __this (&jl_ai_var)

struct _dev_version {
    u16 _sw_ver2: 4; //software l version
    u16 _sw_ver1: 4; //software m version
    u16 _sw_ver0: 4; //software h version
    u16 _hw_ver:  4; //hardware version
};
#pragma pack()

static struct _dev_version rcsp_dev_ver;

u8  JL_rcsp_get_pvid_flag(void);
void lbuf_free_btctrler(void *lbuf);

static u8 get_rcsp_platform(void)
{
#if (TULING_AI_EN == 1)
    return TULING;
#endif
#if (DEEPBRAN_AI_EN == 1)
    return DEEPBRAIN;
#endif
    return 0;
}


static void set_rcsp_dev_version(u8 hw, u8 sw_h, u8 sw_m, u8 sw_l)
{
    rcsp_dev_ver._hw_ver  = hw;
    rcsp_dev_ver._sw_ver0 = sw_h;
    rcsp_dev_ver._sw_ver1 = sw_m;
    rcsp_dev_ver._sw_ver2 = sw_l;
}

static u8 add_one_attr(u8 *buf, u16 max_len, u8 offset, u8 type, u8 *data, u8 size)
{
    if (offset + size + 2 > max_len) {
        log_info("\n\nadd attr err!\n\n");
        return 0;
    }

    buf[offset] = size + 1;
    buf[offset + 1] = type ;
    memcpy(&buf[offset + 2], data, size);
    return size + 2;
}

static void (*fw_update_block_handle)(u8 *buf, u16 len) = NULL;
static u8(*device_refresh_fw_status_handle)(void) = NULL;
static void (*rcsp_state_change_callback)(void *priv, u8 state) = NULL;

static void register_rcsp_state_change_handle(void (*handle)(void *priv, u8 state))
{
    rcsp_state_change_callback = handle;
}

static void register_device_refresh_fw_status_handle(u8(*handle)(void))
{
    device_refresh_fw_status_handle = handle;
}

static void register_receive_fw_update_block_handle(void (*handle)(u8 *buf, u16 len))
{
    fw_update_block_handle = handle;
}

static u32 JL_opcode_get_target_info(void *priv, u8 OpCode, u8 OpCode_SN, u32 mask)
{
    u8 buf[256];
    u8 read_buf[32];
    u8 offset = 0;
    u32 err = 0;

    printf("FEATURE MASK : %x\n", mask);

    u32 ret = 0;

    if (mask & BIT(ATTR_TYPE_SDK_TYPE)) {
        log_info("ATTR_TYPE_SDK_TYPE\n");
        u8 sdk_type = RCSP_SDK_TYPE;
        //ex_cfg_get_content_api(CFG_ITEM_SDK_TYPE, &sdk_type, 1);
        offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_SDK_TYPE, &sdk_type, 1);
        printf(">>>sdk_type:0x%x", sdk_type);
    }


    if (JL_rcsp_get_pvid_flag()) {
        if (mask & BIT(ATTR_TYPE_DEV_VID_PID)) {
            if (ex_cfg_get_content_api(CFG_ITEM_PVID, read_buf, 4) == EX_CFG_ERR_NONE) {
                printf("ATTR_TYPE_DEV_VID_PID:");
                put_buf(read_buf, 4);
                u16 pvid[2] = {0};
                pvid[0] = READ_BIG_U16(read_buf);
                pvid[1] = READ_BIG_U16(read_buf + 2);
                offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_DEV_VID_PID, pvid, 4);
            }
        }

        if (mask & BIT(ATTR_TYPE_DEV_AUTHKEY)) {
            if (ex_cfg_get_content_api(CFG_ITEM_VER_INFO_AUTHKEY, read_buf, 24) == EX_CFG_ERR_NONE) {
                printf("ATTR_TYPE_DEV_AUTHKEY:");
                put_buf(read_buf, 24);
                offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_DEV_AUTHKEY, read_buf, strlen((char *)read_buf));
            }
        }

        if (mask & BIT(ATTR_TYPE_DEV_PROCODE)) {
            if (ex_cfg_get_content_api(CFG_ITEM_VER_INFO_PROCODE, read_buf, 24) == EX_CFG_ERR_NONE) {
                printf("ATTR_TYPE_DEV_PROCODE:");
                put_buf(read_buf, 24);
                offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_DEV_PROCODE, read_buf, strlen((char *)read_buf));
            }
        }

        if (mask & BIT(ATTR_TYPE_MD5_GAME_SUPPORT)) {
            u8 cmp_buf[32] = {0};
            u8 md5_support = 0;
            if (ex_cfg_get_content_api(CFG_ITEM_MD5, read_buf, 32) == EX_CFG_ERR_NONE) {
                printf("ATTR_TYPE_MD5_GAME_SUPPORT:");
                put_buf(read_buf, 32);
                if (memcmp(cmp_buf, read_buf, 32) != 0) {           //不全为0说明md5存在？
                    md5_support = 1;
                }
                offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_MD5_GAME_SUPPORT, &md5_support, 1);
            }
        }
    }

    if (mask & BIT(ATTR_TYPE_UPDATE_STATUS)) {
        u8 update_status_value = 0x1;
        log_info("ATTR_TYPE_UPDATE_STATUS:%d\n", update_status_value);
        offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_UPDATE_STATUS, (u8 *)&update_status_value, sizeof(update_status_value));
    }

    if (mask & BIT(ATTR_TYPE_EDR_ADDR)) {
        u8 edr_addr[6];
        u8 edr_send_addr[6];
        ex_cfg_get_content_api(CFG_ITEM_EDR_ADDR, edr_addr, 6);
        for (u8 i = 0; i < sizeof(edr_addr); i ++) {
            edr_send_addr[sizeof(edr_addr) - 1 - i] = edr_addr[i];
        }
        offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_EDR_ADDR, edr_send_addr, sizeof(edr_addr));

    }

#ifdef APP_USE_MOR_SPACE
    if (mask & BIT(ATTR_TYPE_DEV_MAX_MTU)) {
        u16 rx_max_mtu = JL_packet_get_rx_max_mtu();
        u16 tx_max_mtu = JL_packet_get_tx_max_mtu();
        u8 t_buf[4];
        t_buf[0] = (tx_max_mtu >> 8) & 0xFF;
        t_buf[1] = tx_max_mtu & 0xFF;
        t_buf[2] = (rx_max_mtu >> 8) & 0xFF;
        t_buf[3] = rx_max_mtu & 0xFF;
        offset += add_one_attr(buf, sizeof(buf), offset, ATTR_TYPE_DEV_MAX_MTU, t_buf, 4);
    }
#endif

    log_info_hexdump(buf, offset);

    ret = JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, buf, offset);

    return ret;
}

#define DEV_UPDATE_FILE_INFO_OFFSET	0x40
#define DEV_UPDATE_FILE_INFO_LEN	(0x10 + VER_INFO_EXT_CONUNT * (VER_INFO_EXT_MAX_LEN + 1))

static update_file_ext_id_t update_file_id_info = {
    .update_file_id_info.ver[0] = 0xff,
    .update_file_id_info.ver[1] = 0xff,
};
typedef struct _update_mode_t {
    u8 opcode;
    u8 opcode_sn;

    u16 block_len;
    u32	block_addr;

    u8 last_block_data_status;
} update_mode_t;

static update_mode_t update_record_info;

void JL_controller_save_curr_cmd_para(u8 OpCode, u8 OpCode_SN)
{
    update_record_info.opcode = OpCode;
    update_record_info.opcode_sn = OpCode_SN;
}

void JL_controller_get_curr_cmd_para(u8 *OpCode, u8 *OpCode_SN)
{
    *OpCode = update_record_info.opcode;
    *OpCode_SN = update_record_info.opcode_sn;
}

static rsp[1];
static u8 switch_flag = 0;
//phone send cmd to firmware need respond
extern u8 jl_update_get_reboot_sta(void);
static void JL_rcsp_cmd_resp(void *priv, u8 OpCode, u8 OpCode_SN, u8 *data, u16 len)
{
    log_info("JL_ble_cmd_resp:%x %x\n", OpCode, OpCode_SN);

    u16 ret = 0;
    u8 read_buf[32] = {0};
    u8 status = JL_PRO_STATUS_SUCCESS;
    u32 mask_code;

    switch (OpCode) {
    case JL_OPCODE_GET_TARGET_FEATURE:
        printf("JL_OPCODE_GET_TARGET_FEATURE\n");
        //JL_opcode_get_target_info(priv, OpCode, OpCode_SN, data, len);
        mask_code = READ_BIG_U32(data);
        log_info("mask_code:%X\n", mask_code);
        task_post_msg(
            NULL,
            4,
            MSG_JL_GET_DEV_INFO,
            OpCode,
            OpCode_SN,
            mask_code
        );
        break;

    case JL_OPCODE_GET_DEVICE_UPDATE_FILE_INFO_OFFSET:
        if (0 == len) {
            //log_info("--GET_DEVICE_UPDATE_FILE_INFO_OFFSET\n");
            task_post_msg(NULL,
                          3,
                          MSG_JL_GET_DEV_UPDATE_FILE_INFO_OFFSET,
                          OpCode,
                          OpCode_SN);
        } else {
            //log_info("--GET_DEVICE_UPDATE_FILE_INFO_OFFSET ERR\n");
            status = JL_PRO_STATUS_PARAM_ERR;

        }
        break;

    case JL_OPCODE_INQUIRE_DEVICE_IF_CAN_UPDATE:
        if (DEV_UPDATE_FILE_INFO_LEN == len) {
            //judge if info_err,can update,low voltage;
            memcpy((u8 *)&update_file_id_info, data, DEV_UPDATE_FILE_INFO_LEN);
            printf_buf(data, DEV_UPDATE_FILE_INFO_LEN);
            task_post_msg(NULL,
                          4,
                          MSG_JL_INQUIRE_DEVEICE_IF_CAN_UPDATE,
                          OpCode,
                          OpCode_SN,
                          0x00);
        } else {
            //log_info("INQURE_DEVICE_IF_CAN_UPDATE ERR\n");
            status = JL_PRO_STATUS_PARAM_ERR;
        }
        break;

    case JL_OPCODE_ENTER_UPDATE_MODE:
        if (0x00 == len) {
            //JL_controller_save_curr_cmd_para(OpCode, OpCode_SN);
            //xm_controller_resp_update_data_request(0x00, 0xe00, 0x100);
            //u8 rsp[1];
            rsp[0] = 0;//0:succ 1:failed
            task_post_msg(NULL,
                          3,
                          MSG_JL_ENTER_UPDATE_MODE, OpCode, OpCode_SN);
            //JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, rsp, sizeof(rsp));
        } else {
            //log_info("ENTER UPDATE_MODE ERR\n");
            status = JL_PRO_STATUS_PARAM_ERR;
        }
        break;

    case JL_OPCODE_EXIT_UPDATE_MODE:
        if (0x00 == len) {
            //u8 rsp[1];
            rsp[0] = 1; //0:succ 1:failed
            //JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, rsp, sizeof(rsp));
            task_post_msg(NULL, 3, MSG_JL_SUCCESS_STATUS_HANDLE, OpCode, OpCode_SN);
        } else {
            //log_info("EXIT UPDATE_MODE ERR\n");
            status = JL_PRO_STATUS_PARAM_ERR;
        }
        break;

    case JL_OPCODE_GET_DEVICE_REFRESH_FW_STATUS:
        printf("JL_OPCODE_GET_DEVICE_REFRESH_FW_STATUS");
        JL_controller_save_curr_cmd_para(OpCode, OpCode_SN);
        if (0x00 == len) {
            if (device_refresh_fw_status_handle) {
                u8 update_status = device_refresh_fw_status_handle();
            } else {
                status = JL_PRO_STATUS_FAIL;
            }
        } else {
            status = JL_PRO_STATUS_PARAM_ERR;
        }
        break;

    case JL_OPCODE_SET_DEVICE_REBOOT:
        if (0x01 == len) {
            log_info("set_dev_reboot:%x\n", data[0]);
            //u8 rsp[1];
            rsp[0] = jl_update_get_reboot_sta();
            //JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, rsp, sizeof(rsp));
            task_post_msg(NULL, 3, MSG_JL_SUCCESS_STATUS_HANDLE, OpCode, OpCode_SN);
        } else {
            status = JL_PRO_STATUS_PARAM_ERR;
        }
        break;
#if 1//SPP_FORCE_UPDATE_EN
    case JL_OPCODE_NOTIFY_TO_SWITCH_COMMUNICATION_WAY:
        if (len) {
            //JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, NULL, 0);
            switch_flag = 1;
            task_post_msg(NULL, 3, MSG_JL_SWITCH_CH_SPP, OpCode, OpCode_SN);
        } else {
            status = JL_PRO_STATUS_PARAM_ERR;
        }
        break;
#endif

    case JL_OPCODE_GET_MD5:
        log_info("JL_OPCODE_GET_MD5\n");
        ex_cfg_get_content_api(CFG_ITEM_MD5, read_buf, 32);
        JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, read_buf, 32);
        break;

    default:
        JL_CMD_response_send(OpCode, JL_PRO_STATUS_UNKOWN_CMD, OpCode_SN, NULL, 0);
        break;
    }

    if (JL_PRO_STATUS_SUCCESS != status) {
        log_info("CMD_resp err! [%d %s, %d]\n", status, __FUNCTION__, __LINE__);
        //JL_CMD_response_send(OpCode, status, OpCode_SN, NULL, 0);
        task_post_msg(NULL, 4, MSG_JL_ERR_STATUS_HANDLE, OpCode, status, OpCode_SN);
    }
}

//phone send cmd to firmware not need respond
static void JL_rcsp_cmd_no_resp(void *priv, u8 OpCode, u8 *data, u16 len)
{
    log_info("JL_ble_cmd_no_resp\n");
    switch (OpCode) {
    case 0:
        break;
    case JL_OPCODE_CUSTOMER_USER:
#if JL_RCSP_USER_COUSTOM_EN
        log_info("JL_OPCODE_CUSTOMER_USER\n");
        if (__this->rcsp_user && __this->rcsp_user->recv) {
            __this->rcsp_user->recv(data, len);
        }
#endif
        break;
    default:
        break;
    }
}
//phone respone firmware cmd
static void JL_rcsp_cmd_recieve_resp(void *priv, u8 OpCode, u8 status, u8 *data, u16 len)
{
    printf("rec resp:%x\n", OpCode);
    switch (OpCode) {
    case JL_OPCODE_SEND_FW_UPDATE_BLOCK:
        if (fw_update_block_handle) {
            fw_update_block_handle(data, len);
        } else {
            log_error("#UPDATE DATA HANDLE NOT REG\n");
        }
        break;
    case JL_OPCODE_NOTIFY_UPDATE_CONENT_SIZE:
        //log_info("RX notify_update_content_size; rsp\n");
        break;
    default:
        break;
    }

}
//wait resp timout
static u8 JL_rcsp_wait_resp_timeout(void *priv, u8 OpCode, u8 counter)
{
    log_info("JL_rcsp_wait_resp_timeout\n");

    return 0;
}


static void JL_rcsp_control_init(void);
static void JL_rcsp_control_exit(void);
///**************************************************************************************///
///************     rcsp ble                                                   **********///
///**************************************************************************************///
static void JL_ble_status_callback(void *priv, u8 status)
{
    log_info("JL_ble_status_callback==================== %d\n", status);
    __this->JL_ble_status = status;
    switch (status) {
    case BLE_ST_IDLE:
        JL_rcsp_control_exit();
        if (rcsp_state_change_callback) {
            rcsp_state_change_callback(NULL, DEV_CONN_STATE_DISCONNECTED);
        }
        break;
    case BLE_ST_ADV:
        break;
    case BLE_ST_CONNECT:
        JL_rcsp_control_init();
        if (rcsp_state_change_callback) {
            rcsp_state_change_callback(NULL, DEV_CONN_STATE_CONNECTED);
        }
        break;
    case BLE_ST_SEND_DISCONN:
        break;
    case BLE_ST_NOTIFY_IDICATE:
        JL_rcsp_auth_reset();
        if (rcsp_state_change_callback) {
            rcsp_state_change_callback(NULL, DEV_CONN_STATE_READY_UPDATE);
        }
        break;
    default:
        break;
    }
}


static bool JL_ble_fw_ready(void *priv)
{
    return ((__this->JL_ble_status == BLE_ST_NOTIFY_IDICATE) ? true : false);
}

static s32 JL_ble_send(void *priv, void *data, u16 len)
{
    if ((__this->rcsp_ble != NULL) && (__this->JL_ble_status == BLE_ST_NOTIFY_IDICATE)) {
        int err = __this->rcsp_ble->send_data(NULL, (u8 *)data, len);
        /* log_info("send :%d\n", len); */
        if (len < 128) {
            /* log_info_hexdump(data, len); */
        } else {
            /* log_info_hexdump(data, 128); */
        }

        if (err == 0) {
            return 0;
        } else if (err == APP_BLE_BUFF_FULL) {
            return 1;
        }
    } else {
        log_info("send err -1 !!\n");
    }

    return -1;
}

static const JL_PRO_CB JL_pro_BLE_callback = {
    .priv              = NULL,
    .fw_ready          = JL_ble_fw_ready,
    .fw_send           = JL_ble_send,
    .CMD_resp          = JL_rcsp_cmd_resp,
    .CMD_no_resp       = NULL,
    .DATA_resp         = NULL,
    .DATA_no_resp      = NULL,
    .CMD_recieve_resp  = JL_rcsp_cmd_recieve_resp,
    .DATA_recieve_resp = NULL,
    .wait_resp_timeout = JL_rcsp_wait_resp_timeout,
};

static void rcsp_ble_callback_set(void (*resume)(void), void (*recieve)(void *, void *, u16), void (*status)(void *, ble_state_e))
{
    if (__this->rcsp_ble) {
        if (__this->rcsp_ble->regist_wakeup_send) {
            __this->rcsp_ble->regist_wakeup_send(NULL, resume);
        }
        if (__this->rcsp_ble->regist_recieve_cbk) {
            __this->rcsp_ble->regist_recieve_cbk(NULL, recieve);
        }
        if (__this->rcsp_ble->regist_state_cbk) {
            __this->rcsp_ble->regist_state_cbk(NULL, status);
        }
    }
}


///**************************************************************************************///
///************     rcsp spp                                                   **********///
///**************************************************************************************///
static void JL_spp_status_callback(void *priv, u8 status)
{
    switch (status) {
    case 0:
        __this->JL_spp_status = 0;
        JL_rcsp_control_exit();
        if (rcsp_state_change_callback) {
            rcsp_state_change_callback(NULL, DEV_CONN_STATE_DISCONNECTED);
        }
        break;
    case 1:
        JL_rcsp_control_init();
        __this->JL_spp_status = 1;
        if (rcsp_state_change_callback) {
            rcsp_state_change_callback(NULL, DEV_CONN_STATE_CONNECTED);
        }
        break;
    default:
        __this->JL_spp_status = 0;
        break;
    }
}
#if 0
static tbool JL_spp_fw_ready(void *priv)
{
    return (__this->JL_spp_status ? true : false);
}

static s32 JL_spp_send(void *priv, void *data, u16 len)
{
    if (len < 128) {
        log_info("send: \n");
        log_info_hexdump(data, (u32)len);
    }
    if ((__this->rcsp_spp != NULL) && (__this->JL_spp_status == 1)) {
        u32 err = __this->rcsp_spp->send_data(NULL, (u8 *)data, len);
        if (err == 0) {
            return 0;
        } else if (err == 1) {
            return 1;
        }
    } else {
        log_info("send err -1 !!\n");
    }

    return -1;
}

static const JL_PRO_CB JL_pro_SPP_callback = {
    .priv              = NULL,
    .fw_ready          = JL_spp_fw_ready,
    .fw_send           = JL_spp_send,
    .CMD_resp          = JL_rcsp_cmd_resp,
    .DATA_resp         = NULL,
    .CMD_no_resp       = NULL,
    .DATA_no_resp      = NULL,
    .CMD_recieve_resp  = JL_rcsp_cmd_recieve_resp,
    .DATA_recieve_resp = NULL,
    .wait_resp_timeout = JL_rcsp_wait_resp_timeout,
};

static void rcsp_spp_callback_set(void (*resume)(void), void (*recieve)(void *, void *, u16), void (*status)(u8))
{
    if (__this->rcsp_spp) {
        if (__this->rcsp_spp->regist_wakeup_send) {
            __this->rcsp_spp->regist_wakeup_send(NULL, resume);
        }
        if (__this->rcsp_spp->regist_recieve_cbk) {
            __this->rcsp_spp->regist_recieve_cbk(NULL, recieve);
        }
        if (__this->rcsp_spp->regist_state_cbk) {
            __this->rcsp_spp->regist_state_cbk(NULL, status);
        }
    }
}
#endif

void jl_rcsp_msg_deal(void *hdl, int *msg);
extern void ble_update_msg_deal_handle_register(void (*hdl)(void *priv, int *msg));
static void *rcsp_update_data_read(void *priv, u8 btupdata_sw, u8 seek_type, u32 offset_addr, u16 len)
{
    u32 err;
    u8 data[4 + 2];
    WRITE_BIG_U32(data, offset_addr);
    WRITE_BIG_U16(data + 4, len);
    err = JL_CMD_send(JL_OPCODE_SEND_FW_UPDATE_BLOCK, data, sizeof(data), JL_NEED_RESPOND);

    return (void *)err;
}

static void *rcsp_notify_update_content_size(void *priv, u32 size)
{
    u32 err;

    u8 data[4];

    WRITE_BIG_U32(data, size);

    log_info("send content_size:%x\n", size);
    err = JL_CMD_send(JL_OPCODE_NOTIFY_UPDATE_CONENT_SIZE, data, sizeof(data), JL_NEED_RESPOND);

    return (void *)err;
}

static void *rcsp_dev_disconnect(void *priv)
{
    if (__this->rcsp_ble && __this->rcsp_ble->disconnect) {
        __this->rcsp_ble->disconnect(NULL);
    } else {
        log_error("NOT DISCON HANDLE\n");
    }

    return NULL;
}

static int rcsp_get_number_of_pkt_sending(void *priv)
{
    int ret = 0;
    if (__this->rcsp_ble && __this->rcsp_ble->get_packet_num_of_not_sent) {
        ret = __this->rcsp_ble->get_packet_num_of_not_sent(NULL);
    } else {
        log_error("NOT GET_NUM_OF_PKT_SENDING HANDLE\n");
    }
    return ret;
}

static u8 receieve_data_flag = 0;
u8 JL_rcsp_get_no_auth(void);
static int rcsp_receieve_data_handle(void *priv, void *buf, u16 len)
{
    /* if (!receieve_data_flag) { */
    if (!JL_rcsp_get_auth_flag() && (!JL_rcsp_get_no_auth())) {
        printf("auth...\n");
        JL_rcsp_auth_recieve(buf, len);
        return 0;
    }
    /* } */

    JL_protocol_data_recieve(priv, buf, len);

    return 0;
}

void rcsp_dev_select(u8 type)
{
    printf("%s----type\n", __func__, type);
    if (type == RCSP_BLE) {
        log_info("------RCSP_BLE-----\n");
        //rcsp_spp_callback_set(NULL, NULL, NULL);
        rcsp_ble_callback_set(JL_protocol_resume, rcsp_receieve_data_handle, JL_ble_status_callback);
        //printf_buf((u8 *)&JL_pro_BLE_callback,sizeof(JL_PRO_CBK));
        JL_protocol_dev_switch(&JL_pro_BLE_callback);
        ble_update_msg_deal_handle_register(jl_rcsp_msg_deal);
    } else {
#if 0
        log_info("------RCSP_SPP-----\n");
        rcsp_spp_callback_set(JL_protocol_resume, rcsp_receieve_data_handle, JL_spp_status_callback);
        rcsp_ble_callback_set(NULL, NULL, NULL);
        JL_protocol_dev_switch(&JL_pro_SPP_callback);
#endif
    }
    printf("%s----type end\n", __func__, type);
}

static u8 JL_controller_pool[JL_CONTROLLER_POOL_SIZE] ALIGNED(4);
static struct lbuff_head  *JL_con_ptr = NULL;

void *JL_controller_malloc(int size)
{
    void *p;
    log_info("rcsp_malloc:%d\n", size);
    if (JL_con_ptr == NULL) {
        return NULL;
    }

    p = lbuf_alloc(JL_con_ptr, size);

    if (p) {
        memset((u8 *)p, 0, size);
    } else {
        ASSERT(0, "JL_controller malloc null");
    }

    return p;
}

void *JL_controller_calloc(int count, int size)
{
    return JL_controller_malloc(count * size);
}

void JL_controller_free(void *p)
{
    lbuf_free(p);
}

static void JL_controller_mem_init(void)
{
    JL_con_ptr = lbuf_init(JL_controller_pool, sizeof(JL_controller_pool), 4, 0);
}

/*-----------------ATTR DEFINITION---------------------*/
//static u8 jl_rcsp_ver[2];
static u8 attr_get_rcsp_ver_size(void)
{
    return sizeof(rcsp_dev_ver);
}

static u8 *attr_get_rcsp_ver_ptr(void)
{
    //jl_rcsp_ver[0] = get_rcsp_version();
    return (u8 *)&rcsp_dev_ver;
}

static u8  local_ver_array[2];
static u8 attr_get_dev_ver_size(void)
{
    return sizeof(local_ver_array);
}

static u8 *attr_get_dev_ver_ptr(void)
{
    return local_ver_array;
}

struct DEV_PID_VID {
    u16 VID;
    u16 PID;
};

static struct DEV_PID_VID test_PID_VID_val = {
    .VID = 0xAAAA,
    .PID = 0x8888,
};

static u8 attr_get_PID_VID_val_size(void)
{
    return sizeof(struct DEV_PID_VID); //test !!
}

static u8 *attr_get_PID_VID_val_ptr(void)
{
    return (u8 *)&test_PID_VID_val;
}

#define UBOOT_VER_FLAG          (0x1C000-0x8)

static u8 attr_get_uboot_version_size(void)
{
    return 2;
}

static u8 *attr_get_uboot_version_ptr(void)
{
    //return uboot_version;
    printf("uboot_ver:%x\n", *((u16 *)UBOOT_VER_FLAG));
    return (u8 *)UBOOT_VER_FLAG;
}

enum {
    FW_RUN_SDK = 0,
    FW_RUN_UBOOT,
};

static u8 fw_run_info[1] = {
    FW_RUN_UBOOT,
};

static u8 *attr_get_fw_run_type_ptr(void)
{
    return (u8 *)fw_run_info;
}

static u8 attr_get_fw_run_type_size(void)
{
    return sizeof(fw_run_info);
}

enum {
    FW_SINGLE_BANK = 0,
    FW_DOUBLE_BANK,
};

static u8 fw_bank_num[1] = {
    FW_SINGLE_BANK,
};

static u8 *attr_get_fw_bank_num_ptr(void)
{
    return (u8 *)fw_bank_num;
}

static u8 attr_get_fw_bank_num_size(void)
{
    return sizeof(fw_bank_num);
}

enum {
    EDR_PROFILE_SPP = BIT(0),
    EDR_PROFILE_HFP = BIT(1),
    EDR_PROFILE_A2DP = BIT(2),
    EDR_PROFILE_AVRCP = BIT(3),
    EDR_PROFILE_HID = BIT(4),
    EDR_PROFILE_AVDTP = BIT(5),
    EDR_PROFILE_PBAP = BIT(6),
};

enum {
    EDR_STATE_DISCONNECTED = 0,
    EDR_STATE_CONNECTED,
};

typedef struct _edr_info_t {
    u8 addr[6];
    u8 profile_support;
    u8 edr_state;
} __attribute__((packed)) edr_info_t;


static edr_info_t edr_info = {
    .profile_support = EDR_PROFILE_SPP,
    .edr_state = EDR_STATE_DISCONNECTED,
};

void set_JL_edr_mac(u8 *addr)
{
    memcpy(edr_info.addr, addr, sizeof(edr_info.addr));
}

static u8 *attr_get_edr_info_ptr(void)
{

    return (u8 *)&edr_info;
}

static u8 attr_get_edr_info_size(void)
{
    return sizeof(edr_info);
}

static u8 sdk_type[1] = {2};
static u8 *attr_get_sdk_type_ptr(void)
{
    return (u8 *)sdk_type;
}

static u8 attr_get_sdk_type_size(void)
{
    return sizeof(sdk_type);
}


#if VER_INFO_EXT_CONUNT
static u8 local_ext_info[VER_INFO_EXT_MAX_LEN + 1] = {0};
static u8 type = 0;
static u8 *attr_get_auth_key_data(void)
{
    if (CFG_ITEM_VER_INFO_AUTHKEY == type) {
        return local_ext_info;
    }
    return NULL;
}

static u8 attr_get_auth_key_size(void)
{
    type = CFG_ITEM_VER_INFO_AUTHKEY;
    ex_cfg_get_content_api(type, local_ext_info, sizeof(local_ext_info));
    return strlen(local_ext_info);
}

static u8 *attr_get_pro_code_data(void)
{
    if (CFG_ITEM_VER_INFO_PROCODE == type) {
        return local_ext_info;
    }
    return NULL;
}

static u8 attr_get_pro_code_size(void)
{
    type = CFG_ITEM_VER_INFO_PROCODE;
    ex_cfg_get_content_api(type, local_ext_info, sizeof(local_ext_info));
    return strlen(local_ext_info);
}
#endif
//优化空间占用
static const LTV_ATTR_GET attr_info_tab[] = {
    {
        .type = JL_ATTR_TYPE_PROTOCOL_VERSION,
        .get_size = attr_get_rcsp_ver_size,
        .get_data = attr_get_rcsp_ver_ptr,
    },
    {
        .type = JL_ATTR_TYPE_DEV_VERSION,
        .get_size = attr_get_dev_ver_size,
        .get_data = attr_get_dev_ver_ptr,
    },
    {
        .type = JL_ATTR_TYPE_PID_VID,
        .get_size = attr_get_PID_VID_val_size,
        .get_data = attr_get_PID_VID_val_ptr,
    },
    {
        .type = JL_ATTR_TYPE_UBOOT_VERSION,
        .get_size = attr_get_uboot_version_size,
        .get_data = attr_get_uboot_version_ptr,
    },
    {
        .type = JL_ATTR_TYPE_FW_DOUBLE_BANK,
        .get_size = attr_get_fw_bank_num_size,
        .get_data = attr_get_fw_bank_num_ptr,
    },
    {
        .type = JL_ATTR_TYPE_FORCE_UPDATE_STATUS,
        .get_size = attr_get_fw_run_type_size,
        .get_data = attr_get_fw_run_type_ptr,
    },
    {
        .type = JL_ATTR_TYPE_EDR_INFO,
        .get_size = attr_get_edr_info_size,
        .get_data = attr_get_edr_info_ptr,
    },
    {
        .type = JL_ATTR_TYPE_SDK_TYPE,
        .get_size = attr_get_sdk_type_size,
        .get_data = attr_get_sdk_type_ptr,
    },
#if VER_INFO_EXT_CONUNT
    {
        .type = JL_ATTR_TYPE_AUTH_KEY,
        .get_size = attr_get_auth_key_size,
        .get_data = attr_get_auth_key_data,
    },
    {
        .type = JL_ATTR_TYPE_PRO_CODE,
        .get_size = attr_get_pro_code_size,
        .get_data = attr_get_pro_code_data,
    },
#endif
};


enum {
    UPDATE_FLAG_OK,
    UPDATE_FLAG_LOW_POWER,
    UPDATE_FLAG_FW_INFO_ERR,
};

static JL_ERR JL_controller_resp_dev_info_send(u8 OpCode, u8 OpCode_SN, u32 mask)
{
    JL_ERR send_err = JL_ERR_NONE;
    log_info("dev info resp, OpCode = %d, OpCode_SN = %x, mask = %x\n", OpCode, OpCode_SN, mask);

    u16 local_ver = ex_cfg_get_local_version_info(GET_VER_FROM_CFG);
#if VER_INFO_USE_STRING
    sprintf(test_ver_strings, "V%d.%d.%d.%d",
            (local_ver & 0xf000) >> 12,
            (local_ver & 0x0f00) >> 8,
            (local_ver & 0x00f0) >> 4,
            (local_ver & 0x000f));
#else
    local_ver_array[0] = ((u8 *)&local_ver)[1];
    local_ver_array[1] = ((u8 *)&local_ver)[0];
#endif
    log_info("local_ver:%x\n", local_ver);
    test_PID_VID_val.PID = ex_cfg_get_local_version_info(GET_PID_FROM_CFG);
    test_PID_VID_val.VID = ex_cfg_get_local_version_info(GET_VID_FROM_CFG);
    if (mask == 0) {
        send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_PARAM_ERR, OpCode_SN, NULL, 0);
    } else {
        u8 i;
        u32 attr_total_len = ltv_attr_total_len_get(mask, (LTV_ATTR_GET *)attr_info_tab, sizeof(attr_info_tab) / sizeof(attr_info_tab[0]));

        log_info("attr_total_len = %d\n", attr_total_len);
        /* log_info("--func = %s, line = %d\n", __FUNCTION__, __LINE__); */
        u8 *ptr = JL_controller_calloc(1, attr_total_len);
        u8 *attr_val_ptr = ptr;


        if (attr_val_ptr == NULL) {
            send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_RESP_DATA_OVER_LIMIT, OpCode_SN, NULL, 0);
        } else {
            ltv_attr_fill(mask, (LTV_ATTR_GET *)attr_info_tab, sizeof(attr_info_tab) / sizeof(attr_info_tab[0]), attr_val_ptr);
            /* log_info("attr data:\n"); */
            /* log_info_hexdump(ptr, (u32)attr_total_len); */
            send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, ptr, attr_total_len);
            JL_controller_free(ptr);
        }
    }

    return send_err;
}

static JL_ERR JL_controller_resp_dev_update_file_info_offset(u8 OpCode, u8 OpCode_SN)
{
    JL_ERR send_err = JL_ERR_NONE;

    u8 data[4 + 2];
    u16 update_file_info_offset = DEV_UPDATE_FILE_INFO_OFFSET;
    u16 update_file_info_len = DEV_UPDATE_FILE_INFO_LEN;

    WRITE_BIG_U32(data + 0, update_file_info_offset);
    WRITE_BIG_U16(data + 4, update_file_info_len);
    //log_info("--UPDATE_INFO:\n");
    //log_info_hexdump(data, sizeof(data));

    //printf("update info len:%x\n",sizeof(data));
    send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, data, sizeof(data));

    return send_err;
}

static JL_ERR JL_controller_resp_inquire_device_if_can_update(u8 OpCode, u8 OpCode_SN, u8 update_sta)
{
    JL_ERR send_err = JL_ERR_NONE;
    u8 data[1];

    data[0] = update_sta;

    send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, data, sizeof(data));

    return send_err;
}

JL_ERR JL_controller_resp_exit_update_mode(u8 OpCode, u8 OpCode_SN, u8 status)
{
    JL_ERR send_err = JL_ERR_NONE;
    u8 data[1];

    data[0] = status;	//0:sucess 1:fail;

    send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, data, sizeof(data));

    return send_err;
}


static JL_ERR JL_controller_resp_get_dev_refresh_fw_status(u8 OpCode, u8 OpCode_SN, u8 result)
{
    JL_ERR send_err = JL_ERR_NONE;
    u8 data[1];

    data[0] = result;	//0:sucess 1:fail;

    send_err = JL_CMD_response_send(OpCode, JL_PRO_STATUS_SUCCESS, OpCode_SN, data, sizeof(data));

    return send_err;
}

void JL_controller_resp_udpate_status_request(u8 status)
{
    u8 OpCode;
    u8 OpCode_SN;

    JL_ERR send_err = JL_ERR_NONE;

    JL_controller_get_curr_cmd_para(&OpCode, &OpCode_SN);

    log_info("get cmd para:%x %x\n", OpCode, OpCode_SN);

    if (JL_OPCODE_GET_DEVICE_REFRESH_FW_STATUS == OpCode) {
        send_err = JL_controller_resp_get_dev_refresh_fw_status(OpCode, OpCode_SN, status);
    }
}

static u8 judge_remote_version_can_update(void)
{
    u16 remote_file_version = READ_BIG_U16(update_file_id_info.update_file_id_info.ver);
    u16 local_file_version = 0;//ex_cfg_get_local_version_info(GET_VER_FROM_CFG);
    /*
    if (0 == judge_battery_power_is_enough()) {
        return UPDATE_FLAG_LOW_POWER;
    }
    */
    if (0 == remote_file_version) {
        return UPDATE_FLAG_OK;
    } else if (remote_file_version <= local_file_version) {
        return UPDATE_FLAG_FW_INFO_ERR;
    }
#if (0 == VER_INFO_EXT_CONUNT)
    u16 remote_pid = READ_BIG_U16(update_file_id_info.update_file_id_info.pid);
    u16 local_pid = 0;//ex_cfg_get_local_version_info(GET_PID_FROM_CFG);

    u16 remote_vid = READ_BIG_U16(update_file_id_info.update_file_id_info.vid);
    u16 local_vid = 0;//ex_cfg_get_local_version_info(GET_VID_FROM_CFG);

    if (remote_pid != local_pid || remote_vid != remote_vid) {
        return UPDATE_FLAG_FW_INFO_ERR;
    }
#else
    u8 cfg_type = CFG_ITEM_VER_INFO_AUTHKEY;
    u8 *remote_authkey_procode = update_file_id_info.ext;
    for (; cfg_type < CFG_ITEM_LAST_DEVICE_CONNECT_LINKKEY_INFO; cfg_type++) {
        ex_cfg_get_content_api(cfg_type, local_ext_info, sizeof(local_ext_info));
        if (memcmp(remote_authkey_procode, local_ext_info, strlen(local_ext_info))) {
            return UPDATE_FLAG_FW_INFO_ERR;
        }
        remote_authkey_procode += strlen(local_ext_info) + 1;
    }
#endif
    return UPDATE_FLAG_OK;
}

extern void chip_reset(void);
void jl_rcsp_msg_deal(void *hdl, int *msg)
{
    u8 can_update_flag = UPDATE_FLAG_OK;
    u16 remote_file_version;
    u16 remote_pid;
    u16 remote_vid;
    JL_ERR send_err = JL_ERR_NONE;

    //printf("JL_RCSP msg[1]:%x,msg[2]:%x\n", msg[1], msg[2]);
    switch (msg[0]) {
    case MSG_JL_GET_DEV_INFO:
        //log_info("MSG_JL_GET_DEV_INFO\n");
        send_err = JL_opcode_get_target_info(NULL, (u8)msg[1], (u8)msg[2], msg[3]);
        //send_err = JL_controller_resp_dev_info_send((u8)msg[1], (u8)msg[2], (u32)msg[3]);
        break;

    case MSG_JL_GET_DEV_UPDATE_FILE_INFO_OFFSET:
        //log_info("MSG_JL_GET_DEV_UPDATE_FILE_INFO_OFFSET\n");
        send_err = JL_controller_resp_dev_update_file_info_offset((u8)msg[1], (u8)msg[2]);
        break;

    case MSG_JL_INQUIRE_DEVEICE_IF_CAN_UPDATE:
        can_update_flag = UPDATE_FLAG_OK;//judge_remote_version_can_update();
        send_err = JL_controller_resp_inquire_device_if_can_update((u8)msg[1], (u8)msg[2], can_update_flag);
        break;

    case MSG_JL_ENTER_UPDATE_MODE:
        JL_CMD_response_send(msg[1], JL_PRO_STATUS_SUCCESS, msg[2], rsp, sizeof(rsp));
        if (rcsp_state_change_callback) {
            rcsp_state_change_callback(NULL, DEV_CONN_STATE_START_UPDATE);
        }
        //send_err = JL_controller_resp_enter_update_mode((u8)msg[1],(u8)msg[2]);
        break;

    case MSG_JL_EXIT_UPDATE_MODE:
        //status:0 - success;1 - failure;
        send_err = JL_controller_resp_exit_update_mode((u8)msg[1], (u8)msg[2], 0x01);
        break;

    case MSG_UBOOT_SOFT_POWEROFF :
        puts("soft off\n");
        //extern void soft_poweroff_simple(void);
        //soft_poweroff_simple();

#if 0
    case MSG_JL_UPDATE_REVICE_REBOOT:
        if ((__this->rcsp_ble->get_packet_num_of_not_sent) && \
            (0 == __this->rcsp_ble->get_packet_num_of_not_sent(NULL))) {
            log_info("all data sent\n");
            return;
        } else {
            task_post_msg(NULL, 1, MSG_JL_UPDATE_REVICE_REBOOT);
        }
        break;
#endif

#if 1//SPP_FORCE_UPDATE_EN
    case MSG_JL_SWITCH_CH_SPP:
        if (switch_flag) {
            switch_flag = 0;
            JL_CMD_response_send(msg[1], JL_PRO_STATUS_SUCCESS, msg[2], NULL, 0);
        }
#if 0
        if (0 == __this->rcsp_ble->get_packet_num_of_not_sent(NULL)) {
            __this->rcsp_ble->close(NULL);
            set_updata_type(SPP_UPDATA);
            log_info("jump to spp uboot\n");
            chip_reset();
        } else {
            log_info("w");
            task_post_msg(NULL, 1, MSG_JL_SWITCH_CH_SPP);
        }
#endif
        break;
#endif
    case MSG_JL_SUCCESS_STATUS_HANDLE:
        JL_CMD_response_send(msg[1], JL_PRO_STATUS_SUCCESS, msg[2], rsp, sizeof(rsp));
        break;

    case MSG_JL_ERR_STATUS_HANDLE:
        JL_CMD_response_send(msg[1], msg[2], msg[3], NULL, 0);
        break;
    default:
        //JL_private_msg_deal(hdl, msg);
        break;
    }

    if (send_err != JL_ERR_NONE) {
        log_info("send err! [%d, %s, %d]\n", send_err, __FUNCTION__, __LINE__);
    }
}

#define JL_RCSP_MTU_SIZE 	(512+2)
#ifdef APP_USE_MOR_SPACE
u8 rcsp_buf[0x1714];// sec_used(.JL_pool)__attribute__((aligned(4)));
#else
u8 rcsp_buf[0xA80];// sec_used(.JL_pool)__attribute__((aligned(4)));
#endif
extern void rcsp_timer_register_api_register(void (*handle)(u32 ms, void (*func)(void)));
const u8 link_key_data[16] = {0x06, 0x77, 0x5f, 0x87, 0x91, 0x8d, 0xd4, 0x23, 0x00, 0x5d, 0xf1, 0xd8, 0xcf, 0x0c, 0x14, 0x2b};

#if 0
extern void *periodic_timer_add(u32 period_ms, void (*func)(void));
static void *rcsp_periodic_timer = NULL;
static void rcsp_ms_timer_register_handle(u32 ms, void (*func)(void))
{
    log_info("reg %d ms timer for rcsp:%x\n", ms, func);
    if (NULL == rcsp_periodic_timer) {
        rcsp_periodic_timer = periodic_timer_add(ms, func);
    }
}
#endif
void mpu_disable_by_index(u8 idx);
static void JL_rcsp_control_init(void)
{
    //rcsp_timer_register_api_register(rcsp_ms_timer_register_handle);
    __this->rcsp_tick_cnt = 0;
    JL_protocol_init(rcsp_buf, sizeof(rcsp_buf));
    JL_controller_mem_init();
    rcsp_dev_select(RCSP_BLE);
    //rcsp_periodic_timer = periodic_timer_add(ms, func);
}

static void JL_rcsp_control_exit(void)
{
    JL_protocol_exit();
    /* if (rcsp_periodic_timer) {
        periodic_timer_remove(rcsp_periodic_timer);
    } */
}
extern void JL_protocol_process(void);

volatile u8 loop_resume_cnt = 0;
SET_INTERRUPT
static void JL_rcsp_loop(void)
{
    irq_common_handler(IRQ_SOFT1_IDX);
    JL_protocol_process();

    local_irq_disable();
    if (loop_resume_cnt) {
        loop_resume_cnt--;
        irq_set_pending(IRQ_SOFT1_IDX);
    }
    local_irq_enable();
}

//重写lib weak接口
void JL_rcsp_resume_do(void)
{
    local_irq_disable();
    if (loop_resume_cnt < 0xff) {
        loop_resume_cnt++;
    }
    local_irq_enable();
    irq_set_pending(IRQ_SOFT1_IDX);
}

void rcsp_tick_timer_handle(void)
{
    JL_set_cur_tick(__this->rcsp_tick_cnt++);
    JL_rcsp_resume_do();
}

extern void ble_get_server_operation_table(struct ble_server_operation_t **interface_pt);
void rcsp_init(void (*start_speech)(void), void (*stop_speech)(void))
{
    u8 pin_code[4] = {0};
    // device version
    set_rcsp_dev_version(0x01, 0x01, 0x00, 0x00);

    //从exif获取sdk的认证信息
    ex_cfg_get_content_api(CFG_ITEM_PIN_CODE, pin_code, sizeof(pin_code));
    printf("pin code\n");
    put_buf(pin_code, 4);
    if (pin_code[0] != 0) {
        //不进行校验
        JL_rcsp_set_no_auth();
    }

    if (pin_code[1] != 0) {
        JL_rcsp_set_pvid_flag(1);
    }

    memset((u8 *)__this, 0, sizeof(struct JL_AI_VAR));

#if JL_RCSP_USER_COUSTOM_EN
    __this->rcsp_user = (struct __rcsp_user_var *)get_user_rcsp_opt();
#endif
    ble_get_server_operation_table(&__this->rcsp_ble);

    printf("rcsp need buf size:%x\n", rcsp_protocol_need_buf_size());
    //log_info("set buf:%x %x\n", rcsp_buf, sizeof(rcsp_buf));

    //rcsp_timer_register_api_register(rcsp_ms_timer_register_handle);
    //ex_cfg_get_content_api(CFG_ITEM_PIN_CODE, &receieve_data_flag, 1);
    JL_rcsp_control_init();

    if (__this->rcsp_ble->send_data) {
        log_info("rcsp auth init\n");
        JL_rcsp_auth_init(__this->rcsp_ble->send_data, (u8 *)link_key_data, NULL);
    }
#ifdef APP_USE_MOR_SPACE
    // 防止rcsp被蓝牙的中断打断
    request_irq(IRQ_SOFT1_IDX, 2, JL_rcsp_loop, 0);
#else
    request_irq(IRQ_SOFT1_IDX, 1, JL_rcsp_loop, 0);
#endif
    sys_timer_add(NULL, rcsp_tick_timer_handle, 2500);
}

static void bt_module_close(void)
{
    if (__this->rcsp_ble && __this->rcsp_ble->close) {
        __this->rcsp_ble->close(NULL);
    } else {
        log_error("close handle not reg\n");
    }
}

const app_update_op_api_t rcsp_update_op_obj = {
    .data_send = rcsp_update_data_read,
    .notify_update_content_size = rcsp_notify_update_content_size,
    .dev_disconnect = rcsp_dev_disconnect,
    .get_number_of_pkt_sending = NULL,//rcsp_get_number_of_pkt_sending,
    .register_update_data_handle = register_receive_fw_update_block_handle,
    .register_update_status_req_handle = register_device_refresh_fw_status_handle,
    .register_conn_state_change_handle = register_rcsp_state_change_handle,
    .btstack_init = NULL,
    .task_run = NULL,
    .btstack_resume = NULL,
    .dev_set_discoverable_mode = NULL,
    .dev_close = bt_module_close,
};

void dev_get_update_opearation_table(app_update_op_api_t **interface_ptr)
{
    *interface_ptr = &rcsp_update_op_obj;
}


