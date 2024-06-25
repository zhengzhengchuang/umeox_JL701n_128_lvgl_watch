#include "common/app_common.h"
#include "app_task.h"
#include "app_main.h"
#include "key_event_deal.h"
#include "music/music.h"
#include "pc/pc.h"
/* #include "record/record.h" */
/* #include "linein/linein.h" */
/* #include "fm/fm.h" */
#include "btstack/avctp_user.h"
#include "app_power_manage.h"
#include "app_chargestore.h"
#include "usb/otg.h"
#include "usb/host/usb_host.h"
#include <stdlib.h>
#include "bt/bt_tws.h"
#include "bt/bt.h"
#include "bt/bt_emitter.h"
#include "audio_config.h"
#include "common/power_off.h"
#include "common/user_msg.h"
#include "audio_config.h"
#include "audio_enc.h"
#include "ui/ui_api.h"
#include "le_smartbox_module.h"
#include "le_smartbox_multi_common.h"
/* #include "fm_emitter/fm_emitter_manage.h" */
/* #include "common/fm_emitter_led7_ui.h" */
/* #include "audio_reverb.h" */
#if TCFG_CHARGE_ENABLE
#include "app_charge.h"
#endif
#include "dev_multiplex_api.h"
/* #include "charge_box/charge_ctrl.h" */
#include "device/chargebox.h"
#include "app_online_cfg.h"
/* #include "soundcard/soundcard.h" */
/* #include "smartbox_bt_manage.h" */
#include "dev_manager.h"
#include "common/dev_status.h"
#include "tone_player.h"
#include "ui_manage.h"
#include "soundbox.h"
#include "watch_common.h"
#include "sport/sport_api.h"
#include "sport_info_sync.h"
#define LOG_TAG_CONST       APP_ACTION
#define LOG_TAG             "[APP_ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


//watch切换 app1 or app2
extern u8 select_app1_or_app2;
extern u8 app_change_key_is_press;

int JL_rcsp_event_handler(struct rcsp_event *rcsp);
int bt_background_event_handler(struct sys_event *event);
extern u32 timer_get_ms(void);
extern int alarm_sys_event_handler(struct sys_event *event);
extern void bt_tws_sync_volume();
extern int jl_kws_voice_event_handle(struct key_event *key);

#if TCFG_UI_ENABLE

u32 ui_show_page_list[9] = {DIAL_PAGE_0, PAGE_13};
static int curr_page = 0;

__attribute__((weak))u8 ui_show_menu_page()
{
    UI_HIDE_CURR_WINDOW();
    UI_SHOW_WINDOW(ID_WINDOW_VMENU);
    return true;
}

int ui_page_set_curr_deal(int curr_id)
{
    for (int i = 0; i < sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]); i++) {
        if (curr_id == ui_show_page_list[i]) {
            curr_page = i;
            return true;
        }
    }
    return false;
}


/**
 * @brief ui_com_get_change_page:
 * 跟随复制lcd_ui_key_change_page的相关逻辑来获取
 * 跳转的页面，如果lcd_ui_key_change_page()函数改变逻辑
 * 本函数也要相应的改变!!!
 *
 * @param key_value
 *
 * @return
 */
int ui_com_get_change_page(int key_value)
{
    int window_id = UI_GET_WINDOW_ID();
    int i;

    if (!lcd_backlight_status()) {
        return 0;
    }

    for (i = 0; i < sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]); i++) {
        if (window_id == ui_show_page_list[i]) {
            curr_page = i;
            break;
        }
    }

    if (key_value & BIT(31)) {
        u32 page = key_value & 0x7fffffff;
        int page_in_list = false;
        for (i = 0; i < sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]); i++) {
            if (ui_show_page_list[i] == page) {
                curr_page = i;
                page_in_list = true;
                break;
            }
        }
        if (!page_in_list) {
            return page;
        }
    } else {
        if (!key_value) {
            do {
                curr_page++;
                if (curr_page > (sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]) - 1)) {
                    curr_page = 0;
                }
            } while (ui_show_page_list[curr_page] == 0);
        } else {
            if (key_value == 2) {
                curr_page++; // 先加后减显示当前的
            }
            do {
                curr_page--;
                if (curr_page < 0) {
                    curr_page = sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]) - 1;
                }
            } while (ui_show_page_list[curr_page] == 0);
        }
    }

    return ui_show_page_list[curr_page];
}


