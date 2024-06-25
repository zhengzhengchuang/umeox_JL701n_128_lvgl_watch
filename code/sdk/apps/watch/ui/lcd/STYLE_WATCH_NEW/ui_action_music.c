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
#include "bt/bt.h"
#include "music/music.h"


#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


#if (!TCFG_LUA_ENABLE)

#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_MUSIC

extern void key_ui_takeover(u8 on);

extern u8 a2dp_get_status(void);
extern const char *music_file_get_cur_name(int *len, int *is_unicode);
static void music_check_add(void *ptr);


#define STYLE_NAME  JL

typedef struct _MUSIC_UI_VAR {
    u8 start: 1;
    u8 menu: 1;
    int timer;
    int total_time;
    int file_total;
    int cur_num;
} MUSIC_UI_VAR;


static MUSIC_UI_VAR *ui_handler = NULL;

#define __this (ui_handler)

static int _music_is_play(void)
{
#if TCFG_APP_MUSIC_EN
    if (app_get_curr_task() == APP_MUSIC_TASK) {
        if (music_player_get_play_status() == FILE_DEC_STATUS_PLAY) {
            return true;
        }
    } else
#endif /* #if TCFG_APP_MUSIC_EN */
    {
        if (bt_media_is_running()) {
            return true;
        }
    }
    return false;
}

static int music_pp_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s \n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
#if TCFG_APP_MUSIC_EN
        if (bt_get_music_device_style() == SET_MUSIC_IN_WATCH) 
        {
            if (app_get_curr_task() != APP_MUSIC_TASK) {
                music_set_start_auto_play(1);
                app_task_switch_to(APP_MUSIC_TASK);
            } else {
                app_task_put_key_msg(KEY_MUSIC_PP, 0);
                /* if (_music_is_play() == true) { */
                /* ui_pic_show_image_by_id(MUSIC_PP_PIC, 0); */
                /* } else { */
                /* ui_pic_show_image_by_id(MUSIC_PP_PIC, 1); */
                /* } */

            }
        }
#endif /* #if TCFG_APP_MUSIC_EN */

        if (bt_get_music_device_style() == SET_MUSIC_IN_PHONE) {
            if (app_get_curr_task() != APP_BT_TASK) {
                bt_task_set_window_id(ID_WINDOW_MUSIC);
                app_task_switch_to(APP_BT_TASK);
            }
            bt_key_music_pp();
        }

        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_PP_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_pp_ontouch,
};



static int music_prev_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s \n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        if (bt_get_music_device_style() == SET_MUSIC_IN_PHONE) {
            bt_key_music_prev();
            break;
        }

        app_task_put_key_msg(KEY_MUSIC_PREV, 0);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_PREV_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_prev_ontouch,
};


static int music_next_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s \n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        if (bt_get_music_device_style() == SET_MUSIC_IN_PHONE) {
            bt_key_music_next();
            break;
        }
        app_task_put_key_msg(KEY_MUSIC_NEXT, 0);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_NEXT_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_next_ontouch,
};





static int music_cir_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s \n", __FUNCTION__);
    u8 cycle_mode ;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
#if TCFG_APP_MUSIC_EN
        if (bt_get_music_device_style() != SET_MUSIC_IN_WATCH) {
            break;
        }
        cycle_mode =  music_player_get_repeat_mode();
        log_info("mode = %d \n", cycle_mode);
        switch (cycle_mode) {
        case FCYCLE_ALL:
            music_player_set_repeat_mode(FCYCLE_ONE);
            break;
        case FCYCLE_ONE:
            music_player_set_repeat_mode(FCYCLE_RANDOM);
            break;
        case FCYCLE_RANDOM:
            music_player_set_repeat_mode(FCYCLE_ALL);
            break;
        default:
            music_player_set_repeat_mode(FCYCLE_ALL);
            break;
        };

        switch (music_player_get_repeat_mode()) {
        case FCYCLE_ONE:
            ui_pic_show_image_by_id(MUSIC_CIR_PIC, 0);
            break;
        case FCYCLE_RANDOM:
            ui_pic_show_image_by_id(MUSIC_CIR_PIC, 2);
            break;
        case FCYCLE_ALL:
        default:
            ui_pic_show_image_by_id(MUSIC_CIR_PIC, 1);
            break;
        };
#endif /* #if TCFG_APP_MUSIC_EN */
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_CIR_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_cir_ontouch,
};

static int music_voice_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s \n", __FUNCTION__);
    u8 cycle_mode ;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(PAGE_12);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_VOICE_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_voice_ontouch,
};



