#ifndef __UI_ACT_ID_H__
#define __UI_ACT_ID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lvgl/lvgl.h"

/*********************************************************************************
                                    屏幕的宽高                             
*********************************************************************************/
#define LCD_WIDTH  (368)
#define LCD_HEIGHT (448)

/*********************************************************************************
                                    屏幕UI显示Y偏移                             
*********************************************************************************/
#define LCD_UI_Y_OFFSET (20)

enum
{
    ui_watchface_id_00,
    ui_watchface_id_01,
    ui_watchface_id_02,
    ui_watchface_id_03,
    ui_watchface_id_04,
    ui_watchface_id_05,
    ui_watchface_id_06,
    ui_watchface_id_07,
    ui_watchface_id_08,
    ui_watchface_id_09,
    ui_watchface_id_10,
    ui_watchface_id_11,
  
    ui_watchface_id_num,
};
typedef uint8_t ui_watchface_id_t;

#define list_for_ui_watchface_load(id) \
    for(id = ui_watchface_id_00; id < ui_watchface_id_num; id++)

enum
{
    ui_menu_view_00 = 0x00,
    ui_menu_view_01,
    ui_menu_view_02,

    ui_menu_view_num,
};
typedef uint8_t ui_menu_view_t;

#define list_for_ui_menu_style(id) \
    for(id = ui_menu_view_00; id < ui_menu_view_num; id++)


enum
{
    al_name_list_mode0 = 0x00,
    al_name_list_mode1,

    al_name_list_mode_num,
};
typedef uint8_t al_name_list_mode_t;

#define list_for_ui_al_name_list(id) \
    for(id = al_name_list_mode0; id < al_name_list_mode_num; id++)

enum
{
    ui_act_id_null = 0x0000,

    /*****表盘相关页面*****/
    ui_act_id_watchface,

    /*****菜单相关页面*****/
    ui_act_id_menu,

    /*****绑定相关页面*****/
    ui_act_id_dev_bond,
    ui_act_id_new_bond,

    /*****设置相关页面*****/
    ui_act_id_about,
    ui_act_id_reset, 
    ui_act_id_set_main,
    ui_act_id_lang_sel,
    ui_act_id_backlight,
    ui_act_id_menu_view,
    ui_act_id_sound_ctrl,
    ui_act_id_unit_switch,
    ui_act_id_screen_sleep,
    
    /*****通话相关页面*****/
    ui_act_id_call_main,
    ui_act_id_call_disc,
    ui_act_id_contacts,
    ui_act_id_call_log,
    ui_act_id_call_dial,
    ui_act_id_call_in,
    ui_act_id_call_out,
    ui_act_id_call_online,
    ui_act_id_call_volume,
    ui_act_id_call_end,

    /*****提醒相关页面*****/
    ui_act_id_find_dev,
    ui_act_id_off_wrist,
    ui_act_id_sedentary,
    ui_act_id_device_op,
    ui_act_id_ota_update,
    ui_act_id_low_battery,
    ui_act_id_watchface_transp,
    
    /*****工具盒子相关页面*****/
    ui_act_id_tool_box,
    
    /*****礼拜时间相关页面*****/
    ui_act_id_worship_time,
    
    /*****天气相关页面*****/
    ui_act_id_weather_data,
    ui_act_id_weather_report,

    /*****远程音乐页面*****/
    ui_act_id_remote_music,

    /*****相机页面*****/
    ui_act_id_camera,

    /*****信息相关页面*****/
    ui_act_id_msg_list,
    ui_act_id_msg_detail,

    /*****闹钟相关页面*****/
    ui_act_id_alarm_main,
    ui_act_id_alarm_time,
    ui_act_id_alarm_repeat,
    ui_act_id_alarm_modify,
    ui_act_id_alarm_remind,

    /*****倒计时相关页面*****/
    ui_act_id_countdown_time,
    ui_act_id_countdown_state,
    ui_act_id_countdown_over,

    /*****秒表相关页面*****/
    ui_act_id_stopwatch_main,
    ui_act_id_stopwatch_state,
    ui_act_id_stopwatch_over,

    /*****99真主名相关页面*****/
    ui_act_id_al_name_list,
    ui_act_id_al_name_ex,

    /*****心率相关页面*****/
    ui_act_id_hr_sample,
    
    /*****血氧相关页面*****/
    ui_act_id_bo_sample,

    /*****克尔白朝向相关页面*****/
    ui_act_id_gm_cali,
    ui_act_id_kaaba_position,
    ui_act_id_kaaba_qibla,

    /*****祈祷时间相关页面*****/
    ui_act_id_prayer_time_main,
    ui_act_id_prayer_time_popup,
    ui_act_id_prayer_time_remind,
    ui_act_id_prayer_time_advance,
    ui_act_id_prayer_time_preview,

    /*****祈祷祠相关页面*****/
    ui_act_id_azkar_list,
    ui_act_id_azkar_sec,

    /*****赞念提醒相关页面*****/
    ui_act_id_tasbih_main,
    ui_act_id_tasbih_start_time,
    ui_act_id_tasbih_end_time,
    ui_act_id_tasbih_inv_time,
    ui_act_id_tasbih_days,
    ui_act_id_tasbih_remind,

    /*****回历相关页面*****/
    ui_act_id_Gcalendar_main,
    ui_act_id_Hcalendar_main,
    ui_act_id_Hcalendar_modify,
    ui_act_id_Hcalendar_remind,
    ui_act_id_Hcalendar_festivals,

    /*****睡眠相关页面*****/
    ui_act_id_sleep_main,
    ui_act_id_sleep_detail,

    /*****更多菜单相关页面*****/
    ui_act_id_more_menu,

    /*****古兰经播放相关页面*****/
    ui_act_id_fav_list,
    ui_act_id_audio_list,
    ui_act_id_quran_list,
    ui_act_id_quran_main,
    ui_act_id_quran_volume,

    /*****tws相关页面*****/
    ui_act_id_tws_main,
    ui_act_id_tws_disc,
    ui_act_id_tws_list,
    ui_act_id_tws_unpair,
    ui_act_id_tws_search,
    ui_act_id_tws_not_found,

    /*****工厂测试相关页面*****/
    ui_act_id_factory,

    ui_act_id_max,
};
typedef u16 ui_act_id_t;


#ifdef __cplusplus
}
#endif

#endif
