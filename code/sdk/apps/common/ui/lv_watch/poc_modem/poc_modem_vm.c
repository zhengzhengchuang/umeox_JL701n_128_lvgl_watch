#include "syscfg_id.h"
#include "app_config.h"
#include "../lv_watch.h"
#include "poc_modem_vm.h"
#include "../comm_nor_vm/nor_vm_main.h"

vm_para_info_t *p_vm_para_cache = NULL;
static vm_para_info_t vm_para_cache = {0};
const ui_menu_load_info_t *watchface_load_info[] = {
    &menu_load_watchface_00, &menu_load_watchface_01, \
    &menu_load_watchface_02, &menu_load_watchface_03, \
    &menu_load_watchface_04, &menu_load_watchface_05, \
    &menu_load_watchface_06, &menu_load_watchface_07, \
    &menu_load_watchface_08, &menu_load_watchface_09, \
    &menu_load_watchface_10, &menu_load_watchface_11,
};

const ui_menu_load_info_t *menu_style_load_info[] = {
    &menu_load_style_00, &menu_load_style_01, &menu_load_style_02,
};

const ui_menu_load_info_t *al_name_load_info[] = {
    &menu_load_al_name_list0, &menu_load_al_name_list1,
};

/*********************************************************************************
                             系统带标签参数的缺省值                                  
*********************************************************************************/
static const vm_ctx_t vm_def[Vm_Num] = {
    /*********屏幕背光亮度*********/
    {.label = vm_label_backlight, \
        .val = TCFG_BACKLIGHT_MIN_VAL + TCFG_BACKLIGHT_STEPS_VAL*3}, 

    /*********东八区 时区*10*********/
    {.label = vm_label_time_zone, .val = 80}, 

    /*********菜单风格*********/
    {.label = vm_label_menu_view, .val = ui_menu_view_00},

    /*********时间制:24小时制*********/
    {.label = vm_label_time_format, .val = time_format_24},

    /*********屏幕熄屏*********/
    {.label = vm_label_offscreen_time, .val = 30*1000},

    /*********表盘*********/
    {.label = vm_label_watchface_sel, .val = ui_watchface_id_00},

    /*********系统语言*********/
    {.label = vm_label_sys_language, .val = lang_id_chinese},

    /*********系统声音开启*********/
    {.label = vm_label_sys_sound, .val = 1},

    /*******勿扰状态*******/
    {.label = vm_label_dnd_state, .val = dnd_state_disable},

    /*******久坐步数*******/
    {.label = vm_label_sedentary_steps, .val = 0},

    /*******耳机状态*******/
    {.label = vm_label_earphone_state, .val = earphone_state_disc},

    /*******计量单位*******/
    {.label = vm_label_unit_distance, .val = unit_distance_kilometre},
    {.label = vm_label_unit_temperature, .val = unit_temperature_C},

    /*******99真主名列表模式*******/
    {.label = vm_label_al_name_list_mode, .val = al_name_list_mode0},

    /*******设备绑定*******/
    {.label = vm_label_dev_bond, .val = 0},
    
    /*******心率值*******/
    {.label = vm_label_hr_real_val, .val = 0},

    /*********心率过高值*********/
    {.label = vm_label_hr_low_remind_sw, .val = 1},
    {.label = vm_label_hr_high_remind_sw, .val = 1},
    {.label = vm_label_hr_low_val, .val = 40},
    {.label = vm_label_hr_high_val, .val = 150},

    /*********血氧值*********/
    {.label = vm_label_bo_real_val, .val = 0},

    /*********活动数据*********/
    {.label = vm_label_daily_step, .val = 0},
    {.label = vm_label_daily_calorie, .val = 0},
    {.label = vm_label_daily_distance, .val = 0},

    /*******特定开关*******/
    {.label = vm_label_auto_hr_sw, .val = 0},
    {.label = vm_label_auto_bo_sw, .val = 0},
    {.label = vm_label_lpw_remind_sw, .val = 1},
};

int GetVmParaCacheByLabel(uint16_t label)
{
    for(uint16_t i = 0; i < Vm_Num; i++)
    {
        if(p_vm_para_cache->vm_para[i].label == label)
            return (p_vm_para_cache->vm_para[i].val);
    }
    
    return vm_def[label].val;
}

bool SetVmParaCacheByLabel(uint16_t label, int vm_val)
{
    for(uint16_t i = 0; i < Vm_Num; i++)
    {
        if(p_vm_para_cache->vm_para[i].label == label)
        {
            p_vm_para_cache->vm_para[i].val = \
                vm_val;
            vm_para_cache_write();
            return true;
        }     
    }

    return false;
}

void vm_store_para_init(void)
{
    p_vm_para_cache = &vm_para_cache;

    if(!p_vm_para_cache) ASSERT(0);

    int op_vm_len = \
        sizeof(vm_para_info_t);

    int ret = syscfg_read(CFG_SYS_PARA_INFO, \
        (uint8_t *)&vm_para_cache, op_vm_len);
#if Vm_Debug_En
    if(1)
#else
    if(ret != op_vm_len || p_vm_para_cache->vm_mask != Vm_Mask)
#endif
    {
        vm_para_cache_reset();
    }

    LLInfoParaRead();
    KaabaQiblaParaUpdate();
    PTimeCfgParaRead();
    TasbihRInfoParaRead();
    HcalendarInfoParaRead();
    AlarmInfoParaRead();
    DndInfoParaRead();
    UserInfoParaRead();
    GalgoInfoParaRead();
    BondCodeInfoParaRead();
    MsgNotifyInfoParaRead();
    SedInfoParaRead();
    RmusicInfoParaRead();
    QpUserInfoParaRead();
    
    return;
}

void vm_para_cache_write(void)
{
    int vm_op_len = \
        sizeof(vm_para_info_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_SYS_PARA_INFO, \
            &vm_para_cache, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void vm_para_cache_reset(void)
{
    memcpy(p_vm_para_cache->vm_para, vm_def, \
        sizeof(vm_ctx_t)*Vm_Num);

    p_vm_para_cache->vm_mask = Vm_Mask;

    vm_para_cache_write();

    return;
}