static int music_pic_cir_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
#if TCFG_APP_MUSIC_EN
        if (bt_get_music_device_style() != SET_MUSIC_IN_WATCH) {
            ui_pic_set_hide_by_id(MUSIC_CIR_PIC, 1);
            break;
        }
        /* ui_pic_set_hide_by_id(MUSIC_CIR_PIC, 0); */
        switch (music_player_get_repeat_mode()) {
        case FCYCLE_ONE:
            ui_pic_set_image_index(pic, 0);
            break;
        case FCYCLE_RANDOM:
            ui_pic_set_image_index(pic, 2);
            break;
        case FCYCLE_ALL:
        default:
            ui_pic_set_image_index(pic, 1);
            break;
        };
#endif /* #if TCFG_APP_MUSIC_EN */
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_CIR_PIC)
.onchange = music_pic_cir_onchange,
 .ontouch = NULL,
};


static int music_pic_pp_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_pic *pic = (struct ui_pic *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        if (_music_is_play() == true) {
            ui_pic_set_image_index(pic, 1);
        } else {
            ui_pic_set_image_index(pic, 0);
        }
        break;
    case ON_CHANGE_FIRST_SHOW:
        break;
    case ON_CHANGE_RELEASE:
        break;
    default:
        return false;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_PP_PIC)
.onchange = music_pic_pp_onchange,
 .ontouch = NULL,
};


static int music_start(const char *type, u32 arg)
{

    int len = 0;
    int is_unicode = 0;

    if (type && !strcmp(type, "show_lyric")) {
        if (arg) {
            log_info("music_start_have_lyric \n");
        } else {
            log_info("music_start_without_lyric \n");
        }

#if TCFG_APP_MUSIC_EN
        if (app_get_curr_task() == APP_MUSIC_TASK) {
            const char *file_name = music_file_get_cur_name(&len, &is_unicode);
            if (len && file_name) {
                log_info("is_unicode = %d\n", is_unicode);
                put_buf((const u8 *)file_name, len);
                if (is_unicode) {
                    ui_text_set_textw_by_id(MUSIC_NAME_TEXT, file_name, len, FONT_ENDIAN_SMALL, FONT_DEFAULT | FONT_SHOW_SCROLL);
                } else {
                    ui_text_set_text_by_id(MUSIC_NAME_TEXT, file_name, len, FONT_DEFAULT);
                }

                if (__this && !__this->timer) {
                    __this->timer = sys_timer_add(NULL, music_check_add, 500);
                }

            }
        }
#endif /* #if TCFG_APP_MUSIC_EN */
    }
    return 0;
}

static void music_check_add(void *ptr)
{
    if (!__this) {
        return;
    }

    if (bt_get_music_device_style() == SET_MUSIC_IN_PHONE) {
        u8 a2dp_state = a2dp_get_status();
        /* log_info("a2dp_state :%d \n", a2dp_state); */
        if (a2dp_state == BT_MUSIC_STATUS_STARTING) {
            ui_pic_show_image_by_id(MUSIC_PP_PIC, 1);
        } else {
            ui_pic_show_image_by_id(MUSIC_PP_PIC, 0);
        }
        return ;
    }

    if (true == file_dec_is_play()) {
#if (defined(TCFG_LRC_LYRICS_ENABLE) && (TCFG_LRC_LYRICS_ENABLE))
        lrc_show_api(MUSIC_NAME_TEXT, file_dec_get_cur_time(), 0);
#endif

    } else if (file_dec_is_pause()) {

    }

}



static int music_layout_init(int p)
{
    int len = 0;
    int is_unicode = 0;

#if TCFG_APP_MUSIC_EN
    if (bt_get_music_device_style() == SET_MUSIC_IN_WATCH) {
        ui_pic_show_image_by_id(MUSIC_DEV_LOGO_PIC, 0);
        const char *file_name = music_file_get_cur_name(&len, &is_unicode);
        if (len && file_name) {
            log_info("is_unicode = %d\n", is_unicode);
            put_buf((const u8 *)file_name, len);
            if (is_unicode) {
                ui_text_set_textw_by_id(MUSIC_NAME_TEXT, file_name, len, FONT_ENDIAN_SMALL, FONT_DEFAULT | FONT_SHOW_SCROLL);
            } else {
                ui_text_set_text_by_id(MUSIC_NAME_TEXT, file_name, len, FONT_DEFAULT);
            }
        } else {
            ui_text_set_text_by_id(MUSIC_NAME_TEXT, "SD music", strlen("SD music"), FONT_DEFAULT);
        }
    } else
#endif /* #if TCFG_APP_MUSIC_EN */
    {
        ui_pic_show_image_by_id(MUSIC_DEV_LOGO_PIC, 1);
        ui_text_set_text_by_id(MUSIC_NAME_TEXT, "BT music", strlen("BT music"), FONT_DEFAULT);
    }

    if (_music_is_play() == true) {
        ui_pic_show_image_by_id(MUSIC_PP_PIC, 1);
    } else {
        ui_pic_show_image_by_id(MUSIC_PP_PIC, 0);
    }

    if (__this && !__this->timer) {
        __this->timer = sys_timer_add(NULL, music_check_add, 500);
    }

    return 0;
}


