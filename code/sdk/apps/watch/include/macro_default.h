#ifndef MACRO_DEFAULT_H
#define MACRO_DEFAULT_H

#include "usb_std_class_def.h"
#include "usb_common_def.h"

//*********************************************************************************//
//						APP应用默认配置	                                           //
//注意！！！注意！！！注意！！！         	                                       //
//1、以下配置各个app应用方向的默认宏定义,没有定义的宏会在这里赋一个默认值	       //
//2、禁止直接修改这里的配置														   //
//3、新添加的宏一定要在此处添加默认值                                              //
//*********************************************************************************//
#ifndef DUT_AUDIO_DAC_LDO_VOLT
// #error "DUT_AUDIO_DAC_LDO_VOLT no define !!!!!!!!!!!!!!!!!!!!!!!!!!!!"
//#define  DUT_AUDIO_DAC_LDO_VOLT   							            DACVDD_LDO_2_90V
#endif

#ifndef TCFG_MIXER_EXT_ENABLE
#define TCFG_MIXER_EXT_ENABLE											0
#endif

#ifndef TCFG_PC_BACKMODE_ENABLE
#define TCFG_PC_BACKMODE_ENABLE											0
#endif

#ifndef TCFG_ENC_WRITE_FILE_ENABLE
#define TCFG_ENC_WRITE_FILE_ENABLE		                                0
#endif

#ifndef TCFG_MEDIA_LIB_USE_MALLOC
#define TCFG_MEDIA_LIB_USE_MALLOC										0
#endif

#ifndef TCFG_AEC_ENABLE
#define TCFG_AEC_ENABLE													0
#endif

#ifndef TCFG_DEV_UPDATE_IF_NOFILE_ENABLE
#define TCFG_DEV_UPDATE_IF_NOFILE_ENABLE								0
#endif

#ifndef TCFG_DEV_MANAGER_ENABLE
#define TCFG_DEV_MANAGER_ENABLE											0
#endif

#ifndef SD_BAUD_RATE_CHANGE_WHEN_SCAN
#define SD_BAUD_RATE_CHANGE_WHEN_SCAN                                   (12000000L)
#endif

#ifndef TCFG_REVERB_DODGE_EN
#define TCFG_REVERB_DODGE_EN                DISABLE_THIS_MOUDLE
#endif

#ifndef TCFG_CHARGE_BOX_ENABLE
#define TCFG_CHARGE_BOX_ENABLE             DISABLE_THIS_MOUDLE
#endif

#ifndef SOUNDCARD_ENABLE
#define SOUNDCARD_ENABLE				        0
#endif

#ifndef AUDIO_VOL_MANUAL
#define AUDIO_VOL_MANUAL			            0    // 手动调节系统音量
#endif

#ifndef TCFG_EQ_DIVIDE_ENABLE
#define TCFG_EQ_DIVIDE_ENABLE                   0    // 四声道eq是否独立  0 使用同个eq效果
#endif

#ifndef TCFG_MIXER_CYCLIC_TASK_EN
#define TCFG_MIXER_CYCLIC_TASK_EN			    0	// 循环mixer使能任务输出
#endif

#ifndef TCFG_DEC2TWS_TASK_ENABLE
#define TCFG_DEC2TWS_TASK_ENABLE			    0	// localtws使用单独task
#endif

#ifndef TCFG_NOR_FS_ENABLE
#define TCFG_NOR_FS_ENABLE                      0
#endif

#ifndef TCFG_APP_SLEEP_EN
#define TCFG_APP_SLEEP_EN                       0
#endif

#ifndef CONFIG_APP_DONGLE
#define CONFIG_APP_DONGLE						0
#endif

#ifndef TCFG_NANDFLASH_DEV_ENABLE
#define TCFG_NANDFLASH_DEV_ENABLE               0
#endif

#ifndef TCFG_GX8002_NPU_ENABLE
#define TCFG_GX8002_NPU_ENABLE                  0
#endif

#ifndef MIC_EFFECT_SAMPLERATE
#define MIC_EFFECT_SAMPLERATE			        (44100L)
#endif

#ifndef TCFG_KARAOKE_EARPHONE
#define TCFG_KARAOKE_EARPHONE                   0
#endif

#ifndef MUTIl_CHARGING_BOX_EN
#define MUTIl_CHARGING_BOX_EN                   0
#endif

#ifndef TCFG_USB_MIC_DATA_FROM_DAC
#define TCFG_USB_MIC_DATA_FROM_DAC              0
#endif

#ifndef TCFG_PC_MODE_DRC_ENABLE
#define TCFG_PC_MODE_DRC_ENABLE                 0
#endif

#ifndef APP_ONLINE_DEBUG
#define APP_ONLINE_DEBUG                        0
#endif

#ifndef USER_UART_UPDATE_ENABLE
#define USER_UART_UPDATE_ENABLE                 0//用于客户开发上位机或者多MCU串口升级方案
#endif

//配置双模同名字，同地址
#ifndef DOUBLE_BT_SAME_NAME
#define DOUBLE_BT_SAME_NAME               0 //同名字
#endif
#ifndef DOUBLE_BT_SAME_MAC
#define DOUBLE_BT_SAME_MAC                0 //同地址
#endif


#ifndef TCFG_CFG_TOOL_ENABLE
#define TCFG_CFG_TOOL_ENABLE				0
#endif

#ifndef TCFG_EFFECT_TOOL_ENABLE
#define TCFG_EFFECT_TOOL_ENABLE					0
#endif


//*********************************************************************************//
//						        SDFILE CONFIG                                      //
//*********************************************************************************//
#ifndef CONFIG_SDFILE_ENABLE
#define CONFIG_SDFILE_ENABLE
#endif
#ifndef CONFIG_FLASH_SIZE
#define CONFIG_FLASH_SIZE       (1024 * 1024)
#endif
#ifndef SDFILE_DEV
#define SDFILE_DEV				    "sdfile"
#endif
#ifndef SDFILE_MOUNT_PATH
#define SDFILE_MOUNT_PATH     	    "mnt/sdfile"
#endif

#if (USE_SDFILE_NEW)
#ifndef SDFILE_APP_ROOT_PATH
#define SDFILE_APP_ROOT_PATH       	SDFILE_MOUNT_PATH"/app/"  //app分区
#endif
#ifndef SDFILE_RES_ROOT_PATH
#define SDFILE_RES_ROOT_PATH       	SDFILE_MOUNT_PATH"/res/"  //资源文件分区
#endif
#else
#ifndef SDFILE_RES_ROOT_PATH
#define SDFILE_RES_ROOT_PATH       	SDFILE_MOUNT_PATH"/C/"
#endif
#endif //(USE_SDFILE_NEW)



//*********************************************************************************//
//              rtc时钟源选择:CLK_SEL_32K/CLK_SEL_12M/CLK_SEL_24M
//*********************************************************************************//
#ifndef RTC_CLK_RES_SEL
#define RTC_CLK_RES_SEL             CLK_SEL_32K
#endif


//*********************************************************************************//
//                                 电源切换配置                                    //
//*********************************************************************************//
#ifndef PHONE_CALL_USE_LDO15
#define PHONE_CALL_USE_LDO15            0
#endif


//*********************************************************************************//
//                                 SD、U盘升级配置                                 //
//*********************************************************************************//
#if TCFG_APP_MUSIC_EN
#ifndef CONFIG_SD_UPDATE_ENABLE
#define CONFIG_SD_UPDATE_ENABLE
#endif
#ifndef CONFIG_USB_UPDATE_ENABLE
#define CONFIG_USB_UPDATE_ENABLE
#endif
#endif

//*********************************************************************************//
//                                 升级配置                                        //
//*********************************************************************************//
//升级LED显示使能
#ifndef UPDATE_LED_REMIND
#define UPDATE_LED_REMIND
#endif
//升级提示音使能
#ifndef UPDATE_VOICE_REMIND
// #define UPDATE_VOICE_REMIND
#endif

//*********************************************************************************//
//                                 录音配置                                        //
//*********************************************************************************//
//录音文件夹名称定义，可以通过修改此处修改录音文件夹名称
#ifndef REC_FOLDER_NAME
#define REC_FOLDER_NAME				"JL_REC"
#endif

//*********************************************************************************//
//                                    charge
//*********************************************************************************//
#ifndef tcfg_anc_box_enable
#define tcfg_anc_box_enable                     0
#endif
#ifndef TCFG_SHORT_PROTECT_ENABLE
#define TCFG_SHORT_PROTECT_ENABLE               0
#endif
#ifndef TCFG_CHARGE_MOUDLE_OUTSIDE
#define TCFG_CHARGE_MOUDLE_OUTSIDE              0
#endif
#ifndef TCFG_WIRELESS_ENABLE
#define TCFG_WIRELESS_ENABLE                    0
#endif
#ifndef TCFG_LDO_DET_ENABLE
#define TCFG_LDO_DET_ENABLE                     0
#endif
#ifndef TCFG_CURRENT_LIMIT_ENABLE
#define TCFG_CURRENT_LIMIT_ENABLE               0
#endif
#ifndef TCFG_PWR_CTRL_TYPE
#define TCFG_PWR_CTRL_TYPE                      0
#endif
#ifndef PWR_CTRL_TYPE_PU_PD
#define PWR_CTRL_TYPE_PU_PD                     0
#endif

//*********************************************************************************//
//                                    chargebox
//*********************************************************************************//
#ifndef TCFG_ANC_BOX_ENABLE
#define TCFG_ANC_BOX_ENABLE                     0
#endif

#ifndef TCFG_BOOST_CTRL_IO
#define TCFG_BOOST_CTRL_IO                     NO_CONFIG_PORT
#endif
#ifndef TCFG_PWR_CTRL_IO
#define TCFG_PWR_CTRL_IO                       NO_CONFIG_PORT
#endif

#ifndef TCFG_TEMPERATURE_ENABLE
#define TCFG_TEMPERATURE_ENABLE                0
#endif
#ifndef TCFG_USB_KEY_UPDATE_ENABLE
#define TCFG_USB_KEY_UPDATE_ENABLE             0
#endif
#ifndef TCFG_HANDSHAKE_ENABLE
#define TCFG_HANDSHAKE_ENABLE                  0
#endif
#ifndef TCFG_CHARGE_FULL_ENTER_SOFTOFF
#define TCFG_CHARGE_FULL_ENTER_SOFTOFF         0
#endif
#ifndef TCFG_CHARGE_BOX_UI_ENABLE
#define TCFG_CHARGE_BOX_UI_ENABLE              0
#endif

#ifndef TCFG_BAT_DET_AD_CH
#define TCFG_BAT_DET_AD_CH                     AD_CH_VBAT
#endif

//*********************************************************************************//
//                                    bt
//*********************************************************************************//
#ifndef TCFG_VIRTUAL_FAST_CONNECT_FOR_EMITTER
#define TCFG_VIRTUAL_FAST_CONNECT_FOR_EMITTER   0
#endif

#ifndef TRANS_MULTI_SPP_EN
#define TRANS_MULTI_SPP_EN                      0
#endif

#ifndef USER_SUPPORT_DUAL_A2DP_SOURCE
#define USER_SUPPORT_DUAL_A2DP_SOURCE          0
#endif

#ifndef TCFG_ADSP_UART_ENABLE
#define TCFG_ADSP_UART_ENABLE                  0
#endif

#ifndef TUYA_TRANS_EN
#define TUYA_TRANS_EN 							0
#endif

#ifndef TUYA_DEMO_EN
#define TUYA_DEMO_EN							0
#endif

#ifndef AI_APP_PROTOCOL
#define AI_APP_PROTOCOL							0
#endif

#ifndef CONFIG_BT_RX_BUFF_SIZE
#define CONFIG_BT_RX_BUFF_SIZE                  (12 * 1024)
#endif
#ifndef CONFIG_BT_TX_BUFF_SIZE
#define CONFIG_BT_TX_BUFF_SIZE                  (3 * 1024)
#endif
// #ifndef CONFIG_TWS_BULK_POOL_SIZE
// #define CONFIG_TWS_BULK_POOL_SIZE               (2 * 1024)
// #endif

#ifndef USER_SUPPORT_PROFILE_PBAP_LIST
#define USER_SUPPORT_PROFILE_PBAP_LIST   		0
#endif

