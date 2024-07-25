/*********************************************************************************************
    *   Filename        : le_smartbox_module.c

    *   Description     :

    *   Author          :

    *   Email           : zh-jieli.com

    *   Last modifiled  : 2022-06-08 11:14

    *   Copyright:(c)JIELI  2011-2022  @ , All Rights Reserved.
*********************************************************************************************/

// *****************************************************************************
// *****************************************************************************
#include "system/app_core.h"
#include "system/includes.h"
#include "smartbox/config.h"

#include "app_action.h"

#include "btstack/btstack_task.h"
#include "btstack/bluetooth.h"
#include "user_cfg.h"
#include "vm.h"
#include "app_power_manage.h"
#include "btcontroller_modules.h"
#include "bt_common.h"
#include "3th_profile_api.h"
#include "btstack/avctp_user.h"
#include "app_chargestore.h"

#include "btcrypt.h"
#include "custom_cfg.h"
#include "smartbox_music_info_setting.h"
#include "le_smartbox_adv.h"
#include "le_smartbox_module.h"
#include "classic/tws_api.h"
#include "smartbox_update.h"
#include "le_smartbox_multi_common.h"
#include "btstack/btstack_event.h"
//#include "../../../../common/ui/lv_watch/comm_parse/common_rev.h"
#include "../../../../common/ui/lv_watch/lv_watch.h"
#if TCFG_PAY_ALIOS_ENABLE
#if (TCFG_PAY_ALIOS_WAY_SEL == TCFG_PAY_ALIOS_WAY_ALIYUN)
#else
#include "alipay.h"
#endif
#endif
#if (TCFG_BLE_DEMO_SELECT == DEF_BLE_DEMO_RCSP_DEMO)

//ANCS profile enable
#define TRANS_ANCS_EN  			  	 1

//AMS profile enable
#define TRANS_AMS_EN  			  	 0

// #if (defined(TCFG_UI_ENABLE_NOTICE) && (!TCFG_UI_ENABLE_NOTICE))
// #undef TRANS_ANCS_EN
// #define TRANS_ANCS_EN  			  	 0
// #endif

#if 1
#define log_info(x, ...)       printf("[LE-SMART]" x " ", ## __VA_ARGS__)
#define log_info_hexdump       put_buf
#else
#define log_info(...)
#define log_info_hexdump(...)
#endif

#define EIR_TAG_STRING   0xd6, 0x05, 0x08, 0x00, 'J', 'L', 'A', 'I', 'S', 'D','K'
static const char user_tag_string[] = {EIR_TAG_STRING};

//------
#define ATT_LOCAL_PAYLOAD_SIZE    (247)//(517)                   //note: need >= 20
#define ATT_SEND_CBUF_SIZE        (512*8)                   //note: need >= 20,缓存大小，可修改
#define ATT_RAM_BUFSIZE           (ATT_CTRL_BLOCK_SIZE + ATT_LOCAL_PAYLOAD_SIZE + ATT_SEND_CBUF_SIZE)                   //note:
static u8 att_ram_buffer[ATT_RAM_BUFSIZE] __attribute__((aligned(4)));

//---------------
//---------------
#define ADV_INTERVAL_MIN          (160*5) //n*0.625ms
/* #define ADV_INTERVAL_MIN          (0x30) */

//watch切换 app1 or app2
u8 select_app1_or_app2 = 1;
u8 app_change_key_is_press = 0;
u16 set_adv_interval;
static hci_con_handle_t con_handle;

//加密设置
static const uint8_t sm_min_key_size = 7;

//连接参数设置
static const uint8_t connection_update_enable = 1; ///0--disable, 1--enable
static uint8_t connection_update_cnt = 1; //
static const struct conn_update_param_t connection_param_table[] = {
    {48, 64, 10, 600},
    {25, 32, 16, 600},
    {12, 28, 14, 600},//11
    {8,  20, 20, 600},//3.7
    /* {12, 28, 4, 600},//3.7 */
    /* {12, 24, 30, 600},//3.05 */
};

static const struct conn_update_param_t connection_param_table_update[] = {
    {96, 120, 0,  600},
    {60,  80, 0,  600},
    {60,  80, 0,  600},
    /* {8,   20, 0,  600}, */
    {6, 12, 0, 400},/*ios 提速*/
};

static u8 conn_update_param_index = 0;
static u8 conn_param_len = sizeof(connection_param_table) / sizeof(struct conn_update_param_t);
#define CONN_PARAM_TABLE_CNT      (sizeof(connection_param_table)/sizeof(struct conn_update_param_t))
#define CONN_TRY_NUM			  10 // 重复尝试次数

#if (ATT_RAM_BUFSIZE < 64)
#error "adv_data & rsp_data buffer error!!!!!!!!!!!!"
#endif


static char *gap_device_name = "AC697N_ble_test";
static u8 gap_device_name_len = 0;
volatile static u8 ble_work_state = 0;
static u8 adv_ctrl_en;
static u8 ble_init_flag;

static u8 cur_peer_addr_info[7]; /*当前连接对方地址信息*/
static u8 check_rcsp_auth_flag; /*检查认证是否通过,ios发起回连edr*/
static u8 connect_remote_type;  /*remote's system type*/
static u8 pair_reconnect_flag;  /*配对回连标记*/

#if TCFG_PAY_ALIOS_ENABLE
static u8 upay_mode_enable;/*upay绑定模式*/
static u8 upay_new_adv_enable;/**/
static void (*upay_recv_callback)(const uint8_t *data, u16 len);
static void upay_ble_new_adv_enable(u8 en);
#endif

static void (*app_recieve_callback)(void *priv, void *buf, u16 len) = NULL;
static void (*app_ble_state_callback)(void *priv, ble_state_e state) = NULL;
static void (*ble_resume_send_wakeup)(void) = NULL;
static u32 channel_priv;

extern u8 get_rcsp_connect_status(void);
extern const int config_btctler_le_hw_nums;

static const char *const phy_result[] = {
    "None",
    "1M",
    "2M",
    "Coded"
};
//------------------------------------------------------
//ANCS
#if TRANS_ANCS_EN
//profile event
#define ANCS_SUBEVENT_CLIENT_CONNECTED                              0xF0
#define ANCS_SUBEVENT_CLIENT_NOTIFICATION                           0xF1
#define ANCS_SUBEVENT_CLIENT_DISCONNECTED                           0xF2

#define ANCS_MESSAGE_MANAGE_EN                                      0
void ancs_client_init(void);
void ancs_client_exit(void);
void ancs_client_register_callback(btstack_packet_handler_t callback);
const char *ancs_client_attribute_name_for_id(int id);
void ancs_set_notification_buffer(u8 *buffer, u16 buffer_size);
u32 get_notification_uid(void);
u16 get_controlpoint_handle(void);
void ancs_set_out_callback(void *cb);

//ancs info buffer
#define ANCS_INFO_BUFFER_SIZE  (384)//support 内容长度 128byte
static u8 ancs_info_buffer[ANCS_INFO_BUFFER_SIZE];
#else
#define ANCS_MESSAGE_MANAGE_EN                                      0
#endif