static int music_switch_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;
    switch (e->event) {
    case ELM_EVENT_TOUCH_U_MOVE:
    case ELM_EVENT_TOUCH_D_MOVE:
        if (bt_get_music_device_style() == SET_MUSIC_IN_WATCH) {
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(ID_WINDOW_MUSIC_BROWER);
        }
        break;
    default:
        return false;
        break;
    }
    return true;//接管消息
}




static int music_layout_onchange(void *ctr, enum element_change_event e, void *arg)
{
    switch (e) {
    case ON_CHANGE_INIT:
        int len = 0;
        int is_unicode = 0;
        const char *music_name = NULL;
        struct ui_text *text = (struct ui_text *)ui_core_get_element_by_id(MUSIC_NAME_TEXT);
        struct ui_pic *pic = (struct ui_pic *)ui_core_get_element_by_id(MUSIC_DEV_LOGO_PIC);
        if (text) {
            music_name = music_file_get_cur_name(&len, &is_unicode);
            ui_pic_set_image_index(pic, 0);
            if (len && music_name) {
                if (is_unicode) {
                    text->attrs.encode = FONT_ENCODE_UNICODE;
                } else {
                    /* text->attrs.encode = FONT_ENCODE_UTF8; */
                    text->attrs.encode = FONT_ENCODE_ANSI;
                }
            } else {
                if (bt_get_music_device_style() == SET_MUSIC_IN_WATCH) {
                    music_name = "SD music";
                } else {
                    log_info("play phone music. __FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
                    ui_pic_set_image_index(pic, 1);
                    music_name = "BT music";
                }
                len = strlen(music_name);
                text->attrs.encode = FONT_ENCODE_ANSI;
            }
            text->attrs.offset = 0;
            text->attrs.format = "text";
            text->attrs.str    = music_name;
            text->attrs.strlen = len;
            text->attrs.endian = FONT_ENDIAN_SMALL;
            text->attrs.flags  = FONT_DEFAULT;
        }
        if (__this && !__this->timer) {
            __this->timer = sys_timer_add(NULL, music_check_add, 500);
        }
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
        break;
    case ON_CHANGE_SHOW_PROBE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        ui_set_call(music_layout_init, 0);
        break;
    case ON_CHANGE_RELEASE:
        log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, __LINE__);
        break;
    default:
        return FALSE;
    }
    return FALSE;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_LAYOUT)
.onchange = music_layout_onchange,
 .onkey = NULL,
  .ontouch = music_switch_ontouch,
};


static int music_status(const char *type, u32 arg)
{
    int  status = arg;
    /* if (bt_get_music_device_style() == SET_MUSIC_IN_WATCH)  */
    {
        if (status) {
            ui_pic_show_image_by_id(MUSIC_PP_PIC, 1);
        } else {
            ui_pic_show_image_by_id(MUSIC_PP_PIC, 0);
        }
    }


    return 0;
}

static int music_file_status(const char *type, u32 arg)
{
    ui_show(MUSIC_NOFILE_LAYOUT);
    return 0;
}

static const struct uimsg_handl ui_msg_handler[] = {
    { "music_start",        music_start     }, /* 音乐播放 */
    { "music_status",        music_status     }, /* 音乐状态 */
    { "music_file",        	music_file_status }, /* 音乐文件状态 */
    { NULL, NULL},      /* 必须以此结尾！ */
};


static int music_mode_onchange(void *ctr, enum element_change_event e, void *arg)
{
    struct window *window = (struct window *)ctr;
    switch (e) {
    case ON_CHANGE_INIT:
        puts("\n***music_mode_onchange***\n");
        /*
         * 注册APP消息响应
         */
        if (!__this) {
            __this =  zalloc(sizeof(MUSIC_UI_VAR));
        }

        ui_register_msg_handler(window->elm.id, ui_msg_handler);//注册消息交互的回调
        break;
    case ON_CHANGE_RELEASE:
        puts("\n***music_mode_release***\n");
        if (__this) {
            if (__this->timer) {
                sys_timer_del(__this->timer);
                __this->timer = 0;
            }
            free(__this);
            __this = NULL;
        }
        break;
    default:
        return false;
    }
    return false;
}



REGISTER_UI_EVENT_HANDLER(PAGE_10)
.onchange = music_mode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


#if TCFG_USER_EMITTER_ENABLE

static int music_set_ontouch(void *ctr, struct element_touch_event *e)
{
    log_info("__FUNCTION__ = %s %d\n", __FUNCTION__, e->event);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_MUSIC_SET);
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(MUSIC_SET_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_set_ontouch,
};

#endif /* #if TCFG_USER_EMITTER_ENABLE */



#endif
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

