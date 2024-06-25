#include "../lv_watch.h"

void FindDevEnableHandle(void)
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

    /*勿扰不影响查找设备*/

    motor_run(1, def_motor_duty);

    /*弹窗*/
    ui_menu_jump(ui_act_id_find_dev);

    return;
}

void FindDevDisableHandle(void)
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
    if(cur_act_id != ui_act_id_find_dev)
        return;

    /*返回上一级*/
    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    ui_menu_jump(prev_act_id);

    return;
}

void FindPhoneHandle(void)
{
#if !Vm_Debug_En
    int DevBondFlag = \
        GetVmParaCacheByLabel(\
            vm_label_dev_bond);
    if(!DevBondFlag)
        return;
#endif

    RemoteGetDevEvents(Le_Event_Find_Phone);

    return;
}