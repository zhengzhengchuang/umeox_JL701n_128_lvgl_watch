#ifndef _BT_H
#define _BT_H

// #define  BT_EMITTER_TEST

/*配置通话时前面丢掉的数据包包数*/
#define ESCO_DUMP_PACKET_ADJUST	      1	/*配置使能*/
#define ESCO_DUMP_PACKET_DEFAULT	  0
#define ESCO_DUMP_PACKET_CALL		  120 /*0~0xFF*/

#define BACKGROUND_GOBACK             0   //后台链接是否跳回蓝牙 1：跳回

#define TIMEOUT_CONN_TIME             60 //超时断开之后回连的时间s
#define POWERON_AUTO_CONN_TIME        12  //开机去回连的时间

#define PHONE_CALL_FORCE_POWEROFF     0   //通话时候强制关机

#define SBC_FILTER_TIME_MS			  1000	//后台音频过滤时间ms
#define SBC_ZERO_TIME_MS			  500		//静音多长时间认为已经退出
#define NO_SBC_TIME_MS				  200		//无音频时间ms

#define SNIFF_CNT_TIME                5/////<空闲5S之后进入sniff模式
#define SNIFF_MAX_INTERVALSLOT        800
#define SNIFF_MIN_INTERVALSLOT        100
#define SNIFF_ATTEMPT_SLOT            4
#define SNIFF_TIMEOUT_SLOT            1


struct app_bt_opr {
    u8 init_ok : 1;		// 1-初始化完成
    u8 call_flag : 1;	// 1-由于蓝牙打电话命令切回蓝牙模式
    u8 exit_flag : 1;	// 1-可以退出蓝牙标志
    u8 exiting : 1;	// 1-正在退出蓝牙模式
    u8 wait_exit : 1;	// 1-等退出蓝牙模式
    u8 a2dp_decoder_type: 3;	// 从后台返回时记录解码格式用
    u8 a2dp_en : 1;		// a2dp 播放允许
    u8 music_dev : 3;		// 播放的音乐设备
    u8 last_play_stu : 1;	// 切出蓝牙模式时的播放状态
    u8 cmd_flag ;	// 1-由于蓝牙命令切回蓝牙模式
    u8 ignore_discon_tone;	// 1-退出蓝牙模式， 不响应discon提示音
    u8 sbc_packet_valid_cnt;	// 有效sbc包计数
    u8 sbc_packet_valid_cnt_max;// 最大有效sbc包计数
    u8 sbc_packet_lose_cnt;	// sbc丢失的包数
    u8 sbc_packet_step;	// 0-正常；1-退出中；2-后台
    u8 tws_local_back_role;
    u8 a2dp_start_flag;
    u8 bt_back_flag;
    u8 replay_tone_flag ;
    u8 esco_dump_packet;
    u8 last_connecting_addr[6];

    u32 sbc_packet_filter_to;	// 过滤超时
    u32 no_sbc_packet_to;	// 无声超时
    u32 init_ok_time;	// 初始化完成时间
    u32 auto_exit_limit_time;	// 自动退出时间限制
    u8 bt_direct_init;
    u8 bt_close_bredr;
    u8 hid_mode;
    u8 force_poweroff;
    u8 call_back_flag;  //BIT(0):hfp_status BIT(1):esco_status
    u8 smartbox_watch_upgrade_flag;   //当手机app与手表传输过程中不能响应通话事件

    int timer;
    int tmr_cnt;
    int back_mode_systime;
    int max_tone_timer_hdl;
    int exit_sniff_timer ;
    int window_id;		// 显示音乐界面
};

typedef enum {
    ATCMD_CMD_UNKNOWN = 0,
    ATCMD_CMD_ATA,
    ATCMD_CMD_ATD,
    ATCMD_CMD_BAC,
    ATCMD_CMD_BCC,
    ATCMD_CMD_BCS,
    ATCMD_CMD_BIA,
    ATCMD_CMD_BLDN,
    ATCMD_CMD_BINP,
    ATCMD_CMD_BRSF,
    ATCMD_CMD_BSIR,
    ATCMD_CMD_BTRH,
    ATCMD_CMD_BVRA,
    ATCMD_CMD_CCWA,
    ATCMD_CMD_CHUP,
    ATCMD_CMD_CHLD,
    ATCMD_CMD_CIND,
    ATCMD_CMD_CIND_EQUAL,
    ATCMD_CMD_CLCC,
    ATCMD_CMD_CLIP,
    ATCMD_CMD_CMER,
    ATCMD_CMD_CMEE,
    ATCMD_CMD_CNUM,
    ATCMD_CMD_COPS_SET,
    ATCMD_CMD_COPS_GET,
    ATCMD_CMD_CSRSF,
    ATCMD_CMD_NREC,
    ATCMD_CMD_VGS,
    ATCMD_CMD_VGM,
    ATCMD_CMD_VTS,
    ATCMD_CMD_USER_ANALYZED,
    ATCMD_CMD_XAPL,
    ATCMD_CMD_IPHONEACCEV,
} HFP_HF_ATCMD_TYPE;

extern struct app_bt_opr app_bt_hdl;


