#include "update_main.h"
#include "asm/power/p33.h"
#include "common.h"
#include "msg.h"
#include "sys_timer.h"
#include "btctrler_api_for_update.h"
#include "testbox_update.h"
#include "platform_config.h"

#define LOG_TAG_CONST       BT_UPDATE
#define LOG_TAG             "[BT_UPDATE]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"

//extern
extern u32 bt_ram_begin;
extern u32 bt_ram_size;
extern void dynamic_mem_init(void *malloc_pool, int malloc_size);
extern void update_set_trim_info(u8 *trim);
extern u32 read_slot_clk_with_timeout(u8 ms_cnt);
extern int __bt_updata_connection_resume(u32 time_slice);
//extern end

static u16 bt_f_open(void);
static u16 bt_f_read(void *fp, u8 *buf, u16 len);
static int bt_f_seek(void *fp, u8 type, u32 offset);
static u16 bt_send_update_len(u32 update_len);
static void bt_update_state_cbk(u32 status, void *priv);
static testbox_update_info update_info;
#define __this		(&update_info)

#ifdef CONFIG_BTCTRLE_V2_ENABLE
#define MALLOC_POOL_SIZE    (1024 * 4)
#else
#define MALLOC_POOL_SIZE    (1024 * 2)
#endif

u32 malloc_pool[MALLOC_POOL_SIZE / 4];
u32 malloc_pool_size = MALLOC_POOL_SIZE;

update_op_api_t bt_op_api = {
    .f_open = bt_f_open,
    .f_read = bt_f_read,
    .f_seek = bt_f_seek,
    .notify_update_content_size = bt_send_update_len,
};

update_mode_info_t update_mode_info = {
    .type      = BT_UPDATA,
    .state_cbk = bt_update_state_cbk,
    .file_op   = &bt_op_api,
};

//local end

u16 bt_f_open(void)
{
    u8 remin_cnt = 0;
    u16 prf_cnt = 0;
    //INIT_LIST_HEAD(&read_buf_list);
#if 0
    read_buf_list_init();
#endif
RE_OPEN:
    bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
    log_info("---------UPDATA_OPEN----------\n");
    __this->file_offset = 0;
    __this->seek_type = SEEK_SET;
    if (__this->bt_reinit) {
        bt_updata_control(UPDATA_OPEN, 0, 0, 0);
        __this->bt_time_timeout = 0;
        while (0 == (BIT(UPDATA_REV_DATA) & bt_updata_get_flag())) {
            if (NULL == lmp_conn_for_address(NULL)) {
                log_info("---lmp discon\n");
                return 1;
            }
            if (__this->bt_time_timeout > UPDATE_CMD_WAIT_TIMEOUT) {
                wdt_clr();//清看门狗^
                log_info("---------UPDATA_open timerout----------\n");
                bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
                remin_cnt++;
                if (remin_cnt > 2) {
                    return 1;
                } else {
                    goto RE_OPEN;
                }
            }
        }

        bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
    }
    return 0;
}

static int bt_updata_handle(void *buf, int len)
{
    if (__this->read_buf) {
        memcpy(__this->read_buf, buf, len);
        __this->data_len = len;
    }
    return 0;
}

u16 bt_f_read(void *fp, u8 *buff, u16 len)
{
    __this->data_len = 0;
    __this->read_buf = buff;
    u8 remin_cnt = 0;

    u16 prf_cnt = 0;
re_send:
    bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
    //printf("---------UPDATA_read type %d, offsize %d----------\n", __this->seek_type, __this->file_offset);
    //log_info("ADDR:%x len:%x\n", __this->file_offset, len);
    bt_updata_control(UPDATA_READ_OFFSIZE, len, __this->seek_type, __this->file_offset);
    __this->bt_time_timeout = 0;
    while (0 == (BIT(UPDATA_REV_DATA) & bt_updata_get_flag())) {
        if (NULL == lmp_conn_for_address(NULL)) {
            log_error("---lmp discon\n");
            __this->data_len = 0;
            break;
        }

        if (__this->bt_time_timeout > UPDATE_CMD_WAIT_TIMEOUT) {
            wdt_clr();//清看门狗^
            bt_updata_clr_flag(UPDATA_REV_DATA);
            putchar('O');
            remin_cnt++;
            if (remin_cnt > 2) {
                __this->data_len = 0;
                break;
            } else {
                goto re_send;
            }
        }
    }

    bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
    if (__this->data_len == 0) {
        return (u16) - 1;
    }

    __this->file_offset += len;

    return len;
}

