/*
 ****************************************************************
 *File : audio_dc_linein.c
 *Note :
 *
 ****************************************************************
 */


#include "asm/includes.h"
#include "media/includes.h"
#include "media/pcm_decoder.h"
#include "system/includes.h"
#include "effectrs_sync.h"
#include "app_config.h"
#include "audio_config.h"
#include "audio_dec.h"
#include "app_config.h"
#include "app_main.h"
#include "audio_enc.h"
#include "clock_cfg.h"
#include "dev_manager.h"
#include "media/convert_data.h"
#include "audio_effect/audio_dynamic_eq_demo.h"
#include "media/effects_adj.h"
#include "audio_enc_recoder.h"
#include "audio_effect/audio_eff_default_parm.h"
#if TCFG_PCM_ENC2TWS_ENABLE
#include "bt_tws.h"
#endif

#if (TCFG_LINEIN_ENABLE || TCFG_FM_ENABLE)//外部收音走linein

//////////////////////////////////////////////////////////////////////////////

struct linein_dec_hdl {
    struct audio_stream *stream;	// 音频流
    struct pcm_decoder pcm_dec;		// pcm解码句柄
    struct audio_res_wait wait;		// 资源等待句柄
    struct audio_mixer_ch mix_ch;	// 叠加句柄
#if (RECORDER_MIX_EN)
    struct audio_mixer_ch rec_mix_ch;	// 叠加句柄
#endif/*RECORDER_MIX_EN*/

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
#if TCFG_EQ_DIVIDE_ENABLE
    struct audio_eq *eq_rl_rr;    //eq drc句柄
    struct audio_drc *drc_rl_rr;    // drc句柄
    struct convert_data *convert_rl_rr;
    struct aud_gain_process *gain_rl_rr;
    struct audio_vocal_tract vocal_tract;//声道合并目标句柄
    struct audio_vocal_tract_ch synthesis_ch_fl_fr;//声道合并句柄
    struct audio_vocal_tract_ch synthesis_ch_rl_rr;//声道合并句柄
    struct channel_switch *ch_switch;//声道变换
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    struct audio_eq *ext_eq2;    //eq drc句柄 扩展eq
#endif
#endif

    u32 id;				// 唯一标识符，随机值
    u32 start : 1;		// 正在解码
    u32 source : 8;		// linein音频源
    void *linein;		// 底层驱动句柄
};


//////////////////////////////////////////////////////////////////////////////

struct linein_dec_hdl *linein_dec = NULL;	// linein解码句柄

//////////////////////////////////////////////////////////////////////////////
int linein_sample_size(void *hdl);
int linein_sample_total(void *hdl);


//////////////////////////////////////////////////////////////////////////////

/*----------------------------------------------------------------------------*/
/**@brief    linein解码释放
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void linein_dec_relaese()
{
    if (linein_dec) {
        audio_decoder_task_del_wait(&decode_task, &linein_dec->wait);
        clock_remove(AUDIO_CODING_PCM);
        local_irq_disable();
        free(linein_dec);
        linein_dec = NULL;
        local_irq_enable();
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    linein解码事件处理
   @param    *decoder: 解码器句柄
   @param    argc: 参数个数
   @param    *argv: 参数
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void linein_dec_event_handler(struct audio_decoder *decoder, int argc, int *argv)
{
    switch (argv[0]) {
    case AUDIO_DEC_EVENT_END:
        if (!linein_dec) {
            log_i("linein_dec handle err ");
            break;
        }

        if (linein_dec->id != argv[1]) {
            log_w("linein_dec id err : 0x%x, 0x%x \n", linein_dec->id, argv[1]);
            break;
        }

        linein_dec_close();
        //audio_decoder_resume_all(&decode_task);
        break;
    }
}


/*----------------------------------------------------------------------------*/
/**@brief    linein解码数据输出
   @param    *entry: 音频流句柄
   @param    *in: 输入信息
   @param    *out: 输出信息
   @return   输出长度
   @note     *out未使用
*/
/*----------------------------------------------------------------------------*/
static int linein_dec_data_handler(struct audio_stream_entry *entry,
                                   struct audio_data_frame *in,
                                   struct audio_data_frame *out)
{
    struct audio_decoder *decoder = container_of(entry, struct audio_decoder, entry);
    struct pcm_decoder *pcm_dec = container_of(decoder, struct pcm_decoder, decoder);
    struct linein_dec_hdl *dec = container_of(pcm_dec, struct linein_dec_hdl, pcm_dec);
    if (!dec->start) {
        return 0;
    }
    audio_stream_run(&decoder->entry, in);
    return decoder->process_len;
}

