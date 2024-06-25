#include "update_main.h"
#include "asm/power/p33.h"
#include "common.h"
#include "msg.h"
#include "sys_timer.h"
#include "btctrler_api_for_update.h"
#include "testbox_update.h"
#include "jl_rcsp/rcsp_control.h"
#include "custom_cfg.h"
#include "asm/timer.h"
#include "JL_rcsp_protocol.h"

#define LOG_TAG_CONST       APP_UPDATE
#define LOG_TAG             "[APP_UPDATE]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"

#define UPDATE_RETRY_ERR_PACKET	1			//知否支持收漏包后重发
#define UPDATE_PRE_READ_EN	1               //是否支持预读操作
//-----------------------------extern start-------------------------------//
extern int le_controller_set_mac(void *addr);
extern void bt_ble_init(void);
extern void btstack_init(void);
extern void dynamic_mem_init(void *malloc_pool, int malloc_size);
extern int le_controller_get_mac(void *addr);
extern void ex_api_msg_show(u8 rcsp_state, u8 time_cnt, u8 max_cnt, u8 *addr);
extern void update_set_trim_info(u8 *trim);
//-----------------------------extern end--------------------------------//

//-----------------------------local start--------------------------------//
static u16 app_f_open(void);
static u16 app_f_read(void *fp, u8 *buf, u16 len);
static int app_f_seek(void *fp, u8 type, u32 offset);
static void app_update_state_cbk(u32 status, void *priv);
static int app_notify_update_content_size(u32 size);

#ifdef CONFIG_BTCTRLE_V2_ENABLE
#define MALLOC_POOL_SIZE    ((1024 + 308) * 6)
#else
#define MALLOC_POOL_SIZE    ((1024 + 308) * 4)
#endif
u32 malloc_pool[MALLOC_POOL_SIZE / 4];
u32 malloc_pool_size = MALLOC_POOL_SIZE;
update_mode_info_t update_mode_info;

update_op_api_t app_update_op_api = {
    .f_open = app_f_open,
    .f_read = app_f_read,
    .f_seek = app_f_seek,
    .notify_update_content_size = app_notify_update_content_size,
    .priv = (void *) &update_mode_info, //内部可访问update_mode_info的成员/函数
};

update_mode_info_t update_mode_info = {
    .type      = BLE_APP_UPDATA,
    .state_cbk = app_update_state_cbk,
    .file_op   = &app_update_op_api,
};

//-----------------------------local end--------------------------------//
#define     APP_STA_UPDATA_ACTIVE              0x0800
#define     APP_STA_UPDATA_REC_DATA            0x1000
#define     APP_STA_UPDATA_ACTIVE_SCUSS        0x2000
#define     APP_STA_UPDATA_ACCEPT              0x4000

typedef enum __UPDATA_STATUS {
    UPDATA_STA_DEV_SLEEP = 0,
    UPDATA_STA_DEV_DISCONNECTION,
    UPDATA_STA_DISCONNECTION_COMPLETED,
    UPDATA_STA_DEV_CONNECTION,
    UPDATA_STA_READY_UPDATE,
    UPDATA_STA_BFU_UPDATE_LOOP,
    UPDATA_STA_WAIT_FOR_REBOOT,
} _UPDATA_STATUS;

typedef struct __app_control_info {
    volatile u32 ble_update_state;
    volatile u8 btupdata_sw;
    u32 date_len;
    u32 need_rx_len;
    u32 flash_write_addr;
    u16 once_write_len;
    u16 once_rx_len;
    u8 *read_buf;
    u32 file_offset;
    u32 timeout_instant;       //等待超时时记录的时刻
} app_control_info_t;

typedef struct _update_pre_read_ctl {
    u8 pre_read_cnt;
    u8 pre_read_flag;
    volatile u8 buf_busy_flag;
    u32 curr_file_offset;
    u32 last_addr;
    u8 *buf0;
    u32 buf0_addr;
    u8 *buf1;
    u32 buf1_addr;
} update_pre_read_ctl_t;