static void lcd_ui_key_change_page(int key_value)
{
    int window_id = UI_GET_WINDOW_ID();
    int i;

    if (!lcd_backlight_status()) {
        return;
    }

    /* UI_HIDE_CURR_WINDOW(); */

    /* int page = ui_com_get_change_page(key_value); */
    /* if (page != 0) { */
    /* UI_SHOW_WINDOW(page); */
    /* } */

    if (ui_page_get_auto_sw_effect() == 0) {
        UI_HIDE_CURR_WINDOW();
    }

    int page = ui_com_get_change_page(key_value);
    if (page != 0) {
        UI_SHOW_WINDOW(page);
    }
#if 0
    for (i = 0; i < sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]); i++) {
        if (window_id == ui_show_page_list[i]) {
            curr_page = i;
            break;
        }
    }

    /* __retry: */
    if (key_value & BIT(31)) {
        u32 page = key_value & 0x7fffffff;
        int page_in_list = false;
        for (i = 0; i < sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]); i++) {
            if (ui_show_page_list[i] == page) {
                curr_page = i;
                page_in_list = true;
                break;
            }
        }
        if (!page_in_list) {
            UI_SHOW_WINDOW(page);
            return;
        }
    } else {
        if (!key_value) {
            do {
                curr_page++;
                if (curr_page > (sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]) - 1)) {
                    curr_page = 0;
                }
            } while (ui_show_page_list[curr_page] == 0);
        } else {
            if (key_value == 2) {
                curr_page++; // 先加后减显示当前的
            }
            do {
                curr_page--;
                if (curr_page < 0) {
                    curr_page = sizeof(ui_show_page_list) / sizeof(ui_show_page_list[0]) - 1;
                }
            } while (ui_show_page_list[curr_page] == 0);
        }
    }

    /* if (ui_page_list[curr_page] == PAGE_10 && (!dev_manager_get_total(1))) { */
    /*     goto __retry; */
    /* }//判断音乐播放页面 判断是否有音乐设备 判断是否蓝牙已经连接 */
    /*  */

    /* if (ui_page_list[curr_page] == PAGE_10 && dev_manager_get_total(1) && app_get_curr_task() != APP_MUSIC_TASK) { */
    /*     app_task_switch_to(APP_MUSIC_TASK); */
    /*     //判断音乐播放页面 判断是否音乐模式 */
    /*     return; */
    /* } */
    /*  */

    UI_SHOW_WINDOW(ui_show_page_list[curr_page]);
#endif

}
#else /* #if TCFG_UI_ENABLE */

static void lcd_ui_key_change_page(int key_value)
{
}
#endif /* #if TCFG_UI_ENABLE */