/*----------------------------------------------------------------------------*/
/**@brief    linein解码数据流激活
   @param    *p: 私有句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void linein_dec_out_stream_resume(void *p)
{
    struct linein_dec_hdl *dec = p;
    audio_decoder_resume(&dec->pcm_dec.decoder);
}

/*----------------------------------------------------------------------------*/
/**@brief    linein解码激活
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void linein_dec_resume(void)
{
    if (linein_dec) {
        audio_decoder_resume(&linein_dec->pcm_dec.decoder);
    }
}

/*----------------------------------------------------------------------------*/
/**@brief    计算linein输入采样率
   @param
   @return   采样率
   @note
*/
/*----------------------------------------------------------------------------*/
static int audio_linein_input_sample_rate(void *priv)
{
    struct linein_dec_hdl *dec = (struct linein_dec_hdl *)priv;
    int sample_rate = linein_stream_sample_rate(dec->linein);
    int buf_size = linein_sample_size(dec->linein);
#if TCFG_PCM_ENC2TWS_ENABLE
    if (dec->pcm_dec.dec_no_out_sound) {
        /*TWS的上限在linein输入buffer，下限在tws push buffer*/
        if (buf_size >= linein_sample_total(dec->linein) / 2) {
            sample_rate += (sample_rate * 1 / 10000);
        } else if (tws_api_local_media_trans_check_ready_total() < 1024) {
            sample_rate -= (sample_rate * 1 / 10000);
        }
        return sample_rate;
    }
#endif
    /* if (dec->pcm_dec.sample_rate == 44100 && audio_mixer_get_sample_rate(&mixer) == 44100) { */
    if ((dec->pcm_dec.sample_rate == 44100) && (audio_mixer_get_original_sample_rate_by_type(&mixer, MIXER_SR_SPEC) == 44100)) {
        sample_rate = 44100 + (sample_rate - 44118);
    }
    if (buf_size >= (linein_sample_total(dec->linein) * 3 / 5)) {
        sample_rate += (sample_rate * 6 / 10000);
    }
    if (buf_size <= (linein_sample_total(dec->linein) * 2 / 5)) {
        sample_rate -= (sample_rate * 6 / 10000);
    }
    return sample_rate;
}

