#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "media/a2dp_decoder.h"
#include "media/esco_decoder.h"
#include "classic/tws_api.h"
#include "app_config.h"
#include "audio_config.h"
#include "audio_dec.h"
#include "audio_digital_vol.h"
#include "clock_cfg.h"
#include "btstack/a2dp_media_codec.h"
#include "media/audio_eq_drc_apply.h"
#include "media/audio_equalloudness.h"
#include "media/audio_vbass.h"
#include "aec_user.h"
#include "audio_enc.h"
#include "bt_tws.h"
#include "ui/ui_api.h"
#include "a2dp_syncts.h"
#include "esco_syncts.h"
#include "audio_way.h"
#include "media/convert_data.h"
#include "audio_effect/audio_dynamic_eq_demo.h"
#include "media/effects_adj.h"
#include "app_main.h"
#include "audio_effect/audio_eff_default_parm.h"

#if TCFG_ESCO_PLC
#include "PLC.h"
#define PLC_FRAME_LEN	60
#endif/*TCFG_ESCO_PLC*/

#if TCFG_SMART_VOICE_ENABLE
#include "smart_voice/smart_voice.h"
#include "media/jl_kws.h"
#if TCFG_CALL_KWS_SWITCH_ENABLE
#include "btstack/avctp_user.h"
#endif
#endif


#if TCFG_ESCO_NOISEGATE
/*小于CONST_NOISE_GATE的当成噪声处理,防止清0近端声音*/
#define LIMITER_NOISE_GATE  -55000 /*-12000 = -12dB,放大1000倍,(-30000参考)*/
/*低于噪声门限阈值的增益 */
#define LIMITER_NOISE_GAIN  (0 << 30) /*(0~1)*2^30*/

#endif //TCFG_ESCO_NOISEGATE

#define AUDIO_CODEC_SUPPORT_SYNC	1 // 同步


#ifndef  TCFG_PHONE_DRC_ENABLE
#define TCFG_PHONE_DRC_ENABLE 0
#endif
#define ESCO_DRC_EN	TCFG_PHONE_DRC_ENABLE  //通话下行增加限幅器处理，默认关闭,need en TCFG_DRC_ENABLE

#if (!TCFG_DRC_ENABLE)
#undef ESCO_DRC_EN
#define ESCO_DRC_EN	0
#endif
#define TCFG_ESCO_USE_SPEC_MIX_LEN		0

extern const int CONFIG_A2DP_DELAY_TIME;
const int CONFIG_ESCO_DELAY_TIME = 50;
extern const int config_mixer_en;

#define AUDIO_DEC_BT_MIXER_EN		config_mixer_en



/*
 * 解码输出变双声道。不打开的时候使用mixer中的声道转换。
 * mixer中的声道转换是通用型的，支持任意转换，但效率没有单独的单声道转双声道的好
 */
#define ESCO_DEC_OUT_DUAL		1


//////////////////////////////////////////////////////////////////////////////


struct a2dp_dec_hdl {
    struct a2dp_decoder dec;		// a2dp解码句柄
    struct audio_res_wait wait;		// 资源等待句柄
    struct audio_mixer_ch mix_ch;	// 叠加句柄
#if (RECORDER_MIX_EN)
    struct audio_mixer_ch rec_mix_ch;	// 叠加句柄
#endif//RECORDER_MIX_EN

    struct audio_stream *stream;	// 音频流

#if AUDIO_SURROUND_CONFIG
    surround_hdl *surround;         //环绕音效句柄
#endif
#if AUDIO_VBASS_CONFIG
    struct aud_gain_process *vbass_prev_gain;
    NOISEGATE_API_STRUCT *ns_gate;
    vbass_hdl *vbass;               //虚拟低音句柄
#endif
#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
    struct audio_eq  *high_bass;
    struct audio_drc *hb_drc;//高低音后的drc
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    struct convert_data *hb_convert;
#endif
#endif

    struct audio_eq *eq;    //eq drc句柄
    struct audio_drc *drc;    // drc句柄
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    struct convert_data *convert;
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    struct audio_eq *ext_eq;    //eq drc句柄 扩展eq
#endif
#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
    struct audio_eq *eq2;    //eq drc句柄
    struct dynamic_eq_hdl *dy_eq;
    struct convert_data *convert2;
#endif
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    struct aud_gain_process *gain;
#endif

#if AUDIO_CODEC_SUPPORT_SYNC
    struct decoder_syncts syncts;
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/


#if TCFG_EQ_DIVIDE_ENABLE
    struct audio_eq *eq_rl_rr;    //eq drc句柄
    struct audio_drc *drc_rl_rr;    // drc句柄
    struct convert_data *convert_rl_rr;
    struct __stream_entry *rl_rr_mix;//双声道相加除2处理
    struct aud_gain_process *gain_rl_rr;

    struct audio_vocal_tract vocal_tract;//声道合并目标句柄
    struct audio_vocal_tract_ch synthesis_ch_fl_fr;//声道合并句柄
    struct audio_vocal_tract_ch synthesis_ch_rl_rr;//声道合并句柄
    struct channel_switch *ch_switch;//声道变换
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    struct audio_eq *ext_eq2;    //eq drc句柄 扩展eq
#endif
#endif


    u8 closing;
    u8 preempted;
    u8 slience;
    int slience_time;
    u32 slience_end_time;
    struct audio_stream_entry *slience_entry;

    u8 pcm_capture;             //捕捉pcm数据并输出给配置接口
    u8 pcm_ch_num;              //需要的解码输出声道数
    u32 timeout;
    void *output_priv;          //解码输出私有参数
    int (*output_handler)(void *output_priv, s16 *data, int len);//解码pcm数据输出接口
    void *format_priv ;//同步私有参数
    void (*format_sync)(void *priv, int sample_rate, u8 channel_num); //解码参数更新到同步接口

};

struct esco_dec_hdl {
    struct esco_decoder dec;		// esco解码句柄
    struct audio_res_wait wait;		// 资源等待句柄
    struct audio_mixer_ch mix_ch;	// 叠加句柄
#if (RECORDER_MIX_EN)
    struct audio_mixer_ch rec_mix_ch;	// 叠加句柄
#endif//RECORDER_MIX_EN

    struct audio_stream *stream;	// 音频流

    struct audio_eq *eq;    //eq drc句柄
    struct audio_drc *drc;    // drc句柄
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    struct convert_data *convert;//32转16bit
#endif

    struct channel_switch *ch_switch;//声道变换

    u32 tws_mute_en : 1;	// 静音
    u32 remain : 1;			// 未输出完成

#if TCFG_ESCO_PLC
    void *plc;				// 丢包修护
#endif


#if TCFG_ESCO_NOISEGATE
    void *noisegate;			// 噪声抑制
#endif


#if AUDIO_CODEC_SUPPORT_SYNC
    struct decoder_syncts syncts;
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/

};


//////////////////////////////////////////////////////////////////////////////

struct a2dp_dec_hdl *bt_a2dp_dec = NULL;
struct esco_dec_hdl *bt_esco_dec = NULL;

static u8 a2dp_suspend = 0;
static u16 drop_a2dp_timer = 0;

extern s16 mix_buff[];
extern s16 recorder_mix_buff[];

extern struct audio_dac_hdl dac_hdl;
extern struct audio_dac_channel default_dac;

//////////////////////////////////////////////////////////////////////////////

struct audio_wireless_sync *a2dp_output_sync_open(int sample_rate, int output_sample_rate, u8 channel);
void a2dp_output_sync_close(struct audio_wireless_sync *a2dp_sync);
struct audio_wireless_sync *esco_output_sync_open(int sample_rate, int output_sample_rate, u8 channels);
void esco_output_sync_close(struct audio_wireless_sync *esco_sync);

extern int lmp_private_esco_suspend_resume(int flag);



//此函数获取 关机的状态，临时定义
int get_poweroof_status()
{
    return 0;
}
static void bt_dec_stream_run_stop(struct audio_decoder *decoder)
{
#if AUDIO_DAC_MULTI_CHANNEL_ENABLE
    struct audio_data_frame frame = {0};
    struct audio_data_frame output = {0};
    frame.stop = 1;
    frame.channel = audio_output_channel_num();
    frame.sample_rate = app_audio_output_samplerate_get();
    /* audio_stream_run(&default_dac.entry, &frame); */
    default_dac.entry.data_handler(&default_dac.entry, &frame, &output);
    audio_stream_del_entry(&default_dac.entry);
#else /*AUDIO_DAC_MULTI_CHANNEL_ENABLE*/
    audio_dac_stop(&dac_hdl);
#endif /*AUDIO_DAC_MULTI_CHANNEL_ENABLE*/
}

static int audio_dec_slience_data_handler(struct audio_stream_entry *entry,
        struct audio_data_frame *in,
        struct audio_data_frame *out)
{
    out->data = in->data;
    out->data_len = in->data_len;

    if (in->offset == 0) {
        if (bt_a2dp_dec->slience == 0) {
            bt_a2dp_dec->slience_end_time = jiffies + msecs_to_jiffies(bt_a2dp_dec->slience_time);
            memset(in->data, 0, in->data_len);
            bt_a2dp_dec->slience = 1;
        } else if (bt_a2dp_dec->slience == 1) {
            if (time_after(jiffies, bt_a2dp_dec->slience_end_time)) {
                bt_a2dp_dec->slience = 2;
            }
            memset(in->data, 0, in->data_len);
        }
    }
    return in->data_len;
}

static void audio_dec_slience_process_len(struct audio_stream_entry *entry, int len)
{

}

static struct audio_stream_entry *audio_dec_slience_stream_entry(struct a2dp_dec_hdl *dec, int time)
{
    struct audio_stream_entry *entry = zalloc(sizeof(struct audio_stream_entry));
    entry->data_handler = audio_dec_slience_data_handler;
    entry->data_process_len = audio_dec_slience_process_len;
    dec->slience = 0;
    dec->slience_entry = entry;
    dec->slience_time = time;

    return entry;
}