int app_common_key_msg_deal(struct sys_event *event)
{
    int ret = false;
    struct key_event *key = &event->u.key;
    int key_event = event->u.key.event;
    int key_value = event->u.key.value;
 
    if (key_event == KEY_NULL)
        return false;

#if TCFG_UI_ENABLE
    if (key_is_ui_takeover()) {
        switch (key_event) {
        case KEY_UI_HOME:
        case KEY_UI_SHORTCUT:
            ui_key_msg_post(key_event);
            return false;
            break;

        default:
            if (key_event < 0x80) {
                ui_key_msg_post(key_event);
                return false;
            }
            break;
        }
    }
#endif

#if 0
    switch (key_event) {
    default:
        if (key_event < 0x80) {
            ui_key_msg_post(key_event);
            return false;
        }
        break;
    }
#endif

#if (TCFG_UI_ENABLE && TCFG_APP_FM_EMITTER_EN)
    if (!ui_fm_emitter_common_key_msg(key_event)) {
        return false;
    }
#endif

    log_info("key_value = %d, key_event:%d\n", key_value, key_event);

#if 0
    if ((key_event != KEY_POWEROFF) && (key_event != KEY_POWEROFF_HOLD)) {
        extern u8 goto_poweroff_first_flag;
        goto_poweroff_first_flag = 0;
    }
#endif

#if (SMART_BOX_EN)
    extern bool smartbox_key_event_filter_before(int key_event);
    if (smartbox_key_event_filter_before(key_event)) {
        return true;
    }
#endif
    // struct watch_execise __execise_hd;
    // watch_execise_handle_get(&__execise_hd);
    // u8 sport_status = __execise_hd.execise_ctrl_status_get();

    switch (key_event) 
    {
    case KEY_CHANGE_PAGE:
        log_info("   KEY_CHANGE_PAGE  \n");
        if (app_get_curr_task() == APP_WATCH_UPDATE_TASK ||
            app_get_curr_task() == APP_SMARTBOX_ACTION_TASK) {
            printf(" curr is upate, can not opt \n");
            break;
        }

        //lcd_ui_key_change_page(key_value);
        if (get_call_status() == BT_CALL_HANGUP) {
            extern void smartbox_extra_flash_opt_dial_nodify(void);
            smartbox_extra_flash_opt_dial_nodify();
        }
        break;
#if TCFG_UI_ENABLE
    case KEY_UI_HOME:
        log_info("   KEY_UI_MENU  \n");
        //opp传输文件demo
        /* user_send_cmd_prepare(USER_CTRL_OPP_CONNECTION, 0, NULL); */
        if (UI_WINDOW_PREEMPTION_CHECK()) {
            log_info("   preemption page \n");
            /* break; */
            UI_WINDOW_PREEMPTION_DEL(ID_WINDOW_IDLE); // 充电可以被切走
            if (UI_WINDOW_PREEMPTION_CHECK()) {
                break;
            }
        }
#if SMBOX_MULTI_BLE_EN
        app_change_key_is_press = 1;           //处于切换app的模式
        if(select_app1_or_app2) 
        {
            ble_profile_again_init(0);//关闭app1的adv
            ble_app_disconnect();//断开app1的连接
        }else 
        {
            smbox_multi_ble_module_enable(0);//断开app2的连接、关闭广播
        }

        select_app1_or_app2 = !select_app1_or_app2;
        log_info("SELECT_APP1_OR_APP2: %d\n", select_app1_or_app2);

        /* //重新注册函数 */
        //!!!!!!!需要注意是adv状态还是连接状态--只要是连接切换就需要在断链之后才会切换
        if (!smartbox_get_con_handle() && \
                !mul_dev_get_conn_handle(0, SMBOX_MULTI_ROLE_SERVER) && \
                    !mul_dev_get_conn_handle(0, SMBOX_MULTI_ROLE_CLIENT)) 
        {
            log_info("To_switch_app:%d\n", select_app1_or_app2);
            if (select_app1_or_app2) {
                ble_profile_again_init(1);
            } else {
                smbox_multi_ble_profile_init(1);
            }
        }
#endif

        if ((sport_status != 4) & (sport_status != 0)) 
        {
            if (ID_WINDOW_SPORT_CTRL == UI_GET_WINDOW_ID()) {
                UI_HIDE_CURR_WINDOW();
                UI_SHOW_WINDOW(ID_WINDOW_SPORT_INFO);
            } else {
                __execise_hd.execise_ctrl_status_set(-1, SPORT_STATUS_PAUSE);
                sport_info_sync_pause_exercise_resp();
                watch_sport_pause();

                UI_HIDE_CURR_WINDOW();
                UI_SHOW_WINDOW(ID_WINDOW_SPORT_CTRL);
            }
            break;
        }
        if (ID_WINDOW_BT == UI_GET_WINDOW_ID()) {
            ui_show_menu_page();
        } else 
        {
            if (ui_page_get_auto_sw_effect() == 0) {
                UI_HIDE_CURR_WINDOW();
                UI_SHOW_WINDOW(ID_WINDOW_BT);
            } else {
                static struct effect_sca_alpha sal = {0};
                sal.alpha = 100;
                sal.cnt = 5;
                ui_auto_sw_page_effect_post(UI_GET_WINDOW_ID(), ID_WINDOW_BT, EFFECT_MODE_AUTO_SCA_WITH_BG, NULL, &sal);
            }
        }
        break;
    case KEY_UI_SHORTCUT:
        log_info("  KEY_UI_SHORTCUT  \n");
        if ((sport_status != 4) & (sport_status != 0)) {
            break;
        } else {
            if (ID_WINDOW_SPORT_SHOW == UI_GET_WINDOW_ID()) {
                ui_key_msg_post(key_event);
                break;
            }
            extern u8 ui_show_shortcut_key();
            ui_show_shortcut_key();
        }
        break;
    case KEY_UI_POWEROFF:
        log_info("  KEY_UI_POWEROFF  \n");
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_SHUTDOWN_OR_RESET);
        break;
#endif /* #if TCFG_UI_ENABLE */

#if TCFG_APP_BT_EN

#if TCFG_USER_TWS_ENABLE
    case KEY_TWS_CONN:
        log_info("    KEY_TWS_CONN \n");
        bt_open_tws_conn(0);
        break;
    case KEY_TWS_DISCONN:
        log_info("    KEY_TWS_DISCONN \n");
        bt_disconnect_tws_conn();
        break;
    case KEY_TWS_REMOVE_PAIR:
        log_info("    KEY_TWS_REMOVE_PAIR \n");
        bt_tws_remove_tws_pair();
        break;
    case KEY_TWS_SEARCH_PAIR:
        log_info("    KEY_TWS_SEARCH_PAIR \n");
        bt_tws_start_search_and_pair();
        break;
    case KEY_TWS_SEARCH_REMOVE_PAIR:
        log_info("    KEY_TWS_SEARCH_REMOVE_PAIR \n");
        bt_tws_search_or_remove_pair();
        break;
#endif

    case KEY_BT_DIRECT_INIT:
        bt_direct_init();
        break;
    case KEY_BT_DIRECT_CLOSE:
        bt_direct_close();
        break;
#endif

#if 0
    case  KEY_POWEROFF:
    case  KEY_POWEROFF_HOLD:
        power_off_deal(event, key_event - KEY_POWEROFF);
        break;
#endif

#if 0
    case KEY_IR_NUM_0:
    case KEY_IR_NUM_1:
    case KEY_IR_NUM_2:
    case KEY_IR_NUM_3:
    case KEY_IR_NUM_4:
    case KEY_IR_NUM_5:
    case KEY_IR_NUM_6:
    case KEY_IR_NUM_7:
    case KEY_IR_NUM_8:
    case KEY_IR_NUM_9:
        break;
#endif

    case KEY_CHANGE_MODE:
#if (TCFG_DEC2TWS_ENABLE)
        if (!key->init) {
            break;
        }
#endif
#if TWFG_APP_POWERON_IGNORE_DEV
        if ((timer_get_ms() - app_var.start_time) > TWFG_APP_POWERON_IGNORE_DEV)
#endif//TWFG_APP_POWERON_IGNORE_DEV

        {
            printf("KEY_CHANGE_MODE\n");
            app_task_switch_next();
        }
        break;

    case KEY_VOL_UP:
        log_info("COMMON KEY_VOL_UP\n");
        if (!tone_get_status()) {
            app_audio_volume_up(1);
            printf("common vol+: %d", app_audio_get_volume(APP_AUDIO_CURRENT_STATE));
        }
        if (app_audio_get_volume(APP_AUDIO_CURRENT_STATE) == app_audio_get_max_volume()) {
            if (tone_get_status() == 0) {
#if TCFG_MAX_VOL_PROMPT
                tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 0);
#endif
            }
        }