int bt_f_seek(void *fp, u8 type, u32 offset)
{
    if (type == SEEK_SET) {
        __this->file_offset = offset;
        __this->seek_type = BT_SEEK_SET;
    } else if (type == SEEK_CUR) {
        __this->file_offset += offset;
        __this->seek_type = BT_SEEK_CUR;
    }
    return 0;//FR_OK;
}

u16 bt_f_stop(u8 err)
{
    log_info("---------bt_f_stop ---------- 0x%x\n", err);
    bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
    if (BT_UPDATE_OVER == err) {
        bt_updata_control(UPDATA_STOP, 0, 0, 0);
    } else if (BT_UPDATE_KEY_ERR == err) {
        bt_updata_control(UPDATA_STOP_KEYERR, 0, 0, 0);
    } else {
        bt_updata_control(UPDATA_STOP, err, 0, 0);
    }
    __this->bt_time_timeout = 0;
    while (0 == (BIT(UPDATA_REV_DATA) & bt_updata_get_flag())) {
        if (NULL == lmp_conn_for_address(NULL)) {
            log_info("---lmp discon\n");
            return 0;
        }
        if (__this->bt_time_timeout > UPDATE_CMD_WAIT_TIMEOUT) {
            wdt_clr();//清看门狗^
            log_error("---------UPDATA_STOP timerout----------\n");
            bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
            return 0;
        }
    }
    bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
    return 1;
}

u16 bt_send_update_len(u32 update_len)
{
    u8 remin_cnt = 0;
re_send:
    bt_updata_clr_flag(UPDATA_REV_DATA);    //clr flag
    log_info("---------send_update_len:%x----------\n", update_len);
    bt_updata_control(UPDATA_SEEK, 0, BT_SEEK_TYPE_UPDATE_LEN, update_len);
    __this->bt_time_timeout = 0;
    while (0 == (BIT(UPDATA_REV_DATA) & bt_updata_get_flag())) {
        if (NULL == lmp_conn_for_address(NULL)) {
            log_error("---lmp discon\n");
            break;
        }
        if (__this->bt_time_timeout > UPDATE_CMD_WAIT_TIMEOUT) {
            wdt_clr();//清看门狗^
            log_error("---------send_UPDATA_update_len timeout----------\n");
            bt_updata_clr_flag(UPDATA_REV_DATA);	//clr flag
            remin_cnt++;
            if (remin_cnt > 2) {
                return 0;
            } else {
                goto re_send;
            }
        }
    }

    bt_updata_clr_flag(UPDATA_REV_DATA);    //clr flag
    return 1;
}

__attribute__((noinline))
void bt_update_result_report(u8 err)
{
    u8 res;
    switch (err) {
    case UPDATE_ERR_NONE:
        res = BT_UPDATE_OVER;
        break;

    case UPDATE_ERR_KEY_ERR:
        res = BT_UPDATE_KEY_ERR;
        break;

    default:
        res = fs_update_result_transition(err);
        break;
    }

    log_info("f_stop:%x\n", res);
    bt_f_stop(res);
}

static u8 g_edr_addr[6];
extern void ex_api_msg_show(u8 rcsp_state, u8 time_cnt, u8 max_cnt, u8 *addr);
static u8 testbox_update_before_conn(u8 time_cnt)
{
    u8 max_cnt = 20;
    static u32 timeout_instant = 0;
    if (0 == timeout_instant || __this->bt_time_timeout == timeout_instant) {
        timeout_instant = __this->bt_time_timeout + 50;
        if (max_cnt > time_cnt) {
            time_cnt++;
        }
        ex_api_msg_show(bt_updata_get_flag()&BIT(UPDATA_START), time_cnt, max_cnt, g_edr_addr);
    }
    return time_cnt;
}

//重新初始化之后需要等待UPDATE_START才能继续往下走
void bt_update_wait_start(void)
{
    u8 time_cnt = 0;
    bt_updata_clr_flag(UPDATA_START);
    while (1) {
        wdt_clr();//清看门狗
        time_cnt = testbox_update_before_conn(time_cnt);
        if (bt_updata_get_flag()&BIT(UPDATA_START)) {
            bt_updata_clr_flag(UPDATA_START);
            lmp_update_init();
            break;
        }
    }
}