extern void app_bt_task();
extern u8 get_bt_init_status();
extern u16 bt_key_event_get(struct key_event *key);
extern void bt_function_select_init();
extern void bredr_handle_register();
extern void sys_auto_shut_down_disable(void);
extern void soft_poweroff_mode(u8 mode) ;
extern void sys_enter_soft_poweroff(void *priv);
extern int earphone_a2dp_codec_get_low_latency_mode();
extern void earphone_a2dp_codec_set_low_latency_mode(int enable);
extern u8 bt_sco_state(void);
extern int bt_get_low_latency_mode();
extern void bt_set_low_latency_mode(int enable);
extern int bt_must_work(void);
extern u8 get_bt_back_flag();
extern void set_bt_back_flag(u8 flag);
extern void clr_tws_local_back_role(void);
extern void bt_init_ok_search_index(void);
extern void sys_auto_shut_down_enable(void);
extern void sys_auto_shut_down_disable(void);
extern void phone_num_play_timer(void *priv);
extern u8 phone_ring_play_start(void);
extern int earphone_a2dp_codec_get_low_latency_mode();
extern void earphone_a2dp_codec_set_low_latency_mode(int enable);
extern void tws_local_back_to_bt_mode(u8 mode, u8 value);
extern u8 bt_get_exit_flag();
extern void wait_exit_btstack_flag(void *priv);
extern void bt_direct_close(void);
extern void bt_direct_init();
extern int bt_background_event_handler_filter(struct sys_event *event);
extern int bt_background_event_handler(struct sys_event *event);
extern void sys_auto_sniff_controle(u8 enable, u8 *addr);
extern void bt_drop_a2dp_frame_stop();
extern u8 bt_get_task_state();


void bt_drop_a2dp_frame_start(void);
u8 get_esco_packet_dump(void);
void user_get_bt_music_info(u8 type, u32 time, u8 *info, u16 len);
void phonebook_packet_handler(u8 type, const u8 *name, const u8 *number, const u8 *date);
void bt_set_led_status(u8 status);
void bt_wait_phone_connect_control(u8 enable);
int bt_wait_connect_and_phone_connect_switch(void *p);
void bt_close_page_scan(void *p);
void bt_send_keypress(u8 key);
void spp_data_handler(u8 packet_type, u16 ch, u8 *packet, u16 size);
int phone_get_device_vol(void);
void bt_set_music_device_volume(int volume);
void bt_reverb_status_change(struct bt_event *bt);
int bt_get_battery_value();


u8 bt_status_event_filter(struct bt_event *bt);
void  bt_status_init_ok(struct bt_event *bt);
void bt_status_init_ok_background(struct bt_event *bt);
void bt_status_connect(struct bt_event *bt);
void bt_status_disconnect(struct bt_event *bt);
void bt_status_phone_income(struct bt_event *bt);
void bt_status_phone_out(struct bt_event *bt);
void bt_status_phone_active(struct bt_event *bt);
void bt_status_phone_hangup(struct bt_event *bt);
void bt_status_phone_number(struct bt_event *bt);
void bt_status_phone_name(struct bt_event *bt);
void bt_status_inband_ringtone(struct bt_event *bt);
void bt_status_a2dp_media_start(struct bt_event *bt);
void bt_status_a2dp_media_stop(struct bt_event *bt);
void bt_status_sco_change(struct bt_event *bt);
void bt_status_call_vol_change(struct bt_event *bt);
void bt_status_sniff_state_update(struct bt_event *bt);
void bt_status_last_call_type_change(struct bt_event *bt);
void bt_status_conn_a2dp_ch(struct bt_event *bt);
void bt_status_conn_hfp_ch(struct bt_event *bt);
void bt_status_phone_menufactuer(struct bt_event *bt);
void bt_status_voice_recognition(struct bt_event *bt);
void bt_status_avrcp_income_opid(struct bt_event *bt);
void bt_status_connect_background(struct bt_event *bt);
void bt_status_disconnect_background(struct bt_event *bt);


u8 bt_hci_event_filter(struct bt_event *bt);
void bt_hci_event_inquiry(struct bt_event *bt);
void bt_hci_event_connection(struct bt_event *bt);
void bt_hci_event_disconnect(struct bt_event *bt);
void bt_hci_event_linkkey_missing(struct bt_event *bt);
void bt_hci_event_page_timeout(struct bt_event *bt);
void bt_hci_event_connection_timeout(struct bt_event *bt);
void bt_hci_event_connection_exist(struct bt_event *bt);



void bt_fast_test_api(void);
void bt_dut_api(u8 value);
void bt_fix_fre_api(u8 fre);
void ble_fix_fre_api();
void bt_send_pair(u8 en);
void bt_read_remote_name(u8 status, u8 *addr, u8 *name);
u8 bt_key_event_filter_after(int key_event);
void key_tws_lr_diff_deal(struct sys_event *event, u8 opt);
void user_change_profile_mode(u8 flag);

u8 bt_key_event_filter_before();
void bt_key_music_pp();
void bt_key_music_prev();
void bt_key_music_next();
void bt_key_vol_up();
void bt_key_vol_down();
void bt_key_call_last_on();
void bt_key_call_hand_up();
void bt_key_call_answer();
void bt_key_call_siri();
void bt_key_hid_control();
void bt_key_third_click(struct sys_event *event);
void bt_key_low_lantecy();
u8 bt_key_reverb_open();

u8 bt_app_switch_exit_check();
void bt_task_start();
void bt_task_close();
void bt_direct_init();
void bt_direct_close_check(void *priv);
void bt_direct_close(void);
void bt_close_bredr();
void bt_init_bredr();

u8 bt_search_status();
u8 bt_ui_key_event_filter(int msg);

#define SET_MUSIC_IN_WATCH         1//手表出声（包括发射器）
#define SET_MUSIC_IN_PHONE         2//手机控制出声

void bt_set_a2dp_en_status(u8 on);//手表控制使能a2dp
u8 bt_get_a2dp_en_status();
void bt_set_music_device_style(u8 style);//手表控制出声方式
u8 bt_get_music_device_style();
void bt_task_set_window_id(int id);//设置音乐模式显示界面

#endif