/*----------------------------------------------------------------------------*/
/**@brief    linein解码开始
   @param
   @return   0：成功
   @return   非0：失败
   @note
*/
/*----------------------------------------------------------------------------*/
int linein_dec_start()
{
    int err;
    struct linein_dec_hdl *dec = linein_dec;
    struct audio_mixer *p_mixer = &mixer;

    if (!linein_dec) {
        return -EINVAL;
    }

    err = pcm_decoder_open(&dec->pcm_dec, &decode_task);
    if (err) {
        goto __err1;
    }

    // 打开linein驱动
    dec->linein = linein_sample_open(dec->source, dec->pcm_dec.sample_rate);
    linein_sample_set_resume_handler(dec->linein, linein_dec_resume);

    pcm_decoder_set_event_handler(&dec->pcm_dec, linein_dec_event_handler, dec->id);
    pcm_decoder_set_read_data(&dec->pcm_dec, linein_sample_read, dec->linein);
    pcm_decoder_set_data_handler(&dec->pcm_dec, linein_dec_data_handler);

#if TCFG_PCM_ENC2TWS_ENABLE
    {
        // localtws使用sbc等编码转发
        struct audio_fmt enc_f;
        memcpy(&enc_f, &dec->pcm_dec.decoder.fmt, sizeof(struct audio_fmt));
        enc_f.coding_type = AUDIO_CODING_SBC;
        if (dec->pcm_dec.ch_num == 2) { // 如果是双声道数据，localtws在解码时才变成对应声道
            enc_f.channel = 2;
        }
        int ret = localtws_enc_api_open(&enc_f, LOCALTWS_ENC_FLAG_STREAM);
        if (ret == true) {
            dec->pcm_dec.dec_no_out_sound = 1;
            // 重定向mixer
            p_mixer = &g_localtws.mixer;
            // 关闭资源等待。最终会在localtws解码处等待
            audio_decoder_task_del_wait(&decode_task, &dec->wait);
            if (dec->pcm_dec.output_ch_num != enc_f.channel) {
                dec->pcm_dec.output_ch_num = dec->pcm_dec.decoder.fmt.channel = enc_f.channel;
                if (enc_f.channel == 2) {
                    dec->pcm_dec.output_ch_type = AUDIO_CH_LR;
                } else {
                    dec->pcm_dec.output_ch_type = AUDIO_CH_DIFF;
                }
            }
        }
    }
#endif

    if (!dec->pcm_dec.dec_no_out_sound) {
        audio_mode_main_dec_open(AUDIO_MODE_MAIN_STATE_DEC_LINEIN);
    }

    // 设置叠加功能
    audio_mixer_ch_open_head(&dec->mix_ch, p_mixer);
    audio_mixer_ch_set_no_wait(&dec->mix_ch, 1, 10); // 超时自动丢数
#if (RECORDER_MIX_EN)
    audio_mixer_ch_open_head(&dec->rec_mix_ch, &recorder_mixer);
    audio_mixer_ch_set_no_wait(&dec->rec_mix_ch, 1, 10); // 超时自动丢数
#endif/*RECORDER_MIX_EN*/

#if 0
    if (dec->pcm_dec.dec_no_out_sound) {
        // 自动变采样
        audio_mixer_ch_set_src(&dec->mix_ch, 1, 0);
#if (RECORDER_MIX_EN)
        audio_mixer_ch_set_src(&dec->rec_mix_ch, 1, 0);
#endif/*RECORDER_MIX_EN*/
    } else {
        // 根据buf数据量动态变采样
        struct audio_mixer_ch_sync_info info = {0};
        info.priv = dec->linein;
        info.get_total = linein_sample_total;
        info.get_size = linein_sample_size;
#if (!SOUNDCARD_ENABLE)
        audio_mixer_ch_set_sync(&dec->mix_ch, &info, 1, 1);
#endif/*SOUNDCARD_ENABLE*/
#if (RECORDER_MIX_EN)
        audio_mixer_ch_set_sync(&dec->rec_mix_ch, &info, 1, 1);
#endif/*RECORDER_MIX_EN*/
    }
#else /*0*/

    if (dec->pcm_dec.dec_no_out_sound) {
        audio_mixer_ch_follow_resample_enable(&dec->mix_ch, dec, audio_linein_input_sample_rate);
#if (RECORDER_MIX_EN)
        audio_mixer_ch_follow_resample_enable(&dec->rec_mix_ch, dec, audio_linein_input_sample_rate);
#endif/*RECORDER_MIX_EN*/
    } else {
#if (!SOUNDCARD_ENABLE)
        audio_mixer_ch_set_sample_rate(&dec->mix_ch, dec->pcm_dec.sample_rate);
        if ((dec->pcm_dec.sample_rate == audio_mixer_get_sample_rate(p_mixer)) && (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)) {
        } else {
            audio_mixer_ch_follow_resample_enable(&dec->mix_ch, dec, audio_linein_input_sample_rate);
        }
#endif/*SOUNDCARD_ENABLE*/
#if (RECORDER_MIX_EN)
        audio_mixer_ch_follow_resample_enable(&dec->rec_mix_ch, dec, audio_linein_input_sample_rate);
#endif/*RECORDER_MIX_EN*/
    }
#endif  /*0*/

#if AUDIO_SURROUND_CONFIG
    //环绕音效
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->surround = surround_open_demo(AEID_AUX_SURROUND, dec->pcm_dec.output_ch_type);
#else
    dec->surround = surround_open_demo(AEID_MUSIC_SURROUND, dec->pcm_dec.output_ch_type);
#endif
#endif


#if AUDIO_VBASS_CONFIG
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->vbass_prev_gain = audio_gain_open_demo(AEID_AUX_VBASS_PREV_GAIN, dec->pcm_dec.output_ch_num);
    dec->ns_gate = audio_noisegate_open_demo(AEID_AUX_NS_GATE, dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
    //虚拟低音
    dec->vbass = audio_vbass_open_demo(AEID_AUX_VBASS, dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#else
    dec->vbass_prev_gain = audio_gain_open_demo(AEID_MUSIC_VBASS_PREV_GAIN, dec->pcm_dec.output_ch_num);
    dec->ns_gate = audio_noisegate_open_demo(AEID_MUSIC_NS_GATE, dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
    //虚拟低音
    dec->vbass = audio_vbass_open_demo(AEID_MUSIC_VBASS, dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#endif
#endif

#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->high_bass = high_bass_aux_eq_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
    dec->hb_drc = high_bass_aux_drc_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#else
    dec->high_bass = high_bass_eq_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
    dec->hb_drc = high_bass_drc_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->hb_drc && dec->hb_drc->run32bit) {
        dec->hb_convert = convet_data_open(0, 512);
    }
#endif
#endif

#if TCFG_EQ_ENABLE && TCFG_LINEIN_MODE_EQ_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->eq = linein_eq_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);// eq
#else
    dec->eq = music_eq_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);// eq
#endif
#if TCFG_DRC_ENABLE && TCFG_LINEIN_MODE_DRC_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->drc = linein_drc_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);//drc
#else
    dec->drc = music_drc_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);//drc
#endif
#endif/*TCFG_LINEIN_MODE_DRC_ENABLE*/
#if !TCFG_DYNAMIC_EQ_ENABLE
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->eq && dec->eq->out_32bit) {
        dec->convert = convet_data_open(0, 512);
    }
#endif
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->ext_eq = aux_ext_eq_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#else
    dec->ext_eq = music_ext_eq_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#endif
#endif

#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->eq2 = linein_eq2_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);// eq
    dec->dy_eq = audio_dynamic_eq_ctrl_open(AEID_AUX_DYNAMIC_EQ, dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);//动态eq
#else
    dec->eq2 = music_eq2_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);// eq
    dec->dy_eq = audio_dynamic_eq_ctrl_open(AEID_MUSIC_DYNAMIC_EQ, dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);//动态eq