#ifdef APP_USE_MOR_SPACE
#define ONCE_UPDATE_READ_LEN	4096
#else
#define ONCE_UPDATE_READ_LEN	512
#endif
#define CONTINUE_READ_TIMES	    5
#define APP_UPDATE_CMD_WAIT_TIMEOUT  (4000)     //ms

static volatile u32 g_read_timeout = APP_UPDATE_CMD_WAIT_TIMEOUT;

static u8 update_temp_buf[ONCE_UPDATE_READ_LEN * 2] __attribute__((aligned(0x4)));
static volatile update_pre_read_ctl_t update_pre_read;
static app_update_op_api_t *p_update_op = NULL;
#define __this	(p_update_op)

static volatile _UPDATA_STATUS u_ble_status = UPDATA_STA_DEV_SLEEP;
static volatile app_control_info_t app_control_info;
static u8 jl_device_update_status = DEVICE_UPDATE_STA_FAIL;
static void (*ble_update_msg_deal_handle)(void *hdl, int *msg) = NULL;

void ble_update_msg_deal_handle_register(void (*hdl)(void *priv, int *msg))
{
    ble_update_msg_deal_handle = hdl;
}

//用read_buf的后面512K作为预读buff,当出现开始预读标志的时候,先预读多一包,之前判断地址是否符合,若符合直接拷贝并预读下一包;
//1.判断开始预读条件;连续5次读512byte的时候,开始自动发送预读包;
//2.
//extern void XM_recieve_parse_resume(void);
enum {
    PRE_READ_FLAG_DISABLE = 0,
    PRE_READ_FLAG_TOGGLE_TO_DIS,
    PRE_READ_FLAG_TOGGLE_TO_EN,
    PRE_READ_FLAG_ENABLE,
};
static u32 dev_read_update_block_handle(u8 *buffer, u16 buffer_size)
{
    printf("-rx data:%x\n", buffer_size);
    //putchar('R');
    if (app_control_info.btupdata_sw == UPDATA_READ) {
        //log_info("-up read\n");
        //防止传入buffer小仍被越界写;
        if (app_control_info.need_rx_len !=  buffer_size) {
            log_info("\n\n\nLEN ERR!!!:%x\n\n\n", app_control_info.need_rx_len);
            putchar('L');
            return 0;
        }

        app_control_info.date_len = buffer_size;

        if ((app_control_info.need_rx_len == app_control_info.date_len)) {
#if UPDATE_PRE_READ_EN
            if (update_pre_read.pre_read_flag > PRE_READ_FLAG_DISABLE) {
                if (!(update_pre_read.buf_busy_flag & BIT(0))) {
                    memcpy(update_pre_read.buf0, buffer, app_control_info.date_len);
                    update_pre_read.buf_busy_flag |= BIT(0);
                    update_pre_read.buf0_addr = update_pre_read.curr_file_offset;
                } else if (!(update_pre_read.buf_busy_flag & BIT(1))) {
                    memcpy(update_pre_read.buf1, buffer, app_control_info.date_len);
                    update_pre_read.buf_busy_flag |= BIT(1);
                    update_pre_read.buf1_addr = update_pre_read.curr_file_offset;
                }

                putchar('0' + update_pre_read.buf_busy_flag);
                printf("info.data_len:%d\n", app_control_info.date_len);
                log_info("offset:%x busy:%x\n", update_pre_read.curr_file_offset, update_pre_read.buf_busy_flag);
                update_pre_read.curr_file_offset = update_pre_read.curr_file_offset - ONCE_UPDATE_READ_LEN;
                return 0;
            }
#endif
            memcpy(app_control_info.read_buf, buffer, app_control_info.date_len);
            app_control_info.btupdata_sw = UPDATA_ACCEPT_OK;
            app_control_info.ble_update_state |= APP_STA_UPDATA_ACCEPT;
        } else {
            putchar('N');
        }
    } else {
        putchar('K');
    }

    return 0;
}