//ams
#if TRANS_AMS_EN
//profile event
#define AMS_SUBEVENT_CLIENT_CONNECTED                               0xF3
#define AMS_SUBEVENT_CLIENT_NOTIFICATION                            0xF4
#define AMS_SUBEVENT_CLIENT_DISCONNECTED                            0xF5

void ams_client_init(void);
void ams_client_exit(void);
void ams_client_register_callback(btstack_packet_handler_t handler);
const char *ams_get_entity_id_name(u8 entity_id);
const char *ams_get_entity_attribute_name(u8 entity_id, u8 attr_id);
#endif
//------------------------------------------------------

static int app_send_user_data_check(u16 len);
static int app_send_user_data_do(void *priv, u8 *data, u16 len);
static int app_send_user_data(u16 handle, u8 *data, u16 len, u8 handle_type);
//------------------------------------------------------
//广播参数设置
static void advertisements_setup_init();
extern const char *bt_get_local_name();
static int set_adv_enable(void *priv, u32 en);
static int get_buffer_vaild_len(void *priv);
//------------------------------------------------------
static void send_request_connect_parameter(u8 table_index)
{
    struct conn_update_param_t *param = NULL; //static ram

    switch(conn_update_param_index) 
    {
        case 0:
            param = (void *)&connection_param_table[table_index];
            break;
        case 1:
            param = (void *)&connection_param_table_update[table_index];
            break;
        default:
            break;
    }

    log_info("update_request:-%d-%d-%d-%d-\n", \
        param->interval_min, param->interval_max, param->latency, param->timeout);

    if(con_handle)
    {
        ble_user_cmd_prepare(BLE_CMD_REQ_CONN_PARAM_UPDATE, 2, con_handle, param);
    }
        
    return;
}

static void check_connetion_updata_deal(void)
{
    if (connection_update_enable) {
        if (connection_update_cnt < (CONN_PARAM_TABLE_CNT * CONN_TRY_NUM)) {
            send_request_connect_parameter(connection_update_cnt / CONN_TRY_NUM);
        }
    }
}

void notify_update_connect_parameter(u8 table_index)
{
    u8 conn_update_param_index_record = conn_update_param_index;
    if ((u8) - 1 != table_index) {
        conn_update_param_index = 1;
        send_request_connect_parameter(table_index);
    } else {
        if (connection_update_cnt >= (CONN_PARAM_TABLE_CNT * CONN_TRY_NUM)) {
            log_info("connection_update_cnt >= CONN_PARAM_TABLE_CNT");
            connection_update_cnt = 0;
        }
        send_request_connect_parameter(connection_update_cnt / CONN_TRY_NUM);
    }
    conn_update_param_index = conn_update_param_index_record;
}

static void connection_update_complete_success(u8 *packet)
{
    int con_handle, conn_interval, conn_latency, conn_timeout;

    con_handle = \
        hci_subevent_le_connection_update_complete_get_connection_handle(packet);
    conn_interval = \
        hci_subevent_le_connection_update_complete_get_conn_interval(packet);
    conn_latency = \
        hci_subevent_le_connection_update_complete_get_conn_latency(packet);
    conn_timeout = \
        hci_subevent_le_connection_update_complete_get_supervision_timeout(packet);

    log_info("%s:conn_interval = %d\n", __func__, conn_interval);
    log_info("%s:conn_latency = %d\n", __func__, conn_latency);
    log_info("%s:conn_timeout = %d\n", __func__, conn_timeout);
}


static void set_ble_work_state(ble_state_e state)
{
    if(state != ble_work_state) 
    {
        printf("ble_work_st:%x->%x\n", \
            ble_work_state, state);

        ble_work_state = state;

        if(app_ble_state_callback) 
            app_ble_state_callback((void *)channel_priv, state);
    }
}

static ble_state_e get_ble_work_state(void)
{
    return ble_work_state;
}

ble_state_e smartbox_get_ble_work_state(void)
{
    return ble_work_state;
}

static void cbk_sm_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    u32 tmp32;
    sm_just_event_t *event = (void *)packet;
    

    switch (packet_type) 
    {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) 
            {
                case SM_EVENT_JUST_WORKS_REQUEST:
                    sm_just_works_confirm(sm_event_just_works_request_get_handle(packet));
                    log_info("Just Works Confirmed.\n");
                    break;

                case SM_EVENT_PASSKEY_DISPLAY_NUMBER:
                    log_info_hexdump(packet, size);
                    memcpy(&tmp32, event->data, 4);
                    log_info("Passkey display: %06u.\n", tmp32);
                    break;

                case SM_EVENT_PAIR_PROCESS:
                    log_info("======PAIR_PROCESS: %02x\n", event->data[0]);
                    put_buf(event->data, 4);
                    if (event->data[0] == SM_EVENT_PAIR_SUB_RECONNECT_START) 
                    {
                        pair_reconnect_flag = 1;
                    }

                    switch (event->data[0]) 
                    {
                        case SM_EVENT_PAIR_SUB_RECONNECT_START:
                            pair_reconnect_flag = 1;
                            log_info("reconnect start\n");
                            break;

                        case SM_EVENT_PAIR_SUB_PIN_KEY_MISS:
                            log_info("err:pin or keymiss\n");
                            break;

                        case SM_EVENT_PAIR_SUB_PAIR_FAIL:
                            log_info("err:pair fail,reason=%02x,is_peer? %d\n", event->data[1], event->data[2]);
                            break;

                        case SM_EVENT_PAIR_SUB_PAIR_TIMEOUT:
                            log_info("err:pair timeout\n");
                            break;

                        case SM_EVENT_PAIR_SUB_ADD_LIST_SUCCESS:
                            log_info("first pair,add list success\n");
                            break;

                        case SM_EVENT_PAIR_SUB_ADD_LIST_FAILED:
                            log_info("err:add list fail\n");
                            break;

                        case SM_EVENT_PAIR_SUB_SEND_DISCONN:
                            log_info("err:local send disconnect,reason= %02x\n", event->data[1]);
                            break;

                        default:
                            break;
                    }

                        break;
            }
            break;
    }
}

static void can_send_now_wakeup(void)
{
    putchar('E');
    if (ble_resume_send_wakeup) 
    {
        ble_resume_send_wakeup();
    }
}

_WEAK_
u8 ble_update_get_ready_jump_flag(void)
{
    return 0;
}

extern u8 is_bredr_close(void);
extern u8 is_bredr_inquiry_state(void);
extern void bt_init_bredr();
extern void bredr_conn_last_dev();
extern u8 connect_last_source_device_from_vm();
extern u8 connect_last_device_from_vm();
//参考识别
static void smart_att_check_remote_result(u16 con_handle, remote_type_e remote_type)
{
    log_info("smart %02x:remote_type= %02x\n", con_handle, remote_type);
    connect_remote_type = remote_type;
    //to do
    if (pair_reconnect_flag == 1 && connect_remote_type == REMOTE_TYPE_IOS) { //ble回连ios时启动edr回连
        if (is_bredr_close() == 1) {
            bredr_conn_last_dev();
        } else if (is_bredr_inquiry_state() == 1) {
            user_send_cmd_prepare(USER_CTRL_INQUIRY_CANCEL, 0, NULL);
#if TCFG_USER_EMITTER_ENABLE
            log_info(">>>connect last source edr...\n");
            connect_last_source_device_from_vm();
#else
            log_info(">>>connect last edr...\n");
            connect_last_device_from_vm();
#endif
        } else {
#if TCFG_USER_EMITTER_ENABLE
            log_info(">>>connect last edr...\n");
            connect_last_source_device_from_vm();
#else
            connect_last_device_from_vm();
#endif
        }
    }
}