//*********************************************************************************//
//                                    music
//*********************************************************************************//
#ifndef TCFG_LRC_LYRICS_ENABLE
#define TCFG_LRC_LYRICS_ENABLE                  0
#endif

#ifndef ANC_TRAIN_MODE
#define ANC_TRAIN_MODE					    0
#endif

#ifndef MUSIC_AB_RPT_EN
#define MUSIC_AB_RPT_EN                         0
#endif

#ifndef TCFG_LFN_EN
#define TCFG_LFN_EN                             0
#endif

#ifndef TCFG_RECORD_FOLDER_DEV_ENABLE
#define TCFG_RECORD_FOLDER_DEV_ENABLE          0
#endif

#ifndef TCFG_BROADCAST_ENABLE
#define TCFG_BROADCAST_ENABLE					0
#endif

//*********************************************************************************//
//                                    FM
//*********************************************************************************//
#ifndef TCFG_LINEIN_MULTIPLEX_WITH_FM
#define TCFG_LINEIN_MULTIPLEX_WITH_FM           0
#endif

//*********************************************************************************//
//                                    KEY
//*********************************************************************************//
#ifndef TCFG_IRSENSOR_ENABLE
#define TCFG_IRSENSOR_ENABLE                  0
#endif

#ifndef MOUSE_KEY_SCAN_MODE
#define MOUSE_KEY_SCAN_MODE                   0
#endif

#ifndef TCFG_ADKEY_RTCVDD_ENABLE
#define TCFG_ADKEY_RTCVDD_ENABLE             0
#endif

#ifndef TCFG_SLIDE_KEY_ENABLE
#define TCFG_SLIDE_KEY_ENABLE               0
#endif

#ifndef TCFG_6083_ADKEY_ENABLE
#define TCFG_6083_ADKEY_ENABLE              0
#endif

#ifndef TCFG_TENT600_KEY_ENABLE
#define TCFG_TENT600_KEY_ENABLE             0
#endif

#ifndef TCFG_CTMU_TOUCH_KEY_ENABLE
#define TCFG_CTMU_TOUCH_KEY_ENABLE          0
#endif

#ifndef TCFG_LP_TOUCH_KEY_ENABLE
#define TCFG_LP_TOUCH_KEY_ENABLE            0
#endif

#ifndef TCFG_TOUCH_KEY_ENABLE
#define TCFG_TOUCH_KEY_ENABLE               0
#endif

#ifndef TCFG_UART_KEY_ENABLE
#define TCFG_UART_KEY_ENABLE				0
#endif

#ifndef TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
#define TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE	0
#endif


//*********************************************************************************//
//                                  audio配置                                   //
//*********************************************************************************//
#ifndef TCFG_AUDIO_ANC_ENABLE
#define TCFG_AUDIO_ANC_ENABLE               0
#endif

#ifndef TCFG_ANC_TOOL_DEBUG_ONLINE
#define TCFG_ANC_TOOL_DEBUG_ONLINE          0
#endif

#ifndef TCFG_SUPPORT_MIC_CAPLESS
#define TCFG_SUPPORT_MIC_CAPLESS            0
#endif

#ifndef DAC2IIS_EN
#define DAC2IIS_EN                          0
#endif

#ifndef TCFG_MIC_EFFECT_ENABLE
#define TCFG_MIC_EFFECT_ENABLE             0
#endif

#ifndef TCFG_MIC_DODGE_EN
#define TCFG_MIC_DODGE_EN                  0
#endif

#ifndef TCFG_IIS_ENABLE
#define TCFG_IIS_ENABLE                    0
#endif

#ifndef RECORDER_MIX_EN
#define RECORDER_MIX_EN                    0
#endif
#ifndef AUDIO_SPECTRUM_CONFIG
#define AUDIO_SPECTRUM_CONFIG              0
#endif
#ifndef TCFG_IIS_OUTPUT_EN
#define TCFG_IIS_OUTPUT_EN                 0
#endif
#ifndef TCFG_AUDIO_DEC_OUT_TASK
#define TCFG_AUDIO_DEC_OUT_TASK            0
#endif
#ifndef TCFG_KEY_TONE_EN
#define TCFG_KEY_TONE_EN                   0
#endif

#ifndef AUDIO_MIDI_CTRL_CONFIG
#define AUDIO_MIDI_CTRL_CONFIG             0
#endif

#ifndef TCFG_IIS_INPUT_EN
#define TCFG_IIS_INPUT_EN                  0
#endif

#ifndef AUDIO_VOCAL_REMOVE_EN
#define AUDIO_VOCAL_REMOVE_EN              0
#endif

#ifndef TCFG_AUDIO_DECODER_OCCUPY_TRACE
#define TCFG_AUDIO_DECODER_OCCUPY_TRACE			DISABLE
#endif

#ifndef APP_AUDIO_STATE_WTONE_BY_MUSIC
#define APP_AUDIO_STATE_WTONE_BY_MUSIC 			DISABLE
#endif

#ifndef TONE_MODE_DEFAULE_VOLUME
#define TONE_MODE_DEFAULE_VOLUME				DISABLE
#endif

#ifndef TCFG_AEC_TOOL_ONLINE_ENABLE
#define TCFG_AEC_TOOL_ONLINE_ENABLE				DISABLE
#endif

#ifndef TCFG_SMART_VOICE_ENABLE
#define TCFG_SMART_VOICE_ENABLE 		    	DISABLE
#endif

#ifndef TCFG_KWS_VOICE_EVENT_HANDLE_ENABLE
#define TCFG_KWS_VOICE_EVENT_HANDLE_ENABLE 		DISABLE
#endif

#ifndef TCFG_CALL_KWS_SWITCH_ENABLE
#define TCFG_CALL_KWS_SWITCH_ENABLE         	DISABLE
#endif

#ifndef TCFG_VAD_LP_CLOSE
#define TCFG_VAD_LP_CLOSE						DISABLE
#endif

//*********************************************************************************//
//                                    SD
//*********************************************************************************//

#ifndef TCFG_SD0_SD1_USE_THE_SAME_HW
#define TCFG_SD0_SD1_USE_THE_SAME_HW            0
#endif
#ifndef TCFG_SDX_CAN_OPERATE_MMC_CARD
#define TCFG_SDX_CAN_OPERATE_MMC_CARD           0
#endif

#ifndef TCFG_IO_MULTIPLEX_WITH_SD
#define TCFG_IO_MULTIPLEX_WITH_SD               0
#endif
//*********************************************************************************//
//                                  updata配置                                   //
//*********************************************************************************//
#ifndef RCSP_UPDATE_EN
#define RCSP_UPDATE_EN                  0
#endif
#ifndef OTA_TWS_SAME_TIME_ENABLE
#define OTA_TWS_SAME_TIME_ENABLE        0
#endif
#ifndef JL_SMART_BOX_EXTRA_FLASH_OPT
#define JL_SMART_BOX_EXTRA_FLASH_OPT    0
#endif
#ifndef CONFIG_DOUBLE_BANK_ENABLE
#define CONFIG_DOUBLE_BANK_ENABLE       0
#endif
#ifndef OTA_TWS_SAME_TIME_NEW
#define OTA_TWS_SAME_TIME_NEW           0
#endif
#ifndef UPDATE_MD5_ENABLE
#define UPDATE_MD5_ENABLE               0
#endif


//*********************************************************************************//
//                                  三方平台配置                                   //
//*********************************************************************************//
#ifndef DUEROS_DMA_EN
#define DUEROS_DMA_EN       0
#endif
#ifndef USE_DMA_TONE
#define USE_DMA_TONE        0
#endif

#ifndef TRANS_DATA_EN
#define TRANS_DATA_EN       0
#endif
#ifndef BLE_CLIENT_EN
#define BLE_CLIENT_EN 			 		       0
#endif
#ifndef RCSP_BTMATE_EN
#define RCSP_BTMATE_EN      0
#endif
#ifndef RCSP_ADV_EN
#define RCSP_ADV_EN         0
#endif
#ifndef RCSP_ADV_NAME_SET_ENABLE
#define RCSP_ADV_NAME_SET_ENABLE                0
#endif

#ifndef RCSP_ADV_KEY_SET_ENABLE
#define RCSP_ADV_KEY_SET_ENABLE                 0
#endif

#ifndef RCSP_ADV_LED_SET_ENABLE
#define RCSP_ADV_LED_SET_ENABLE                 0
#endif

#ifndef RCSP_ADV_MIC_SET_ENABLE
#define RCSP_ADV_MIC_SET_ENABLE                 0
#endif

#ifndef RCSP_ADV_WORK_SET_ENABLE
#define RCSP_ADV_WORK_SET_ENABLE                0
#endif

#ifndef RCSP_ADV_HIGH_LOW_SET
#define RCSP_ADV_HIGH_LOW_SET                   0
#endif
#ifndef RCSP_ADV_MUSIC_INFO_ENABLE
#define RCSP_ADV_MUSIC_INFO_ENABLE              0
#endif
#ifndef RCSP_ADV_KARAOKE_SET_ENABLE
#define RCSP_ADV_KARAOKE_SET_ENABLE             0
#endif

#ifndef RCSP_ADV_PRODUCT_MSG_ENABLE
#define RCSP_ADV_PRODUCT_MSG_ENABLE        		0
#endif
#ifndef RCSP_ADV_COLOR_LED_SET_ENABLE
#define RCSP_ADV_COLOR_LED_SET_ENABLE   		0
#endif
#ifndef RCSP_ADV_KARAOKE_EQ_SET_ENABLE
#define RCSP_ADV_KARAOKE_EQ_SET_ENABLE			0
#endif
#ifndef RCSP_ADV_EQ_SET_ENABLE
#define RCSP_ADV_EQ_SET_ENABLE          		0
#endif
#ifndef RCSP_ADV_FIND_DEVICE_ENABLE
#define RCSP_ADV_FIND_DEVICE_ENABLE				0
#endif

#ifndef GMA_EN
#define GMA_EN              0
#endif
#ifndef GMA_USED_FIXED_TRI_PARA
#define GMA_USED_FIXED_TRI_PARA    0
#endif
#ifndef SMART_BOX_EN
#define SMART_BOX_EN        0
#endif
#ifndef TME_EN
#define TME_EN              0
#endif
#ifndef ANCS_CLIENT_EN
#define ANCS_CLIENT_EN      0
#endif
#ifndef XM_MMA_EN
#define XM_MMA_EN           0
#endif
#ifndef XIAOMI_EN
#define XIAOMI_EN           0
#endif
#ifndef JL_EARPHONE_APP_EN
#define JL_EARPHONE_APP_EN  0
#endif
#ifndef RCSP_FILE_OPT
#define RCSP_FILE_OPT       0
#endif
#ifndef BT_MIC_EN
#define BT_MIC_EN           0
#endif
#ifndef LL_SYNC_EN
#define LL_SYNC_EN           0
#endif
#ifndef BLE_WIRELESS_MIC_SERVER_EN
#define BLE_WIRELESS_MIC_SERVER_EN           0
#endif
#ifndef BLE_WIRELESS_MIC_CLIENT_EN
#define BLE_WIRELESS_MIC_CLIENT_EN           0
#endif

#ifndef GMA_TWS_PAIR_USED_FIXED_MAC
#define GMA_TWS_PAIR_USED_FIXED_MAC         0
#endif

#ifndef APP_PROTOCOL_READ_CFG_EN
#define APP_PROTOCOL_READ_CFG_EN			0
#endif




//*********************************************************************************//
//                                 对耳配置方式配置                                    //
//*********************************************************************************//
#ifndef CONFIG_TWS_CONNECT_SIBLING_TIMEOUT
#define CONFIG_TWS_CONNECT_SIBLING_TIMEOUT      4    /* 开机或超时断开后对耳互连超时时间，单位s */
#endif //CONFIG_TWS_CONNECT_SIBLING_TIMEOUT

#ifndef CONFIG_TWS_REMOVE_PAIR_ENABLE
#define CONFIG_TWS_REMOVE_PAIR_ENABLE                /* 不连手机的情况下双击按键删除配对信息 */
#endif //CONFIG_TWS_REMOVE_PAIR_ENABLE

#ifndef CONFIG_TWS_POWEROFF_SAME_TIME
#define CONFIG_TWS_POWEROFF_SAME_TIME           0    /*按键关机时两个耳机同时关机*/
#endif //CONFIG_TWS_POWEROFF_SAME_TIME

