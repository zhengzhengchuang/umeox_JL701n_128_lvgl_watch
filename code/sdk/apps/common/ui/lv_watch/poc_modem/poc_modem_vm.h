#ifndef __POC_MODEM_VM_H__
#define __POC_MODEM_VM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../include/ui_act_id.h"

/*********************************************************************************
                                  时间小时制枚举                                      
*********************************************************************************/
enum
{
    time_format_24 = 0x00,
    time_format_12,
};
typedef uint8_t comm_time_format_t;

/*********************************************************************************
                             距离计量单位                              
*********************************************************************************/
enum
{
    unit_distance_kilometre = 0x00,
    unit_distance_mile,
    
    unit_distance_num,
};

/*********************************************************************************
                             温度计量单位                              
*********************************************************************************/
enum
{
    unit_temperature_C = 0x00,
    unit_temperature_F,

    unit_temperature_num,
};

/*********************************************************************************
                             勿扰状态枚举                              
*********************************************************************************/
enum
{
    dnd_state_disable = 0x00,
    dnd_state_enable,
};

/*********************************************************************************
                             耳机连接状态枚举                              
*********************************************************************************/
enum
{
    earphone_state_disc = 0x00,
    earphone_state_conn,
};

/*********************************************************************************
                             vm存储信息，关机会备份到vm                              
*********************************************************************************/
enum
{
    vm_label_none = 0x00,

    /*******背光亮度*******/
    vm_label_backlight,

    /*******时区*******/
    vm_label_time_zone,

    /*******菜单风格*******/
    vm_label_menu_view,

    /*******小时制*******/
    vm_label_time_format,

    /*******表盘选择*******/
    vm_label_watchface_sel,

    /*******熄屏时间*******/
    vm_label_offscreen_time,

    /*******系统语言*******/
    vm_label_sys_language,

    /*******系统声音*******/
    vm_label_sys_sound,

    /*******勿扰状态*******/
    vm_label_dnd_state,

    /*******久坐步数*******/
    vm_label_sedentary_steps,

    /*******耳机状态*******/
    vm_label_earphone_state,

    /*******计量单位*******/
    vm_label_unit_distance,
    vm_label_unit_temperature,

    /*******99真主名列表模式*******/
    vm_label_al_name_list_mode,

    /*******设备绑定标志*******/
    vm_label_dev_bond,

    /*******心率值*******/
    vm_label_hr_real_val,
    
    /*******心率异常区间*******/
    vm_label_hr_low_remind_sw,  //心率过低开关
    vm_label_hr_high_remind_sw, //心率过高开关
    vm_label_hr_low_val,        //心率过低参数
    vm_label_hr_high_val,       //心率过高参数

    /*******血氧值*******/
    vm_label_bo_real_val,

    /*******活动数据*******/
    vm_label_daily_step,       //步
    vm_label_daily_calorie,    //千卡
    vm_label_daily_distance,   //米

    /*******特定开关*******/
    vm_label_auto_hr_sw,        //自动心率开关
    vm_label_auto_bo_sw,        //自动血氧开关
    vm_label_lpw_remind_sw,     //低电提醒开关

    vm_label_num,
};

#define Vm_Num \
    (vm_label_num)
#define Vm_Mask (0x55b5)//每次更新vm参数的时候都需要更改

typedef struct
{
    uint16_t label;
    int val;
}vm_ctx_t;

typedef struct
{
    uint16_t vm_mask;

    vm_ctx_t vm_para[Vm_Num];  
}vm_para_info_t;
extern vm_para_info_t *p_vm_para_cache;
extern const ui_menu_load_info_t *al_name_load_info[];
extern const ui_menu_load_info_t *watchface_load_info[];
extern const ui_menu_load_info_t *menu_style_load_info[];

void vm_store_para_init(void);
void vm_para_cache_write(void);
void vm_para_cache_reset(void);
int GetVmParaCacheByLabel(uint16_t label);
bool SetVmParaCacheByLabel(uint16_t label, int vm_val);
#ifdef __cplusplus
}
#endif

#endif
