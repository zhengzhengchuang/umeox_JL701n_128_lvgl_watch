#include "../lv_watch.h"

static void shutdown_cb(void *priv)
{
    int task_post[1];
    task_post[0] = comm_msg_dev_shutdown;
    PostCommTaskMsg(task_post, 1);

    return;
}

static void restart_cb(void *priv)
{
    int task_post[1];
    task_post[0] = comm_msg_dev_restart;
    PostCommTaskMsg(task_post, 1);

    return;
}

static void reset_cb(void *priv)
{
    int task_post[1];
    task_post[0] = comm_msg_dev_reset;
    PostCommTaskMsg(task_post, 1);

    return;
}

static void DevAllSensorDisable(void)
{
    DisableGsModule();
    DisablePpgModule();
    DisableGmModule();

    return;
}

void DevOpMenuPopUp(void)
{
    if(!MenuSupportPopup()) return;

    ui_menu_jump(ui_act_id_device_op);

    return;
}

void DevOpResetHandle(void)
{
    /* 充电中... */ 
    // u8 charge_state = GetChargeState();
    // if(charge_state == 1) return;

    ResetAllVmData();

    int ui_msg_post[1];
    ui_msg_post[0] = ui_msg_nor_data_clear;
    post_ui_msg(ui_msg_post, 1);
  
    //DevAllSensorDisable();
    common_offscreen_handle(); 
    sys_timeout_add(NULL, reset_cb, 200);

    return;
}

void DevOpRestartHandle(void)
{
    /* 充电中... */ 
    // u8 charge_state = GetChargeState();
    // if(charge_state == 1) return;

    PowerOffVmDataWrite();
    //DevAllSensorDisable();
    common_offscreen_handle(); 
    sys_timeout_add(NULL, restart_cb, 200);

    return;
}

void DevOpShutdownHandle(void)
{  
    /* 充电中... */ 
    // u8 charge_state = GetChargeState();
    // if(charge_state == 1) return;

    PowerOffVmDataWrite();
    //DevAllSensorDisable();
    common_offscreen_handle(); 
    motor_run(1, sdw_motor_duty);
    sys_timeout_add(NULL, shutdown_cb, 300);

    return;
}