#ifndef ONE_KEY_CTL_DIFF_FUNC
#define ONE_KEY_CTL_DIFF_FUNC                   0    /*通过左右耳实现一个按键控制两个功能*/
#endif //ONE_KEY_CTL_DIFF_FUNC

#ifndef CONFIG_TWS_SCO_ONLY_MASTER
#define CONFIG_TWS_SCO_ONLY_MASTER			    0	 /*通话的时候只有主机出声音*/
#endif //CONFIG_TWS_SCO_ONLY_MASTER

#ifndef CONFIG_TWS_PAIR_MODE
#define CONFIG_TWS_PAIR_MODE                    CONFIG_TWS_PAIR_BY_CLICK
#endif //CONFIG_TWS_PAIR_MODE

#ifndef CONFIG_TWS_USE_COMMMON_ADDR
#define CONFIG_TWS_USE_COMMMON_ADDR             1    /* tws 使用公共地址 */
#endif //CONFIG_TWS_USE_COMMMON_ADDR

#ifndef CONFIG_TWS_PAIR_ALL_WAY
#define CONFIG_TWS_PAIR_ALL_WAY                 0    /* tws 任何时候 链接搜索  */
#endif //CONFIG_TWS_PAIR_ALL_WAY

#ifndef CONFIG_TWS_PAIR_BY_BOTH_SIDES
#define CONFIG_TWS_PAIR_BY_BOTH_SIDES           0    /* tws 要两边同时按下才能进行配对 */
#endif //CONFIG_TWS_PAIR_BY_BOTH_SIDES

#ifndef CONFIG_TWS_DISCONN_NO_RECONN
#define CONFIG_TWS_DISCONN_NO_RECONN            0    /* 按键断开tws 不会回连 调用  tws_api_detach(TWS_DETACH_BY_REMOVE_NO_RECONN);*/
#endif //CONFIG_TWS_DISCONN_NO_RECONN

#ifndef CONFIG_TWS_CHANNEL_SELECT
#define CONFIG_TWS_CHANNEL_SELECT               CONFIG_TWS_LEFT_START_PAIR   //配对方式选择
#endif //CONFIG_TWS_CHANNEL_SELECT

#ifndef CONFIG_TWS_CHANNEL_CHECK_IO
#define CONFIG_TWS_CHANNEL_CHECK_IO             IO_PORTA_07					//上下拉电阻检测引脚
#endif //CONFIG_TWS_CHANNEL_CHECK_IO

#ifndef CONFIG_TWS_SECECT_CHARGESTORE_PRIO
#define CONFIG_TWS_SECECT_CHARGESTORE_PRIO      1                             //测试盒配置左右耳优先
#endif //CONFIG_TWS_SECECT_CHARGESTORE_PRIO

//对耳电量显示方式
#ifndef CONFIG_DISPLAY_TWS_BAT_TYPE
#define CONFIG_DISPLAY_TWS_BAT_TYPE             CONFIG_DISPLAY_TWS_BAT_LOWER
#endif //CONFIG_DISPLAY_TWS_BAT_TYPE

#ifndef CONFIG_DISPLAY_DETAIL_BAT
#define CONFIG_DISPLAY_DETAIL_BAT               0 //BLE广播显示具体的电量
#endif //CONFIG_DISPLAY_DETAIL_BAT

#ifndef CONFIG_NO_DISPLAY_BUTTON_ICON
#define CONFIG_NO_DISPLAY_BUTTON_ICON           1 //BLE广播不显示按键界面,智能充电仓置1
#endif //CONFIG_NO_DISPLAY_BUTTON_ICON

//*********************************************************************************//
//                                 FPGA特殊配置                                    //
//*********************************************************************************//
#ifdef CONFIG_FPGA_ENABLE
#undef TCFG_CLOCK_OSC_HZ
#define TCFG_CLOCK_OSC_HZ		12000000
#endif

//*********************************************************************************//
//                                 IIC配置                                        //
//*********************************************************************************//
/*软件IIC设置*/
#ifndef TCFG_SW_I2C0_CLK_PORT
#define TCFG_SW_I2C0_CLK_PORT               IO_PORTA_09                             //软件IIC  CLK脚选择
#endif
#ifndef TCFG_SW_I2C0_DAT_PORT
#define TCFG_SW_I2C0_DAT_PORT               IO_PORTA_10                             //软件IIC  DAT脚选择
#endif
#ifndef TCFG_SW_I2C0_DELAY_CNT
#define TCFG_SW_I2C0_DELAY_CNT              50                                      //IIC延时参数，影响通讯时钟频率
#endif

#ifndef TCFG_HW_I2C0_PORTS
#define TCFG_HW_I2C0_PORTS                  'B'
#endif
#ifndef TCFG_HW_I2C0_CLK
#define TCFG_HW_I2C0_CLK                    100000                                  //硬件IIC波特率
#endif


//*********************************************************************************//
//                                 硬件SPI 配置                                        //
//*********************************************************************************//
//spi 1
#ifndef TCFG_HW_SPI1_ENABLE
#define	TCFG_HW_SPI1_ENABLE		            DISABLE_THIS_MOUDLE
#endif
#ifndef TCFG_HW_SPI1_PORT
#define TCFG_HW_SPI1_PORT		            'A'
#endif
#ifndef TCFG_HW_SPI1_BAUD
#define TCFG_HW_SPI1_BAUD		            4000000L
#endif
#ifndef TCFG_HW_SPI1_MODE
#define TCFG_HW_SPI1_MODE		            SPI_MODE_BIDIR_1BIT
#endif
#ifndef TCFG_HW_SPI1_ROLE
#define TCFG_HW_SPI1_ROLE		            SPI_ROLE_MASTER
#endif

//spi 2
#ifndef TCFG_HW_SPI2_ENABLE
#define	TCFG_HW_SPI2_ENABLE		            DISABLE_THIS_MOUDLE
#endif
#ifndef TCFG_HW_SPI2_PORT
#define TCFG_HW_SPI2_PORT		            'A'
#endif
#ifndef TCFG_HW_SPI2_BAUD
#define TCFG_HW_SPI2_BAUD		            2000000L
#endif
#ifndef TCFG_HW_SPI2_MODE
#define TCFG_HW_SPI2_MODE		            SPI_MODE_BIDIR_1BIT
#endif
#ifndef TCFG_HW_SPI2_ROLE
#define TCFG_HW_SPI2_ROLE		            SPI_ROLE_MASTER
#endif

//*********************************************************************************//
//                                 FLASH 配置                                      //
//*********************************************************************************//
#ifndef TCFG_NORFLASH_DEV_ENABLE
#define TCFG_NORFLASH_DEV_ENABLE			DISABLE_THIS_MOUDLE //需要关闭SD0
#endif
#ifndef TCFG_FLASH_DEV_SPI_HW_NUM
#define TCFG_FLASH_DEV_SPI_HW_NUM			1// 1: SPI1    2: SPI2
#endif
#ifndef TCFG_FLASH_DEV_SPI_CS_PORT
#define TCFG_FLASH_DEV_SPI_CS_PORT	    	IO_PORTA_03
#endif

#ifndef TCFG_VIRFAT_INSERT_FLASH_ENABLE
#define TCFG_VIRFAT_INSERT_FLASH_ENABLE		0
#endif

//*********************************************************************************//
//                                  充电参数配置                                   //
//*********************************************************************************//
//是否支持芯片内置充电
#ifndef TCFG_CHARGE_ENABLE
#define TCFG_CHARGE_ENABLE					DISABLE_THIS_MOUDLE
#endif
//是否支持开机充电
#ifndef TCFG_CHARGE_POWERON_ENABLE
#define TCFG_CHARGE_POWERON_ENABLE			DISABLE
#endif
//是否支持拔出充电自动开机功能
#ifndef TCFG_CHARGE_OFF_POWERON_NE
#define TCFG_CHARGE_OFF_POWERON_NE			DISABLE
#endif
#ifndef TCFG_CHARGE_FULL_V
#define TCFG_CHARGE_FULL_V					CHARGE_FULL_V_4202
#endif
#ifndef TCFG_CHARGE_FULL_MA
#define TCFG_CHARGE_FULL_MA					CHARGE_FULL_mA_10
#endif
#ifndef TCFG_CHARGE_MA
#define TCFG_CHARGE_MA						CHARGE_mA_60
#endif


//*********************************************************************************//
//                                 USB 配置                                        //
//*********************************************************************************//
#ifndef TCFG_PC_ENABLE
#define TCFG_PC_ENABLE						0 //PC模块使能
#endif
#ifndef TCFG_USB_SLAVE_USER_HID
#define TCFG_USB_SLAVE_USER_HID				0
#endif
#ifndef TCFG_UDISK_ENABLE
#define TCFG_UDISK_ENABLE					0 //U盘模块使能
#endif
#ifndef TCFG_OTG_USB_DEV_EN
#define TCFG_OTG_USB_DEV_EN                 0 //USB0 = BIT(0)  USB1 = BIT(1)
#endif
#ifndef USB_MALLOC_ENABLE
#define USB_MALLOC_ENABLE                   0
#endif
#ifndef TCFG_USB_PORT_CHARGE
#define TCFG_USB_PORT_CHARGE                0
#endif
#ifndef TCFG_USB_DM_MULTIPLEX_WITH_SD_DAT0
#define TCFG_USB_DM_MULTIPLEX_WITH_SD_DAT0  0
#endif
#ifndef USB_PC_NO_APP_MODE
#define  USB_PC_NO_APP_MODE                 0
#endif
#ifndef TCFG_DM_MULTIPLEX_WITH_SD_PORT
#define TCFG_DM_MULTIPLEX_WITH_SD_PORT      0 //0:sd0  1:sd1 //dm 参与复用的sd配置
#endif
#ifndef USB_MEM_NO_USE_OVERLAY_EN
#define USB_MEM_NO_USE_OVERLAY_EN		    0
#endif

#ifndef TCFG_HID_HOST_ENABLE
#define TCFG_HID_HOST_ENABLE                0
#endif
#ifndef TCFG_AOA_ENABLE
#define TCFG_AOA_ENABLE                     0
#endif
#ifndef TCFG_ADB_ENABLE
#define TCFG_ADB_ENABLE                     0
#endif
#ifndef TCFG_USB_APPLE_DOCK_EN
#define TCFG_USB_APPLE_DOCK_EN              0
#endif
#ifndef TCFG_HOST_AUDIO_ENABLE
#define TCFG_HOST_AUDIO_ENABLE              0
#endif
#ifndef TCFG_USB_MIC_ECHO_ENABLE
#define TCFG_USB_MIC_ECHO_ENABLE            0
#endif
#ifndef TCFG_USB_MIC_DATA_FROM_MICEFFECT
#define TCFG_USB_MIC_DATA_FROM_MICEFFECT    0
#endif
#ifndef TCFG_USB_CUSTOM_HID_ENABLE
#define TCFG_USB_CUSTOM_HID_ENABLE			0
#endif
#ifndef TCFG_USB_SLAVE_AUDIO_ENABLE
#define TCFG_USB_SLAVE_AUDIO_ENABLE			0
#endif
#ifndef TCFG_VIR_UDISK_ENABLE
#define TCFG_VIR_UDISK_ENABLE               0
#endif
#ifndef PC_VOL_INDEPENDENT_EN
#define PC_VOL_INDEPENDENT_EN				0
#endif

#ifndef TCFG_TYPEC_EARPHONE_TEST_FILTER
#define TCFG_TYPEC_EARPHONE_TEST_FILTER		0
#endif


//*********************************************************************************//
//                                  SD 配置                                        //
//*********************************************************************************//
#ifndef TCFG_SD0_ENABLE
#define TCFG_SD0_ENABLE					    0
#endif
#ifndef TCFG_SD0_PORTS
#define TCFG_SD0_PORTS						'D'
#endif
#ifndef TCFG_SD0_DAT_MODE
#define TCFG_SD0_DAT_MODE					1//AC696x不支持4线模式
#endif
#ifndef TCFG_SD0_DET_MODE
#define TCFG_SD0_DET_MODE                   SD_CLK_DECT
#endif
#ifndef TCFG_SD0_DET_IO
#define TCFG_SD0_DET_IO 					IO_PORT_DM//当SD_DET_MODE为2时有效
#endif
#ifndef TCFG_SD0_DET_IO_LEVEL
#define TCFG_SD0_DET_IO_LEVEL				0//IO检查，0：低电平检测到卡。 1：高电平(外部电源)检测到卡。 2：高电平(SD卡电源)检测到卡。
#endif
#ifndef TCFG_SD0_CLK
#define TCFG_SD0_CLK						(3000000*2L)
#endif

