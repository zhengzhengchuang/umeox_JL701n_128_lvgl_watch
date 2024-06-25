#ifndef UI_STYLE_H
#define UI_STYLE_H

#include "app_config.h"

// #include "ui/style_led7.h"//led7

#if(CONFIG_UI_STYLE == STYLE_JL_WTACH)
#include "ui/style_JL.h"
#include "ui/style_DIAL.h"
#include "ui/style_upgrade.h"
#include "ui/style_sidebar.h"
//#include "ui/style_WATCH.h"
//#include "ui/style_WATCH1.h"
//#include "ui/style_WATCH2.h"
//#include "ui/style_WATCH3.h"
//#include "ui/style_WATCH4.h"
//#include "ui/style_WATCH5.h"

#ifdef PAGE_0
#undef PAGE_0
#define PAGE_0 DIAL_PAGE_0
#endif

#define ID_WINDOW_BT                    PAGE_7
#define ID_WINDOW_CLOCK                 PAGE_0
#define ID_WINDOW_ACTIVERECORD          PAGE_1
#define ID_WINDOW_SLEEP                 PAGE_2
#define ID_WINDOW_MAIN                  PAGE_3
#define ID_WINDOW_POWER_ON              PAGE_4
#define ID_WINDOW_POWER_OFF             PAGE_5
#define ID_WINDOW_VMENU                 PAGE_7
#define ID_WINDOW_PHONE                 PAGE_6
#define ID_WINDOW_MUSIC                 PAGE_10
#define ID_WINDOW_IDLE                  PAGE_15
#define ID_WINDOW_PHONEBOOK             PAGE_19
#define ID_WINDOW_PHONEBOOK_SYNC        PAGE_20
#define ID_WINDOW_MUSIC_SET             PAGE_21
#define ID_WINDOW_CALLRECORD            PAGE_22
#define ID_WINDOW_PAGE                  PAGE_23
#define ID_WINDOW_MUSIC_BROWER          PAGE_28
#define ID_WINDOW_PC                    PAGE_32
#define ID_WINDOW_STOPWATCH             PAGE_33
#define ID_WINDOW_CALCULAGRAPH          PAGE_34
#define ID_WINDOW_SET                   PAGE_41
#define ID_WINDOW_ALARM                 PAGE_51
#define ID_WINDOW_FLASHLIGHT            PAGE_52
#define ID_WINDOW_FINDPHONE             PAGE_53
#define ID_WINDOW_TRAIN                 PAGE_54
#define ID_WINDOW_SPORT_SHOW            PAGE_55
#define ID_WINDOW_BREATH_TRAIN          PAGE_57
#define ID_WINDOW_ALTIMETER				PAGE_63
#define ID_WINDOW_BARO                  PAGE_64
#define ID_WINDOW_WEATHER               PAGE_66
#define ID_WINDOW_PRESSURE              PAGE_67
#define ID_WINDOW_MESS                  PAGE_74
#define ID_WINDOW_HEART                 PAGE_77
#define ID_WINDOW_SPORT_RECORD          PAGE_75
#define ID_WINDOW_BLOOD_OXYGEN          PAGE_78
#define ID_WINDOW_SPORT_INFO            PAGE_79
#define ID_WINDOW_SPORT_CTRL			PAGE_80
#define ID_WINDOW_DETECTION				PAGE_82
#define ID_WINDOW_FALL					PAGE_81
#define ID_WINDOW_SHUTDOWN_OR_RESET     0
// #define ID_WINDOW_BT                 PAGE_17
// #define ID_WINDOW_BT_MENU            PAGE_3
// #define ID_WINDOW_TARGET             PAGE_54
#define ID_WINDOW_TRAIN_STATUS			0
#define ID_WINDOW_UPGRADE			    PAGE_0

#define ID_WINDOW_ALARM_RING_START			    PAGE_71
#define ID_WINDOW_ALARM_RING_STOP			    PAGE_72
#define ID_WINDOW_ALARM_RING_SOON			    PAGE_73

#define CONFIG_UI_STYLE_JL_ENABLE

#elif (CONFIG_UI_STYLE == STYLE_JL_WTACH_NEW)

#include "ui/style_JL_new.h"
#include "ui/style_DIAL_new.h"
#include "ui/style_upgrade_new.h"
#include "ui/style_sidebar.h"

#ifdef PAGE_0
#undef PAGE_0
#define PAGE_0 DIAL_PAGE_0
#endif

