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
#include "bt/bt_emitter.h"
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



#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)

#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_USER_EMITTER_ENABLE
#if TCFG_UI_ENABLE_MUSIC_MENU

#define STYLE_NAME  JL


#define P_MUSIC_IN_WATCH         1
#define P_MUSIC_IN_PHONE         2
#define P_MUSIC_IN_EARPHONE      3
#define P_MUSIC_IN_DEL      	 4

typedef struct _MUSIC_UI_VAR_SET {
    u8 play_state: 1;
    u8 play_dev: 3;
} MUSIC_UI_VAR_SET;

static MUSIC_UI_VAR_SET music_hd = {
#if TCFG_APP_MUSIC_EN
    .play_dev = P_MUSIC_IN_WATCH,
#else /* #if TCFG_APP_MUSIC_EN */
    .play_dev = P_MUSIC_IN_PHONE,
#endif /* #if TCFG_APP_MUSIC_EN */
};

#define __this (&music_hd)



extern u8 *get_cur_connect_emitter_mac_addr(void);
static int music_children_redraw(int id);

static int music_get_play_dev()
{
    if (!__this) {
        return 0;
    }
    return __this->play_dev;
}



static int music_set_play_dev(u8 dev)
{
    if (!__this) {
        return false;
    }
    /* if(__this->play_dev == dev) */
    /* return false; */
    /*  */
    switch (dev) {
    case P_MUSIC_IN_WATCH://手表播放
#if TCFG_APP_MUSIC_EN
        __this->play_dev = dev;
        bt_set_music_device_style(__this->play_dev);

        if (app_get_curr_task() != APP_MUSIC_TASK) {
            app_task_switch_to(APP_MUSIC_TASK);
        }

        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_MUSIC);

        /* if (get_cur_connect_emitter_mac_addr()) { //连接了耳机 */
        /*     bt_emitter_audio_set_mute(1);//发射出声 */
        /*     bt_emitter_pp(1); */
        /* } else { */
        /*     bt_emitter_audio_set_mute(0); */
        /* } */
        bt_set_a2dp_en_status(0);
#endif /* #if TCFG_APP_MUSIC_EN */
        break;
    case P_MUSIC_IN_PHONE://手机播放
        __this->play_dev = dev;
        bt_set_music_device_style(__this->play_dev);

        if (app_get_curr_task() != APP_BT_TASK) {
            bt_task_set_window_id(ID_WINDOW_MUSIC);
            app_task_switch_to(APP_BT_TASK);
            /* if (music_player_get_play_status() == FILE_DEC_STATUS_PLAY) { */
            /*     app_task_put_key_msg(KEY_MUSIC_PP, 0); */
            /* } */
        }

        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_MUSIC);

        /* musi
         * c_player_stop(0); */
        /* bt_emitter_pp(0);//暂停发射 */
        /* bt_emitter_audio_set_mute(0); */
        bt_set_a2dp_en_status(1);
        break;
    case P_MUSIC_IN_EARPHONE:
        /* if (__this->play_dev != P_MUSIC_IN_WATCH) { */
        /* break; */
        /* } */
        /* __this->play_dev = P_MUSIC_IN_WATCH; */
        /* bt_set_music_device_style(__this->play_dev); */
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(ID_WINDOW_PAGE);
        return true;
        break;
    case P_MUSIC_IN_DEL:
        ui_hide(BT_MUSIC_SET);
        ui_show(MUSIC_DEL_FILE);
        return true;
        break;
    default:
        return false;
        break;
    }
    music_children_redraw(BT_SETTING_MENU);
    return true;
}


static int grid_child_cb(void *_ctrl, int id, int type, int index)
{
    if (!__this) {
        return false;
    }

    switch (type) {
    case CTRL_TYPE_PROGRESS:
        break;
    case CTRL_TYPE_MULTIPROGRESS:
        break;
    case CTRL_TYPE_TEXT:
        struct ui_text *text = (struct ui_text *)_ctrl;
        if (!strcmp(text->source, "status")) {
            {
                ui_text_set_index(text, !!(get_cur_connect_emitter_mac_addr()));
            }
        }
        break;
    case CTRL_TYPE_NUMBER:
        struct ui_number *number = (struct ui_number *)_ctrl;
        break;
    case CTRL_TYPE_PIC://图片控件因为没有添加数据源 这里使用一些投机方式 即只有一个图片是有列表的
        struct ui_pic *pic = (struct ui_pic *)_ctrl;
        u8 dev = music_get_play_dev();
        ui_pic_set_image_index(pic, (dev == index));
        break;
    case CTRL_TYPE_TIME:
        break;
    }
    return 0;
}