#endif

    dec->convert2 = convet_data_open(0, 512);
#endif/*TCFG_DYNAMIC_EQ_ENABLE*/
    u16 gain_name =  AEID_MUSIC_GAIN;
    u16 swap_name = AEID_MUSIC_CH_SWAP;
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    gain_name =  AEID_AUX_GAIN;
    swap_name = AEID_AUX_CH_SWAP;
#endif
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    dec->gain = audio_gain_open_demo(gain_name, dec->pcm_dec.output_ch_num);
#endif
#endif/*TCFG_LINEIN_MODE_EQ_ENABLE*/



#if TCFG_EQ_DIVIDE_ENABLE

#if TCFG_EQ_ENABLE && TCFG_LINEIN_MODE_EQ_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    dec->eq_rl_rr = aux_eq_rl_rr_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);// eq
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    dec->drc_rl_rr = aux_drc_rl_rr_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);//drc
#endif
#else
    dec->eq_rl_rr = music_eq_rl_rr_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);// eq
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    dec->drc_rl_rr = music_drc_rl_rr_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);//drc
#endif
#endif

    if (dec->eq_rl_rr && dec->eq_rl_rr->out_32bit) {
        dec->convert_rl_rr = convet_data_open(0, 512);
    }
#endif

    dec->gain_rl_rr = audio_gain_open_demo(AEID_MUSIC_RL_GAIN, dec->pcm_dec.output_ch_num);
    if (dec->eq_rl_rr) {
        audio_vocal_tract_open(&dec->vocal_tract, AUDIO_SYNTHESIS_LEN);
        {
            u8 entry_cnt = 0;
            struct audio_stream_entry *entries[8] = {NULL};
            entries[entry_cnt++] = &dec->vocal_tract.entry;
            entries[entry_cnt++] = &dec->mix_ch.entry;
            dec->vocal_tract.stream = audio_stream_open(&dec->vocal_tract, audio_vocal_tract_stream_resume);
            audio_stream_add_list(dec->vocal_tract.stream, entries, entry_cnt);
        }
        audio_vocal_tract_synthesis_open(&dec->synthesis_ch_fl_fr, &dec->vocal_tract, FL_FR);
        audio_vocal_tract_synthesis_open(&dec->synthesis_ch_rl_rr, &dec->vocal_tract, RL_RR);
    } else {
        dec->ch_switch = channel_switch_open(AUDIO_CH_QUAD, AUDIO_SYNTHESIS_LEN / 2);
    }
