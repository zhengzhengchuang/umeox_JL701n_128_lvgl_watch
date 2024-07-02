#include "../lv_watch.h"

void common_rdec_msg_handle(int state)
{
    if(lcd_sleep_status())
        return;

    /*******编码器操作时，需重置熄屏定时器*******/
    common_offscreen_timer_restart();

    ui_act_id_t act_id = \
        p_ui_info_cache->cur_act_id;

    if(state == Rdec_Forward)
    {
        //printf("Rdec_Forward\n");

        if(act_id == ui_act_id_watchface)
        {
            int id = \
                GetVmParaCacheByLabel(vm_label_watchface_sel);
            id++;
            id = (id + ui_watchface_id_num)%ui_watchface_id_num;
            SetVmParaCacheByLabel(vm_label_watchface_sel, id);
            ui_menu_jump(ui_act_id_watchface);

            return;
        }
    }else if(state == Rdec_Backward)
    {
        //printf("Rdec_Backward\n");

        if(act_id == ui_act_id_watchface)
        {
            int id = \
                GetVmParaCacheByLabel(vm_label_watchface_sel);
            id--;
            id = (id + ui_watchface_id_num)%ui_watchface_id_num;
            SetVmParaCacheByLabel(vm_label_watchface_sel, id);
            ui_menu_jump(ui_act_id_watchface);

            return;
        }
    }

    /*******编码器无通用功能，回调到页面做相应地处理*******/
        lv_obj_t *obj = tileview_get_center_menu();
        ui_menu_rdec_key_func_cb rdec_func_cb = \
            p_ui_info_cache->menu_load_info.rdec_func_cb;
        if(rdec_func_cb)
            rdec_func_cb(obj, state);

    return;
}