static u8 dev_update_status_req_handle(void)
{
    log_info("-TRIGGER RETURN\n");
    if (app_control_info.btupdata_sw == UPDATA_STOP) {
        app_control_info.btupdata_sw = UPDATA_ACCEPT_OK;
        app_control_info.ble_update_state |= APP_STA_UPDATA_ACCEPT;
    } else if (app_control_info.btupdata_sw == UPDATA_STOP_KEYERR) {
        app_control_info.btupdata_sw = UPDATA_ACCEPT_OK;
        app_control_info.ble_update_state |= APP_STA_UPDATA_ACCEPT;
    }

    return jl_device_update_status;
}

static void ble_set_jl_update_status(u8 status)
{
    jl_device_update_status = status;
}

static int app_update_control(void *conn, u8 btupdata_sw, u32 len, u8 seek_type, u32 seek_offsize)
{
    void *ret;
    //printf("---addr:%x len:%x\n", seek_offsize, len);
    if (p_update_op && p_update_op->data_send) {
        ret = p_update_op->data_send(0, btupdata_sw, seek_type, seek_offsize, len);
    } else {
        log_error("p_update_op->data_send null\n");
        ret = NULL;
    }

    return (int)ret;
}

static int app_notify_update_content_size(u32 size)
{
    void *ret;
    if (p_update_op && p_update_op->notify_update_content_size) {
        printf("%s:%d\n", __func__, size);
        ret = p_update_op->notify_update_content_size(NULL, size);
    } else {
        log_error("notify_update_content_size handle is null\n");
        ret = NULL;
    }

    return (int)ret;
}

void update_pre_read_ctl_init(void)
{
    update_pre_read.pre_read_cnt = 0;
    update_pre_read.pre_read_flag = PRE_READ_FLAG_DISABLE;
    update_pre_read.last_addr = 0;
    update_pre_read.buf_busy_flag = 0;
    update_pre_read.buf0 = update_temp_buf;
    update_pre_read.buf1 = update_temp_buf + ONCE_UPDATE_READ_LEN;
}

u8 *get_update_pre_read_buf(u32 addr, u8 *buf,  u16 len)
{
    u8 *buf_ptr = NULL;

    if (update_pre_read.buf_busy_flag) {
        if ((update_pre_read.buf0_addr == addr) && (update_pre_read.buf_busy_flag & BIT(0))) {
            printf("-len:%d\n", len);
            memcpy(buf, update_pre_read.buf0, len);
            update_pre_read.buf_busy_flag &= ~BIT(0);
            buf_ptr = update_pre_read.buf0;
        } else if ((update_pre_read.buf1_addr == addr) && (update_pre_read.buf_busy_flag & BIT(1))) {
            printf("-len:%d\n", len);
            memcpy(buf, update_pre_read.buf1, len);
            update_pre_read.buf_busy_flag &= ~BIT(1);
            buf_ptr = update_pre_read.buf1;
        }
    }


    if (buf_ptr != NULL) {
        log_info("BUF_PTR:%x\n", buf_ptr);
    }
    return buf_ptr;
}