static void audio_dec_slience_entry_free(struct a2dp_dec_hdl *dec)
{
    if (dec->slience_entry) {
        audio_stream_del_entry(dec->slience_entry);
        free(dec->slience_entry);
        dec->slience_entry = NULL;
    }
}


/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码close
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void a2dp_audio_res_close(void)
{
    if (bt_a2dp_dec->dec.start == 0) {
        log_i("bt_a2dp_dec->dec.start == 0");
        return ;
    }

    if (!bt_a2dp_dec->closing) {
        bt_a2dp_dec->preempted = 1;
    }
    bt_a2dp_dec->dec.start = 0;
    a2dp_decoder_close(&bt_a2dp_dec->dec);
#if AUDIO_SURROUND_CONFIG
    surround_close_demo(bt_a2dp_dec->surround);
#endif

#if AUDIO_VBASS_CONFIG
    audio_gain_close_demo(bt_a2dp_dec->vbass_prev_gain);
    audio_noisegate_close_demo(bt_a2dp_dec->ns_gate);
    audio_vbass_close_demo(bt_a2dp_dec->vbass);
#endif
#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
    high_bass_eq_close(bt_a2dp_dec->high_bass);
    high_bass_drc_close(bt_a2dp_dec->hb_drc);
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(bt_a2dp_dec->hb_convert);
#endif
#endif
#if TCFG_EQ_ENABLE &&TCFG_BT_MUSIC_EQ_ENABLE
    music_eq_close(bt_a2dp_dec->eq);
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    music_drc_close(bt_a2dp_dec->drc);
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(bt_a2dp_dec->convert);
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    music_ext_eq_close(bt_a2dp_dec->ext_eq);
#endif

#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
    music_eq2_close(bt_a2dp_dec->eq2);
    audio_dynamic_eq_ctrl_close(bt_a2dp_dec->dy_eq);
    convet_data_close(bt_a2dp_dec->convert2);
#endif
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    audio_gain_close_demo(bt_a2dp_dec->gain);
#endif
#endif

#if TCFG_EQ_DIVIDE_ENABLE

#if TCFG_EQ_ENABLE &&TCFG_BT_MUSIC_EQ_ENABLE
    music_eq_rl_rr_close(bt_a2dp_dec->eq_rl_rr);
    music_drc_rl_rr_close(bt_a2dp_dec->drc_rl_rr);
    convet_data_close(bt_a2dp_dec->convert_rl_rr);
#endif

    audio_gain_close_demo(bt_a2dp_dec->gain_rl_rr);

    audio_vocal_tract_synthesis_close(&bt_a2dp_dec->synthesis_ch_fl_fr);
    audio_vocal_tract_synthesis_close(&bt_a2dp_dec->synthesis_ch_rl_rr);
    audio_vocal_tract_close(&bt_a2dp_dec->vocal_tract);
    channel_switch_close(&bt_a2dp_dec->ch_switch);
#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
    music_ext_eq2_close(bt_a2dp_dec->ext_eq2);
#endif
#endif
#if AUDIO_CODEC_SUPPORT_SYNC
    sound_pcm_device_unmount_syncts(bt_a2dp_dec->syncts.way_type, bt_a2dp_dec->syncts.syncts);
    a2dp_decoder_syncts_free(&bt_a2dp_dec->syncts);
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/
    if (AUDIO_DEC_BT_MIXER_EN) {
        audio_mixer_ch_close(&bt_a2dp_dec->mix_ch);
    } else {
        default_dac.entry.prob_handler = NULL;
        bt_dec_stream_run_stop(&bt_a2dp_dec->dec.decoder);
    }

    audio_dec_slience_entry_free(bt_a2dp_dec);



#if (RECORDER_MIX_EN)
    audio_mixer_ch_close(&bt_a2dp_dec->rec_mix_ch);
#endif//RECORDER_MIX_EN

#if SYS_DIGVOL_GROUP_EN
    sys_digvol_group_ch_close("music_a2dp");
#endif // SYS_DIGVOL_GROUP_EN

    // 先关闭各个节点，最后才close数据流
    if (bt_a2dp_dec->stream) {
        audio_stream_close(bt_a2dp_dec->stream);
        bt_a2dp_dec->stream = NULL;
    }

    app_audio_state_exit(APP_AUDIO_STATE_MUSIC);
}

/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码释放
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void a2dp_dec_release()
{
    audio_decoder_task_del_wait(&decode_task, &bt_a2dp_dec->wait);
    a2dp_drop_frame_stop();

    if (bt_a2dp_dec->dec.coding_type == AUDIO_CODING_SBC) {
        clock_remove(DEC_SBC_CLK);
    } else if (bt_a2dp_dec->dec.coding_type == AUDIO_CODING_AAC) {
        clock_remove(DEC_AAC_CLK);
    }

    local_irq_disable();
    free(bt_a2dp_dec);
    bt_a2dp_dec = NULL;
    local_irq_enable();
}

/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码事件返回
   @param    *decoder: 解码器句柄
   @param    argc: 参数个数
   @param    *argv: 参数
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void a2dp_dec_event_handler(struct audio_decoder *decoder, int argc, int *argv)
{
    switch (argv[0]) {
    case AUDIO_DEC_EVENT_END:
        log_i("AUDIO_DEC_EVENT_END\n");
        a2dp_dec_close();
        break;
    }
}


/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码数据流激活
   @param    *p: 私有句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void a2dp_dec_out_stream_resume(void *p)
{
    struct a2dp_dec_hdl *dec = (struct a2dp_dec_hdl *)p;

    audio_decoder_resume(&dec->dec.decoder);
}