/*
 * @section Packet Handler
 *
 * @text The packet handler is used to:
 *        - stop the counter after a disconnect
 *        - send a notification when the requested ATT_EVENT_CAN_SEND_NOW is received
 */


/* LISTING_START(packetHandler): Packet Handler */
static void cbk_packet_handler(uint8_t packet_type, uint16_t channel, \
    uint8_t *packet, uint16_t size)
{
    int mtu;
    u32 tmp;
    const char *attribute_name;

    switch(packet_type) 
    {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet)) 
            {
                /* case DAEMON_EVENT_HCI_PACKET_SENT: */
                    /* break; */

                case ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE:
                    log_info("ATT_EVENT_HANDLE_VALUE_INDICATION_COMPLETE\n");

                case ATT_EVENT_CAN_SEND_NOW:
                    can_send_now_wakeup();
                    break;

                case HCI_EVENT_LE_META:
                    switch (hci_event_le_meta_get_subevent_code(packet)) 
                    {
                        case HCI_SUBEVENT_LE_CONNECTION_COMPLETE: 
                        {
                            if(!hci_subevent_le_enhanced_connection_complete_get_role(packet)) 
                            {
                                //is master
                                break;
                            }
                            set_app_connect_type(TYPE_BLE);
                            app_change_key_is_press = 0; //更新模式为非app切换模式
                            con_handle = little_endian_read_16(packet, 4);
                            log_info("HCI_SUBEVENT_LE_CONNECTION_COMPLETE: %0x\n", con_handle);
                            log_info_hexdump(packet + 7, 7);
                            memcpy(cur_peer_addr_info, packet + 7, 7);
                            connection_update_complete_success(packet + 8);
                            ble_user_cmd_prepare(BLE_CMD_ATT_SEND_INIT, 4, con_handle, att_ram_buffer, ATT_RAM_BUFSIZE, ATT_LOCAL_PAYLOAD_SIZE);
                            bt_3th_type_dev_select(RCSP_BLE);
                            set_ble_work_state(BLE_ST_CONNECT);
        #if (0 == BT_CONNECTION_VERIFY)
                            JL_rcsp_auth_reset();
        #endif
                            set_adv_notify_state(1);
                            pair_reconnect_flag = 0;
                            connect_remote_type = REMOTE_TYPE_UNKNOWN;
        #if TCFG_BLE_BRIDGE_EDR_ENALBE
                            check_rcsp_auth_flag = 0;
        #endif
                    }
                        break;

                    case HCI_SUBEVENT_LE_CONNECTION_UPDATE_COMPLETE:
                        if(con_handle != little_endian_read_16(packet, 4)) 
                        {
                            break;
                        }

                        connection_update_complete_success(packet);
                        
                        break;

                    case HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE:
                        if(con_handle != little_endian_read_16(packet, 3)) 
                        {
                            break;
                        }

                        log_info("HCI_SUBEVENT_LE_DATA_LENGTH_CHANGE\n");
                        
                        break;

                    case HCI_SUBEVENT_LE_PHY_UPDATE_COMPLETE:
                        if(con_handle != little_endian_read_16(packet, 4)) 
                        {
                            break;
                        }

                        if(con_handle) 
                        {
                            log_info("HCI_SUBEVENT_LE_PHY_UPDATE %s\n", hci_event_le_meta_get_phy_update_complete_status(packet) ? "Fail" : "Succ");
                            log_info("Tx PHY: %s\n", phy_result[hci_event_le_meta_get_phy_update_complete_tx_phy(packet)]);
                            log_info("Rx PHY: %s\n", phy_result[hci_event_le_meta_get_phy_update_complete_rx_phy(packet)]);
                        }

                        break;
                    }
                        break;

                case HCI_EVENT_DISCONNECTION_COMPLETE:
                    if(con_handle != little_endian_read_16(packet, 3)) 
                        break;

                    log_info("HCI_EVENT_DISCONNECTION_COMPLETE: %0x\n", packet[5]);

                    con_handle = 0;

#if (TCFG_BLE_ADV_DYNAMIC_SWITCH && TCFG_UI_ENABLE)
                    void le_adv_interval_reset(void);
                    le_adv_interval_reset();
#endif

#if TCFG_PAY_ALIOS_ENABLE && UPAY_ONE_PROFILE
                    upay_ble_new_adv_enable(0);
#endif

#if RCSP_ADV_MUSIC_INFO_ENABLE
                    extern void stop_get_music_timer(u8 en);
                    stop_get_music_timer(1);
#endif

                    /* #if (0 == BT_CONNECTION_VERIFY) */
                    /*             JL_rcsp_auth_reset(); */
                    /* #endif */

                    ble_user_cmd_prepare(BLE_CMD_ATT_SEND_INIT, \
                        4, con_handle, 0, 0, 0);
                    set_ble_work_state(BLE_ST_DISCONN);
                    set_app_connect_type(TYPE_NULL);
    #if RCSP_UPDATE_EN
                    if(get_jl_update_flag()) 
                        break;
    #endif
                    if(!ble_update_get_ready_jump_flag()) 
                    {
    #if TCFG_USER_TWS_ENABLE
                        if(!(tws_api_get_tws_state() & TWS_STA_SIBLING_CONNECTED)) 
                        {
                            log_info(")))))))) 1\n");
                            bt_ble_adv_enable(1);
                        }else 
                        {
                            log_info("))))))))>>>> %d\n", tws_api_get_role());
                            if(tws_api_get_role() == TWS_ROLE_MASTER) 
                            {
                                bt_ble_adv_enable(1);
                            }
                        }
    #else
    #if SMBOX_MULTI_BLE_EN
                        log_info("Disconn_to_switch_app  :%d\n", select_app1_or_app2);
                        log_info("app_change_key_is_press  :%d\n", app_change_key_is_press);
                        if(app_change_key_is_press) 
                        {
                            if(select_app1_or_app2) 
                            {
                                ble_profile_again_init(1);
                            }else 
                            {
                                smbox_multi_ble_profile_init(1);
                            }

                            app_change_key_is_press = 0;
                        }else 
                        {
                            bt_ble_adv_enable(1);
                        }
    #else
                        bt_ble_adv_enable(1);
    #endif
    #endif
                    }
                    
                    connection_update_cnt = 0;
                    break;

                case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
                    if(con_handle != little_endian_read_16(packet, 2)) 
                        break;

                    mtu = att_event_mtu_exchange_complete_get_MTU(packet) - 3;
                    log_info("%s:ATT MTU = %u\n", __func__, mtu);
                    ble_user_cmd_prepare(BLE_CMD_ATT_MTU_SIZE, 1, mtu);
                    break;

                case HCI_EVENT_VENDOR_REMOTE_TEST:
                    log_info("--- HCI_EVENT_VENDOR_REMOTE_TEST\n");
                    break;

                case L2CAP_EVENT_CONNECTION_PARAMETER_UPDATE_RESPONSE:
                    if(con_handle != little_endian_read_16(packet, 2)) 
                        break;

                    tmp = little_endian_read_16(packet, 4);
                    log_info("-update_rsp: %02x\n", tmp);

                    if(tmp)
                    {
                        connection_update_cnt++;
                        log_info("remoter reject!!!\n");
                        check_connetion_updata_deal();
                    }else 
                    {
                        connection_update_cnt = (CONN_PARAM_TABLE_CNT * CONN_TRY_NUM);
                    }
                    break;

                case HCI_EVENT_ENCRYPTION_CHANGE:
                    if(con_handle != little_endian_read_16(packet, 3)) 
                        break;

                    log_info("HCI_EVENT_ENCRYPTION_CHANGE= %d\n", packet[2]);

                    if(packet[2] == 0) 
                    {
                        /*ENCRYPTION is ok */
                        check_connetion_updata_deal();
                        /*要配对加密后才能检测对方系统类型*/
                        att_server_set_check_remote(con_handle, smart_att_check_remote_result);
                    }
                    break;

    #if TRANS_ANCS_EN
            case HCI_EVENT_ANCS_META:
                switch (hci_event_ancs_meta_get_subevent_code(packet)) {
                case ANCS_SUBEVENT_CLIENT_NOTIFICATION:
                    log_info("ANCS_SUBEVENT_CLIENT_NOTIFICATION\n");
                    attribute_name = ancs_client_attribute_name_for_id(ancs_subevent_client_notification_get_attribute_id(packet));
                    if (!attribute_name) {
                        log_info("ancs unknow attribute_id :%d", ancs_subevent_client_notification_get_attribute_id(packet));
                        break;
                    } else {
                        u16 attribute_strlen = little_endian_read_16(packet, 7);
                        u8 *attribute_str = (void *)little_endian_read_32(packet, 9);
                        //log_info("Notification: %s - %s \n", attribute_name, attribute_str);

                        MsgNotifyFromAncs(attribute_name, attribute_str, attribute_strlen);
    #if ANCS_MESSAGE_MANAGE_EN
                        extern void notice_set_info_from_ancs(void *name, void *data, u16 len);
                        notice_set_info_from_ancs(attribute_name, attribute_str, attribute_strlen);
    #endif
                    }
                    break;

                case ANCS_SUBEVENT_CLIENT_CONNECTED:
                    log_info("ANCS_SUBEVENT_CLIENT_CONNECTED\n");
                    break;

                case ANCS_SUBEVENT_CLIENT_DISCONNECTED:
                    log_info("ANCS_SUBEVENT_CLIENT_DISCONNECTED\n");
                    break;

                default:
                    break;
                }

                break;
    #endif

    #if TRANS_AMS_EN
            case HCI_EVENT_AMS_META:
                switch (packet[2]) {
                case AMS_SUBEVENT_CLIENT_NOTIFICATION: {
                    log_info("AMS_SUBEVENT_CLIENT_NOTIFICATION\n");
                    u16 Entity_Update_len = little_endian_read_16(packet, 7);
                    u8 *Entity_Update_data = (void *)little_endian_read_32(packet, 9);
                    /* log_info("EntityID:%d, AttributeID:%d, Flags:%d, utf8_len(%d):",\ */
                    /* Entity_Update_data[0],Entity_Update_data[1],Entity_Update_data[2],Entity_Update_len-3); */
                    log_info("%s(%s), Flags:%d, utf8_len(%d)", ams_get_entity_id_name(Entity_Update_data[0]),
                            ams_get_entity_attribute_name(Entity_Update_data[0], Entity_Update_data[1]),
                            Entity_Update_data[2], Entity_Update_len - 3);

    #if 1 //for printf debug
                    static u8 music_files_buf[128];
                    u8 str_len = Entity_Update_len - 3;
                    if (str_len > sizeof(music_files_buf)) {
                        str_len = sizeof(music_files_buf) - 1;
                    }
                    memcpy(music_files_buf, &Entity_Update_data[3], str_len);
                    music_files_buf[str_len] = 0;
                    printf("string:%s\n", music_files_buf);
    #endif

                    log_info_hexdump(&Entity_Update_data[3], Entity_Update_len - 3);
                    /* if (Entity_Update_data[0] == 1 && Entity_Update_data[1] == 2) { */
                    /* log_info("for test: send pp_key"); */
                    /* ams_send_request_command(AMS_RemoteCommandIDTogglePlayPause); */
                    /* } */
                }
                break;

                case AMS_SUBEVENT_CLIENT_CONNECTED:
                    log_info("AMS_SUBEVENT_CLIENT_CONNECTED\n");
                    break;

                case AMS_SUBEVENT_CLIENT_DISCONNECTED:
                    log_info("AMS_SUBEVENT_CLIENT_DISCONNECTED\n");
                    break;


                default:
                    break;
                }
                break;
    #endif
            }
            break;
    }
}

