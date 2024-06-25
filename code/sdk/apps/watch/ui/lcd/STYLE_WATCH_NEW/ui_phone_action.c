#include "app_config.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "btstack/avctp_user.h"
#include "app_main.h"
#include "ui/ui_sys_param.h"
#include "audio_config.h"


#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_PHONE_ACTION

#define STYLE_NAME  JL


static u16 phone_timer = 0;


extern void __set_bt_esco_by_watch(bool flag);
extern void set_call_log_type(u8 type);
extern BT_USER_PRIV_VAR bt_user_priv_var;

static int phone_ui_switch_init(int id)
{

    printf("%s STATUS = %d\n", __FUNCTION__, get_call_status());
    ui_hide(PHONE_VOLUME_LAYOUT);
    if ((get_call_status() == BT_CALL_OUTGOING) ||
        (get_call_status() == BT_CALL_ALERT)) {
        ui_show(PHONE_OUT_LAYOUT);
    } else if (get_call_status() == BT_CALL_INCOMING) {
        ui_show(PHONE_INCOMING_LAYOUT);
    } else if (get_call_status() == BT_CALL_ACTIVE) {
        ui_show(PHONE_ACTIVE_LAYOUT);
    } else {

    }
    return 0;
}

static void phone_status_check(void *p)
{
    if (!phone_timer) {
        return ;
    }
}

static int bt_phone_handler(const char *type, u32 arg)
{
    if (arg) {
    }
    return 0;
}


static int bt_phone_active_handler(const char *type, u32 arg)
{
    if (arg) {
        ui_hide(PHONE_OUT_LAYOUT);
        ui_hide(PHONE_INCOMING_LAYOUT);
        ui_show(PHONE_ACTIVE_LAYOUT);
    }
    return 0;
}


void PHONE_name_test();
static int bt_phone_name_handler(const char *type, u32 arg)
{
    if (arg) {
        PHONE_name_test();
    }
    return 0;
}
/************************************************************
    窗口app与ui的消息交互注册 app可以发生消息到ui进行显示
 ************************************************************/

static const struct uimsg_handl ui_msg_handler[] = {
    { "phone_num",        bt_phone_handler     },
    { "phone_active",     bt_phone_active_handler     },
    { "phone_name",       bt_phone_name_handler     },
    { NULL, NULL},      /* 必须以此结尾！ */
};


/************************************************************
                         蓝牙电话主页窗口控件
              可以在这个窗口注册各个布局需要用的资源
 ************************************************************/


static int bt_phone_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        puts("\n***bt_phone_mode_onchange***\n");
        /*
        * 注册APP消息响应
        */
        ui_register_msg_handler(ID_WINDOW_PHONE, ui_msg_handler);//注册消息交互的回调
        ui_set_call(phone_ui_switch_init, 0);
        if (!phone_timer) {
            phone_timer = sys_timer_add(NULL, phone_status_check, 500);
        }
#if TCFG_UI_ENABLE
        ui_auto_shut_down_disable();
        key_ui_takeover(1);
#endif
        break;
    case ON_CHANGE_RELEASE:
        if (phone_timer) {
            sys_timeout_del(phone_timer);
        }
        phone_timer = 0;
#if TCFG_UI_ENABLE
        ui_auto_shut_down_enable();
        key_ui_takeover(0);
#endif
        break;
    default:
        return false;
    }
    return false;
}


static int bt_phone_mode_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
    case ELM_EVENT_TOUCH_L_MOVE:
        printf("phone mode ontouch \n");
        return true; // 通话左右滑动不响应
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(ID_WINDOW_PHONE)
.onchange = bt_phone_mode_onchange,
 .onkey = NULL,
  .ontouch = bt_phone_mode_ontouch,
};






static int phone_coming_layout_onkey(void *ctr, struct element_key_event *e)
{
    printf("%s %d\n", __FUNCTION__, e->value);
    switch (e->value) {
    case KEY_OK:
        if (get_call_status() == BT_CALL_INCOMING) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_ANSWER, 0, NULL);
        } else {
            ui_hide(PAGE_6);
        }
        break;
    default:
        if (get_call_status() == BT_CALL_INCOMING) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        } else {
            ui_hide(PAGE_6);
        }
        break;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(PHONE_INCOMING_LAYOUT)