/*----------------------------------------------------------------------------*/
/**@brief    a2dp接受回调
   @param
   @return
   @note	 蓝牙库里面接受到了a2dp音频
*/
/*----------------------------------------------------------------------------*/
void a2dp_rx_notice_to_decode(void)
{
    if (bt_a2dp_dec && bt_a2dp_dec->dec.start) {
        /* a2dp_decoder_resume_from_bluetooth(&bt_a2dp_dec->dec); */
        audio_decoder_resume(&bt_a2dp_dec->dec.decoder);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码输出到dac时的预处理
   @param    *entry: 数据流节点
   @param    *in: 输入数据
   @return
   @note	 应用在没有使用mixer情况下传输sync标志
*/
/*----------------------------------------------------------------------------*/
static int a2dp_to_dac_probe_handler(struct audio_stream_entry *entry,  struct audio_data_frame *in)
{
    if (!in->stop/* && in->data_len*/) {
#if AUDIO_CODEC_SUPPORT_SYNC
        /* if (bt_a2dp_dec->syncts.syncts) { */
        /* in->data_sync = 1; */
        /* } */
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/
    }

    return 0;
}


#if 0
// 解码输出数据流节点数据处理回调示例
static int demo_decoder_data_handler(struct audio_stream_entry *entry,
                                     struct audio_data_frame *in,
                                     struct audio_data_frame *out)
{
    struct audio_decoder *decoder = container_of(entry, struct audio_decoder, entry);
    // 自定义数据处理
    // ...
    put_buf(in->data, 16);
    // 输出到数据流示例。
    // 如果不使用audio_stream流程，输出后，需要把实际输出长度更新到decoder->process_len
    decoder->process_len = 0;
    audio_stream_run(&decoder->entry, in);
    int wlen = decoder->process_len;
    return wlen;
}
#endif

#if 0
// 数据流预处理回调示例
static int demo_stream_mix_prob_handler(struct audio_stream_entry *entry,  struct audio_data_frame *in)
{
    // 该函数中如果返回负数，数据流将截止运行，解码会被挂起
    // 对数据进行处理，如打印一下数据
    /* put_buf(in->data, in->data_len); */
    put_buf(in->data, 16);
    /* memset(in->data, 0, in->data_len); */
    return 0;
}
#endif

#if 0
// 保存原来的数据流数据处理
static void *demo_data_handler_save;
// 数据流data_handler处理
static int demo_new_data_handler(struct audio_stream_entry *entry,
                                 struct audio_data_frame *in,
                                 struct audio_data_frame *out)
{
    // 对数据进行处理
    put_buf(in->data, 8);
    // 调用原来的接口输出，这里就可以保存一下是否输出完，就可以做比较多的处理了
    int wlen = ((int (*)(struct audio_stream_entry *, struct audio_data_frame *, struct audio_data_frame *))demo_data_handler_save)(entry, in, out);
    return wlen;
}
#endif

void a2dp_event_handler(void *priv, int event, int param)	// 事件回调接口
{
    int slience_frames = -1;
    switch (event) {
    case MIXER_EVENT_CH_OPEN:
        slience_frames = decoder_syncts_frame_num_calc_start(priv);
        if (slience_frames >= 0) {
            audio_mixer_ch_add_slience_samples(&bt_a2dp_dec->mix_ch, slience_frames * audio_output_channel_num());
        }
        break;

    case MIXER_EVENT_CH_CLOSE:
        break;
    }
}

static void a2dp_decoder_syncts_open(struct a2dp_dec_hdl *dec, u8 channels, int sample_rate)
{
#if AUDIO_CODEC_SUPPORT_SYNC
    memset(&dec->syncts, 0, sizeof(struct decoder_syncts));

    dec->syncts.way_type = AUDIO_OUT_WAY_TYPE;

    if (AUDIO_DEC_BT_MIXER_EN) {
        a2dp_decoder_syncts_setup(&dec->syncts, sample_rate, audio_mixer_get_sample_rate(&mixer), channels);
    } else {
        a2dp_decoder_syncts_setup(&dec->syncts, sample_rate, sample_rate, channels);
    }

    /*设置SYNCTS的关联SOUND PCM设备选项*/
    a2dp_syncts_set_sound_pcm_handler(&dec->syncts, (void *)AUDIO_OUT_WAY_TYPE, sound_pcm_device_buffered_len);

    /*使能同步，配置延时时间*/
    a2dp_decoder_stream_syncts_enable(&dec->dec, &dec->syncts, sample_rate, CONFIG_A2DP_DELAY_TIME);
    //sound pcm 设置同步句柄
    sound_pcm_device_mount_syncts(dec->syncts.way_type, dec->syncts.syncts);
    audio_mixer_ch_set_event_handler(&dec->mix_ch, &dec->syncts, a2dp_event_handler);
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief   广播 上层激活解码
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void broadcast_a2dp_decoder_kick()
{
    if (!bt_a2dp_dec) {
        return ;
    }
    if (bt_a2dp_dec->pcm_capture) {
        audio_decoder_resume(&bt_a2dp_dec->dec.decoder);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief   解码输出pcm数据 自动激活
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void a2dp_pcm_capture_wait_timeout(void *priv)
{
    struct a2dp_dec_hdl *a2dp_dec = (struct a2dp_dec_hdl *)priv;
    local_irq_disable();
    sys_hi_timeout_del(a2dp_dec->timeout);
    a2dp_dec->timeout = 0;
    audio_stream_resume(&a2dp_dec->dec.decoder.entry);
    local_irq_enable();
}
/*----------------------------------------------------------------------------*/
/**@brief   a2dp解码输出pcm 数据接口
   @param
   @return   wlen : 实际输出长度
   @note
*/
/*----------------------------------------------------------------------------*/
static int a2dp_pcm_capture_data_handler(struct audio_stream_entry *entry,
        struct audio_data_frame *in,
        struct audio_data_frame *out)
{
    struct audio_decoder *decoder = container_of(entry, struct audio_decoder, entry);
    struct a2dp_decoder *dec = container_of(decoder, struct a2dp_decoder, decoder);
    struct a2dp_dec_hdl *a2dp_dec = container_of(dec, struct a2dp_dec_hdl, dec);
    // 自定义数据处理
    /* putchar('D'); */
    int wlen;
    wlen = a2dp_dec->output_handler(a2dp_dec->output_priv, in->data, in->data_len);
    if (wlen != in->data_len) {
        a2dp_dec->timeout = sys_hi_timeout_add((void *)a2dp_dec, (void (*)(void *))a2dp_pcm_capture_wait_timeout, 4);
    }
    //不使用audio_stream流程，输出后，需要把实际输出长度更新到decoder->process_len
    decoder->process_len = wlen;
    return wlen;
}
/*----------------------------------------------------------------------------*/
/**@brief    开始a2dp解码
   @param
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
static int a2dp_dec_pcm_capture_start(void)
{
    int err;
    struct audio_fmt *fmt;
    struct a2dp_dec_hdl *dec = bt_a2dp_dec;
    u8 ch_num = 0;
    u8 ch_type = 0;

    if (!bt_a2dp_dec) {
        return -EINVAL;
    }

    log_i(">>> a2dp_dec_to_pcm_start: in\n");

    // 打开a2dp解码
    err = a2dp_decoder_open(&dec->dec, &decode_task);
    if (err) {
        goto __err1;
    }
    // 使能事件回调
    audio_decoder_set_event_handler(&dec->dec.decoder, a2dp_dec_event_handler, 0);

    // 获取解码格式
    err = audio_decoder_get_fmt(&dec->dec.decoder, &fmt);
    if (err) {
        goto __err2;
    }
    // 设置输出声道数和声道类型
    ch_num = audio_output_channel_num();
    ch_type = audio_output_channel_type();
    ch_num = dec->pcm_ch_num;
    ch_type = dec->pcm_ch_num == 2 ? AUDIO_CH_LR :	AUDIO_CH_DIFF;
    dec->format_sync(dec->format_priv, fmt->sample_rate, dec->pcm_ch_num);
    a2dp_decoder_set_output_channel(&dec->dec, ch_num, ch_type);

    dec->dec.decoder.entry.data_handler = a2dp_pcm_capture_data_handler;

    // 数据流串联
    struct audio_stream_entry *entries[32] = {NULL};
    u8 entry_cnt = 0;
    entries[entry_cnt++] = &dec->dec.decoder.entry;
    // 创建数据流，把所有节点连接起来
    dec->stream = audio_stream_open(dec, a2dp_dec_out_stream_resume);
    audio_stream_add_list(dec->stream, entries, entry_cnt);
    /* 设置音频输出类型 */
    audio_output_set_start_volume(APP_AUDIO_STATE_MUSIC);
    log_i("dec->ch:%d, fmt->channel:%d\n", dec->dec.ch, fmt->channel);
    // 关闭丢数
    a2dp_drop_frame_stop();
    // 开始解码
    dec->dec.start = 1;
    err = audio_decoder_start(&dec->dec.decoder);
    if (err) {
        goto __err3;
    }
    clock_set_cur();
    return 0;

__err3:
    dec->dec.start = 0;
    // 先关闭各个节点，最后才close数据流
    if (dec->stream) {
        audio_stream_close(dec->stream);
        dec->stream = NULL;
    }

__err2:
    a2dp_decoder_close(&dec->dec);
__err1:
    a2dp_dec_release();

    return err;

}


/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码close
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void a2dp_dec_pcm_capture_res_close(void)
{
    if (bt_a2dp_dec->dec.start == 0) {
        log_i("bt_a2dp_dec->dec.start == 0");
        return ;
    }

    if (!bt_a2dp_dec->closing) {
        bt_a2dp_dec->preempted = 1;
    }
    bt_a2dp_dec->dec.start = 0;
    y_printf(">> %s, %d\n", __func__, __LINE__);
    a2dp_decoder_close(&bt_a2dp_dec->dec);

    if (bt_a2dp_dec->pcm_capture) {
        local_irq_disable();
        if (bt_a2dp_dec->timeout) {
            sys_hi_timeout_del(bt_a2dp_dec->timeout);
            bt_a2dp_dec->timeout = 0;
        }
        local_irq_enable();
    }

    // 先关闭各个节点，最后才close数据流
    if (bt_a2dp_dec->stream) {
        audio_stream_close(bt_a2dp_dec->stream);
        bt_a2dp_dec->stream = NULL;
    }
    app_audio_state_exit(APP_AUDIO_STATE_MUSIC);
}


/*----------------------------------------------------------------------------*/
/**@brief    开始a2dp解码
   @param
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
static int a2dp_dec_start(void)
{
    int err;
    struct audio_fmt *fmt;
    struct a2dp_dec_hdl *dec = bt_a2dp_dec;
    u8 ch_num = 0;
    u8 ch_type = 0;

    if (!bt_a2dp_dec) {
        return -EINVAL;
    }

    log_i("a2dp_dec_start: in\n");

    // 打开a2dp解码
    err = a2dp_decoder_open(&dec->dec, &decode_task);
    if (err) {
        goto __err1;
    }
    // 使能事件回调
    audio_decoder_set_event_handler(&dec->dec.decoder, a2dp_dec_event_handler, 0);

    // 获取解码格式
    err = audio_decoder_get_fmt(&dec->dec.decoder, &fmt);
    if (err) {
        goto __err2;
    }

    // 设置输出声道数和声道类型
    ch_num = audio_output_channel_num();
    ch_type = audio_output_channel_type();
    a2dp_decoder_set_output_channel(&dec->dec, ch_num, ch_type);

    if (AUDIO_DEC_BT_MIXER_EN) {
        u8 mix_src_en = 1;
#if AUDIO_CODEC_SUPPORT_SYNC
        mix_src_en = 0;
#endif
        audio_mode_main_dec_open(AUDIO_MODE_MAIN_STATE_DEC_A2DP);
        // 配置mixer通道参数
        audio_mixer_ch_open_head(&dec->mix_ch, &mixer); // 挂载到mixer最前面
#if (SOUNDCARD_ENABLE)
        audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 0, BIT(0) | BIT(2) | BIT(3));
        audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 1, BIT(1) | BIT(2) | BIT(3));
#endif/*SOUNDCARD_ENABLE*/
        audio_mixer_ch_set_src(&dec->mix_ch, mix_src_en, 0);
        audio_mixer_ch_set_no_wait(&dec->mix_ch, 1, 20); // 超时自动丢数
        audio_mixer_ch_sample_sync_enable(&dec->mix_ch, mix_src_en); // 标志为sync通道
        audio_mixer_ch_set_sample_rate(&dec->mix_ch, fmt->sample_rate);
    }

#if (RECORDER_MIX_EN)
    audio_mixer_ch_open_head(&dec->rec_mix_ch, &recorder_mixer); // 挂载到mixer最前面
    audio_mixer_ch_set_src(&dec->rec_mix_ch, 1, 0);
    audio_mixer_ch_set_no_wait(&dec->rec_mix_ch, 1, 10); // 超时自动丢数
    /* audio_mixer_ch_sample_sync_enable(&dec->rec_mix_ch, 1); */
#endif//RECORDER_MIX_EN

    u32 sr = fmt->sample_rate;
    if (AUDIO_DEC_BT_MIXER_EN) {
        sr = audio_mixer_get_sample_rate(&mixer);
    }


#if AUDIO_SURROUND_CONFIG
    //环绕音效
    dec->surround = surround_open_demo(AEID_MUSIC_SURROUND, ch_type);
#endif

#if AUDIO_VBASS_CONFIG
    dec->vbass_prev_gain = audio_gain_open_demo(AEID_MUSIC_VBASS_PREV_GAIN, ch_num);
    dec->ns_gate = audio_noisegate_open_demo(AEID_MUSIC_NS_GATE, sr, ch_num);
    //虚拟低音
    dec->vbass = audio_vbass_open_demo(AEID_MUSIC_VBASS, sr, ch_num);
#endif


#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
    dec->high_bass = high_bass_eq_open(sr, ch_num);
    dec->hb_drc = high_bass_drc_open(sr, ch_num);
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->hb_drc && dec->hb_drc->run32bit) {
        dec->hb_convert = convet_data_open(0, 512);
    }
#endif
#endif
#if TCFG_EQ_ENABLE && TCFG_BT_MUSIC_EQ_ENABLE
    dec->eq = music_eq_open(sr, ch_num);// eq
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    dec->drc = music_drc_open(sr, ch_num);//drc
#endif/*TCFG_BT_MUSIC_DRC_ENABLE*/

#if !TCFG_DYNAMIC_EQ_ENABLE
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->eq && dec->eq->out_32bit) {
        dec->convert = convet_data_open(0, 512);
    }
#endif
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    dec->ext_eq = music_ext_eq_open(fmt->sample_rate, ch_num);
#endif
#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
    dec->eq2 = music_eq2_open(sr, ch_num);// eq
    dec->dy_eq = audio_dynamic_eq_ctrl_open(AEID_MUSIC_DYNAMIC_EQ, sr, ch_num);//动态eq
    dec->convert2 = convet_data_open(0, 512);
#endif/*TCFG_DYNAMIC_EQ_ENABLE*/
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    dec->gain = audio_gain_open_demo(AEID_MUSIC_GAIN, ch_num);
#endif
#endif/*TCFG_BT_MUSIC_EQ_ENABLE*/


#if TCFG_EQ_DIVIDE_ENABLE

#if TCFG_EQ_ENABLE &&TCFG_BT_MUSIC_EQ_ENABLE
    dec->eq_rl_rr = music_eq_rl_rr_open(sr, ch_num);// eq
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    dec->drc_rl_rr = music_drc_rl_rr_open(sr, ch_num);//drc
#endif
    if (dec->eq_rl_rr && dec->eq_rl_rr->out_32bit) {
        dec->convert_rl_rr = convet_data_open(0, 512);
    }
#endif


    dec->gain_rl_rr = audio_gain_open_demo(AEID_MUSIC_RL_GAIN, ch_num);
    if (dec->eq_rl_rr) {
        audio_vocal_tract_open(&dec->vocal_tract, AUDIO_SYNTHESIS_LEN);
        {
            u8 entry_cnt = 0;
            struct audio_stream_entry *entries[8] = {NULL};
            entries[entry_cnt++] = &dec->vocal_tract.entry;
            if (AUDIO_DEC_BT_MIXER_EN) {
                entries[entry_cnt++] = &dec->mix_ch.entry;
            } else {
                entries[entry_cnt++] = &default_dac.entry;
            }

            dec->vocal_tract.stream = audio_stream_open(&dec->vocal_tract, audio_vocal_tract_stream_resume);
            audio_stream_add_list(dec->vocal_tract.stream, entries, entry_cnt);
        }
        audio_vocal_tract_synthesis_open(&dec->synthesis_ch_fl_fr, &dec->vocal_tract, FL_FR);
        audio_vocal_tract_synthesis_open(&dec->synthesis_ch_rl_rr, &dec->vocal_tract, RL_RR);
    } else {
        dec->ch_switch = channel_switch_open(AUDIO_CH_QUAD, AUDIO_SYNTHESIS_LEN / 2);
    }

#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
    dec->ext_eq2 = music_ext_eq2_open(fmt->sample_rate, ch_num);
#endif
#ifdef CONFIG_MIXER_CYCLIC
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 0, BIT(0));
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 1, BIT(1));
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 2, BIT(2));
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 3, BIT(3));
#endif
#endif




    // sync初始化