#ifndef TCFG_SD1_ENABLE
#define TCFG_SD1_ENABLE						0
#endif
#ifndef TCFG_SD1_PORTS
#define TCFG_SD1_PORTS						'F'
#endif
#ifndef TCFG_SD1_DAT_MODE
#define TCFG_SD1_DAT_MODE					1//AC696x不支持4线模式
#endif
#ifndef TCFG_SD1_DET_MODE
#define TCFG_SD1_DET_MODE					SD_CLK_DECT
#endif
#ifndef TCFG_SD1_DET_IO
#define TCFG_SD1_DET_IO 					IO_PORT_DM//当SD_DET_MODE为2时有效
#endif
#ifndef TCFG_SD1_DET_IO_LEVEL
#define TCFG_SD1_DET_IO_LEVEL				0//IO检查，0：低电平检测到卡。 1：高电平(外部电源)检测到卡。 2：高电平(SD卡电源)检测到卡。
#endif
#ifndef TCFG_SD1_CLK
#define TCFG_SD1_CLK						(3000000*2L)
#endif

#ifndef TCFG_KEEP_CARD_AT_ACTIVE_STATUS
#define TCFG_KEEP_CARD_AT_ACTIVE_STATUS     0 //保持卡活跃状态
#endif

//*********************************************************************************//
//                                 fat_FLASH 配置                                      //
//*********************************************************************************//

#ifndef TCFG_CODE_FLASH_ENABLE
#define TCFG_CODE_FLASH_ENABLE				DISABLE_THIS_MOUDLE
#endif
#ifndef FLASH_INSIDE_REC_ENABLE
#define FLASH_INSIDE_REC_ENABLE             0
#endif
#ifndef TCFG_NOR_FAT
#define TCFG_NOR_FAT                        0
#endif
#ifndef TCFG_NOR_FS
#define TCFG_NOR_FS                         0
#endif
#ifndef TCFG_NOR_REC
#define TCFG_NOR_REC                        0
#endif
#ifndef TCFG_VIRFAT_FLASH_ENABLE
#define TCFG_VIRFAT_FLASH_ENABLE            0
#endif

//*********************************************************************************//
//                                 key 配置                                        //
//*********************************************************************************//
#ifndef KEY_IO_NUM_MAX
#define KEY_IO_NUM_MAX					    6
#endif
#ifndef KEY_AD_NUM_MAX
#define KEY_AD_NUM_MAX						10
#endif
#ifndef KEY_IR_NUM_MAX
#define KEY_IR_NUM_MAX					    21
#endif
#ifndef KEY_TOUCH_NUM_MAX
#define KEY_TOUCH_NUM_MAX					6
#endif
#ifndef KEY_RDEC_NUM_MAX
#define KEY_RDEC_NUM_MAX                    3
#endif
#ifndef KEY_CTMU_TOUCH_NUM_MAX
#define KEY_CTMU_TOUCH_NUM_MAX				6
#endif
#ifndef MULT_KEY_ENABLE
#define MULT_KEY_ENABLE						DISABLE 		//是否使能组合按键消息, 使能后需要配置组合按键映射表
#endif

//*********************************************************************************//
//                                 iokey 配置                                      //
//*********************************************************************************//
#ifndef TCFG_IOKEY_ENABLE
#define TCFG_IOKEY_ENABLE					DISABLE_THIS_MOUDLE //是否使能IO按键
#define IOKEY_DEFAULT
#endif

#ifdef IOKEY_DEFAULT
#define TCFG_IOKEY_POWER_CONNECT_WAY		ONE_PORT_TO_LOW    //按键一端接低电平一端接IO
#define TCFG_IOKEY_POWER_ONE_PORT			IO_PORTB_01        //IO按键端口
#define TCFG_IOKEY_PREV_CONNECT_WAY			ONE_PORT_TO_LOW  //按键一端接低电平一端接IO
#define TCFG_IOKEY_PREV_ONE_PORT			IO_PORTB_00
#define TCFG_IOKEY_NEXT_CONNECT_WAY 		ONE_PORT_TO_LOW  //按键一端接低电平一端接IO
#define TCFG_IOKEY_NEXT_ONE_PORT			IO_PORTB_02
#endif

//*********************************************************************************//
//                                 adkey 配置                                      //
//*********************************************************************************//
#ifndef TCFG_ADKEY_ENABLE
#define TCFG_ADKEY_ENABLE                   DISABLE_THIS_MOUDLE//是否使能AD按键
#define ADKEY_DEFAULT
#endif

#ifndef TCFG_ADKEY_LED_IO_REUSE
#define TCFG_ADKEY_LED_IO_REUSE				DISABLE_THIS_MOUDLE	//ADKEY 和 LED IO复用，led只能设置蓝灯显示
#endif
#ifndef TCFG_ADKEY_IR_IO_REUSE
#define TCFG_ADKEY_IR_IO_REUSE				DISABLE_THIS_MOUDLE	//ADKEY 和 红外IO复用
#endif
#ifndef TCFG_ADKEY_LED_SPI_IO_REUSE
#define TCFG_ADKEY_LED_SPI_IO_REUSE			DISABLE_THIS_MOUDLE	//ADKEY 和 LED SPI IO复用
#endif

#ifdef ADKEY_DEFAULT

#define TCFG_ADKEY_PORT                     IO_PORTA_06         //AD按键端口(需要注意选择的IO口是否支持AD功能)
#define TCFG_ADKEY_AD_CHANNEL               AD_CH_PA6
#define TCFG_ADKEY_EXTERN_UP_ENABLE         ENABLE_THIS_MOUDLE //是否使用外部上拉
#define R_UP    220                 //22K，外部上拉阻值在此自行设置

//必须从小到大填电阻，没有则同VDDIO,填0x3ffL
#define TCFG_ADKEY_AD0      (0)                                 //0R
#define TCFG_ADKEY_AD1      (0x3ffL * 30   / (30   + R_UP))     //3k
#define TCFG_ADKEY_AD2      (0x3ffL * 62   / (62   + R_UP))     //6.2k
#define TCFG_ADKEY_AD3      (0x3ffL * 91   / (91   + R_UP))     //9.1k
#define TCFG_ADKEY_AD4      (0x3ffL * 150  / (150  + R_UP))     //15k
#define TCFG_ADKEY_AD5      (0x3ffL * 240  / (240  + R_UP))     //24k
#define TCFG_ADKEY_AD6      (0x3ffL * 330  / (330  + R_UP))     //33k
#define TCFG_ADKEY_AD7      (0x3ffL * 510  / (510  + R_UP))     //51k
#define TCFG_ADKEY_AD8      (0x3ffL * 1000 / (1000 + R_UP))     //100k
#define TCFG_ADKEY_AD9      (0x3ffL * 2200 / (2200 + R_UP))     //220k
#define TCFG_ADKEY_VDDIO    (0x3ffL)

#define TCFG_ADKEY_VOLTAGE0 ((TCFG_ADKEY_AD0 + TCFG_ADKEY_AD1) / 2)
#define TCFG_ADKEY_VOLTAGE1 ((TCFG_ADKEY_AD1 + TCFG_ADKEY_AD2) / 2)
#define TCFG_ADKEY_VOLTAGE2 ((TCFG_ADKEY_AD2 + TCFG_ADKEY_AD3) / 2)
#define TCFG_ADKEY_VOLTAGE3 ((TCFG_ADKEY_AD3 + TCFG_ADKEY_AD4) / 2)
#define TCFG_ADKEY_VOLTAGE4 ((TCFG_ADKEY_AD4 + TCFG_ADKEY_AD5) / 2)
#define TCFG_ADKEY_VOLTAGE5 ((TCFG_ADKEY_AD5 + TCFG_ADKEY_AD6) / 2)
#define TCFG_ADKEY_VOLTAGE6 ((TCFG_ADKEY_AD6 + TCFG_ADKEY_AD7) / 2)
#define TCFG_ADKEY_VOLTAGE7 ((TCFG_ADKEY_AD7 + TCFG_ADKEY_AD8) / 2)
#define TCFG_ADKEY_VOLTAGE8 ((TCFG_ADKEY_AD8 + TCFG_ADKEY_AD9) / 2)
#define TCFG_ADKEY_VOLTAGE9 ((TCFG_ADKEY_AD9 + TCFG_ADKEY_VDDIO) / 2)

#define TCFG_ADKEY_VALUE0                   0
#define TCFG_ADKEY_VALUE1                   1
#define TCFG_ADKEY_VALUE2                   2
#define TCFG_ADKEY_VALUE3                   3
#define TCFG_ADKEY_VALUE4                   4
#define TCFG_ADKEY_VALUE5                   5
#define TCFG_ADKEY_VALUE6                   6
#define TCFG_ADKEY_VALUE7                   7
#define TCFG_ADKEY_VALUE8                   8
#define TCFG_ADKEY_VALUE9                   9

#endif

//*********************************************************************************//
//                                 irkey 配置                                      //
//*********************************************************************************//
#ifndef TCFG_IRKEY_ENABLE
#define TCFG_IRKEY_ENABLE                   DISABLE_THIS_MOUDLE//是否使能ir按键
#endif
#ifndef TCFG_IRKEY_PORT
#define TCFG_IRKEY_PORT                         IO_PORTA_02        //IR按键端口
#endif


//*********************************************************************************//
//                             tocuh key 配置 (不支持)                                      //
//*********************************************************************************//
#ifndef TCFG_TOUCH_KEY_ENABLE
#define TCFG_TOUCH_KEY_ENABLE 				DISABLE_THIS_MOUDLE 		//是否使能触摸按键
#define TOUCHKEY_DEFAULT
#endif

#ifdef TOUCHKEY_DEFAULT
/* 触摸按键计数参考时钟选择, 频率越高, 精度越高
** 可选参数:
	1.TOUCH_KEY_OSC_CLK,
    2.TOUCH_KEY_MUX_IN_CLK,  //外部输入, ,一般不用, 保留
    3.TOUCH_KEY_PLL_192M_CLK,
    4.TOUCH_KEY_PLL_240M_CLK,
*/
#define TCFG_TOUCH_KEY_CLK 					TOUCH_KEY_PLL_192M_CLK 	//触摸按键时钟配置
#define TCFG_TOUCH_KEY_CHANGE_GAIN 			4 	//变化放大倍数, 一般固定
#define TCFG_TOUCH_KEY_PRESS_CFG 			-100//触摸按下灵敏度, 类型:s16, 数值越大, 灵敏度越高
#define TCFG_TOUCH_KEY_RELEASE_CFG0 		-50 //触摸释放灵敏度0, 类型:s16, 数值越大, 灵敏度越高
#define TCFG_TOUCH_KEY_RELEASE_CFG1 		-80 //触摸释放灵敏度1, 类型:s16, 数值越大, 灵敏度越高

//key0配置
#define TCFG_TOUCH_KEY0_PORT 				IO_PORTB_06  //触摸按键IO配置
#define TCFG_TOUCH_KEY0_VALUE 				1 		 	 //触摸按键key0 按键值

//key1配置
#define TCFG_TOUCH_KEY1_PORT 				IO_PORTB_07  //触摸按键key1 IO配置
#define TCFG_TOUCH_KEY1_VALUE 				2 		 	 //触摸按键key1按键值

#endif//!TCFG_TOUCH_KEY_ENABLE

//*********************************************************************************//
//                            ctmu tocuh key 配置 (不支持)                                     //
//*********************************************************************************//
#ifndef TCFG_CTMU_TOUCH_KEY_ENABLE
#define TCFG_CTMU_TOUCH_KEY_ENABLE              DISABLE_THIS_MOUDLE             //是否使能CTMU触摸按键
#define CTMUKEY_DEFAULT
#endif

#ifdef CTMUKEY_DEFAULT
//key0配置
#define TCFG_CTMU_TOUCH_KEY0_PORT 				IO_PORTB_06  //触摸按键key0 IO配置
#define TCFG_CTMU_TOUCH_KEY0_VALUE 				0 		 	 //触摸按键key0 按键值