static u8 get_update_pre_read_flag(u32 addr, u16 len)
{
    //是否满足addr和len同时符合规律
    if ((addr > ONCE_UPDATE_READ_LEN) \
        && (addr == (update_pre_read.last_addr - ONCE_UPDATE_READ_LEN)) \
        && (len == ONCE_UPDATE_READ_LEN)) {
        if (PRE_READ_FLAG_DISABLE == update_pre_read.pre_read_flag) {
            update_pre_read.pre_read_cnt++;
            if (update_pre_read.pre_read_cnt >= CONTINUE_READ_TIMES) {
                update_pre_read.pre_read_flag = PRE_READ_FLAG_TOGGLE_TO_EN;
            }
        } else if (PRE_READ_FLAG_TOGGLE_TO_EN == update_pre_read.pre_read_flag) {
            update_pre_read.pre_read_flag = PRE_READ_FLAG_ENABLE;
        }

    } else {
        if (PRE_READ_FLAG_ENABLE == update_pre_read.pre_read_flag) {
            update_pre_read.pre_read_flag = PRE_READ_FLAG_TOGGLE_TO_DIS;
        } else if (PRE_READ_FLAG_TOGGLE_TO_DIS == update_pre_read.pre_read_flag || PRE_READ_FLAG_TOGGLE_TO_EN == update_pre_read.pre_read_flag) {
            update_pre_read.pre_read_flag = PRE_READ_FLAG_DISABLE;
        }
        //update_pre_read.pre_read_flag = 0;
        update_pre_read.pre_read_cnt = 0;
    }

    log_info("pre_read_flag:%x last:%x\n", update_pre_read.pre_read_flag, update_pre_read.last_addr);
    update_pre_read.last_addr = addr;

    return update_pre_read.pre_read_flag;
}

void ble_updata_rsp_complete(void)
{
    if (UPDATA_STA_BFU_UPDATE_LOOP != u_ble_status) {
        log_info("ble sta err2\n");
        return;
    }
    app_control_info.btupdata_sw = UPDATA_STOP;
    app_control_info.ble_update_state &= ~APP_STA_UPDATA_ACCEPT;
    app_control_info.date_len = 0;
    app_control_info.need_rx_len = 0;

    log_info("send zero addr\n");
    app_update_control(NULL, UPDATA_STOP, 0, 0, 0);
}

#if UPDATE_RETRY_ERR_PACKET
// 一般因为丢了某段收据而导致一包错误而触发的重发机制，都会需要接收四包的时间作为超时时间
static bool app_update_retry_handle(void)
{
    static u8 retry_cnt = 3;
    if (jl_rcsp_retry_handle(JL_ERR_PACKAGE_RETRY_OPT_QUERY) && retry_cnt) {
        retry_cnt--;
        return true;
    }
    if (3 != retry_cnt) {
        retry_cnt = 3;
    }
    return false;
}
#endif

u32 app_update_handler(u8 btupdata_sw, u8 *rec, u32 len)
{
    u32 t;
    u8 *pre_read_buf;

    app_control_info.timeout_instant = tmr_2ms_cnt_get() + g_read_timeout / 2;   //APP直接切到后台，响应不了命令，在这里添加超时
    //printf("sw:%x\n", btupdata_sw);
    while (1) {
        wdt_clr();
        if (tmr_2ms_cnt_get() >= app_control_info.timeout_instant) {           //已经到达超时的时刻，直接返回
#if UPDATE_RETRY_ERR_PACKET
            if (app_update_retry_handle()) {
                app_control_info.timeout_instant = tmr_2ms_cnt_get() + g_read_timeout / 2;
                continue;
            }
#endif
            log_info("already timeout.\n");
            return -1;
        }
        switch (btupdata_sw) {
        case UPDATA_WAIT_REBOOT:
        //log_info("tmr:%d wait_timeout_instant:%d\n", tmr_2ms_cnt_get(), app_control_info.wait_timeout_instant);
        case UPDATA_OPEN:
        case UPDATA_SEEK:
        case UPDATA_STOP:
        case UPDATA_STOP_KEYERR:
            if ((app_control_info.ble_update_state & APP_STA_UPDATA_ACCEPT) && (app_control_info.btupdata_sw == UPDATA_ACCEPT_OK)) {
                log_info("return form handler\n");
                return 1;
            }
            break;

        case UPDATA_READ:
#if UPDATE_PRE_READ_EN
            pre_read_buf = get_update_pre_read_buf((u32)rec, app_control_info.read_buf, len);
            if (NULL != pre_read_buf) {
                return len;
            }
#endif
            if ((app_control_info.ble_update_state & APP_STA_UPDATA_ACCEPT) &&
                (app_control_info.btupdata_sw == UPDATA_ACCEPT_OK)) {
                //(app_control_info.date_len == len)) {
                app_control_info.read_buf = NULL;
                if (0 == app_control_info.date_len) {
                    log_info("-UPDATE READ ERR!\n");
                }
                return app_control_info.date_len;
            }
            break;

        default:
            break;
            //return 0;

        }

        if (UPDATA_STA_DISCONNECTION_COMPLETED == u_ble_status) {
            log_info("RETURN DUE TO DISCON\n");
            return 0;
        } else if (UPDATA_STA_READY_UPDATE == u_ble_status) {
            log_info("RETURN DUE TO ATT_READY\n");
            return 0;
        }

        if (t++ == 100000) {
            putchar('.');
            t = 0;
        }

    }

    log_info("EXCEPTION RETURN\n");
    return 0;
}