#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif
        //UI_SHOW_MENU(MENU_MAIN_VOL, 1000, app_audio_get_volume(APP_AUDIO_CURRENT_STATE), NULL);
        break;

    case KEY_VOL_DOWN:
        log_info("COMMON KEY_VOL_DOWN\n");
        app_audio_volume_down(1);
        printf("common vol-: %d", app_audio_get_volume(APP_AUDIO_CURRENT_STATE));
#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif
        //UI_SHOW_MENU(MENU_MAIN_VOL, 1000, app_audio_get_volume(APP_AUDIO_CURRENT_STATE), NULL);
        break;

    case  KEY_EQ_MODE:
#if(TCFG_EQ_ENABLE == 1)
        eq_mode_sw();
#endif
        break;

#if AUDIO_OUTPUT_INCLUDE_BT

    case KEY_BT_EMITTER_RECEIVER_SW:
        printf(" KEY_BT_EMITTER_RECEIVER_SW\n");
        bt_emitter_receiver_sw();
        break;

    case KEY_BT_EMITTER_PAUSE:
        r_printf(" KEY_BT_EMITTER_PAUSE\n");
        UI_MSG_POST("bt_emitter:status=%d", 0);
        bt_emitter_pp(0);
        break;

    case KEY_BT_EMITTER_PLAY:
        r_printf(" KEY_BT_EMITTER_PLAY\n");
        UI_MSG_POST("bt_emitter:status=%d", 1);
        bt_emitter_pp(1);
        break;

    case KEY_BT_EMITTER_SW:
        r_printf("KEY_BT_EMITTER_SW\n");
        {
            extern u8 bt_emitter_stu_sw(void);

            if (bt_emitter_stu_sw()) {
                printf("bt emitter start \n");
            } else {
                printf("bt emitter stop \n");
            }
        }
        break;