//key1配置
#define TCFG_CTMU_TOUCH_KEY1_PORT 				IO_PORTB_07  //触摸按键key1 IO配置
#define TCFG_CTMU_TOUCH_KEY1_VALUE 				1 		 	 //触摸按键key1 按键值
#endif

//*********************************************************************************//
//                                 rdec_key 配置                                      //
//*********************************************************************************//
#ifndef TCFG_RDEC_KEY_ENABLE
#define TCFG_RDEC_KEY_ENABLE					DISABLE_THIS_MOUDLE //是否使能RDEC按键
#define RDECKEY_DEFAULT
#endif

#ifdef RDECKEY_DEFAULT
//RDEC0配置
#define TCFG_RDEC0_ECODE1_PORT					IO_PORTA_03
#define TCFG_RDEC0_ECODE2_PORT					IO_PORTA_04
#define TCFG_RDEC0_KEY0_VALUE 				 	0
#define TCFG_RDEC0_KEY1_VALUE 				 	1

//RDEC1配置
#define TCFG_RDEC1_ECODE1_PORT					IO_PORTB_02
#define TCFG_RDEC1_ECODE2_PORT					IO_PORTB_03
#define TCFG_RDEC1_KEY0_VALUE 				 	2
#define TCFG_RDEC1_KEY1_VALUE 				 	3

//RDEC2配置
#define TCFG_RDEC2_ECODE1_PORT					IO_PORTB_04
#define TCFG_RDEC2_ECODE2_PORT					IO_PORTB_05
#define TCFG_RDEC2_KEY0_VALUE 				 	4
#define TCFG_RDEC2_KEY1_VALUE 				 	5

#endif

//*********************************************************************************//
//                                  充电仓配置                                  //
//*********************************************************************************//
#ifndef TCFG_CHARGESTORE_ENABLE
#define TCFG_CHARGESTORE_ENABLE				DISABLE_THIS_MOUDLE       //是否支持智能充点仓
#endif
#ifndef TCFG_TEST_BOX_ENABLE
#define TCFG_TEST_BOX_ENABLE			    0
#endif
#ifndef TCFG_CHARGESTORE_PORT
#define TCFG_CHARGESTORE_PORT				IO_PORTA_02               //耳机和充点仓通讯的IO口
#endif
#ifndef TCFG_CHARGESTORE_UART_ID
#define TCFG_CHARGESTORE_UART_ID			IRQ_UART1_IDX             //通讯使用的串口号
#endif

#if (TCFG_TEST_BOX_ENABLE) && (AUDIO_PCM_DEBUG)
#undef 	TCFG_TEST_BOX_ENABLE
#define TCFG_TEST_BOX_ENABLE			    0		                 //因为使用PCM使用到了串口1
#endif

#ifndef TCFG_UMIDIGI_BOX_ENABLE
#define TCFG_UMIDIGI_BOX_ENABLE				0
#endif

//*********************************************************************************//
//                                  LED 配置                                       //
//******************************************************************************
#ifndef TCFG_PWMLED_ENABLE
#define TCFG_PWMLED_ENABLE					DISABLE_THIS_MOUDLE			//是否支持PMW LED推灯模块
#endif
#ifndef TCFG_PWMLED_IOMODE
#define TCFG_PWMLED_IOMODE					LED_ONE_IO_MODE				//LED模式，单IO还是两个IO推灯
#endif
#ifndef TCFG_PWMLED_PIN
#define TCFG_PWMLED_PIN						IO_PORTB_06					//LED使用的IO口 注意和led7是否有io冲突
#endif
#ifndef TCFG_IOLED_USE_MICIN_IO
#define TCFG_IOLED_USE_MICIN_IO				DISABLE						//使用mic脚去驱动LED，软件需要做特殊处理
#endif
#ifndef TCFG_MICIN_IO0
#define TCFG_MICIN_IO0						NO_CONFIG_PORT
#endif
#ifndef TCFG_MICIN_IO1
#define TCFG_MICIN_IO1						NO_CONFIG_PORT
#endif
//*********************************************************************************//
//                                  时钟配置                                       //
//*********************************************************************************//
#ifndef TCFG_CLOCK_SYS_SRC
#define TCFG_CLOCK_SYS_SRC					SYS_CLOCK_INPUT_PLL_BT_OSC   //系统时钟源选择
#endif
#ifndef TCFG_CLOCK_SYS_HZ
#define TCFG_CLOCK_SYS_HZ					24000000                     //系统时钟设置
#endif
#ifndef TCFG_CLOCK_OSC_HZ
#define TCFG_CLOCK_OSC_HZ					24000000                     //外界晶振频率设置
#endif
#ifndef TCFG_CLOCK_MODE
#define TCFG_CLOCK_MODE                     CLOCK_MODE_ADAPTIVE
#endif

//*********************************************************************************//
//                                  低功耗配置                                     //
//*********************************************************************************//
#ifndef TCFG_LOWPOWER_POWER_SEL
#define TCFG_LOWPOWER_POWER_SEL				PWR_LDO15                   //电源模式设置，可选DCDC和LDO
#endif
#ifndef TCFG_LOWPOWER_BTOSC_DISABLE
#define TCFG_LOWPOWER_BTOSC_DISABLE			0                           //低功耗模式下BTOSC是否保持
#endif
#ifndef TCFG_LOWPOWER_LOWPOWER_SEL
#define TCFG_LOWPOWER_LOWPOWER_SEL			0                          //SNIFF状态下芯片是否进入powerdown
#endif
#ifndef TCFG_LOWPOWER_VDDIOM_LEVEL
#define TCFG_LOWPOWER_VDDIOM_LEVEL			VDDIOM_VOL_32V             //VDDIO 设置的值要和vbat的压差要大于300mv左右，否则会出现DAC杂音
#endif
/*弱VDDIO等级配置，可选：
    VDDIOW_VOL_21V    VDDIOW_VOL_24V    VDDIOW_VOL_28V    VDDIOW_VOL_32V*/
#ifndef TCFG_LOWPOWER_VDDIOW_LEVEL
#define TCFG_LOWPOWER_VDDIOW_LEVEL			VDDIOW_VOL_28V            //弱VDDIO等级配置
#endif

#ifndef TCFG_POWER_MODE_QUIET_ENABLE
#define TCFG_POWER_MODE_QUIET_ENABLE		0
#endif

#ifndef TCFG_LOWPOWER_RAM_SIZE
#define TCFG_LOWPOWER_RAM_SIZE				0	// 低功耗掉电ram大小，单位：128K
#endif

//*********************************************************************************//
//                                  UI 配置                                        //
//*********************************************************************************//
#ifndef TCFG_UI_ENABLE
#define TCFG_UI_ENABLE 						    DISABLE_THIS_MOUDLE 	//UI总开关
#endif

#ifndef CONFIG_UI_STYLE
#define CONFIG_UI_STYLE                         STYLE_JL_LED7
#endif

//led屏
#ifndef TCFG_UI_LED1888_ENABLE
#define TCFG_UI_LED1888_ENABLE				    0//UI使用LED1888
#endif
#ifndef TCFG_UI_LED7_ENABLE
#define TCFG_UI_LED7_ENABLE 			 	    0//UI使用LED7显示
#endif
#ifndef TCFG_LED7_RUN_RAM
#define TCFG_LED7_RUN_RAM 					    DISABLE_THIS_MOUDLE 	    //led7跑ram 不屏蔽中断(需要占据2k附近ram)
#endif

//oled屏
#ifndef TCFG_LCD_OLED_ENABLE
#define TCFG_LCD_OLED_ENABLE                    0
#endif
#ifndef TCFG_COLORLED_ENABLE
#define TCFG_COLORLED_ENABLE                    0
#endif

//段码屏
#ifndef TCFG_UI_LCD_SEG3X9_ENABLE
#define TCFG_UI_LCD_SEG3X9_ENABLE 		        DISABLE_THIS_MOUDLE         //UI使用LCD段码屏显示
#endif

//LCD屏
#ifndef TCFG_LCD_ST7735S_ENABLE
#define TCFG_LCD_ST7735S_ENABLE	                0
#endif
#ifndef TCFG_LCD_ST7789VW_ENABLE
#define TCFG_LCD_ST7789VW_ENABLE	            0
#endif
#ifndef TCFG_SPI_LCD_ENABLE
#define TCFG_SPI_LCD_ENABLE                     0//spi lcd开关
#endif
#ifndef TCFG_LCD_RM69330_ENABLE
#define TCFG_LCD_RM69330_ENABLE                 0
#endif
#ifndef TCFG_LCD_RM69330_QSPI_ENABLE
#define TCFG_LCD_RM69330_QSPI_ENABLE            0
#endif
#ifndef TCFG_LCD_GC9A01_ENABLE
#define TCFG_LCD_GC9A01_ENABLE                  0
#endif
#ifndef TCFG_LCD_ST7789V_ENABLE
#define TCFG_LCD_ST7789V_ENABLE                 0
#endif
#ifndef TCFG_LCD_MCU_JD5858_ZHAOYU_ENABLE
#define TCFG_LCD_MCU_JD5858_ZHAOYU_ENABLE		0
#endif
#ifndef TCFG_SIMPLE_LCD_ENABLE
#define TCFG_SIMPLE_LCD_ENABLE                  0
#endif
#ifndef TCFG_LCD_QSPI_ST77903_ENABLE
#define TCFG_LCD_QSPI_ST77903_ENABLE			0
#endif
#ifndef TCFG_LCD_QSPI_ST77903_V1_ENABLE
#define TCFG_LCD_QSPI_ST77903_V1_ENABLE 		0
#endif
#ifndef TCFG_LCD_QSPI_ST77903_V2_ENABLE
#define TCFG_LCD_QSPI_ST77903_V2_ENABLE			0
#endif
#ifndef TCFG_LCD_QSPI_ST77903_V3_ENABLE
#define TCFG_LCD_QSPI_ST77903_V3_ENABLE			0
#endif
#ifndef TCFG_LCD_QSPI_ST77903_V4_ENABLE
#define TCFG_LCD_QSPI_ST77903_V4_ENABLE			0
#endif
#ifndef TCFG_LCD_SPI_ICNA3310B_ENABLE
#define TCFG_LCD_SPI_ICNA3310B_ENABLE			0
#endif
#ifndef TCFG_LCD_GC9307_240X283
#define TCFG_LCD_GC9307_240X283					0
#endif


#ifndef TCFG_UI_SHUT_DOWN_TIME
#define TCFG_UI_SHUT_DOWN_TIME                  0
#endif
#ifndef UI_USED_DOUBLE_BUFFER
#define UI_USED_DOUBLE_BUFFER                   0
#endif
#ifndef UI_UPGRADE_RES_ENABLE
#define UI_UPGRADE_RES_ENABLE                   0
#endif
#ifndef TCFG_TFT_LCD_DEV_SPI_HW_NUM
#define TCFG_TFT_LCD_DEV_SPI_HW_NUM			    1                       // 1: SPI1    2: SPI2 配置lcd选择的spi口
#endif

#ifndef TCFG_TP_MPU9250_ENABLE
#define TCFG_TP_MPU9250_ENABLE					DISABLE
#endif

#ifndef TCFG_TP_CST816D_ENABLE
#define TCFG_TP_CST816D_ENABLE					0
#endif

#ifndef TCFG_TP_SPT511X_ENABLE
#define TCFG_TP_SPT511X_ENABLE					0
#endif


#ifndef TCFG_MPU6887P_ENABLE
#define TCFG_MPU6887P_ENABLE					DISABLE
#endif

#ifndef TCFG_SH3001_ENABLE
#define TCFG_SH3001_ENABLE						DISABLE
#endif

#ifndef TCFG_IMUSENSOR_ENABLE
#define TCFG_IMUSENSOR_ENABLE					DISABLE
#endif


#ifndef TCFG_MSA310_EN
#define TCFG_MSA310_EN							DISABLE
#endif


#ifndef TCFG_LUA_ENABLE
#define TCFG_LUA_ENABLE							0
#endif

