#include "smartbox/smartbox_task.h"
#include "smartbox/smartbox.h"
#include "smartbox_extra_flash_opt.h"
#include "file_transfer.h"
#include "file_delete.h"
#include "dev_format.h"
#include "clock_cfg.h"
#include "app_task.h"
#include "btstack/avctp_user.h"
#include "file_bluk_trans_prepare.h"
#include "sport_data_func.h"
#include "app_power_manage.h"
#include "bt/bt.h"

#include "le_smartbox_module.h"

#if (SMART_BOX_EN)
//这个模式主要是提供一个空模式， 处理一些需要占用时间不较长的交互处理， 处理做完之后退回到原来的模式
struct __action_event {
    u8	type;		//1:手机端触发, 0:固件触发
    u8 	OpCode_SN;
    u8	action;
};
static struct __action_event action_prepare = {0};
static u8 file_transfer_idle = 1;
static u8 temp_a2dp_en_flag = 0;
static u8 file_bluk_trans_flag = 0;
static u8 g_disable_opt_before_start = 1;
static u16 task_switch_flag;

u8 bt_get_a2dp_en_status();
void bt_set_a2dp_en_status(u8 on);
void lmp_esco_rejust_establish(u8 value);
void set_smartbox_watch_upgrade_flag(u8 flag);
bool is_testbox_ble_update_type(void);
/* void bredr_hfp_open_and_close(); */

static void app_smartbox_task_get_ready(void)
{
    printf("%s\n", __FUNCTION__);
    if (!is_testbox_ble_update_type()) {
        notify_update_connect_parameter(3);
    }
    if (action_prepare.type) {
        JL_CMD_response_send(JL_OPCODE_ACTION_PREPARE, JL_PRO_STATUS_SUCCESS, action_prepare.OpCode_SN, NULL, 0);
    }
}

static int app_smartbox_action_mode(void)
{
    if (smartbox_eflash_update_flag_get() || file_bluk_trans_flag) {
        return -1;
    }
    return 0;
}

static void app_smartbox_action_end_callback(void)
{
    app_smartbox_task_switch_stop();
    if (app_smartbox_action_mode()) {
        return;
    }
    if (app_get_curr_task() == APP_SMARTBOX_ACTION_TASK) {
        printf("action end callback!!\n");
        app_task_switch_back();
    }
}

static void app_smartbox_bluk_trans_end_callback(void)
{
    file_bluk_trans_flag = 0;
    app_smartbox_action_end_callback();
}

static void app_smartbox_task_start(void)
{
    lmp_esco_rejust_establish(1);
    set_smartbox_watch_upgrade_flag(1);
    temp_a2dp_en_flag = bt_get_a2dp_en_status();
    bt_set_a2dp_en_status(0);
    if (g_disable_opt_before_start) {
        clock_add_set(SMARTBOX_ACTION_CLK);
    }
    user_send_cmd_prepare(USER_CTRL_ALL_SNIFF_EXIT, 0, NULL);
    /* bredr_hfp_open_and_close(); */
    /* if (get_curr_channel_state() & HFP_CH) { */
    /*     user_send_cmd_prepare(USER_CTRL_HFP_DISCONNECT, 0, NULL); */
    /* } */
    file_transfer_idle = 0;
    app_smartbox_task_get_ready();
    //根据不同的场景， 做不同的处理， 例如：初始化不同的UI显示
    switch (action_prepare.action)		{
    case SMARTBOX_TASK_ACTION_FILE_TRANSFER:
        if (!app_smartbox_action_mode()) {
            UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_UPGRADE, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_UPGRADE);
            UI_MSG_POST("upgrade:wait=%4", 4);
        }
        file_transfer_init(app_smartbox_action_end_callback);
        break;
    case SMARTBOX_TASK_ACTION_FILE_DELETE:
        if (!app_smartbox_action_mode()) {
            UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_UPGRADE, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_UPGRADE);
            UI_MSG_POST("upgrade:wait=%4", 4);
        }
        file_delete_init(app_smartbox_action_end_callback);
        break;
    case SMARTBOX_TASK_ACTION_DEV_FORMAT:
        if (!app_smartbox_action_mode()) {
            UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_UPGRADE, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_UPGRADE);
            UI_MSG_POST("upgrade:wait=%4", 4);
        }
        dev_format_init(app_smartbox_action_end_callback);
        break;
    case SMARTBOX_TASK_ACTION_WATCH_TRANSFER:
        smartbox_extra_flash_init();
        break;
    case SMARTBOX_TASK_ACTION_BLUK_TRANSFER:
        file_bluk_trans_flag = 1;
        UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_UPGRADE, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_UPGRADE);
        UI_MSG_POST("upgrade:wait=%4", 4);
        file_bluk_trans_init(app_smartbox_bluk_trans_end_callback);
        break;
    case SMARTBOX_TASK_ACTION_RESET_EX_FLASH:
#if RCSP_UPDATE_EN
        smartbox_eflash_update_flag_set(1);
        smartbox_eflash_flag_set(1);
        extern void set_update_ex_flash_flag(u8 update_flag);
        set_update_ex_flash_flag(1);
        UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_UPGRADE, NULL, NULL, UI_WINDOW_PREEMPTION_TYPE_UPGRADE);
        UI_MSG_POST("upgrade:wait=%4", 4);
#else
        app_smartbox_action_end_callback();
#endif
        break;
    default:
        break;
    }
}

