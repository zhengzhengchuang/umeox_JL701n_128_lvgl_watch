#ifndef __LANG_TXTID_H__
#define __LANG_TXTID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

/*********************************************************************************
                                  文本id枚举                                       
*********************************************************************************/
enum
{
    lang_txtid_phone,  //电话
    lang_txtid_notify, //消息通知
    lang_txtid_kaaba_qibla, //克尔白朝向
    lang_txtid_quran_player, //古兰经播放器
    lang_txtid_prayer_times, //礼拜时间
    lang_txtid_azkar, //祈祷词
    lang_txtid_tasbih_reminder, //赞念提醒
    lang_txtid_allah_99_name, //阿拉的 99 个尊名
    lang_txtid_hijri_calendar, //回历
    lang_txtid_sleep, //睡眠
    lang_txtid_weather, //天气
    lang_txtid_heart_rate, //心率
    lang_txtid_spo2, //血氧
    lang_txtid_alarm, //闹钟
    lang_txtid_more, //更多
    lang_txtid_settings, //设置
    lang_txtid_pedometer, //计步
    lang_txtid_menu_view, //菜单视图
    lang_txtid_sound, //声音
    lang_txtid_display, //展示
    lang_txtid_language, //语言
    lang_txtid_unit, //单位
    lang_txtid_shortcuts, //快捷方式
    lang_txtid_about, //关于
    lang_txtid_sound_on, //声音
    lang_txtid_view_list, //列表
    lang_txtid_view_grid_1, //网格1
    lang_txtid_view_grid_2, //网格2
    lang_txtid_screen_sleep, //屏幕休眠
    lang_txtid_distance, //距离
    lang_txtid_temperature, //温度
    lang_txtid_kilometer, //公里
    lang_txtid_mile, //英里
    lang_txtid_unit_C, //℃
    lang_txtid_unit_F, //℉
    lang_txtid_quran_watch, //QWatch
    lang_txtid_bluetooth_name, //蓝牙名称
    lang_txtid_bluetooth_mac,//蓝牙 MAC
    lang_txtid_version,//版本
    lang_txtid_QR_code,//二维码
    lang_txtid_reset,//恢复出厂设置
    lang_txtid_reset_tips,//恢复出厂设置将清除手表数据。
    lang_txtid_today,//今天
    lang_txtid_tomorrow,//明天
    lang_txtid_sunny,//晴
    lang_txtid_snow,//雪
    lang_txtid_thunderstorm,//雷阵雨
    lang_txtid_wind,//风
    lang_txtid_misthaze,//雾霾
    lang_txtid_sandstorm,//沙尘暴
    lang_txtid_cloudy,//多云
    lang_txtid_rain_snow,//雨夹雪
    lang_txtid_shower,//阵雨
    lang_txtid_overcast,//阴天
    lang_txtid_pouring,//大雨
    lang_txtid_light_rain,//小雨
    lang_txtid_no_data,//无数据
    lang_txtid_every_day,//每天
    lang_txtid_sunday,//周日
    lang_txtid_monday,//周一
    lang_txtid_tuesday,//周二
    lang_txtid_wednesday,//周三
    lang_txtid_thursday,//周四
    lang_txtid_friday,//周五
    lang_txtid_saturday,//周六
    lang_txtid_call_log,//通话记录
    lang_txtid_contacts,//联系人
    lang_txtid_call_disc_tips,//电话未连接，请在连接电话后重试。
    lang_txtid_no_record,//无记录
    lang_txtid_call_end,//通话结束
    lang_txtid_connect_and_open,//请连接手机，并打开播放器。
    lang_txtid_not_connected,//未连接到手机
    lang_txtid_search_phone,//正在寻找附近的手机...
    lang_txtid_dnd_mode,//勿扰模式
    lang_txtid_no_reminder,//无提醒
    lang_txtid_no_message,//无消息
    lang_txtid_gm_cali,//按照磁性校准说明进行操作。
    lang_txtid_real_pos,//蓝牙连接到手机，实现实时定位。
    lang_txtid_Fajr,//晨礼
    lang_txtid_Sunrise,//日出
    lang_txtid_Dhuhr,//晌礼
    lang_txtid_Asr,//晡礼
    lang_txtid_Sunset,//日落
    lang_txtid_Maghrib,//昏礼
    lang_txtid_Isha,//霄礼
    lang_txtid_remaining_time,//剩余时间
    lang_txtid_elapsed_time,//经过时间
    lang_txtid_reminder_ad,//提前提醒
    lang_txtid_mins,//分钟
    lang_txtid_remind,//提醒
    lang_txtid_voice,//语音
    lang_txtid_MR,//晨间纪念
    lang_txtid_ER,//晚间纪念
    lang_txtid_BS,//睡觉前
    lang_txtid_APR,//祈祷后赞念
    lang_txtid_HAU,//朝觐与乌姆拉
    lang_txtid_FAS,//斋月
    lang_txtid_WWU,//起床
    lang_txtid_GAR,//服装
    lang_txtid_ABU,//沐浴
    lang_txtid_HOME,//家
    lang_txtid_MOS,//清真寺
    lang_txtid_ATH,//祈祷的呼唤
    lang_txtid_FOOD,//食物
    lang_txtid_TRA,//旅行
    lang_txtid_OR,//其他祷告
    lang_txtid_PRA,//礼拜
    lang_txtid_set_reminder_tasbih,//为赞念设置提醒。
    lang_txtid_time_range,//时间范围
    lang_txtid_inv_time,//间隔时间
    lang_txtid_days,//天
    lang_txtid_tasbih_time,//赞念时间
    lang_txtid_hours,//小时
    lang_txtid_exc_time_range,//间隔时间超过了时间范围
    lang_txtid_start_time,//开始时间
    lang_txtid_end_time,//结束时间
    lang_txtid_January,//一月
    lang_txtid_February,//二月
    lang_txtid_March,//三月
    lang_txtid_April,//四月
    lang_txtid_May,//五月
    lang_txtid_June,//六月
    lang_txtid_July,//七月
    lang_txtid_August,//八月
    lang_txtid_September,//九月
    lang_txtid_October,//十月
    lang_txtid_November,//十一月
    lang_txtid_December,//十二月
    lang_txtid_Muharram,//穆哈兰姆月
    lang_txtid_Safar,//色法尔月
    lang_txtid_Rabi_al_Awwal,//赖比尔·敖外鲁月
    lang_txtid_Rabi_al_Thani,//赖比尔·阿色尼月
    lang_txtid_Jumada_al_Awwal,//主马达·敖外鲁月
    lang_txtid_Jumada_al_Thani,//主马达·阿色尼月
    lang_txtid_Rajab,//赖哲卜月
    lang_txtid_Shaban,//舍尔邦月
    lang_txtid_Ramadan,//赖买丹月
    lang_txtid_Shawwal,//闪瓦鲁月
    lang_txtid_Dhu_al_Qadah,//都尔喀尔德月
    lang_txtid_Dhu_al_Hijjah,//都尔黑哲月
    lang_txtid_Hcalendar_modify,//回历修改
    lang_txtid_Hijri_New_Year,//伊斯兰新年
    lang_txtid_Ashuraa,//阿舒拉节
    lang_txtid_Prophets_birthday,//圣纪节
    lang_txtid_TN_of_Isra_Mi,//登霄夜
    lang_txtid_Mid_of_Shaban,//拜拉特夜
    lang_txtid_Fes_Ramadan,//斋月
    lang_txtid_Laylat_al_Qadr,//盖德尔夜
    lang_txtid_Eid_ul_Fitr,//开斋节
    lang_txtid_Tarwiyah_day,//塔维耶日
    lang_txtid_Arafa_day,//阿拉法日
    lang_txtid_Eid_ul_Adha,//古尔邦节
    lang_txtid_Days_of_Tashreeq,//塔什里格日
    lang_txtid_festival_reminder,//节日提醒
    lang_txtid_wake_up,//清醒
    lang_txtid_Rems,//快速眼动
    lang_txtid_light_sleep,//浅睡
    lang_txtid_deep_sleep,//深睡
    lang_txtid_wear_condition,//测量异常，请检查佩戴情况。
    lang_txtid_women_health,//女性健康
    lang_txtid_timer,//计时器
    lang_txtid_stopwatch,//秒表
    lang_txtid_call_volumn,//通话音量
    lang_txtid_media_volumn,//媒体音量
    lang_txtid_find_dev,//寻找到您的手表
    lang_txtid_move,//起身走动
    lang_txtid_dev_bond,//下载iQibla(Smart Qibla) APP，用APP扫描绑定Quran Watch。
    lang_txtid_new_bond,//当前您为新用户，设备数据已清除。
    lang_txtid_Fav_list,//收藏列表
    lang_txtid_Mor_remembrance,//早上祈祷词
    lang_txtid_Eve_remembrance,//晚上祈祷词
    lang_txtid_add_new_devices,//添加新的无线音频设备
    lang_txtid_paired_tws,//配对设备
    lang_txtid_unpaired_tws,//取消配对音频设备？
    lang_txtid_disc_tws,//断开音频设备？
    lang_txtid_not_found_tws,//未找到无线音频设备，请重试。
   
    lang_txtid_max,
};
typedef uint32_t lang_txtid_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
