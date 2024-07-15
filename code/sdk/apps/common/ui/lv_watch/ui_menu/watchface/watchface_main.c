#include "watchface_main.h"

void wf_tileview_register_all_menu(\
    lv_obj_t *obj)
{
#if 1
    ui_act_id_t up_act_id = ui_act_id_tool_box;
    ui_act_id_t down_act_id = ui_act_id_msg_list;
    ui_act_id_t left_act_id = ui_act_id_weather_data;
    ui_act_id_t right_act_id = ui_act_id_quran_main;
#else
    ui_act_id_t up_act_id = ui_act_id_null;
    ui_act_id_t down_act_id = ui_act_id_null;
    ui_act_id_t left_act_id = ui_act_id_null;
    ui_act_id_t right_act_id = ui_act_id_null;
#endif

    tileview_register_all_menu(obj, up_act_id, down_act_id, \
        left_act_id, right_act_id, ui_act_id_watchface);

    return;
}

