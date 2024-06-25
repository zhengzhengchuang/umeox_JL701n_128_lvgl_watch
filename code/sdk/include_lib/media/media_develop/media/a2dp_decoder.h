#ifndef A2DP_DECODER_H
#define A2DP_DECODER_H


#include "media/includes.h"



#define A2DP_CODEC_SBC			0x00
#define A2DP_CODEC_MPEG12		0x01
#define A2DP_CODEC_MPEG24		0x02


#define A2DP_STREAM_NO_ERR                  0
#define A2DP_STREAM_UNDERRUN                1
#define A2DP_STREAM_OVERRUN                 2
#define A2DP_STREAM_MISSED                  3
#define A2DP_STREAM_DECODE_ERR              4
#define A2DP_STREAM_LOW_UNDERRUN            5


struct a2dp_decoder {
    u8 start;			// 解码开始
    u8 ch;				// 解码声道数
    u8 output_ch_num;	// 解码输出声道数
    u8 output_ch_type;	// 解码输出声道类型
    u8 header_len;		// 帧头长度
    u8 sync_step;
    u8 fetch_lock;
    u8 join_tws;
    u8 state;
    u8 wait_bt_resume;
    u8 stream_error;
    u8 underrun_feedback;
    u8 new_frame;
    u8 repair;
    u8 preempt;
    u16 seqn;			// 帧序号
    s16 drop_samples;
    u16 sample_rate;
    u16 resume_tmr_id;
    u32 resume_time;
    u32 pending_time;
    int wait_time;
    int delay_time;
    int coding_type;	// 解码类型
    enum audio_channel ch_type;	// 声道类型
    struct audio_decoder decoder;	// 解码器
    void *sync;
    void *syncts;
    void *prev_packet;
    int prev_packet_len;

    void *sample_detect;
    void *repair_pkt;
    s16 repair_pkt_len;
    u16 missed_num;
    u16 repair_frames;
    u16 overrun_seqn;
    u16 detect_timer;
    u16 slience_frames;
};

// 打开a2dp解码
int a2dp_decoder_open(struct a2dp_decoder *, struct audio_decoder_task *decode_task);
// 关闭a2dp解码
void a2dp_decoder_close(struct a2dp_decoder *dec);
// 设置a2dp解码输出声道
void a2dp_decoder_set_output_channel(struct a2dp_decoder *dec, u8 ch_num, u8 ch_type);
// 开始a2dp传输中止
void a2dp_drop_frame_start();
// 停止a2dp传输中止
void a2dp_drop_frame_stop();

void a2dp_decoder_stream_syncts_enable(struct a2dp_decoder *dec, void *syncts, int sample_rate, int delay_time);
void a2dp_decoder_stream_sync_enable(struct a2dp_decoder *dec, void *sync, int sample_rate, int delay_time);

void a2dp_decoder_join_tws(struct a2dp_decoder *dec);

void a2dp_decoder_resume_from_bluetooth(struct a2dp_decoder *dec);



int get_rtp_header_len(u8 new_frame, u8 *buf, int len);



#endif
