/*
 ****************************************************************
 *File : audio_spdif.c
 *Note :
 *
 ****************************************************************
***********************   spdif_dec ******************************/
#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "effectrs_sync.h"
#include "media/audio_eq_drc_apply.h"
#include "app_config.h"
#include "audio_config.h"
#include "audio_dec.h"
#include "app_config.h"
#include "app_main.h"
#include "audio_enc.h"
#include "clock_cfg.h"
#include "media/pcm_decoder.h"

#if TCFG_SPDIF_ENABLE

#define SPDIF_DEC_DATA_BUF_LEN 1024*4
struct s_spdif_decode {
    struct pcm_decoder pcm_dec;
    struct file_decoder file_dec;
    struct audio_stream *audio_stream;		// 音频流
    struct audio_res_wait wait;
    struct audio_mixer_ch mix_ch;
    u32 coding_type;
    struct audio_fmt fmt;
    volatile  u32 pcm_sr;
    cbuffer_t dec_cbuf;
    u8 dec_data_buf[SPDIF_DEC_DATA_BUF_LEN];
    u8 status;

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

};

static void reset_pcm_sample_rate(u32 sr);
int spdif_dec_open(struct audio_fmt fmt);
void spdif_dec_close(void);

struct audio_spdif_hdl spdif_slave_hdl;
struct s_spdif_decode *spdif_dec_hdl = NULL;

int spdif_non_linear_packet(u16 *pkt, u16 pkt_len)
{
    enum IEC61937DataType data_type;
    u16 search_index = 0;
    int data_cp_begin_idx = 0;
    int data_cp_len = 0;
    u16 *data = pkt;
    u16 *data_ptr = NULL;
    int zero_cnt = 0;
    u32 state = 0;
    int pkt_size_bits, offset, skip_len;
    if (spdif_slave_hdl.length_code > 0) {
        if (spdif_slave_hdl.length_code >= SPDIF_DATA_DAM_LEN * 2) {
            data_cp_begin_idx = 0;
            data_cp_len       = SPDIF_DATA_DAM_LEN * 2;
            spdif_slave_hdl.length_code       -= data_cp_len;
        } else {
            data_cp_begin_idx    = 0;
            data_cp_len = spdif_slave_hdl.length_code;
            spdif_slave_hdl.length_code = 0;
        }
        //写入解码cbuf
        /* int spdif_dec_write_data(s16 *data, int len) */
        spdif_dec_write_data(&data[data_cp_begin_idx], data_cp_len * 2);
        if (spdif_slave_hdl.length_code != 0) {
            return 0;
        } else {
            search_index = data_cp_len;
        }
    }
    if (spdif_slave_hdl.find_sync_word != 0) {
        printf("[last:%d,%x]", search_index, data[search_index]);
        if (data[search_index] == SYNCWORD2) {
            puts("3");
        } else {
            puts("M");
            spdif_slave_hdl.find_sync_word = 0;
            return -1;
        }
    } else {
        while (search_index < pkt_len) {
            if (data[search_index] == SYNCWORD1) {
                spdif_slave_hdl.find_sync_word = 1;
            }
            if (++search_index >= pkt_len) {
                return -1;
            }
            if (spdif_slave_hdl.find_sync_word) {
                if ((data[search_index] == SYNCWORD2) && (data[search_index - 1] == SYNCWORD1)) {
                    break;
                } else {
                    spdif_slave_hdl.find_sync_word = 0;
                }
            }
        }
    }
    spdif_slave_hdl.find_sync_word = 0;

    data_type = data[++search_index];
    spdif_slave_hdl.decoder_type = data_type & 0xff;
    pkt_size_bits = data[++search_index];
    spdif_slave_hdl.length_code = (pkt_size_bits >> 3) >> 1; //16bit
    if (pkt_size_bits % 16) {
        printf("packet not endinf at a 16-bit boundary %d-%d \n", pkt_size_bits, search_index);
        for (int i = 0; i < pkt_len; i++) {
            printf("%x ", data[i]);
        }
    }
    ++search_index;
    data_cp_len    = pkt_len - search_index;
    skip_len = 2048 - (spdif_slave_hdl.length_code << 1) - BURST_HEADER_SZIE;
    if (data_cp_len) {
        spdif_dec_write_data(&data[search_index], data_cp_len * 2);
        data_cp_len = 0;
    }
    return 0;
}
void reset_data_buf(void)
{
    if (!spdif_dec_hdl) {
        return;
    }
    cbuf_clear(&spdif_dec_hdl->dec_cbuf);
}

