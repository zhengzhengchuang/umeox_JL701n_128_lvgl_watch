#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "asm/audio_src.h"
#include "asm/audio_adc.h"
#include "audio_enc.h"
#include "app_main.h"
#include "clock_cfg.h"
#include "classic/hci_lmp.h"
#include "app_config.h"
#include "Resample_api.h"

#ifndef CONFIG_LITE_AUDIO
#include "app_task.h"
#include "aec_user.h"
#include "loud_speaker.h"
#include "mic_effect.h"
#else
__attribute__((weak))void audio_aec_inbuf(s16 *buf, u16 len)
{
}
#endif/*CONFIG_LITE_AUDIO*/

/* #include "encode/encode_write.h" */
extern struct adc_platform_data adc_data;

struct audio_adc_hdl adc_hdl;
struct esco_enc_hdl *esco_enc = NULL;
struct audio_encoder_task *encode_task = NULL;

static u16 ladc_irq_point_unit = 0;
/* static u16 linein_irq_point =  0; */

#define ESCO_ADC_BUF_NUM        3
#define ESCO_ADC_IRQ_POINTS     256
#define ESCO_ADC_BUFS_SIZE      (ESCO_ADC_BUF_NUM * ESCO_ADC_IRQ_POINTS)

struct esco_enc_hdl {
    struct audio_encoder encoder;
    struct audio_adc_output_hdl adc_output;
    struct adc_mic_ch mic_ch;
    //OS_SEM pcm_frame_sem;
    s16 output_frame[30];               //align 4Bytes
    int pcm_frame[60];                 //align 4Bytes
    /* s16 adc_buf[ESCO_ADC_BUFS_SIZE];    //align 4Bytes */
    u8 state;
    RS_STUCT_API *mic_sw_src_api ;
    u8 *mic_sw_src_buf;
};

static void adc_mic_output_handler(void *priv, s16 *data, int len)
{
    /* printf("buf:%x,data:%x,len:%d",esco_enc->adc_buf,data,len); */
    audio_aec_inbuf(data, len);
}


#if TCFG_IIS_INPUT_EN
#define IIS_MIC_SRC_DIFF_MAX        (50)
#define IIS_MIC_BUF_SIZE    (2*1024)
cbuffer_t *iis_mic_cbuf = NULL;
static RS_STUCT_API *iis_mic_sw_src_api = NULL;
static u8 *iis_mic_sw_src_buf = NULL;
static s16 iis_mic_sw_src_output[ALNK_BUF_POINTS_NUM];
s32 sw_src_in_sr = 0;
s32 sw_src_in_sr_top = 0;
s32 sw_src_in_sr_botton = 0;

int iis_mic_sw_src_init()
{
    printf("%s !!\n", __func__);
    if (iis_mic_sw_src_api) {
        printf("iis mic sw src is already open !\n");
        return -1;
    }
    iis_mic_sw_src_api = get_rs16_context();
    g_printf("iis_mic_sw_src_api:0x%x\n", iis_mic_sw_src_api);
    ASSERT(iis_mic_sw_src_api);
    u32 iis_mic_sw_src_need_buf = iis_mic_sw_src_api->need_buf();
    g_printf("iis_mic_sw_src_buf:%d\n", iis_mic_sw_src_need_buf);
    iis_mic_sw_src_buf = malloc(iis_mic_sw_src_need_buf);
    ASSERT(iis_mic_sw_src_buf);
    RS_PARA_STRUCT rs_para_obj;
    rs_para_obj.nch = 1;
    rs_para_obj.new_insample = TCFG_IIS_INPUT_SR;
    rs_para_obj.new_outsample = 16000;

    sw_src_in_sr = rs_para_obj.new_insample;
    sw_src_in_sr_top = rs_para_obj.new_insample + IIS_MIC_SRC_DIFF_MAX;
    sw_src_in_sr_botton = rs_para_obj.new_insample - IIS_MIC_SRC_DIFF_MAX;
    printf("sw src,in = %d,out = %d\n", rs_para_obj.new_insample, rs_para_obj.new_outsample);
    iis_mic_sw_src_api->open(iis_mic_sw_src_buf, &rs_para_obj);
    return 0;
}

int iis_mic_sw_src_uninit()
{
    printf("%s !!\n", __func__);
    if (iis_mic_sw_src_api) {
        iis_mic_sw_src_api = NULL;
    }
    if (iis_mic_sw_src_buf) {
        free(iis_mic_sw_src_buf);
        iis_mic_sw_src_buf = NULL;
    }
    return 0;
}

static void iis_mic_output_handler(void *priv, s16 *data, int len)
{
    s16 *outdat = NULL;
    int outlen = 0;
    int wlen = 0;
    int i = 0;

    // dual to mono
    for (i = 0; i < len / 2 / 2; i++) {
        /* data[i] = ((s32)(data[i*2]) + (s32)(data[i*2-1])) / 2; */
        data[i] = data[i * 2];
    }
    len >>= 1;





    if (iis_mic_cbuf) {
        if (iis_mic_sw_src_api && iis_mic_sw_src_buf) {
            if (iis_mic_cbuf->data_len > IIS_MIC_BUF_SIZE * 3 / 4) {
                sw_src_in_sr--;
                if (sw_src_in_sr < sw_src_in_sr_botton) {
                    sw_src_in_sr = sw_src_in_sr_botton;
                }
            } else if (iis_mic_cbuf->data_len < IIS_MIC_BUF_SIZE / 4) {
                sw_src_in_sr++;
                if (sw_src_in_sr > sw_src_in_sr_top) {
                    sw_src_in_sr = sw_src_in_sr_top;
                }
            }

            outlen = iis_mic_sw_src_api->run(iis_mic_sw_src_buf,    \
                                             data,                  \
                                             len >> 1,              \
                                             iis_mic_sw_src_output);
            ASSERT(outlen <= (sizeof(iis_mic_sw_src_output) >> 1));
            outlen = outlen << 1;
            outdat = iis_mic_sw_src_output;
        }
        wlen = cbuf_write(iis_mic_cbuf, outdat, outlen);
        if (wlen != outlen) {
            putchar('w');
        }
        esco_enc_resume();
    }
}

static int iis_mic_output_read(s16 *buf, u16 len)
{
    int rlen = 0;
    if (iis_mic_cbuf) {
        rlen = cbuf_read(iis_mic_cbuf, buf, len);
    }
    return rlen;
}

#endif // TCFG_IIS_INPUT_EN

#if	TCFG_MIC_EFFECT_ENABLE
unsigned int jl_sr_table[] = {
    7616,
    10500,
    11424,
    15232,
    21000,
    22848,
    30464,
    42000,
    45696,
};

unsigned int normal_sr_table[] = {
    8000,
    11025,
    12000,
    16000,
    22050,
    24000,
    32000,
    44100,
    48000,
};
static u8 get_sample_rate_index(u32 sr)
{
    u8 i;
    for (i = 0; i < ARRAY_SIZE(normal_sr_table); i++) {
        if (normal_sr_table[i] == sr) {
            return i;
        }
    }
    return i - 1;
}
int mic_sw_src_init(u16 out_sr)
{
    if (!esco_enc) {
        printf(" mic  is not open !\n");
        return -1;
    }
    esco_enc->mic_sw_src_api = get_rsfast_context();
    esco_enc->mic_sw_src_buf = malloc(esco_enc->mic_sw_src_api->need_buf());

    ASSERT(esco_enc->mic_sw_src_buf);
    RS_PARA_STRUCT rs_para_obj;
    rs_para_obj.nch = 1;
    /* rs_para_obj.new_insample = MIC_EFFECT_SAMPLERATE; */
    /* rs_para_obj.new_outsample = out_sr; */
    rs_para_obj.new_insample = jl_sr_table[get_sample_rate_index(MIC_EFFECT_SAMPLERATE)];
    rs_para_obj.new_outsample = jl_sr_table[get_sample_rate_index(out_sr)];
    esco_enc->mic_sw_src_api->open(esco_enc->mic_sw_src_buf, &rs_para_obj);
    return 0;
}

int mic_sw_src_uninit(void)
{
    if (!esco_enc) {
        return 0;
    }
    if (esco_enc->mic_sw_src_buf) {
        free(esco_enc->mic_sw_src_buf);
        esco_enc->mic_sw_src_buf = NULL;
    }
    return 0;
}

#endif //TCFG_MIC_EFFECT_ENABLE