#endif


#if(TCFG_CHARGE_BOX_ENABLE)
    case  KEY_BOX_POWER_CLICK:
    case  KEY_BOX_POWER_LONG:
    case  KEY_BOX_POWER_HOLD:
    case  KEY_BOX_POWER_UP:
        charge_box_key_event_handler(key_event);
        break;
#endif
#if (TCFG_MIC_EFFECT_ENABLE)
    case KEY_REVERB_OPEN:
#if TCFG_USER_TWS_ENABLE
        if (!key->init) {
            break;
        }
#endif

        if (mic_effect_get_status()) {
            mic_effect_stop();
        } else {
            mic_effect_start();
        }
        break;
#endif
    case KEY_ENC_START:
#if (RECORDER_MIX_EN)
        if (recorder_mix_get_status()) {
            printf("recorder_encode_stop\n");
            recorder_mix_stop();
        } else {
            printf("recorder_encode_start\n");
            recorder_mix_start();
        }
#endif/*RECORDER_MIX_EN*/
        break;
    default:
        // extern void ui_key_msg_post(int key_value, int key_event);
        // ui_key_msg_post(key_value, key_event);
#ifdef CONFIG_BOARD_AC695X_SOUNDCARD
        soundcard_key_event_deal(key_event);
#endif
        break;

    }
#if (SMART_BOX_EN)
    extern int smartbox_common_key_event_deal(u8 key_event, int ret);
    ret = smartbox_common_key_event_deal(key_event, ret);
#endif
    return ret;
}

int app_power_user_event_handler(struct device_event *dev)
{
#if(TCFG_SYS_LVD_EN == 1)
    switch (dev->event) {
    case POWER_EVENT_POWER_WARNING:
        ui_update_status(STATUS_LOWPOWER);
        tone_play_by_path(tone_table[IDEX_TONE_LOW_POWER], 1);
        /* return 0; */
    }
#endif
    return app_power_event_handler(dev);
}