static int music_children_init(struct ui_grid *grid)
{
    struct element *k;
    if (!grid) {
        return 0;
    }

    for (int i = 0; i < grid->avail_item_num; i++) {
        list_for_each_child_element(k, &grid->item[i].elm) {
            grid_child_cb(k, k->id, ui_id2type(k->id), i);
        }
    }

    if (music_get_play_dev() == P_MUSIC_IN_WATCH) { //播放手表显示最后一项目
        grid->item[grid->avail_item_num - 1].elm.css.invisible = 0;
    } else {
        grid->item[grid->avail_item_num - 1].elm.css.invisible = 1;
    }
    return 0;
}

static int music_children_redraw(int id)
{
    struct ui_grid *grid = (struct ui_grid *)ui_core_get_element_by_id(id);
    if (!grid) {
        return 0;
    }
    music_children_init(grid);
    ui_core_redraw(&grid->elm);
    return 0;
}

static void music_grid_touch_move_callback(void *priv)
{
    struct ui_grid *grid = priv;
    if (music_get_play_dev() == P_MUSIC_IN_WATCH) { //播放手表显示最后一项目
        grid->item[grid->avail_item_num - 1].elm.css.invisible = 0;
    } else {
        grid->item[grid->avail_item_num - 1].elm.css.invisible = 1;
    }
    ui_core_redraw(&grid->elm);
}

static int music_grid_touch_move(struct ui_grid *grid, struct element_touch_event *e)
{
    int ret = 0;
    int x = e->pos.x - grid->pos.x;
    int y = e->pos.y - grid->pos.y;

    if (!grid->pix_scroll) {
        return false;
    }

    ret = ui_grid_slide_with_callback(grid, SCROLL_DIRECTION_UD, y, music_grid_touch_move_callback);
    if (ret) {
        grid->pos.x = e->pos.x;
        grid->pos.y =  e->pos.y;
    }
    return ret;
}


static int menu_enter_onchane(void *ctr, enum element_change_event e, void *arg)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    switch (e) {
    case ON_CHANGE_INIT_PROBE:
        ui_grid_set_item_num(grid, 4);//hide BT_PLAY_DEL_LAYOUT
        break;
    case ON_CHANGE_INIT:
        ui_grid_set_slide_direction(grid, SCROLL_DIRECTION_UD);
        __this->play_dev = bt_get_music_device_style();
        if (__this->play_dev == P_MUSIC_IN_WATCH) {
            if (music_player_get_play_status() == FILE_DEC_STATUS_PLAY) {
                ui_show(BT_PLAY_DEL_LAYOUT);
            }
        }
        break;
    case ON_CHANGE_RELEASE:
        break;
    case ON_CHANGE_FIRST_SHOW:
        music_children_init(grid);
        break;
    default:
        return false;
    }
    return false;
}





static int menu_switch_ontouch(void *ctr, struct element_touch_event *e)
{
    struct ui_grid *grid = (struct ui_grid *)ctr;
    int sel_item;
    static u8 move_flag = 0;
    switch (e->event) {
    case ELM_EVENT_TOUCH_L_MOVE:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        move_flag = 1;
        return music_grid_touch_move(grid, e);
        return false;//不接管消息
        break;
    case ELM_EVENT_TOUCH_DOWN:
        move_flag = 0;
        return false;//不接管消息
        break;
    case ELM_EVENT_TOUCH_UP:
        log_info("line:%d", __LINE__);
        if (move_flag) {
            move_flag = 0;
            return false;//不接管消息
        }
        int sel_item = ui_grid_touch_item(grid);
        if (sel_item == -1) {
            move_flag = 0;
            return false;//不接管消息
        }
        music_set_play_dev(sel_item);
        break;
    default:
        return false;
        break;
    }
    return true;//接管消息
}





REGISTER_UI_EVENT_HANDLER(BT_SETTING_MENU)
.onchange = menu_enter_onchane,
 .onkey = NULL,
  .ontouch = menu_switch_ontouch,
};



static int music_del_yes_ontouch(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s \n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        if (__this->play_dev == P_MUSIC_IN_WATCH) {
            if (music_player_get_play_status() == FILE_DEC_STATUS_PLAY) {
                app_task_put_key_msg(KEY_MUSIC_DELETE_FILE, 0);
                UI_HIDE_CURR_WINDOW();
                UI_SHOW_WINDOW(ID_WINDOW_MUSIC);
                return true;
            }
        }
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(MUSIC_DEL_FILE_YES)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_del_yes_ontouch,
};


static int music_del_no_ontouch(void *ctr, struct element_touch_event *e)
{
    printf("__FUNCTION__ = %s \n", __FUNCTION__);
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:

        break;
    case ELM_EVENT_TOUCH_UP:
        ui_hide(MUSIC_DEL_FILE);
        ui_show(BT_MUSIC_SET);
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(MUSIC_DEL_FILE_NO)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = music_del_no_ontouch,
};










#endif
#endif /* #if TCFG_USER_EMITTER_ENABLE */
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