static void adc_mic_output_handler_downsr(void *priv, s16 *data, int len)
{
    //printf("buf:%x,data:%x,len:%d",esco_enc->adc_buf,data,len);
    u16 i;
    s16 temp_buf[160];
    if (esco_enc && esco_enc->mic_sw_src_buf) {
        int wlen = esco_enc->mic_sw_src_api->run(esco_enc->mic_sw_src_buf, data, len / 2, temp_buf);
        audio_aec_inbuf(temp_buf, wlen << 1);
    }
    /* audio_aec_inbuf(temp_buf, len / 2); */
}

__attribute__((weak)) int audio_aec_output_read(s16 *buf, u16 len)
{
    return 0;
}

void esco_enc_resume(void)
{
    if (esco_enc) {
        //os_sem_post(&esco_enc->pcm_frame_sem);
        audio_encoder_resume(&esco_enc->encoder);
    }
}

static int esco_enc_pcm_get(struct audio_encoder *encoder, s16 **frame, u16 frame_len)
{
    int rlen = 0;
    if (encoder == NULL) {
        r_printf("encoder NULL");
    }
    struct esco_enc_hdl *enc = container_of(encoder, struct esco_enc_hdl, encoder);

    if (enc == NULL) {
        r_printf("enc NULL");
    }

    while (1) {

#if TCFG_IIS_INPUT_EN
        rlen = iis_mic_output_read(enc->pcm_frame, frame_len);
#else // TCFG_IIS_INPUT_EN

        rlen = audio_aec_output_read(enc->pcm_frame, frame_len);
#endif // TCFG_IIS_INPUT_EN

        if (rlen == frame_len) {
            /*esco编码读取数据正常*/
#if (RECORDER_MIX_EN)
            recorder_mix_sco_data_write(enc->pcm_frame, frame_len);
#endif/*RECORDER_MIX_EN*/
            break;
        } else if (rlen == 0) {
            /*esco编码读不到数,返回0*/
            return 0;
            /*esco编码读不到数，pend住*/
            /* int ret = os_sem_pend(&enc->pcm_frame_sem, 100);
            if (ret == OS_TIMEOUT) {
                r_printf("esco_enc pend timeout\n");
                break;
            } */
        } else {
            /*通话结束，aec已经释放*/
            printf("audio_enc end:%d\n", rlen);
            rlen = 0;
            break;
        }
    }

    *frame = enc->pcm_frame;
    return rlen;
}
static void esco_enc_pcm_put(struct audio_encoder *encoder, s16 *frame)
{
}

static const struct audio_enc_input esco_enc_input = {
    .fget = esco_enc_pcm_get,
    .fput = esco_enc_pcm_put,
};

static int esco_enc_probe_handler(struct audio_encoder *encoder)
{
    return 0;
}

static int esco_enc_output_handler(struct audio_encoder *encoder, u8 *frame, int len)
{
    lmp_private_send_esco_packet(NULL, frame, len);
    //printf("frame:%x,out:%d\n",frame, len);

    return len;
}

const static struct audio_enc_handler esco_enc_handler = {
    .enc_probe = esco_enc_probe_handler,
    .enc_output = esco_enc_output_handler,
};

static void esco_enc_event_handler(struct audio_encoder *encoder, int argc, int *argv)
{
    printf("esco_enc_event_handler:0x%x,%d\n", argv[0], argv[0]);
    switch (argv[0]) {
    case AUDIO_ENC_EVENT_END:
        puts("AUDIO_ENC_EVENT_END\n");
        break;
    }
}

int esco_enc_open(u32 coding_type, u8 frame_len)
{
    int err;
    struct audio_fmt fmt;

    printf("esco_enc_open: %d,frame_len:%d\n", coding_type, frame_len);

    fmt.channel = 1;
    fmt.frame_len = frame_len;
    if (coding_type == AUDIO_CODING_MSBC) {
        fmt.sample_rate = 16000;
        fmt.coding_type = AUDIO_CODING_MSBC;
        clock_add(ENC_MSBC_CLK);
    } else if (coding_type == AUDIO_CODING_CVSD) {
        fmt.sample_rate = 8000;
        fmt.coding_type = AUDIO_CODING_CVSD;
        clock_add(ENC_CVSD_CLK);
    } else {
        /*Unsupoport eSCO Air Mode*/
    }

#if (RECORDER_MIX_EN)
    recorder_mix_pcm_set_info(fmt.sample_rate, fmt.channel);
#endif/*RECORDER_MIX_EN*/

    audio_encoder_task_open();

    if (!esco_enc) {
        esco_enc = zalloc(sizeof(*esco_enc));
    }
    //os_sem_create(&esco_enc->pcm_frame_sem, 0);

    audio_encoder_open(&esco_enc->encoder, &esco_enc_input, encode_task);
    audio_encoder_set_handler(&esco_enc->encoder, &esco_enc_handler);
    audio_encoder_set_fmt(&esco_enc->encoder, &fmt);
    audio_encoder_set_event_handler(&esco_enc->encoder, esco_enc_event_handler, 0);
    audio_encoder_set_output_buffs(&esco_enc->encoder, esco_enc->output_frame,
                                   sizeof(esco_enc->output_frame), 1);

    if (!esco_enc->encoder.enc_priv) {
        log_e("encoder err, maybe coding(0x%x) disable \n", fmt.coding_type);
        err = -EINVAL;
        goto __err;
    }

    audio_encoder_start(&esco_enc->encoder);

    printf("esco sample_rate: %d,mic_gain:%d\n", fmt.sample_rate, app_var.aec_mic_gain);

#if TCFG_IIS_INPUT_EN
    if (iis_mic_cbuf == NULL) {
        iis_mic_cbuf = zalloc(sizeof(cbuffer_t) + IIS_MIC_BUF_SIZE);
        if (iis_mic_cbuf) {
            cbuf_init(iis_mic_cbuf, iis_mic_cbuf + 1, IIS_MIC_BUF_SIZE);
        } else {
            printf("iis_mic_cbuf zalloc err !!!!!!!!!!!!!!\n");
        }
    }
    iis_mic_sw_src_init();
    audio_iis_input_start(TCFG_IIS_INPUT_PORT, TCFG_IIS_INPUT_DATAPORT_SEL, iis_mic_output_handler);
#else // TCFG_IIS_INPUT_EN

#if	TCFG_MIC_EFFECT_ENABLE
    if (fmt.sample_rate != MIC_EFFECT_SAMPLERATE) {//8K时需把mic数据采样率降低
        mic_sw_src_init(fmt.sample_rate);
        esco_enc->adc_output.handler = adc_mic_output_handler_downsr;
    } else {
        esco_enc->adc_output.handler = adc_mic_output_handler;
    }
    audio_mic_open(&esco_enc->mic_ch, MIC_EFFECT_SAMPLERATE, app_var.aec_mic_gain);
#else
    esco_enc->adc_output.handler = adc_mic_output_handler;
    audio_mic_open(&esco_enc->mic_ch, fmt.sample_rate, app_var.aec_mic_gain);
#endif
    audio_mic_add_output(&esco_enc->adc_output);
#if (TCFG_AUDIO_OUTPUT_IIS == 0)
    audio_mic_start(&esco_enc->mic_ch);
#endif /*TCFG_AUDIO_OUTPUT_IIS*/

#endif // TCFG_IIS_INPUT_EN

    clock_set_cur();
    esco_enc->state = 1;

    /* #if TCFG_AUDIO_OUTPUT_IIS */
    /*     extern void audio_aec_ref_start(u8 en); */
    /*     audio_aec_ref_start(1); */
    /* #endif #<{(|TCFG_AUDIO_OUTPUT_IIS|)}># */

    return 0;
__err:
    audio_encoder_close(&esco_enc->encoder);

    local_irq_disable();
    free(esco_enc);
    esco_enc = NULL;
    local_irq_enable();

    audio_encoder_task_close();

    return err;
}

int esco_adc_mic_en()
{
    if (esco_enc && esco_enc->state) {
        /* audio_adc_mic_start(&esco_enc->mic_ch); */
        audio_mic_start(&esco_enc->mic_ch);
        return 0;
    }
    return -1;
}