.onchange = NULL,
 .onkey = phone_coming_layout_onkey,
  .ontouch = NULL,
};


static int phone_active_layout_onkey(void *ctr, struct element_key_event *e)
{
    printf("%s %d\n", __FUNCTION__, e->value);
    switch (e->value) {
    case KEY_OK:
        if (get_call_status() == BT_CALL_ACTIVE) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        }
        /* ui_hide(PAGE_6); */
        break;
    default:
        if (get_call_status() == BT_CALL_ACTIVE) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        }
        /* ui_hide(PAGE_6); */
        break;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(PHONE_ACTIVE_LAYOUT)
.onchange = NULL,
 .onkey = phone_active_layout_onkey,
  .ontouch = NULL,
};

extern u32 bt_get_phone_active_start_time_ms(void);
extern u32 timer_get_ms(void);

static int phone_active_time_onchange(void *ctrl, enum element_change_event e, void *arge)
{
    struct ui_time *time = (struct ui_time *)ctrl;
    struct utime time_r = {0};
    u32 start_tmr, cur_tmr;

    switch (e) {
    case ON_CHANGE_INIT:
        start_tmr = bt_get_phone_active_start_time_ms();
        cur_tmr = timer_get_ms();
        if (cur_tmr >= start_tmr) {
            cur_tmr = (cur_tmr - start_tmr) / 1000;
        } else {
            cur_tmr = (((u32) - 1) - start_tmr + cur_tmr) / 1000;
        }
        time_r.hour = cur_tmr / 3600;
        time_r.min = cur_tmr % 3600 / 60;
        time_r.sec = cur_tmr % 60;
        ui_time_update(time, &time_r);
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(BASEFORM_23)
.onchange = phone_active_time_onchange,
 .onkey = NULL,
  .ontouch  = NULL,
};

static int phone_out_layout_onkey(void *ctr, struct element_key_event *e)
{
    printf("%s %d\n", __FUNCTION__, e->value);
    switch (e->value) {
    case KEY_OK:
        if (get_call_status() == BT_CALL_OUTGOING) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        }
        /* ui_hide(PAGE_6); */
        break;
    default:
        if (get_call_status() == BT_CALL_OUTGOING) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        }
        /* ui_hide(PAGE_6); */

        break;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(PHONE_OUT_LAYOUT)
.onchange = NULL,
 .onkey = phone_out_layout_onkey,
  .ontouch = NULL,
};



static u16 phone_num_timer = 0;
static u8 phone_num[16] = {"               "};
static u8 phone_name[20] = {0};

static void PHONE_TEXT_test(void *ptr)
{
    int len = 0;
    if (bt_user_priv_var.phone_num_flag) {
        snprintf((char *)phone_num, sizeof(phone_num), "%s", bt_user_priv_var.income_phone_num);
        extern int phonebook_get_name_by_number(u8 * number, u8 * name);
        len = phonebook_get_name_by_number(phone_num, phone_name);
        if (len > 0) {
            ui_text_set_textu_by_id(PHONE_NAME_TEXT, (const char *)phone_name, len, FONT_DEFAULT);
        } else {
            ui_text_set_textu_by_id(PHONE_NAME_TEXT, "UNKNOW", strlen("UNKNOW"), FONT_DEFAULT);
        }
        ui_text_set_str_by_id(PHONE_NUM_TEXT, "ascii", (const char *)phone_num);
        if (phone_num_timer) {
            sys_timeout_del(phone_num_timer);
            phone_num_timer = 0;
        }
    }
}

extern u8 phone_name_text[30];
static void PHONE_name_test()
{
    u8 len = strlen((const char *)phone_name_text);
    /* printf(">>%d\n", len); */
    /* put_buf(phone_name_text, len); */
    snprintf((char *)phone_num, sizeof(phone_num), "%s", bt_user_priv_var.income_phone_num);
    if (len > 0) {
        ui_text_set_textu_by_id(PHONE_NAME_TEXT, (const char *)phone_name_text, len, FONT_DEFAULT);
    } else {
        ui_text_set_textu_by_id(PHONE_NAME_TEXT, "UNKNOW", strlen("UNKNOW"), FONT_DEFAULT);
    }
    ui_text_set_str_by_id(PHONE_NUM_TEXT, "ascii", (const char *)phone_num);
}