static void app_common_device_event_handler(struct sys_event *event)
{
    int ret = 0;
    const char *logo = NULL;
    const char *usb_msg = NULL;
    u8 app  = 0xff ;
    u8 alarm_flag = 0;
    switch ((u32)event->arg) {
#if (TCFG_CFG_TOOL_ENABLE || TCFG_ONLINE_ENABLE)
    case DEVICE_EVENT_FROM_CFG_TOOL:
        extern int app_cfg_tool_event_handler(struct cfg_tool_event * cfg_tool_dev);
        app_cfg_tool_event_handler(&event->u.cfg_tool);
        break;
#endif
#if TCFG_CHARGE_ENABLE
    case DEVICE_EVENT_FROM_CHARGE:
        app_charge_event_handler(&event->u.dev);
        break;
#endif//TCFG_CHARGE_ENABLE

#if (TCFG_ONLINE_ENABLE || TCFG_CFG_TOOL_ENABLE)
    case DEVICE_EVENT_FROM_CI_UART:
        ci_data_rx_handler(CI_UART);
        break;

#if TCFG_USER_TWS_ENABLE
    case DEVICE_EVENT_FROM_CI_TWS:
        ci_data_rx_handler(CI_TWS);
        break;
#endif//TCFG_USER_TWS_ENABLE
#endif//TCFG_ONLINE_ENABLE

    case DEVICE_EVENT_FROM_POWER:
        app_power_user_event_handler(&event->u.dev);
        break;

#if TCFG_CHARGESTORE_ENABLE || TCFG_TEST_BOX_ENABLE
    case DEVICE_EVENT_CHARGE_STORE:
        app_chargestore_event_handler(&event->u.chargestore);
        break;
#endif//TCFG_CHARGESTORE_ENABLE || TCFG_TEST_BOX_ENABLE

#if(TCFG_CHARGE_BOX_ENABLE)
    case DEVICE_EVENT_FROM_CHARGEBOX:
        charge_box_ctrl_event_handler(&event->u.chargebox);
        break;
#endif


    case DEVICE_EVENT_FROM_OTG:
        printf("66666666666DEVICE_EVENT_FROM_OTG\n");
        ///先分析OTG设备类型
        usb_msg = (const char *)event->u.dev.value;
        if (usb_msg[0] == 's') {
            ///是从机
#if TCFG_PC_ENABLE
            ret = pc_device_event_handler(event);
            if (ret == true) {
                app = APP_PC_TASK;
            }
#endif
            break;
        } else if (usb_msg[0] == 'h') {
            ///是主机, 统一于SD卡等响应主机处理，这里不break
        } else {
            log_e("unknow otg devcie !!!\n");
            break;
        }
    case DRIVER_EVENT_FROM_SD0:
    case DRIVER_EVENT_FROM_SD1:
    case DRIVER_EVENT_FROM_SD2:
#if TCFG_APP_MUSIC_EN
        ret = dev_status_event_filter(event);///解码设备上下线， 设备挂载等处理
        if (ret == true) {
            if (event->u.dev.event == DEVICE_EVENT_IN) {
                ///设备上线， 非解码模式切换到解码模式播放
                if (app_get_curr_task() != APP_MUSIC_TASK) {
                    app = APP_MUSIC_TASK;
                }
            }
        }
#if (TCFG_SD_ALWAY_ONLINE_ENABLE && (TCFG_SD0_ENABLE || TCFG_SD1_ENABLE))
        extern void sdx_dev_detect_timer_del();
        sdx_dev_detect_timer_del();
#endif
#endif
        break;

#if TCFG_APP_LINEIN_EN
    case DEVICE_EVENT_FROM_LINEIN:
        ret = linein_device_event_handler(event);
        if (ret == true) {
            app = APP_LINEIN_TASK;
        }
        break;
#endif//TCFG_APP_LINEIN_EN

#if TCFG_APP_RTC_EN
    case DEVICE_EVENT_FROM_ALM:
        ret = alarm_sys_event_handler(event);
        if (ret == true) {
            alarm_flag = 1;
            app = APP_RTC_TASK;
        }
        break;
#endif//TCFG_APP_RTC_EN

    default:
        /* printf("unknow SYS_DEVICE_EVENT!!, %x\n", (u32)event->arg); */
        break;
    }

#if (SMART_BOX_EN)
    extern void smartbox_update_dev_state(u32 event, void *param);
    if (ret == true) {
        smartbox_update_dev_state((u32)event->arg, &app);
    }
#endif

    if (app != 0xff) {
        if ((true != app_check_curr_task(APP_PC_TASK)) || alarm_flag) {

            //PC 不响应因为设备上线引发的模式切换
#if TWFG_APP_POWERON_IGNORE_DEV
            if ((timer_get_ms() - app_var.start_time) > TWFG_APP_POWERON_IGNORE_DEV)
#endif//TWFG_APP_POWERON_IGNORE_DEV
            {
#if (TCFG_CHARGE_ENABLE && (!TCFG_CHARGE_POWERON_ENABLE))
                extern u8 get_charge_online_flag(void);
                if (get_charge_online_flag()) {

                } else
#endif
                {

                    printf(">>>>>>>>>>>>>%s %d \n", __FUNCTION__, __LINE__);
                    app_task_switch_to(app);
                }
            }
        }
    }
}