u16 app_updata_stop(u8 err)
{
    //return 1;
    u16 stop_state;
    if (UPDATA_STA_BFU_UPDATE_LOOP != u_ble_status) {
        log_info("ble sta err0\n");
        return 0;
    }
    if ((err >= DEVICE_UPDATE_STA_SUCCESS) && (err < DEVICE_UPDATE_STA_MAX_ERR)) {
        stop_state = app_update_handler(UPDATA_STOP, NULL, 0);
        JL_controller_resp_udpate_status_request(err);
        return stop_state;

    } else {
        log_info("-UPDATE STOP CODE ERR!\n");
        return 0;
    }
}

u32 app_updata_read(void *handle, void *buf, u32 btr, u8 read_type, u8 seek_type, u32 seek_offsize)
{
    u32 read_len = 0;
    if (UPDATA_STA_BFU_UPDATE_LOOP != u_ble_status) {
        log_info("ble sta err3\n");
        return 0;
    }
    u32 file_offset;

#if UPDATE_PRE_READ_EN
    u32 last_addr = update_pre_read.last_addr;
    u8 pre_read_flag = get_update_pre_read_flag(seek_offsize, btr);
    if (pre_read_flag == PRE_READ_FLAG_TOGGLE_TO_DIS) {
        printf("wait last buff\n");
        read_len = app_update_handler(UPDATA_READ, (u8 *)(last_addr - ONCE_UPDATE_READ_LEN), btr);
        printf("wait last buff11\n");
        update_pre_read.pre_read_flag = PRE_READ_FLAG_DISABLE;
    }
    file_offset = (PRE_READ_FLAG_ENABLE == pre_read_flag) ? seek_offsize - ONCE_UPDATE_READ_LEN : seek_offsize;
    app_control_info.read_buf = buf;
    app_control_info.btupdata_sw = UPDATA_READ;
    app_control_info.ble_update_state &= ~APP_STA_UPDATA_ACCEPT;
    app_control_info.date_len = 0;
    app_control_info.need_rx_len = btr;
    app_update_control(NULL, UPDATA_READ_OFFSIZE, btr, seek_type, file_offset);
#else
    app_control_info.read_buf = buf;
    app_control_info.btupdata_sw = UPDATA_READ;
    app_control_info.ble_update_state &= ~APP_STA_UPDATA_ACCEPT;
    app_control_info.date_len = 0;
    app_control_info.need_rx_len = btr;
    app_update_control(NULL, UPDATA_READ_OFFSIZE, btr, seek_type, seek_offsize);
#endif

#if UPDATE_PRE_READ_EN
    if (PRE_READ_FLAG_TOGGLE_TO_EN == pre_read_flag) {
        log_info("only send one time\n");
        update_pre_read.curr_file_offset = seek_offsize;
        app_update_control(NULL, UPDATA_READ_OFFSIZE, btr, seek_type, seek_offsize - ONCE_UPDATE_READ_LEN);

    }
#endif
    log_info("addr=0x%x,0x%x\n", seek_offsize, app_control_info.need_rx_len);
    read_len = app_update_handler(UPDATA_READ, (u8 *)seek_offsize, btr);
    return read_len;
}