void esco_enc_close()
{
    printf("esco_enc_close\n");
    if (!esco_enc) {
        printf("esco_enc NULL\n");
        return;
    }

    if (esco_enc->encoder.fmt.coding_type == AUDIO_CODING_MSBC) {
        clock_remove(ENC_MSBC_CLK);
    } else if (esco_enc->encoder.fmt.coding_type == AUDIO_CODING_CVSD) {
        clock_remove(ENC_CVSD_CLK);
    }
#if TCFG_IIS_INPUT_EN
    audio_iis_input_stop(TCFG_IIS_INPUT_PORT, TCFG_IIS_INPUT_DATAPORT_SEL);
    if (iis_mic_cbuf) {
        free(iis_mic_cbuf);
        iis_mic_cbuf = NULL;
    }
    iis_mic_sw_src_uninit();
    audio_encoder_close(&esco_enc->encoder);
#else // TCFG_IIS_INPUT_EN
    audio_mic_close(&esco_enc->mic_ch, &esco_enc->adc_output);
#if	TCFG_MIC_EFFECT_ENABLE
    mic_sw_src_uninit();
#endif //TCFG_MIC_EFFECT_ENABLE

    audio_encoder_close(&esco_enc->encoder);
#endif // TCFG_IIS_INPUT_EN

    local_irq_disable();
    free(esco_enc);
    esco_enc = NULL;
    local_irq_enable();

    audio_encoder_task_close();
    clock_set_cur();
}

int esco_enc_mic_gain_set(u8 gain)
{
    app_var.aec_mic_gain = gain;
    if (esco_enc) {
        printf("esco mic 0 set gain %d\n", app_var.aec_mic_gain);
        audio_adc_mic_set_gain(&esco_enc->mic_ch, app_var.aec_mic_gain);
    }
    return 0;
}
int esco_enc_mic1_gain_set(u8 gain)
{
    app_var.aec_mic_gain = gain;
    if (esco_enc) {
        printf("esco mic 1 set gain %d\n", app_var.aec_mic_gain);
        audio_adc_mic1_set_gain(&esco_enc->mic_ch, app_var.aec_mic_gain);
    }
    return 0;
}
int esco_enc_mic2_gain_set(u8 gain)
{
    app_var.aec_mic_gain = gain;
    if (esco_enc) {
        printf("esco mic 2 set gain %d\n", app_var.aec_mic_gain);
        audio_adc_mic2_set_gain(&esco_enc->mic_ch, app_var.aec_mic_gain);
    }
    return 0;
}
int esco_enc_mic3_gain_set(u8 gain)
{
    app_var.aec_mic_gain = gain;
    if (esco_enc) {
        printf("esco mic 3 set gain %d\n", app_var.aec_mic_gain);
        audio_adc_mic3_set_gain(&esco_enc->mic_ch, app_var.aec_mic_gain);
    }
    return 0;
}

struct __encoder_task {
    u8 init_ok;
    atomic_t used;
    OS_MUTEX mutex;
};

static struct __encoder_task enc_task = {0};

int audio_encoder_task_open(void)
{
    local_irq_disable();
    if (enc_task.init_ok == 0) {
        atomic_set(&enc_task.used, 0);
        os_mutex_create(&enc_task.mutex);
        enc_task.init_ok = 1;
    }
    local_irq_enable();

    os_mutex_pend(&enc_task.mutex, 0);
    if (!encode_task) {
        encode_task = zalloc(sizeof(*encode_task));
        audio_encoder_task_create(encode_task, "audio_enc");
    }
    atomic_inc_return(&enc_task.used);
    os_mutex_post(&enc_task.mutex);
    return 0;
}

void audio_encoder_task_close(void)
{
    os_mutex_pend(&enc_task.mutex, 0);
    if (encode_task) {
        if (atomic_dec_and_test(&enc_task.used)) {
            audio_encoder_task_del(encode_task);
            //local_irq_disable();
            free(encode_task);
            encode_task = NULL;
            //local_irq_enable();
        }
    }
    os_mutex_post(&enc_task.mutex);
}



//////////////////////////////////////////////////////////////////////////////
int audio_enc_init()
{
    printf("audio_enc_init\n");

    audio_adc_init(&adc_hdl, &adc_data);

    init_audio_adc();
    return 0;
}


/**************************mic linein 接口***************************************************/
#define LADC_BUF_NUM        2
#define LADC_CH_NUM         3
#if TCFG_MIC_EFFECT_ENABLE
#if (RECORDER_MIX_EN)
#define LADC_IRQ_POINTS     160
#else
#if (TCFG_MIC_EFFECT_SEL == MIC_EFFECT_REVERB)
#define LADC_IRQ_POINTS    REVERB_LADC_IRQ_POINTS
#else
#define LADC_IRQ_POINTS     ((MIC_EFFECT_SAMPLERATE/1000)*4)
#endif
#endif/*RECORDER_MIX_EN*/
#else
#define LADC_IRQ_POINTS     256
#endif
/* #define LADC_BUFS_SIZE      (LADC_CH_NUM * LADC_BUF_NUM * LADC_IRQ_POINTS) */

/*******************************
 *28有4个LADC通道,都可用于mic输入(低压)和linenin输入(高压),
 *封装的mic接口通过宏TCFG_AUDIO_ADC_MIC_CHA来选择ladc通道，linein接口通过宏TCFG_LINEIN_LR_CH来选择ladc通道
 *要保证mic和linein选用的CH不冲突
 * PA1    PA3    PG6    PG8
 * BIT(0) BIT(1) BIT(2) BIT(3)
 * mic0   mic1   mic2   mic3
 * L0     L1     L2     L3
 ***********************/
//默认开启混响宏TCFG_MIC_EFFECT_ENABLE即使能多路LADC同开，


/* #define ENABLE_MIC_LINEIN */
#define LADC_LINE_L_MASK			(BIT(0))
#define LADC_LINE_R_MASK			(BIT(1))
#define LADC_MIC_CH_MASK    	    (BIT(2))
#define LADC_LINE1_L_MASK			(BIT(3))
#define LADC_LINE1_R_MASK			(BIT(4))

typedef struct  {
    struct audio_adc_hdl *p_adc_hdl;
    struct adc_mic_ch mic_ch;
    struct adc_linein_ch linein_ch;
    struct adc_linein_ch linein1_ch;
    atomic_t used;
    /* s16 adc_buf[MIC_ADC_BUFS_SIZE];    //align 4Bytes */
    s16 *adc_buf;
    OS_MUTEX mutex;
    u8 init_flag: 2;
    u8 states: 4;
    u16 sample_rate;
    struct list_head mic_head;
    struct list_head mic1_head;
    struct list_head linein_head;
    struct list_head linein1_head;
    int mic_gain;
    int linein_gain;
    int linein1_gain;
    u8 mic_en: 2;
    u8 ladc_en: 2;
    u8 ladc_ch_mark; //
    u8 ladc_ch_num;// 1 2 3 4

    u8 mic_ch_mark;
    u8 mic_ch_num;
    u8 mic_data_index;
    u8 linein_Lch_mark;
    u8 linein_Ldata_index;
    u8 linein_Rch_mark;
    u8 linein_Rdata_index;
    u8 linein_ch_num;

    u8 linein1_Lch_mark;
    u8 linein1_Ldata_index;
    u8 linein1_Rch_mark;
    u8 linein1_Rdata_index;
    u8 linein1_ch_num;
    //s16 temp_buf[LADC_IRQ_POINTS];
    s16 *temp_buf;
    struct audio_adc_output_hdl output;


} audio_adc_t;
static audio_adc_t ladc_var = {.init_flag = 1};