#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
    dec->ext_eq2 = music_ext_eq2_open(dec->pcm_dec.sample_rate, dec->pcm_dec.output_ch_num);
#endif
#ifdef CONFIG_MIXER_CYCLIC
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 0, BIT(0));
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 1, BIT(1));
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 2, BIT(2));
    audio_mixer_ch_set_aud_ch_out(&dec->mix_ch, 3, BIT(3));
#endif

#endif

    // 数据流串联
    struct audio_stream_entry *entries[32] = {NULL};
    u8 entry_cnt = 0;
    u8 rl_rr_entry_start = 0;
    entries[entry_cnt++] = &dec->pcm_dec.decoder.entry;

#if SYS_DIGVOL_GROUP_EN
    void *dvol_entry = sys_digvol_group_ch_open("music_linein", -1, NULL);
    entries[entry_cnt++] = dvol_entry;
#endif // SYS_DIGVOL_GROUP_EN





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
#if TCFG_EQ_ENABLE && TCFG_LINEIN_MODE_EQ_ENABLE
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




#if TCFG_EQ_DIVIDE_ENABLE
    if (dec->eq_rl_rr) {
        entries[entry_cnt++] = &dec->synthesis_ch_fl_fr.entry;//四声道eq独立时，该节点后不接节点
    } else {
        if (dec->ch_switch) {
            entries[entry_cnt++] = &dec->ch_switch->entry;
        }
        entries[entry_cnt++] = &dec->mix_ch.entry;
    }
#else
    entries[entry_cnt++] = &dec->mix_ch.entry;
#endif

    // 创建数据流，把所有节点连接起来
    dec->stream = audio_stream_open(dec, linein_dec_out_stream_resume);
    audio_stream_add_list(dec->stream, entries, entry_cnt);
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
#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DR
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

#if (RECORDER_MIX_EN)
    audio_stream_add_entry(entries[entry_cnt - 2], &dec->rec_mix_ch.entry);
#endif/*RECORDER_MIX_EN*/

    // 设置音频输出音量
    audio_output_set_start_volume(APP_AUDIO_STATE_MUSIC);

    // 开始解码
    dec->start = 1;
    err = audio_decoder_start(&dec->pcm_dec.decoder);
    if (err) {
        goto __err3;
    }
    clock_set_cur();
    return 0;
__err3:
    dec->start = 0;

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

#if TCFG_EQ_ENABLE && TCFG_LINEIN_MODE_EQ_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    linein_eq_close(dec->eq);
#else
    music_eq_close(dec->eq);
#endif
#if TCFG_DRC_ENABLE && TCFG_LINEIN_MODE_DRC_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    linein_drc_close(dec->drc);
#else
    music_drc_close(dec->drc);
#endif
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    convet_data_close(dec->convert);
#endif

#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
    aux_ext_eq_close(dec->ext_eq);
#else
    music_ext_eq_close(dec->ext_eq);
#endif
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

    if (dec->linein) {
        linein_sample_close(dec->linein);
        dec->linein = NULL;
    }
    audio_mixer_ch_close(&dec->mix_ch);
#if (RECORDER_MIX_EN)
    audio_mixer_ch_close(&dec->rec_mix_ch);