#if AUDIO_CODEC_SUPPORT_SYNC
    a2dp_decoder_syncts_open(dec, ch_num, fmt->sample_rate);
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/


#if 0
    // 获取解码输出数据示例
    // 可以重新实现data_handler，在data_handler中再调用数据流输出
    // 或者可以查看紧接着dec->dec.decoder.entry之后的数据流预处理数据
    dec->dec.decoder.entry.data_handler = demo_decoder_data_handler;
#endif
#if 0
    // 获取节点数据示例1
    // 获取传入数据流节点中的数据仅需实现prob_handler接口就可以了
    // 该方式仅用于查看节点数据，或者做一些清零之类的简单动作
    // 这里以获取传入mix节点的数据为例
    dec->mix_ch.entry.prob_handler = demo_stream_mix_prob_handler;
#endif
#if 0
    // 获取节点数据示例2
    // 可以用变量保存原来的数据处理接口，然后重新赋值新的数据处理
    // 这里以获取mix节点的数据为例
    demo_data_handler_save = (void *)dec->mix_ch.entry.data_handler;
    dec->mix_ch.entry.data_handler = demo_new_data_handler;
#endif

    // 数据流串联
    struct audio_stream_entry *entries[32] = {NULL};
    u8 entry_cnt = 0;
    u8 rl_rr_entry_start = 0;
    entries[entry_cnt++] = &dec->dec.decoder.entry;

#if SYS_DIGVOL_GROUP_EN
    void *dvol_entry = sys_digvol_group_ch_open("music_a2dp", -1, NULL);
    entries[entry_cnt++] = dvol_entry;
#endif // SYS_DIGVOL_GROUP_EN

#if AUDIO_CODEC_SUPPORT_SYNC
    if (dec->syncts.syncts) {
        entries[entry_cnt++] = &dec->syncts.entry;
    }
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/



#if AUDIO_VBASS_CONFIG
    if (dec->vbass_prev_gain) {
        entries[entry_cnt++] = &dec->vbass_prev_gain->entry;
    }
    if (dec->ns_gate) {
        entries[entry_cnt++] = &dec->ns_gate->entry;
    }
    if (dec->vbass) {
        entries[entry_cnt++] = &dec->vbass->entry;
    }
#endif
#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
    if (dec->high_bass) { //高低音
        entries[entry_cnt++] = &dec->high_bass->entry;
    }
    if (dec->hb_drc) { //高低音后drc
        entries[entry_cnt++] = &dec->hb_drc->entry;
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
        if (dec->hb_convert) {
            entries[entry_cnt++] = &dec->hb_convert->entry;
        }
#endif
    }
#endif

    rl_rr_entry_start = entry_cnt - 1;//记录eq的上一个节点
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    if (dec->gain) {
        entries[entry_cnt++] = &dec->gain->entry;
    }
#endif
#if AUDIO_SURROUND_CONFIG
    if (dec->surround) {
        entries[entry_cnt++] = &dec->surround->entry;
    }
#endif
#if TCFG_EQ_ENABLE && TCFG_BT_MUSIC_EQ_ENABLE
    if (dec->eq) {
        entries[entry_cnt++] = &dec->eq->entry;
        if (dec->drc) {
            entries[entry_cnt++] = &dec->drc->entry;
        }
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
        if (dec->convert) {
            entries[entry_cnt++] = &dec->convert->entry;
        }
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
        if (dec->ext_eq) {
            entries[entry_cnt++] = &dec->ext_eq->entry;
        }
#endif
#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
        if (dec->eq2) {
            entries[entry_cnt++] = &dec->eq2->entry;
        }
        if (dec->dy_eq && dec->dy_eq->dy_eq) {
            entries[entry_cnt++] = &dec->dy_eq->dy_eq->entry;
        }
        if (dec->convert2) {
            entries[entry_cnt++] = &dec->convert2->entry;
        }
#endif

    }
#endif




#if TCFG_USER_TWS_ENABLE
    if (dec->preempted) {
        dec->preempted = 0;
        entries[entry_cnt++] = audio_dec_slience_stream_entry(dec, 700);
    }
#endif
#if TCFG_EQ_DIVIDE_ENABLE
    if (dec->eq_rl_rr) {
        entries[entry_cnt++] = &dec->synthesis_ch_fl_fr.entry;//四声道eq独立时，该节点后不接节点
    } else {
        if (dec->ch_switch) {
            entries[entry_cnt++] = &dec->ch_switch->entry;
        }
        if (AUDIO_DEC_BT_MIXER_EN) {
            entries[entry_cnt++] = &dec->mix_ch.entry;
        } else {
            entries[entry_cnt++] = &default_dac.entry;
        }
    }
#else
    if (AUDIO_DEC_BT_MIXER_EN) {
        entries[entry_cnt++] = &dec->mix_ch.entry;
    } else {
        default_dac.entry.prob_handler = a2dp_to_dac_probe_handler;
        entries[entry_cnt++] = &default_dac.entry;
    }
#endif

    // 创建数据流，把所有节点连接起来
    dec->stream = audio_stream_open(dec, a2dp_dec_out_stream_resume);
    audio_stream_add_list(dec->stream, entries, entry_cnt);

#if (RECORDER_MIX_EN)
    audio_stream_add_entry(entries[entry_cnt - 2], &dec->rec_mix_ch.entry);
#endif//RECORDER_MIX_EN

#if TCFG_EQ_DIVIDE_ENABLE
    struct audio_stream_entry *rl_rr_entries[16] = {NULL};
    entry_cnt = 0;
    if (dec->eq_rl_rr) { //接在eq_drc的上一个节点
        rl_rr_entries[entry_cnt++] = entries[rl_rr_entry_start];
        if (dec->gain_rl_rr) {
            rl_rr_entries[entry_cnt++] = &dec->gain_rl_rr->entry;
        }
        rl_rr_entries[entry_cnt++] = &dec->eq_rl_rr->entry;
        if (dec->drc_rl_rr) {
            rl_rr_entries[entry_cnt++] = &dec->drc_rl_rr->entry;
        }
        if (dec->convert_rl_rr) {
            rl_rr_entries[entry_cnt++] = &dec->convert_rl_rr->entry;
        }

#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
        if (dec->ext_eq2) {
            rl_rr_entries[entry_cnt++] = &dec->ext_eq2->entry;
        }
#endif

        rl_rr_entries[entry_cnt++] = &dec->synthesis_ch_rl_rr.entry;//必须是最后一个
        for (int i = 0; i < (entry_cnt - 1); i++) {
            audio_stream_add_entry(rl_rr_entries[i], rl_rr_entries[i + 1]);
        }
    }
#endif

    // 设置音频输出类型
    audio_output_set_start_volume(APP_AUDIO_STATE_MUSIC);

    log_i("dec->ch:%d, fmt->channel:%d\n", dec->dec.ch, fmt->channel);

    // 关闭丢数
    a2dp_drop_frame_stop();
#if TCFG_USER_TWS_ENABLE
    if (tws_network_audio_was_started()) {
        /*a2dp播放中副机加入*/
        tws_network_local_audio_start();
        a2dp_decoder_join_tws(&dec->dec);
    }
#endif

    // 开始解码
    dec->dec.start = 1;
    err = audio_decoder_start(&dec->dec.decoder);
    if (err) {
        goto __err3;
    }

    clock_set_cur();

    return 0;

