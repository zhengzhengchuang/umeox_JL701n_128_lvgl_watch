#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_music.h"
#include "lua_bsp.h"

#include "generic/typedef.h"
#include "app_config.h"
#include "app_main.h"
#include "app_task.h"
#include "key_event_deal.h"
#include "audio_config.h"
#include "btstack/avctp_user.h"
#include "audio_dec.h"
#include "audio_dec_file.h"
#include "music/music_player.h"
#include "music/music.h"
#include "bt/bt.h"
#include "font/font_all.h"

#if (TCFG_LUA_ENABLE)


#define IS_BT_MODE()		(bt_get_music_device_style() == SET_MUSIC_IN_PHONE)
#define IS_MUSIC_MODE()		(bt_get_music_device_style() == SET_MUSIC_IN_WATCH)


/* music 库设置和状态记录 */
typedef struct __MusicModuleSet {
    u8 play_dev;	// 音乐播放设备
} MusicModuleSet;

static MusicModuleSet music_hd = {
    /* 默认音乐播放设备为手表音乐 */
    .play_dev = MUSIC_IN_WATCH,
};
#define __this (&music_hd)


/* 音乐播放控制
 * 控制参数，参考头文件的枚举
 * */
static int music_controlMusicPlay(lua_State *L)
{
    int control_arg = lua_tointeger(L, 2);
    lua_db(">>>>> %s, %d: control arg: %d\n", __FUNCTION__, __LINE__, control_arg);

    switch (control_arg) {
    case MUSIC_PLAY_START:
        if (IS_BT_MODE()) {
            if ((get_call_status() == BT_CALL_ACTIVE) ||
                (get_call_status() == BT_CALL_OUTGOING) ||
                (get_call_status() == BT_CALL_ALERT) ||
                (get_call_status() == BT_CALL_INCOMING)) {
                ;//通话过程
            } else {
                if (a2dp_get_status() != BT_MUSIC_STATUS_STARTING) {
                    if (app_get_curr_task() != APP_BT_TASK) {
                        app_task_switch_to(APP_BT_TASK);
                    }
                    user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PLAY, 0, NULL);
                }
            }
        } else if (IS_MUSIC_MODE()) {
            if (music_player_get_play_status() != FILE_DEC_STATUS_PLAY) {
                if (app_get_curr_task() != APP_MUSIC_TASK) {
                    music_set_start_auto_play(1);
                    app_task_switch_to(APP_MUSIC_TASK);
                } else {
                    app_task_put_key_msg(KEY_MUSIC_PP, 0);
                }
            }
        }
        break;
    case MUSIC_PLAY_INDEX:
        u32 sclust = lua_tointeger(L, 3);
        lua_db("open music file sclust:%d\n", sclust);
        if (app_get_curr_task() != APP_MUSIC_TASK) {
            music_task_set_parm(MUSIC_TASK_START_BY_SCLUST, sclust);
            music_set_start_auto_play(1);
            app_task_switch_to(APP_MUSIC_TASK);
        } else {
            app_task_put_key_msg(KEY_MUSIC_PLAYE_BY_DEV_SCLUST, sclust);
        }
        break;
    case MUSIC_PLAY_PAUSE:
    case MUSIC_PLAY_STOP:
        if (IS_BT_MODE()) {
            if ((get_call_status() == BT_CALL_ACTIVE) ||
                (get_call_status() == BT_CALL_OUTGOING) ||
                (get_call_status() == BT_CALL_ALERT) ||
                (get_call_status() == BT_CALL_INCOMING)) {
                ;//通话过程
            } else {
                if (a2dp_get_status() == BT_MUSIC_STATUS_STARTING) {
                    if (app_get_curr_task() == APP_BT_TASK) {
                        user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PLAY, 0, NULL);
                    }
                }
            }
        } else if (IS_MUSIC_MODE()) {
            if (music_player_get_play_status() == FILE_DEC_STATUS_PLAY) {
                app_task_put_key_msg(KEY_MUSIC_PP, 0);
            }
        }
        break;
    case MUSIC_PLAY_LAST:
        if (IS_BT_MODE()) {
            user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_PREV, 0, NULL);
        } else if (IS_MUSIC_MODE()) {
            app_task_put_key_msg(KEY_MUSIC_PREV, 0);
        }
        break;
    case MUSIC_PLAY_NEXT:

        if (IS_BT_MODE()) {
            user_send_cmd_prepare(USER_CTRL_AVCTP_OPID_NEXT, 0, NULL);
        } else if (IS_MUSIC_MODE()) {
            app_task_put_key_msg(KEY_MUSIC_NEXT, 0);
        }
        break;
    case MUSIC_PLAY_DELETE:
        // 删除歌曲
        lua_db("The function of deleting songs is not implemented\n");
        break;
    default:
        printf("ERROR! unknow music play control arg\n");
        break;
    }

    return 0;
}