static void audio_adc_output_demo(void *priv, s16 *data, int len)
{
    u16 i;
    struct audio_adc_output_hdl *p;
    u32 samples = len / 2;
    /* struct audio_adc_hdl *hdl = priv; */
    if (ladc_var.ladc_ch_num == 1) {
        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) { //单mic
            if (!list_empty(&ladc_var.mic_head)) {
                list_for_each_entry(p, &ladc_var.mic_head, entry) {
                    p->handler(p->priv, data, len);
                }
            }
        } else { //单linein
            if (!list_empty(&ladc_var.linein_head)) {
                list_for_each_entry(p, &ladc_var.linein_head, entry) {
                    p->handler(p->priv, data, len);
                }
            } else if (!list_empty(&ladc_var.linein1_head)) {
                list_for_each_entry(p, &ladc_var.linein1_head, entry) {
                    p->handler(p->priv, data, len);
                }
            }
        }
    } else if (ladc_var.ladc_ch_num == 2) {
        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) { //数据结构：
            if (!list_empty(&ladc_var.mic_head)) {
                for (i = 0; i < samples; i++) {
                    ladc_var.temp_buf[i] = data[i * 2 + ladc_var.mic_data_index];
                }
                list_for_each_entry(p, &ladc_var.mic_head, entry) {
                    p->handler(p->priv, ladc_var.temp_buf, len);
                }
            }
            if (!list_empty(&ladc_var.linein_head)) {

                for (i = 0; i < samples; i++) {
                    data[i] = data[i * 2 + ladc_var.linein_Ldata_index];
                }
                list_for_each_entry(p, &ladc_var.linein_head, entry) {
                    p->handler(p->priv, data, len);
                }
            } else if (!list_empty(&ladc_var.linein1_head)) {
                for (i = 0; i < samples; i++) {
                    data[i] = data[i * 2 + ladc_var.linein1_Ldata_index];
                }
                list_for_each_entry(p, &ladc_var.linein1_head, entry) {
                    p->handler(p->priv, data, len);
                }
            }
        } else { //两路linein  LINL0 LINR0 LINL1 LINR1
            if (ladc_var.linein_ch_num == 1) { //接口不支持两路linein多开
            } else {
                if (!list_empty(&ladc_var.linein_head)) {
                    list_for_each_entry(p, &ladc_var.linein_head, entry) {
                        p->handler(p->priv, data, len);
                    }
                } else if (!list_empty(&ladc_var.linein1_head)) {
                    list_for_each_entry(p, &ladc_var.linein1_head, entry) {
                        p->handler(p->priv, data, len);
                    }
                }
            }
        }

    } else if (ladc_var.ladc_ch_num == 3) {//数据结构：

        if (!list_empty(&ladc_var.linein_head)) {
            if (ladc_var.linein_ch_num == 1) {
                for (i = 0; i < samples; i++) {
                    ladc_var.temp_buf[i] = data[i * 3 + ladc_var.linein_Ldata_index];
                }
            } else {
                for (i = 0; i < samples; i++) {
                    ladc_var.temp_buf[i * 2] = data[i * 3 + ladc_var.linein_Ldata_index];
                    ladc_var.temp_buf[i * 2 + 1] = data[i * 3 + ladc_var.linein_Rdata_index];
                }
            }
            list_for_each_entry(p, &ladc_var.linein_head, entry) {
                p->handler(p->priv, ladc_var.temp_buf, len);
            }
        }
        if (!list_empty(&ladc_var.linein1_head)) {
            if (ladc_var.linein1_ch_num == 1) {
                for (i = 0; i < samples; i++) {
                    ladc_var.temp_buf[i] = data[i * 3 + ladc_var.linein1_Ldata_index];
                    printf("[%d],", ladc_var.linein1_Ldata_index);
                }
            } else {
                for (i = 0; i < samples; i++) {
                    ladc_var.temp_buf[i * 2] = data[i * 3 + ladc_var.linein1_Ldata_index];
                    ladc_var.temp_buf[i * 2 + 1] = data[i * 3 + ladc_var.linein1_Rdata_index];
                }
                printf("[%d][%d],", ladc_var.linein1_Ldata_index, ladc_var.linein1_Rdata_index);
            }
            list_for_each_entry(p, &ladc_var.linein1_head, entry) {
                p->handler(p->priv, ladc_var.temp_buf, len);
            }
        }
        if (!list_empty(&ladc_var.mic_head)) {
            for (i = 0; i < samples; i++) {
                ladc_var.temp_buf[i] = data[i * 3 + ladc_var.mic_data_index];
            }
            list_for_each_entry(p, &ladc_var.mic_head, entry) {
                p->handler(p->priv, ladc_var.temp_buf, len);
            }
        }
    } else {
        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) { //带mic+2路linein
            if (!list_empty(&ladc_var.linein_head)) {
                if (ladc_var.linein_ch_num == 1) {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i] = data[i * 4 + ladc_var.linein_Ldata_index];
                    }
                } else {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i * 2] = data[i * 4 + ladc_var.linein_Ldata_index];
                        ladc_var.temp_buf[i * 2 + 1] = data[i * 4 + ladc_var.linein_Rdata_index];
                    }
                }
                list_for_each_entry(p, &ladc_var.linein_head, entry) {
                    p->handler(p->priv, ladc_var.temp_buf, len);
                }

            }
            if (!list_empty(&ladc_var.linein1_head)) {
                if (ladc_var.linein1_ch_num == 1) {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i] = data[i * 4 + ladc_var.linein1_Ldata_index];
                    }
                } else {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i * 2] = data[i * 4 + ladc_var.linein1_Ldata_index];
                        ladc_var.temp_buf[i * 2 + 1] = data[i * 4 + ladc_var.linein1_Rdata_index];
                    }
                }
                list_for_each_entry(p, &ladc_var.linein1_head, entry) {
                    p->handler(p->priv, ladc_var.temp_buf, len);
                }
            }
            if (!list_empty(&ladc_var.mic_head)) {//mic会复用buf
                for (i = 0; i < samples; i++) {
                    ladc_var.temp_buf[i] = data[i * 4 + ladc_var.mic_data_index];
                }
                list_for_each_entry(p, &ladc_var.mic_head, entry) {
                    p->handler(p->priv, ladc_var.temp_buf, len);
                }
            }
        } else { //2路立体声lineiin 默认通道不交叉
            if (!list_empty(&ladc_var.linein_head)) {
                if (ladc_var.linein_Lch_mark < ladc_var.linein1_Lch_mark) {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i * 2] = data[i * 4 + 0];
                        ladc_var.temp_buf[i * 2 + 1] = data[i * 4 + 1];
                    }
                } else {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i * 2] = data[i * 4 + 2];
                        ladc_var.temp_buf[i * 2 + 1] = data[i * 4 + 3];
                    }
                }
                list_for_each_entry(p, &ladc_var.linein_head, entry) {
                    p->handler(p->priv, ladc_var.temp_buf, len);
                }
            }
            if (!list_empty(&ladc_var.linein1_head)) {
                if (ladc_var.linein_Lch_mark < ladc_var.linein1_Lch_mark) {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i * 2] = data[i * 4 + 2];
                        ladc_var.temp_buf[i * 2 + 1] = data[i * 4 + 3];
                    }
                } else {
                    for (i = 0; i < samples; i++) {
                        ladc_var.temp_buf[i * 2] = data[i * 4 + 0];
                        ladc_var.temp_buf[i * 2 + 1] = data[i * 4 + 1];
                    }
                }
                list_for_each_entry(p, &ladc_var.linein1_head, entry) {
                    p->handler(p->priv, ladc_var.temp_buf, len);
                }
            }
        }
    }
}

void init_audio_adc()
{
    if (ladc_var.init_flag) {
        log_i("\n mic init_flag \n\n\n\n");
        ladc_var.init_flag = 0;
        atomic_set(&ladc_var.used, 0);
        os_mutex_create(&ladc_var.mutex);
        INIT_LIST_HEAD(&ladc_var.mic_head);
        INIT_LIST_HEAD(&ladc_var.linein_head);
        INIT_LIST_HEAD(&ladc_var.linein1_head);
        /* ladc_var.adc_buf = (s16 *)zalloc(LADC_BUFS_SIZE * 2); */
        ladc_var.states = 0;
    }
}

void audio_adc_set_irq_point_unit(u16 point_unit)
{
    ladc_irq_point_unit = point_unit;
}
void set_data_ch_index(void)
{
    u8 i, j = 0;
    for (i = 0; i < ladc_var.ladc_ch_num; i++) {

        for (; j < 4; j++) {
            /* printf("i[%d][%d]",i,BIT(i)); */
            if (BIT(j)&ladc_var.mic_ch_mark) {
                ladc_var.mic_data_index = i;
                break;
            }
            if (BIT(j)&ladc_var.linein_Lch_mark) {
                ladc_var.linein_Ldata_index = i;
                break;

            }
            if (BIT(j)&ladc_var.linein_Rch_mark) {
                ladc_var.linein_Rdata_index = i;
                break;
            }
            if (BIT(j)&ladc_var.linein1_Lch_mark) {
                ladc_var.linein1_Ldata_index = i;
                break;
            }
            if (BIT(j)&ladc_var.linein1_Rch_mark) {
                ladc_var.linein1_Rdata_index = i;
                break;
            }
        }
        j++;
    }
    printf("index[%d],[%d],[%d],[%d],[%d]", ladc_var.mic_data_index, ladc_var.linein_Ldata_index, ladc_var.linein_Rdata_index, ladc_var.linein1_Ldata_index, ladc_var.linein1_Rdata_index);
}