__err3:
    dec->dec.start = 0;

#if AUDIO_SURROUND_CONFIG
    surround_close_demo(dec->surround);
#endif


#if AUDIO_VBASS_CONFIG
    audio_gain_close_demo(dec->vbass_prev_gain);
    audio_noisegate_close_demo(dec->ns_gate);
    audio_vbass_close_demo(dec->vbass);
#endif
#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
    high_bass_eq_close(dec->high_bass);
    high_bass_drc_close(dec->hb_drc);
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(dec->hb_convert);
#endif
#endif
#if TCFG_EQ_ENABLE && TCFG_BT_MUSIC_EQ_ENABLE
    music_eq_close(dec->eq);
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    music_drc_close(dec->drc);
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(dec->convert);
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    music_ext_eq_close(dec->ext_eq);
#endif
#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
    music_eq2_close(dec->eq2);
    audio_dynamic_eq_ctrl_close(dec->dy_eq);
    convet_data_close(dec->convert2);
#endif
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    audio_gain_close_demo(dec->gain);
#endif
#endif

#if TCFG_EQ_DIVIDE_ENABLE
    music_eq_rl_rr_close(dec->eq_rl_rr);
    music_drc_rl_rr_close(dec->drc_rl_rr);
    convet_data_close(dec->convert_rl_rr);
    audio_gain_close_demo(dec->gain_rl_rr);

    audio_vocal_tract_synthesis_close(&dec->synthesis_ch_fl_fr);
    audio_vocal_tract_synthesis_close(&dec->synthesis_ch_rl_rr);
    audio_vocal_tract_close(&dec->vocal_tract);
    channel_switch_close(&dec->ch_switch);
#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
    music_ext_eq2_close(dec->ext_eq2);
#endif
#endif



    if (AUDIO_DEC_BT_MIXER_EN) {
        audio_mixer_ch_close(&dec->mix_ch);
    } else {
        bt_dec_stream_run_stop(&dec->dec.decoder);
    }
#if (RECORDER_MIX_EN)
    audio_mixer_ch_close(&dec->rec_mix_ch);
#endif//RECORDER_MIX_EN

#if AUDIO_CODEC_SUPPORT_SYNC
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/

#if SYS_DIGVOL_GROUP_EN
    sys_digvol_group_ch_close("music_a2dp");
#endif // SYS_DIGVOL_GROUP_EN


    // 先关闭各个节点，最后才close数据流
    if (dec->stream) {
        audio_stream_close(dec->stream);
        dec->stream = NULL;
    }

__err2:
    a2dp_decoder_close(&dec->dec);
__err1:
    a2dp_dec_release();

    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    a2dp解码资源等待
   @param    *wait: 句柄
   @param    event: 事件
   @return   0：成功
   @note     用于多解码打断处理
*/
/*----------------------------------------------------------------------------*/
static int a2dp_wait_res_handler(struct audio_res_wait *wait, int event)
{
    int err = 0;

    log_i("a2dp_wait_res_handler: %d\n", event);

    if (event == AUDIO_RES_GET) {
        if (!bt_a2dp_dec->pcm_capture) {
            err = a2dp_dec_start();
        } else {
            err = a2dp_dec_pcm_capture_start();
        }
    } else if (event == AUDIO_RES_PUT) {
        if (bt_a2dp_dec->dec.start) {
            if (!bt_a2dp_dec->pcm_capture) {
                a2dp_audio_res_close();
            } else {
                a2dp_dec_pcm_capture_res_close();
            }
            a2dp_drop_frame_start();
        }
    }
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    打开a2dp解码
   @param    media_type: 媒体类型
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
int a2dp_dec_open(int media_type)
{
    struct a2dp_dec_hdl *dec;

    if (bt_a2dp_dec) {
        return 0;
    }

    log_i("a2dp_dec_open: %d\n", media_type);

    if (a2dp_suspend) {
#if (TCFG_USER_TWS_ENABLE)
        if (tws_api_get_role() == TWS_ROLE_MASTER)
#endif//TCFG_USER_TWS_ENABLE
        {
            if (drop_a2dp_timer == 0) {
                drop_a2dp_timer = sys_timer_add(NULL,
                                                a2dp_media_clear_packet_before_seqn, 100);
            }
        }
        return 0;
    }

    dec = zalloc(sizeof(*dec));
    if (!dec) {
        return -ENOMEM;
    }

    switch (media_type) {
    case A2DP_CODEC_SBC:
        log_i("a2dp_media_type:SBC");
        dec->dec.coding_type = AUDIO_CODING_SBC;
        clock_add(DEC_SBC_CLK);
        break;
    case A2DP_CODEC_MPEG24:
        log_i("a2dp_media_type:AAC");
        dec->dec.coding_type = AUDIO_CODING_AAC;
        clock_add(DEC_AAC_CLK);
        break;
    default:
        log_i("a2dp_media_type unsupoport:%d", media_type);
        free(dec);
        return -EINVAL;
    }

    UI_MSG_POST("music_status:status=%4", 1);

    bt_a2dp_dec = dec;
    dec->pcm_capture = 0;
    dec->wait.priority = 1;		// 解码优先级
    dec->wait.preemption = 0;	// 不使能直接抢断解码
    dec->wait.snatch_same_prio = 1;	// 可抢断同优先级解码
#if SOUNDCARD_ENABLE
    dec->wait.protect = 1;
#endif
    dec->wait.handler = a2dp_wait_res_handler;
    audio_decoder_task_add_wait(&decode_task, &dec->wait);

    if (bt_a2dp_dec && (bt_a2dp_dec->dec.start == 0)) {
        a2dp_drop_frame_start();
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief    打开2dp解码 输出pcm数据
   @param    params: 参数
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
int a2dp_dec_to_pcm_open(struct a2dp_dec_params *params)
{
    struct a2dp_dec_hdl *dec;
    struct a2dp_dec_params *a2dp_dec_params = (struct a2dp_dec_params *)params;

    if (bt_a2dp_dec) {
        return 0;
    }
    int media_type = a2dp_dec_params->media_type;
    log_i("a2dp_dec_open: %d\n", media_type);

    if (a2dp_suspend) {
#if (TCFG_USER_TWS_ENABLE)
        if (tws_api_get_role() == TWS_ROLE_MASTER)
#endif//TCFG_USER_TWS_ENABLE
        {
            if (drop_a2dp_timer == 0) {
                drop_a2dp_timer = sys_timer_add(NULL,
                                                a2dp_media_clear_packet_before_seqn, 100);
            }
        }
        return 0;
    }

    dec = zalloc(sizeof(*dec));
    if (!dec) {
        return -ENOMEM;
    }
    switch (media_type) {
    case A2DP_CODEC_SBC:
        log_i("a2dp_media_type:SBC");
        dec->dec.coding_type = AUDIO_CODING_SBC;
        clock_add(DEC_SBC_CLK);
        break;
    case A2DP_CODEC_MPEG24:
        log_i("a2dp_media_type:AAC");
        dec->dec.coding_type = AUDIO_CODING_AAC;
        clock_add(DEC_AAC_CLK);
        break;
    default:
        log_i("a2dp_media_type unsupoport:%d", media_type);
        free(dec);
        return -EINVAL;
    }

    UI_MSG_POST("music_status:status=%4", 1);

    bt_a2dp_dec = dec;
    dec->pcm_capture = 1;
    //解码输出pcm 参数配置
    dec->pcm_ch_num = a2dp_dec_params->ch_num;
    dec->output_priv = a2dp_dec_params->output_priv;
    dec->output_handler = a2dp_dec_params->output_handler;
    dec->format_priv = a2dp_dec_params->format_priv;
    dec->format_sync = a2dp_dec_params->format_sync;



    dec->wait.priority = 1;		// 解码优先级
    dec->wait.protect = 1;		//后台解码不能被打断
    dec->wait.preemption = 0;	// 不使能直接抢断解码
    dec->wait.snatch_same_prio = 1;	// 可抢断同优先级解码
#if SOUNDCARD_ENABLE
    dec->wait.protect = 1;
#endif
    dec->wait.handler = a2dp_wait_res_handler;
    audio_decoder_task_add_wait(&decode_task, &dec->wait);

    if (bt_a2dp_dec && (bt_a2dp_dec->dec.start == 0)) {
        a2dp_drop_frame_start();
    }

    return 0;
}
/*----------------------------------------------------------------------------*/
/**@brief    关闭a2dp解码
   @param
   @return   0: 没有a2dp解码
   @return   1: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
int a2dp_dec_close(void)
{
    if (drop_a2dp_timer) {
        sys_timer_del(drop_a2dp_timer);
        drop_a2dp_timer = 0;
    }
    if (!bt_a2dp_dec) {
        return 0;
    }
    bt_a2dp_dec->closing = 1;

    if (bt_a2dp_dec->dec.start) {
        if (!bt_a2dp_dec->pcm_capture) {
            a2dp_audio_res_close();
        } else {
            a2dp_dec_pcm_capture_res_close();
        }
    }
    a2dp_dec_release();/*free bt_a2dp_dec*/
    clock_set_cur();
    log_i("a2dp_dec_close: exit\n");

    UI_MSG_POST("music_status:status=%4", 0);
    return 1;
}


/*----------------------------------------------------------------------------*/
/**@brief    a2dp挂起
   @param    *p: 私有参数
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
int a2dp_tws_dec_suspend(void *p)
{
    r_printf("a2dp_tws_dec_suspend\n");
    /*mem_stats();*/

    if (a2dp_suspend) {
        return -EINVAL;
    }
    a2dp_suspend = 1;

    if (bt_a2dp_dec) {
        a2dp_dec_close();
        a2dp_media_clear_packet_before_seqn(0);
#if (TCFG_USER_TWS_ENABLE)
        if (tws_api_get_role() == 0)
#endif//TCFG_USER_TWS_ENABLE
        {
            drop_a2dp_timer = sys_timer_add(NULL, a2dp_media_clear_packet_before_seqn, 100);
        }
    }

    int err = audio_decoder_fmt_lock(&decode_task, AUDIO_CODING_AAC);
    if (err) {
        log_e("AAC_dec_lock_faild\n");
    }

    return err;
}


/*----------------------------------------------------------------------------*/
/**@brief    a2dp挂起激活
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void a2dp_tws_dec_resume(void)
{
    r_printf("a2dp_tws_dec_resume\n");

    if (a2dp_suspend) {
        a2dp_suspend = 0;

        if (drop_a2dp_timer) {
            sys_timer_del(drop_a2dp_timer);
            drop_a2dp_timer = 0;
        }

        audio_decoder_fmt_unlock(&decode_task, AUDIO_CODING_AAC);

        int type = a2dp_media_get_codec_type();
        printf("codec_type: %d\n", type);
        if (type >= 0) {
#if (TCFG_USER_TWS_ENABLE)
            if (tws_api_get_role() == 0)
#endif//TCFG_USER_TWS_ENABLE
            {
                a2dp_media_clear_packet_before_seqn(0);
            }
            /* a2dp_resume_time = jiffies + msecs_to_jiffies(80); */
            a2dp_dec_open(type);
        }
    }
}