void ancs_update_status(u8 status)
{
    switch (status) 
    {
        case 1:
            log_info("ancs trunk start \n");
            break;

        case 2:
            log_info("ancs trunk end \n");
#if ANCS_MESSAGE_MANAGE_EN
            extern void notice_add_info_from_ancs();
            notice_add_info_from_ancs();
#endif
            break;

        default:
            break;
    }
}

/* LISTING_END */

/*
 * @section ATT Read
 *
 * @text The ATT Server handles all reads to constant data. For dynamic data like the custom characteristic, the registered
 * att_read_callback is called. To handle long characteristics and long reads, the att_read_callback is first called
 * with buffer == NULL, to request the total value length. Then it will be called again requesting a chunk of the value.
 * See Listing attRead.
 */


static uint16_t att_read_callback(hci_con_handle_t connection_handle, \
    uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size)
{

    uint16_t  att_value_len = 0;
    uint16_t handle = att_handle;

    /* log_info("READ Callback, handle %04x, offset %u, buffer size %u\n", handle, offset, buffer_size); */

    log_info("read_callback, handle= 0x%04x,buffer= %08x\n", handle, (u32)buffer);

    switch (handle) 
    {
        case ATT_CHARACTERISTIC_2a00_01_VALUE_HANDLE:
            att_value_len = strlen(gap_device_name);

            if((offset >= att_value_len) || \
                (offset + buffer_size) > att_value_len) 
                break;

            if(buffer) 
            {
                memcpy(buffer, &gap_device_name[offset], buffer_size);
                att_value_len = buffer_size;
                log_info("\n------read gap_name: %s \n", gap_device_name);
            }
            break;
#if TCFG_PAY_ALIOS_ENABLE
        case ATT_CHARACTERISTIC_4a02_01_UNKOWN_DESCRIPTION_HANDLE:
            att_value_len = 6;

            if(buffer) 
            {
                u8 tmp_data[6];
                le_controller_get_mac(tmp_data);
                swapX(tmp_data, &buffer[0], 6);
            }
            break;
#endif

#if TCFG_PAY_ALIOS_ENABLE
        case ATT_CHARACTERISTIC_4a02_01_CLIENT_CONFIGURATION_HANDLE:
#endif
        case ATT_CHARACTERISTIC_2a05_01_CLIENT_CONFIGURATION_HANDLE:
        case ATT_CHARACTERISTIC_ae02_01_CLIENT_CONFIGURATION_HANDLE:
        case ATT_CHARACTERISTIC_A6ED0102_D344_460A_8075_B9E8EC90D71B_01_CLIENT_CONFIGURATION_HANDLE:
            if(buffer) 
            {
                buffer[0] = att_get_ccc_config(handle);
                buffer[1] = 0;
            }
            att_value_len = 2;
            break;

        default:
            break;
    }

    log_info("att_value_len= %d\n", att_value_len);

    return att_value_len;
}


