#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "btstack/avctp_user.h"
#include "app_main.h"
#include "init.h"
#include "../lyrics_api.h"
#include "key_event_deal.h"
#include "bt_emitter.h"
#include "audio_dec.h"
#include "audio_dec_file.h"
#include "music/music_player.h"

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
#if TCFG_SPI_LCD_ENABLE

extern void key_ui_takeover(u8 on);

#define STYLE_NAME  JL

extern void volume_up(void);
extern void volume_down(void);

static int button_ontouch_1(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        /* app_audio_volume_down(1); */
        volume_down();
        log_info("common vol-: %d", app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif

        ui_slider_set_persent_by_id(MUSIC_VOICE_SLIDER,  app_audio_get_volume(APP_AUDIO_STATE_MUSIC) * 100 / get_max_sys_vol());

        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_VOICE_DOWN_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_ontouch_1,
};



static int button_ontouch_2(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        /* app_audio_volume_up(1); */
        volume_up();
        log_info("common vol+: %d", app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
        if (app_audio_get_volume(APP_AUDIO_STATE_MUSIC) == app_audio_get_max_volume()) {
            if (tone_get_status() == 0) {
#if TCFG_MAX_VOL_PROMPT
                tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 0);
#endif
            }
        }


#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif
        ui_slider_set_persent_by_id(MUSIC_VOICE_SLIDER,  app_audio_get_volume(APP_AUDIO_STATE_MUSIC) * 100 / get_max_sys_vol());



        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(MUSIC_VOICE_UP_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_ontouch_2,
};




static int button_ontouch_3(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s\n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(PAGE_10);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_VOICE_BACK_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = button_ontouch_3,
};



static int music_voice_layout_init(int p)
{
    ui_slider_set_persent_by_id(MUSIC_VOICE_SLIDER,  app_audio_get_volume(APP_AUDIO_STATE_MUSIC) * 100 / get_max_sys_vol());

    return 0;
}



static int music_voice_layout_onkey(void *ctr, struct element_key_event *e)
{
    log_info("%s %d\n", __FUNCTION__, e->value);
    switch (e->value) {
    case KEY_UI_SHORTCUT:
    case KEY_UP:
        volume_down();
        log_info("common vol-: %d", app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif

        ui_slider_set_persent_by_id(MUSIC_VOICE_SLIDER,  app_audio_get_volume(APP_AUDIO_STATE_MUSIC) * 100 / get_max_sys_vol());
        break;

    case KEY_UI_HOME:
    case KEY_DOWN:
        volume_up();
        log_info("common vol+: %d", app_audio_get_volume(APP_AUDIO_STATE_MUSIC));
        if (app_audio_get_volume(APP_AUDIO_STATE_MUSIC) == app_audio_get_max_volume()) {
            if (tone_get_status() == 0) {
#if TCFG_MAX_VOL_PROMPT
                tone_play_by_path(tone_table[IDEX_TONE_MAX_VOL], 0);
#endif
            }
        }


#if (TCFG_DEC2TWS_ENABLE)
        bt_tws_sync_volume();
#endif
        ui_slider_set_persent_by_id(MUSIC_VOICE_SLIDER,  app_audio_get_volume(APP_AUDIO_STATE_MUSIC) * 100 / get_max_sys_vol());

        break;
    default:
        return false;

    }
    return TRUE;
}



static int music_voice_layout_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct layout *layout = (struct layout *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);

        key_ui_takeover(1);//接管消息
        layout_on_focus(layout);//设置焦点在本布局(如果布局下还有列表，焦点会最后显示出的列表这里，防止焦点不在，可以把焦点控制在本布局)
        break;
    case ON_CHANGE_SHOW_PROBE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        ui_set_call(music_voice_layout_init, 0);
        break;
    case ON_CHANGE_RELEASE:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
        layout_lose_focus(layout);//失去本焦点
        key_ui_takeover(0);//取消接管
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_VOICE_LAYOUT)
.onchange = music_voice_layout_onchange,
 .onkey = music_voice_layout_onkey,
  .ontouch = NULL,
};








#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