#if TCFG_ESCO_PLC
/*----------------------------------------------------------------------------*/
/**@brief    esco丢包修护初始化
   @param
   @return   丢包修护句柄
   @note
*/
/*----------------------------------------------------------------------------*/
static void *esco_plc_init(void)
{
    void *plc = audio_decoder_malloc(PLC_query()); /*buf_size:1040*/
    //plc = zalloc_mux(PLC_query());
    log_i("PLC_buf:0x%x,size:%d\n", (int)plc, PLC_query());
    if (!plc) {
        return NULL;
    }
    int err = PLC_init(plc);
    if (err) {
        log_i("PLC_init err:%d", err);
        audio_decoder_free(plc);
        plc = NULL;
    }
    return plc;
}
/*----------------------------------------------------------------------------*/
/**@brief    esco丢包修护关闭
   @param    *plc: 丢包修护句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void esco_plc_close(void *plc)
{
    audio_decoder_free(plc);
}
/*----------------------------------------------------------------------------*/
/**@brief    esco丢包修护运行
   @param    *data: 数据
   @param    len: 数据长度
   @param    repair: 修护标记
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void esco_plc_run(s16 *data, u16 len, u8 repair)
{
    u16 repair_point, tmp_point;
    s16 *p_in, *p_out;
    p_in    = data;
    p_out   = data;
    tmp_point = len / 2;

#if 0	//debug
    static u16 repair_cnt = 0;
    if (repair) {
        repair_cnt++;
        y_printf("[E%d]", repair_cnt);
    } else {
        repair_cnt = 0;
    }
    //log_i("[%d]",point);
#endif/*debug*/

    while (tmp_point) {
        repair_point = (tmp_point > PLC_FRAME_LEN) ? PLC_FRAME_LEN : tmp_point;
        tmp_point = tmp_point - repair_point;
        PLC_run(p_in, p_out, repair_point, repair);
        p_in  += repair_point;
        p_out += repair_point;
    }
}
#endif




/*----------------------------------------------------------------------------*/
/**@brief    esco解码输出数据处理
   @param    *entry: 数据流入口
   @param    *in: 输入数据
   @param    *out: 输出数据
   @return   处理了多长数据
   @note
*/
/*----------------------------------------------------------------------------*/
static int esco_dec_data_handler(struct audio_stream_entry *entry,
                                 struct audio_data_frame *in,
                                 struct audio_data_frame *out)
{
    struct audio_decoder *decoder = container_of(entry, struct audio_decoder, entry);
    struct esco_decoder *esco_dec = container_of(decoder, struct esco_decoder, decoder);
    struct esco_dec_hdl *dec = container_of(esco_dec, struct esco_dec_hdl, dec);

    if (dec->remain == 0) {
        if (esco_dec->slience_frames) {
            memset(in->data, 0, in->data_len);
            esco_dec->slience_frames--;
        }

#if TCFG_ESCO_PLC
        if (dec->plc && out && out->data) {
            esco_plc_run(in->data, in->data_len, *(u8 *)out->data);
        }
#endif/*TCFG_ESCO_PLC*/
#if TCFG_ESCO_NOISEGATE
        if (dec->noisegate) {
            run_noisegate(dec->noisegate, in->data, in->data, in->data_len);
        }
#endif /*TCFG_ESCO_NOISEGATE*/


    }

    out->no_subsequent = 1;

    struct audio_data_frame out_frame;
    memcpy(&out_frame, in, sizeof(struct audio_data_frame));
    audio_stream_run(&decoder->entry, &out_frame);
    int wlen = decoder->process_len;
    /* int wlen = esco_decoder_output_handler(&dec->dec, in); */


    if (in->data_len != wlen) {
        dec->remain = 1;
    } else {
        dec->remain = 0;
    }
    return wlen;
}


/*----------------------------------------------------------------------------*/
/**@brief    esco解码释放
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void esco_dec_release()
{
    audio_decoder_task_del_wait(&decode_task, &bt_esco_dec->wait);

    if (bt_esco_dec->dec.coding_type == AUDIO_CODING_MSBC) {
        clock_remove(DEC_MSBC_CLK);
    } else if (bt_esco_dec->dec.coding_type == AUDIO_CODING_CVSD) {
        clock_remove(DEC_CVSD_CLK);
    }

    local_irq_disable();
    free(bt_esco_dec);
    bt_esco_dec = NULL;
    local_irq_enable();
}

/*----------------------------------------------------------------------------*/
/**@brief    esco解码事件返回
   @param    *decoder: 解码器句柄
   @param    argc: 参数个数
   @param    *argv: 参数
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void esco_dec_event_handler(struct audio_decoder *decoder, int argc, int *argv)
{
    switch (argv[0]) {
    case AUDIO_DEC_EVENT_END:
        log_i("AUDIO_DEC_EVENT_END\n");
        esco_dec_close();
        break;
    }
}

static int esco_to_dac_probe_handler(struct audio_stream_entry *entry,  struct audio_data_frame *in)
{
    if (!in->stop) {
#if AUDIO_CODEC_SUPPORT_SYNC
        /* if (bt_esco_dec->syncts.syncts) { */
        /* in->data_sync = 1; */
        /* } */
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief    esco解码close
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
extern void audio_aec_ref_src_close();
void audio_aec_ref_stream_close();
static void esco_audio_res_close(void)
{
    /*
     *先关闭aec，里面有复用到enc的buff，再关闭enc，
     *如果没有buf复用，则没有先后顺序要求。
     */
    if (!bt_esco_dec->dec.start) {
        return ;
    }
    bt_esco_dec->dec.start = 0;
    bt_esco_dec->dec.enc_start = 0;
#if (TCFG_IIS_ENABLE && TCFG_IIS_OUTPUT_EN)
    audio_aec_ref_src_close();
#endif //(TCFG_IIS_ENABLE && TCFG_IIS_OUTPUT_EN)
    audio_aec_close();
    esco_enc_close();
    esco_decoder_close(&bt_esco_dec->dec);
#if TCFG_EQ_ENABLE &&TCFG_PHONE_EQ_ENABLE
    esco_eq_close(bt_esco_dec->eq);
#endif
#if TCFG_DRC_ENABLE && ESCO_DRC_EN
    esco_drc_close(bt_esco_dec->drc);
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(bt_esco_dec->convert);
#endif


    spectrum_switch_demo(1);//打开频谱计算
    channel_switch_close(&bt_esco_dec->ch_switch);

#if AUDIO_CODEC_SUPPORT_SYNC
    sound_pcm_device_unmount_syncts(bt_esco_dec->syncts.way_type, bt_esco_dec->syncts.syncts);
    esco_decoder_syncts_free(&bt_esco_dec->syncts);
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/

    if (AUDIO_DEC_BT_MIXER_EN) {
        audio_mixer_ch_close(&bt_esco_dec->mix_ch);
    } else {
        default_dac.entry.prob_handler = NULL;
        bt_dec_stream_run_stop(&bt_esco_dec->dec.decoder);
    }
#if (RECORDER_MIX_EN)
    audio_mixer_ch_close(&bt_esco_dec->rec_mix_ch);
#endif//RECORDER_MIX_EN


#if TCFG_ESCO_PLC
    if (bt_esco_dec->plc) {
        esco_plc_close(bt_esco_dec->plc);
        bt_esco_dec->plc = NULL;
    }
#endif/*TCFG_ESCO_PLC*/

#if TCFG_ESCO_NOISEGATE
    if (bt_esco_dec->noisegate) {
        close_noisegate(bt_esco_dec->noisegate);
        bt_esco_dec->noisegate = NULL;
    }
#endif /*TCFG_ESCO_NOISEGATE*/

#if SYS_DIGVOL_GROUP_EN
    sys_digvol_group_ch_close("call_esco");
#endif // SYS_DIGVOL_GROUP_EN

    // 先关闭各个节点，最后才close数据流
    if (bt_esco_dec->stream) {
        audio_stream_close(bt_esco_dec->stream);
        bt_esco_dec->stream = NULL;
    }

#if TCFG_AUDIO_OUTPUT_IIS
    audio_aec_ref_stream_close();
#endif /*TCFG_AUDIO_OUTPUT_IIS*/

#if TCFG_ESCO_USE_SPEC_MIX_LEN
    /*恢复mix_buf的长度*/
    audio_mixer_set_output_buf(&mixer, mix_buff, AUDIO_MIXER_LEN);
#endif /*TCFG_ESCO_USE_SPEC_MIX_LEN*/
    app_audio_state_exit(APP_AUDIO_STATE_CALL);
    bt_esco_dec->dec.start = 0;
}

/*----------------------------------------------------------------------------*/
/**@brief    esco解码数据流激活
   @param    *p: 私有句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void esco_dec_out_stream_resume(void *p)
{
    struct esco_dec_hdl *dec = (struct esco_dec_hdl *)p;

    /* audio_decoder_resume(&dec->dec.decoder); */
}