/* LISTING_END */
/*
 * @section ATT Write
 *
 * @text The only valid ATT write in this example is to the Client Characteristic Configuration, which configures notification
 * and indication. If the ATT handle matches the client configuration handle, the new configuration value is stored and used
 * in the heartbeat handler to decide if a new value should be sent. See Listing attWrite.
 */

/* LISTING_START(attWrite): ATT Write */
static int att_write_callback(hci_con_handle_t connection_handle, \
    uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, \
        uint8_t *buffer, uint16_t buffer_size)
{
    int result = 0;
    u16 tmp16;

    u16 handle = att_handle;

    switch (handle) 
    {
        case ATT_CHARACTERISTIC_2a00_01_VALUE_HANDLE:
            tmp16 = BT_NAME_LEN_MAX;
            if((offset >= tmp16) || (offset + buffer_size) > tmp16) 
                break;

            if(offset == 0)
                memset(gap_device_name, 0x00, BT_NAME_LEN_MAX);

            memcpy(&gap_device_name[offset], buffer, buffer_size);
            log_info("\n------write gap_name:");
            break;

#if TCFG_PAY_ALIOS_ENABLE
        case ATT_CHARACTERISTIC_4a02_01_CLIENT_CONFIGURATION_HANDLE:
            log_info("\n------write ccc 4a20:%04x, %02x\n", handle, buffer[0]);
            set_ble_work_state(BLE_ST_NOTIFY_IDICATE);
            att_set_ccc_config(handle, buffer[0]);
            /* check_connetion_updata_deal(); */
            break;
#endif

        case ATT_CHARACTERISTIC_ae02_01_CLIENT_CONFIGURATION_HANDLE:
        case ATT_CHARACTERISTIC_A6ED0102_D344_460A_8075_B9E8EC90D71B_01_CLIENT_CONFIGURATION_HANDLE:
#if (0 == BT_CONNECTION_VERIFY)
            JL_rcsp_auth_reset();
#endif
            set_ble_work_state(BLE_ST_NOTIFY_IDICATE);
            check_connetion_updata_deal();
            log_info("\n------write ccc:%04x, %02x\n", handle, buffer[0]);
            att_set_ccc_config(handle, buffer[0]);
            can_send_now_wakeup();
#if TCFG_BLE_BRIDGE_EDR_ENALBE
            if (buffer[0] == 1 && pair_reconnect_flag == 1) 
            {
                if(connect_remote_type == REMOTE_TYPE_IOS) 
                {
                    /*set tag,ios ble回连后，检查认证通过后发起edr连接*/
                    check_rcsp_auth_flag = 1;
                }
            }
#endif
            extern void rcsp_update_ancs_disconn_handler(void);
            rcsp_update_ancs_disconn_handler();
            break;

        case ATT_CHARACTERISTIC_2a05_01_CLIENT_CONFIGURATION_HANDLE:
            att_set_ccc_config(handle, buffer[0]);
            break;

#if 1
        case ATT_CHARACTERISTIC_ae01_01_VALUE_HANDLE:
            /* if (!JL_rcsp_get_auth_flag()) { */
            /*     JL_rcsp_auth_recieve(buffer, buffer_size); */
            /*     break; */
            /* } */
            printf("+++++++%s\n", __func__);
            if (app_recieve_callback) {
                app_recieve_callback(0, buffer, buffer_size);
            }
            //		JL_rcsp_auth_recieve(data, len);
            break;
#endif

        case ATT_CHARACTERISTIC_A6ED0103_D344_460A_8075_B9E8EC90D71B_01_VALUE_HANDLE:
            printf("--------%s\n", __func__);
            umeox_common_le_rev_cb(NULL, buffer, buffer_size);
            break;

#if TCFG_PAY_ALIOS_ENABLE
        case ATT_CHARACTERISTIC_4a02_01_VALUE_HANDLE:
            log_info("upay_ble_recv(%d):", buffer_size);
            log_info_hexdump(buffer, buffer_size);
            if(upay_recv_callback) 
            {
                upay_recv_callback(buffer, buffer_size);
            }
            /* upay_ble_send_data("9876543210",10);// */
            break;
#endif  //TCFG_PAY_ALIOS_ENABLE

        default:
            break;
    }

    return 0;
}

static int app_send_user_data(u16 handle, u8 *data, u16 len, u8 handle_type)
{
    ble_cmd_ret_e ret = APP_BLE_NO_ERROR;

    if (!con_handle) {
        return APP_BLE_OPERATION_ERROR;
    }

    ret = ble_user_cmd_prepare(BLE_CMD_ATT_SEND_DATA, 4, handle, data, len, handle_type);
    if (ret == BLE_BUFFER_FULL) {
        ret = APP_BLE_BUFF_FULL;
    }

    if (ret) {
        log_info("app_send_fail:%d !!!!!!\n", ret);
    }

    return (int)ret;
}


static int make_set_adv_data(void)
{
    return smartbox_make_set_adv_data();
}

static int make_set_rsp_data(void)
{
    return smartbox_make_set_rsp_data();
}


u8 *ble_get_gatt_profile_data(u16 *len)
{
    *len = sizeof(profile_data);
    return (u8 *)profile_data;
}

bool ble_switch_new_adv_interval(u16 adv_interval)
{
    if (set_adv_interval == adv_interval) {
        return true;
    }

    log_info("switch adv_interval:%d-->%d\n", adv_interval, set_adv_interval);
    set_adv_interval = adv_interval;

#if TCFG_PAY_ALIOS_ENABLE
    if (upay_mode_enable && upay_new_adv_enable) {
        //upay new adv模式
        log_info("upay new adv switch do\n");
        upay_ble_new_adv_enable(0);
        upay_ble_new_adv_enable(1);
        return true;
    }
#endif

    if (BLE_ST_ADV == get_ble_work_state()) {
        log_info("adv switch do\n");
        bt_ble_adv_enable(0);
        bt_ble_adv_enable(1);
    }
    return true;
}