//*********************************************************************************//
//                                  EQ配置                                         //
//*********************************************************************************//
//EQ配置，使用在线EQ时，EQ文件和EQ模式无效。有EQ文件时，使能TCFG_USE_EQ_FILE,默认不用EQ模式切换功能
#ifndef TCFG_EQ_ENABLE
#define TCFG_EQ_ENABLE                      0     //支持EQ功能,EQ总使能
#endif
#ifndef TCFG_EQ_ONLINE_ENABLE
#define TCFG_EQ_ONLINE_ENABLE               0     //支持在线EQ调试
#endif
#ifndef TCFG_BT_MUSIC_EQ_ENABLE
#define TCFG_BT_MUSIC_EQ_ENABLE             0      //支持蓝牙音乐EQ
#endif
#ifndef TCFG_PHONE_EQ_ENABLE
#define TCFG_PHONE_EQ_ENABLE                0      //支持通话近端EQ
#endif
#ifndef TCFG_MUSIC_MODE_EQ_ENABLE
#define TCFG_MUSIC_MODE_EQ_ENABLE           0     //支持音乐模式EQ
#endif
#ifndef TCFG_LINEIN_MODE_EQ_ENABLE
#define TCFG_LINEIN_MODE_EQ_ENABLE          0     //支持linein近端EQ
#endif
#ifndef TTCFG_FM_MODE_EQ_ENABLE
#define TCFG_FM_MODE_EQ_ENABLE              0     //支持fm模式EQ
#endif
#ifndef TCFG_SPDIF_MODE_EQ_ENABLE
#define TCFG_SPDIF_MODE_EQ_ENABLE           0     //支持SPDIF模式EQ
#endif
#ifndef TCFG_PC_MODE_EQ_ENABLE
#define TCFG_PC_MODE_EQ_ENABLE              0     //支持pc模式EQ
#endif
#ifndef TCFG_AUDIO_OUT_EQ_ENABLE
#define TCFG_AUDIO_OUT_EQ_ENABLE			0 	  //mix_out后高低音EQ
#endif

#ifndef TCFG_USE_EQ_FILE
#define TCFG_USE_EQ_FILE                    0    //离线eq使用配置文件还是默认系数表 1：使用文件  0 使用默认系数表
#endif
#ifndef TCFG_USER_EQ_MODE_NUM
#define TCFG_USER_EQ_MODE_NUM               7    //eq默认系数表的模式个数，默认是7个
#endif

#ifndef EQ_SECTION_MAX
#define EQ_SECTION_MAX                      10    //eq段数
#endif

#ifndef TCFG_DRC_ENABLE
#define TCFG_DRC_ENABLE						0 	  //DRC 总使能
#endif
#ifndef TCFG_BT_MUSIC_DRC_ENABLE
#define TCFG_BT_MUSIC_DRC_ENABLE            0     //支持蓝牙音乐DRC
#endif
#ifndef TCFG_MUSIC_MODE_DRC_ENABLE
#define TCFG_MUSIC_MODE_DRC_ENABLE          0     //支持音乐模式DRC
#endif
#ifndef TCFG_LINEIN_MODE_DRC_ENABLE
#define TCFG_LINEIN_MODE_DRC_ENABLE         0     //支持LINEIN模式DRC
#endif
#ifndef TCFG_FM_MODE_DRC_ENABLE
#define TCFG_FM_MODE_DRC_ENABLE             0     //支持FM模式DRC
#endif
#ifndef TCFG_SPDIF_MODE_DRC_ENABLE
#define TCFG_SPDIF_MODE_DRC_ENABLE          0     //支持SPDIF模式DRC
#endif
#ifndef TCFG_PC_MODE_DRC_ENABLE
#define TCFG_PC_MODE_DRC_ENABLE             0     //支持PC模式DRC
#endif
#ifndef TCFG_AUDIO_OUT_DRC_ENABLE
#define TCFG_AUDIO_OUT_DRC_ENABLE			0 	  //mix_out后drc
#endif

// ONLINE CCONFIG
// 如果调试串口是DP DM,使用eq调试串口时，需关闭usb宏
#ifndef TCFG_ONLINE_ENABLE
#define TCFG_ONLINE_ENABLE                  (TCFG_EQ_ONLINE_ENABLE)    //是否支持EQ在线调试功能
#endif
#ifndef TCFG_ONLINE_TX_PORT
#define TCFG_ONLINE_TX_PORT					IO_PORT_DP                 //EQ调试TX口选择
#endif
#ifndef TCFG_ONLINE_RX_PORT
#define TCFG_ONLINE_RX_PORT					IO_PORT_DM                 //EQ调试RX口选择
#endif

#ifndef LINEIN_MODE_SOLE_EQ_EN
#define LINEIN_MODE_SOLE_EQ_EN              0  //linein模式是否需要独立的音效
#endif

#ifndef TCFG_DYNAMIC_EQ_ENABLE
#define TCFG_DYNAMIC_EQ_ENABLE				0
#endif

//*********************************************************************************//
//                                  混响 配置                                     //
//*********************************************************************************//
#ifndef TCFG_MIC_EFFECT_ENABLE
#define TCFG_MIC_EFFECT_ENABLE              DISABLE
#endif
#ifndef TCFG_MIC_EFFECT_DEBUG
#define TCFG_MIC_EFFECT_DEBUG               0//调试打印
#endif
#ifndef TCFG_MIC_EFFECT_ONLINE_ENABLE
#define TCFG_MIC_EFFECT_ONLINE_ENABLE       0//混响音效在线调试使能
#endif
#ifndef TCFG_MIC_EFFECT_SEL
#define TCFG_MIC_EFFECT_SEL                 MIC_EFFECT_ECHO
#endif
#ifndef MIC_EFFECT_EQ_EN
#define  MIC_EFFECT_EQ_EN                   0//混响音效的EQ
#endif
#ifndef TCFG_REVERB_SAMPLERATE_DEFUAL
#define TCFG_REVERB_SAMPLERATE_DEFUAL       (44100L)
#endif
#ifndef MIC_EFFECT_SAMPLERATE
#define MIC_EFFECT_SAMPLERATE			    (16000L)
#endif

/*********扩音器功能使用mic_effect.c混响流程，功能选配在effect_reg.c中 ***********/
/*********配置MIC_EFFECT_CONFIG宏定义即可********************************/
#ifndef TCFG_LOUDSPEAKER_ENABLE
#define TCFG_LOUDSPEAKER_ENABLE            DISABLE //扩音器功能使能
#endif

//*********************************************************************************//
//                                  g-sensor配置                                   //
//*********************************************************************************//
#ifndef TCFG_GSENSOR_ENABLE
#define TCFG_GSENSOR_ENABLE                       0     //gSensor使能
#endif
#ifndef TCFG_DA230_EN
#define TCFG_DA230_EN                             0
#endif
#ifndef TCFG_SC7A20_EN
#define TCFG_SC7A20_EN                            0
#endif
#ifndef TCFG_STK8321_EN
#define TCFG_STK8321_EN                           0
#endif
#ifndef TCFG_GSENOR_USER_IIC_TYPE
#define TCFG_GSENOR_USER_IIC_TYPE                 0     //0:软件IIC  1:硬件IIC
#endif

//*********************************************************************************//
//                                  系统配置                                         //
//*********************************************************************************//
#ifndef TCFG_AUTO_SHUT_DOWN_TIME
#define TCFG_AUTO_SHUT_DOWN_TIME		          0       //没有蓝牙连接自动关机时间
#endif
#ifndef TCFG_SYS_LVD_EN
#define TCFG_SYS_LVD_EN						      1       //电量检测使能
#endif
#ifndef TCFG_POWER_ON_NEED_KEY
#define TCFG_POWER_ON_NEED_KEY				      0	    //是否需要按按键开机配置
#endif
#ifndef TWFG_APP_POWERON_IGNORE_DEV
#define TWFG_APP_POWERON_IGNORE_DEV               4000    //上电忽略挂载设备，0时不忽略，非0则n毫秒忽略
#endif

//*********************************************************************************//
//                                  蓝牙配置                                       //
//*********************************************************************************//
#ifndef TCFG_USER_TWS_ENABLE
#define TCFG_USER_TWS_ENABLE                0   //tws功能使能
#endif
#ifndef TCFG_USER_BLE_ENABLE
#define TCFG_USER_BLE_ENABLE                0   //BLE功能使能
#endif
#ifndef TCFG_USER_BT_CLASSIC_ENABLE
#define TCFG_USER_BT_CLASSIC_ENABLE         0   //经典蓝牙功能使能
#endif
#ifndef TCFG_BT_SUPPORT_AAC
#define TCFG_BT_SUPPORT_AAC                 0   //AAC格式支持
#endif
#ifndef TCFG_USER_EMITTER_ENABLE
#define TCFG_USER_EMITTER_ENABLE            0   //emitter功能使能
#endif
#ifndef TCFG_BT_SNIFF_ENABLE
#define TCFG_BT_SNIFF_ENABLE                0   //bt sniff 功能使能
#endif
#ifndef TCFG_USER_BLE_CTRL_BREDR_EN
#define TCFG_USER_BLE_CTRL_BREDR_EN         0
#endif

#ifndef TCFG_BLE_ADV_DYNAMIC_SWITCH
#define TCFG_BLE_ADV_DYNAMIC_SWITCH			0
#endif

#ifndef USER_SUPPORT_PROFILE_SPP
#define USER_SUPPORT_PROFILE_SPP            0
#endif
#ifndef USER_SUPPORT_PROFILE_HFP
#define USER_SUPPORT_PROFILE_HFP            0
#endif
#ifndef USER_SUPPORT_PROFILE_A2DP
#define USER_SUPPORT_PROFILE_A2DP           0
#endif
#ifndef USER_SUPPORT_PROFILE_AVCTP
#define USER_SUPPORT_PROFILE_AVCTP          0
#endif
#ifndef USER_SUPPORT_PROFILE_HID
#define USER_SUPPORT_PROFILE_HID            0
#endif
#ifndef USER_SUPPORT_PROFILE_PNP
#define USER_SUPPORT_PROFILE_PNP            0
#endif
#ifndef USER_SUPPORT_PROFILE_PBAP
#define USER_SUPPORT_PROFILE_PBAP           0
#endif
#ifndef USER_SUPPORT_PROFILE_HFP_AG
#define USER_SUPPORT_PROFILE_HFP_AG         0
#endif

//BLE多连接,多开注意RAM的使用
#ifndef TRANS_MULTI_BLE_EN
#define TRANS_MULTI_BLE_EN                  0 //蓝牙BLE多连:1主1从,或者2主
#endif
#ifndef TRANS_MULTI_BLE_SLAVE_NUMS
#define TRANS_MULTI_BLE_SLAVE_NUMS          0 //range(0~1)
#endif
#ifndef TRANS_MULTI_BLE_MASTER_NUMS
#define TRANS_MULTI_BLE_MASTER_NUMS         0 //range(0~2)
#endif

#ifndef TCFG_BD_NUM
#define TCFG_BD_NUM						    1   //连接设备个数配置
#endif
#ifndef TCFG_AUTO_STOP_PAGE_SCAN_TIME
#define TCFG_AUTO_STOP_PAGE_SCAN_TIME       0   //配置一拖二第一台连接后自动关闭PAGE SCAN的时间(单位分钟)
#endif
#ifndef TCFG_USER_ESCO_SLAVE_MUTE
#define TCFG_USER_ESCO_SLAVE_MUTE           0   //对箱通话slave出声音
#endif

#ifndef BT_INBAND_RINGTONE
#define BT_INBAND_RINGTONE                  0   //是否播放手机自带来电铃声
#endif
#ifndef BT_PHONE_NUMBER
#define BT_PHONE_NUMBER                     1   //是否播放来电报号
#endif
#ifndef BT_SYNC_PHONE_RING
#define BT_SYNC_PHONE_RING                  0   //是否TWS同步播放来电铃声
#endif
#ifndef BT_SUPPORT_DISPLAY_BAT
#define BT_SUPPORT_DISPLAY_BAT              0   //是否使能电量检测
#endif
#ifndef BT_SUPPORT_MUSIC_VOL_SYNC
#define BT_SUPPORT_MUSIC_VOL_SYNC           0   //是否使能音量同步
#endif

#ifndef TCFG_BLUETOOTH_BACK_MODE
#define TCFG_BLUETOOTH_BACK_MODE			0	//不支持后台模式
#endif

#ifndef TCFG_DEC2TWS_ENABLE
#define TCFG_DEC2TWS_ENABLE					0
#endif
#ifndef TCFG_PCM_ENC2TWS_ENABLE
#define TCFG_PCM_ENC2TWS_ENABLE				0
#endif
#ifndef TCFG_TONE2TWS_ENABLE
#define TCFG_TONE2TWS_ENABLE				0
#endif