/*----------------------------------------------------------------------------*/
/**@brief    esco接受回调
   @param
   @return
   @note	 蓝牙库里面接受到了esco音频
*/
/*----------------------------------------------------------------------------*/
void esco_rx_notice_to_decode(void)
{
    if (bt_esco_dec && bt_esco_dec->dec.start) {
        audio_decoder_resume(&bt_esco_dec->dec.decoder);
        /* if (bt_esco_dec->dec.wait_resume) { */
        /* bt_esco_dec->dec.wait_resume = 0; */
        /* audio_decoder_resume(&bt_esco_dec->dec.decoder); */
        /* } */
    }
}

void audio_aec_ref_src_get_output_rate(u16 *input_rate, u16 *output_rate)
{
#if AUDIO_CODEC_SUPPORT_SYNC
    /* if (bt_esco_dec->sync) { */
    /* audio_wireless_sync_get_rate(bt_esco_dec->sync->context, input_rate, output_rate); */
    /* } */
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/
}

void esco_event_handler(void *priv, int event, int param)	// 事件回调接口
{
    int slience_frames = -1;
    switch (event) {
    case MIXER_EVENT_CH_OPEN:
        slience_frames = decoder_syncts_frame_num_calc_start(priv);
        if (slience_frames >= 0) {
            audio_mixer_ch_add_slience_samples(&bt_esco_dec->mix_ch, slience_frames * audio_output_channel_num());
        }
        break;

    case MIXER_EVENT_CH_CLOSE:
        break;
    }
}