// #define ID_WINDOW_BT                    PAGE_76
#define ID_WINDOW_DIAL                  PAGE_0
#define ID_WINDOW_BT                    PAGE_0
#define ID_WINDOW_CLOCK                 PAGE_0
#define ID_WINDOW_ACTIVERECORD          PAGE_1
#define ID_WINDOW_SLEEP                 PAGE_2
#define ID_WINDOW_MAIN                  PAGE_3
#define ID_WINDOW_POWER_ON              PAGE_4
#define ID_WINDOW_POWER_OFF             PAGE_5
#define ID_WINDOW_VMENU                 PAGE_7
#define ID_WINDOW_PHONE                 PAGE_6
#define ID_WINDOW_MUSIC                 PAGE_10
#define ID_WINDOW_IDLE                  PAGE_15
#define ID_WINDOW_PHONEBOOK             PAGE_19
#define ID_WINDOW_PHONEBOOK_SYNC        PAGE_20
#define ID_WINDOW_MUSIC_SET             PAGE_21
#define ID_WINDOW_CALLRECORD            PAGE_22
#define ID_WINDOW_PAGE                  PAGE_23
#define ID_WINDOW_MUSIC_BROWER          PAGE_28
#define ID_WINDOW_PC                    PAGE_32
#define ID_WINDOW_STOPWATCH             PAGE_33
#define ID_WINDOW_CALCULAGRAPH          PAGE_34
#define ID_WINDOW_SET                   PAGE_41
#define ID_WINDOW_ALARM                 PAGE_51
#define ID_WINDOW_FLASHLIGHT            PAGE_52
#define ID_WINDOW_FINDPHONE             PAGE_53
#define ID_WINDOW_TRAIN                 PAGE_54
#define ID_WINDOW_SPORT_SHOW            PAGE_55
#define ID_WINDOW_BREATH_TRAIN          PAGE_57
#define ID_WINDOW_ALTIMETER				PAGE_63
#define ID_WINDOW_BARO                  PAGE_64
#define ID_WINDOW_WEATHER               PAGE_66
#define ID_WINDOW_PRESSURE              PAGE_67
#define ID_WINDOW_COMPASS               PAGE_73
#define ID_WINDOW_MESS                  PAGE_81
#define ID_WINDOW_SPORT_INFO            PAGE_78
#define ID_WINDOW_HEART                 PAGE_76
#define ID_WINDOW_BLOOD_OXYGEN          PAGE_77
#define ID_WINDOW_SPORT_CTRL			PAGE_79
#define ID_WINDOW_SPORT_RECORD          PAGE_80
#define ID_WINDOW_SHUTDOWN_OR_RESET     PAGE_86
#define ID_WINDOW_TRAIN_STATUS			PAGE_87
#define ID_WINDOW_DETECTION				PAGE_84
#define ID_WINDOW_FALL					PAGE_83
#define ID_WINDOW_UPGRADE			    UPGRADE_PAGE_0
// #define ID_WINDOW_BT                 PAGE_17
// #define ID_WINDOW_BT_MENU            PAGE_3
// #define ID_WINDOW_TARGET             PAGE_54
#define ID_WINDOW_STAR_MENU 			PAGE_17
#define ID_WINDOW_RECORD				PAGE_96
#define ID_WINDOW_CALENDAR				PAGE_97
#define ID_WINDOW_CALCULATOR			PAGE_98
#define ID_WINDOW_DRAWER                PAGE_99

#define ID_WINDOW_SPORT_COURSE					PAGE_88
#define ID_WINDOW_ALARM_RING_START			    PAGE_74
#define ID_WINDOW_ALARM_RING_STOP			    PAGE_89
#define ID_WINDOW_ALARM_RING_SOON			    PAGE_90
#define ID_WINDOW_ALIPAY			            PAGE_94

#define ID_WINDOW_SPORT_COURSE					PAGE_88
#define ID_WINDOW_ALARM_RING_START			    PAGE_74
#define ID_WINDOW_ALARM_RING_STOP			    PAGE_89
#define ID_WINDOW_ALARM_RING_SOON			    PAGE_90

#define CONFIG_UI_STYLE_JL_ENABLE
#endif

#if(CONFIG_UI_STYLE == STYLE_JL_SOUNDBOX)
#include "ui/style_jl02.h"//点阵//
#define ID_WINDOW_MAIN     PAGE_0
#define ID_WINDOW_BT       PAGE_1
#define ID_WINDOW_FM       PAGE_2
#define ID_WINDOW_CLOCK    PAGE_3
#define ID_WINDOW_MUSIC    PAGE_4
#define ID_WINDOW_LINEIN   PAGE_0
#define ID_WINDOW_POWER_ON     PAGE_5
#define ID_WINDOW_POWER_OFF    PAGE_6
#define ID_WINDOW_SYS      PAGE_7
#endif

