#ifndef __LUA_MUSIC_H__
#define __LUA_MUSIC_H__



// 播放设备为手表
#define MUSIC_IN_WATCH         1

// 播放设备为手机
#define MUSIC_IN_PHONE         2

// 播放设备为耳机
#define MUSIC_IN_EARPHONE      3


// 音乐播放控制参数
typedef enum {
    // 开始播放音乐
    MUSIC_PLAY_START = 1,

    // 播放指定索引音乐
    MUSIC_PLAY_INDEX = 2,

    // 暂停播放音乐
    MUSIC_PLAY_PAUSE = 3,

    // 停止播放音乐
    MUSIC_PLAY_STOP,

    // 播放上一曲
    MUSIC_PLAY_LAST,

    // 播放下一曲
    MUSIC_PLAY_NEXT,

    // 删除音乐
    MUSIC_PLAY_DELETE,

    MUSIC_PLAY_UNKNOW
} MUSIC_PLAY_CONTROL;




#endif