//广播参数设置
static void advertisements_setup_init()
{
    uint8_t adv_type = 0;
    uint8_t adv_channel = 7;
    int   ret = 0;
    u16 adv_interval = set_adv_interval;//0x30;

    ble_op_set_adv_param(adv_interval, adv_type, adv_channel);


#if TCFG_PAY_ALIOS_ENABLE
    if (upay_mode_enable) {
        ret = upay_ble_adv_data_set();
    } else
#endif
    {
        ret |= make_set_adv_data();
        ret |= make_set_rsp_data();
    }

    if (ret) {
        puts("advertisements_setup_init fail !!!!!!\n");
        return;
    }

}


void ble_sm_setup_init(io_capability_t io_type, u8 auth_req, uint8_t min_key_size, u8 security_en)
{
    if (config_le_sm_support_enable) {
        //setup SM: Display only
        sm_init();
        sm_set_io_capabilities(io_type);
        sm_set_authentication_requirements(auth_req);
        sm_set_encryption_key_size_range(min_key_size, 16);
        sm_set_request_security(security_en);
        sm_event_callback_set(&cbk_sm_packet_handler);
    }
}

static void ancs_notification_message(u8 *packet, u16 size)
{
#if 0
    u8 *value;
    u32 ancs_notification_uid;
    value = &packet[8];
    ancs_notification_uid = little_endian_read_32(value, 4);
    log_info("Notification: EventID %02x, EventFlags %02x, CategoryID %02x, CategoryCount %u, UID %04x",
             value[0], value[1], value[2], value[3], little_endian_read_32(value, 4));

    if (value[1] & BIT(2)) {
        log_info("is PreExisting Message!!!");
    }

    if (value[0] == 2) { //0:added 1:modifiled 2:removed
        log_info("remove message:ancs_notification_uid %04x", ancs_notification_uid);
        extern void notice_remove_info_from_ancs(u32 uid);
        notice_remove_info_from_ancs(ancs_notification_uid);
    } else if (value[0] == 0) {
        extern void notice_set_info_from_ancs(void *name, void *data, u16 len);
        log_info("add message:ancs_notification_uid %04x", ancs_notification_uid);
        notice_set_info_from_ancs("UID", &ancs_notification_uid, sizeof(ancs_notification_uid));
    }
#endif
}

#define SMART_TCFG_BLE_SECURITY_EN          0/*是否发请求加密命令*/
extern void le_device_db_init(void);
void ble_profile_init(void)
{
    log_info("ble profile init\n");

#if 1
    le_device_db_init();

#if TCFG_BLE_BRIDGE_EDR_ENALBE
    ble_sm_setup_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, \
        SM_AUTHREQ_SECURE_CONNECTION | SM_AUTHREQ_BONDING, 7, \
            SMART_TCFG_BLE_SECURITY_EN);
#else
    ble_sm_setup_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, \
        SM_AUTHREQ_MITM_PROTECTION | SM_AUTHREQ_BONDING, 7, \
            SMART_TCFG_BLE_SECURITY_EN);
#endif

    /* setup ATT server */
    att_server_init(profile_data, att_read_callback, att_write_callback);
    att_server_register_packet_handler(cbk_packet_handler);
    /* gatt_client_register_packet_handler(packet_cbk); */

    // register for HCI events
    hci_event_callback_set(&cbk_packet_handler);
    /* ble_l2cap_register_packet_handler(packet_cbk); */
    /* sm_event_packet_handler_register(packet_cbk); */
    le_l2cap_register_packet_handler(&cbk_packet_handler);


#if TRANS_ANCS_EN || TRANS_AMS_EN
    if ((!config_le_sm_support_enable) || (!config_le_gatt_client_num)) {
        printf("ANCS need sm and client support!!!\n");
        ASSERT(0);
    }
#endif

    printf("client_num = %d\n", \
        config_le_gatt_client_num);
    if(config_le_gatt_client_num)
        gatt_client_init();

#if TRANS_ANCS_EN
    log_info("ANCS init...");
    //setup ANCS clent
    ancs_client_init();
    ancs_set_notification_buffer(ancs_info_buffer, sizeof(ancs_info_buffer));
    ancs_client_register_callback(&cbk_packet_handler);
    ancs_set_out_callback(ancs_notification_message);
#endif

#if TRANS_AMS_EN
    log_info("AMS init...");
    ams_client_init();
    ams_client_register_callback(&cbk_packet_handler);
    ams_entity_attribute_config(AMS_IDPlayer_ENABLE | AMS_IDQueue_ENABLE | AMS_IDTrack_ENABLE);
    /* ams_entity_attribute_config(AMS_IDTrack_ENABLE); */
#endif
#endif
}

void ble_profile_again_init(u8 enable)
{

    u8 tmp_ble_addr[6];

    if(enable) 
    {
        log_info("*********ble profile init\n");
        /* setup ATT server */
        att_server_init(profile_data, att_read_callback, att_write_callback);
        att_server_register_packet_handler(cbk_packet_handler);
        /* gatt_client_register_packet_handler(packet_cbk); */

        // register for HCI events
        hci_event_callback_set(&cbk_packet_handler);
        /* ble_l2cap_register_packet_handler(packet_cbk); */
        /* sm_event_packet_handler_register(packet_cbk); */
        le_l2cap_register_packet_handler(&cbk_packet_handler);
        /* bt_ble_init(); */

#if DOUBLE_BT_SAME_MAC
        memcpy(tmp_ble_addr, (void *)bt_get_mac_addr(), 6);
#else
        lib_make_ble_address(tmp_ble_addr, (void *)bt_get_mac_addr());
#endif
        le_controller_set_mac((void *)tmp_ble_addr);
        log_info("ble address:\n");
        log_info_hexdump(tmp_ble_addr, 6);

#if TRANS_ANCS_EN
        log_info("ANCS init...");
        ancs_client_init();
#endif

#if TRANS_AMS_EN
        log_info("AMS init...");
        ams_client_init();
#endif

#if (!SMBOX_MULTI_BLE_EN)
        bt_ble_adv_enable(1);
#else
        bt_ble_adv_enable(0);
#endif
    }else
    {
        bt_ble_adv_enable(0);
    }
}