/*iis输出通话回音消除参考数据获取节点*/
struct audio_aec_ref {
    int remain_len;
    /* s16 tmp_buf[512]; */
    struct audio_stream_entry entry;    // 音频流入口
};
static struct audio_aec_ref *aec_ref_hdl = NULL;
static void audio_aec_ref_data_process_len(struct audio_stream_entry *entry,  int len)
{
    struct audio_aec_ref *hdl = container_of(entry, struct audio_aec_ref, entry);
}
static int audio_aec_ref_data_handler(struct audio_stream_entry *entry, struct audio_data_frame *in, struct audio_data_frame *out)
{
    struct audio_aec_ref *hdl = container_of(entry, struct audio_aec_ref, entry);
    /* 设置输出信息，这里输出和输入相同 */
    out->data = in->data;
    out->data_len = in->data_len;
    if (in->data_len - in->offset > 0) {
        /* 数据处理 */
        audio_aec_refbuf(in->data + in->offset / 2, in->data_len - in->offset);
    }
    /* printf("%d\n", in->data_len); */
    return in->data_len;
}
void *audio_aec_ref_stream_open()
{
    struct audio_aec_ref *hdl = zalloc(sizeof(struct audio_aec_ref));
    if (!hdl) {
        printf("audio aec ref malloc failed \n");
    }

    hdl->entry.data_process_len = audio_aec_ref_data_process_len;
    hdl->entry.data_handler = audio_aec_ref_data_handler;
    aec_ref_hdl = hdl;
    return hdl;
}
void audio_aec_ref_stream_close()
{
    if (aec_ref_hdl) {
        free(aec_ref_hdl);
        aec_ref_hdl = NULL;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    开始esco解码
   @param
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
extern void audio_aec_ref_src_open();
static int esco_dec_start()
{
    int err;
    struct esco_dec_hdl *dec = bt_esco_dec;

    if (!bt_esco_dec) {
        return -EINVAL;
    }
    // 打开esco解码
    err = esco_decoder_open(&dec->dec, &decode_task);
    if (err) {
        goto __err1;
    }
    // 使能事件回调
    audio_decoder_set_event_handler(&dec->dec.decoder, esco_dec_event_handler, 0);

#if TCFG_ESCO_USE_SPEC_MIX_LEN
    /*
     *(1)bt_esco_dec输出是120或者240，所以通话的时候，修改mix_buff的长度，提高效率
     *(2)其他大部分时候解码输出是512的倍数，通话结束，恢复mix_buff的长度，提高效率
     */
    audio_mixer_set_output_buf(&mixer, mix_buff, AUDIO_MIXER_LEN / 240 * 240);
#endif /*TCFG_ESCO_USE_SPEC_MIX_LEN*/

    if (AUDIO_DEC_BT_MIXER_EN) {

        u8 mix_src_en = 1;
#if AUDIO_CODEC_SUPPORT_SYNC
        mix_src_en = 0;
#endif

        audio_mode_main_dec_open(AUDIO_MODE_MAIN_STATE_DEC_ESCO);
        // 配置mixer通道参数
        audio_mixer_ch_open_head(&dec->mix_ch, &mixer); // 挂载到mixer最前面
        audio_mixer_ch_set_src(&dec->mix_ch, mix_src_en, 0);
        audio_mixer_ch_set_no_wait(&dec->mix_ch, 1, 10); // 超时自动丢数
        audio_mixer_ch_sample_sync_enable(&dec->mix_ch, mix_src_en);
        audio_mixer_ch_set_sample_rate(&dec->mix_ch, dec->dec.sample_rate);
    }

#if (RECORDER_MIX_EN)
    audio_mixer_ch_open_head(&dec->rec_mix_ch, &recorder_mixer); // 挂载到mixer最前面
    audio_mixer_ch_set_src(&dec->rec_mix_ch, 1, 0);
    audio_mixer_ch_set_no_wait(&dec->rec_mix_ch, 1, 10); // 超时自动丢数
    audio_mixer_ch_sample_sync_enable(&dec->rec_mix_ch, 1);
    audio_mixer_ch_set_sample_rate(&dec->mix_ch, dec->dec.sample_rate);
    /* audio_mixer_ch_set_sample_rate(&dec->rec_mix_ch, dec->dec.sample_rate); */
    printf("[%s], dec->dec.sample_rate = %d\n", __FUNCTION__, dec->dec.sample_rate);
#endif//RECORDER_MIX_EN

    u32 sr = dec->dec.sample_rate;
    if (AUDIO_DEC_BT_MIXER_EN) {
        sr = audio_mixer_get_sample_rate(&mixer);
    }
    // eq、drc音效
    u8 bit_wide = 0;
#if TCFG_EQ_ENABLE && TCFG_PHONE_EQ_ENABLE
    if (ESCO_DRC_EN) {
        bit_wide = 1;
    }
    dec->eq = esco_eq_open(sr, dec->dec.ch_num, bit_wide);// eq
#endif

#if TCFG_DRC_ENABLE && ESCO_DRC_EN
    dec->drc = esco_drc_open(sr, dec->dec.ch_num, bit_wide);//drc
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (bit_wide) {
        dec->convert = convet_data_open(0, 512);
    }
#endif/*TCFG_DRC_ENABLE*/
#endif/**/




    spectrum_switch_demo(0);//关闭频谱计算
#ifdef CONFIG_MIXER_CYCLIC
#if ESCO_DEC_OUT_DUAL
    if (dec->dec.out_ch_num == 2) {
        // 单独的单变双
        dec->ch_switch = channel_switch_open(AUDIO_CH_LR, (120 + 5) * 2 * 2);
    } else
#endif /* #if ESCO_DEC_OUT_DUAL */
    {
        // mixer_cyclic 中自带声道处理
        audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 0, BIT(0) | BIT(1));
#if (RECORDER_MIX_EN)
        audio_mixer_ch_set_aud_ch_out(&dec->rec_mix_ch, 0, BIT(0) | BIT(1));
#endif
    }

#else
    if (dec->dec.ch_num != dec->dec.out_ch_num) {
        u8 out_ch_type = AUDIO_CH_DIFF;
        if (dec->dec.out_ch_num == 4) {
            out_ch_type = AUDIO_CH_QUAD;
        } else if (dec->dec.out_ch_num == 2) {
            out_ch_type = AUDIO_CH_LR;
        }
        dec->ch_switch = channel_switch_open(out_ch_type, AUDIO_SYNTHESIS_LEN / 2);
    }
#endif

    dec->dec.decoder.entry.data_handler = esco_dec_data_handler;

    // sync初始化
#if AUDIO_CODEC_SUPPORT_SYNC
    memset(&dec->syncts, 0, sizeof(struct decoder_syncts));

    u8 out_ch = (dec->dec.out_ch_num << 4);

    dec->syncts.way_type = AUDIO_OUT_WAY_TYPE;
    if (AUDIO_DEC_BT_MIXER_EN) {
        esco_decoder_syncts_setup(&dec->syncts, dec->dec.sample_rate, audio_mixer_get_sample_rate(&mixer), dec->dec.ch_num | out_ch, dec->dec.esco_len);
    } else {
        esco_decoder_syncts_setup(&dec->syncts, dec->dec.sample_rate, dec->dec.sample_rate, dec->dec.ch_num | out_ch, dec->dec.esco_len);
    }
    /*使能同步，配置延时时间*/
    esco_decoder_stream_syncts_enable(&dec->dec, &dec->syncts, dec->dec.sample_rate, 25);

    //sound pcm 设置同步句柄
    sound_pcm_device_mount_syncts(dec->syncts.way_type, dec->syncts.syncts);
    audio_mixer_ch_set_event_handler(&dec->mix_ch, &dec->syncts, esco_event_handler);
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/

    // 数据流串联
    struct audio_stream_entry *entries[16] = {NULL};
    u8 entry_cnt = 0;
    entries[entry_cnt++] = &dec->dec.decoder.entry;

#if TCFG_AUDIO_OUTPUT_IIS
    struct audio_aec_ref *aec_ref_entry = audio_aec_ref_stream_open();
    entries[entry_cnt++] = &aec_ref_entry->entry;
#endif /*TCFG_AUDIO_OUTPUT_IIS*/

#if AUDIO_CODEC_SUPPORT_SYNC
    if (dec->syncts.syncts) {
        entries[entry_cnt++] = &dec->syncts.entry;
    }
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/


#if TCFG_EQ_ENABLE && TCFG_PHONE_EQ_ENABLE
    if (dec->eq) {
        entries[entry_cnt++] = &dec->eq->entry;
    }
#endif

#if TCFG_DRC_ENABLE && ESCO_DRC_EN
    if (dec->drc) {
        entries[entry_cnt++] = &dec->drc->entry;
    }
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->convert) {
        entries[entry_cnt++] = &dec->convert->entry;
    }
#endif
#endif

#if SYS_DIGVOL_GROUP_EN
    void *dvol_entry = sys_digvol_group_ch_open("call_esco", -1, NULL);
    entries[entry_cnt++] = dvol_entry;
#endif // SYS_DIGVOL_GROUP_EN

    if (dec->ch_switch) {
        entries[entry_cnt++] = &dec->ch_switch->entry;
    }

    if (AUDIO_DEC_BT_MIXER_EN) {
        entries[entry_cnt++] = &dec->mix_ch.entry;
    } else {
        default_dac.entry.prob_handler = esco_to_dac_probe_handler;
        entries[entry_cnt++] = &default_dac.entry;
    }
    // 创建数据流，把所有节点连接起来
    dec->stream = audio_stream_open(dec, esco_dec_out_stream_resume);
    audio_stream_add_list(dec->stream, entries, entry_cnt);

#if (RECORDER_MIX_EN)
    audio_stream_add_entry(entries[entry_cnt - 2], &dec->rec_mix_ch.entry);
#endif//RECORDER_MIX_EN
    // 设置音频输出类型
    audio_output_set_start_volume(APP_AUDIO_STATE_CALL);

#if TCFG_ESCO_PLC
    // 丢包修护
    dec->plc = esco_plc_init();
#endif

#if TCFG_ESCO_NOISEGATE
    NOISEGATE_PARM esco_noisegate_parm = {
        .attackTime = 300,
        .releaseTime = 5,
        .threshold = LIMITER_NOISE_GATE,
        .low_th_gain = LIMITER_NOISE_GAIN,
        .sampleRate = dec->dec.sample_rate,
        .channel = 1,
    };
    dec->noisegate = 	open_noisegate(&esco_noisegate_parm, 0, 0);
#endif /*TCFG_ESCO_NOISEGATE*/

#if TCFG_USER_TWS_ENABLE
    if (tws_network_audio_was_started()) {
        tws_network_local_audio_start();
        esco_decoder_join_tws(&dec->dec);
    }
#endif
    lmp_private_esco_suspend_resume(2);
    // 开始解码
    dec->dec.start = 1;
    err = audio_decoder_start(&dec->dec.decoder);
    if (err) {
        goto __err3;
    }
    dec->dec.frame_get = 0;

    err = audio_aec_init(dec->dec.sample_rate);
#if (TCFG_IIS_ENABLE && TCFG_IIS_OUTPUT_EN)
    audio_aec_ref_src_open();
#endif //(TCFG_IIS_ENABLE && TCFG_IIS_OUTPUT_EN)
    if (err) {
        log_i("audio_aec_init failed:%d", err);
        //goto __err3;
    }
    err = esco_enc_open(dec->dec.coding_type, dec->dec.esco_len);
    if (err) {
        log_i("audio_enc_open failed:%d", err);
        goto __err3;
    }
    dec->dec.enc_start = 1;

    clock_set_cur();
    return 0;

__err3:
    dec->dec.start = 0;
#if TCFG_EQ_ENABLE &&TCFG_PHONE_EQ_ENABLE
    esco_eq_close(dec->eq);
#endif
#if TCFG_DRC_ENABLE && ESCO_DRC_EN
    esco_drc_close(dec->drc);
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(dec->convert);
#endif


    spectrum_switch_demo(1);//打开频谱计算
    channel_switch_close(&dec->ch_switch);
    if (AUDIO_DEC_BT_MIXER_EN) {
        audio_mixer_ch_close(&dec->mix_ch);
    } else {
        bt_dec_stream_run_stop(&dec->dec.decoder);
    }
#if (RECORDER_MIX_EN)
    audio_mixer_ch_close(&dec->rec_mix_ch);
#endif//RECORDER_MIX_EN

#if AUDIO_CODEC_SUPPORT_SYNC
    sound_pcm_device_unmount_syncts(dec->syncts.way_type, dec->syncts.syncts);
    esco_decoder_syncts_free(&dec->syncts);
#endif /*AUDIO_CODEC_SUPPORT_SYNC*/

#if SYS_DIGVOL_GROUP_EN
    sys_digvol_group_ch_close("call_esco");
#endif // SYS_DIGVOL_GROUP_EN


    if (dec->stream) {
        audio_stream_close(dec->stream);
        dec->stream = NULL;
    }

__err2:
    esco_decoder_close(&dec->dec);
__err1:
    esco_dec_release();
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    esco解码资源等待
   @param    *wait: 句柄
   @param    event: 事件
   @return   0：成功
   @note     用于多解码打断处理
*/
/*----------------------------------------------------------------------------*/
static int esco_wait_res_handler(struct audio_res_wait *wait, int event)
{
    int err = 0;
    log_d("esco_wait_res_handler %d\n", event);
    if (event == AUDIO_RES_GET) {
        err = esco_dec_start();
    } else if (event == AUDIO_RES_PUT) {
        if (bt_esco_dec->dec.start) {
            lmp_private_esco_suspend_resume(1);
            esco_audio_res_close();
        }
    }

    return err;
}

#define ESCO_SIRI_WAKEUP()      (app_var.siri_stu == 1 || app_var.siri_stu == 2)
static void esco_smart_voice_detect_handler(void)
{
#if TCFG_SMART_VOICE_ENABLE
#if TCFG_CALL_KWS_SWITCH_ENABLE
    if (ESCO_SIRI_WAKEUP() || (get_call_status() != BT_CALL_INCOMING)) {
        audio_smart_voice_detect_close();
    }
#else
    audio_smart_voice_detect_close();
#endif
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief    打开esco解码
   @param    *param: 媒体信息
   @param    mutw: 静音
   @return   0: 成功
   @note
*/
/*----------------------------------------------------------------------------*/
int esco_dec_open(void *param, u8 mute)
{
    int err;
    struct esco_dec_hdl *dec;
    u32 esco_param = *(u32 *)param;
    int esco_len = esco_param >> 16;
    int codec_type = esco_param & 0x000000ff;

    log_i("esco_dec_open, type=%d,len=%d\n", codec_type, esco_len);

    esco_smart_voice_detect_handler();
    dec = zalloc(sizeof(*dec));
    if (!dec) {
        return -ENOMEM;
    }

#if TCFG_DEC2TWS_ENABLE
    localtws_media_disable();
#endif

    bt_esco_dec = dec;

    dec->tws_mute_en = mute;
    dec->dec.esco_len = esco_len;
#if (TCFG_AUDIO_DAC_CONNECT_MODE == DAC_OUTPUT_FRONT_LR_REAR_LR)

#if !TCFG_EQ_DIVIDE_ENABLE
    dec->dec.out_ch_num = 2;
#else
    dec->dec.out_ch_num = 4;
#endif
#else
    dec->dec.out_ch_num = audio_output_channel_num();
#endif

    printf("dec->dec.out_ch_num %d\n", dec->dec.out_ch_num);
    if (codec_type == 3) {
        dec->dec.coding_type = AUDIO_CODING_MSBC;
        dec->dec.sample_rate = 16000;
        dec->dec.ch_num = 1;
        clock_add(DEC_MSBC_CLK);
    } else if (codec_type == 2) {
        dec->dec.coding_type = AUDIO_CODING_CVSD;
        dec->dec.sample_rate = 8000;
        dec->dec.ch_num = 1;
        clock_add(DEC_CVSD_CLK);
    }

    dec->wait.priority = 2;		// 解码优先级
    dec->wait.preemption = 0;	// 不使能直接抢断解码
    dec->wait.snatch_same_prio = 1;	// 可抢断同优先级解码
    dec->wait.handler = esco_wait_res_handler;
    err = audio_decoder_task_add_wait(&decode_task, &dec->wait);
    if (bt_esco_dec && (bt_esco_dec->dec.start == 0)) {
        lmp_private_esco_suspend_resume(1);
    }

#if AUDIO_OUTPUT_AUTOMUTE
    extern void mix_out_automute_skip(u8 skip);
    mix_out_automute_skip(1);
#endif

    return err;
}


/*----------------------------------------------------------------------------*/
/**@brief    关闭esco解码
   @param    :
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void esco_dec_close()
{
    if (!bt_esco_dec) {
        return;
    }

    esco_audio_res_close();
    esco_dec_release();

    log_i("esco_dec_close: exit\n");
#if AUDIO_OUTPUT_AUTOMUTE
    extern void mix_out_automute_skip(u8 skip);
    mix_out_automute_skip(0);
#endif
    clock_set_cur();
#if TCFG_DEC2TWS_ENABLE
    localtws_media_enable();
#endif
#if TCFG_SMART_VOICE_ENABLE
    audio_smart_voice_detect_open(JL_KWS_COMMAND_KEYWORD);
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief    蓝牙音频正在运行
   @param
   @return   1: 正在运行
   @note
*/
/*----------------------------------------------------------------------------*/
u8 bt_audio_is_running(void)
{
    return (bt_a2dp_dec || bt_esco_dec);
}
/*----------------------------------------------------------------------------*/
/**@brief    蓝牙播放正在运行
   @param
   @return   1: 正在运行
   @note
*/
/*----------------------------------------------------------------------------*/
u8 bt_media_is_running(void)
{
    return bt_a2dp_dec != NULL;
}
/*----------------------------------------------------------------------------*/
/**@brief    蓝牙电话正在运行
   @param
   @return   1: 正在运行
   @note
*/
/*----------------------------------------------------------------------------*/
u8 bt_phone_dec_is_running()
{
    return bt_esco_dec != NULL;
}


/*----------------------------------------------------------------------------*/
/**@brief    蓝牙解码idle判断
   @param
   @return   1: idle
   @return   0: busy
   @note
*/
/*----------------------------------------------------------------------------*/
static u8 bt_dec_idle_query()
{
    if (bt_audio_is_running()) {
        return 0;
    }

    return 1;
}
REGISTER_LP_TARGET(bt_dec_lp_target) = {
    .name = "bt_dec",
    .is_idle = bt_dec_idle_query,
};