void app_smartbox_prepare_update_ex_flash(void)
{
    app_smartbox_task_prepare(0, SMARTBOX_TASK_ACTION_RESET_EX_FLASH, 0);
}

static void app_smartbox_task_stop(void)
{
    extern int f_flush_wbuf(const char *path);
    if (!is_testbox_ble_update_type()) {
        notify_update_connect_parameter(-1);
    }
    switch (action_prepare.action)		{
    case SMARTBOX_TASK_ACTION_FILE_TRANSFER:
        f_flush_wbuf("storage/sd1/C/");
        break;
    case SMARTBOX_TASK_ACTION_FILE_DELETE:
        break;
    case SMARTBOX_TASK_ACTION_DEV_FORMAT:
        break;
    case SMARTBOX_TASK_ACTION_WATCH_TRANSFER:
        smartbox_extra_flash_close();
        f_flush_wbuf("storage/virfat_flash/C/");
        break;
    default:
        break;
    }

    if (g_disable_opt_before_start) {
        clock_remove_set(SMARTBOX_ACTION_CLK);
    } else {
        g_disable_opt_before_start = 1;
    }
    file_transfer_idle = 1;
    /* bredr_hfp_open_and_close(); */
    /* if (!(get_curr_channel_state() & HFP_CH)) { */
    /*     user_send_cmd_prepare(USER_CTRL_HFP_CMD_BEGIN, 0, NULL); */
    /* } */
    lmp_esco_rejust_establish(0);
    extern u8 get_call_status();
    //传输结束后如果仍在通话状态，则不清变量标志位，等到挂断后再清
    if (get_call_status() == BT_CALL_HANGUP) {
        set_smartbox_watch_upgrade_flag(0);
    }
    bt_set_a2dp_en_status(temp_a2dp_en_flag);
    printf("app_smartbox_task_stop\n");
    if (0 == get_bt_init_status() && vbat_is_low_power()) {
        // 没有初始化过蓝牙且触发低电才执行
        bt_task_close();
        bt_ble_exit();
        btstack_exit();
    }
    UI_WINDOW_PREEMPTION_POP(ID_WINDOW_UPGRADE);
}

void file_trans_idle_set(u8 file_trans_idle_flag)
{
    file_transfer_idle = file_trans_idle_flag;
}

u8 file_trans_idle_query(void)
{
    return file_transfer_idle;
}

REGISTER_LP_TARGET(smartbox_lp_target) = {
    .name = "smartbox",
    .is_idle = file_trans_idle_query,
};

static int app_smartbox_task_event_handle(struct sys_event *event)
{
    switch (action_prepare.action)		{
    case SMARTBOX_TASK_ACTION_FILE_TRANSFER:
        break;
    case SMARTBOX_TASK_ACTION_FILE_DELETE:
        break;
    case SMARTBOX_TASK_ACTION_DEV_FORMAT:
        break;
    case SMARTBOX_TASK_ACTION_WATCH_TRANSFER:
        return smartbox_extra_flash_event_deal(event);
    case SMARTBOX_TASK_ACTION_BLUK_TRANSFER:
        break;
    default:
        break;
    }
    return 0;
}

static void app_smartbox_task_switch(void *priv)
{
    u16 flag = (u16)priv;
    if (flag != task_switch_flag) {
        printf("\n\n %s, %d \n\n", __func__, __LINE__);
        printf("flag:%d, %d \n", flag, task_switch_flag);
        return ;
    }
    task_switch_flag ++;
    int ret = app_task_switch_to(APP_SMARTBOX_ACTION_TASK);
    if (ret == false) {
        /* printf("\n\n %s, %d \n\n", __func__, __LINE__); */
        sys_timeout_add((void *)task_switch_flag, app_smartbox_task_switch, 500);
    }
}

void app_smartbox_task_switch_stop(void)
{
    task_switch_flag ++;
}

void app_smartbox_task_prepare(u8 type, u8 action, u8 OpCode_SN)
{
    action_prepare.type = type;
    action_prepare.action = action;
    action_prepare.OpCode_SN = OpCode_SN;

    task_switch_flag ++;

    //切换模式
    if (app_get_curr_task() != APP_SMARTBOX_ACTION_TASK) {
        int ret = app_task_switch_to(APP_SMARTBOX_ACTION_TASK);
        if (ret == false) {
            /* printf("\n\n %s, %d \n\n", __func__, __LINE__); */
            sys_timeout_add((void *)task_switch_flag, app_smartbox_task_switch, 500);
        }
    } else if (app_smartbox_action_mode()) {
        app_smartbox_task_start();
    } else {
        app_smartbox_task_get_ready();
    }
}

void app_smartbox_task_disable_opt(void)
{
    g_disable_opt_before_start = 0;
}

void app_smartbox_task(void)
{
    int msg[32];
    app_smartbox_task_start();
    while (1) {
        app_task_get_msg(msg, ARRAY_SIZE(msg), 1);

        switch (msg[0]) {
        case APP_MSG_SYS_EVENT:
            if (app_smartbox_task_event_handle((struct sys_event *)(msg + 1)) == false) {
                app_default_event_deal((struct sys_event *)(&msg[1]));
            }
            break;
        default:
            break;
        }

        if (app_task_exitting()) {
            app_smartbox_task_stop();
            return;
        }
    }
}

#else
void file_trans_idle_set(u8 file_trans_idle_flag)
{

}
#endif//SMART_BOX_EN