///公共事件处理， 各自模式没有处理的事件， 会统一在这里处理
void app_default_event_deal(struct sys_event *event)
{
    int ret;
    struct key_event *key;
    SYS_EVENT_HANDLER_SPECIFIC(event);
    switch (event->type) {
    case SYS_DEVICE_EVENT:
        /*默认公共设备事件处理*/
        /* printf(">>>>>>>>>>>>>%s %d \n", __FUNCTION__, __LINE__); */
        app_common_device_event_handler(event);
        break;
#if TCFG_APP_BT_EN
    case SYS_BT_EVENT:
        if (true != app_check_curr_task(APP_BT_TASK)) {
            /*默认公共BT事件处理*/
            bt_background_event_handler(event);
        }
        break;
#endif
    case SYS_KEY_EVENT:
    case SYS_UI_EVENT:
        key = &event->u.key;
        if (key->type == KEY_DRIVER_TYPE_VOICE) {
            /* 语音消息 */
            ret = jl_kws_voice_event_handle(event);
            break;
        }
        app_common_key_msg_deal(event);
        break;
    default:
        printf("unknow event\n");
        break;
    }
}
#if 0
//opp传输文件demo
static FILE *fp = NULL;
__attribute__((weak))
int opp_continue_handle(u8 *packet, u8 event, u32 send_read_len)
{
    u8 data[] = "storage/sd0/C/23333.txt";
    u8 data_name[] = "23333.txt";
    switch (event) {
    case 0xa0://将name数据填入packet；将文件总长度返回
        fp = fopen(data, "r");
        memcpy(packet, data_name, strlen(data_name));
        if (fp == NULL) {
            printf("检查tf卡，或者name错误\n");
            return -1;
        }
        u32 fp_len = flen(fp);
        return fp_len;
    case 0x90://上一包文件数据发送完成，填入下一包
        /* r_printf(">> send_read_len = %d\n",send_read_len); */
        if (fp == NULL) {
            printf("检查tf卡，或者name错误\n");
            return -1;
        }
        u32 read_len = fread(fp, packet, send_read_len);
        /* r_printf(">> read_len = %d\n",read_len); */
        if (read_len < send_read_len) {
            fclose(fp);
        }
        return read_len;
    case 0xC3://结束事件
        if (fp) {
            r_printf(">>fclose(fp) \n");
            fclose(fp);
        }
        break;
    }
    return -1; //错误返回-1

}
//opp接收文件demo
__attribute__((weak))
int opp_rx_data(u8 *packet, u32 packet_len, u8 event)
{
    /* put_buf(packet+3, packet_len); */
    switch (event) {
    case 0x01: //收到name数据
        u8 sd_root_path[255] = "storage/sd0/C/\\U";
        u8 sd_root_path_len = strlen(sd_root_path);
        for (int i = 0; i < packet_len - 2; i += 2) {
            sd_root_path[sd_root_path_len + i] = packet[1 + i];        //高低位交换
            sd_root_path[sd_root_path_len + i + 1] = packet[i];
        }
        printf("name: %s", sd_root_path);
        put_buf(sd_root_path, sd_root_path_len + packet_len);
        fp = fopen(sd_root_path, "w+");
        if (fp == NULL) {
            printf("检查tf卡，或者name错误\n");
            return -1;
        }
        break;
    case 0x02: //收到文件数据
        putchar('W');
        if (fp == NULL) {
            printf("检查tf卡，或者name错误\n");
            return -1;
        }
        fwrite(fp, packet, packet_len);
        break;
    case 0x03: //收到结束命令
        putchar('F');
        if (fp) {
            fclose(fp);
            fp = NULL;
        }
        break;
    }
    return 1;
}
#endif