void audio_linein_set_irq_point(u16 point_unit)
{
#if !TCFG_MIC_EFFECT_ENABLE
    /* linein_irq_point = point_unit; */
    ladc_irq_point_unit = point_unit;
#endif
}
//------------------
int audio_mic_open(struct adc_mic_ch *mic, u16 sample_rate, u8 gain)
{
    u16 irq_point_unit = LADC_IRQ_POINTS;
    u8 i;
    if (ladc_irq_point_unit != 0) {
        irq_point_unit = ladc_irq_point_unit;
    }

#if	TCFG_AUDIO_ADC_ENABLE
    os_mutex_pend(&ladc_var.mutex, 0);
#if (defined(TCFG_AUDIO_MIC_BIAS_SEL) && TCFG_AUDIO_MIC_BIAS_SEL)
    audio_adc_mic_ldo_en(TCFG_AUDIO_MIC_BIAS_SEL, 1, &adc_hdl);
#endif /* #if (defined(TCFG_AUDIO_MIC_BIAS_SEL) && TCFG_AUDIO_MIC_BIAS_SEL) */
    if (!ladc_var.adc_buf) {
        log_i("\n mic malloc \n\n\n\n");
        ladc_var.ladc_ch_mark = 0;
        ladc_var.linein_ch_num = 0;
        ladc_var.linein1_ch_num = 0;
        ladc_var.linein_Lch_mark = 0;
        ladc_var.linein_Rch_mark = 0;
        ladc_var.linein1_ch_num = 0;
        ladc_var.linein1_Lch_mark = 0;
        ladc_var.linein1_Rch_mark = 0;
        ladc_var.ladc_ch_mark |= LADC_MIC_CH_MASK;
        ladc_var.ladc_ch_num = 1;
        ladc_var.mic_ch_num = 1;
        ladc_var.mic_ch_mark = TCFG_AUDIO_ADC_MIC_CHA;

#if TCFG_MIC_EFFECT_ENABLE //开混响时启用多路AD
#if (TCFG_LINEIN_ENABLE)
        for (i = 0; i < 4; i++) {
            if (TCFG_LINEIN_LR_CH & BIT(i)) {
                ladc_var.linein_ch_num++;
                ladc_var.ladc_ch_num++;
                if (ladc_var.linein_ch_num == 1) {
                    ladc_var.ladc_ch_mark |= LADC_LINE_L_MASK;
                    ladc_var.linein_Lch_mark = BIT(i) ; //AUDIO_LIN0L_CH;
                } else {
                    ladc_var.ladc_ch_mark |= LADC_LINE_R_MASK;
                    ladc_var.linein_Rch_mark = BIT(i);//AUDIO_LIN0R_CH;
                }
            }
        }
        printf("ladc_var.linein_Lch_mark[%d]\n", ladc_var.linein_Lch_mark);
#endif //TCFG_LINEIN_ENABLE
#if TCFG_FM_ENABLE
        for (i = 0; i < 4; i++) {
            if (TCFG_FMIN_LR_CH & BIT(i)) {
                ladc_var.linein1_ch_num++;
                ladc_var.ladc_ch_num++;
                if (ladc_var.linein1_ch_num == 1) {
                    ladc_var.ladc_ch_mark |= LADC_LINE1_L_MASK;
                    ladc_var.linein1_Lch_mark = BIT(i);
                } else {
                    ladc_var.ladc_ch_mark |= LADC_LINE1_R_MASK;
                    ladc_var.linein1_Rch_mark = BIT(i);
                }
            }
        }
        printf("ladc_var.linein1_Lch_mark[%d]\n", ladc_var.linein1_Lch_mark);
#endif //TCFG_FM_ENABLE
#endif//TCFG_MIC_EFFECT_ENABLE

        ladc_var.adc_buf = (s16 *)zalloc(ladc_var.ladc_ch_num * irq_point_unit * 2 * LADC_BUF_NUM);
        if (ladc_var.ladc_ch_num > 1) {
            ladc_var.temp_buf = (s16 *)zalloc(irq_point_unit * 2 * 2);
        }
        printf("ladc_ch_num[%d],[%x]", ladc_var.ladc_ch_num, ladc_var.ladc_ch_mark);
        if (ladc_var.ladc_ch_num > ladc_var.mic_ch_num) {
            set_data_ch_index();
        }
    }
    if (ladc_var.states == 0) {
        atomic_inc_return(&ladc_var.used);

        //根据CH 选择对应的打开接口
        if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0) {
            audio_adc_mic_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
            audio_adc_mic_set_gain(&ladc_var.mic_ch, gain);
            printf("\naudio_adc_mic_open\n");
        }
        if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1) {
            audio_adc_mic1_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
            audio_adc_mic1_set_gain(&ladc_var.mic_ch, gain);
        }
        if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2) {
            audio_adc_mic2_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
            audio_adc_mic2_set_gain(&ladc_var.mic_ch, gain);
        }
        if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3) {
            audio_adc_mic3_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
            audio_adc_mic3_set_gain(&ladc_var.mic_ch, gain);
        }
        if (ladc_var.ladc_ch_mark & (LADC_LINE_L_MASK | LADC_LINE_R_MASK)) {
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein_set_gain(&ladc_var.linein_ch, 0);
                printf("\n audio_adc_linein0_open \n");
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein1_set_gain(&ladc_var.linein_ch, 0);
                printf("\n audio_adc_linein1_open \n");
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein2_set_gain(&ladc_var.linein_ch, 0);
                printf("\n audio_adc_linein2_open \n");
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE3) {
                printf("\n audio_adc_linein3_open \n");
                audio_adc_linein3_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein3_set_gain(&ladc_var.linein_ch, 0);
            }
        }
        if (ladc_var.ladc_ch_mark & (LADC_LINE1_L_MASK | LADC_LINE1_R_MASK)) {
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein_set_gain(&ladc_var.linein1_ch, 0);
                printf("\n audio_adc_linein0_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, 0);
                printf("\n audio_adc_linein1_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein2_set_gain(&ladc_var.linein1_ch, 0);
                printf("\n audio_adc_linein2_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE3) {
                printf("\n audio_adc_linein3_open \n");
                audio_adc_linein3_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein3_set_gain(&ladc_var.linein1_ch, 0);
            }
        }
        ladc_var.mic_gain = gain;
        ladc_var.linein_gain = -1;
        ladc_var.linein1_gain = -1;
        printf("ladc_ch_num[%d] irq_point_unit[%d]\n\n", ladc_var.ladc_ch_num, irq_point_unit);

        audio_adc_mic_set_sample_rate(&ladc_var.mic_ch, sample_rate);
        audio_adc_mic_set_buffs(&ladc_var.mic_ch, ladc_var.adc_buf, irq_point_unit * 2, LADC_BUF_NUM);

        ladc_var.output.handler = audio_adc_output_demo;
        ladc_var.output.priv = &adc_hdl;
        audio_adc_add_output_handler(&adc_hdl, &ladc_var.output);
        ladc_var.sample_rate = sample_rate;
        ladc_var.states = 1;
    } else {
        if (ladc_var.sample_rate != sample_rate) {
            log_e("err: mic is on,sample_rate not match \n");
            os_mutex_post(&ladc_var.mutex);
            return -1;
        }
        if (ladc_var.mic_gain < 0) {
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0) {
                audio_adc_mic_set_gain(&ladc_var.mic_ch, gain);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1) {
                audio_adc_mic1_set_gain(&ladc_var.mic_ch, gain);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2) {
                audio_adc_mic2_set_gain(&ladc_var.mic_ch, gain);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3) {
                audio_adc_mic3_set_gain(&ladc_var.mic_ch, gain);
            }
            /* audio_adc_mic_set_gain(&ladc_var.mic_ch, gain); */
            ladc_var.mic_gain = gain;
        }
        atomic_inc_return(&ladc_var.used);
    }
    mic->adc = &adc_hdl;
    log_i("mic open success \n");
    os_mutex_post(&ladc_var.mutex);
    return 0;
#else
    return -1;
