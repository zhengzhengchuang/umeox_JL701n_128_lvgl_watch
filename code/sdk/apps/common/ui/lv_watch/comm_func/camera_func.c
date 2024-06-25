#include "../lv_watch.h"

static bool IsRemoteExit;
bool GetCameraIsRemoteExit(void)
{
    return IsRemoteExit;
}

void SetCameraIsRemoteExit(bool f)
{
    IsRemoteExit = f;

    return;
}

static bool UnlockExit;
bool GetCameraUnlockExit(void)
{
    return UnlockExit;
}

void SetCameraUnlockExit(bool f)
{
    UnlockExit = f;

    return;
}

void RemoteReqEnterCameraHandle(void)
{
#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    //判断当前是否符合弹出的条件
    if(!MenuSupportPopup())
        return;

    /*震动*/
    motor_run(1, def_motor_duty);

    ui_menu_jump(ui_act_id_camera);

    return;
}

void RemoteReqExitCameraHandle(void)
{  
#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif
 
    /*如果不在当前页面，不必跳转退出*/
    ui_act_id_t cur_act_id = \
        p_ui_info_cache->cur_act_id;
    if(cur_act_id != ui_act_id_camera)
        return;

    /*远程退出标志*/
    SetCameraIsRemoteExit(true);

    /*震动*/
    motor_run(1, def_motor_duty);

    /*返回上一级*/
    ui_act_id_t act_id = \
        read_menu_return_level_id();
    ui_menu_jump(act_id);

    return;
}

void DevReqOpCameraHandle(CameraOpCmd_t OpCmd)
{
    DevSendCameraOpCmd(OpCmd);

    return;
}