static int set_adv_enable(void *priv, u32 en)
{
    ble_state_e next_state, cur_state;

    if(!adv_ctrl_en && en) {
        return APP_BLE_OPERATION_ERROR;
    }

    printf("con_handle %d\n", con_handle);
    
    if(con_handle) 
    {
        return APP_BLE_OPERATION_ERROR;
    }

#if TCFG_PAY_ALIOS_ENABLE
    if (upay_mode_enable) {
        /*upay模式,跳过spp检测,可以直接开ADV*/
        goto contrl_adv;
    }
#endif

    if(en) 
    {
        /*控制开adv*/
        if (get_app_connect_type() == TYPE_SPP) 
        {
            log_info("spp connect type\n");
            return APP_BLE_OPERATION_ERROR;
        }

        // 防止ios只连上ble的情况下，android(spp)回连导致ble断开后重新开广播的情况
        if(get_rcsp_connect_status()) 
        {
            log_info("spp is connecting\n");
            /* en = 0; */
            return APP_BLE_OPERATION_ERROR;
        }
    }

contrl_adv:

    if(en){
        next_state = BLE_ST_ADV;
    }else{
        next_state = BLE_ST_IDLE;
    }

    cur_state = get_ble_work_state();

    printf("get_ble_work_state  %d\n", cur_state);

    switch(cur_state) 
    {
        case BLE_ST_ADV:
        case BLE_ST_IDLE:
        case BLE_ST_INIT_OK:
        case BLE_ST_NULL:
        case BLE_ST_DISCONN:
            break;

        default:
            return APP_BLE_OPERATION_ERROR;
            break;
    }

    if(cur_state == next_state) 
    {
        log_info("cur_state == next_state \n");
        return APP_BLE_NO_ERROR;
    }

    log_info("adv_en:%d\n", en);

    set_ble_work_state(next_state);

    if(en){
        advertisements_setup_init();
    }

    ble_user_cmd_prepare(BLE_CMD_ADV_ENABLE, 1, en);

    log_info(">  set_adv_enable  %d\n", en);
    
    return APP_BLE_NO_ERROR;
}

int smartbox_set_adv_enable(void *priv, u32 en)
{
    return set_adv_enable(priv, en);
}

static int ble_disconnect(void *priv)
{
    if(con_handle) 
    {
        if(BLE_ST_SEND_DISCONN != get_ble_work_state()) 
        {
            log_info(">>>ble send disconnect\n");
            set_ble_work_state(BLE_ST_SEND_DISCONN);
            ble_user_cmd_prepare(BLE_CMD_DISCONNECT, 1, con_handle);
        }else 
        {
            log_info(">>>ble wait disconnect...\n");
        }

        return APP_BLE_NO_ERROR;
    }else 
    {
        return APP_BLE_OPERATION_ERROR;
    }
}


static int get_buffer_vaild_len(void *priv)
{
    u32 vaild_len = 0;
    ble_user_cmd_prepare(BLE_CMD_ATT_VAILD_LEN, 1, &vaild_len);
    return vaild_len;
}

static int app_send_user_data_do(void *priv, u8 *data, u16 len)
{
#if TCFG_BLE_BRIDGE_EDR_ENALBE
    if(check_rcsp_auth_flag) 
    {
        /*触发检测是否认证通过,通过后回连edr*/
        if(JL_rcsp_get_auth_flag()) 
        {
            check_rcsp_auth_flag = 0;
            log_info("auth_flag is ok\n");
            if(is_bredr_close() == 1) 
            {
                bredr_conn_last_dev();
            }else if(is_bredr_inquiry_state() == 1) 
            {
                user_send_cmd_prepare(USER_CTRL_INQUIRY_CANCEL, 0, NULL);
#if TCFG_USER_EMITTER_ENABLE
                log_info(">>>connect last source edr...\n");
                connect_last_source_device_from_vm();
#else
                log_info(">>>connect last edr...\n");
                connect_last_device_from_vm();
#endif
            }else 
            {
#if TCFG_USER_EMITTER_ENABLE
                log_info(">>>connect last edr...\n");
                connect_last_source_device_from_vm();
#else
                connect_last_device_from_vm();
#endif
            }
        }
    }
#endif

    // return app_send_user_data(ATT_CHARACTERISTIC_ae02_01_VALUE_HANDLE, \
    //     data, len, ATT_OP_AUTO_READ_CCC);

#if 1
    if(priv)
    {
        u8 att_service = *(u8 *)priv;
        //printf("____%s:%d\n", __func__, att_service);
        if(att_service == BLE_USER_SERVICE)
            return app_send_user_data(ATT_CHARACTERISTIC_A6ED0102_D344_460A_8075_B9E8EC90D71B_01_VALUE_HANDLE, \
                data, len, ATT_OP_AUTO_READ_CCC); 
    }

    //printf("......%s:OTA\n", __func__);

    return app_send_user_data(ATT_CHARACTERISTIC_ae02_01_VALUE_HANDLE, \
        data, len, ATT_OP_AUTO_READ_CCC);  
#endif
}

static int app_send_user_data_check(u16 len)
{
    u32 buf_space = get_buffer_vaild_len(0);
    if (len <= buf_space) {
        return 1;
    }
    return 0;
}

static int regiest_wakeup_send(void *priv, void *cbk)
{
    ble_resume_send_wakeup = cbk;
    return APP_BLE_NO_ERROR;
}

static int regiest_recieve_cbk(void *priv, void *cbk)
{
    channel_priv = (u32)priv;
    app_recieve_callback = cbk;
    return APP_BLE_NO_ERROR;
}

static int regiest_state_cbk(void *priv, void *cbk)
{
    channel_priv = (u32)priv;
    app_ble_state_callback = cbk;
    return APP_BLE_NO_ERROR;
}

void bt_ble_adv_enable(u8 enable)
{
    set_adv_enable(0, enable);
}

void ble_module_enable(u8 en)
{
    printf("%s:mode_en:%d\n", __func__, en);

    if (en) 
    {
        adv_ctrl_en = 1;
#if (TCFG_BLE_ADV_DYNAMIC_SWITCH && TCFG_UI_ENABLE)
        void le_adv_interval_reset(void);
        le_adv_interval_reset();
#endif
#if (!MUTIl_CHARGING_BOX_EN)
        bt_ble_adv_enable(1);
#endif
    }else 
    {
        if(con_handle) {
            adv_ctrl_en = 0;
            ble_disconnect(NULL);
        } else {
            bt_ble_adv_enable(0);
            adv_ctrl_en = 0;
        }
    }
}

const struct ble_server_operation_t ble_operation = 
{
    .adv_enable = set_adv_enable,
    .disconnect = ble_disconnect,
    .get_buffer_vaild = get_buffer_vaild_len,
    .send_data = (void *)app_send_user_data_do,
    .regist_wakeup_send = regiest_wakeup_send,
    .regist_recieve_cbk = regiest_recieve_cbk,
    .regist_state_cbk = regiest_state_cbk,
};

void ble_get_server_operation_table(struct ble_server_operation_t **interface_pt)
{
    *interface_pt = (void *)&ble_operation;
}

void bt_ble_init(void)
{
    log_info("***** ble_init******\n");

    if(ble_init_flag) 
    {
        return;
    }

    set_adv_interval = ADV_INTERVAL_MIN;

#if TCFG_PAY_ALIOS_ENABLE
    upay_mode_enable = 0;
    upay_new_adv_enable = 0;

#if UPAY_ONE_PROFILE
    if (config_btctler_le_hw_nums < 2) {
        log_info("error:need add hw to adv new device!!!\n");
        ASSERT(0);
    }
#endif

#endif

    gap_device_name = bt_get_local_name();
    gap_device_name_len = strlen(gap_device_name);
    if (gap_device_name_len > BT_NAME_LEN_MAX) {
        gap_device_name_len = BT_NAME_LEN_MAX;
    }

    set_ble_work_state(BLE_ST_INIT_OK);

    bt_ble_init_do();

    ble_vendor_set_default_att_mtu(\
        ATT_LOCAL_PAYLOAD_SIZE);

    bt_ble_rcsp_adv_enable();

#if (!SMBOX_MULTI_BLE_EN)
    ble_module_enable(1);
    log_info("ble name(%d): %s \n", \
        gap_device_name_len, gap_device_name);
#else
    ble_module_enable(0);
#endif

    ble_init_flag = 1;
}

