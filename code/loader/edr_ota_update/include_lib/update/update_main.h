#ifndef __UPDATE_MAIN_H__
#define __UPDATE_MAIN_H__

#include "typedef.h"
#include "tff.h"

extern u32 UPDATE_BEG;
#define UART_UPDATE_FLAG_ADDR   (u32)(&UPDATE_BEG)
#define UPDATE_FLAG_ADDR        ((u32)(&UPDATE_BEG) + 0x8)

#define UPDATE_PRIV_PARAM_LEN	32
#define UPDATA_MAGIC			(0x5A00)		//防止CRC == 0 的情况

// #define SEEK_SET	0	#<{(| Seek from beginning of file.  |)}>#
// #define SEEK_CUR	1	#<{(| Seek from current position.  |)}>#
// #define SEEK_END	2	#<{(| Seek from end of file.  |)}>#

//给SDK的升级结果
typedef enum {
    UPDATA_NON = UPDATA_MAGIC,
    UPDATA_READY,
    UPDATA_SUCCESSFULLY,
    UPDATA_PARM_ERR,
    UPDATA_DEV_ERR,
    UPDATA_KEY_ERR,
} UPDATA_RESULT_TO_SDK;

//区分是否跳转还是断电重新上电进入loader
typedef enum {
    UPDATE_JUMP = 0,
    UPDATE_POWERON,
} UPDATE_MODE;

typedef enum {
    UPDATE_PARM,
    UPDATE_START,
    UPDATE_END,
} UPDATE_STEP;

typedef enum {
    UPDATE_READ_TOUT_PARM = UPDATE_END + 1,	// 根据远端ini文件的配合更新读超时时间
    UPDATE_SDK_BEGIN_PARM,	// 开始升级
};

typedef enum {
    USB_UPDATA = UPDATA_MAGIC,		//0x5A00
    SD0_UPDATA,						//0x5A01
    SD1_UPDATA,
    PC_UPDATA,
    UART_UPDATA,
    BT_UPDATA,
    BLE_APP_UPDATA,
    SPP_APP_UPDATA,
    DUAL_BANK_UPDATA,
    BLE_TEST_UPDATA,
    NORFLASH_UPDATA,
    // BLE_UPDATA,
    USER_NORFLASH_UFW_UPDATA,

    DEV_UPDATE = 0xFFFE,
    NON_DEV_UPDATA = 0xFFFF,
} UPDATA_TYPE;

enum EXT_ARG_TYPE {
    EXT_LDO_TRIM_RES = 0,
    EXT_JUMP_FLAG,
    EXT_BTIF_MAC_ADDR, // 用于备份btif中的mac地址
    EXT_RF_PA_INFO,
    EXT_RESERVED_UPDATE, // 用于sdk传入参数控制是否升级预留区域
    EXT_TYPE_MAX = 0xff,
};


typedef enum _UPDATA_RESULT {
    UPDATA_RESULT_SUCC = 1,
    UPDATA_RESULT_FAIL,
    UPDATA_RESULT_KEY_ERR,
} UPDATA_RESULT;

//回报给测试盒的？
enum {
    BT_UPDATE_OVER = 0,
    BT_UPDATE_KEY_ERR,
    BT_UPDATE_CONNECT_ERR,
};

enum {
    UPDATE_ERR_NONE = 0,
    UPDATE_ERR_UFW_HEAD_CRC_ERR,
    UPDATE_ERR_NOT_FIND_LOADER_FILE,
    UPDATE_ERR_NOT_FIND_TARGET_LOADER,

    UPDATE_ERR_LOADER_HEAD_CRC_ERR = 4,
    UPDATE_ERR_MALLOC_ERR,
    UPDATE_ERR_LOADER_WRITE_ERR,
    UPDATE_ERR_FILE_HANDLE_ERR,

    UPDATE_ERR_LOADER_VERIFY_ERR = 8,
    UPDATE_ERR_NOT_FIND_FLASH_BIN,
    UPDATE_ERR_FLASH_HEAD_CRC_ERR,
    UPDATE_ERR_NOT_FIND_TARGET_FILE,

    UPDATE_ERR_KEY_ERR = 12,
    UPDATE_ERR_UBOOT_NOT_MATCH,
    UPDATE_ERR_READ_REMOTE_FILE_ERR,
    UPDATE_ERR_REMOTE_RES_FILE_CRC_ERR,

    UPDATE_ERR_CODE_VERIFY_ERR = 16,
    UPDATE_ERR_RES0_VERIFY_ERR,
    UPDATE_ERR_NOT_FIND_RESERVE_DIR_FILE,
    UPDATE_ERR_LOCAL_FILE_HEAD_CRC_ERR,

    UPDATE_ERR_LOCAL_FILE_DATA_CRC_ERR = 20,
    UPDATE_ERR_PARAM_ERR,
    UPDATE_ERR_BT_CFG_UPDATE_ERR,
    UPDATE_ERR_TONE_UPDATE_ERR,

    UPDATE_ERR_RESERVED_CONFIG_UPDATE_ERR = 24,
    UPDATE_ERR_PRODUCT_ID_NOT_MATCH,
    UPDATE_ERR_EX_DSP_UPDATE_ERR,
    UPDATE_ERR_CONN_ERR,

    UPDATE_ERR_ERASE_FAIL = 28,
    UPDATE_ERR_UPDATE_FILE_SIZE_ERR,
    UPDATE_ERR_ANC_CFG_UPDATE_ERR,
    UPDATE_ERR_ANC_COEF_UPDATE_ERR,

    UPDATE_ERR_RESERVED_ZONE_UPDATE_ERR = 32,

};

//for rcsp update
enum {
    DEV_CONN_STATE_DISCONNECTED = 0,
    DEV_CONN_STATE_CONNECTED,
    DEV_CONN_STATE_READY_UPDATE,
    DEV_CONN_STATE_START_UPDATE,
};

typedef struct _UPDATA_PARM {
    u16 parm_crc;
    u16 parm_type;				//UPDATA_TYPE:sdk pass parm to uboot
    u16 parm_result;			//UPDATA_TYPE:uboot return result to sdk
    u16 magic;
    u8 	file_patch[32];
    u8 	parm_priv[32];
    u32 ota_addr;
    u16	ext_arg_len;
    u16 ext_arg_crc;
} UPDATA_PARM;

typedef struct _update_op_api_t {
    int (*ch_init)(void (*resume_hdl)(void *priv), int (*sleep_hdl)(void *priv));
    u16(*f_open)(void);
    u16(*f_read)(void *fp, u8 *buff, u16 len);
    int (*f_seek)(void *fp, u8 type, u32 offset);
    u16(*f_stop)(u8 err);
    int (*notify_update_content_size)(u32 size);
    void (*ch_exit)(void *priv);
    void *priv;
} update_op_api_t;

typedef struct _update_mode_info_t {
    s32 type;
    void (*state_cbk)(u32 status, void *priv);
    const update_op_api_t *file_op;
    u8 task_en;
} update_mode_info_t;

void update_main();
void set_updata_result(u16 type, u16 result);
u8 fs_update_result_transition(u8 err);
extern u32 fs_dev_mount(u16 dev_type);
void update_reset(void);
u8 *update_param_ext_get(UPDATA_PARM *p, u8 ext_type);

#endif