static int phone_num_ascii_onchange(void *ctr, enum element_change_event e, void *arg)
{
    int index;
    struct ui_text *text = (struct ui_text *)ctr;
    struct ui_text_attrs *text_attrs = &(text->attrs);
    switch (e) {
    case ON_CHANGE_INIT:
        memset(phone_num, ' ', sizeof(phone_num));
        phone_num[sizeof(phone_num) - 1] = 0;

        text_attrs->str = (const char *)phone_num;
        if (!phone_num_timer) {
#if !USER_SUPPORT_PROFILE_PBAP_LIST
            sys_timeout_add(NULL, PHONE_TEXT_test, 1000);
#endif
        }
        break;
    case ON_CHANGE_RELEASE:
        if (phone_num_timer) {
            sys_timeout_del(phone_num_timer);
        }
        phone_num_timer = 0;
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PHONE_NUM_TEXT)
.onchange = phone_num_ascii_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int button_phone_incoming_hangup(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        if (get_call_status() == BT_CALL_INCOMING) {
            set_call_log_type(3);
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        } else {
            ui_hide(PAGE_6);
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_INCOMING_HANGUP)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_phone_incoming_hangup,
};


static int button_phone_incoming_answer(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        if (get_call_status() == BT_CALL_INCOMING) {
            __set_bt_esco_by_watch(1);
            set_call_log_type(2);
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_ANSWER, 0, NULL);
        } else {
            ui_hide(PAGE_6);
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_INCOMING_ANSWER)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_phone_incoming_answer,
};


static int button_phone_active_hangup(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        if (get_call_status() == BT_CALL_ACTIVE) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        } else {
            ui_hide(PAGE_6);
        }
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_ACTIVE_HANGUP)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_phone_active_hangup,
};

static int phone_active_volume_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;
    static u8 touch_action = 0;
    u8 voice_mute_sel;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (touch_action != 1) {
            break;
        }
        ui_hide(PHONE_ACTIVE_LAYOUT);
        ui_show(PHONE_VOLUME_LAYOUT);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        touch_action = 2;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        touch_action = 1;
        return true;
    default :
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_ACTIVE_VOLUME)//声音
.onchange = NULL,
 .onkey = NULL,
  .ontouch = phone_active_volume_ontouch,
};

static int button_phone_out_hangup(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        if (get_call_status() == BT_CALL_OUTGOING) {
            user_send_cmd_prepare(USER_CTRL_HFP_CALL_HANGUP, 0, NULL);
        }
        //ui_hide(PAGE_6); //修复个别双卡机型选卡时ui提前挂断的异常问题
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PHONE_OUT_HANGUP)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_phone_out_hangup,
};


//--------------------------------音量设置界面--------------------------------//

#define GET_PHONE_VOLUME_MAX()		15	//get_max_sys_vol()
static void phone_ui_set_voice(int precent)
{
    s8 volume;

    volume = GET_PHONE_VOLUME_MAX() * precent / 100;
    if (volume > GET_PHONE_VOLUME_MAX()) {
        volume = GET_PHONE_VOLUME_MAX();
    }

    /* printf("%s %d--volume:%d, max_volume:%d",__FUNCTION__,__LINE__,volume,GET_PHONE_VOLUME_MAX()); */
    app_audio_set_volume(APP_AUDIO_STATE_CALL, volume, 1);
}

static int phone_ui_voice_to_percent(s8 volume)
{
    return volume * 100 / GET_PHONE_VOLUME_MAX();
}