void baseband_init()
{
    //bt_reset_cap(0xa, 0xa);
    //不初始化，断电后会搜不到蓝牙;
    //bt_set_osc_cap(0xa, 0xa);
    bt_rf_init();
    bredr_bd_init();
    bredr_link_init();
    lmp_init();
    lmp_hci_write_scan_enable(0x3);
}

//重新初始化baseband
void bt_update_reinit(void)
{
    local_irq_disable();
    log_info("bt_ram:%x size:%x\n", &bt_ram_begin, &bt_ram_size);
    memset((u8 *)&bt_ram_begin, 0x00, (u32)&bt_ram_size);
    baseband_init();
    local_irq_enable();
    __this->bt_reinit = 1;
    bt_update_wait_start();
}

//baseband_resume:
static int baseband_resume(void)
{
    int ret = 0;
    /* local_irq_enable(); */
    ret = read_slot_clk_with_timeout(10);
    /* local_irq_disable(); */
    if (-1 == ret) {
        log_error("read slot clk fail\n");
        return -1;
    }

    if (__bt_updata_connection_resume(0) != 1) {
        printf("[BASEBAND] resume failed...\n");
        return -1;
    }
    lmp_update_init();

    return 0;
}

//恢复baseband
__attribute__((noinline))
bool bt_update_resume(void)
{
    if (0 == baseband_resume()) {
        return true;
    } else {
        return false;
    }
}

__attribute__((noinline))
void bt_updata_init_low_level()
{
    u8 bt_addr[6];
    u8 i;
    //stack init
    task_message_init();

    sys_timer_add(NULL, stack_run_loop_resume, 5);

    btctler_nv_memory_apply();

    link_bulk_init();
    init_soft_interrupt();                   //btstack loop软中断初始化
    reg_revic_buf_addr(bt_updata_handle);    //注册升级接收数据handle

    //设置本地蓝牙名称
    lmp_hci_write_local_name(CONFIG_BT_NAME);
    //生成随机地址
    for (i = 0; i < sizeof(bt_addr); i++) {
        bt_addr[i] = get_random();
    }
    log_info("bt_addr:\n");
    log_info_hexdump(bt_addr, sizeof(bt_addr));
    //配置蓝牙地址
    lmp_hci_write_local_address(bt_addr);
    memcpy(g_edr_addr, bt_addr, sizeof(bt_addr));

    //设置超时时间
    lmp_hci_write_super_timeout(4000);

    //rf_bredr_max_pwr_set(10);
    bt_max_pwr_set(6, 6, 6, 6);
}

void bt_update_timeout(void *priv)
{
    __this->bt_time_timeout ++;
}

void bredr_bd_close_frame(void);
void bt_update_state_cbk(u32 status, void *priv)
{
    switch (status) {
    case UPDATE_PARM:       //升级需要的参数
#ifdef CONFIG_BTCTRLE_V2_ENABLE
        update_set_trim_info(update_param_ext_get(priv, EXT_LDO_TRIM_RES));
#endif
        break;
    case UPDATE_START:      //测试盒bt升级需要根据priv判断是否恢复基带
        log_info("BT_UPDATE_START...\n");
        bt_updata_init_low_level();
        dynamic_mem_init(malloc_pool, malloc_pool_size);
        sys_timer_add(NULL, bt_update_timeout, 10);
        /* bredr_bd_close_frame(); */
        /* bt_update_reinit(); */
        if (*(u8 *)priv == UPDATE_JUMP) {     //resume baseband
            if (bt_update_resume() == false) {
                hci_controller_destory();
                bt_update_reinit();
            }
        } else {                        //reinit bt
            bt_update_reinit();
        }
        break;

    case UPDATE_END:		//升级结束需要保存结果到Ram给SDK获取，并回复主机升级结果
        log_info("UPDATE_END\n");
        if (*((u8 *)priv) == UPDATE_ERR_NONE) {
            set_updata_result(BT_UPDATA, UPDATA_SUCCESSFULLY);
        } else {
            set_updata_result(BT_UPDATA, UPDATA_DEV_ERR);
        }
        bt_update_result_report(*((u8 *)priv));
        /* cpu_reset(); */
        update_reset();
        break;
    }
}