#if(CONFIG_UI_STYLE == STYLE_JL_LED7)//led7 显示
#define ID_WINDOW_BT           UI_BT_MENU_MAIN
#define ID_WINDOW_FM           UI_FM_MENU_MAIN
#define ID_WINDOW_CLOCK        UI_RTC_MENU_MAIN
#define ID_WINDOW_MUSIC        UI_MUSIC_MENU_MAIN
#define ID_WINDOW_LINEIN       UI_AUX_MENU_MAIN
#define ID_WINDOW_PC           UI_PC_MENU_MAIN
#define ID_WINDOW_POWER_ON     UI_IDLE_MENU_MAIN
#define ID_WINDOW_POWER_OFF    UI_IDLE_MENU_MAIN
#define ID_WINDOW_SPDIF        UI_IDLE_MENU_MAIN
#define ID_WINDOW_IDLE         UI_IDLE_MENU_MAIN
#endif


#if ((CONFIG_UI_STYLE == STYLE_JL_WTACH) || (CONFIG_UI_STYLE == STYLE_JL_WTACH_NEW))

#if (defined(ID_WINDOW_ACTIVERECORD) && defined(TCFG_UI_ENABLE_SPORTRECORD) && (!TCFG_UI_ENABLE_SPORTRECORD))
#undef ID_WINDOW_ACTIVERECORD
#define ID_WINDOW_ACTIVERECORD          0
#endif

#if (defined(ID_WINDOW_MUSIC) && defined(TCFG_UI_ENABLE_MUSIC) && (!TCFG_UI_ENABLE_MUSIC))
#undef ID_WINDOW_MUSIC
#define ID_WINDOW_MUSIC					0
#endif

#if (defined(ID_WINDOW_MUSIC_SET) && defined(TCFG_UI_ENABLE_MUSIC_MENU) && (!TCFG_UI_ENABLE_MUSIC_MENU))
#undef ID_WINDOW_MUSIC_SET
#define ID_WINDOW_MUSIC_SET             0
#endif

#if (defined(ID_WINDOW_MUSIC_BROWER) && defined(TCFG_UI_ENABLE_FILE) && (!TCFG_UI_ENABLE_FILE))
#undef ID_WINDOW_MUSIC_BROWER
#define ID_WINDOW_MUSIC_BROWER          0
#endif

#if (defined(ID_WINDOW_PHONEBOOK) && defined(TCFG_UI_ENABLE_PHONEBOOK) && (!TCFG_UI_ENABLE_PHONEBOOK))
#undef ID_WINDOW_PHONEBOOK
#define ID_WINDOW_PHONEBOOK				0
#undef ID_WINDOW_PHONEBOOK_SYNC
#define ID_WINDOW_PHONEBOOK_SYNC        0
#endif

#if (defined(ID_WINDOW_PC) && defined(TCFG_UI_ENABLE_PC) && (!TCFG_UI_ENABLE_PC))
#undef ID_WINDOW_PC
#define ID_WINDOW_PC                    0
#endif

#if (defined(ID_WINDOW_STOPWATCH) && defined(TCFG_UI_ENABLE_STOPWATCH) && (!TCFG_UI_ENABLE_STOPWATCH))
#undef ID_WINDOW_STOPWATCH
#define ID_WINDOW_STOPWATCH             0
#endif

#if (defined(ID_WINDOW_CALCULAGRAPH) && defined(TCFG_UI_ENABLE_TIMER_ACTION) && (!TCFG_UI_ENABLE_TIMER_ACTION))
#undef ID_WINDOW_CALCULAGRAPH
#define ID_WINDOW_CALCULAGRAPH          0
#endif

#if (defined(ID_WINDOW_ALARM) && defined(TCFG_UI_ENABLE_ALARM) && (!TCFG_UI_ENABLE_ALARM))
#undef ID_WINDOW_ALARM
#define ID_WINDOW_ALARM                 0
#endif

#if (defined(ID_WINDOW_FLASHLIGHT) && defined(TCFG_UI_ENABLE_FLASHLIGHT) && (!TCFG_UI_ENABLE_FLASHLIGHT))
#undef ID_WINDOW_FLASHLIGHT
#define ID_WINDOW_FLASHLIGHT            0
#endif

#if (defined(ID_WINDOW_FINDPHONE) && defined(TCFG_UI_ENABLE_FINDPHONE) && (!TCFG_UI_ENABLE_FINDPHONE))
#undef ID_WINDOW_FINDPHONE
#define ID_WINDOW_FINDPHONE             0
#endif