#endif/*RECORDER_MIX_EN*/
#if TCFG_PCM_ENC2TWS_ENABLE
    if (linein_dec->pcm_dec.dec_no_out_sound) {
        linein_dec->pcm_dec.dec_no_out_sound = 0;
        localtws_enc_api_close();
    }
#endif
#if SYS_DIGVOL_GROUP_EN
    sys_digvol_group_ch_close("music_linein");
#endif // SYS_DIGVOL_GROUP_EN


    if (dec->stream) {
        audio_stream_close(dec->stream);
        dec->stream = NULL;
    }

    pcm_decoder_close(&dec->pcm_dec);
__err1:
    linein_dec_relaese();
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    fm解码关闭
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void __linein_dec_close(void)
{
    if (linein_dec && linein_dec->start) {
        linein_dec->start = 0;

        pcm_decoder_close(&linein_dec->pcm_dec);
        linein_sample_close(linein_dec->linein);
        linein_dec->linein = NULL;

#if AUDIO_SURROUND_CONFIG
        surround_close_demo(linein_dec->surround);
#endif



#if AUDIO_VBASS_CONFIG
        audio_gain_close_demo(linein_dec->vbass_prev_gain);
        audio_noisegate_close_demo(linein_dec->ns_gate);
        audio_vbass_close_demo(linein_dec->vbass);
#endif

#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
        high_bass_eq_close(linein_dec->high_bass);
        high_bass_drc_close(linein_dec->hb_drc);
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
        convet_data_close(linein_dec->hb_convert);
#endif
#endif

#if TCFG_EQ_ENABLE && TCFG_LINEIN_MODE_EQ_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        linein_eq_close(linein_dec->eq);
#else
        music_eq_close(linein_dec->eq);
#endif
#if TCFG_DRC_ENABLE && TCFG_LINEIN_MODE_DRC_ENABLE
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        linein_drc_close(linein_dec->drc);
#else
        music_drc_close(linein_dec->drc);
#endif
#endif
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
        convet_data_close(linein_dec->convert);
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        aux_ext_eq_close(linein_dec->ext_eq);
#else
        music_ext_eq_close(linein_dec->ext_eq);
#endif
#endif

#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
        music_eq2_close(linein_dec->eq2);
        audio_dynamic_eq_ctrl_close(linein_dec->dy_eq);
        convet_data_close(linein_dec->convert2);
#endif
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
        audio_gain_close_demo(linein_dec->gain);
#endif
#endif

#if TCFG_EQ_DIVIDE_ENABLE
        music_eq_rl_rr_close(linein_dec->eq_rl_rr);
        music_drc_rl_rr_close(linein_dec->drc_rl_rr);
        convet_data_close(linein_dec->convert_rl_rr);
        audio_gain_close_demo(linein_dec->gain_rl_rr);

        audio_vocal_tract_synthesis_close(&linein_dec->synthesis_ch_fl_fr);
        audio_vocal_tract_synthesis_close(&linein_dec->synthesis_ch_rl_rr);
        audio_vocal_tract_close(&linein_dec->vocal_tract);
        channel_switch_close(&linein_dec->ch_switch);
#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
        music_ext_eq2_close(linein_dec->ext_eq2);
#endif
#endif
        audio_mixer_ch_close(&linein_dec->mix_ch);
#if (RECORDER_MIX_EN)
        audio_mixer_ch_close(&linein_dec->rec_mix_ch);
#endif/*RECORDER_MIX_EN*/
#if TCFG_PCM_ENC2TWS_ENABLE
        if (linein_dec->pcm_dec.dec_no_out_sound) {
            linein_dec->pcm_dec.dec_no_out_sound = 0;
            localtws_enc_api_close();
        }
#endif
#if SYS_DIGVOL_GROUP_EN
        sys_digvol_group_ch_close("music_linein");
#endif // SYS_DIGVOL_GROUP_EN


        // 先关闭各个节点，最后才close数据流
        if (linein_dec->stream) {
            audio_stream_close(linein_dec->stream);
            linein_dec->stream = NULL;
        }

    }
}

