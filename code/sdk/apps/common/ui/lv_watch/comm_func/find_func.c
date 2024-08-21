#include "../lv_watch.h"

void FindDevEnableHandle(void)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) return;

    if(!MenuSupportPopup()) return;

    motor_run(1, def_motor_duty);

    ui_menu_jump(ui_act_id_find_dev);

    return;
}

void FindDevDisableHandle(void)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) return;

    /*如果不在当前页面，不必跳转退出*/
    ui_act_id_t act_id = p_ui_info_cache->cur_act_id;
    if(act_id != ui_act_id_find_dev)
        return;

    /*返回上一级*/
    ui_act_id_t prev_act_id = \
        read_menu_return_level_id();
    ui_menu_jump(prev_act_id);

    return;
}

void FindPhoneHandle(void)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) return;

    RemoteGetDevEvents(Le_Event_Find_Phone);

    return;
}