#define SAMPLE_RATE_TABLE_SIZE 14
static u8 old_samplerate_index = 6;   /*位置记得要跟默认采样率对应*/
u16 spdif_sample_rate_table[SAMPLE_RATE_TABLE_SIZE] = {
    80,
    110,
    160,
    221,
    240,
    320,
    441,
    480,
    504,
    640,
    882,
    960,
    1764,
    1920,
};
#define SPDIF_DEFAULT_OUT_SAMPLE_RATE 44100
static u32 cal_samplerate(void)
{
    static u32 pre_sr_cnt = 0;
    static u32 sr_cnt = 0;
    sr_cnt = JL_SS->SR_CNT;
    u8  search_index = 0, i = 0;
    u32 tick_result = 0 ;
    tick_result = (((clk_get("lsb") / 100 * SPDIF_DATA_DAM_LEN)) / sr_cnt);
    for (i = 0; i < SAMPLE_RATE_TABLE_SIZE; i++) {
        if ((tick_result > (spdif_sample_rate_table[i] - 20)) && (tick_result < (spdif_sample_rate_table[i] + 20))) {
            search_index = i;
        }
    }
    if ((search_index != 0) && (old_samplerate_index != search_index)) {
        printf("\n\n spdif SET src param %d %d--- \n\n", spdif_sample_rate_table[search_index], tick_result);
        reset_data_buf();
        reset_pcm_sample_rate(spdif_sample_rate_table[search_index] * 100);
        old_samplerate_index = search_index;
    }
    return 0;
}

___interrupt
static void spdif_isr_handler()
{
    int index = 0;
    s32 *ptr;
    u8 *info_ptr = NULL;
    s16 data_ptr[SPDIF_DATA_DAM_LEN * SPDIF_CHANNEL_NUMBER];
    u16 *u_data_ptr = (u16 *)data_ptr;
    /* putchar('A'); */
    if (I_PND) { //information pending
        I_PND_CLR;
        index = !(USING_INF_BUF_INDEX >> 13);
        info_ptr = (u8 *) &spdif_slave_hdl.p_spdif_info_buf[index];
        for (int i = 0; i < SPDIF_INFO_LEN; i++) {
            if (!(info_ptr[0]&INFO_VALIDITY_BIT)) {
                break;
            }
            spdif_slave_hdl.validity_bit_flag = 1;
        }
    }

    if (ERROR_VALUE) { // some error flag
        if (ERROR_VALUE & (BIT(8) | BIT(9))) {//块/帧错误
            memset(&spdif_slave_hdl.p_spdif_data_buf[0], 0x0, SPDIF_DATA_DAM_LEN * SPDIF_CHANNEL_NUMBER);
            memset(&spdif_slave_hdl.p_spdif_data_buf[1], 0x0, SPDIF_DATA_DAM_LEN * SPDIF_CHANNEL_NUMBER);
        }
        if (JL_SS->CON & BIT(11)) {//电平长度接收错误
            //模块复位
            putchar('E');
            audio_spdif_slave_close(&spdif_slave_hdl);
            spdif_slave_hdl.error_flag = 1;
            ERR_CLR;
            return;
        }
        ERR_CLR;
    }
    if (spdif_slave_hdl.error_flag) {
        return;
    }
    if (D_PND) { //data pending
        D_PND_CLR;
        if (spdif_slave_hdl.validity_bit_flag) { //DST
            spdif_slave_hdl.validity_bit_flag = 0;
            index = !(USING_BUF_INDEX >> 12);
            ptr = (s32 *)spdif_slave_hdl.p_spdif_data_buf[index];
            for (int i = 0; i < SPDIF_DATA_DAM_LEN * 2; i += 2) {
                u_data_ptr[i]   = (u16)(ptr[i] >> 8);
                u_data_ptr[i + 1] = (u16)(ptr[i + 1] >> 8);
            }
            if (!spdif_dec_hdl) {
#if 0
                struct audio_fmt fmt;
                fmt.channel = 2;
                fmt.coding_type = 	AUDIO_CODING_DTS;
                fmt.sample_rate = SPDIF_DEFAULT_OUT_SAMPLE_RATE;
                spdif_dec_open(fmt);
#endif
                audio_spdif_slave_close(&spdif_slave_hdl);
                spdif_slave_hdl.error_flag = 2;

            } else if (spdif_dec_hdl->fmt.coding_type != AUDIO_CODING_DTS) {
                putchar('d');
                //模块复位
                audio_spdif_slave_close(&spdif_slave_hdl);
                spdif_slave_hdl.error_flag = 2;
                return;
            }
            if (spdif_dec_hdl && (spdif_slave_hdl.status == SPDIF_STATE_START)) {
                spdif_non_linear_packet(u_data_ptr, SPDIF_DATA_DAM_LEN * 2);
            }

            /* putchar('D'); */
            return;
        } else { //PCM
            if (!spdif_dec_hdl) {
#if 0
                putchar('p');
                struct audio_fmt fmt;
                fmt.channel = 2;
                fmt.coding_type = 	AUDIO_CODING_PCM;
                fmt.sample_rate = SPDIF_DEFAULT_OUT_SAMPLE_RATE;
                spdif_dec_open(fmt);
#endif
                audio_spdif_slave_close(&spdif_slave_hdl);
                spdif_slave_hdl.error_flag = 1;

                return;
            } else if (spdif_dec_hdl->fmt.coding_type != AUDIO_CODING_PCM) {
                putchar('p');
                audio_spdif_slave_close(&spdif_slave_hdl);
                spdif_slave_hdl.error_flag = 1;
                return;
            }

            /* putchar('D'); */
            cal_samplerate();
            index = !(USING_BUF_INDEX >> 12);
            ptr = (s32 *) spdif_slave_hdl.p_spdif_data_buf[index];
            for (int i = 0; i < SPDIF_DATA_DAM_LEN * 2; i += 2) {
                data_ptr[i]   = (s16)(ptr[i] >> 8);
                data_ptr[i + 1] = (s16)(ptr[i + 1] >> 8);
            }
            if ((spdif_dec_hdl) && (spdif_slave_hdl.status == SPDIF_STATE_START)) {
                int wlen = spdif_dec_write_data(data_ptr, SPDIF_DATA_DAM_LEN * SPDIF_CHANNEL_NUMBER * 2);
                if (wlen == 0) {
                    /* printf(" wlen = 0"); */
                }
            }
            memset(spdif_slave_hdl.p_spdif_data_buf[index], 0, SPDIF_DATA_DAM_LEN * SPDIF_CHANNEL_NUMBER * 2);
        }
    }
    if (CSB_PND) {
        CSB_PND_CLR;
    }

}