void bt_ble_exit(void)
{
    log_info("***** ble_exit******\n");
    if(!ble_init_flag) {
        return;
    }

    ble_module_enable(0);
#if SMART_BOX_EN
    extern void smartbox_exit(void);
    smartbox_exit();
#endif
    ble_init_flag = 0;
}


void ble_app_disconnect(void)
{
    ble_disconnect(NULL);
}

hci_con_handle_t smartbox_get_con_handle(void)
{
    return con_handle;
}

#if TRANS_ANCS_EN
void hangup_ans_call_handle(u8 en)
{

    u32 notification_id;
    u16 control_point_handle;

    log_info("hang_up or answer\n");
    notification_id = get_notification_uid();
    control_point_handle = get_controlpoint_handle();
    u8 ble_hangup[] = {0x02, 0, 0, 0, 0, en};
    memcpy(&ble_hangup[1], &notification_id, 4);
    log_info_hexdump(ble_hangup, 6);
    u8 ble_hangup_size = 6;
    ble_op_att_send_data(control_point_handle, (void *)&ble_hangup, ble_hangup_size, ATT_OP_WRITE);

}
#endif


#if TCFG_PAY_ALIOS_ENABLE

#if UPAY_ONE_PROFILE
static void upay_ble_new_adv_enable(u8 en)
{
    if (upay_new_adv_enable == en) {
        return;
    }
    upay_new_adv_enable = en;
    if (en) {
        ble_op_set_adv_param(set_adv_interval, ADV_SCAN_IND, 7);/*just adv*/
        upay_ble_adv_data_set();
    }
    ble_user_cmd_prepare(BLE_CMD_ADV_ENABLE, 1, en);
    log_info(">>>new_adv_enable  %d\n", en);
}
#endif

/*upay recieve data regies*/
void upay_ble_regiest_recv_handle(void (*handle)(const uint8_t *data, u16 len))
{
    upay_recv_callback = handle;
}

/*upay send data api*/
int upay_ble_send_data(const uint8_t *data, u16 len)
{
    log_info("upay_ble_send(%d):", len);
    log_info_hexdump(data, len);
    if (att_get_ccc_config(ATT_CHARACTERISTIC_4a02_01_CLIENT_CONFIGURATION_HANDLE)) {
        return ble_op_att_send_data(ATT_CHARACTERISTIC_4a02_01_VALUE_HANDLE, data, len, ATT_OP_NOTIFY);
    }
    return BLE_CMD_CCC_FAIL;
}

/*open or close upay*/
void upay_ble_mode_enable(u8 enable)
{
    if (enable == upay_mode_enable) {
        return;
    }

    upay_mode_enable = enable;
    log_info("upay_mode_enable= %d\n", upay_mode_enable);

#if UPAY_ONE_PROFILE
    if (upay_mode_enable) {
        if (con_handle) {
            /*已连接,要开新设备广播*/
            ble_op_latency_skip(con_handle, 0xffff);
            upay_ble_new_adv_enable(1);
        } else {
            /*未连接，只切换原设备广播*/
            ble_module_enable(0);
            ble_module_enable(1);
        }
    } else {
        upay_ble_new_adv_enable(0);
        if (!con_handle) {
            /*未连接，只切换广播*/
            ble_module_enable(0);
            ble_module_enable(1);
        } else {
            ble_op_latency_skip(con_handle, 0);
        }
    }
#else
    ble_module_enable(0);
    if (upay_mode_enable) {
        att_server_change_profile(profile_data_upay);
#if TRANS_ANCS_EN
        ancs_client_exit();
#endif
#if TRANS_AMS_EN
        ams_client_exit();
#endif

    } else {
        att_server_change_profile(profile_data);
#if TRANS_ANCS_EN
        ancs_client_init();
#endif
#if TRANS_AMS_EN
        ams_client_init();
#endif

    }
    ble_module_enable(1);

#endif

}
#endif


#if (TCFG_BLE_ADV_DYNAMIC_SWITCH && TCFG_UI_ENABLE)
#include "ui/ui_api.h"

#define TCFG_BLE_ADV_TO_CLOSE		0 // 超时关闭

extern int lcd_sleep_status();

static u16 le_lcd_sleep_to = 0;

static void le_lcd_sleep_enter(void);
static void le_lcd_sleep_exit(void);

static int le_lcd_sleep_deal(int enter)
{
    /* printf("\n\n le_lcd_sleep_deal:%d \n\n", enter); */
    if (enter) {
#if TCFG_BLE_ADV_TO_CLOSE
        if (BLE_ST_IDLE == get_ble_work_state()) {
            ble_module_enable(1);
        } else
#endif
        {
            ble_switch_new_adv_interval(ADV_INTERVAL_MIN);
        }
    } else {
#if TCFG_BLE_ADV_TO_CLOSE
        if (BLE_ST_ADV == get_ble_work_state()) {
            ble_module_enable(0); // 如果BLE和EDR都关闭了，应该要创建一个TIMER，负责会按默认唤醒节奏唤醒
        } else
#endif
        {
            ble_switch_new_adv_interval(ADV_INTERVAL_MIN * 2);
        }

    }
    return 0;
}

static void le_lcd_sleep_to_deal(void *priv)
{
    /* printf("\n\n le_lcd_sleep_to_deal \n\n"); */
    le_lcd_sleep_to = 0;
    int msg[3];
    msg[0] = (int)le_lcd_sleep_deal;
    msg[1] = 1;
    msg[2] = (int)0;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
}

static void le_adv_interval_reset(void)
{
    set_adv_interval = ADV_INTERVAL_MIN;
    if (lcd_sleep_status()) { // 息屏中
        le_lcd_sleep_enter();
    }
}

static void le_lcd_sleep_enter(void)
{
    // 灭屏
    if (le_lcd_sleep_to) {
        sys_timeout_del(le_lcd_sleep_to);
        le_lcd_sleep_to = 0;
    }
    le_lcd_sleep_to = sys_timeout_add(NULL, le_lcd_sleep_to_deal, 1 * 60 * 1000L);
}

static void le_lcd_sleep_exit(void)
{
    // 亮屏
    if (le_lcd_sleep_to) {
        sys_timeout_del(le_lcd_sleep_to);
        le_lcd_sleep_to = 0;
    }
    int msg[3];
    msg[0] = (int)le_lcd_sleep_deal;
    msg[1] = 1;
    msg[2] = (int)1;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
}

REGISTER_LCD_SLEEP_HEADLER(le_lcd_sleep) = {
    .name = "vad",
    .enter = le_lcd_sleep_enter,
    .exit = le_lcd_sleep_exit,
};

#endif

#endif

