#ifndef __A2DP_SYCNTS__H
#define __A2DP_SYCNTS__H
#include "asm/dac.h"
#include "media/includes.h"
#include "asm/gpio.h"
#include "media/audio_syncts.h"
#include "media/bt_audio_timestamp.h"


struct decoder_syncts {
    void *syncts;
    void *ts_handle;
    void *plc_ops;
    void *plc_mem;
    void *dec;
    u32 timestamp;
    u32 time_base;
    u32 tar_time_base;
    u32 sample_rate;
    u32 out_sample_rate;
    u8 ts_start;
    u8 sync_step;
    u8 ch_num;
    u8 out_ch_num;
    u8 module_start;
    u8 bit24_mode_en;
    u8 way_type;
    u8 remain;
    u8 frame_time;
    u8 plc_en;

    s16 *out_buf;
    u32 buf_len;
    u32 total_len;
    u32 out_len;
    int (*snd_pcm_buffered_len)(void *priv);
    void *sound_pcm;
    struct audio_stream_entry entry;	// 音频流入口
};


int a2dp_decoder_syncts_setup(struct decoder_syncts *hdl, u32 in_sample_rate, u32 out_sample_rate, u8 ch_num);
void a2dp_decoder_syncts_free(struct decoder_syncts *hdl);

int a2dp_decoder_set_timestamp(struct audio_decoder *decoder, u16 seqn);
int a2dp_decoder_stream_is_available(struct audio_decoder *decoder);
int a2dp_decoder_audio_sync_handler(struct audio_decoder *decoder);
void a2dp_decoder_share_timestamp(struct audio_decoder *decoder);

int  decoder_syncts_frame_num_calc(void *priv, u32 bytes);
int  decoder_syncts_frame_num_calc_start(void *priv);
int  decoder_syncts_set_base_time(void *priv, u32 base_time);//esco use


void a2dp_decoder_sample_detect_setup(struct audio_decoder *decoder);
void a2dp_decoder_sample_detect_free(struct audio_decoder *decoder);


void a2dp_decoder_delay_time_setup(struct audio_decoder *decoder);
void a2dp_decoder_delay_time_free(struct audio_decoder *decoder);

int a2dp_stream_error_filter(struct audio_decoder *decoder, u8 new_packet, u8 *packet, int len);
int a2dp_stream_underrun_handler(struct audio_decoder *decoder, u8 **packet);
int a2dp_stream_overrun_handler(struct audio_decoder *decoder, u8 **frame, int *len);

int a2dp_stream_missed_handler(struct audio_decoder *decoder, u8 **frame, int *len);

void a2dp_decoder_stream_free(struct audio_decoder *decoder, void *packet);

void a2dp_syncts_set_sound_pcm_handler(struct decoder_syncts *hdl, void *priv, int (*snd_pcm_buffered_len)(void *));

extern const int CONFIG_LOW_LATENCY_ENABLE;
extern const int CONFIG_A2DP_DELAY_TIME;
extern const int CONFIG_A2DP_DELAY_TIME_LO;
extern const int CONFIG_A2DP_SBC_DELAY_TIME_LO;
extern const int CONFIG_A2DP_DELAY_TIME;
extern const int CONFIG_BTCTLER_TWS_ENABLE;


#endif