static void phone_ui_volume_up(u8 step)
{
    s8 volume = app_audio_get_volume(APP_AUDIO_STATE_CALL);
    volume += step;
    if (volume > GET_PHONE_VOLUME_MAX()) {
        volume = GET_PHONE_VOLUME_MAX();
    }
    /* printf("%s %d--volume:%d, max_volume:%d",__FUNCTION__,__LINE__,volume,GET_PHONE_VOLUME_MAX()); */
    app_audio_set_volume(APP_AUDIO_STATE_CALL, volume, 1);
}

static void phone_ui_volume_down(u8 step)
{
    s8 volume = app_audio_get_volume(APP_AUDIO_STATE_CALL);
    volume -= step;
    if (volume < 0) {
        volume = 0;
    }
    /* printf("%s %d--volume:%d, max_volume:%d",__FUNCTION__,__LINE__,volume,GET_PHONE_VOLUME_MAX()); */
    app_audio_set_volume(APP_AUDIO_STATE_CALL, volume, 1);
}

static int vslider_voice_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_vslider *vslider = (struct ui_vslider *)_ctrl;
    int vslider_percent;

    switch (event) {
    case ON_CHANGE_INIT:
        vslider_percent = phone_ui_voice_to_percent(app_audio_get_volume(APP_AUDIO_STATE_CALL));
        /* log_info("%s %d--vslider_percent:%d",__FUNCTION__,__LINE__,vslider_percent); */
        ui_vslider_set_persent(vslider, vslider_percent);
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}

static int vslider_voice_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_vslider *vslider = (struct ui_vslider *)ctr;
    int vslider_percent;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        vslider_percent = vslider_get_percent(vslider);
        /* log_info("%s %d--vslider_percent:%d",__FUNCTION__,__LINE__,vslider_percent); */
        phone_ui_set_voice(vslider_percent);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        vslider_touch_slider_move(vslider, e);
        break;
    case ELM_EVENT_TOUCH_DOWN:
        /* break; */
        return true;
    default :
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_VOLUME_VSLIDER)//声音-vslider
.onchange = vslider_voice_onchange,
 .onkey = NULL,
  .ontouch = vslider_voice_ontouch,
};

static int pic_voice_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)_ctrl;
    int vslider_percent;

    switch (event) {
    case ON_CHANGE_INIT:
        /* vslider_percent = phone_ui_voice_to_percent(app_audio_get_volume(APP_AUDIO_STATE_CALL)); */
        break;
    case ON_CHANGE_RELEASE:
        break;
    }
    return 0;
}

static int pic_voice_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;
    static u8 touch_action = 0;
    int vslider_percent;

    switch (e->event) {
    case ELM_EVENT_TOUCH_UP:
        if (touch_action != 1) {
            break;
        }
        switch (pic->elm.id) {
        case PHONE_VOLUME_ADD:
            phone_ui_volume_up(1);
            break;
        case PHONE_VOLUME_DEL:
            phone_ui_volume_down(1);
            break;
        default:
            return false;
        }
        vslider_percent = phone_ui_voice_to_percent(app_audio_get_volume(APP_AUDIO_STATE_CALL));
        ui_vslider_set_persent_by_id(PHONE_VOLUME_VSLIDER, vslider_percent);
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        touch_action = 2;
        break;
    case ELM_EVENT_TOUCH_DOWN:
        touch_action = 1;
        return true;
    default :
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_VOLUME_ADD)//声音加
.onchange = NULL,
 .onkey = NULL,
  .ontouch = pic_voice_ontouch,
};
REGISTER_UI_EVENT_HANDLER(PHONE_VOLUME_DEL)//声音减
.onchange = pic_voice_onchange,
 .onkey = NULL,
  .ontouch = pic_voice_ontouch,
};

static int layout_voice_ontouch(void *ctr, struct element_touch_event *e)
{
    struct layout *layout = (struct layout *)ctr;

    switch (e->event) {
    case ELM_EVENT_TOUCH_R_MOVE:
        phone_ui_switch_init(0);
        return true;
        break;
    default:
        break;
    }

    return false;
}
REGISTER_UI_EVENT_HANDLER(PHONE_VOLUME_LAYOUT)//声音
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_voice_ontouch,
};


#endif /* #if TCFG_UI_ENABLE_PHONE_ACTION */
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

