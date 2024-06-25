#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "app_task.h"
#include "system/timer.h"
#include "device/device.h"
#include "key_event_deal.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "sport_data/watch_common.h"
#include "sport_data/detection_response.h"
#include "btstack/avctp_user.h"

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"




#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE

#define STYLE_NAME  JL



static int detection_type = SEDENTARY;
static u16 timer_sedentary = 0;

int detection_type_show_set(int type)
{
    detection_type = type;
    return detection_type;
}

static void ui_return_page_pop_by_timer(void *priv)
{
    if (!timer_sedentary) {
        return ;
    }
    /* UI_WINDOW_BACK_SHOW((u8)priv); */
    UI_WINDOW_PREEMPTION_POP(ID_WINDOW_DETECTION);
}

//SEDENTARY
static int SEDENTARY_LAYOUT_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        if (!timer_sedentary) {
            timer_sedentary = sys_timeout_add(0, ui_return_page_pop_by_timer, 3 * 1000);
        }
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        if (timer_sedentary) {
            sys_timeout_del(timer_sedentary);
            timer_sedentary = 0;
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(SEDENTARY_LAYOUT)
.onchange = SEDENTARY_LAYOUT_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};
//HRWARM
static int timer_hr = 0;
extern int get_heart_rate_threshold(void);
static void HR_WARM_NUM_show(void *p)
{
    if (!timer_hr) {
        return ;
    }
    log_info("%s1", __func__);
    struct watch_heart_rate __heart_rate_hd;
    watch_heart_rate_handle_get(&__heart_rate_hd);
    int vaild;
    struct unumber num1, num2;
    num1.type = TYPE_NUM;
    num1.numbs =  1;
    num1.number[0] = __heart_rate_hd.heart_rate_data_get(LAST_DATA, &vaild);
    ui_number_update_by_id(DETE_REAL_HEART, &num1);
    num2.type = TYPE_NUM;
    num2.numbs =  1;
    num2.number[0] = get_heart_rate_threshold();
    ui_number_update_by_id(DETE_HEART_THRESHOLD, &num2);
}
static int HR_WARM_LAYOUT_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        ui_auto_shut_down_disable();
        if (!timer_hr) {
            timer_hr = sys_timeout_add(NULL, HR_WARM_NUM_show, 100);
        }
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        if (timer_hr) {
            sys_timeout_del(timer_hr);
            timer_hr = 0;
        }
        break;
    default:
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(HRWARM_LAYOUT)
.onchange = HR_WARM_LAYOUT_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int DETECTION_LAYER_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    int timer_sedentary = 0;
    struct element *elm = (struct element *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW_PROBE:
        if (detection_type == SEDENTARY) {
            ui_hide(HRWARM_LAYOUT);
            ui_show(SEDENTARY_LAYOUT);
        } else if (detection_type == EXERCISE_HEART_RATE) {
            ui_hide(SEDENTARY_LAYOUT);
            ui_show(HRWARM_LAYOUT);
        }
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return false;
}

static int DETECTION_LAYER_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
        /* printf("%s,%d \n", __func__, __LINE__); */
        UI_WINDOW_PREEMPTION_POP(ID_WINDOW_DETECTION);
        return true;
        break;
    default :
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(DETECTION_LAYER)
.onchange = DETECTION_LAYER_onchange,
 .onkey = NULL,
  .ontouch = NULL,//DETECTION_LAYER_ontouch,
};

//FALL
static int fall_timer = 0;
static int fall_phone_timer = 0;
#define PHONE_TIMER_INTERVAL 300
static u8 fall_phone_cnt = 0;
static u8 fall_phone_flag = 0;
static u8 fall_cnt = 0;

static void fall_phone_call_detection(void *p)//通话后回到主页面
{
    if ((get_call_status() == BT_CALL_OUTGOING) | (get_call_status() == BT_CALL_ACTIVE) | (get_call_status() == BT_CALL_INCOMING)) { //拨出或通话中
        fall_phone_flag = 1; //已拨出
        return ;
    } else {
        if ((fall_phone_flag == 0) & (fall_phone_cnt * PHONE_TIMER_INTERVAL <= 3000)) { //个别手机需要3-5秒的时间选卡
            fall_phone_cnt++;
        } else {
            ASSERT(fall_phone_timer);
            if ((fall_phone_timer) && ((UI_GET_WINDOW_ID() == ID_WINDOW_CLOCK) | (UI_GET_WINDOW_ID() == ID_WINDOW_SPORT_INFO))) { //运动时也可进入运动界面
                sys_timer_del(fall_phone_timer);
                fall_phone_timer = 0;
                fall_phone_cnt = 0;
                fall_phone_flag = 0;
            } else {
                // 返回
                UI_WINDOW_PREEMPTION_POP(ID_WINDOW_FALL);
            }
        }
    }
}
static void fall_count_down(void *priv)
{
    ui_pic_show_image_by_id(FALL_COUNT_DOWN_PIC, fall_cnt);
    fall_cnt++;
    if (fall_cnt == 10) {
        if (fall_timer) {
            log_info("fall_timer_del");
            sys_timer_del(fall_timer);
            fall_timer = 0;
            fall_cnt = 0;
        }
        ASSERT((fall_phone_timer == 0));
        fall_phone_timer = sys_timer_add(NULL, fall_phone_call_detection, PHONE_TIMER_INTERVAL);
        call_emergency_contact_number();
    }
}
static int FALL_LAYOUT_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    switch (event) {
    case ON_CHANGE_INIT:
        break;
    case ON_CHANGE_SHOW:
        break;
    case ON_CHANGE_FIRST_SHOW:
        ui_auto_shut_down_disable();
        fall_cnt = 0;
        if (!fall_timer) {
            fall_timer = sys_timer_add(NULL, fall_count_down, 1000);
        }
        break;
    case ON_CHANGE_RELEASE:
        ui_auto_shut_down_enable();
        //关闭倒计时
        if (fall_timer) {
            sys_timer_del(fall_timer);
            fall_timer = 0;
            fall_cnt = 0;
        }


        break;
    default:
        break;
    }
    return false;
}

static int FALL_LAYOUT_ontouch(void *ctrl, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
        /* printf("%s,%d \n", __func__, __LINE__); */
        UI_WINDOW_PREEMPTION_POP(ID_WINDOW_FALL);
        return true;
        break;
    default :
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(FALL_LAYOUT)
.onchange = FALL_LAYOUT_onchange,
 .onkey = NULL,
  .ontouch = NULL,//FALL_LAYOUT_ontouch,
};

static int FALL_STOP_BUTTON_ontouch(void *_ctrl, struct element_touch_event *e)
{
    struct element *elm = (struct element *)_ctrl;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (move_flag) {
            move_flag = 0;
            break;
        }
        //关闭倒计时
        if (fall_timer) {
            sys_timer_del(fall_timer);
            fall_timer = 0;
            fall_cnt = 0;
        }
        ui_auto_shut_down_enable();
        // 返回
        UI_WINDOW_PREEMPTION_POP(ID_WINDOW_FALL);
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;

        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(STOP_FALL_CALL_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = FALL_STOP_BUTTON_ontouch,
};








#endif
#endif
