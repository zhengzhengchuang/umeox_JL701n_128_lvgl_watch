#include "../lv_watch.h"
#include "common_offscreen.h"
#include "../poc_modem/poc_modem_vm.h"

/**************熄屏定时器id**************/
static uint16_t offscreen_timer_id = 0;

/**************页面定时锁定定时器id**************/
static uint16_t menu_lock_timer_id = 0;

/**************当前熄屏时间**************/
static uint32_t cur_offscreen_time = 0;

/**************是否进入灭屏**************/
static bool is_enter_offscreen = false;

/**************函数声明**************/
static void common_menu_lock_timer_cb(void *priv);

bool GetIsEnterOffScreen(void)
{
    return is_enter_offscreen;
}

void SetIsEnterOffScreen(bool status)
{
    is_enter_offscreen = status;

    return;
}

void common_menu_lock_timer_del(void)
{
    set_menu_timer_lock_flag(false);

    if(menu_lock_timer_id)
        sys_timeout_del(menu_lock_timer_id);
    menu_lock_timer_id = 0;
        
    return;
}

void common_menu_lock_timer_add(void)
{
    /* 灭屏页面锁定 */
    set_menu_timer_lock_flag(true);

    if(!menu_lock_timer_id)
        menu_lock_timer_id = sys_timeout_add(NULL, \
            common_menu_lock_timer_cb, 1*60*1000);

    return;
}

void common_offscreen_handle(void)
{
    common_offscreen_timer_destroy();
    
    SetIsEnterOffScreen(true);

    int ui_msg_post[1];
    ui_msg_post[0] = ui_msg_menu_offscreen;
    post_ui_msg(ui_msg_post, 1);

    return;
}

static void common_offscreen_timer_cb(void *priv)
{
    if(cur_offscreen_time == Always_OnScreen)
        return;
        
    common_offscreen_handle();

    return;
}

static void common_menu_lock_timer_cb(void *priv)
{
    /*页面超时解锁*/
    common_menu_lock_timer_del();

    //地磁如果开启，关掉
    bool GmEnableFlag = \
        GetSensorGmEnableFlag();
    if(GmEnableFlag == true)
        DisableSensorGmModule();

    /*相机解锁退出*/
    bool CameraUnlock = \
        GetCameraUnlockExit();
    if(CameraUnlock == true)
        le_task_post(Le_Req_Exit_Camera, NULL, 0);
    SetCameraUnlockExit(false);

    return;
}

//extern int lcd_sleep_ctrl(u8 enter);
void common_offscreen_msg_handle(void)
{
    /**************清除当前页面**************/
    ui_menu_jump_handle(ui_act_id_null);//传ui_act_id_null即是销毁页面

    /**************灭屏菜单定时锁定机制**************/
    common_menu_lock_timer_add();

    /**************关掉TE, 下次亮屏能缩短时间**************/
    SetUsrWaitTe(0);

    /*关掉lcd外设*/
    AppCtrlLcdEnterSleep(true);

    return;
}

void common_offscreen_timer_create(void)
{
    uint32_t user_offscreen_time = \
        p_ui_info_cache->menu_load_info.user_offscreen_time;
    int sys_offscreen_time = \
        GetVmParaCacheByLabel(vm_label_offscreen_time);

    if(user_offscreen_time > 0)
        cur_offscreen_time = user_offscreen_time;
    else
        cur_offscreen_time = sys_offscreen_time;

    if(!offscreen_timer_id)
        offscreen_timer_id = sys_timeout_add(NULL, \
            common_offscreen_timer_cb, cur_offscreen_time);

    SetIsEnterOffScreen(false);

    return;
}

void common_offscreen_timer_restart(void)
{
    if(offscreen_timer_id)
        sys_timer_re_run(offscreen_timer_id);

    return;
}

void common_offscreen_timer_destroy(void)
{
    if(offscreen_timer_id)
        sys_timeout_del(offscreen_timer_id);

    offscreen_timer_id = 0;

    return;
}