// 需要根据列表构建数组返回
// 获取音乐列表
// 如果没传参数，一次获取所有歌曲文件，如果传了参数，则以参数为页，按页获取
// (start_index, number) --> 从start_index开始获取number个歌曲名称
static int music_loadMusicList(lua_State *L)
{
    int number = lua_tointeger(L, -1);
    int start_index = lua_tointeger(L, -2);
    lua_pop(L, 2);

    lua_db(">> music << %s, start_index:%d, number:%d\n", __FUNCTION__, start_index, number);

    void *dev = dev_manager_find_active(1);
    if (!dev) {
        return 0;
    }

    FILE *music_file = NULL;
    FS_DIR_INFO *music_list = zalloc(sizeof(FS_DIR_INFO) * number);
    fset_ext_type(dev_manager_get_root_path(dev), "MP1MP2MP3");
    fopen_dir_info(dev_manager_get_root_path(dev), &music_file, 0);
    if (!music_file) {
        free(music_list);
        return 0;
    }

    int cur_total = fenter_dir_info(music_file, music_list);
    printf("cur_total: %d\n", cur_total);
    if (start_index > cur_total) {
        printf("music only %d\n", cur_total);
    }

    int get_number = number;
    if ((start_index + number) > cur_total) {
        get_number = cur_total - start_index;
    }
    fget_dir_info(music_file, start_index, get_number, music_list);

    lua_newtable(L);
    for (int k = 0; k < get_number; k++) {
        lua_pushinteger(L, k + 1);
        lua_newtable(L);

        // 文件名
        lua_pushstring(L, "name");
        if (music_list[k].fn_type) {
            // 长文件名
            char *name = (char *)malloc(music_list[k].lfn_buf.lfn_cnt + 2);
            memcpy(name, music_list[k].lfn_buf.lfn, music_list[k].lfn_buf.lfn_cnt);
            lua_pushinteger(L, (u32)name);
            put_buf(name, music_list[k].lfn_buf.lfn_cnt);
        } else {
            // 短文件名 8 + 3
            char name[13];
            memcpy(name, music_list[k].lfn_buf.lfn, 11);
            name[12] = '\0';
            lua_pushstring(L, name);
            printf("name: %s\n", name);
        }
        lua_settable(L, -3);

        // 文件名长度
        lua_pushstring(L, "len");
        lua_pushinteger(L, music_list[k].lfn_buf.lfn_cnt);
        lua_settable(L, -3);

        // 文件簇
        lua_pushstring(L, "sclust");
        lua_pushinteger(L, music_list[k].sclust);
        lua_settable(L, -3);

        // 文件夹或文件
        lua_pushstring(L, "dir_type");
        lua_pushinteger(L, music_list[k].dir_type);
        lua_settable(L, -3);

        // 长短文件名
        lua_pushstring(L, "fn");
        lua_pushinteger(L, music_list[k].fn_type);
        lua_settable(L, -3);

        lua_settable(L, -3);
    }
    free(music_list);

    return 1;
}


// 释放音乐列表的name buffer
static int music_freeMusicList(lua_State *L)
{
    int index = 1;

    while (1) {
        // 从索引1开始找
        lua_pushinteger(L, index++);
        lua_gettable(L, -2);

        int type = lua_type(L, -1);
        if (type == LUA_TTABLE) {
            // 如果是table，查找看有没有name
            lua_pushstring(L, "name");
            lua_gettable(L, -2);

            int t = lua_type(L, -1);
            /* printf("$$$$$$$$$$$$$$$ t:%d\n", t); */
            /* printf("> %d\n", lua_isnumber(L, -1)); */
            if (t == LUA_TNUMBER) {
                // 如果name的值是数值，那代表它是地址。否则是普通字符串，不处理
                char *addr = (char *)lua_tointeger(L, -1);
                free(addr);
                lua_db("free index: %d, addr: 0x%x\n", index, addr);
            } else {
                lua_db("is name: %s\n", lua_tostring(L, -1));
            }
            lua_pop(L, 1);
        } else if (type == LUA_TNIL) {
            // 一直找到索引值为空的时候退出
            break;
        }
        lua_pop(L, 1);
    }
    return 0;
}