u16 app_f_open(void)
{
    app_control_info.file_offset = 0;
#if UPDATE_PRE_READ_EN
    update_pre_read_ctl_init();
#endif
    return 0;
}

u16 app_f_read(void *fp, u8 *buff, u16 len)
{
    return app_updata_read(NULL, buff, len, 0, 0, app_control_info.file_offset);
}

int app_f_seek(void *fp, u8 type, u32 offsize)
{
    if (type == SEEK_SET) {
        app_control_info.file_offset = offsize;
    } else if (type == SEEK_CUR) {
        app_control_info.file_offset += offsize;
    }

    return 0;//FR_OK;
}

void app_update_result_report(u8 err)
{
    u8 res =  DEVICE_UPDATE_STA_SUCCESS;
    switch (err) {
    case UPDATE_ERR_NONE:
        res = DEVICE_UPDATE_STA_SUCCESS;
        break;

    case UPDATE_ERR_KEY_ERR:
        res = DEVICE_UPDATE_STA_KEY_ERR;
        break;

    //升级文件头校验失败
    case UPDATE_ERR_UFW_HEAD_CRC_ERR:
    case UPDATE_ERR_LOADER_HEAD_CRC_ERR:
    case UPDATE_ERR_FLASH_HEAD_CRC_ERR:
    case UPDATE_ERR_REMOTE_RES_FILE_CRC_ERR:
        res = DEVICE_UPDATE_STA_FILE_ERR;
        break;

    //flash头文件校验失败
    case UPDATE_ERR_LOCAL_FILE_HEAD_CRC_ERR:
        res = DEVICE_UPDATE_STA_FILE_ERR;
        break;

    //找不到目标文件
    case UPDATE_ERR_NOT_FIND_LOADER_FILE:
    case UPDATE_ERR_NOT_FIND_TARGET_LOADER:
    case UPDATE_ERR_NOT_FIND_FLASH_BIN:
    case UPDATE_ERR_NOT_FIND_RESERVE_DIR_FILE:
    case UPDATE_ERR_NOT_FIND_TARGET_FILE:
        res = DEVICE_UPDATE_STA_FILE_ERR;
        break;

    //文件操作错误
    case UPDATE_ERR_MALLOC_ERR:
    case UPDATE_ERR_LOADER_WRITE_ERR:
    case UPDATE_ERR_FILE_HANDLE_ERR:
    case UPDATE_ERR_READ_REMOTE_FILE_ERR:
        res = DEVICE_UPDATE_STA_FILE_ERR;
        break;

    //文件内容校验失败
    case UPDATE_ERR_LOADER_VERIFY_ERR:
        res = DEVICE_UPDATE_STA_VERIFY_ERR;
        break;

    //flash数据校验失败
    case UPDATE_ERR_LOCAL_FILE_DATA_CRC_ERR:
        res = DEVICE_UPDATE_STA_FILE_ERR;
        break;

    //uboot不匹配
    case UPDATE_ERR_UBOOT_NOT_MATCH:
        res = DEVICE_UPDATE_STA_TYPE_ERR;
        break;

    case UPDATE_ERR_PRODUCT_ID_NOT_MATCH:
        res = DEVICE_UPDATE_STA_FILE_ERR;
        break;
    default:
        res = DEVICE_UPDATE_STA_FAIL;
        break;
    }

    app_updata_stop(res);
}

static void dev_conn_state_change_handle(void *priv, u8 state)
{
    log_info("dev_conn_state:%x\n", state);

    switch (state) {
    case DEV_CONN_STATE_READY_UPDATE:
        u_ble_status = UPDATA_STA_READY_UPDATE;
        break;
    case DEV_CONN_STATE_DISCONNECTED:
        u_ble_status = UPDATA_STA_DISCONNECTION_COMPLETED;
        break;
    case DEV_CONN_STATE_CONNECTED:
        u_ble_status = UPDATA_STA_DEV_CONNECTION;
        break;
    case DEV_CONN_STATE_START_UPDATE:
        log_info("--UPDATE_START\n");
        u_ble_status = UPDATA_STA_BFU_UPDATE_LOOP;
        break;
    default:
        break;
    }
}

