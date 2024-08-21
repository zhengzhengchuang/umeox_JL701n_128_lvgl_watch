#ifndef _USER_CFG_ID_H_
#define _USER_CFG_ID_H_

//=================================================================================//
//                            与APP CASE相关配置项[1 ~ 60]                         //
//=================================================================================//
// #define 	VM_FM_EMITTER_FREQ				 1
// #define 	VM_FM_EMITTER_DIG_VOL		 	 2
// #define 	VM_MUSIC_LAST_DEV	    		 3
// #define 	VM_WAKEUP_PND					 4
#define     VM_USB_MIC_GAIN             	 4
#define     VM_ALARM_0                  	 5
#define     VM_ALARM_1                  	 6
#define     VM_ALARM_2                  	 7
#define     VM_ALARM_3                  	 8
#define     VM_ALARM_4                  	 9
#define     VM_ALARM_SNOOZE                	 10
#define     VM_ALARM_MASK               	 11
#define     VM_ALARM_NAME_0             	 12
#define     VM_ALARM_NAME_1             	 13
#define     VM_ALARM_NAME_2             	 14
#define     VM_ALARM_NAME_3             	 15
#define     VM_ALARM_NAME_4             	 16
#define     VM_FM_INFO				    	 17
// #define		VM_RTC_TRIM				    	 18

#define		CFG_RCSP_ADV_HIGH_LOW_VOL		 19
#define     CFG_RCSP_ADV_EQ_MODE_SETTING     20
#define     CFG_RCSP_ADV_EQ_DATA_SETTING     21
#define     ADV_SEQ_RAND                     22
#define     CFG_RCSP_ADV_TIME_STAMP          23
#define     CFG_RCSP_ADV_WORK_SETTING        24
#define     CFG_RCSP_ADV_MIC_SETTING         25
#define     CFG_RCSP_ADV_LED_SETTING         26
#define     CFG_RCSP_ADV_KEY_SETTING         27
#define     CFG_RCSP_MISC_DRC_SETTING        28
#define     CFG_RCSP_MISC_REVERB_ON_OFF      29
#define     VM_ALARM_RING_NAME_0             30
#define     VM_ALARM_RING_NAME_1             31
#define     VM_ALARM_RING_NAME_2             32
#define     VM_ALARM_RING_NAME_3             33
#define     VM_ALARM_RING_NAME_4             34
#define     VM_COLOR_LED_SETTING			 35
#define     VM_EXTRA_FLASH_UPDATE_FLAG		 36
#define     VM_EXTRA_FLASH_ALL_UPDATE_FLAG	 37

// #define 	CFG_FLASH_BREAKPOINT			 29
// #define 	CFG_USB_BREAKPOINT				 30
// #define 	CFG_SD0_BREAKPOINT				 31
// #define 	CFG_SD1_BREAKPOINT				 32
// #define 	CFG_MUSIC_DEVICE				 33
// #define 	CFG_FM_RECEIVER_INFO			 34
// #define 	CFG_FM_TRANSMIT_INFO			 35
// #define 	CFG_AAP_MODE_INFO				 36
#define    VM_TWS_ROLE                       38
#define    VM_WATCH_SELECT                   39


#define    VM_WATCH_DEFAULT_NFC_ID           40
#define    VM_RESET_EX_FLASH_FLAG            41

#define    VM_SPORT_INFO_SWITCH_FLAG         42
#define    VM_SPORT_INFO_MODE_FLAG           43
#define    VM_SPORT_INFO_EXERCISE_HEART_RATE 44
#define    VM_SPORT_INFO_PERSONAL_INFO_FLAG  45
#define    VM_SPORT_INFO_SEDENTARY           46
#define    VM_SPORT_INFO_SLEEP_DETECTION	 47
#define    VM_SPORT_INFO_FALL_DETECTION      48
#define    VM_SPORT_INFO_RAISE_WRIST         49
#define    VM_SPORT_INFO_DAILY_DATA		     50

#define     VM_VIR_RTC_TIME                  51
#define     VM_VIR_ALM_TIME                  52
#define     VM_VIR_SUM_NSEC                  53

#if (VM_ITEM_MAX_NUM > 128)

