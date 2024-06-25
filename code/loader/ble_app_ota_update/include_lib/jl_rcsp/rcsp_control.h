#ifndef __RCSP_CONTROL_H__
#define __RCSP_CONTROL_H__

#define JL_RCSP_USER_COUSTOM_EN		0
#include "typedef.h"
//#include "custom_cfg.h"
//#include <le_user.h>
#if JL_RCSP_USER_COUSTOM_EN
#include "rcsp_user_api.h"
#endif

#define JL_CONTROLLER_POOL_SIZE 512

//for update cmd
#define JL_OPCODE_GET_DEVICE_UPDATE_FILE_INFO_OFFSET	0xe1	//rsp:addr[4]+len[2]
#define JL_OPCODE_INQUIRE_DEVICE_IF_CAN_UPDATE			0xe2	//req:len[2]+data[len]
#define JL_OPCODE_ENTER_UPDATE_MODE						0xe3	//
#define JL_OPCODE_EXIT_UPDATE_MODE						0xe4	//rsp:0x01(fail)
#define JL_OPCODE_SEND_FW_UPDATE_BLOCK					0xe5	//len:
#define JL_OPCODE_GET_DEVICE_REFRESH_FW_STATUS			0xe6
#define JL_OPCODE_SET_DEVICE_REBOOT						0xe7
#define JL_OPCODE_NOTIFY_TO_SWITCH_COMMUNICATION_WAY	0x0B   //
#define JL_OPCODE_NOTIFY_UPDATE_CONENT_SIZE				0xe8
#define JL_OPCODE_GET_MD5 								0xD4


#define JL_OPCODE_JL_PRIVATE							0xF0   //
#define JL_OPCODE_DATA_JL_PRIVATE						0xF1

//......................gpio.........................
//#ifndef UART_ENABLE
#define GET_GPIO_INFO_FLAG
//#endif

#define IO_GROUP_NUM        16
#define NUM_ERR             2
#define PARAM_ERR           1
#define NO_ERR              0
/*
typedef struct {
    __RW __u32 OUT;
    __RW __u32 IN;
    __RW __u32 DIR;
    __RW __u32 DIE;
    __RW __u32 PU;
    __RW __u32 PD;
    __RW __u32 HD;
} JL_PORT_TypeDef;

#define JL_PORTA_BASE       (ls_base + map_adr(0x01, 0x00))
#define JL_PORTA            ((JL_PORT_TypeDef   *)JL_PORTA_BASE)

#define JL_PORTB_BASE       (ls_base + map_adr(0x01, 0x08))
#define JL_PORTB            ((JL_PORT_TypeDef   *)JL_PORTB_BASE)

#define JL_PORTC_BASE       (ls_base + map_adr(0x01, 0x10))
#define JL_PORTC            ((JL_PORT_TypeDef   *)JL_PORTC_BASE)

#define JL_PORTD_BASE       (ls_base + map_adr(0X01, 0X18))
#define JL_PORTD            ((JL_PORT_TypeDef   *)JL_PORTD_BASE)
*/
//......................gpio.........................

typedef enum __DEVICE_REFRESH_FW_STATUS {
    DEVICE_UPDATE_STA_SUCCESS = 0,		//升级成功(default)
    DEVICE_UPDATE_STA_VERIFY_ERR,		//升级完校验代码出错(default)
    DEVICE_UPDATE_STA_FAIL,				//升级失败(default)
    DEVICE_UPDATE_STA_KEY_ERR,			//加密key不匹配
    DEVICE_UPDATE_STA_FILE_ERR,			//升级文件出错
    DEVICE_UPDATE_STA_TYPE_ERR,			//升级类型出错,仅code_type;
    DEVICE_UPDATE_STA_MAX_ERR,
} DEVICE_UPDATE_STA;

enum {
    REBOOT_STA_READY = 0,
    REBOOT_STA_NOT_ALLOW,
};

void rcsp_init(void (*)(void), void (*)(void));
void rcsp_dev_select(u8 type);
void function_change_inform(void);

