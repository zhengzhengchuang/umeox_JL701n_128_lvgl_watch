
#ifndef _AUDIO_DEC_BT_H_
#define _AUDIO_DEC_BT_H_

#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "media/audio_decoder.h"


struct a2dp_dec_params {
    int media_type;     //媒体类型
    u8 ch_num; 			//通道数
    void *output_priv;                      //解码输出私有参数
    int (*output_handler)(void *output_priv, s16 *data, int len);//解码pcm数据输出接口
    void *format_priv ;//同步私有参数
    void (*format_sync)(void *priv, int sample_rate, u8 channel_num); //解码参数更新到同步接口
};

// a2dp或者esco正在播放
u8 bt_audio_is_running(void);
// a2dp正在播放
u8 bt_media_is_running(void);
// esco正在播放
u8 bt_phone_dec_is_running();;

// 打开a2dp解码
int a2dp_dec_open(int media_type);
// 关闭a2dp解码
int a2dp_dec_close();

// 打开esco解码
int esco_dec_open(void *, u8);
// 关闭esco解码
void esco_dec_close();

// a2dp传输中止
void __a2dp_drop_frame(void *p);

#endif