#endif
}
void audio_mic_add_output(struct audio_adc_output_hdl *output)
{
#if	TCFG_AUDIO_ADC_ENABLE
    os_mutex_pend(&ladc_var.mutex, 0);
    if (ladc_var.states == 1) {
        struct audio_adc_output_hdl *p;
        local_irq_disable();
        list_for_each_entry(p, &ladc_var.mic_head, entry) {
            if (p == (struct audio_adc_output_hdl *)&output->entry) {
                goto __exit;
            }
        }
        list_add_tail(&output->entry, &ladc_var.mic_head);
__exit:
        local_irq_enable();
    } else {
        printf("audio mic not open \n");
    }
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_mic_start(struct adc_mic_ch *mic)
{
#if	TCFG_AUDIO_ADC_ENABLE
    if (!mic || mic->adc != &adc_hdl) {
        log_i("\n adc_mic_ch not open 1\n");
        return;
    }
    if (adc_hdl.state == LADC_STATE_START) {
        return;
    }
    os_mutex_pend(&ladc_var.mutex, 0);
    audio_adc_mic_start(&ladc_var.mic_ch);
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_mic_close(struct adc_mic_ch *mic, struct audio_adc_output_hdl *output)
{
#if	TCFG_AUDIO_ADC_ENABLE
    if (!mic || mic->adc != &adc_hdl) {
        log_i("\n adc_mic_ch not open 2\n");
        return;
    }
    os_mutex_pend(&ladc_var.mutex, 0);
    struct audio_adc_output_hdl *p;
    local_irq_disable();
    list_for_each_entry(p, &ladc_var.mic_head, entry) {
        if (p == (struct audio_adc_output_hdl *)&output->entry) {
            list_del(&output->entry);
            break;
        }
    }
    local_irq_enable();

    if (atomic_dec_and_test(&ladc_var.used)) {
        log_i("\n audio_adc_mic_close \n");
        audio_adc_mic_close(&ladc_var.mic_ch);
        audio_adc_del_output_handler(&adc_hdl, &ladc_var.output);
        list_del_init(&ladc_var.mic_head);
        list_del_init(&ladc_var.linein_head);
        list_del_init(&ladc_var.linein1_head);
        free(ladc_var.adc_buf);
        ladc_var.adc_buf = NULL;
        if (ladc_var.temp_buf) {
            free(ladc_var.temp_buf);
        }
        ladc_var.temp_buf = NULL;
        ladc_var.states = 0;
        ladc_var.ladc_ch_mark = 0;
    } else {
        if (list_empty(&ladc_var.mic_head)) {
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0) {
                audio_adc_mic_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1) {
                audio_adc_mic1_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2) {
                audio_adc_mic2_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3) {
                audio_adc_mic3_set_gain(&ladc_var.mic_ch, 0);
            }
            ladc_var.mic_gain = -1;
        }
    }
#if (defined(TCFG_AUDIO_MIC_BIAS_SEL) && TCFG_AUDIO_MIC_BIAS_SEL)
    audio_adc_mic_ldo_en(TCFG_AUDIO_MIC_BIAS_SEL, 0, &adc_hdl);
#endif /* #if (defined(TCFG_AUDIO_MIC_BIAS_SEL) && TCFG_AUDIO_MIC_BIAS_SEL) */
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_mic_set_gain(u8 gain)
{
#if	TCFG_AUDIO_ADC_ENABLE
    os_mutex_pend(&ladc_var.mutex, 0);
    if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0) {
        audio_adc_mic_set_gain(&ladc_var.mic_ch, gain);
    }
    if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1) {
        audio_adc_mic1_set_gain(&ladc_var.mic_ch, gain);
    }
    if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2) {
        audio_adc_mic2_set_gain(&ladc_var.mic_ch, gain);
    }
    if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3) {
        audio_adc_mic3_set_gain(&ladc_var.mic_ch, gain);
    }
    /* audio_adc_mic_set_gain(&ladc_var.mic_ch, gain); */
    ladc_var.mic_gain = gain;
    os_mutex_post(&ladc_var.mutex);
#endif
}
//------------------
int audio_linein_open(struct adc_linein_ch *linein, u16 sample_rate, int gain)
{
    u16 irq_point_unit = LADC_IRQ_POINTS;
    if (ladc_irq_point_unit != 0) {
        irq_point_unit = ladc_irq_point_unit;
    }
#if (TCFG_LINEIN_ENABLE ||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (!ladc_var.adc_buf) {
        log_i("\n ladc malloc \n\n\n\n");
        ladc_var.ladc_ch_num = 0;
        ladc_var.ladc_ch_mark = 0;
        ladc_var.linein_ch_num = 0;
        ladc_var.linein1_ch_num = 0;
        ladc_var.mic_ch_num = 0;
        ladc_var.mic_ch_mark = 0;
        ladc_var.linein_Lch_mark = 0;
        ladc_var.linein_Rch_mark = 0;
        ladc_var.linein1_Lch_mark = 0;
        ladc_var.linein1_Rch_mark = 0;
#if	(TCFG_AUDIO_ADC_ENABLE&&TCFG_MIC_EFFECT_ENABLE)//开混响时启用多路AD
        ladc_var.ladc_ch_mark |= LADC_MIC_CH_MASK;
        ladc_var.ladc_ch_num = 1;
        ladc_var.mic_ch_num = 1;
        ladc_var.mic_ch_mark = TCFG_AUDIO_ADC_MIC_CHA;
#if TCFG_FM_ENABLE
        for (u8 i = 0; i < 4; i++) {
            if (TCFG_FMIN_LR_CH & BIT(i)) {
                ladc_var.linein1_ch_num++;
                ladc_var.ladc_ch_num++;
                if (ladc_var.linein1_ch_num == 1) {
                    ladc_var.ladc_ch_mark |= LADC_LINE1_L_MASK;
                    ladc_var.linein1_Lch_mark = BIT(i);
                } else {
                    ladc_var.ladc_ch_mark |= LADC_LINE1_R_MASK;
                    ladc_var.linein1_Rch_mark = BIT(i);
                }
            }
        }
#endif //TCFG_FM_ENABLE
#endif //TCFG_AUDIO_ADC_ENABLE&&TCFG_MIC_EFFECT_ENABLE

        for (u8 i = 0; i < 4; i++) {
            if (TCFG_LINEIN_LR_CH & BIT(i)) {
                ladc_var.linein_ch_num++;
                ladc_var.ladc_ch_num++;
                if (ladc_var.linein_ch_num == 1) {
                    ladc_var.ladc_ch_mark |= LADC_LINE_L_MASK;
                    ladc_var.linein_Lch_mark = BIT(i);
                } else {
                    ladc_var.ladc_ch_mark |= LADC_LINE_R_MASK;
                    ladc_var.linein_Rch_mark = BIT(i);
                }
            }
        }

        ladc_var.adc_buf = (s16 *)zalloc(ladc_var.ladc_ch_num * irq_point_unit * 2 * LADC_BUF_NUM);
        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) { //mic通路有打开
            ladc_var.temp_buf = (s16 *)zalloc(irq_point_unit * 2 * 2);
        }
        if (ladc_var.ladc_ch_num > ladc_var.linein_ch_num) {
            set_data_ch_index();
        }
        printf("ladc_ch_num[%d],[%x]", ladc_var.ladc_ch_num, ladc_var.ladc_ch_mark);
    }
    if (ladc_var.states == 0) {
        atomic_inc_return(&ladc_var.used);

        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) {

            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0) {
                audio_adc_mic_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1) {
                audio_adc_mic1_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic1_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2) {
                audio_adc_mic2_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic2_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3) {
                audio_adc_mic3_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic3_set_gain(&ladc_var.mic_ch, 0);
            }
        }
        ladc_var.mic_gain = -1 ;

        if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE0) {
            printf("\n audio_adc_linein0_open \n");
            audio_adc_linein_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
            audio_adc_linein_set_gain(&ladc_var.linein_ch, gain);
        }
        if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE1) {
            audio_adc_linein1_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
            audio_adc_linein1_set_gain(&ladc_var.linein_ch, gain);
            printf("\n audio_adc_linein1_open \n");
        }
        if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE2) {
            audio_adc_linein2_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
            audio_adc_linein2_set_gain(&ladc_var.linein_ch, gain);
            printf("\n audio_adc_linein2_open \n");
        }
        if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE3) {
            printf("\n audio_adc_linein3_open \n");
            audio_adc_linein3_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
            audio_adc_linein3_set_gain(&ladc_var.linein_ch, gain);
        }
        ladc_var.linein_gain = gain;
        if (ladc_var.ladc_ch_mark & (LADC_LINE1_L_MASK | LADC_LINE1_R_MASK)) {
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein_set_gain(&ladc_var.linein1_ch, 0);
                printf("\n audio_adc_linein0_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, 0);
                printf("\n audio_adc_linein1_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein2_set_gain(&ladc_var.linein1_ch, 0);
                printf("\n audio_adc_linein2_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE3) {
                printf("\n audio_adc_linein3_open \n");
                audio_adc_linein3_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein3_set_gain(&ladc_var.linein1_ch, 0);
            }
        }
        ladc_var.linein1_gain = -1;

        audio_adc_linein_set_sample_rate(&ladc_var.linein_ch, sample_rate);
        audio_adc_linein_set_buffs(&ladc_var.linein_ch, ladc_var.adc_buf,  irq_point_unit * 2, LADC_BUF_NUM);
        ladc_var.output.handler = audio_adc_output_demo;
        ladc_var.output.priv = &adc_hdl;
        audio_adc_add_output_handler(&adc_hdl, &ladc_var.output);
        ladc_var.sample_rate = sample_rate;
        ladc_var.states = 1;
        log_i("sample_rate [%d]\n", sample_rate);
    } else {
        if (ladc_var.sample_rate != sample_rate) {
            log_e("err: linein is on,sample_rate not match \n");
            os_mutex_post(&ladc_var.mutex);
            return -1;
        }
        if (ladc_var.linein_gain < 0) {
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_set_gain(&ladc_var.linein_ch, gain);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_set_gain(&ladc_var.linein_ch, gain);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_set_gain(&ladc_var.linein_ch, gain);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE3) {
                audio_adc_linein3_set_gain(&ladc_var.linein_ch, gain);
            }
            /* audio_adc_linein_set_gain(&ladc_var.linein_ch, gain); */
            ladc_var.linein_gain = gain;
        }

        atomic_inc_return(&ladc_var.used);
    }
    linein->adc = &adc_hdl;
    log_i("linein open success \n");

    os_mutex_post(&ladc_var.mutex);
    return 0;
