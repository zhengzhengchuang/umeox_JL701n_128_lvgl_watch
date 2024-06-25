#ifndef APP_MAIN_H
#define APP_MAIN_H

#include "typedef.h"

typedef struct _APP_VAR {
    s8 music_volume;
    s8 music_volume_r;
    s8 call_volume;
    s8 call_volume_r;
    s8 wtone_volume;
    s8 wtone_volume_r;
    u8 opid_play_vol_sync;
    u8 aec_dac_gain;
    u8 aec_mic_gain;
    u8 aec_mode;        	//AEC模式,default:advance(diable(0), reduce(1), advance(2))
    u8 ul_eq_en;        	//上行EQ使能,default:enable(disable(0), enable(1))
    /*AGC*/
    float ndt_fade_in;  	//单端讲话淡入步进default: 1.3f(0.1 ~ 5 dB)
    float ndt_fade_out;  	//单端讲话淡出步进default: 0.7f(0.1 ~ 5 dB)
    float dt_fade_in;  		//双端讲话淡入步进default: 1.3f(0.1 ~ 5 dB)
    float dt_fade_out;  	//双端讲话淡出步进default: 0.7f(0.1 ~ 5 dB)
    float ndt_max_gain;   	//单端讲话放大上限,default: 12.f(0 ~ 24 dB)
    float ndt_min_gain;   	//单端讲话放大下限,default: 0.f(-20 ~ 24 dB)
    float ndt_speech_thr;   //单端讲话放大阈值,default: -50.f(-70 ~ -40 dB)
    float dt_max_gain;   	//双端讲话放大上限,default: 12.f(0 ~ 24 dB)
    float dt_min_gain;   	//双端讲话放大下限,default: 0.f(-20 ~ 24 dB)
    float dt_speech_thr;    //双端讲话放大阈值,default: -40.f(-70 ~ -40 dB)
    float echo_present_thr; //单端双端讲话阈值,default:-70.f(-70 ~ -40 dB)
    /*AEC*/
    float aec_dt_aggress;   //原音回音追踪等级, default: 1.0f(1 ~ 5)
    float aec_refengthr;    //进入回音消除参考值, default: -70.0f(-90 ~ -60 dB)
    /*ES*/
    float es_aggress_factor;//回音前级动态压制,越小越强,default: -3.0f(-1 ~ -5)
    float es_min_suppress;	//回音后级静态压制,越大越强,default: 4.f(0 ~ 10)
    /*ANS*/
    float ans_aggress;    	//噪声前级动态压制,越大越强default: 1.25f(1 ~ 2)
    float ans_suppress;    	//噪声后级静态压制,越小越强default: 0.04f(0 ~ 1)
    u8 ble_rf_power;
    u8 rf_power;
    u8 poweron_charge;                  //开机进充电标志
    u8 goto_poweroff_flag;
    u8 goto_poweroff_cnt;
    u8 play_poweron_tone;
    u8 remote_dev_company;
    u8 siri_stu;
    int auto_stop_page_scan_timer;     //用于1拖2时，有一台连接上后，超过三分钟自动关闭Page Scan
    volatile int auto_shut_down_timer;
    volatile int wait_exit_timer;
    u16 auto_off_time;
    u16 warning_tone_v;
    u16 poweroff_tone_v;
    u32 start_time;
    s8  usb_mic_gain;
    u8  reverb_status;                 //用于tws+混响在pc模式时，同步关闭混响, 0:非pc模式
    u8 	cycle_mode;
} APP_VAR;

typedef struct _BT_USER_PRIV_VAR {
    //phone
    u8 phone_ring_flag: 1;
    u8 phone_num_flag: 1;
    u8 phone_income_flag: 1;
    u8 phone_call_dec_begin: 1;
    u8 phone_con_sync_num_ring: 1;
    u8 phone_con_sync_ring: 1;
    u8 emitter_or_receiver: 2;
    u16 get_phone_num_timecnt;

    u8 inband_ringtone;
    u8 phone_vol;
    u16 phone_timer_id;
    u8 last_call_type;
    u8 income_phone_num[30];
    u8 income_phone_len;
    s32 auto_connection_counter;
    s32 search_counter;
    int auto_connection_timer;
    u8 auto_connection_addr[6];
    int tws_con_timer;
    u8 tws_start_con_cnt;
    u8 tws_conn_state;
    bool search_tws_ing;
    int sniff_timer;
    bool fast_test_mode;
    u8 set_call_vol_flag;  /*有些客户希望通话默认给手机设置最大音量*/
} BT_USER_PRIV_VAR;

#define    BT_EMITTER_EN     1
#define    BT_RECEIVER_EN    2

typedef struct _BT_USER_COMM_VAR {
} BT_USER_COMM_VAR;

extern APP_VAR app_var;
extern BT_USER_PRIV_VAR bt_user_priv_var;

extern void app_main();

#define earphone (&bt_user_priv_var)

#endif