// 获取歌曲信息
// 参数：传入参数时只获取歌曲名、这个功能调用场景多
// 返回值：歌曲名、播放设备、播放模式、播放状态
static int music_getMusicPlayInfo(lua_State *L)
{
    int n = lua_gettop(L);

    int len = 0;
    int is_unicode = FONT_DEFAULT;
    char *file_name = NULL;

    if (bt_get_music_device_style() == SET_MUSIC_IN_WATCH) {
        extern const char *music_file_get_cur_name(int *len, int *is_unicode);
        file_name = music_file_get_cur_name(&len, &is_unicode);
        if (!(len && file_name)) {
            file_name = "SD music";
            is_unicode = 0;
        }
    } else {
        file_name = "BT music";
        is_unicode = 0;
    }
    lua_db("%s, file name:%s, is_unicode:%d\n", __FUNCTION__, file_name, is_unicode);
    // 歌曲名入栈
    if (2 == n) {
        /* 如果有参数传进来，这里就直接返回歌曲名称 */
        lua_pop(L, 1);
        lua_pushstring(L, file_name);
        if (is_unicode) {
            lua_pushinteger(L, len);
            lua_pushinteger(L, FONT_ENCODE_UNICODE);
        } else {
            len = strlen(file_name);
            lua_pushinteger(L, len);
            lua_pushinteger(L, FONT_ENCODE_ANSI);
        }
        return 3;
    }

    /* 歌曲名入栈 */
    lua_pushstring(L, file_name);

    /* 播放设备入栈 */
    lua_pushinteger(L, __this->play_dev);

    /* 播放模式入栈 */
    int play_mode = music_player_get_repeat_mode();
    lua_pushinteger(L, play_mode);

    /* 播放状态入栈 */
    int play_state = 0;
    if (IS_BT_MODE()) {
        if (a2dp_get_status() == BT_MUSIC_STATUS_STARTING) {
            play_state = 1;
        }
    } else if (IS_MUSIC_MODE()) {
        if (music_player_get_play_status() == FILE_DEC_STATUS_PLAY) {
            play_state = 1;
        }
    }
    lua_pushboolean(L, play_state);
    return 4;
}

// 设置播放信息
// （模式，设备）
static int music_setMusicPlayInfo(lua_State *L)
{
    int type = lua_type(L, 2);
    if (type == LUA_TNUMBER) {
        if (IS_MUSIC_MODE()) {
            int play_mode = lua_tointeger(L, 2);
            music_player_set_repeat_mode(play_mode);
        }
    }

    type = lua_type(L, 3);
    if (type == LUA_TNUMBER) {
        int play_dev = lua_tointeger(L, 3);
        __this->play_dev = play_dev;
        bt_set_music_device_style(__this->play_dev);

        /* 切换APP */
        if (__this->play_dev == MUSIC_IN_WATCH) {
            if (app_get_curr_task() != APP_MUSIC_TASK) {
                app_task_switch_to(APP_MUSIC_TASK);
            }
            bt_set_a2dp_en_status(0);
        } else if (__this->play_dev == MUSIC_IN_PHONE) {
            if (app_get_curr_task() != APP_BT_TASK) {
                app_task_switch_to(APP_BT_TASK);
            }
            bt_set_a2dp_en_status(1);
        }
    }
    return 0;
}


// music module
const luaL_Reg music_method[] = {
    /*
     * 音乐控制需要方法：
     * 		开始播放音乐
     * 		播放指定音乐
     * 		暂停播放音乐
     * 		停止播放音乐
     * 		播放上一首
     * 		播放下一首
     * 		controlMusicPlay()
     *
     * 		获取音乐列表，扫描多级文件夹并把MP3文件和文件夹扫描出来
     * 		loadMusicList()
     * 		释放音乐列表（由于中文字符串显示需申请buffer，目前必须通过手动释放申请的buffer）
     * 		freeMusicList()
     *
     * 		获取当前歌曲名称（会经常调用，如打开页面时、切换歌曲时、耳机控制播歌时）
     * 		getMusicPlayInfo()	获取音乐播放信息，当前歌曲名、设备、模式、播放状态
     * 		setMusicPlayInfo(模式, 设备)：对不设置的内容传入nil
     *
     * */
    {"controlMusicPlay",	music_controlMusicPlay},

    {"loadMusicList",		music_loadMusicList},
    {"freeMusicList",		music_freeMusicList},

    {"getMusicPlayInfo",	music_getMusicPlayInfo},
    {"setMusicPlayInfo",	music_setMusicPlayInfo},

    {NULL, NULL}
};


// 注册music模块
LUALIB_API int luaopen_music(lua_State *L)
{
    luaL_newlib(L, music_method);
    return 1;
}

#endif /* #if (TCFG_LUA_ENABLE) */