void ble_updata_init()
{
    dynamic_mem_init(malloc_pool, malloc_pool_size);
    task_message_init();
    btstack_init();
    bt_ble_init();
    sys_timer_add(NULL, stack_run_loop_resume, 5);
    init_soft_interrupt();                   //btstack loop软中断初始化

    dev_get_update_opearation_table(&__this);

    ASSERT(__this);

    __this->register_update_data_handle(dev_read_update_block_handle);
    __this->register_update_status_req_handle(dev_update_status_req_handle);
    __this->register_conn_state_change_handle(dev_conn_state_change_handle);

    rcsp_init(NULL, NULL);
}

static u8 reboot_status = REBOOT_STA_NOT_ALLOW;
u8 jl_update_get_reboot_sta(void)
{
    printf("get reboot sta\n");
    if (app_control_info.btupdata_sw == UPDATA_WAIT_REBOOT) {
        app_control_info.btupdata_sw = UPDATA_ACCEPT_OK;
        app_control_info.ble_update_state |= APP_STA_UPDATA_ACCEPT;
    } else {
        log_info("#BLE STATUS ERR\n");
    }
    return reboot_status;
}

void jl_update_set_reboot_status(u8 status)
{
    reboot_status = status;
}

static void ble_update_wait_reboot_cmd(void)
{
    log_info("-wait reboot cmd\n");
    app_control_info.btupdata_sw = UPDATA_WAIT_REBOOT;
    app_control_info.ble_update_state &= ~APP_STA_UPDATA_ACCEPT;
    app_update_handler(UPDATA_WAIT_REBOOT, NULL, 0);
}

static void ble_update_wait_disconnect(void)
{
    if (__this && __this->get_number_of_pkt_sending) {
        while (__this->get_number_of_pkt_sending(NULL)) {
            log_info("w");
            wdt_clr();
            if (__this->task_run) {
                __this->task_run();
            }
        }
    }

    if (__this->dev_disconnect) {
        __this->dev_disconnect(NULL);
        //ble_updata_handler(0xff, NULL, 0);
    } else {
        ASSERT(__this->dev_disconnect);
    }
}

static void app_update_ext_state_cbk(u32 status, void *priv)
{
    switch (status) {
    case UPDATE_READ_TOUT_PARM:
        g_read_timeout = *((u32 *)priv);
        printf("OTA_TOUT %dms\n", g_read_timeout);
        // 计算超时时间时是除以2，所以这里要*2
        g_read_timeout *= 2;
        break;
#if UPDATE_RETRY_ERR_PACKET
    case UPDATE_SDK_BEGIN_PARM:
        jl_rcsp_retry_handle(JL_ERR_PACKAGE_RETRY_OPT_FLAG_ENABLE);
        break;
#endif
    }
}