#define		CFG_FLASH_BREAKPOINT0		128
#define		CFG_FLASH_BREAKPOINT1		129
#define		CFG_FLASH_BREAKPOINT2		130
#define		CFG_FLASH_BREAKPOINT3		131
#define		CFG_FLASH_BREAKPOINT4		132
#define		CFG_FLASH_BREAKPOINT5		133
#define		CFG_FLASH_BREAKPOINT6		134
#define		CFG_FLASH_BREAKPOINT7		135
#define		CFG_FLASH_BREAKPOINT8		136
#define		CFG_FLASH_BREAKPOINT9		137
#define 	CFG_USB_BREAKPOINT0			138
#define 	CFG_USB_BREAKPOINT1			139
#define 	CFG_USB_BREAKPOINT2			140
#define 	CFG_USB_BREAKPOINT3			141
#define 	CFG_USB_BREAKPOINT4			142
#define 	CFG_USB_BREAKPOINT5			143
#define 	CFG_USB_BREAKPOINT6			144
#define 	CFG_USB_BREAKPOINT7			145
#define 	CFG_USB_BREAKPOINT8			146
#define 	CFG_USB_BREAKPOINT9			147
#define 	CFG_SD0_BREAKPOINT0		    148
#define 	CFG_SD0_BREAKPOINT1		    149
#define 	CFG_SD0_BREAKPOINT2		    150
#define 	CFG_SD0_BREAKPOINT3		    151
#define 	CFG_SD0_BREAKPOINT4		    152
#define 	CFG_SD0_BREAKPOINT5		    153
#define 	CFG_SD0_BREAKPOINT6		    154
#define 	CFG_SD0_BREAKPOINT7		    155
#define 	CFG_SD0_BREAKPOINT8		    156
#define 	CFG_SD0_BREAKPOINT9		    157
#define 	CFG_SD1_BREAKPOINT0		    158
#define 	CFG_SD1_BREAKPOINT1		    159
#define 	CFG_SD1_BREAKPOINT2		    160
#define 	CFG_SD1_BREAKPOINT3		    161
#define 	CFG_SD1_BREAKPOINT4		    162
#define 	CFG_SD1_BREAKPOINT5		    163
#define 	CFG_SD1_BREAKPOINT6		    164
#define 	CFG_SD1_BREAKPOINT7		    165
#define 	CFG_SD1_BREAKPOINT8		    166
#define 	CFG_SD1_BREAKPOINT9		    167
#define     CFG_CHGBOX_ADDR             168
#define 	CFG_REMOTE_DN_00		    169
#define 	CFG_REMOTE_DN_01		    170
#define 	CFG_REMOTE_DN_02		    171
#define 	CFG_REMOTE_DN_03		    172
#define 	CFG_REMOTE_DN_04		    173
#define 	CFG_REMOTE_DN_05		    174
#define 	CFG_REMOTE_DN_06		    175
#define 	CFG_REMOTE_DN_07		    176
#define 	CFG_REMOTE_DN_08		    177
#define 	CFG_REMOTE_DN_09		    178
#define 	CFG_REMOTE_DN_10		    179
#define 	CFG_REMOTE_DN_11		    180
#define 	CFG_REMOTE_DN_12		    181
#define 	CFG_REMOTE_DN_13		    182
#define 	CFG_REMOTE_DN_14		    183
#define 	CFG_REMOTE_DN_15		    184
#define 	CFG_REMOTE_DN_16		    185
#define 	CFG_REMOTE_DN_17		    186
#define 	CFG_REMOTE_DN_18		    187
#define 	CFG_REMOTE_DN_19		    188
#define 	CFG_BT_PAGE_LIST		    189
#define		CFG_MUSIC_MODE				190

#define     CFG_SYS_PARA_INFO			191
#define     CFG_GM_CALIPARA_INFO	    192//存储地磁校准信息
#define     CFG_LL_PARA_INFO	        193//存储当前位置的经纬度
#define     CFG_PT_CFG_PARA_INFO	    194//存储祈祷时间配置参数信息
#define     CFG_TASBIH_R_PARA_INFO	    195//存储赞念提醒参数信息
#define     CFG_HCALENDAR_PARA_INFO	    196//存储回历参数信息
#define     CFG_ALARM_PARA_INFO	        197//存储闹钟参数信息
#define     CFG_DND_PARA_INFO	        198//存储勿扰参数信息
#define     CFG_USER_PERSONAL_INFO	    199//存储用户个人信息
#define     CFG_GOMORE_PARA_INFO	    200//存储GoMore参数
#define     CFG_DEV_BOND_CODE_INFO	    201//存储设备绑定码
#define     CFG_MSG_NOTIFY_PARA_INFO	202//存储消息通知参数信息
#define     CFG_SEDENTARY_PARA_INFO	    203//存储久坐提醒参数
#define     CFG_QPUSER_PARA_INFO	    204//古兰经播放参数
#define     CFG_TWS_PARA_INFO	        205//tws参数
#define     CFG_PEDO_DATA_INFO	        206//日常活动参数
#define     CFG_HR_PARA_INFO	        207//心率参数信息
#define     CFG_BO_PARA_INFO	        208//血氧参数信息
#define     CFG_SLP_PARA_INFO	        209//睡眠参数信息
#define     CFG_POWER_PARA_INFO	        210//电量参数信息
#if 0
#define     VM_WATCH_EX_BEGIN            191//拓展表盘信息
#define     VM_WATCH_EX0                      VM_WATCH_EX_BEGIN//拓展表盘信息

#define     VM_WATCH_EX1                 192
#define     VM_WATCH_EX2                 193
#define     VM_WATCH_EX3                 194
#define     VM_WATCH_EX4                 195
#define     VM_WATCH_EX5                 196
#define     VM_WATCH_EX_END              VM_WATCH_EX5//拓展表盘信息
#endif
#endif//(VM_ITEM_MAX_NUM > 128)

#endif /* #ifndef _USER_CFG_ID_H_ */