/*----------------------------------------------------------------------------*/
/**@brief    linein解码资源等待
   @param    *wait: 句柄
   @param    event: 事件
   @return   0：成功
   @note     用于多解码打断处理
*/
/*----------------------------------------------------------------------------*/
static int linein_wait_res_handler(struct audio_res_wait *wait, int event)
{
    int err = 0;
    log_i("linein_wait_res_handler, event:%d\n", event);
    if (event == AUDIO_RES_GET) {
        // 启动解码
        err = linein_dec_start();
    } else if (event == AUDIO_RES_PUT) {
        // 被打断
        __linein_dec_close();
    }

    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    打开linein解码
   @param    source: 音频源
   @param    sample_rate: 采样率
   @return   0：成功
   @return   非0：失败
   @note
*/
/*----------------------------------------------------------------------------*/
int linein_dec_open(u8 source, u32 sample_rate)
{
    int err;
    struct linein_dec_hdl *dec;
    dec = zalloc(sizeof(*dec));
    if (!dec) {
        return -ENOMEM;
    }
    linein_dec = dec;
    dec->id = rand32();

    dec->pcm_dec.ch_num = 0;
    if (source & AUDIO_ADC_LINE0) {
        dec->pcm_dec.ch_num++;
    }
    if (source & AUDIO_ADC_LINE1) {
        dec->pcm_dec.ch_num++;
    }
    if (source & AUDIO_ADC_LINE2) {
        dec->pcm_dec.ch_num++;
    }
    if (source & AUDIO_ADC_LINE3) {
        dec->pcm_dec.ch_num++;
    }

    ASSERT(dec->pcm_dec.ch_num <= 2, "%d \n", dec->pcm_dec.ch_num);
    dec->source = source;

    dec->pcm_dec.output_ch_num = audio_output_channel_num();
    dec->pcm_dec.output_ch_type = audio_output_channel_type();
#if TCFG_MIC_EFFECT_ENABLE
    dec->pcm_dec.sample_rate = MIC_EFFECT_SAMPLERATE;
#else
    dec->pcm_dec.sample_rate = sample_rate;
#endif
    dec->wait.priority = 2;
    dec->wait.preemption = 0;
    dec->wait.snatch_same_prio = 1;
#if SOUNDCARD_ENABLE
    dec->wait.protect = 1;
#endif
    dec->wait.handler = linein_wait_res_handler;
    clock_add(AUDIO_CODING_PCM);


#if TCFG_DEC2TWS_ENABLE
    // 设置localtws重播接口
    localtws_globle_set_dec_restart(linein_dec_push_restart);
#endif

    err = audio_decoder_task_add_wait(&decode_task, &dec->wait);
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    关闭linein解码
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void linein_dec_close(void)
{
    if (!linein_dec) {
        return;
    }

    __linein_dec_close();
    linein_dec_relaese();
    clock_set_cur();
    log_i("linein dec close \n\n ");
}

/*----------------------------------------------------------------------------*/
/**@brief    linein解码重新开始
   @param    id: 文件解码id
   @return   0：成功
   @return   非0：失败
   @note
*/
/*----------------------------------------------------------------------------*/
int linein_dec_restart(int id)
{
    if ((!linein_dec) || (id != linein_dec->id)) {
        return -1;
    }
    u8 source = linein_dec->source;
    u32 sample_rate = linein_dec->pcm_dec.sample_rate;
    linein_dec_close();
    int err = linein_dec_open(source, sample_rate);
    return err;
}

/*----------------------------------------------------------------------------*/
/**@brief    推送linein解码重新开始命令
   @param
   @return   true：成功
   @return   false：失败
   @note
*/
/*----------------------------------------------------------------------------*/
int linein_dec_push_restart(void)
{
    if (!linein_dec) {
        return false;
    }
    int argv[3];
    argv[0] = (int)linein_dec_restart;
    argv[1] = 1;
    argv[2] = (int)linein_dec->id;
    os_taskq_post_type(os_current_task(), Q_CALLBACK, ARRAY_SIZE(argv), argv);
    return true;
}
#endif
