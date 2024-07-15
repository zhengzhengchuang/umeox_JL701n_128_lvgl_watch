#include "app_task.h"
#include "../lv_watch.h"
#include "../../../../watch/include/app_main.h"
#include "../../../../../include_lib/system/event.h"
#include "../../../../../include_lib/btstack/le/le_user.h"
#include "../../../../watch/include/ui/lcd_spi/lcd_drive.h"

static bool menu_timer_lock_flag = false;

bool get_menu_timer_lock_flag(void)
{
    return menu_timer_lock_flag;
}

void set_menu_timer_lock_flag(bool flag)
{
    menu_timer_lock_flag = flag;

    return;
}

void common_key_msg_handle(int key_value, int key_event)
{
    bool power_on = \
        GetPowerOnStatus();
    if(power_on == true)
        return;
    
    printf("key_value = %d, key_event = %d\n", \
        key_value, key_event);

    if(lcd_sleep_status())
    {
        if(key_value >= Common_Key_Val_Max)
            return;

        if(key_event == KEY_EVENT_CLICK)
        {
            /************按键亮屏显示页************/
            ui_act_id_t act_id = ui_act_id_watchface;
            ui_menu_load_info_t *menu_load_info = \
                &p_ui_info_cache->exit_menu_load_info;
            bool timer_lock_flag = \
                get_menu_timer_lock_flag();
            if(menu_load_info->lock_flag)
                act_id = menu_load_info->menu_id;
            else
            {
                if(timer_lock_flag)
                    act_id = menu_load_info->menu_id;
            }
            
            ui_menu_jump(act_id);
        }
    }else
    {
        /*******亮屏按键操作时，需重置刷新定时器*******/
        //common_refresh_timer_re_run();

        /*******亮屏按键操作时，需重置熄屏定时器*******/
        common_offscreen_timer_restart();

        bool menu_lock = \
            p_ui_info_cache->menu_load_info.lock_flag;
        ui_act_id_t cur_act_id = \
            p_ui_info_cache->cur_act_id;

        if(key_value == Common_Key_Val_1)//电源键
        {
            if(key_event == KEY_EVENT_CLICK)
            {
                common_offscreen_handle();
            }else if(key_event == KEY_EVENT_LONG_3S)
            {
                int DevBondFlag = \
                    GetVmParaCacheByLabel(vm_label_dev_bond);
                if(!DevBondFlag && cur_act_id == ui_act_id_dev_bond)
                    ui_menu_jump(ui_act_id_device_op);
                else
                    DevOpMenuPopUp();
            }
        }else if(key_value == Common_Key_Val_0)//Home键
        {
            if(key_event == KEY_EVENT_CLICK)
            {
                if(cur_act_id == ui_act_id_watchface)
                {
                    /*在表盘页面下，菜单键点击进入菜单列表*/
                    ui_menu_jump(ui_act_id_menu);
                    return;
                }

                if(menu_lock == false)
                {
                    /*如果当前页面没有锁定，直接home到表盘*/
                    ui_menu_jump(ui_act_id_watchface);
                    return;
                }
            }else if(key_event == KEY_EVENT_DOUBLE_CLICK)
            {
#if TCFG_APP_PC_EN
                if (pc_app_check() == false) {
                    printf("pc dev check error!!!!!!!\n");
                } else {
                    app_task_switch_to(APP_PC_TASK);
                }
#else
                if(cur_act_id == ui_act_id_menu)
                {
                    /*在菜单页面，双击更换风格*/
                    int cur_menu = \
                        GetVmParaCacheByLabel(vm_label_menu_view);
                    cur_menu++;
                    cur_menu %= ui_menu_view_num;
                    SetVmParaCacheByLabel(vm_label_menu_view, cur_menu);
                    ui_menu_jump(ui_act_id_menu);
                    return;
                }
#if 0               
                else if(cur_act_id == ui_act_id_about)
                {
                    //测试用
                    ui_menu_jump(ui_act_id_factory);
                }
#endif
#endif
            }else if(key_event == KEY_EVENT_FIRTH_CLICK)
            {
                int DevBondFlag = \
                    GetVmParaCacheByLabel(vm_label_dev_bond);
                if(!DevBondFlag && cur_act_id == ui_act_id_dev_bond)
                    ui_menu_jump(ui_act_id_watchface);
            }
        }else
        {
            return;
        }

        /*******按键无通用功能，回调到页面做相应地处理*******/
        lv_obj_t *obj = tileview_get_center_menu();
        ui_menu_key_func_cb key_func_cb = \
            p_ui_info_cache->menu_load_info.key_func_cb;
        if(key_func_cb)
            key_func_cb(obj, key_value, key_event);
    }

    return;
}

