#include "system/includes.h"
#include "rtc/alarm.h"
#include "common/app_common.h"
#include "system/timer.h"
#include "app_main.h"
#include "tone_player.h"
#include "app_task.h"
#include "btstack/avctp_user.h"
#if (SMART_BOX_EN && JL_SMART_BOX_EXTRA_FLASH_OPT)
#include "smartbox_extra_flash_opt.h"
#endif
#include "ui/ui_sys_param.h"


#if TCFG_APP_RTC_EN

#if TCFG_SPI_LCD_ENABLE
#include "ui/ui_api.h"
#include "ui/ui_style.h"
#include "key_event_deal.h"
#endif

#define ALARM_RING_MAX 50
volatile u16 g_alarm_ring_cnt = 0;

extern void emitter_open(u8 source);
extern void emitter_close(u8 source);

/*************************************************************
   此文件函数主要是用户主要修改的文件

void set_rtc_default_time(struct sys_time *t)
设置默认时间的函数

void alm_wakeup_isr(void)
闹钟到达的函数

void alarm_event_handler(struct sys_event *event, void *priv)
监听按键消息接口,应用于随意按键停止闹钟


int alarm_sys_event_handler(struct sys_event *event)
闹钟到达响应接口

void alarm_ring_start()
闹钟铃声播放接口


void alarm_stop(u8 reason)
闹钟铃声停止播放接口
**************************************************************/

static u8 rtc_can_run(void)
{
    printf("%s", __func__);
    u8 call_status = get_call_status();
    if ((call_status == BT_CALL_ACTIVE) ||
        (call_status == BT_CALL_OUTGOING) ||
        (call_status == BT_CALL_ALERT) ||
        (call_status == BT_CALL_INCOMING)) {
        //通话过程不允许开rtc
        return false;
    }
    return true;
}

u8 set_rtc_default_time(struct sys_time *t)
{
#if (defined(CONFIG_CPU_BR28))
    extern bool read_p11_sys_time(struct sys_time * t);
    if (read_p11_sys_time(t)) {
        g_printf("rtc_read_sys_time: %d-%d-%d %d:%d:%d\n",
                 t->year, t->month, t->day, t->hour, t->min, t->sec);
        return 1;
    }
#else
    t->year = 2019;
    t->month = 5;
    t->day = 5;
    t->hour = 18;
    t->min = 18;
    t->sec = 18;
#endif
    return 0;
}


__attribute__((weak))
u8 rtc_app_alarm_ring_play(u8 alarm_state)
{
    return 0;
}


void alm_wakeup_isr(void)
{
    if (!is_sys_time_online()) {
        alarm_active_flag_set(true);
    } else {
        alarm_active_flag_set(true);
        struct sys_event e;
        e.type = SYS_DEVICE_EVENT;
        e.arg  = (void *)DEVICE_EVENT_FROM_ALM;
        e.u.dev.event = DEVICE_EVENT_IN;
        e.u.dev.value = 0;
        sys_event_notify(&e);
    }
}


void alarm_ring_cnt_clear(void)
{
    g_alarm_ring_cnt = 0;
}

void alarm_stop(u8 reason)
{
    u32 rets_addr;
    __asm__ volatile("%0 = rets ;" : "=r"(rets_addr));
    printf("%s:0x%x", __func__, rets_addr);

#if TCFG_UI_ENABLE
    if (reason == 1) {            //闹钟铃声播放超时
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_ALARM_RING_STOP);
    }
#if TCFG_UI_ENABLE_MOTO
    UI_MOTO_RUN(0);
#endif

#endif
    printf("ALARM_STOP !!!\n");
#if TCFG_USER_EMITTER_ENABLE
    emitter_close(1);
#endif
    alarm_active_flag_set(0);
    alarm_ring_cnt_clear();
    rtc_app_alarm_ring_play(0);
}

static void  __alarm_ring_play(void *p)
{
    if (alarm_active_flag_get()) {
        if (g_alarm_ring_cnt > 0) {
            u8 app = app_get_curr_task();
            if (app != APP_RTC_TASK) {
                alarm_stop(0);
                printf("...not in rtc task\n");
                return;
            }

            if (!tone_get_status()) {
                if (!rtc_app_alarm_ring_play(1)) {
                    tone_play_by_path(tone_table[IDEX_TONE_NORMAL], 0);
                    g_alarm_ring_cnt--;
                }
                sys_timeout_add(NULL, __alarm_ring_play, 500);
            } else {
                sys_timeout_add(NULL, __alarm_ring_play, 500);
            }
        } else {
            alarm_stop(1);
        }
    }
}

void alarm_ring_start()
{
#if TCFG_UI_ENABLE_MOTO
    UI_MOTO_RUN(1);
#endif
    if (g_alarm_ring_cnt == 0) {
        g_alarm_ring_cnt = ALARM_RING_MAX;
#if TCFG_USER_EMITTER_ENABLE
        emitter_open(1);
#endif
        sys_timeout_add(NULL, __alarm_ring_play, 500);
    }
}

void alarm_event_handler(struct sys_event *event)
{
    switch (event->type) {
    case SYS_KEY_EVENT:
        if (alarm_active_flag_get()) {
            alarm_stop(1);
            event->consumed = 1;//接管按键消息,app应用不会收到消息
        }
        break;
    default:
        break;
    }
}

SYS_EVENT_HANDLER(SYS_KEY_EVENT, alarm_event_handler, 2);

static u8 alarm_prev_task = APP_BT_TASK;
static int alarm_ui_entry(int page)
{
    u8 app = app_get_curr_task();
    if (app != APP_RTC_TASK) {
        app_task_switch_to(APP_RTC_TASK);
    }
    alarm_prev_task = app_prev_task;
    alarm_ring_start();
    return false;
}

static int alarm_ui_exit(int page)
{
    alarm_stop(0);
    u8 app = app_get_curr_task();
    if (page != ID_WINDOW_ALARM_RING_START) { // 被打断
        if (app == APP_RTC_TASK) {
            app_task_switch_to(APP_BT_TASK);
        }
        if (app_prev_task == APP_RTC_TASK) {
            app_prev_task = alarm_prev_task;
        }
        if ((app_prev_task == APP_BT_TASK) && (app == APP_BT_TASK)) { // 不需要返回，清除蓝牙后台相关标志
            extern void bt_background_flag_clean(void);
            bt_background_flag_clean();
        }
    } else { // 自己结束的
        if (app == APP_RTC_TASK) {
            app_task_switch_back();
        }
    }
    return false;
}

int alarm_sys_event_handler(struct sys_event *event)
{
    struct application *cur;
    /* printf("%s alarm_event:%x",__func__,(u32)event->arg); */
    if ((u32)event->arg == DEVICE_EVENT_FROM_ALM) {
        if (event->u.dev.event == DEVICE_EVENT_IN) {
            printf("alarm_sys_event_handler\n");
            alarm_update_info_after_isr();
            // int ret = UI_WINDOW_PREEMPTION_POSH(ID_WINDOW_ALARM_RING_START, alarm_ui_entry, alarm_ui_exit, UI_WINDOW_PREEMPTION_TYPE_ALARM);
            // return ret;
            return 0;
        }
    }

    return false;
}

#endif