static u16 g_reset_without_conn_time = 0;
static void get_reset_without_conn_time_by_update_pram(UPDATA_PARM *p)
{
    u8 *reset_time = update_param_ext_get(p, EXT_RESET_TIME_WITHOUT_CONN);
    if (reset_time) {
        g_reset_without_conn_time = (reset_time[0] & (~BIT(7))) * 60;
        if (reset_time[0] >> 7) {
            g_reset_without_conn_time |= BIT(15);
        }
    }
}
static void reset_without_conn(void)
{
    u16 reset_time = g_reset_without_conn_time & (~BIT(15));
    printf("%s, %d\n", __func__, reset_time);
    switch (reset_time) {
    case 0:
        return;
    case 1:
        if (g_reset_without_conn_time >> 15) {
            clr_update_loader_record();
            set_updata_result(BLE_APP_UPDATA, UPDATA_DEV_ERR);
        }
        update_reset();
    default:
        g_reset_without_conn_time--;
        break;
    }
}
static u8 app_update_before_conn(u8 time_cnt, u8 *ble_mac_addr)
{
    static u32 timeout_instant = 0;
    u8 max_cnt = 20;
    // 500ms触发
    if (0 == timeout_instant || tmr_2ms_cnt_get() == timeout_instant) {
        timeout_instant = tmr_2ms_cnt_get() + 250;
        // 等待上位机触发升级
        // 10s内显示等待连接或等待升级，10s后显示等待连接或提示手动触发升级
        if (max_cnt > time_cnt) {
            time_cnt++;
        }
        reset_without_conn();
        ex_api_msg_show(UPDATA_STA_READY_UPDATE == u_ble_status, time_cnt, max_cnt, ble_mac_addr);
    }
    return time_cnt;
}

void app_update_state_cbk(u32 status, void *priv)
{
    UPDATA_PARM *p = priv;
    u8 ble_mac_addr[6] = {0x11, 0x22, 0x33, 0x33, 0x22, 0x11};
    switch (status) {
    case UPDATE_PARM:       //升级需要的参数
        ex_cfg_start_addr_set(*((u32 *)(p->parm_priv)));                                    //设置exif地址
        ex_cfg_get_content_api(CFG_ITEM_BLE_ADDR, ble_mac_addr, sizeof(ble_mac_addr));      //从exif中获取地址信息
        puts("ble_mac_addr:\n");
        put_buf(ble_mac_addr, sizeof(ble_mac_addr));
        le_controller_set_mac(ble_mac_addr);//BLE广播地址
#if defined(CONFIG_CPU_BR36)
#ifdef CONFIG_BTCTRLE_V2_ENABLE
        update_set_trim_info(update_param_ext_get(priv, EXT_LDO_TRIM_RES));
#endif
#endif
        get_reset_without_conn_time_by_update_pram(priv);
        break;

    case UPDATE_START:
        log_info("BT_UPDATE_START...\n");
        u8 time_cnt = 0;
        // 还原超时时间，防止升级断开后，拿另一个没有设置超时时间的升级文件进行升级
        g_read_timeout = APP_UPDATE_CMD_WAIT_TIMEOUT;
        ble_updata_init();
        le_controller_get_mac(ble_mac_addr);
        while (UPDATA_STA_BFU_UPDATE_LOOP != u_ble_status) {
            wdt_clr();
            time_cnt = app_update_before_conn(time_cnt, ble_mac_addr);
        }
        jl_update_set_reboot_status(REBOOT_STA_NOT_ALLOW);
        break;

    case UPDATE_END:		//升级结束需要保存结果到Ram给SDK获取，并回复主机升级结果
        log_info("UPDATE_END\n");
#if UPDATE_RETRY_ERR_PACKET
        jl_rcsp_retry_handle(JL_ERR_PACKAGE_RETRY_OPT_FLAG_DISENABLE);
#endif
        ble_updata_rsp_complete();
        app_update_result_report(*((u8 *)priv));

        if (*((u8 *)priv) == UPDATE_ERR_NONE) {
            set_updata_result(BLE_APP_UPDATA, UPDATA_SUCCESSFULLY);
            jl_update_set_reboot_status(REBOOT_STA_READY);
            u_ble_status = UPDATA_STA_WAIT_FOR_REBOOT;
            ble_update_wait_reboot_cmd();
        } else {
            set_updata_result(BLE_APP_UPDATA, UPDATA_DEV_ERR);
        }

        ble_update_wait_disconnect();
        udelay(200 * 1000);     //200ms
        u_ble_status = UPDATA_STA_DISCONNECTION_COMPLETED;
        putchar('f');

        /* cpu_reset(); */
        update_reset();
        break;
    default:
        app_update_ext_state_cbk(status, priv);
        break;
    }
}
