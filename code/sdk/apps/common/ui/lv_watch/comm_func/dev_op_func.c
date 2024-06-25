#include "../lv_watch.h"

static u16 timer_id;

static void shutdown_cb(void *priv)
{
    if(timer_id)
        sys_timeout_del(timer_id);
    timer_id = 0;

    int task_post[1];
    task_post[0] = comm_msg_dev_shutdown;
    post_comm_task_msg(task_post, 1);

    return;
}

static void restart_cb(void *priv)
{
    if(timer_id)
        sys_timeout_del(timer_id);
    timer_id = 0;

    int task_post[1];
    task_post[0] = comm_msg_dev_restart;
    post_comm_task_msg(task_post, 1);

    return;
}

static void reset_cb(void *priv)
{
    if(timer_id)
        sys_timeout_del(timer_id);
    timer_id = 0;

    int task_post[1];
    task_post[0] = comm_msg_dev_reset;
    post_comm_task_msg(task_post, 1);

    return;
}

static void DevAllSensorDisable(void)
{
    DisableSensorGsModule();
    DisableSensorGmModule();
    HrSensorStopSample();

    return;
}

void DevOpMenuPopUp(void)
{
    //当前菜单是否支持弹窗
    if(!MenuSupportPopup())
        return;

    ui_menu_jump(ui_act_id_device_op);

    return;
}

void DevOpResetHandle(void)
{
    ResetAllVmData();

    int ui_msg_post[1];
    ui_msg_post[0] = ui_msg_nor_vm_clear;
    post_ui_msg(ui_msg_post, 1);
  
    DevAllSensorDisable();

    common_offscreen_handle(); 

    /*程序重新启动马达会震动*/

    timer_id = sys_timeout_add(NULL, reset_cb, 500);

    return;
}

void DevOpRestartHandle(void)
{
    PowerOffVmDataWrite();

    DevAllSensorDisable();

    common_offscreen_handle(); 

    /*程序重新启动马达会震动*/

    timer_id = sys_timeout_add(NULL, restart_cb, 500);

    return;
}

void DevOpShutdownHandle(void)
{
    PowerOffVmDataWrite();

    DevAllSensorDisable();

    common_offscreen_handle(); 

    motor_run(1, sdw_motor_duty);

    timer_id = sys_timeout_add(NULL, shutdown_cb, 500);

    return;
}



