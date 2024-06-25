#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

/*
 *  板级配置选择
 */
// #define CONFIG_BOARD_701N_DEMO
// #define CONFIG_BOARD_7012A_DEMO
// #define CONFIG_BOARD_7012A7_DEMO //内封8/16m flash
#define CONFIG_BOARD_701N_LVGL_DEMO
// #define CONFIG_BOARD_AD2802E_DEMO

#include "media/audio_def.h"
#include "board_701n_demo/board_701n_demo_cfg.h"
#include "board_7012a_demo/board_7012a_demo_cfg.h"
#include "board_7012a7_demo/board_7012a7_demo_cfg.h"
#include "board_701n_lvgl_demo/board_701n_lvgl_demo_cfg.h"
#include "board_ad2802e_demo/board_ad2802e_demo_cfg.h"

//#define  DUT_AUDIO_DAC_LDO_VOLT   				DACVDD_LDO_2_80V

//*********************************************************************************//
//                                 编译警告                                         //
//*********************************************************************************//
#if ((TCFG_COMM_TYPE == TCFG_UART_COMM) && TCFG_ONLINE_ENABLE) && (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE)
#error "eq online adjust enable, plaease close usb marco"
#endif// ((TCFG_ONLINE_ENABLE) && (TCFG_PC_ENABLE || TCFG_UDISK_ENABLE))

#if ((TCFG_COMM_TYPE == TCFG_UART_COMM) && TCFG_CFG_TOOL_ENABLE ) && TCFG_PC_ENABLE
#error "soundbox tool enable, plaease close pc marco"
#endif// (TCFG_CFG_TOOL_ENABLE) && (TCFG_PC_ENABLE)




#if ((AUDIO_EQUALLOUDNESS_CONFIG) && (SYS_VOL_TYPE == VOL_TYPE_ANALOG))
#error "开启等响度 需要使用数字音量"
#endif


#if AUDIO_EQUALLOUDNESS_CONFIG
#if (TCFG_EQ_ENABLE == 0)
#error "开启等响度 需要打开EQ总使能"
#endif
#if (TCFG_EQ_ENABLE && TCFG_BT_MUSIC_EQ_ENABLE)
#error "开启等响度 需要打开EQ总使能，关闭其他模式的eq,例如关闭蓝牙播歌eq"
#endif
#endif
#if defined(TCFG_MIC_DODGE_EN) && TCFG_MIC_DODGE_EN
#if !SYS_DIGVOL_GROUP_EN
#error "请使能软件数字音量SYS_DIGVOL_GROUP_EN"
#endif
#endif

#if TCFG_DYNAMIC_EQ_ENABLE && !TCFG_DRC_ENABLE //动态eq使能后，需接入drc进行幅度控制
#error "动态eq使能后，需使能TCFG_DRC_ENABLE接入drc进行幅度控制"
#endif
#if TCFG_DYNAMIC_EQ_ENABLE && !TCFG_AUDIO_MDRC_ENABLE
#error "动态eq使能后，需使能TCFG_AUDIO_MDRC_ENABLE 为1接入多带drc进行幅度控制"
#endif
#if defined(SOUND_TRACK_2_P_X_CH_CONFIG)&&SOUND_TRACK_2_P_X_CH_CONFIG && LINEIN_MODE_SOLE_EQ_EN
#error "2.1/2.2声道不支持 linein模式独立的eq\drc配置"
#endif
#if defined(TCFG_EQ_DIVIDE_ENABLE)&&TCFG_EQ_DIVIDE_ENABLE && (TCFG_AUDIO_DAC_CONNECT_MODE != DAC_OUTPUT_FRONT_LR_REAR_LR)
#error "eq效果独立，需使能四声道宏DAC_OUTPUT_FRONT_LR_REAR_LR"
#endif
#if defined(SOUND_TRACK_2_P_X_CH_CONFIG)&&SOUND_TRACK_2_P_X_CH_CONFIG && defined(TCFG_EQ_DIVIDE_ENABLE)&& !TCFG_EQ_DIVIDE_ENABLE
#error  "2.1/2.2声道，需使能宏TCFG_EQ_DIVIDE_ENABLE"
#endif
#if defined(LINEIN_INPUT_WAY_ANALOG) && (TCFG_LINEIN_INPUT_WAY == LINEIN_INPUT_WAY_ANALOG)
#error "AC701N Linein 仅支持数字通路"
#endif
#if (AUDIO_OUT_WAY_TYPE == AUDIO_WAY_TYPE_IIS) && (SYS_VOL_TYPE != VOL_TYPE_DIGITAL)
#error "iis输出使用 数字音量"
#endif

#endif