bool common_msg_deal(u32 param);
bool ble_msg_deal(u32 param);
bool music_msg_deal(u32 param);
bool linein_msg_deal(u32 param);
bool rtc_msg_deal(u32 param);

u16 JL_rcsp_speech_data_send(u8 *buf, u16 len);

/* enum {
    RCSP_BLE,
    RCSP_SPP,
}; */

#define RCSP_BLE 0
#define RCSP_SPP 1

enum {
    ANDROID,
    APPLE_IOS,
};

struct ble_server_operation_t {
    int(*adv_enable)(void *priv, u32 enable);
    int(*disconnect)(void *priv);
    int(*get_buffer_vaild)(void *priv);
    int(*send_data)(void *priv, void *buf, u16 len);
    int(*regist_wakeup_send)(void *priv, void *cbk);
    int(*regist_recieve_cbk)(void *priv, void *cbk);
    int(*regist_state_cbk)(void *priv, void *cbk);
    int(*latency_enable)(void *priv, u32 enable);
    int(*get_packet_num_of_not_sent)(void *priv);
    int(*close)(void *priv);
};

struct JL_AI_VAR {
    u8  JL_ble_status;
    struct ble_server_operation_t *rcsp_ble;
    u8 JL_spp_status;
    struct spp_operation_t *rcsp_spp;
    volatile u8 speech_state;
    u32 feature_mask;
    u8 device_type;
    u8 phone_platform;
    void (*start_speech)(void);
    void (*stop_speech)(void);
    u8 err_report;
    volatile u8 file_browse_lock_flag;
    u32 return_msg;
    u8 spec_mode;
#if JL_RCSP_USER_COUSTOM_EN
    struct __rcsp_user_var *rcsp_user;
#endif
    volatile u8 rcsp_resume_cnt;
    volatile u16 rcsp_tick_cnt;
};

struct _SPEECH_OVER_DEAL {
    u8 last_task;
    u8 status;
};

typedef enum __JL_ATTR_TYPE {
    JL_ATTR_TYPE_PROTOCOL_VERSION = 0x0,
    JL_ATTR_TYPE_SYS_INFO,
    JL_ATTR_TYPE_EDR_INFO,
    JL_ATTR_TYPE_PLATFORM,

    JL_ATTR_TYPE_FUNCTION_INFO = 0x4,
    JL_ATTR_TYPE_DEV_VERSION,
    JL_ATTR_TYPE_SDK_TYPE,
    JL_ATTR_TYPE_UBOOT_VERSION,

    JL_ATTR_TYPE_FW_DOUBLE_BANK = 0x8,
    JL_ATTR_TYPE_FORCE_UPDATE_STATUS,
    JL_ATTR_TYPE_PID_VID,
#if 1//VER_INFO_EXT_CONUNT
    JL_ATTR_TYPE_AUTH_KEY,
    JL_ATTR_TYPE_PRO_CODE,
#endif
    JL_ATTR_TYPE_CUSTOM_VER = 31,
    JL_ATTR_TYPE_MAX,

} JL_ATTR_TYPE;

typedef struct _app_update_op_api_t {
    void *(*data_send)(void *priv, u8 btupdata_sw, u8 seek_type, u32 offset_addr, u16 len);
    void *(*notify_update_content_size)(void *priv, u32 size);
    void *(*dev_disconnect)(void *priv);
    int (*get_number_of_pkt_sending)(void *priv);
    void (*register_update_data_handle)(void (*handle)(u8 *buf, u16 len));
    void (*register_update_status_req_handle)(u8(*handle)(void));
    void (*register_conn_state_change_handle)(void (*handle)(void *priv, u8 state));
    void (*btstack_init)(void);
    void (*task_run)(void);
    void (*btstack_resume)(void);
    void (*dev_set_discoverable_mode)(u8 mode);
    void (*dev_close)(void);
} app_update_op_api_t;


extern struct JL_AI_VAR jl_ai_var;
extern struct _SPEECH_OVER_DEAL speech_deal_val;

extern void JL_controller_resp_udpate_status_request(u8 status);
extern void dev_get_update_opearation_table(app_update_op_api_t **interface_ptr);
#endif