#ifndef TCFG_BT_RAM_CODE_ENABLE
#define TCFG_BT_RAM_CODE_ENABLE				0
#endif

//*********************************************************************************//
//                                  REC 配置                                       //
//*********************************************************************************//
#ifndef RECORDER_MIX_EN
#define RECORDER_MIX_EN						DISABLE  //混合录音使能
#endif
#ifndef TCFG_RECORD_FOLDER_DEV_ENABLE
#define TCFG_RECORD_FOLDER_DEV_ENABLE       DISABLE  //音乐播放录音区分使能
#endif

//*********************************************************************************//
//                                  linein配置                                     //
//*********************************************************************************//
#ifndef TCFG_LINEIN_ENABLE
#define TCFG_LINEIN_ENABLE					0                   // linein使能
#endif
#ifndef TCFG_LINEIN_LR_CH
#define TCFG_LINEIN_LR_CH					AUDIO_LIN0L_CH      // AUDIO_LIN0_LR
#endif
#ifndef TCFG_LINEIN_CHECK_PORT
#define TCFG_LINEIN_CHECK_PORT				NO_CONFIG_PORT      // linein检测IO
#endif
#ifndef TCFG_LINEIN_PORT_UP_ENABLE
#define TCFG_LINEIN_PORT_UP_ENABLE        	1					// 检测IO上拉使能
#endif
#ifndef TCFG_LINEIN_PORT_DOWN_ENABLE
#define TCFG_LINEIN_PORT_DOWN_ENABLE       	0					// 检测IO下拉使能
#endif
#ifndef TCFG_LINEIN_AD_CHANNEL
#define TCFG_LINEIN_AD_CHANNEL             	NO_CONFIG_PORT		// 检测IO是否使用AD检测
#endif
#ifndef TCFG_LINEIN_VOLTAGE
#define TCFG_LINEIN_VOLTAGE                	0					// AD检测时的阀值
#endif
#ifndef TCFG_LINEIN_INPUT_WAY
#define TCFG_LINEIN_INPUT_WAY               LINEIN_INPUT_WAY
#endif
#ifndef TCFG_LINEIN_MULTIPLEX_WITH_FM
#define TCFG_LINEIN_MULTIPLEX_WITH_FM		DISABLE 			// linein 脚与 FM 脚复用
#endif
#ifndef TCFG_LINEIN_MULTIPLEX_WITH_SD
#define TCFG_LINEIN_MULTIPLEX_WITH_SD		DISABLE 			// linein 检测与 SD cmd 复用
#endif
#ifndef TCFG_LINEIN_SD_PORT
#define TCFG_LINEIN_SD_PORT		            0                   // 0:sd0 1:sd1     //选择复用的sd
#endif

//*********************************************************************************//
//                                  music 配置                                     //
//*********************************************************************************//
#ifndef TCFG_DEC_G729_ENABLE
#define TCFG_DEC_G729_ENABLE                DISABLE
#endif
#ifndef TCFG_DEC_MP3_ENABLE
#define TCFG_DEC_MP3_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_WMA_ENABLE
#define TCFG_DEC_WMA_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_WAV_ENABLE
#define TCFG_DEC_WAV_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_FLAC_ENABLE
#define TCFG_DEC_FLAC_ENABLE				DISABLE
#endif
#ifndef TCFG_DEC_APE_ENABLE
#define TCFG_DEC_APE_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_M4A_ENABLE
#define TCFG_DEC_M4A_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_ALAC_ENABLE
#define TCFG_DEC_ALAC_ENABLE				DISABLE
#endif
#ifndef TCFG_DEC_AMR_ENABLE
#define TCFG_DEC_AMR_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_DTS_ENABLE
#define TCFG_DEC_DTS_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_MIDI_ENABLE
#define TCFG_DEC_MIDI_ENABLE                DISABLE
#endif
#ifndef TCFG_DEC_G726_ENABLE
#define TCFG_DEC_G726_ENABLE                DISABLE
#endif
#ifndef TCFG_DEC_MTY_ENABLE
#define TCFG_DEC_MTY_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_WTGV2_ENABLE
#define TCFG_DEC_WTGV2_ENABLE				DISABLE
#endif

#ifndef TCFG_DEC_SBC_ENABLE
#define TCFG_DEC_SBC_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_PCM_ENABLE
#define TCFG_DEC_PCM_ENABLE					DISABLE
#endif
#ifndef TCFG_DEC_CVSD_ENABLE
#define TCFG_DEC_CVSD_ENABLE				DISABLE
#endif

#ifndef TCFG_DEC_LC3_ENABLE
#define TCFG_DEC_LC3_ENABLE					DISABLE
#endif

#ifndef TCFG_DEC_ID3_V1_ENABLE
#define TCFG_DEC_ID3_V1_ENABLE				DISABLE
#endif
#ifndef TCFG_DEC_ID3_V2_ENABLE
#define TCFG_DEC_ID3_V2_ENABLE				DISABLE
#endif
#ifndef TCFG_DEC_DECRYPT_ENABLE
#define TCFG_DEC_DECRYPT_ENABLE				DISABLE
#endif
#ifndef TCFG_DEC_DECRYPT_KEY
#define TCFG_DEC_DECRYPT_KEY				(0x12345678)
#endif

////<变速变调
#ifndef TCFG_SPEED_PITCH_ENABLE
#define TCFG_SPEED_PITCH_ENABLE             DISABLE//
#endif

//*********************************************************************************//
//                                  fm 配置                                     //
//*********************************************************************************//
#ifndef TCFG_FM_ENABLE
#define TCFG_FM_ENABLE							0       // fm 使能
#endif
#ifndef TCFG_FM_INSIDE_ENABLE
#define TCFG_FM_INSIDE_ENABLE				    0
#endif
#ifndef TCFG_FM_RDA5807_ENABLE
#define TCFG_FM_RDA5807_ENABLE					DISABLE
#endif
#ifndef TCFG_FM_BK1080_ENABLE
#define TCFG_FM_BK1080_ENABLE					DISABLE
#endif
#ifndef TCFG_FM_QN8035_ENABLE
#define TCFG_FM_QN8035_ENABLE					DISABLE
#endif

#ifndef TCFG_FMIN_LADC_IDX
#define TCFG_FMIN_LADC_IDX				        1				// linein使用的ladc通道，对应ladc_list
#endif
#ifndef TCFG_FMIN_LR_CH
#define TCFG_FMIN_LR_CH					        AUDIO_LIN1_LR
#endif
#ifndef TCFG_FM_INPUT_WAY
#define TCFG_FM_INPUT_WAY                       LINEIN_INPUT_WAY_ANALOG
#endif

#ifndef TCFG_CODE_RUN_RAM_FM_MODE
#define TCFG_CODE_RUN_RAM_FM_MODE 			    DISABLE_THIS_MOUDLE 	//FM模式 代码跑ram
#endif

//*********************************************************************************//
//                                  fm emitter 配置 (不支持)                                    //
//*********************************************************************************//
#ifndef TCFG_APP_FM_EMITTER_EN
#define TCFG_APP_FM_EMITTER_EN                  DISABLE_THIS_MOUDLE
#endif
#ifndef TCFG_FM_EMITTER_INSIDE_ENABLE
#define TCFG_FM_EMITTER_INSIDE_ENABLE			DISABLE
#endif
#ifndef TCFG_FM_EMITTER_AC3433_ENABLE
#define TCFG_FM_EMITTER_AC3433_ENABLE			DISABLE
#endif
#ifndef TCFG_FM_EMITTER_QN8007_ENABLE
#define TCFG_FM_EMITTER_QN8007_ENABLE			DISABLE
#endif
#ifndef TCFG_FM_EMITTER_QN8027_ENABLE
#define TCFG_FM_EMITTER_QN8027_ENABLE			DISABLE
#endif

//*********************************************************************************//
//                                  rtc 配置(不支持)                               //
//*********************************************************************************//
#ifndef TCFG_RTC_ENABLE
#define TCFG_RTC_ENABLE						    TCFG_APP_RTC_EN
#endif
#ifndef TCFG_USE_VIRTUAL_RTC
#define TCFG_USE_VIRTUAL_RTC                    DISABLE
#endif

//*********************************************************************************//
//                                  SPDIF & ARC 配置(不支持)                                     //
//*********************************************************************************//
#ifndef TCFG_SPDIF_ENABLE
#define TCFG_SPDIF_ENABLE                       TCFG_APP_SPDIF_EN
#endif
#ifndef TCFG_SPDIF_OUTPUT_ENABLE
#define TCFG_SPDIF_OUTPUT_ENABLE                ENABLE
#endif
#ifndef TCFG_HDMI_ARC_ENABLE
#define TCFG_HDMI_ARC_ENABLE                    ENABLE
#endif
#ifndef TCFG_HDMI_CEC_PORT
#define TCFG_HDMI_CEC_PORT                      IO_PORTA_02
#endif

//*********************************************************************************//
//                                  IIS 配置                                     //
//*********************************************************************************//
#ifndef TCFG_IIS_ENABLE
#define TCFG_IIS_ENABLE                       DISABLE_THIS_MOUDLE
#endif
#ifndef TCFG_IIS_MODE
#define TCFG_IIS_MODE                         (0)   //  0:master  1:slave
#endif

#ifndef TCFG_IIS_OUTPUT_EN
#define TCFG_IIS_OUTPUT_EN                    (ENABLE && TCFG_IIS_ENABLE)
#endif
#ifndef TCFG_IIS_OUTPUT_PORT
#define TCFG_IIS_OUTPUT_PORT                  ALINK0_PORTA
#endif
#ifndef TCFG_IIS_OUTPUT_CH_NUM
#define TCFG_IIS_OUTPUT_CH_NUM                1 //0:mono,1:stereo
#endif
#ifndef TCFG_IIS_OUTPUT_SR
#define TCFG_IIS_OUTPUT_SR                    44100
#endif
#ifndef TCFG_IIS_OUTPUT_DATAPORT_SEL
#define TCFG_IIS_OUTPUT_DATAPORT_SEL          (BIT(0)|BIT(1))
#endif

#ifndef TCFG_IIS_INPUT_EN
#define TCFG_IIS_INPUT_EN                    (DISABLE && TCFG_IIS_ENABLE)
#endif
#ifndef TCFG_IIS_INPUT_PORT
#define TCFG_IIS_INPUT_PORT                  ALINK0_PORTA
#endif
#ifndef TCFG_IIS_INPUT_CH_NUM
#define TCFG_IIS_INPUT_CH_NUM                1 //0:mono,1:stereo
#endif
#ifndef TCFG_IIS_INPUT_SR
#define TCFG_IIS_INPUT_SR                    44100
#endif
#ifndef TCFG_IIS_INPUT_DATAPORT_SEL
#define TCFG_IIS_INPUT_DATAPORT_SEL          (BIT(0))
#endif

//*********************************************************************************//
//                                  fat 文件系统配置                                       //
//*********************************************************************************//
#ifndef CONFIG_FATFS_ENABLE
#define CONFIG_FATFS_ENABLE					  0
#endif

#ifndef TCFG_LFN_EN
#define TCFG_LFN_EN                         0
#endif

#ifndef TCFG_FS_RAM_CODE_ENABLE
#define TCFG_FS_RAM_CODE_ENABLE				0
#endif

//*********************************************************************************//
//                                  REC 配置                                       //
//*********************************************************************************//
#ifndef TCFG_LINEIN_REC_EN
#define TCFG_LINEIN_REC_EN					DISABLE
#endif
#ifndef TCFG_MIXERCH_REC_EN
#define	TCFG_MIXERCH_REC_EN				    DISABLE
#endif
#ifndef TCFG_MIC_REC_PITCH_EN
#define TCFG_MIC_REC_PITCH_EN               DISABLE
#endif
#ifndef TCFG_MIC_REC_REVERB_EN
#define TCFG_MIC_REC_REVERB_EN              DISABLE
#endif