#if (defined(ID_WINDOW_ACTIVERECORD) && defined(TCFG_UI_ENABLE_SPORTRECORD) && (!TCFG_UI_ENABLE_SPORTRECORD))
#undef ID_WINDOW_ACTIVERECORD
#define ID_WINDOW_ACTIVERECORD          0
#endif

#if (defined(ID_WINDOW_ACTIVERECORD) && defined(TCFG_UI_ENABLE_SPORTRECORD) && (!TCFG_UI_ENABLE_SPORTRECORD))
#undef ID_WINDOW_ACTIVERECORD
#define ID_WINDOW_ACTIVERECORD          0
#endif

#if (defined(ID_WINDOW_ACTIVERECORD) && defined(TCFG_UI_ENABLE_SPORTRECORD) && (!TCFG_UI_ENABLE_SPORTRECORD))
#undef ID_WINDOW_ACTIVERECORD
#define ID_WINDOW_ACTIVERECORD          0
#endif

#if (defined(ID_WINDOW_WEATHER) && defined(TCFG_UI_ENABLE_WEATHER) && (!TCFG_UI_ENABLE_WEATHER))
#undef ID_WINDOW_WEATHER
#define ID_WINDOW_WEATHER               0
#endif

#if (defined(ID_WINDOW_MESS) && defined(TCFG_UI_ENABLE_NOTICE) && (!TCFG_UI_ENABLE_NOTICE))
#undef ID_WINDOW_MESS
#define ID_WINDOW_MESS                  0
#endif

#if (defined(ID_WINDOW_ALTIMETER) && defined(TCFG_UI_ENABLE_ALTIMETER) && (!TCFG_UI_ENABLE_ALTIMETER))
#undef ID_WINDOW_ALTIMETER
#define ID_WINDOW_ALTIMETER				0
#endif

#if (defined(ID_WINDOW_BARO) && defined(TCFG_UI_ENABLE_PRESSURE) && (!TCFG_UI_ENABLE_PRESSURE))
#undef ID_WINDOW_BARO
#define ID_WINDOW_BARO                  0
#endif

#if (defined(ID_WINDOW_SPORT_INFO) && defined(TCFG_UI_ENABLE_SPORT_INFO) && (!TCFG_UI_ENABLE_SPORT_INFO))
#undef ID_WINDOW_SPORT_INFO
#define ID_WINDOW_SPORT_INFO            0
#endif

#if (defined(ID_WINDOW_BLOOD_OXYGEN) && defined(TCFG_UI_ENABLE_OXYGEN) && (!TCFG_UI_ENABLE_OXYGEN))
#undef ID_WINDOW_BLOOD_OXYGEN
#define ID_WINDOW_BLOOD_OXYGEN          0
#endif

#if (defined(ID_WINDOW_SPORT_RECORD) && defined(TCFG_UI_ENABLE_SPORTRECORD) && (!TCFG_UI_ENABLE_SPORTRECORD))
#undef ID_WINDOW_SPORT_RECORD
#define ID_WINDOW_SPORT_RECORD          0
#endif

#if (defined(ID_WINDOW_TRAIN) && defined(TCFG_UI_ENABLE_TRAIN) && (!TCFG_UI_ENABLE_TRAIN))
#undef ID_WINDOW_TRAIN
#define ID_WINDOW_TRAIN                 0
#undef ID_WINDOW_SPORT_SHOW
#define ID_WINDOW_SPORT_SHOW            0
#endif


#if (defined(ID_WINDOW_BREATH_TRAIN) && defined(TCFG_UI_ENABLE_BREATH_TRAIN) && (!TCFG_UI_ENABLE_BREATH_TRAIN))
#undef ID_WINDOW_BREATH_TRAIN
#define ID_WINDOW_BREATH_TRAIN          0
#endif

#if (defined(ID_WINDOW_PRESSURE) && defined(TCFG_UI_ENABLE_HEAT) && (!TCFG_UI_ENABLE_HEAT))
#undef ID_WINDOW_PRESSURE
#define ID_WINDOW_PRESSURE          	0
#endif

#if (defined(ID_WINDOW_HEART) && defined(TCFG_UI_ENABLE_HEART) && (!TCFG_UI_ENABLE_HEART))
#undef ID_WINDOW_HEART
#define ID_WINDOW_HEART                 0
#endif

#if (defined(ID_WINDOW_SLEEP) && defined(TCFG_UI_ENABLE_SLEEP) && (!TCFG_UI_ENABLE_SLEEP))
#undef ID_WINDOW_SLEEP
#define ID_WINDOW_SLEEP                 0
#endif

#endif // #if ((CONFIG_UI_STYLE == STYLE_JL_WTACH) || (CONFIG_UI_STYLE == STYLE_JL_WTACH_NEW))

#endif