// 写入spdif数据
/* int spdif_dec_write_data(s16 *data, int len) */
int spdif_dec_write_data(void *data, int len)
{
    if (!spdif_dec_hdl) {
        return 0;
    }
    u16 wlen = cbuf_write(&spdif_dec_hdl->dec_cbuf, data, len);
    if (wlen) {
        audio_decoder_resume(&spdif_dec_hdl->pcm_dec.decoder);
    }
    /* printf("wl:%d ", wlen); */
    return wlen;
}

static void spdif_dec_release(void)
{
    audio_decoder_task_del_wait(&decode_task, &spdif_dec_hdl->wait);
    if (spdif_dec_hdl) {
        clock_remove(SPDIF_CLK);
        free(spdif_dec_hdl);
        spdif_dec_hdl = NULL;
    }
}
static void file_dec_event_handler(struct audio_decoder *decoder, int argc, int *argv)
{
    switch (argv[0]) {
    case AUDIO_DEC_EVENT_END:
        printf("\n--func=%s\n", __FUNCTION__);
        spdif_dec_close();
        break;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief    文件指针定位
   @param    *decoder: 解码器句柄
   @param    offset: 定位偏移
   @param    seek_mode: 定位类型
   @return   0：成功
   @return   非0：错误
   @note
*/
/*----------------------------------------------------------------------------*/
static int spdif_file_seek(struct audio_decoder *decoder, u32 offset, int seek_mode)
{
    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief    读取文件数据
   @param    *decoder: 解码器句柄
   @param    *buf: 数据
   @param    len: 数据长度
   @return   >=0：读到的数据长度
   @return   <0：错误
   @note
*/
/*----------------------------------------------------------------------------*/
static int spdif_file_read(struct audio_decoder *decoder, void *buf, u32 len)
{
    int rlen;
    rlen = cbuf_read(&spdif_dec_hdl->dec_cbuf, buf, len);
    return rlen;
}

static const struct audio_dec_input file_input = {
    .coding_type = 	AUDIO_CODING_DTS,
    .data_type   = AUDIO_INPUT_FILE,
    .ops = {
        .file = {
            .fread = spdif_file_read,
            .fseek = spdif_file_seek,
        }
    }
};


static int spdif_dec_read(void *hdl, void *buf, int len)
{
    int rlen = 0;
    rlen = cbuf_read(&spdif_dec_hdl->dec_cbuf, buf, len);
    /* printf("rlen:%d\n",rlen); */
    return rlen;
}

int spdif_data_size(void *hdl)
{
    struct s_spdif_decode *spdif_hdl = (struct s_spdif_decode *)hdl;
    return cbuf_get_data_size(&spdif_hdl->dec_cbuf);
}

int spdif_data_total(void *hdl)
{
    struct s_spdif_decode *spdif_hdl = (struct s_spdif_decode *)hdl;
    return spdif_hdl->dec_cbuf.total_len;
}
static void reset_pcm_sample_rate(u32 sr)
{
    spdif_dec_hdl->pcm_sr = sr;
}
static u32 get_pcm_sample_rate(void)
{
    /* printf("[%d]",spdif_dec_hdl->pcm_sr); */
    return spdif_dec_hdl->pcm_sr;
}
/*----------------------------------------------------------------------------*/
/**@brief    pcm解码数据输出
   @param    *entry: 音频流句柄
   @param    *in: 输入信息
   @param    *out: 输出信息
   @return   输出长度
   @note     *out未使用
*/
/*----------------------------------------------------------------------------*/
static int pcm_dec_data_handler(struct audio_stream_entry *entry,
                                struct audio_data_frame *in,
                                struct audio_data_frame *out)
{
    struct audio_decoder *decoder = container_of(entry, struct audio_decoder, entry);
    struct pcm_decoder *pcm_dec = container_of(decoder, struct pcm_decoder, decoder);
    if (in->offset == 0) {
        in->sample_rate = get_pcm_sample_rate();
    }
    audio_stream_run(&decoder->entry, in);
    return decoder->process_len;
}
static void pcm_dec_event_handler(struct audio_decoder *decoder, int argc, int *argv)
{
}
/*----------------------------------------------------------------------------*/
/**@brief    pcm解码数据流激活
   @param    *p: 私有句柄
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
static void dec_out_stream_resume(void *p)
{
    struct s_spdif_decode *dec = (struct s_spdif_decode *)p;

    audio_decoder_resume(&dec->pcm_dec.decoder);
}


int spdif_dec_start(void)
{
    int err = 0;
    u8 ch_num = 0;
    struct audio_fmt f;
    if (!spdif_dec_hdl) {
        return -EINVAL;
    }
    struct s_spdif_decode *dec = spdif_dec_hdl;
    if (dec->fmt.coding_type == AUDIO_CODING_DTS) {
        puts("dts \n");//DTS未测试
        return -EINVAL;
        err = file_decoder_open(&dec->file_dec, &file_input, &decode_task, NULL, 0);
        if (err) {
            return err;
        }
        file_decoder_set_event_handler(&dec->file_dec, file_dec_event_handler, 0);

    } else {
        puts("pcm \n");
        err = pcm_decoder_open(&dec->pcm_dec, &decode_task);
        if (err) {
            return err;
        }
        pcm_decoder_set_event_handler(&dec->pcm_dec, pcm_dec_event_handler, 0);
        pcm_decoder_set_read_data(&dec->pcm_dec, spdif_dec_read, dec);
        pcm_decoder_set_data_handler(&dec->pcm_dec, pcm_dec_data_handler);
    }
    printf("\n--func=%s\n", __FUNCTION__);

    audio_mode_main_dec_open(AUDIO_MODE_MAIN_STATE_DEC_SPDIF);

    audio_mixer_ch_open_head(&dec->mix_ch, &mixer);
    audio_mixer_ch_set_no_wait(&dec->mix_ch, 1, 10); // 超时自动丢数
    /* audio_mixer_ch_set_src(&dec->mix_ch, 1, 0); */
#if 1
    struct audio_mixer_ch_sync_info info = {0};
    info.priv = dec;
    info.get_total = spdif_data_total;
    info.get_size = spdif_data_size;
    audio_mixer_ch_set_sync(&dec->mix_ch, &info, 1, 1);
#endif

    ch_num = audio_output_channel_num();
    u16 sample_rate = dec->fmt.sample_rate;

#if AUDIO_SURROUND_CONFIG
    //环绕音效
    dec->surround = surround_open_demo(AEID_MUSIC_SURROUND, dec->pcm_dec.output_ch_type);
#endif


#if AUDIO_VBASS_CONFIG
    dec->vbass_prev_gain = audio_gain_open_demo(AEID_MUSIC_VBASS_PREV_GAIN, ch_num);
    dec->ns_gate = audio_noisegate_open_demo(AEID_MUSIC_NS_GATE, sample_rate, ch_num);
    //虚拟低音
    dec->vbass = audio_vbass_open_demo(AEID_MUSIC_VBASS, sample_rate, ch_num);
#endif

#if TCFG_EQ_ENABLE && TCFG_AUDIO_OUT_EQ_ENABLE
    dec->high_bass = high_bass_eq_open(sample_rate, ch_num);
    dec->hb_drc = high_bass_drc_open(sample_rate, ch_num);
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->hb_drc && dec->hb_drc->run32bit) {
        dec->hb_convert = convet_data_open(0, 512);
    }
#endif
#endif

#if TCFG_EQ_ENABLE && TCFG_SPDIF_MODE_EQ_ENABLE
    dec->eq = music_eq_open(sample_rate, ch_num);// eq
#if TCFG_DRC_ENABLE && TCFG_SPDIF_MODE_DRC_ENABLE
    dec->drc = music_drc_open(sample_rate, ch_num);//drc
#endif/*TCFG_SPDIF_MODE_DRC_ENABLE*/
#if !TCFG_DYNAMIC_EQ_ENABLE
#if defined(TCFG_DRC_ENABLE) && TCFG_DRC_ENABLE
    if (dec->eq && dec->eq->out_32bit) {
        dec->convert = convet_data_open(0, 512);
    }
#endif
#endif
#if defined(MUSIC_EXT_EQ_AFTER_DRC) && MUSIC_EXT_EQ_AFTER_DRC
    dec->ext_eq = music_ext_eq_open(sample_rate, ch_num);
#endif

#if defined(TCFG_DYNAMIC_EQ_ENABLE) && TCFG_DYNAMIC_EQ_ENABLE
    dec->eq2 = music_eq2_open(sample_rate, ch_num);// eq
    dec->dy_eq = audio_dynamic_eq_ctrl_open(AEID_MUSIC_DYNAMIC_EQ, sample_rate, ch_num);//动态eq
    dec->convert2 = convet_data_open(0, 512);
#endif/*TCFG_DYNAMIC_EQ_ENABLE*/
#if defined(TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE) && TCFG_PHASER_GAIN_AND_CH_SWAP_ENABLE
    dec->gain = audio_gain_open_demo(AEID_MUSIC_GAIN, ch_num);
#endif
#endif/*TCFG_SPDIF_MODE_EQ_ENABLE*/



#if TCFG_EQ_DIVIDE_ENABLE

#if TCFG_EQ_ENABLE && TCFG_SPDIF_MODE_EQ_ENABLE
    dec->eq_rl_rr = music_eq_rl_rr_open(sample_rate, ch_num);// eq
#if TCFG_DRC_ENABLE && TCFG_BT_MUSIC_DRC_ENABLE
    dec->drc_rl_rr = music_drc_rl_rr_open(sample_rate, ch_num);//drc
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
    dec->ext_eq2 = music_ext_eq2_open(sample_rate, ch_num);
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

    if (dec->fmt.coding_type == AUDIO_CODING_DTS) {
        entries[entry_cnt++] = &dec->file_dec.decoder.entry;
    } else {
        entries[entry_cnt++] = &dec->pcm_dec.decoder.entry;
    }




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
#if TCFG_EQ_ENABLE && TCFG_SPDIF_MODE_EQ_ENABLE
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

    dec->audio_stream = audio_stream_open(dec, dec_out_stream_resume);
    audio_stream_add_list(dec->audio_stream, entries, entry_cnt);
#if TCFG_EQ_DIVIDE_ENABLE
    struct audio_stream_entry *rl_rr_entries[8] = {NULL};
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

    audio_output_set_start_volume(APP_AUDIO_STATE_MUSIC);
    err = audio_decoder_start(&dec->pcm_dec.decoder);
    if (err) {

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

#if TCFG_EQ_ENABLE && TCFG_SPDIF_MODE_EQ_ENABLE
        music_eq_close(dec->eq);
#if TCFG_DRC_ENABLE && TCFG_SPDIF_MODE_DRC_ENABLE
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
        return err;
    }
    clock_set_cur();
    dec->status = SPDIF_STATE_START;
    return 0;
}

int spdif_dec_stop(void)
{
    if (spdif_dec_hdl) {
        spdif_dec_hdl->status = SPDIF_STATE_STOP;
        pcm_decoder_close(&spdif_dec_hdl->pcm_dec);
        struct s_spdif_decode *dec = spdif_dec_hdl;

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
#if TCFG_EQ_ENABLE && TCFG_SPDIF_MODE_EQ_ENABLE
        music_eq_close(dec->eq);
#if TCFG_DRC_ENABLE && TCFG_SPDIF_MODE_DRC_ENABLE
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
        music_eq_rl_rr_close(spdif_dec_hdl->eq_rl_rr);
        music_drc_rl_rr_close(spdif_dec_hdl->drc_rl_rr);
        convet_data_close(spdif_dec_hdl->convert_rl_rr);
        audio_gain_close_demo(spdif_dec_hdl->gain_rl_rr);

        audio_vocal_tract_synthesis_close(&spdif_dec_hdl->synthesis_ch_fl_fr);
        audio_vocal_tract_synthesis_close(&spdif_dec_hdl->synthesis_ch_rl_rr);
        audio_vocal_tract_close(&spdif_dec_hdl->vocal_tract);
        channel_switch_close(&spdif_dec_hdl->ch_switch);
#if defined(MUSIC_EXT_EQ2_AFTER_DRC) && MUSIC_EXT_EQ2_AFTER_DRC
        music_ext_eq2_close(spdif_dec_hdl->ext_eq2);
#endif
#endif

        audio_mixer_ch_close(&spdif_dec_hdl->mix_ch);
        if (spdif_dec_hdl->audio_stream) {
            audio_stream_close(spdif_dec_hdl->audio_stream);
            spdif_dec_hdl->audio_stream = 0;
        }
    }
    return 0;
}
static int spdif_wait_res_handler(struct audio_res_wait *wait, int event)
{
    int err = 0;
    if (event == AUDIO_RES_GET) {
        err = spdif_dec_start();
    } else if (event == AUDIO_RES_PUT) {
        err = spdif_dec_stop();
    }
    return err;
}


static void spdif_status_check(void *priv)
{
    static u8 cnt = 0;
    if (spdif_dec_hdl && (spdif_slave_hdl.status == SPDIF_STATE_START || spdif_slave_hdl.status == SPDIF_STATE_STOP)) {
        /* printf("spdif status \n"); */
        if (spdif_slave_hdl.error_flag == 1) {
            printf("spdif reset \n");
            /* audio_spdif_slave_close(&spdif_slave_hdl); */
            spdif_dec_close();
            struct audio_fmt fmt;
            fmt.channel = 2;
            fmt.coding_type = 	AUDIO_CODING_PCM;
            fmt.sample_rate = SPDIF_DEFAULT_OUT_SAMPLE_RATE;
            spdif_dec_open(fmt);
            audio_spdif_slave_open(&spdif_slave_hdl);
            audio_spdif_slave_start(&spdif_slave_hdl);
            spdif_slave_hdl.error_flag = 0;
        } else if (spdif_slave_hdl.error_flag == 2) {
            printf("spdif reset \n");
            /* audio_spdif_slave_close(&spdif_slave_hdl); */
            spdif_dec_close();
            struct audio_fmt fmt;
            fmt.channel = 2;
            fmt.coding_type = 	AUDIO_CODING_DTS;
            fmt.sample_rate = SPDIF_DEFAULT_OUT_SAMPLE_RATE;
            spdif_dec_open(fmt);
            audio_spdif_slave_open(&spdif_slave_hdl);
            audio_spdif_slave_start(&spdif_slave_hdl);
            spdif_slave_hdl.error_flag = 0;
        } else {
#if 10
            if (spdif_dec_hdl->status == SPDIF_STATE_START) {
                if (spdif_data_size(spdif_dec_hdl) == 0) {
                    if (cnt < 6) {
                        cnt++;
                    }
                    if (cnt == 5) {
                        audio_mixer_ch_pause(&spdif_dec_hdl->mix_ch, 1);
                        audio_decoder_resume_all(&decode_task);
                    }
                } else {
                    if (cnt > 4) {
                        audio_mixer_ch_pause(&spdif_dec_hdl->mix_ch, 0);
                        audio_decoder_resume_all(&decode_task);
                    }
                    cnt = 0;
                }
            }
#endif
        }
    }
}
///*----------------------------------------------------------------------------*/
/**@brief    spdif 硬件设置
   @param    无
   @return   无
   @note
*/
/*----------------------------------------------------------------------------*/
static void _spdif_open(void)
{
#if	TCFG_SPDIF_OUTPUT_ENABLE
    spdif_slave_hdl.output_port = SPDIF_OUT_PORT_A;//PB11
#endif
    spdif_slave_hdl.input_port  = SPDIF_IN_PORT_A;//PA9
    audio_spdif_slave_open(&spdif_slave_hdl);

    audio_spdif_slave_start(&spdif_slave_hdl);
#if TCFG_HDMI_ARC_ENABLE
    extern void hdmi_cec_init(void);
    hdmi_cec_init();
#endif
}

void spdif_init(void)
{
    audio_spdif_slave_init(&spdif_slave_hdl);
    request_irq(IRQ_SPDIF_IDX, 2, spdif_isr_handler, 0);
    sys_timer_add(NULL, spdif_status_check, 250);

#if (TCFG_APP_SPDIF_EN == 0)
    spdif_dec_init();
    _spdif_open();  //for test
#endif

}
void spdif_dec_init(void)
{
    struct audio_fmt fmt;
    fmt.channel = 2;
    fmt.coding_type = 	AUDIO_CODING_PCM;
    fmt.sample_rate = SPDIF_DEFAULT_OUT_SAMPLE_RATE;
    spdif_dec_open(fmt);
}

int spdif_dec_open(struct audio_fmt fmt)
{
    int err;
    if (spdif_dec_hdl) {
        return -1;
    }
    spdif_dec_hdl = zalloc(sizeof(struct s_spdif_decode));
    if (!spdif_dec_hdl) {
        return -ENOMEM;
    }

    clock_add(SPDIF_CLK);
    cbuf_init(&spdif_dec_hdl->dec_cbuf, spdif_dec_hdl->dec_data_buf, SPDIF_DEC_DATA_BUF_LEN);
    spdif_dec_hdl->fmt.channel = fmt.channel;
    spdif_dec_hdl->fmt.coding_type = fmt.coding_type;
    spdif_dec_hdl->fmt.sample_rate = fmt.sample_rate;
    spdif_dec_hdl->fmt.coding_type = fmt.coding_type;
    spdif_dec_hdl->coding_type = fmt.coding_type;
    if (fmt.coding_type == AUDIO_CODING_DTS) {
        spdif_dec_hdl->file_dec.ch_type = AUDIO_CH_MAX;
        spdif_dec_hdl->file_dec.output_ch_num = audio_output_channel_num();
        spdif_dec_hdl->file_dec.output_ch_type = audio_output_channel_type();
    } else {
        old_samplerate_index = 6;
        spdif_dec_hdl->pcm_dec.ch_num = fmt.channel;
        spdif_dec_hdl->pcm_dec.output_ch_num = audio_output_channel_num();
        spdif_dec_hdl->pcm_dec.output_ch_type = audio_output_channel_type();
        spdif_dec_hdl->pcm_dec.sample_rate = fmt.sample_rate;
        spdif_dec_hdl->pcm_sr = fmt.sample_rate;
    }
    spdif_dec_hdl->wait.priority = 4;
    spdif_dec_hdl->wait.preemption = 0;
    spdif_dec_hdl->wait.protect = 1;//1 叠加模式
    spdif_dec_hdl->wait.snatch_same_prio = 1;
    spdif_dec_hdl->wait.handler = spdif_wait_res_handler;
    spdif_dec_hdl->status = SPDIF_STATE_OPEN;
    err = audio_decoder_task_add_wait(&decode_task, &spdif_dec_hdl->wait);
    return err;
}

void spdif_dec_close(void)
{
    printf("\n--func1=%s\n", __FUNCTION__);
    if (!spdif_dec_hdl) {
        return;
    }
    spdif_dec_stop();
    spdif_dec_release();
    clock_set_cur();
}

bool spdif_dec_check(void)
{
    if (spdif_dec_hdl) {

        return true;
    }
    return false;
}
#endif      // TCFG_SPDIF_ENABLE
/***********************spdif dec end*****************************/