#else
    return -1;
#endif
}
void audio_linein_add_output(struct audio_adc_output_hdl *output)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (ladc_var.states == 1) {
        struct audio_adc_output_hdl *p;

        local_irq_disable();
        list_for_each_entry(p, &ladc_var.linein_head, entry) {
            if (p == (struct audio_adc_output_hdl *)&output->entry) {
                goto __exit;
            }
        }
        list_add_tail(&output->entry, &ladc_var.linein_head);
__exit:
        local_irq_enable();
    } else {
        printf("audio linein not open \n");
    }
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_linein_start(struct adc_linein_ch *linein)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    if (!linein || linein->adc != &adc_hdl) {
        log_i("\n adc_mic_ch not open 1\n");
        return;
    }
    if (adc_hdl.state == LADC_STATE_START) {
        return;
    }
    printf("\n--func=%s\n", __FUNCTION__);
    os_mutex_pend(&ladc_var.mutex, 0);
    audio_adc_linein_start(&ladc_var.linein_ch);
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_linein_close(struct adc_linein_ch *linein, struct audio_adc_output_hdl *output)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    if (!linein || linein->adc != &adc_hdl) {
        log_i("\n adc_mic_ch not open 2\n");
        return;
    }
    os_mutex_pend(&ladc_var.mutex, 0);
    struct audio_adc_output_hdl *p;
    local_irq_disable();
    list_for_each_entry(p, &ladc_var.linein_head, entry) {
        if (p == (struct audio_adc_output_hdl *)&output->entry) {
            list_del(&output->entry);
            break;
        }
    }
    local_irq_enable();
    if (atomic_dec_and_test(&ladc_var.used)) {
        log_i("\n audio_adc_mic_close \n");
        audio_adc_linein_close(&ladc_var.linein_ch);
        audio_adc_del_output_handler(&adc_hdl, &ladc_var.output);
        list_del_init(&ladc_var.mic_head);
        list_del_init(&ladc_var.linein_head);
        list_del_init(&ladc_var.linein1_head);
        free(ladc_var.adc_buf);
        ladc_var.adc_buf = NULL;
        if (ladc_var.temp_buf) {
            free(ladc_var.temp_buf);
        }
        ladc_var.temp_buf = NULL;
        ladc_var.states = 0;
        ladc_var.ladc_ch_mark = 0;
    } else {
        if (list_empty(&ladc_var.linein_head)) {
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_set_gain(&ladc_var.linein_ch, 0);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_set_gain(&ladc_var.linein_ch, 0);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_set_gain(&ladc_var.linein_ch, 0);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE3) {
                audio_adc_linein3_set_gain(&ladc_var.linein_ch, 0);
            }
            ladc_var.linein_gain = -1;
        }
    }
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_linein_set_gain(int gain)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE0) {
        audio_adc_linein_set_gain(&ladc_var.linein_ch, gain);
    }
    if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE1) {
        audio_adc_linein1_set_gain(&ladc_var.linein_ch, gain);
    }
    if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE2) {
        audio_adc_linein2_set_gain(&ladc_var.linein_ch, gain);
    }
    if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE3) {
        audio_adc_linein3_set_gain(&ladc_var.linein_ch, gain);
    }
    /* audio_adc_linein_set_gain(&ladc_var.linein_ch, gain); */
    ladc_var.linein_gain = gain;
    os_mutex_post(&ladc_var.mutex);