//*********************************************************************************//
//                                  encoder 配置                                   //
//*********************************************************************************//
#ifndef TCFG_ENC_CVSD_ENABLE
#define TCFG_ENC_CVSD_ENABLE                ENABLE
#endif
#ifndef TCFG_ENC_MSBC_ENABLE
#define TCFG_ENC_MSBC_ENABLE                ENABLE
#endif
#ifndef TCFG_ENC_G726_ENABLE
#define TCFG_ENC_G726_ENABLE                DISABLE
#endif
#ifndef TCFG_ENC_MP3_ENABLE
#define TCFG_ENC_MP3_ENABLE                 DISABLE
#endif
#ifndef TCFG_ENC_ADPCM_ENABLE
#define TCFG_ENC_ADPCM_ENABLE               DISABLE
#endif
#ifndef TCFG_ENC_SBC_ENABLE
#define TCFG_ENC_SBC_ENABLE                 ENABLE
#endif
#ifndef TCFG_ENC_OPUS_ENABLE
#define TCFG_ENC_OPUS_ENABLE                DISABLE
#endif
#ifndef TCFG_ENC_SPEEX_ENABLE
#define TCFG_ENC_SPEEX_ENABLE               DISABLE
#endif
#ifndef TCFG_ENC_LC3_ENABLE
#define TCFG_ENC_LC3_ENABLE					DISABLE
#endif

//*********************************************************************************//
//                                 电源切换配置                                    //
//*********************************************************************************//
#ifndef CONFIG_PHONE_CALL_USE_LDO15
#define CONFIG_PHONE_CALL_USE_LDO15	        0
#endif

//*********************************************************************************//
//                                人声消除使能
//*********************************************************************************//
#ifndef AUDIO_VOCAL_REMOVE_EN
#define AUDIO_VOCAL_REMOVE_EN               0
#endif

///*********************************************************************************//
//          等响度 开启后，需要固定模拟音量,调节软件数字音量
//          等响度使用eq实现，同个数据流中，若打开等响度，请开eq总使能，关闭其他eq,例如蓝牙模式eq
//*********************************************************************************//
#ifndef AUDIO_EQUALLOUDNESS_CONFIG
#define AUDIO_EQUALLOUDNESS_CONFIG         0  //等响度
#endif
#ifndef AUDIO_VBASS_CONFIG
#define AUDIO_VBASS_CONFIG                 0  //虚拟低音
#endif
#ifndef AUDIO_SURROUND_CONFIG
#define AUDIO_SURROUND_CONFIG              0  //环绕音效
#endif


//*********************************************************************************//
//                               解码独立任务设置，需要消耗额外的ram，慎用！
//*********************************************************************************//
#ifndef TCFG_AUDIO_DEC_OUT_TASK
#define TCFG_AUDIO_DEC_OUT_TASK		      0	// 解码使用单独任务做输出
#endif

//*********************************************************************************//
//                                 测试模式配置                                    //
//*********************************************************************************//
#if (CONFIG_BT_MODE != BT_NORMAL)

#undef  TCFG_BD_NUM
#define TCFG_BD_NUM						            1

#undef  TCFG_USER_TWS_ENABLE
#define TCFG_USER_TWS_ENABLE                        0     //tws功能使能

#undef  TCFG_USER_BLE_ENABLE
#define TCFG_USER_BLE_ENABLE                        1     //BLE功能使能

#undef  TCFG_AUTO_SHUT_DOWN_TIME
#define TCFG_AUTO_SHUT_DOWN_TIME		            0

#undef  TCFG_SYS_LVD_EN
#define TCFG_SYS_LVD_EN						        0

#undef  TCFG_LOWPOWER_LOWPOWER_SEL
#define TCFG_LOWPOWER_LOWPOWER_SEL                  0

#undef TCFG_AUDIO_DAC_LDO_VOLT
#define TCFG_AUDIO_DAC_LDO_VOLT			            DUT_AUDIO_DAC_LDO_VOLT

#undef TCFG_LOWPOWER_POWER_SEL
#define TCFG_LOWPOWER_POWER_SEL				        PWR_LDO15

#undef  TCFG_PWMLED_ENABLE
#define TCFG_PWMLED_ENABLE					        DISABLE_THIS_MOUDLE

#undef  TCFG_ADKEY_ENABLE
#define TCFG_ADKEY_ENABLE                           DISABLE_THIS_MOUDLE

#undef  TCFG_IOKEY_ENABLE
#define TCFG_IOKEY_ENABLE					        DISABLE_THIS_MOUDLE

#undef TCFG_TEST_BOX_ENABLE
#define TCFG_TEST_BOX_ENABLE			            0

#undef TCFG_AUTO_SHUT_DOWN_TIME
#define TCFG_AUTO_SHUT_DOWN_TIME		            0

#undef TCFG_POWER_ON_NEED_KEY
#define TCFG_POWER_ON_NEED_KEY				        0

#undef TCFG_SD0_ENABLE
#define TCFG_SD0_ENABLE				                0

#undef TCFG_SD1_ENABLE
#define TCFG_SD1_ENABLE				                0

#undef TCFG_APP_PC_EN
#define TCFG_APP_PC_EN					            0

#undef TCFG_PC_ENABLE
#define TCFG_PC_ENABLE                              0

#undef TCFG_UDISK_ENABLE
#define TCFG_UDISK_ENABLE				            0

////bqb 如果测试3M tx buf 最好加大一点
#undef  CONFIG_BT_TX_BUFF_SIZE
#define CONFIG_BT_TX_BUFF_SIZE                      (6 * 1024)

#endif //(CONFIG_BT_MODE != BT_NORMAL)



//*********************************************************************************//
//                                  无线麦配置                                     //
//*********************************************************************************//
#ifndef BLE_WIRELESS_MIC_SERVER_EN
#define BLE_WIRELESS_MIC_SERVER_EN           		0
#endif

#ifndef BLE_WIRELESS_MIC_CLIENT_EN
#define BLE_WIRELESS_MIC_CLIENT_EN           		0
#endif

#ifndef WIRELESS_MIC_CODING_SAMPLERATE
#define WIRELESS_MIC_CODING_SAMPLERATE				(44100)
#endif

#ifndef WIRELESS_MIC_DECODE_SAMPLERATE
#define WIRELESS_MIC_DECODE_SAMPLERATE				(44100)
#endif

#ifndef WIRELESS_MIC_CODING_BIT_RATE
#define WIRELESS_MIC_CODING_BIT_RATE				(64000)
#endif

#ifndef WIRELESS_MIC_CODING_FRAME_LEN
#define WIRELESS_MIC_CODING_FRAME_LEN				(25)
#endif

#ifndef WIRELESS_MIC_CODING_CHANNEL_NUM
#define WIRELESS_MIC_CODING_CHANNEL_NUM				1
#endif

#ifndef WIRELESS_MIC_DECODE_CHANNEL_NUM
#define WIRELESS_MIC_DECODE_CHANNEL_NUM				1
#endif

#ifndef WIRELESS_MIC_ADC_GAIN
#define WIRELESS_MIC_ADC_GAIN						(10)
#endif

#ifndef WIRELESS_MIC_ADC_POINT_UNIT
#define WIRELESS_MIC_ADC_POINT_UNIT					(120)
#endif

#ifndef WIRELESS_MIC_DENOISE_ENABLE
#define WIRELESS_MIC_DENOISE_ENABLE					DISABLE
#endif

#ifndef WIRELESS_MIC_SLAVE_NOISEGATE_EN
#define WIRELESS_MIC_SLAVE_NOISEGATE_EN				DISABLE
#endif

#ifndef WIRELESS_MIC_ROLE_SEL
#define WIRELESS_MIC_ROLE_SEL						APP_WIRELESS_MIC_SLAVE
#endif

#ifndef WIRELESS_MIC_SLAVE_OUTPUT_SEL
#define WIRELESS_MIC_SLAVE_OUTPUT_SEL				WIRELESS_MIC_SLAVE_OUTPUT_DAC
#endif

#ifndef WIRELESS_MIC_24G_ENABLE
#define WIRELESS_MIC_24G_ENABLE						DISABLE
#endif

#ifndef WIRELESS_MIC_SLAVE_OUTPUT_USB_DAC
#define WIRELESS_MIC_SLAVE_OUTPUT_USB_DAC			DISABLE
#endif

#ifndef WIRELESS_MIC_ECHO_ENABLE
#define WIRELESS_MIC_ECHO_ENABLE					DISABLE
#endif

#ifndef WIRELESS_MIC_DENOISE_EMITTER
#define WIRELESS_MIC_DENOISE_EMITTER					DISABLE
#endif

#ifndef WIRELESS_MIC_ECHO_EMITTER
#define WIRELESS_MIC_ECHO_EMITTER					DISABLE
#endif

#ifndef WIRELESS_MIC_PAIR_BONDING
#define WIRELESS_MIC_PAIR_BONDING					DISABLE
#endif


#ifndef TCFG_WIFI_DETCET_ENABLE
#define TCFG_WIFI_DETCET_ENABLE					DISABLE
#endif

#ifndef TCFG_TP_CST816S_ENABLE
#define TCFG_TP_CST816S_ENABLE					DISABLE
#endif

#ifndef TCFG_PWMLED_USE_SLOT_TIME
#define TCFG_PWMLED_USE_SLOT_TIME				DISABLE
#endif

#ifndef SOUND_TRACK_2_P_X_CH_CONFIG
#define SOUND_TRACK_2_P_X_CH_CONFIG 0
#endif
#ifndef TCFG_EQ_DIVIDE_ENABLE
#define TCFG_EQ_DIVIDE_ENABLE 0
#endif
#ifndef AEC_PITCHSHIFTER_CONFIG
#define AEC_PITCHSHIFTER_CONFIG 0
#endif
#ifndef TCFG_PHONE_DRC_ENABLE
#define TCFG_PHONE_DRC_ENABLE 0
#endif

//*********************************************************************************//
//                                  pay 配置                                       //
//*********************************************************************************//
#ifndef TCFG_PAY_ALIOS_ENABLE
#define TCFG_PAY_ALIOS_ENABLE				0
#endif

#ifndef TCFG_PAY_ALIOS_WAY_ALIYUN
#define TCFG_PAY_ALIOS_WAY_ALIYUN			1
#endif

#ifndef TCFG_PAY_ALIOS_WAY_T_HEAD
#define TCFG_PAY_ALIOS_WAY_T_HEAD			2
#endif

#ifndef TCFG_PAY_ALIOS_WAY_SEL
#define TCFG_PAY_ALIOS_WAY_SEL				0
#endif

#ifndef TCFG_PAY_ALIOS_PRODUCT_MODEL
#define TCFG_PAY_ALIOS_PRODUCT_MODEL		" "
#endif

//*********************************************************************************//
//                                  产测配置                                       //
//*********************************************************************************//
#ifndef PRODUCT_TEST_ENABLE
#define PRODUCT_TEST_ENABLE			0
#endif

#ifndef PT_GPIO_ENABLE
#define PT_GPIO_ENABLE				0
#endif
#ifndef PT_GSENSOR_ENABLE
#define PT_GSENSOR_ENABLE			0
#endif
#ifndef PT_MOTOR_ENABLE
#define PT_MOTOR_ENABLE				0
#endif
#ifndef PT_SD_ENABLE
#define PT_SD_ENABLE				0
#endif
#ifndef PT_HR_ENABLE
#define PT_HR_ENABLE				0
#endif
#ifndef PT_MR_ENABLE
#define PT_MR_ENABLE				0
#endif
#ifndef PT_PRESSURE_ENABLE
#define PT_PRESSURE_ENABLE			0
#endif
#ifndef PT_GPS_ENABLE
#define PT_GPS_ENABLE				0
#endif
#ifndef PT_SPEAKER_MIC_ENABLE
#define PT_SPEAKER_MIC_ENABLE		0
#endif
#ifndef PT_LCD_TP_ENABLE
#define PT_LCD_TP_ENABLE			0
#endif
#ifndef PT_GPIO_CHECK_LCD_TP
#define PT_GPIO_CHECK_LCD_TP		0
#endif

//*********************************************************************************//
//                                  UC SHELL                                       //
//*********************************************************************************//
#ifndef TCFG_UC_SHELL_EN
#define TCFG_UC_SHELL_EN			        DISABLE
#endif
#ifndef UC_SHELL_IO_TX_PIN
#define UC_SHELL_IO_TX_PIN					NO_CONFIG_PORT
#endif
#ifndef UC_SHELL_IO_RX_PIN
#define UC_SHELL_IO_RX_PIN					NO_CONFIG_PORT
#endif

#endif //MACRO_DEFAULT_H
