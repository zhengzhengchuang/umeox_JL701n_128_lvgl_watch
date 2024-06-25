#ifndef __AUDIO_CH_SWAP__H
#define __AUDIO_CH_SWAP__H

#include "media/audio_stream.h"

#ifndef RUN_NORMAL
#define RUN_NORMAL  0
#endif

#ifndef RUN_BYPASS
#define RUN_BYPASS  1
#endif

struct aud_ch_swap {
    u32 swap_name;
    u8 channel;//通道数
    u8 indata_inc;//2;
    u8 bit_wide;//0:16bit  1：32bit
};

struct audio_ch_swap {
    struct audio_stream_entry entry;	// 音频流入口
    struct list_head hentry;                         //
    struct aud_ch_swap parm;
    u8 status;                           //内部运行状态机
    u8 update;                           //设置参数更新标志
};

struct audio_ch_swap *audio_ch_swap_process_open(struct aud_ch_swap *parm);
void audio_ch_swap_run(struct audio_ch_swap *hdl, s16 *data, int len);
void audio_ch_swap_close(struct audio_ch_swap *hdl);
void audio_ch_swap_bypass(u32 swap_name, u8 bypass);
struct audio_ch_swap *get_cur_swap_hdl_by_name(u32 swap_name);

/*
*声道互换接口，只支持立体声
*/
void TwoChannelSwap_16Bit(short *in, short *out, int channel, int IndataInc, int per_channel_npoint);
void TwoChannelSwap_32Bit(int *in, int *out, int channel, int IndataInc, int per_channel_npoint);
#endif