#endif
}
u8 get_audio_linein_ch_num(void)
{
    u8 ret = 0;
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (ladc_var.states == 1) {
        ret = ladc_var.linein_ch_num;
    }
    os_mutex_post(&ladc_var.mutex);
#endif
    return ret;
}
//-----------linein fm----
int audio_linein_fm_open(struct adc_linein_ch *linein, u16 sample_rate, int gain)
{
    u16 irq_point_unit = LADC_IRQ_POINTS;
    if (ladc_irq_point_unit != 0) {
        irq_point_unit = ladc_irq_point_unit;
    }
    printf("\n--func=%s\n", __FUNCTION__);
#if (TCFG_FM_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (!ladc_var.adc_buf) {
        log_i("\n ladc malloc \n\n\n\n");
        ladc_var.ladc_ch_mark = 0;
        ladc_var.ladc_ch_num = 0;
        ladc_var.linein_ch_num = 0;
        ladc_var.linein1_ch_num = 0;
        ladc_var.mic_ch_num = 0;
        ladc_var.mic_ch_mark = 0;
        ladc_var.linein_Lch_mark = 0;
        ladc_var.linein_Rch_mark = 0;
        ladc_var.linein1_Lch_mark = 0;
        ladc_var.linein1_Rch_mark = 0;
#if	(TCFG_AUDIO_ADC_ENABLE&&TCFG_MIC_EFFECT_ENABLE)//开混响时启用多路AD
        ladc_var.ladc_ch_mark |= LADC_MIC_CH_MASK;
        ladc_var.ladc_ch_num = 1;
        ladc_var.mic_ch_num = 1;
        ladc_var.mic_ch_mark = TCFG_AUDIO_ADC_MIC_CHA;
#if TCFG_LINEIN_ENABLE
        for (u8 i = 0; i < 4; i++) {
            if (TCFG_LINEIN_LR_CH & BIT(i)) {
                ladc_var.linein_ch_num++;
                ladc_var.ladc_ch_num++;
                if (ladc_var.linein_ch_num == 1) {
                    ladc_var.ladc_ch_mark |= LADC_LINE_L_MASK;
                    ladc_var.linein_Lch_mark = BIT(i);
                } else {
                    ladc_var.ladc_ch_mark |= LADC_LINE_R_MASK;
                    ladc_var.linein_Rch_mark = BIT(i);
                }
            }
        }
#endif //TCFG_FM_ENABLE
#endif //TCFG_AUDIO_ADC_ENABLE&&TCFG_MIC_EFFECT_ENABLE

        for (u8 i = 0; i < 4; i++) {
            if (TCFG_FMIN_LR_CH & BIT(i)) {
                ladc_var.linein1_ch_num++;
                ladc_var.ladc_ch_num++;
                if (ladc_var.linein1_ch_num == 1) {
                    ladc_var.ladc_ch_mark |= LADC_LINE1_L_MASK;
                    ladc_var.linein1_Lch_mark = BIT(i);
                } else {
                    ladc_var.ladc_ch_mark |= LADC_LINE1_R_MASK;
                    ladc_var.linein1_Rch_mark = BIT(i);
                }
            }
        }

        ladc_var.adc_buf = (s16 *)zalloc(ladc_var.ladc_ch_num * irq_point_unit * 2 * LADC_BUF_NUM);
        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) { //mic通路有打开
            ladc_var.temp_buf = (s16 *)zalloc(irq_point_unit * 2 * 2);
        }
        if (ladc_var.ladc_ch_num > ladc_var.linein_ch_num) {
            set_data_ch_index();
        }
        printf("ladc_ch_num[%d],[%x]", ladc_var.ladc_ch_num, ladc_var.ladc_ch_mark);
    }
    if (ladc_var.states == 0) {
        atomic_inc_return(&ladc_var.used);

        if (ladc_var.ladc_ch_mark & LADC_MIC_CH_MASK) {

            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_0) {
                audio_adc_mic_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_1) {
                audio_adc_mic1_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic1_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_2) {
                audio_adc_mic2_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic2_set_gain(&ladc_var.mic_ch, 0);
            }
            if (TCFG_AUDIO_ADC_MIC_CHA & AUDIO_ADC_MIC_3) {
                audio_adc_mic3_open(&ladc_var.mic_ch, TCFG_AUDIO_ADC_MIC_CHA, &adc_hdl);
                audio_adc_mic3_set_gain(&ladc_var.mic_ch, 0);
            }
        }
        ladc_var.mic_gain = -1 ;

        if (ladc_var.ladc_ch_mark & (LADC_LINE_L_MASK | LADC_LINE_R_MASK)) {
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein_set_gain(&ladc_var.linein_ch, 0);
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein1_set_gain(&ladc_var.linein_ch, 0);
                printf("\n audio_adc_linein1_open \n");
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein2_set_gain(&ladc_var.linein_ch, 0);
                printf("\n audio_adc_linein2_open \n");
            }
            if (TCFG_LINEIN_LR_CH & AUDIO_ADC_LINE3) {
                audio_adc_linein3_open(&ladc_var.linein_ch, TCFG_LINEIN_LR_CH, &adc_hdl);
                audio_adc_linein3_set_gain(&ladc_var.linein_ch, 0);
            }
        }
        ladc_var.linein_gain = -1;
        if (ladc_var.ladc_ch_mark & (LADC_LINE1_L_MASK | LADC_LINE1_R_MASK)) {
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein_set_gain(&ladc_var.linein1_ch, gain);
                printf("\n audio_adc_linein0_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, gain);
                printf("\n audio_adc_linein1_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein2_set_gain(&ladc_var.linein1_ch, gain);
                printf("\n audio_adc_linein2_open \n");
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE3) {
                printf("\n audio_adc_linein3_open \n");
                audio_adc_linein3_open(&ladc_var.linein1_ch, TCFG_FMIN_LR_CH, &adc_hdl);
                audio_adc_linein3_set_gain(&ladc_var.linein1_ch, gain);
            }
        }
        ladc_var.linein1_gain = gain;

        audio_adc_linein_set_sample_rate(&ladc_var.linein1_ch, sample_rate);
        audio_adc_linein_set_buffs(&ladc_var.linein1_ch, ladc_var.adc_buf,  irq_point_unit * 2, LADC_BUF_NUM);
        ladc_var.output.handler = audio_adc_output_demo;
        ladc_var.output.priv = &adc_hdl;
        audio_adc_add_output_handler(&adc_hdl, &ladc_var.output);
        ladc_var.sample_rate = sample_rate;
        ladc_var.states = 1;
        log_i("sample_rate [%d]\n", sample_rate);
    } else {
        if (ladc_var.sample_rate != sample_rate) {
            log_e("err: linein is on,sample_rate not match \n");
            os_mutex_post(&ladc_var.mutex);
            return -1;
        }
        if (ladc_var.linein1_gain < 0) {
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_set_gain(&ladc_var.linein1_ch, gain);
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, gain);
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein2_set_gain(&ladc_var.linein1_ch, gain);
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE3) {
                audio_adc_linein3_set_gain(&ladc_var.linein1_ch, gain);
            }
            /* audio_adc_linein_set_gain(&ladc_var.linein_ch, gain); */
            ladc_var.linein1_gain = gain;
        }

        atomic_inc_return(&ladc_var.used);
    }
    linein->adc = &adc_hdl;
    log_i("linein open success \n");

    os_mutex_post(&ladc_var.mutex);
    return 0;
#else
    return -1;
#endif
}
void audio_linein_fm_add_output(struct audio_adc_output_hdl *output)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (ladc_var.states == 1) {
        struct audio_adc_output_hdl *p;

        local_irq_disable();
        list_for_each_entry(p, &ladc_var.linein1_head, entry) {
            if (p == (struct audio_adc_output_hdl *)&output->entry) {
                goto __exit;
            }
        }
        list_add_tail(&output->entry, &ladc_var.linein1_head);
__exit:
        local_irq_enable();
    } else {
        printf("audio linein not open \n");
    }
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_linein_fm_start(struct adc_linein_ch *linein)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    if (!linein || linein->adc != &adc_hdl) {
        log_i("\n adc_mic_ch not open 1\n");
        return;
    }
    if (adc_hdl.state == LADC_STATE_START) {
        return;
    }
    os_mutex_pend(&ladc_var.mutex, 0);
    audio_adc_linein_start(&ladc_var.linein1_ch);
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_linein_fm_close(struct adc_linein_ch *linein, struct audio_adc_output_hdl *output)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    if (!linein || linein->adc != &adc_hdl) {
        log_i("\n adc_mic_ch not open 2\n");
        return;
    }
    os_mutex_pend(&ladc_var.mutex, 0);
    struct audio_adc_output_hdl *p;
    local_irq_disable();
    list_for_each_entry(p, &ladc_var.linein1_head, entry) {
        if (p == (struct audio_adc_output_hdl *)&output->entry) {
            list_del(&output->entry);
            break;
        }
    }
    local_irq_enable();
    if (atomic_dec_and_test(&ladc_var.used)) {
        log_i("\n audio_adc_mic_close \n");
        audio_adc_linein_close(&ladc_var.linein1_ch);
        audio_adc_del_output_handler(&adc_hdl, &ladc_var.output);
        list_del_init(&ladc_var.mic_head);
        list_del_init(&ladc_var.linein_head);
        list_del_init(&ladc_var.linein1_head);
        free(ladc_var.adc_buf);
        ladc_var.adc_buf = NULL;
        if (ladc_var.temp_buf) {
            free(ladc_var.temp_buf);
        }
        ladc_var.temp_buf = NULL;
        ladc_var.states = 0;
        ladc_var.ladc_ch_mark = 0;
    } else {
        if (list_empty(&ladc_var.linein1_head)) {
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE0) {
                audio_adc_linein_set_gain(&ladc_var.linein1_ch, 0);
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE1) {
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, 0);
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE2) {
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, 0);
            }
            if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE3) {
                audio_adc_linein1_set_gain(&ladc_var.linein1_ch, 0);
            }
            /* audio_adc_linein_set_gain(&ladc_var.linein_ch, 0); */
            ladc_var.linein1_gain = -1;
        }
    }
    os_mutex_post(&ladc_var.mutex);
#endif
}
void audio_linein_fm_set_gain(int gain)
{
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE0) {
        audio_adc_linein_set_gain(&ladc_var.linein1_ch, gain);
    }
    if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE1) {
        audio_adc_linein1_set_gain(&ladc_var.linein1_ch, gain);
    }
    if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE2) {
        audio_adc_linein2_set_gain(&ladc_var.linein1_ch, gain);
    }
    if (TCFG_FMIN_LR_CH & AUDIO_ADC_LINE3) {
        audio_adc_linein3_set_gain(&ladc_var.linein1_ch, gain);
    }
    /* audio_adc_linein_set_gain(&ladc_var.linein_ch, gain); */
    ladc_var.linein1_gain = gain;
    os_mutex_post(&ladc_var.mutex);
#endif
}
u8 get_audio_linein_fm_ch_num(void)
{
    u8 ret = 0;
#if (TCFG_LINEIN_ENABLE||TCFG_FM_ENABLE)
    os_mutex_pend(&ladc_var.mutex, 0);
    if (ladc_var.states == 1) {
        ret = ladc_var.linein1_ch_num;
    }
    os_mutex_post(&ladc_var.mutex);
#endif
    return ret;
}



/*****************************************************************************/
